#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include "../MoveGen/mgen6.h"
#include <algorithm>
#include <array>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
extern "C" {
#include "../MoveGen/move.h"
#include "../MoveGen/mgen_s.h"
struct Position { uint64_t hashkey,hashkey2;int a,b,p1;unsigned char d,z,*pbt2,hash_doppel,lwm,lsm,wch1,sch1,wr1,sr1,fesselung; };
struct Hash4 { unsigned int code1;unsigned short code2;short level;uint64_t count; };
struct Hash8 { unsigned int code1;unsigned short code2;short count; };
struct Hash1 { unsigned int code1;unsigned short code2;short level; };
struct Hash2 { unsigned int code1;char score,level,move,level2; };
extern size_t hsize,hsize1;
extern unsigned int hmask,hmask1;
extern uint64_t hstep,step0,step1,position_mask;
extern int size_hash,opmode,anz_step,level,maxlevel,overflow_flag,g,mvtop;
extern unsigned int mv1[10012];
extern unsigned char mv2[10012],lwm,lsm;
extern Position pos[72];
extern uint64_t zobrist[16][128],zobrist_rochade[4];
extern int64_t *hash0;
extern Hash1 *hash1;
extern Hash2 *hash2;
extern Hash4 *hash3;
extern Hash8 *hash7,*hash8;
void hash1_delete(void);void hash2_delete(void);void hash3_delete(void);
void search_perft1c(void);void search_perft1c_nor(void);
void search_perft1d(void);void search_perft1d_nor(void);
}
static_assert(sizeof(Position)==48 && sizeof(Hash4)==16 && sizeof(Hash8)==8,"legacy layouts");
static void require(bool ok,const char* why) { if(!ok) throw std::runtime_error(why); }
struct Reservation { void* pointer;size_t bytes;std::vector<void*> pages; };
static std::vector<Reservation> reservations;
static size_t allocationCalls=0,failCall=0,limit=SIZE_MAX,zeroCalls=0;
extern "C" void* hash_test_malloc(size_t bytes) {
    ++allocationCalls;
    if(bytes>limit || allocationCalls==failCall) return nullptr;
    void* p=VirtualAlloc(nullptr,bytes,MEM_RESERVE,PAGE_READWRITE);
    require(p!=nullptr,"virtual reservation failed");reservations.push_back({p,bytes,{}});return p;
}
extern "C" void hash_test_free(void* pointer) {
    if(!pointer) return;
    auto i=std::find_if(reservations.begin(),reservations.end(),[&](auto& r){return r.pointer==pointer;});
    require(i!=reservations.end(),"unknown allocation freed");
    require(VirtualFree(pointer,0,MEM_RELEASE)!=0,"virtual free failed");reservations.erase(i);
}
extern "C" void* hash_test_memset(void* pointer,int value,size_t bytes) {
    auto i=std::find_if(reservations.begin(),reservations.end(),[&](auto& r){return r.pointer==pointer;});
    if(i==reservations.end()) return std::memset(pointer,value,bytes);
    require(value==0 && bytes==i->bytes,"table clear length overflow");++zeroCalls;
    for(void* page:i->pages) std::memset(page,0,4096);
    return pointer;
}
static size_t bytes_of(void* pointer) {
    auto i=std::find_if(reservations.begin(),reservations.end(),[&](auto& r){return r.pointer==pointer;});
    if(i==reservations.end()) { std::cerr<<"missing pointer "<<pointer<<" active="<<reservations.size()<<"\n"; for(auto& r:reservations)std::cerr<<r.pointer<<" bytes="<<r.bytes<<"\n"; throw std::runtime_error("allocation missing"); } return i->bytes;
}
static void commit(void* pointer,size_t bytes) {
    auto address=reinterpret_cast<uintptr_t>(pointer);
    auto i=std::find_if(reservations.begin(),reservations.end(),[&](auto& r){auto p=reinterpret_cast<uintptr_t>(r.pointer);return address>=p && address+bytes<=p+r.bytes;});
    require(i!=reservations.end(),"probe beyond allocation");
    for(auto p=address&~uintptr_t(4095);p<address+bytes;p+=4096) {
        void* page=reinterpret_cast<void*>(p);
        if(std::find(i->pages.begin(),i->pages.end(),page)==i->pages.end()) {
            require(VirtualAlloc(page,4096,MEM_COMMIT,PAGE_READWRITE)==page,"page commit failed");i->pages.push_back(page);
        }
    }
}
static void check_layout(int mode,int mb) {
    const size_t bytes=size_t(mb)*1024*1024;
    if(mode==1) {
        const size_t entries=bytes/32;
        require(hsize==entries+8 && hmask==entries-1,"Perft layout");
        require(bytes_of(hash3)==hsize*16 && bytes_of(hash7)==hsize*8 && bytes_of(hash8)==hsize*8,"Perft allocation sizes");
        commit(hash3+hmask,3*sizeof(Hash4));hash3[size_t(hmask)+2].count=42;require(hash3[size_t(hmask)+2].count==42,"Perft padding index");
    } else if(mode==2) {
        const size_t entries=bytes/8,intermediate=bytes/4/8;
        require(hstep==entries && hsize==entries+128 && hmask==entries-1,"Position end layout");
        require(hsize1==intermediate+8 && hmask1==intermediate-1,"Position intermediate layout");
        require(bytes_of(hash0)==hsize*8 && bytes_of(hash1)==hsize1*8,"Position allocation sizes");
    } else {
        require(hsize==bytes/8+8 && hmask==bytes/8-1,"Mate layout");
        require(bytes_of(hash2)==hsize*8,"Mate allocation size");
        commit(hash2+hmask,sizeof(Hash2));hash2[hmask].code1=42;require(hash2[hmask].code1==42,"Mate high index");
    }
}
static uint64_t child_key(int a,int b) {
    auto w=lwm,s=lsm;make_move_h_hashonly(a,b);auto key=pos[g].hashkey;--g;lwm=w;lsm=s;return key;
}
static void high_position_probes() {
    require(set_opmode(2)>0 && hash_set(32768)==32768,"32 GB Position setup");
    for(auto fn:{search_perft1c,search_perft1c_nor,search_perft1d,search_perft1d_nor}) {
        hash_zero();char fen[]="k7/8/8/8/8/8/8/4K3 w - -";require(!fen_to_board(fen),"boundary FEN");
        lwm=pos[g].lwm;lsm=pos[g].lsm;make_king();mvtop=0;int n=generate_moves();
        std::vector<std::pair<int,int>> moves;for(int i=0;i<n;++i)moves.emplace_back(mv1[i],mv2[i]);mvtop=0;
        require(!moves.empty(),"boundary moves missing");
        bool partitioned=(fn==search_perft1d || fn==search_perft1d_nor);
        anz_step=partitioned?32:1;step0=partitioned?31*hstep:0;step1=step0+hstep-1;position_mask=hstep*anz_step-1;
        uint64_t target=UINT64_C(0x1234000000000000)|(partitioned?position_mask:UINT64_C(0xffffffff));
        auto first=moves.front();auto delta=child_key(first.first,first.second)^pos[g].hashkey;pos[g].hashkey=target^delta;
        for(auto [a,b]:moves) {
            uint64_t v=child_key(a,b)&position_mask;
            if(v>=step0 && v<=step1) commit(hash0+size_t(v-step0),32*sizeof(*hash0));
        }
        commit(hash0,sizeof(*hash0));hash0[0]=0;
        hash0[UINT64_C(0xffffffff)]=int64_t(target^UINT64_C(0x100000000));
        level=0;maxlevel=1;overflow_flag=0;fn();
        require(!overflow_flag && uint64_t(hash0[UINT64_C(0x100000000)])==target,"probe must reach padding past 2^32");
        require(hash0[0]==0,"32-bit probe wrapped to slot zero");
        if(partitioned)require(step0>UINT64_C(0xffffffff),"virtual partition did not exceed 32 bits");
    }
}
int main() {
    try {
        set_winflag(1);init_tables();init_mytime();require(size_hash==1024,"default must be 1 GB");
        const uint64_t splitmixFirst[]={UINT64_C(0xe4bacea5c4b9b499),UINT64_C(0x3f4aa7cabeb88997),UINT64_C(0x0588ce2416e3da9c),UINT64_C(0x28cb80f7cecb0a9f)};
        const uint64_t splitmixRights[]={UINT64_C(0x5e4c2fd8168c9fc8),UINT64_C(0x8ea94459e8009921),UINT64_C(0x9b69a0b3bdc6cc00),UINT64_C(0x7be2f47e7f755aaa)};
        const uint64_t splitmixHistory[]={UINT64_C(0x5fe62ba230975670),UINT64_C(0xc79991f3d2a239d8),UINT64_C(0x45b45b86b1fef4fd),UINT64_C(0x89ef4e18306a75c6)};
        for(int i=0;i<4;++i) {
            require(zobrist[0][i]==splitmixFirst[i],"SplitMix64 initial vector");
            require(zobrist_rochade[i]==splitmixRights[i],"SplitMix64 castling vector");
            require(pos[i].hashkey==splitmixHistory[i],"SplitMix64 history vector");
        }
        for(int mode:{1,2,3,4}) {
            require(set_opmode(mode)>0,"mode setup");
            for(int mb:{1024,2048,4096,8192,16384,32768}) {
                require(hash_set(mb)==mb,"size selection");check_layout(mode,mb);
            }
            auto calls=allocationCalls;
            for(int bad:{-1,0,64,128,256,512,1536,65536})require(hash_set(bad)==32768,"invalid size changed selection");
            require(allocationCalls==calls,"invalid sizes allocated memory");
        }
        high_position_probes();
        for(int mode:{1,2,3,4}) {
            limit=SIZE_MAX;failCall=0;require(set_opmode(mode)>0,"failure test mode");
            limit=size_t(1536)*1024*1024;require(hash_set(32768)==1024,"failure must fall back to 1 GB");check_layout(mode,1024);
            limit=1;require(hash_set(32768)==0 && reservations.empty(),"complete failure leaked a table");
            limit=SIZE_MAX;require(hash_set(1024)==1024,"recovery after allocation failure");
        }
        require(set_opmode(1)>0,"partial failure mode");failCall=allocationCalls+2;
        require(hash_set(32768)==1024 && reservations.size()==3,"partial failure cleanup/fallback");
        failCall=0;hash1_delete();hash2_delete();hash3_delete();require(reservations.empty(),"tables leaked");
        std::cout<<"SplitMix64 vectors, 24 hash layouts, rejected sub-GB sizes, 64-bit boundary/partition probes and allocation-failure fallback passed\n";
        return 0;
    } catch(const std::exception& e) { std::cerr<<e.what()<<'\n';return 1; }
}
