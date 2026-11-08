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
}
#include <algorithm>
#include <array>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

static_assert(sizeof(LegacyPosition)==48 && sizeof(long)==4,"legacy layout");
static void require(bool ok,const std::string& message) { if(!ok) throw std::runtime_error(message); }
static std::string square(int sq) {
    return std::string(1,char('a'+(sq-28)%12))+char('1'+(sq-28)/12);
}
struct Move { int a,b; };
static std::string uci(Move m) { return square(flp[m.a&255])+square(m.b); }
static void load(const std::string& fen) {
    std::string copy=fen;
    require(fen_to_board(copy.data())==0,"FEN rejected: "+fen);
    mvtop=0; lwm=pos[g].lwm; lsm=pos[g].lsm;
}
static void prepare() { lwm=pos[g].lwm; lsm=pos[g].lsm; make_king(); }
static std::vector<Move> moves() {
    prepare(); const int start=mvtop,n=generate_moves(); std::vector<Move> out;
    for(int i=start;i<start+n;++i) out.push_back({int(mv1[i]),mv2[i]});
    mvtop=start; return out;
}

// Independent reference: 8x8 board, enumerating enemy pieces and their attacks.
static bool attacked(const std::array<int,64>& board,bool black) {
    const int kingPiece=black?8:6;
    auto k=std::find(board.begin(),board.end(),kingPiece);
    require(k!=board.end(),"reference king missing");
    int king=int(k-board.begin()),kf=king%8,kr=king/8;
    for(int sq=0;sq<64;++sq) {
        int piece=board[sq];
        if(!piece || (black ? piece>=7 : piece<8)) continue;
        int df=kf-sq%8,dr=kr-sq/8,af=std::abs(df),ar=std::abs(dr);
        if(piece==5 || piece==9) { if(af==1 && dr==(piece==5?1:-1)) return true; continue; }
        if(piece==4 || piece==10) { if((af==1&&ar==2)||(af==2&&ar==1)) return true; continue; }
        if(piece==6 || piece==8) { if(std::max(af,ar)==1) return true; continue; }
        bool diagonal=af==ar && af!=0,straight=(df==0)!=(dr==0);
        if((piece==3||piece==11)&&!diagonal) continue;
        if((piece==2||piece==12)&&!straight) continue;
        if((piece==1||piece==13)&&!diagonal&&!straight) continue;
        int sf=(df>0)-(df<0),sr=(dr>0)-(dr<0),f=sq%8+sf,r=sq/8+sr;
        while(f!=kf || r!=kr) { if(board[r*8+f]) break; f+=sf; r+=sr; }
        if(f==kf && r==kr) return true;
    }
    return false;
}
static std::set<std::string> reference_ep() {
    bool black=(g&1)!=0; int pawn=black?pos[g].lwm:pos[g].lsm;
    std::set<std::string> out;
    if(pawn<=16) return out;
    for(int offset:{-1,1}) {
        int from=pawn+offset,to=pawn+(black?-12:12);
        if(cb.cc[from]!=(black?9:5)||cb.cc[pawn]!=(black?5:9)||cb.cc[to]) continue;
        std::array<int,64> board;
        for(int rank=0;rank<8;++rank) for(int file=0;file<8;++file) board[rank*8+file]=cb.cc[28+12*rank+file];
        auto index=[](int sq) { return ((sq-28)/12)*8+(sq-28)%12; };
        board[index(from)]=board[index(pawn)]=0; board[index(to)]=black?9:5;
        if(!attacked(board,black)) out.insert(square(from)+square(to));
    }
    return out;
}
struct Snapshot {
    LegacyBoard board=cb; LegacyPosition position=pos[g];
    int ply=g,top=mvtop; unsigned char white=lwm,black=lsm;
    std::array<int,16> counts; std::array<unsigned char,192> lists;
    Snapshot() { std::copy(fln,fln+16,counts.begin()); std::memcpy(lists.data(),flc,sizeof(flc)); }
    void check(bool checkTop=true) const {
        require(!std::memcmp(&board,&cb,sizeof(cb)) && !std::memcmp(&position,&pos[g],sizeof(position)) &&
            g==ply && (!checkTop||mvtop==top) && lwm==white && lsm==black &&
            std::equal(counts.begin(),counts.end(),fln) && !std::memcmp(lists.data(),flc,sizeof(flc)),"EP query changed position");
    }
};
static void check_ep(const std::set<std::string>& expected) {
    prepare(); Snapshot state;
    require(reference_ep()==expected,"independent EP reference differs from fixture");
    require(bool(generate_moves_ep())==!expected.empty(),"EP existence differs"); state.check();
    for(int variant=0;variant<2;++variant) {
        int start=mvtop;
        if(g&1) { if(variant) mv2sep(); else mv1sep(); }
        else { if(variant) mv2wep(); else mv1wep(); }
        std::set<std::string> actual;
        for(int i=start;i<mvtop;++i) actual.insert(uci({int(mv1[i]),mv2[i]}));
        require(actual==expected,"EP entry point differs"); mvtop=start; state.check();
    }
    std::set<std::string> actual;
    for(auto m:moves()) if((m.a>>8)>16 && (m.a>>8)<120) actual.insert(uci(m));
    require(actual==expected,"normal generator EP set differs");
}
static std::vector<std::string> fields(const std::string& fen) {
    std::istringstream in(fen); std::vector<std::string> out; for(std::string s;in>>s;) out.push_back(s); return out;
}
static std::string join(const std::vector<std::string>& f) {
    std::string out; for(auto& s:f) { if(!out.empty()) out+=' ';out+=s; } return out;
}
static std::string mirrored(const std::string& fen) {
    auto f=fields(fen);std::vector<std::string> ranks;std::istringstream in(f[0]);
    for(std::string rank;std::getline(in,rank,'/');) ranks.push_back(rank);
    f[0].clear();std::reverse(ranks.begin(),ranks.end());
    for(auto& rank:ranks) { if(!f[0].empty()) f[0]+='/'; for(char c:rank) f[0]+=c>='a'&&c<='z'?char(c-'a'+'A'):c>='A'&&c<='Z'?char(c-'A'+'a'):c; }
    f[1]=f[1]=="w"?"b":"w"; if(f[3]!="-") f[3][1]=char('9'-f[3][1]+'0');return join(f);
}
static std::string mirror_move(std::string s) { s[1]=char('9'-s[1]+'0');s[3]=char('9'-s[3]+'0');return s; }
static std::string before_double(const std::string& fen) {
    auto f=fields(fen); char board[8][8]{};int rank=7,file=0;
    for(char c:f[0]) { if(c=='/') {--rank;file=0;} else if(c>='1'&&c<='8') file+=c-'0';else board[rank][file++]=c; }
    bool whiteNext=f[1]=="w"; file=f[3][0]-'a';int from=whiteNext?6:1,to=whiteNext?4:3;
    board[from][file]=board[to][file];board[to][file]=0;f[0].clear();
    for(rank=7;rank>=0;--rank) { int empty=0;if(rank<7) f[0]+='/';for(file=0;file<8;++file) { char c=board[rank][file];if(!c) ++empty;else {if(empty) f[0]+=char('0'+empty);empty=0;f[0]+=c;}} if(empty)f[0]+=char('0'+empty); }
    f[1]=whiteNext?"b":"w";f[3]="-";return join(f);
}
static std::string identity() {
    std::string key; for(int rank=0;rank<8;++rank) for(int file=0;file<8;++file) key+=char(cb.cc[28+12*rank+file]);
    key+=char(g&1);key+=char(pos[g].wr1);key+=char(pos[g].sr1);
    auto ep=reference_ep();key+=ep.empty()?char(0):char((g&1)?pos[g].lwm:pos[g].lsm);return key;
}
static void collect(int depth,std::set<std::string>& out) {
    if(!depth) { out.insert(identity());return; }
    for(auto m:moves()) { make_move_h(m.a,m.b);collect(depth-1,out);undo_move(); }
}
int main() {
    try {
        set_winflag(1);init_tables();init_mytime();require(set_opmode(2)==1024,"Position allocation");
        struct Case { const char* name;const char* fen;std::set<std::string> legal;bool reachable; };
        const Case cases[]={
            {"no adjacent pawn","k7/8/8/3p4/8/8/8/4K3 w - d6 0 2",{},true},
            {"ordinary EP","k7/8/8/3pP3/8/8/8/4K3 w - d6 0 2",{"e5d6"},true},
            {"vertical pin","k3r3/8/8/3pP3/8/8/8/4K3 w - d6 0 2",{},true},
            {"diagonal pin","k7/8/1K6/2Pp4/8/8/5b2/8 w - d6 0 2",{},true},
            {"horizontal opening","k7/8/8/KPp4r/8/8/8/8 w - c6 0 2",{},true},
            {"double step opens rook check","7k/K6r/8/2Pp4/8/8/8/8 w - d6 0 2",{},true},
            {"capture checking pawn","k7/8/8/1PpK4/8/8/8/8 w - c6 0 2",{"b5c6"},true},
            {"block bishop check","1K5k/8/8/2Pp4/8/8/7b/8 w - d6 0 2",{"c5d6"},false},
            {"one candidate pinned","7K/k7/8/2PpP3/8/8/8/b7 w - d6 0 2",{"c5d6"},true},
            {"two legal candidates","k7/8/8/2PpP3/8/8/8/4K3 w - d6 0 2",{"c5d6","e5d6"},true},
            {"knight check remains","k7/8/8/3pP3/8/5n2/8/4K3 w - d6 0 2",{},false},
            {"pawn check remains","k7/8/8/3pP3/8/8/3p4/4K3 w - d6 0 2",{},false},
            {"rook check remains","k3r3/8/8/2Pp4/8/8/8/4K3 w - d6 0 2",{},false},
            {"double check","k3r3/8/8/2Pp4/8/5n2/8/4K3 w - d6 0 2",{},false}
        };
        int checked=0,precursors=0,positionChecks=0;
        for(auto& c:cases) for(bool reflected:{false,true}) {
            std::string fen=reflected?mirrored(c.fen):c.fen;std::set<std::string> expected;
            for(auto move:c.legal) expected.insert(reflected?mirror_move(move):move);
            load(fen);check_ep(expected);++checked;
            std::set<std::string> exact;collect(2,exact);
            for(int steps:{1,2,4}) {
                std::string copy=fen;set_stopflag(0);Anzahl_DoAll(2,copy.data(),steps);
                std::string result=get_result();result=result.substr(0,result.find(' '));result.erase(std::remove(result.begin(),result.end(),'.'),result.end());
                require(std::stoull(result)==exact.size(),std::string("Position/canonical reference differs: ")+c.name);++positionChecks;
            }
            if(c.reachable) {
                load(before_double(fen));auto f=fields(fen);std::string text=f[3].substr(0,1)+(f[1]=="w"?"7":"2")+f[3].substr(0,1)+(f[1]=="w"?"5":"4");
                auto all=moves();auto m=std::find_if(all.begin(),all.end(),[&](Move m){return uci(m)==text;});
                require(m!=all.end(),"predecessor double step missing");make_move_h(m->a,m->b);check_ep(expected);undo_move();++precursors;
            }
        }
        std::cout<<checked<<" EP fixtures, "<<precursors<<" double-step paths and "<<positionChecks<<" independent Position checks passed\n";
        return 0;
    } catch(const std::exception& e) { std::cerr<<e.what()<<'\n';return 1; }
}
