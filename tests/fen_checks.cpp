#include "../MoveGen/mgen6.h"
extern "C" {
#include "../MoveGen/move.h"
#include "../MoveGen/mgen_s.h"
struct LegacyPosition {
    unsigned long long hashkey,hashkey2;
    int a,b,p1;
    unsigned char d,z,*pbt2,hash_doppel,lwm,lsm,wch1,sch1,wr1,sr1,fesselung;
};
union LegacyBoard { unsigned char cc[148]; long ii[37]; };
extern LegacyPosition pos[72];
extern LegacyBoard cb;
extern int g,mvtop,fln[16];
extern unsigned int mv1[10012];
extern unsigned char mv2[10012],flc[16][12],*flp,lwm,lsm;
extern char akt_fen[256],my_fens[32][256];
extern unsigned long long zobrist[16][128];
unsigned long long get_board_hash(int ply);
}
#include <algorithm>
#include <array>
#include <cstring>
#include <cstdint>
#include <iostream>
#include <memory>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
#ifdef FEN_REFERENCE
#include "mgen.h"
static std::unique_ptr<MGen> reference;
#endif
static_assert(sizeof(LegacyPosition)==48,"unchanged search layout");
static void require(bool ok,const std::string& why) { if(!ok) throw std::runtime_error(why); }
static const char* start="rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
static void load(const std::string& text) {
    std::string copy=text; require(!fen_to_board(copy.data()),"valid FEN rejected: "+text);
    require(copy==text,"input string modified");
}
static std::string square(int sq) { return std::string(1,char('a'+(sq-28)%12))+char('1'+(sq-28)/12); }
struct Move { int a,b; };
static std::string uci(Move move) {
    int from=flp[move.a&255],to=move.b,special=move.a>>8;
    if(special>=120) { from=(special<122)?32:116;to=from+((special&1)?-2:2); }
    std::string out=square(from)+square(to);
    if(special>0 && special<16) out+=std::string(" qrbn")[special<7?special:14-special];
    return out;
}
static std::vector<Move> moves() {
    lwm=pos[g].lwm;lsm=pos[g].lsm;make_king();int top=mvtop,n=generate_moves();
    std::vector<Move> out;for(int i=top;i<top+n;++i)out.push_back({int(mv1[i]),mv2[i]});mvtop=top;return out;
}
static std::set<std::string> move_set() { std::set<std::string> out;for(auto m:moves())out.insert(uci(m));return out; }
static uint64_t perft(int depth) {
    if(!depth) return 1;
    uint64_t count=0;for(auto m:moves()) { make_move_h(m.a,m.b);count+=perft(depth-1);undo_move(); }return count;
}
static void play(const std::string& text) {
    auto list=moves();auto i=std::find_if(list.begin(),list.end(),[&](Move m){return uci(m)==text;});
    require(i!=list.end(),"test move missing: "+text);make_move_h(i->a,i->b);
}
// Test-only four-field representation. The old library has no board-to-FEN API.
static std::string canonical() {
    std::string out;const char* pieces=" QRBNPK#kpnbrq";
    for(int rank=7;rank>=0;--rank) {
        int empty=0;for(int file=0;file<8;++file) {
            int piece=cb.cc[28+rank*12+file];
            if(!piece) { ++empty;continue; }
            if(empty) {out+=char('0'+empty);empty=0;}out+=pieces[piece];
        }
        if(empty)out+=char('0'+empty);if(rank)out+='/';
    }
    out+=(g&1)?" b ":" w ";std::string rights;
    if(pos[g].wr1&1)rights+='K';if(pos[g].wr1&2)rights+='Q';
    if(pos[g].sr1&1)rights+='k';if(pos[g].sr1&2)rights+='q';out+=rights.empty()?"-":rights;out+=' ';
    out+=pos[g].hash_doppel>16?square(pos[g].hash_doppel+((g&1)?-12:12)):"-";return out;
}
struct Snapshot {
    LegacyBoard board=cb;
    std::array<unsigned char,sizeof(pos)> history;
    std::array<unsigned char,sizeof(flc)> lists;
    std::array<int,16> counts;
    std::array<char,256> fen;
    int ply=g,top=mvtop;unsigned char white=lwm,black=lsm;
    Snapshot() { std::memcpy(history.data(),pos,sizeof(pos));std::memcpy(lists.data(),flc,sizeof(flc));std::copy(fln,fln+16,counts.begin());std::memcpy(fen.data(),akt_fen,sizeof(akt_fen)); }
    void check() const {
        require(ply==g && top==mvtop && white==lwm && black==lsm && !std::memcmp(&board,&cb,sizeof(cb)) &&
            !std::memcmp(history.data(),pos,sizeof(pos)) && !std::memcmp(lists.data(),flc,sizeof(flc)) &&
            std::equal(counts.begin(),counts.end(),fln) && !std::memcmp(fen.data(),akt_fen,sizeof(akt_fen)),"failed import changed previous position/history");
    }
};
static std::string plain(const std::string& fen) { return fen.substr(0,fen.find(';')); }
static std::string first_four(const std::string& fen) {
    std::istringstream in(fen);std::string out,s;for(int i=0;i<4;++i) {in>>s;if(i)out+=' ';out+=s;}return out;
}
int main() {
  try {
    set_winflag(1);init_tables();init_mytime();
#ifdef FEN_REFERENCE
    reference=std::make_unique<MGen>();
#endif
    const std::vector<std::string> valid={
        start,"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR b KQkq - 0 1",
        "4k3/8/8/8/8/8/8/4K3 w - -", "4k3/8/8/8/8/8/8/4K3 b - - 99 123456",
        " \t4k3/8/8/8/8/8/8/4K3\tw\t-\t-\r\n0 1  ",
        "4k3/8/8/8/8/8/8/4K3 w - -; id \"Old EPD\"",
        "4k3/8/8/8/8/8/8/4K3 w - - 0 1; id \"Six fields\"",
        "4k3/8/8/8/8/8/8/4K3 w - - 18446744073709551615 18446744073709551615",
        "4k3/8/8/8/8/8/8/4K3 w KQkq - 0 1",
        "r3k2r/8/8/8/8/8/8/R3K2R w qkQK - 0 1",
        "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
        "4k3/8/8/8/8/3n4/8/4K3 w - - 0 1",
        "4k3/8/3N4/8/8/8/8/4K3 b - - 0 1",
        "4k3/8/8/8/8/8/4r3/4K3 w - - 0 1",
        "4k3/4R3/8/8/8/8/8/4K3 b - - 0 1",
        "k7/8/8/3pP3/8/8/8/4K3 w - d6 0 2",
        "4k3/8/8/8/3Pp3/8/8/K7 b - d3 0 2",
        "k3r3/8/8/3pP3/8/8/8/4K3 w - d6 0 2",
        "4k3/8/8/8/3Pp3/8/8/K3R3 b - d3 0 2",
        "k7/8/8/3p4/8/8/8/4K3 w - d6 0 2",
        "4k3/8/8/8/3P4/8/8/K7 b - d3 0 2",
        "k7/8/8/1PpK4/8/8/8/8 w - c6 0 2",
        "k7/8/8/KPp4r/8/8/8/8 w - c6 0 2",
        "4k3/8/8/8/8/8/P7/4K3 w - - 0 1"
    };
    for(const auto& fen:valid) {
        load(fen);const auto normalized=canonical();const auto rootKey=pos[g].hashkey2;
        require(rootKey==get_board_hash(g),"imported hash differs from recomputation");
        const int ply=g;g^=1;
        require(rootKey==get_board_hash(ply) && g==(ply^1),"hash helper used global side instead of requested position");g=ply;
#ifdef FEN_REFERENCE
        require(!reference->fen_to_board(plain(fen).c_str()),"reference rejected valid fixture");
        require(normalized==first_four(reference->board_to_fen()),"canonical FEN differs: "+fen+" -> "+normalized);
        std::set<std::string> expected;int top=reference->mvtop,n=reference->generate_moves();
        for(int i=top;i<top+n;++i) { char text[6];reference->printmove_eng(reference->mv1[i],reference->mv2[i],text);expected.insert(text); }
        reference->mvtop=top;require(move_set()==expected,"legal moves differ from JetChess: "+fen);
#endif
        load(normalized);require(rootKey==pos[g].hashkey2,"canonical re-import changed hash");
    }
    const std::vector<std::string> bad={
        "", "8/8/8/8/8/8/8/8 w - - 0 1", "4k3/8/8/8/8/8/8/4KK2 w - - 0 1",
        "4k3/8/8/8/8/8/8/4K3z w - - 0 1", "4k3/8/8/8/8/8/8/5K3/8 w - - 0 1",
        "4k3/8/8/8/8/8/8/9K w - - 0 1", "4k3/8/8/8/8/8/8/04K3 w - - 0 1",
        "4k3/8/8/8/8/8/8/4K3 x - - 0 1", "4k3/8/8/8/8/8/8/4K3 W - - 0 1",
        "4k3/8/8/8/8/8/8/4K3 white - - 0 1", "4k3/8/8/8/8/8/8/4K3 w X - 0 1",
        "4k3/8/8/8/8/8/8/4K3 w KK - 0 1", "4k3/8/8/8/8/8/8/4K3 w K- - 0 1",
        "4k3/8/8/8/8/8/8/4K3 w - d6 0 1", "4k3/8/8/8/8/8/8/4K3 w - z6 0 1",
        "4k3/8/8/3p4/8/8/8/4K3 w - d3 0 1", "4k3/8/8/3p4/8/8/8/4K3 w - d6x 0 1",
        "4k3/8/8/3p4/8/8/8/4K3 w - nope 0 1", "4k3/8/8/3p4/8/8/8/4K3 w - -- 0 1",
        "4k3/3n4/8/3p4/8/8/8/4K3 w - d6 0 1", "4k3/8/3n4/3p4/8/8/8/4K3 w - d6 0 1",
        "4k3/8/8/8/8/8/8/4K3 w - - -1 1", "4k3/8/8/8/8/8/8/4K3 w - - 0 0",
        "4k3/8/8/8/8/8/8/4K3 w - - 0", "4k3/8/8/8/8/8/8/4K3 w - - 0 1 junk",
        "4k3/8/8/8/8/8/8/4K3 w - - 18446744073709551616 1",
        "4k3/8/8/8/8/8/8/4K3 w - - 0 18446744073709551616",
        "4k3/8/8/8/8/8/8/4K3 w - - +1 1", "4k3/8/8/8/8/8/8/4K3 w - - 0 -1",
        "4k3/8/8/8/8/8/8/4K3 w -", "4k3/8/8/8/8/8/8/4K2P w - - 0 1",
        "8/8/8/8/8/8/4k3/4K3 w - - 0 1", "4k3/8/8/8/8/8/8/4R1K1 w - - 0 1",
        "4k3/8/8/8/8/3n4/8/4K3 b - - 0 1", "4k3/8/3N4/8/8/8/8/4K3 w - - 0 1",
        "QQQQQQQQ/QQQQk3/8/8/8/8/8/4K3 w - - 0 1", std::string(4096,'x'),
        "4k3/8/8/8/8/8/8/4K3junk w - -", "4k3/8/8/8/8/8/8/4K3/ w - -",
        "4k3/8/8/8/8/8/8/4K3 w - - 0 1 "+std::string(80,'x')
    };
    load(start);play("e2e4");play("e7e5");mvtop=17;
    for(auto fen:bad) {
        Snapshot before;std::string copy=fen;require(fen_to_board(copy.data())!=0,"invalid FEN accepted: "+fen);before.check();require(copy==fen,"invalid input modified");
#ifdef FEN_REFERENCE
        require(reference->fen_to_board(plain(fen).c_str())!=0,"reference accepted invalid fixture: "+fen);
#endif
    }
    { Snapshot before;require(fen_to_board(nullptr)!=0,"null FEN accepted");before.check(); }
    load(start);require(perft(3)==8902,"initial Perft 3");
    load("4k3/8/3N4/8/8/8/8/4K3 b - - 0 1");
    require(pos[g].lwm==4 && make_king()==1,"white knight check not initialized for black");
    const auto blackCheck=move_set();require(blackCheck.size()==4,"black knight-check evasions");
    for(const auto& fen:std::vector<std::string>{"k7/8/8/3pP3/8/8/8/4K3 w - d6 0 2","4k3/8/8/8/3Pp3/8/8/K7 b - d3 0 2"}) {
        load(fen);require(pos[g].hash_doppel>16,"legal EP missing in imported hash");const auto epKey=pos[g].hashkey2;
        std::string noEp=first_four(fen);noEp.replace(noEp.rfind(' ')+1,std::string::npos,"-");load(noEp);require(pos[g].hashkey2!=epKey,"EP hash ignored");
    }
    for(const auto& path:std::vector<std::vector<std::string>>{{"e2e4","a7a6","e4e5","d7d5"},{"a2a3","d7d5","a3a4","d5d4","e2e4"}}) {
        load(start);for(auto text:path)play(text);std::string fen=canonical();auto key=pos[g].hashkey;
        if(generate_moves_ep())key^=zobrist[15][pos[g].hash_doppel];
        require(key==get_board_hash(g),"incremental EP key differs from board recomputation");
        load(fen);require(key==pos[g].hashkey2,"FEN import differs from incremental EP hash");
    }
#ifdef FEN_REFERENCE
    uint64_t random=42;int compared=0;
    for(int game=0;game<20;++game) {
        reference->set_initial_position();
        for(int ply=0;ply<80;++ply) {
            const std::string fen=reference->board_to_fen();load(fen);
            require(canonical()==first_four(fen),"random canonical import differs: "+fen);
            int top=reference->mvtop,n=reference->generate_moves();std::set<std::string> expected;
            for(int i=top;i<top+n;++i) { char text[6];reference->printmove_eng(reference->mv1[i],reference->mv2[i],text);expected.insert(text); }
            require(move_set()==expected,"random imported move set differs: "+fen);++compared;
            if(!n) {reference->mvtop=top;break;}
            random=random*UINT64_C(6364136223846793005)+1;int i=top+int((random>>32)%unsigned(n));
            int a=reference->mv1[i],b=reference->mv2[i];reference->mvtop=top;reference->make_move_h(a,b);
        }
    }
    std::cout<<compared<<" generated FENs/canonical fields/legal move sets matched JetChess\n";
#endif
    std::array<char,sizeof(my_fens)> stored;std::memcpy(stored.data(),my_fens,sizeof(my_fens));
    std::string longFen(256,'x');require(add_fen(longFen.data())==-1 && add_fen(nullptr)==-1,"unbounded stored FEN accepted");
    require(!std::memcmp(stored.data(),my_fens,sizeof(my_fens)),"oversize FEN changed stored list");
    std::cout<<valid.size()<<" valid and "<<bad.size()+1<<" invalid FEN cases, transactional import, canonical/hash round trips, knight-check/EP and storage bounds passed\n";
    return 0;
  } catch(const std::exception& e) { std::cerr<<e.what()<<'\n';return 1; }
}
