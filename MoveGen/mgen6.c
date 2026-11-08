// Stellungsdaten:
// Zugrecht: g gerade = Weiss, g ungerade = Schwarz.
// lsm>16: Feld des schwarzen Doppelschrittbauern bei Weiss am Zug.
// lwm>16: Feld des weissen Doppelschrittbauern bei Schwarz am Zug.
// lwm,lsm <16: zuletzt gezogene Figurenart.
// Bei Stellungseingabe muss Springerschach in lwm bzw. lsm vermerkt werden.
//
// wr1, sr1: Rochaderechte fuer Weiss bzw. Schwarz; Bitwerte 1 (kurz), 2 (lang).
// wch1, sch1: Anzahl der Schachgebote gegen den jeweiligen Koenig.
// Sie werden von makewking() bzw. makesking() berechnet.
//
// (uint64)-1 unterscheidet das Zugrecht im Hashschluessel.
// pos[g+1].hashkey = pos[g].hashkey ^ zobrist[c][a] ^ zobrist[c][b] ^ (uint64)-1;
// hashkey: Schluessel ohne EP-Kennung fuer die inkrementelle Fortschreibung.
// hashkey2: Vergleichsschluessel mit EP-Kennung.
// hash_doppel: Feld des fuer das EP-Hashen vorgemerkten Doppelschrittbauern.
//
// Zugcodierung:
// mv2: Zielfeld.
// Unteres Byte von mv1: Index in der Figurenliste.
// Oberes Byte von mv1:
//   >0, <16: Umwandlungsfigur; der Zug enthaelt den Bauernzug.
//            Dabei koennen sich bis zu drei Figurenlisten aendern.
//   ==16: Doppelschritt; das Bauernfeld wird in lwm bzw. lsm vermerkt.
//   >16, <120: Feld des en passant geschlagenen Bauern.
//              Bei Weiss: z=b-12, bei Schwarz: z=b+12; b ist das Zielfeld.
//   >=120: Rochade; der Zug enthaelt den Turmzug.
//          120/121: kurze/lange weisse Rochade.
//          122/123: kurze/lange schwarze Rochade.
//
// Hashmoves: Verschiedene Zugwege koennen dieselbe Stellung mit unterschiedlicher
// Figurenlistenreihenfolge erreichen. Ein gespeicherter Listenindex allein reicht
// deshalb nicht zur Identifikation eines Zuges. Dafuer werden Figurenart,
// Ausgangs- und Zielfeld sowie die Sonderzugkennung benoetigt.
//
// En passant kann durch das Entfernen beider Bauern eine Angriffslinie oeffnen.
// Die EP-Pruefung testet deshalb die Koenigssicherheit auf dem Brett nach dem Schlag.

#define _CRT_SECURE_NO_DEPRECATE 1

#include <windows.h>
#include <stdint.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>

#include "move.h"
#include "mgen_s.h"
#include "wintools.h"

extern struct POS1
{
  uint64 hashkey,hashkey2;
  int a,b,p1;
  unsigned char d,z,*pbt2;
  unsigned char hash_doppel;
  unsigned char lwm,lsm,wch1,sch1,wr1,sr1,fesselung;
} pos[72];

extern union board1
{
  unsigned char cc[148];
  long ii[37];
} cb;

extern union board2
{
  signed char cc[148];
  long ii[37];
} sb,wb,sb2;

typedef struct
{
  unsigned int hashcode1;
  unsigned short hashcode2;
  short level;
} Hash1;

typedef struct
{
  unsigned int hashcode1;
  unsigned short hashcode2;
  short level;
  uint64 movecount;
} Hash4;

typedef struct
{
  unsigned int hashcode1;
  unsigned short hashcode2;
  short movecount;
} Hash8;

typedef struct
{
  unsigned int hashcode1;
  char score;
  char level;
  char move_nr;
  char level2;
} Hash2;

_Static_assert(sizeof(Hash1) == 8, "Hash1 layout");
_Static_assert(sizeof(Hash4) == 16, "Hash4 layout");
_Static_assert(sizeof(Hash8) == 8, "Hash8 layout");
_Static_assert(sizeof(Hash2) == 8, "Hash2 layout");
_Static_assert(sizeof(uint64) == 8 && sizeof(long) == 4, "Windows hash/board word sizes");


extern int g;

extern int fln[16];
extern unsigned char flc[16][12];
extern unsigned char *flp;
extern unsigned char lwm,lsm;

extern char knight_attack[144+144];

extern uint64 zobrist[16][128];
extern uint64 zobrist_rochade[4];

extern __int64 bit64[64],rook64[64],bishop64[64];
extern __int64 bit120[120],rook120[120],bishop120[120];
extern __int64 rook_bits,bishop_bits;

short fen_board[64]; // Index von der Fen-Durchlaufordnung (oben nach unten) auf das 12*12 board
unsigned char fen_piece[256];
int anz_fen;
char akt_fen[256]; // Zwischenspeicher für fen
char my_fens[32][256];
char bench_fens[8][129];
int bench_level[8];

unsigned int hmask,hmask1;
size_t hsize,hsize1;
int anz_step;
uint64 hstep,step0,step1,position_mask;
_Static_assert(sizeof(size_t)==8, "64-bit table sizes required");

char fig1[16][2]={" ","D","T","L","S","B","K",
  "#","k","b","s","l","t","d","?",};
char fig2[16][2]={" ","Q","R","B","N"," ","K",
  "#","k"," ","n","b","r","q","?",};
char pmv[148][3];        // Feldnamen a1,a2 etc.

// Grafik Unterstuetzung
char board_color[148];   // 0=schwarz, 1=weiss
static char offset1_x[64];
static char offset1_y[64];

unsigned int mv1[10012];
unsigned char mv2[10012];

int mvtop,level,maxlevel;

// perft (movepath)
Hash4 *hash3;
Hash8 *hash7;
Hash8 *hash8;

// Anzahl Stellungen
__int64 *hash0;
Hash1 *hash1;
int overflow_flag;

// Matt-Suche
Hash2 *hash2;

volatile LONG stopflag;
int winflag, opmode, size_hash;
static SRWLOCK message_lock = SRWLOCK_INIT;
static __declspec(thread) char message_copy[256];
int anz_result, anz_info, anz_text;
char result_text[128];
char info_text[128];
char etext[128][256]; // 128 Text-Zeilen

int hash1_set(int size_mb);
int hash2_set(int size_mb1, int size_mb2);
int hash3_set(int size_mb);
void hash1_zero();
void hash2_zero();
void hash3_zero();
int hash_set(int size_mb);
void reset_anzcounter();
void hash1_delete();
void hash2_delete();
void hash3_delete();

void set_stopflag(int sf)
{
  InterlockedExchange(&stopflag, sf);
}

int get_stopflag()
{
  return InterlockedCompareExchange(&stopflag, 0, 0);
}

// 0=console, 1=window
void set_winflag(int wf)
{
  int i;
  
  winflag=wf;
  size_hash=1024; // Standard: 1 GB; muss vor set_opmode() feststehen.
  anz_text=0;
  reset_anzcounter();

  for (i=0; i<32; i++)
    memset(my_fens[i],0,256);

  strcpy(my_fens[0],"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq -; id \"Initial Position\"");
  strcpy(my_fens[1],"r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -; id \"Perft test 1\"");
  strcpy(my_fens[2],"8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - -; id \"Perft test 2\"");

  strcpy(my_fens[3],"n1rb4/1p3p1p/1p6/1R5K/8/p3p1PN/1PP1R3/N6k w - -; id \"Mate in 5\"");
  strcpy(my_fens[4],"5RKb/4P1n1/2p4p/3p2p1/3B2Q1/5B2/r6k/4r3 w - -; id \"Mate in 5\"");
  strcpy(my_fens[5],"7r/3B4/k7/8/6Qb/8/Kn6/6R1 w - - ; id \"Mate in 5\"");
  strcpy(my_fens[6],"b1N1rb2/3p4/r6p/2Pp1p1K/3Pk3/2PN1p2/2B2P2/8 w - -; id \"Mate in 6\"");
  strcpy(my_fens[7],"1kN2bb1/4r1r1/Q1P1p3/8/6n1/8/8/2B1K2B w - -; id \"Mate in 6\"");
  strcpy(my_fens[8],"8/8/7K/6p1/NN5k/8/6PP/8 w - -; id \"Mate in 7\""); 
  strcpy(my_fens[9],"8/2p5/2Pb4/2pp3R/1ppk1pR1/2n2P1p/1B2PPpP/K7 w - -; id \"Mate in 7\"");
  strcpy(my_fens[10],"8/6pp/8/p5p1/Pp6/1P3p2/pPK4P/krQ4R w - -; id \"Mate in 7\"");
  strcpy(my_fens[11],"8/8/3B4/3p4/1p1p2B1/1p1P4/1P3p1p/KN2k1br w - -; id \"Mate in 8\"");
  strcpy(my_fens[12],"5B2/Kn3b1k/p5p1/2p5/6p1/8/2r2r2/6Q1 w - -; id \"Mate in 8\"");
  strcpy(my_fens[13],"8/7b/6p1/3R4/p7/p7/p1p5/k1K5 w - -; id \"Mate in 10\"");
  strcpy(bench_fens[0],my_fens[0]);
  strcpy(bench_fens[1],my_fens[1]);
  strcpy(bench_fens[2],my_fens[4]);
  strcpy(bench_fens[3],my_fens[5]);
  strcpy(bench_fens[4],my_fens[7]);
  bench_level[0]=12;
  bench_level[1]=10;
  bench_level[2]=10;
  bench_level[3]=10;
  bench_level[4]=12;
  anz_fen=14;
}


// 1=MovePath, 2=Positions, 3=Mate, 4=Benchmark.
// Passt die Hashtabellen an den Modus an; Rueckgabe: eingestellte Groesse in MiB.
int set_opmode(int op)
{
  if (op==opmode) return size_hash;
  opmode=op;
  return hash_set(size_hash); // Auf andere Arrays umschalten
}

// Hashgroesse fuer den aktuellen Modus (1 bis 4) setzen.
// Rueckgabe: tatsaechlich eingestellte Groesse in MiB, 0 bei Speicherfehler.
int hash_set(int size_mb)
{
  int erg;
  
  if ((opmode<1)||(opmode>4)) return size_hash;
  if (size_mb<1024 || size_mb>32768 || (size_mb & (size_mb-1))!=0) return size_hash;
  
  if (opmode==1)
  {
    erg = hash1_set(size_mb);
    if (erg==0)
    {
      hash1_zero();
      size_hash = size_mb;
      return size_mb;
    }
    erg = hash1_set(1024);
    if (erg != 0) {
      hash1_delete(); hash2_delete(); hash3_delete();
      size_hash=0;
      return 0;
    }
    hash1_zero();
    size_hash=1024;
    return 1024;
  }

  if (opmode==2)
  {
    erg = hash2_set(size_mb,size_mb/4);

    if (erg==0)
    {
      hash2_zero();
      size_hash = size_mb;
      return size_mb;
    }
    erg = hash2_set(1024,256);
    if (erg != 0) {
      hash1_delete(); hash2_delete(); hash3_delete();
      size_hash=0;
      return 0;
    }
    hash2_zero();
    size_hash=1024;
    return 1024;
  }

  if ((opmode==3)||(opmode==4)) // Benchmark laueft mit Mattsuche
  {
    erg = hash3_set(size_mb);
    if (erg==0)
    {
      hash3_zero();
      size_hash = size_mb;
      return size_mb;
    }
    erg = hash3_set(1024);
    if (erg != 0) {
      hash1_delete(); hash2_delete(); hash3_delete();
      size_hash=0;
      return 0;
    }
    hash3_zero();
    size_hash=1024;
    return 1024;
  }

  return size_hash;
}

void hash_zero()
{
  if (opmode == 1) hash1_zero();
  if (opmode == 2) hash2_zero();
  if (opmode == 3) hash3_zero();
  if (opmode == 4) hash3_zero();
}

// Ausgabe des Ergebnisses der Berechnung
void set_result(char *txt)
{
  AcquireSRWLockExclusive(&message_lock);
  strncpy_s(result_text, sizeof(result_text), txt, _TRUNCATE);
  anz_result++;
  ReleaseSRWLockExclusive(&message_lock);
  if (winflag==0) printf("%s\n",txt);
}

// Ausgabe einer Info-Meldung
void set_info(char *txt)
{
  AcquireSRWLockExclusive(&message_lock);
  strncpy_s(info_text, sizeof(info_text), txt, _TRUNCATE);
  anz_info++;
  ReleaseSRWLockExclusive(&message_lock);
  if (winflag==0) printf("%s\n",txt);
}

// Ausgabe von Ablauf-Meldungen, z.B. Zuege, an denen gerade gerechnet wird.
void set_text(char *txt)
{
  AcquireSRWLockExclusive(&message_lock);
  if (anz_text<127) {
    strncpy_s(etext[anz_text], sizeof(etext[anz_text]), txt, _TRUNCATE);
    anz_text++;
  }
  ReleaseSRWLockExclusive(&message_lock);
  if (winflag==0) printf("%s\n",txt);
}

char *dot_print(__int64 n)
{
  static char c1[32];
  char c2[32];
  int len,i,j,k;

  memset(c1,0,32);
  memset(c2,0,32);
  sprintf(c1,"%I64d",n);
  len=(int)strlen(c1);

  k=0;
  j=0;
  for (i=len-1; i>=0; i--)
  {
    c2[j++]=c1[i];
    k++;
    if ((k==3)&&(i!=0))
    {
      c2[j++]='.';
      k=0;
    }
  }

  len=j;
  memset(c1,0,32);
  for (i=0; i<len; i++)
  {
    c1[i]=c2[len-i-1];
  }

  return c1;
}


// Counter fuer die Steuerung des Ausgabe unter Windows-Gui
void reset_anzcounter()
{
  AcquireSRWLockExclusive(&message_lock);
  result_text[0] = info_text[0] = '\0';
  anz_result = anz_info = anz_text = 0;
  ReleaseSRWLockExclusive(&message_lock);
}

int get_anzresult()
{
  int count;
  AcquireSRWLockShared(&message_lock);
  count = anz_result;
  ReleaseSRWLockShared(&message_lock);
  return count;
}


// Erhoehung des Counter bedeutet: Neue Meldung liegt vor
int get_anzinfo()
{
  int count;
  AcquireSRWLockShared(&message_lock);
  count = anz_info;
  ReleaseSRWLockShared(&message_lock);
  return count;
}

int get_anztext()
{
  int count;
  AcquireSRWLockShared(&message_lock);
  count = anz_text;
  ReleaseSRWLockShared(&message_lock);
  return count;
}

char *get_result()
{
  AcquireSRWLockShared(&message_lock);
  strcpy_s(message_copy, sizeof(message_copy), result_text);
  ReleaseSRWLockShared(&message_lock);
  return message_copy;
}

char *get_info()
{
  AcquireSRWLockShared(&message_lock);
  strcpy_s(message_copy, sizeof(message_copy), info_text);
  ReleaseSRWLockShared(&message_lock);
  return message_copy;
}

char *get_text(int n)
{
  AcquireSRWLockShared(&message_lock);
  strcpy_s(message_copy, sizeof(message_copy), (n>=0 && n<127) ? etext[n] : "");
  ReleaseSRWLockShared(&message_lock);
  return message_copy;
}

char *get_fen(int n)
{
  return my_fens[n];
}

// FEN-Text speichern und seinen Index in der FEN-Liste zurueckgeben.
int add_fen(char *fen)
{
  if (!fen || strnlen_s(fen,sizeof(my_fens[0]))>=sizeof(my_fens[0])) return -1;
  if (anz_fen<32)
  {
    strcpy(my_fens[anz_fen++],fen);
    return anz_fen-1;
  }
  else
  {
    strcpy(my_fens[anz_fen-1],fen);
    return anz_fen-1;
  }
}


void printboard(void);

// perft
void hash1_delete()
{
  if (hash3 != NULL)
  {  
     free(hash3);
     hash3=NULL;
  }
  if (hash7 != NULL)
  {  
     free(hash7);
     hash7=NULL;
  }
  if (hash8 != NULL)
  {  
     free(hash8);
     hash8=NULL;
  }
}

// Fuer Anzahl Stellungen
void hash2_delete()
{
  if (hash0 != NULL)
  {  
     free(hash0);
     hash0=NULL;
  }

  if (hash1 != NULL)
  {  
     free(hash1);
     hash1=NULL;
  }
}


// Fuer Matt-Suche
void hash3_delete()
{
  if (hash2 != NULL)
  {  
     free(hash2);
     hash2=NULL;
  }
}


// Perft
int hash1_set(int size_mb)
{
  size_t entries=(size_t)size_mb*1024*1024/(sizeof(Hash4)+2*sizeof(Hash8));
  hash1_delete(); hash2_delete(); hash3_delete();
  hsize=entries+8;
  hmask=(unsigned int)(entries-1);
  hash3=malloc(hsize*sizeof(*hash3));
  if (hash3==NULL) return -1;
  hash7=malloc(hsize*sizeof(*hash7));
  if (hash7==NULL) return -2;
  hash8=malloc(hsize*sizeof(*hash8));
  if (hash8==NULL) return -3;
  return 0;
}

// Endstellungen plus die bisherige Zwischenknotentabelle mit 25 Prozent.
int hash2_set(int size_mb1, int size_mb2)
{
  hash1_delete(); hash2_delete(); hash3_delete();
  hstep=(uint64)size_mb1*1024*1024/sizeof(*hash0);
  hsize=(size_t)hstep+128;
  hmask=(unsigned int)(hstep-1);
  position_mask=hstep-1;
  hsize1=(size_t)size_mb2*1024*1024/sizeof(*hash1)+8;
  hmask1=(unsigned int)(hsize1-9);
  hash0=malloc(hsize*sizeof(*hash0));
  if (hash0==NULL) return -1;
  hash1=malloc(hsize1*sizeof(*hash1));
  if (hash1==NULL) return -2;
  return 0;
}

// Matt/Benchmark: maximal 2^32 Eintraege; ein physischer Index bleibt 32 Bit.
int hash3_set(int size_mb)
{
  size_t entries=(size_t)size_mb*1024*1024/sizeof(*hash2);
  hash1_delete(); hash2_delete(); hash3_delete();
  hsize=entries+8;
  hmask=(unsigned int)(entries-1);
  hash2=malloc(hsize*sizeof(*hash2));
  if (hash2==NULL) return -1;
  return 0;
}

// Fuer perft
void hash1_zero()
{  
  if (hash3 != NULL) memset(hash3,0,(size_t)hsize * sizeof(*hash3));
  if (hash7 != NULL) memset(hash7,0,(size_t)hsize * sizeof(*hash7));
  if (hash8 != NULL) memset(hash8,0,(size_t)hsize * sizeof(*hash8));
}

// Fuer Anzahl Stellungen
void hash2_zero()
{
  if (hash0 != NULL) memset(hash0,0,(size_t)hsize * sizeof(*hash0));
  if (hash1 != NULL) memset(hash1,0,(size_t)hsize1 * sizeof(*hash1));
}

// Fuer Matt Suche
void hash3_zero()
{
  if (hash2 != NULL) memset(hash2,0,(size_t)hsize * sizeof(*hash2));
}


// Anzahl Figuren der Farbe am Zug
int get_anz_pieces()
{  
  if (g&1)
    return fln[9]+fln[10]+fln[11]+fln[12]+fln[13]+1;    
  else
    return fln[1]+fln[2]+fln[3]+fln[4]+fln[5]+1;     
}

// Check auf Weisser Koenig im Schach und ob Springer-Schach
int anz_check_w(int *knight_check)
{
  unsigned char *pbt,*pbtx;
  int anz=0;

  *knight_check=0;

  pbt=cb.cc+flc[6][0];   /* Pointer auf Position des Koenigs */
  pbtx=pbt;

  if (fln[13]+fln[12]>0)
  {
    pbt=pbtx;
    do
    {
      pbt+=1;
    } while (*pbt == 0);
    if (*pbt>11) anz++;
    pbt=pbtx;
    do
    {
      pbt-=1;
    } while (*pbt == 0);
    if (*pbt>11) anz++;
    pbt=pbtx;
    do
    {
      pbt+=12;
    } while (*pbt == 0);
    if (*pbt>11) anz++;
    pbt=pbtx;
    do
    {
      pbt-=12;
    } while (*pbt == 0);
    if (*pbt>11) anz++;
  }

  if (fln[13]+fln[11]>0)
  {
    pbt=pbtx;
    do
    {
      pbt+=11;
    } while (*pbt == 0);
    if ((*pbt>10)&&(*pbt!=12)) anz++;
    pbt=pbtx;
    do
    {
      pbt+=13;
    } while (*pbt == 0);
    if ((*pbt>10)&&(*pbt!=12)) anz++;
    pbt=pbtx;
    do
    {
      pbt-=11;
    } while (*pbt == 0);
    if ((*pbt>10)&&(*pbt!=12)) anz++;
    pbt=pbtx;
    do
    {
      pbt-=13;
    } while (*pbt == 0);
    if ((*pbt>10)&&(*pbt!=12)) anz++;
  }
  
  pbt=pbtx;
      
  if (*(pbt+10)==10) 
  {      
    anz++;
    *knight_check=10;
  }
  if (*(pbt+23)==10) 
  {      
    anz++;
    *knight_check=10;
  }
  if (*(pbt+25)==10) 
  {      
    anz++;
    *knight_check=10;
  }
  if (*(pbt+14)==10) 
  {      
    anz++;
    *knight_check=10;
  }
  if (*(pbt-10)==10) 
  {      
    anz++;
    *knight_check=10;
  }
  if (*(pbt-23)==10) 
  {      
    anz++;
    *knight_check=10;
  }
  if (*(pbt-25)==10) 
  {      
    anz++;
    *knight_check=10;
  }
  if (*(pbt-14)==10) 
  {      
    anz++;
    *knight_check=10;
  }
  
  if (*(pbt+11)==9) anz++;
  if (*(pbt+13)==9) anz++;
    
  return anz;
}


// Check auf Schwarzer Koenig im Schach und ob Springer-Schach
int anz_check_b(int *knight_check)
{
  unsigned char *pbt, *pbtx;
  int anz=0;

  *knight_check=0;

  pbt=cb.cc+flc[8][0];   /* Pointer auf Position des Koenigs */
  pbtx=pbt;

  if (fln[1]+fln[2]>0)
  {
    pbt=pbtx;
    do
    {
      pbt+=1;
    } while (*pbt == 0);
    if (*pbt<3) anz++;
    pbt=pbtx;
    do
    {
      pbt-=1;
    } while (*pbt == 0);
    if (*pbt<3) anz++;
    pbt=pbtx;
    do
    {
      pbt+=12;
    } while (*pbt == 0);
    if (*pbt<3) anz++;
    pbt=pbtx;
    do
    {
      pbt-=12;
    } while (*pbt == 0);
    if (*pbt<3) anz++;
  }

  if (fln[1]+fln[3]>0)
  {
    pbt=pbtx;
    do
    {
      pbt+=11;
    } while (*pbt == 0);
    if ((*pbt<4)&&(*pbt!=2)) anz++;
    pbt=pbtx;
    do
    {
      pbt+=13;
    } while (*pbt == 0);
    if ((*pbt<4)&&(*pbt!=2)) anz++;
    pbt=pbtx;
    do
    {
      pbt-=11;
    } while (*pbt == 0);
    if ((*pbt<4)&&(*pbt!=2)) anz++;
    pbt=pbtx;
    do
    {
      pbt-=13;
    } while (*pbt == 0);
    if ((*pbt<4)&&(*pbt!=2)) anz++;
  }

  pbt=pbtx;
  
  if (*(pbt+10)==4) 
  {  
    anz++;
    *knight_check=4;
  }
  if (*(pbt+23)==4) 
  {  
    anz++;
    *knight_check=4;
  }
  if (*(pbt+25)==4) 
  {  
    anz++;
    *knight_check=4;
  }
  if (*(pbt+14)==4) 
  {  
    anz++;
    *knight_check=4;
  }
  if (*(pbt-10)==4) 
  {  
    anz++;
    *knight_check=4;
  }
  if (*(pbt-23)==4) 
  {  
    anz++;
    *knight_check=4;
  }
  if (*(pbt-25)==4) 
  {  
    anz++;
    *knight_check=4;
  }
  if (*(pbt-14)==4) 
  {  
    anz++;
    *knight_check=4;
  }   
 
  if (*(pbt-11)==5) anz++;
  if (*(pbt-13)==5) anz++;
 
  return anz;
}


/* Stellung aus ASCII-Datei auslesen */
void read_datei(unsigned char *data, char *fnam)
{
  int c,i,k=0;
  FILE *fpt; 
  char ct[256];
  
  fpt=fopen(fnam,"rb");
  if (fpt==NULL)
  {
    sprintf(ct,"Open-Error bei %s\n",fnam);
    set_info(ct);
    exit(0);
  }
  
  for (i=0; i<1024; i++)
  {
    c=getc(fpt);
    if (c==EOF) break;
    data[k++]=c;
  }

  data[k]='\0';

  fclose(fpt);
}


/* Stellung in ASCII ausgeben */

void printboard()
{
  char *bptr;
  char row[12][16];
  int i,k,p;

  if (winflag==1) return;

  bptr=cb.cc+2;

  k=0;
  for (i=0;i<12;i++)
  {
    for (k=0;k<12;k++)
    {
      p=(*bptr++);
      if ((p<0)||(p>15))
        row[i][k]='?';
      else  
        row[i][k]=fig1[p][0];
    }
    row[i][12]='\0';
  }

  for (i=11; i>=0; i--)
    set_info(row[i]);
}

void printboard_check()
{
  char *bptr;
  char row[12][16];
  int i,k,p;

  bptr=sb2.cc+2;

  k=0;
  for (i=0;i<12;i++)
  {
    for (k=0;k<12;k++)
    {
      p=(*bptr++);
      row[i][k]='0' + p;   
    }
    row[i][12]='\0';
  }

  for (i=11; i>=0; i--)
    set_info(row[i]);
}


/**
*  Einen String wie die Kommandozeile (wie bei main - argc,argv)
*  in einzelne Argumente parsen
*/
int parse_zeile(char *cl, char **argv)
{
  int i,k=0,stat=0,anzpar=0,len=(int)strlen(cl);

  for (i=0; i<len; i++)
  {
    if (cl[i]=='\"')
    {
      stat=1-stat;
    }
    else
    {
      if ((stat==0)&&(cl[i]==' ')) cl[i]='\0';
      cl[k++]=cl[i];
    }
  }
  len=k;
  cl[len]='\0';

  if (cl[0]!='\0') argv[anzpar++]=cl;

  for (i=1; i<len; i++)
  {
    if ((cl[i]!='\0')&&(cl[i-1]=='\0')) argv[anzpar++]=cl+i;
  }
  return anzpar;
}


// 64 64-Bit Werte mit jeweils einem Bit gesetzt
void gen01()
{
  int i;
  __int64 sh = 1;

  for (i=0; i<64; i++)
  {
    bit64[i]=sh;
    sh = sh<<1;
  }
}

// Die Turm-Zug-Bits auf dem 64-Bit Brett für jedes Feld generieren
void gen02()
{
  int i,j,i1,j1;
  __int64 sh;
  
  for (j=0; j<8; j++)  // y, Hoehe (unten nach oben)
  {
    for (i=0; i<8; i++) // x, Breite (links nach rechts)
    {
      sh=0;

      for (i1=i-1; i1>=0; i1--)      
        sh = (sh | bit64[j*8+i1]); 

      for (i1=i+1; i1<8; i1++)      
        sh = (sh | bit64[j*8+i1]); 

      for (j1=j-1; j1>=0; j1--)
        sh = (sh | bit64[j1*8+i]);
      
      for (j1=j+1; j1<8; j1++)
        sh = (sh | bit64[j1*8+i]);
      
      rook64[j*8+i]=sh;
    }
  }
}

// Die Laeufer-Zug-Bits auf dem 64-Bit Brett für jedes Feld generieren
void gen03()
{
  int i,j,i1,j1;
  __int64 sh;
  
  for (j=0; j<8; j++)  // y, Hoehe (unten nach oben)
  {
    for (i=0; i<8; i++) // x, Breite (links nach rechts)
    {
      sh=0;

      i1=i; j1=j;
      while ((i1>=1)&&(j1>=1))
      {
        i1--;
        j1--;
        sh = (sh | bit64[j1*8+i1]);
      }

      i1=i; j1=j;
      while ((i1<7)&&(j1<7))
      {
        i1++;
        j1++;
        sh = (sh | bit64[j1*8+i1]);
      }

      i1=i; j1=j;
      while ((i1>=1)&&(j1<7))
      {
        i1--;
        j1++;
        sh = (sh | bit64[j1*8+i1]);
      }

      i1=i; j1=j;
      while ((i1<7)&&(j1>=1))
      {
        i1++;
        j1--;
        sh = (sh | bit64[j1*8+i1]);
      }

      bishop64[j*8+i]=sh;
    }
  }
}

// Die Bitboards auf das 12*12-Brett uebertragen
void gen04()
{
  int i,j;

  for (j=0; j<8; j++)
  {
    for (i=0; i<8; i++)
    {
      bit120[28+j*12+i] = bit64[j*8+i];
      rook120[28+j*12+i] = rook64[j*8+i];
      bishop120[28+j*12+i] = bishop64[j*8+i];
    }
  }
}

// SplitMix64 mit festem Startwert fuer reproduzierbare Zobrist-Schluessel.
static uint64 splitmix64_next(void)
{
  static uint64 state=UINT64_C(0xbf58476d1ce4e5b9);
  uint64 z=(state+=UINT64_C(0x9e3779b97f4a7c15));
  z=(z^(z>>30))*UINT64_C(0xbf58476d1ce4e5b9);
  z=(z^(z>>27))*UINT64_C(0x94d049bb133111eb);
  return z^(z>>31);
}

// Hash-Keys erzeugen
void init_zobrist()
{
  int i,k;

  for (i=0; i<16; i++)
  {
    for (k=0; k<128; k++)
    {
      zobrist[i][k]=splitmix64_next();
    }
  }
  for (i=0; i<4; i++)
    zobrist_rochade[i]=splitmix64_next();
};


// Bitboards fuer Turm und Laeufer Zuege generieren
void gen_bits()
{
  gen01();
  gen02();
  gen03();
  gen04();
}

int init_piece_list_from_board()
{
  int i,c,anz;
  unsigned char k;
  
  for (i=0; i<16; i++) fln[i]=0;
  anz=0;

  for (i=0; i<148; i++)
  {
    k=cb.cc[i];
    if ((k != 7)&&(k!=0))
    {
      c=fln[k];
      flc[k][c]=i;
      fln[k]++;
      anz++;

      if (fln[k]>10) return -40;            
    }
  }

  if (anz>32) return -41;
  if (fln[6] != 1) return -42;
  if (fln[8] != 1) return -43;
  if (fln[5]>8) return -44;
  if (fln[9]>8) return -45;

  if (fln[5]+fln[1]>9) return -46;
  if (fln[5]+fln[2]>10) return -46;
  if (fln[5]+fln[3]>10) return -46;
  if (fln[5]+fln[4]>10) return -46;

  if (fln[9]+fln[13]>9) return -47;
  if (fln[9]+fln[12]>10) return -47;
  if (fln[9]+fln[11]>10) return -47;
  if (fln[9]+fln[10]>10) return -47;

  return 0;
}

/* These helpers are used for FEN import/debugging, not for incremental search. */
static unsigned char board_ep_pawn(int gpos)
{
  unsigned char pawn=(gpos & 1) ? pos[gpos].lwm : pos[gpos].lsm;
  int previous_g=g, legal;
  if (pawn<=16) return 0;
  g=gpos;
  legal=generate_moves_ep();
  g=previous_g;
  return legal ? pawn : 0;
}

static uint64 board_hash_without_ep(int gpos)
{
  uint64 h=(gpos & 1) ? (uint64)-1 : 0;
  int i, square, piece;
  for (i=0; i<64; i++)
  {
    square=fen_board[i];
    piece=cb.cc[square];
    if (piece) h^=zobrist[piece][square];
  }
  if (pos[gpos].wr1 & 1) h^=zobrist_rochade[0];
  if (pos[gpos].wr1 & 2) h^=zobrist_rochade[1];
  if (pos[gpos].sr1 & 1) h^=zobrist_rochade[2];
  if (pos[gpos].sr1 & 2) h^=zobrist_rochade[3];
  return h;
}

// Hashkey ohne EP zum Weiterschalten; hashkey2 mit legal nutzbarem EP-Recht.
void set_board_hash(int gpos)
{
  pos[gpos].hashkey=board_hash_without_ep(gpos);
  pos[gpos].hash_doppel=board_ep_pawn(gpos);
  pos[gpos].hashkey2=pos[gpos].hashkey;
  if (pos[gpos].hash_doppel>16) pos[gpos].hashkey2^=zobrist[15][pos[gpos].hash_doppel];
}

uint64 get_board_hash(int gpos)
{
  uint64 h=board_hash_without_ep(gpos);
  unsigned char pawn=board_ep_pawn(gpos);
  if (pawn>16) h^=zobrist[15][pawn];
  return h;
}


// Array für das Parsen eines FEN-Strings anlegen
init_fen()
{
  int i,j,k;

  k=0;

  for (i=112; i>=28; i-=12)
  {
    for (j=0; j<8; j++) fen_board[k++]=i+j;
  }

  // PNBRQK pnbrqk
  for (i=0; i<256; i++) fen_piece[i]=0;
  
  fen_piece['Q']=1;
  fen_piece['R']=2;
  fen_piece['B']=3;
  fen_piece['N']=4;
  fen_piece['P']=5;
  fen_piece['K']=6;
  
  fen_piece['q']=13;
  fen_piece['r']=12;
  fen_piece['b']=11;
  fen_piece['n']=10;
  fen_piece['p']= 9;
  fen_piece['k']= 8;
}

void init_tables()
{
  int i,k,f1,f2;
  unsigned char c;
  
  init_zobrist();
  gen_bits();
  init_fen();

  flp=(char *) flc;

  for (i=0; i<148;i++)
  {
    sb.cc[i]=7;
    wb.cc[i]=7;
    sb2.cc[i]=7;
  }

  memset(knight_attack,0,288);
  knight_attack[144+10]=1;
  knight_attack[144+23]=1;
  knight_attack[144+25]=1;
  knight_attack[144+14]=1;
  knight_attack[144-10]=1;
  knight_attack[144-23]=1;
  knight_attack[144-25]=1;
  knight_attack[144-14]=1;  

  memset(pos,0,32*(sizeof(pos[0])));
  
  pos[0].hashkey=splitmix64_next();
  pos[1].hashkey=splitmix64_next();
  pos[2].hashkey=splitmix64_next();
  pos[3].hashkey=splitmix64_next();  
  
  /* Feld - Namen erzeugen e2,e4, etc. */
  for (i=0; i<148; i++) strcpy(pmv[i],"??");
  c='1';
  for (i=28; i<28+8*12; i+=12)
  {
    for (k=0; k<8;k++)
    {
      pmv[i+k][0]='a'+k;
      pmv[i+k][1]=c;
    }
    c++;
  }

  // Feld-Farben
  memset(board_color,0,148);
  f1=0;
  for (i=28; i<28+8*12; i+=12)
  {
    f2=f1;
    for (k=0; k<8;k++)
    {
      board_color[i+k]=f2;
      f2 = 1-f2;
    }
    f1=1-f1;
  }
}

/* Import only: no counter fields or additional work in the search positions. */
static int fen_counter_valid(const char *text, int nonzero)
{
  uint64 value=0, digit;
  if (*text=='\0') return 0;
  for (; *text; text++)
  {
    if (*text<'0' || *text>'9') return 0;
    digit=(unsigned)(*text-'0');
    if (value>((uint64)-1-digit)/10) return 0;
    value=value*10+digit;
  }
  return !nonzero || value!=0;
}

int fen_to_board(char *fen)
{
  char text[sizeof(akt_fen)], *args[6], *p;
  union board1 board, old_board;
  unsigned char old_list[16][12];
  int old_counts[16], count=0, rank=7, file=0, piece, n, bit, seen=0;
  int next_g, pawn=0, delta, erg, white_check, black_check, white_knight, black_knight;
  size_t len=0;
  unsigned char c;
  if (!fen) return -10;
  while (len<sizeof(text) && fen[len]) len++;
  if (len==sizeof(text)) return -10;
  memcpy(text,fen,len+1);
  p=strchr(text,';');
  if (p) *p='\0'; /* Preserve the historical FEN/EPD "...; id ..." input. */
  p=text;
  while (*p)
  {
    while (isspace((unsigned char)*p)) p++;
    if (!*p) break;
    if (count==6) return -10;
    args[count++]=p;
    while (*p && !isspace((unsigned char)*p)) p++;
    if (*p) *p++='\0';
  }
  if (count!=4 && count!=6) return -10;
  if (count==6 && (!fen_counter_valid(args[4],0) || !fen_counter_valid(args[5],1))) return -10;
  if (!strcmp(args[1],"w")) next_g=2;
  else if (!strcmp(args[1],"b")) next_g=3;
  else return -30;

  /* Parse into a temporary board: every rank must contain exactly eight squares. */
  memset(board.cc,7,sizeof(board.cc));
  for (p=args[0]; *p; p++)
  {
    c=(unsigned char)*p;
    if (c=='/')
    {
      if (file!=8 || rank==0) return -20;
      rank--; file=0;
    }
    else if (c>='1' && c<='8')
    {
      n=c-'0';
      if (file+n>8) return -20;
      while (n--) board.cc[28+12*rank+file++]=0;
    }
    else
    {
      piece=fen_piece[c];
      if (!piece || file>=8) return -20;
      if ((piece==5 || piece==9) && (rank==0 || rank==7)) return -21;
      board.cc[28+12*rank+file++]=(unsigned char)piece;
    }
  }
  if (rank!=0 || file!=8) return -25;
  if (strcmp(args[2],"-")) for (p=args[2]; *p; p++)
  {
    bit=*p=='K' ? 1 : *p=='Q' ? 2 : *p=='k' ? 4 : *p=='q' ? 8 : 0;
    if (!bit || (seen & bit)) return -50;
    seen|=bit;
  }
  if (strcmp(args[3],"-"))
  {
    p=args[3];
    if (strlen(p)!=2 || p[0]<'a' || p[0]>'h' || p[1]!=(next_g==3 ? '3' : '6')) return 60;
    pawn=(next_g==3 ? 64 : 76)+p[0]-'a';
    delta=next_g==3 ? -12 : 12;
    if (board.cc[pawn]!=(next_g==3 ? 5 : 9) || board.cc[pawn+delta] || board.cc[pawn+2*delta]) return 61;
  }

  /* List/legality validation may fail too; restore the complete previous board/lists. */
  old_board=cb;
  memcpy(old_counts,fln,sizeof(old_counts));
  memcpy(old_list,flc,sizeof(old_list));
  cb=board;
  erg=init_piece_list_from_board();
  if (!erg)
  {
    n=abs((int)flc[6][0]-(int)flc[8][0]);
    white_check=anz_check_w(&white_knight);
    black_check=anz_check_b(&black_knight);
    if (n==1 || n==11 || n==12 || n==13 ||
        (next_g==2 ? black_check!=0 || white_check>2 : white_check!=0 || black_check>2)) erg=70;
  }
  if (erg)
  {
    cb=old_board;
    memcpy(fln,old_counts,sizeof(old_counts));
    memcpy(flc,old_list,sizeof(old_list));
    return erg;
  }

  g=next_g;
  memset(&pos[g],0,sizeof(pos[g]));
  /* Stale rights are removed, as in JetChess; blocked/attacked paths keep their rights. */
  if (cb.cc[32]==6)
  {
    if (cb.cc[35]==2 && (seen & 1)) pos[g].wr1|=1;
    if (cb.cc[28]==2 && (seen & 2)) pos[g].wr1|=2;
  }
  if (cb.cc[116]==8)
  {
    if (cb.cc[119]==12 && (seen & 4)) pos[g].sr1|=1;
    if (cb.cc[112]==12 && (seen & 8)) pos[g].sr1|=2;
  }
  if (g==2) pos[g].lsm=(unsigned char)(white_knight ? 10 : pawn);
  else pos[g].lwm=(unsigned char)(black_knight ? 4 : pawn);
  lwm=pos[g].lwm;
  lsm=pos[g].lsm;
  mvtop=0;
  set_board_hash(g);
  memmove(akt_fen,fen,len+1);
  return 0;
}


void printmove1(int a, char b, char *ct)
{
  register char *pbt;
  char hit[2],nf[2],d,z;

  pbt=cb.cc;
  hit[1]='\0';
  nf[0]=' ';
  nf[1]='\0';

  z=a>>8;
  a=(a&255);
  d=*(pbt+b);
  hit[0]='-';
  if (d!=0) hit[0]='*';
  if (z>27)
  {
    nf[0]='e';
    hit[0]='*';
  }
  if ((z>0)&&(z<16))
    nf[0]=fig2[z][0];
  
  if (z<120)
  {
    sprintf(ct,"%s%s%s%s%s",fig2[*(pbt+*(flp+a))],pmv[*(flp+a)],hit,pmv[b],nf);
  }
  else
  {
    if ((z==120)||(z==122))
      sprintf(ct,"   0-0 ");
    else
      sprintf(ct," 0-0-0 ");
  }
}


void printmove2(int a, char b, char *ct)
{
  register char *pbt;
  char hit[2],nf[2],d,z;

  pbt=cb.cc;
  hit[1]='\0';
  nf[0]=' ';
  nf[1]='\0';

  z=a>>8;
  a=(a&255);
  d=*(pbt+b);
  hit[0]='-';
  if (d!=0) hit[0]='*';
  if ((z>0)&&(z<16))
    nf[0]=fig2[z][0];
  
  if (z<120)
  {
    sprintf(ct,"%s%-s%s",pmv[*(flp+a)],pmv[b],nf);
  }
  else
  {
    if (z==120)      
      sprintf(ct,"e1g1 ");
    if (z==121)      
      sprintf(ct,"e1c1 ");
    if (z==122)      
      sprintf(ct,"e8g8 ");
    if (z==123)      
      sprintf(ct,"e8c8 ");    
  }
}

void set_move_erg(int a, char b, __int64 erg, int style, int num)
{
  char ct[128],ct2[32];

  ct[0]='\0';
  if (style != 3) 
    sprintf(ct,"%3d  ",num);
  
  if (style==1)
    printmove1(a,b,ct2);
  else
    printmove2(a,b,ct2);

  strcat(ct,ct2);

  if (erg != -777)
  {  
    if (style != 3)    
      sprintf(ct2,"%10I64d",erg);
    else
      sprintf(ct2,"%I64d",erg);

    strcat(ct," ");
    strcat(ct,ct2);
  }

  set_text(ct);
}

void Fuellgrad()
{   
  __int64 count1, count2;
  double c1,c2;
  size_t i;
  char ct[128];

  count2=0;
  count1=0;
    
  for (i=0; i<hsize; i++)
  {
    if (hash0[i] != 0)
      count2++;        
    else 
      count1++;
  }

   c1 = (double)hsize;
   c2 = (double)count2;

   sprintf(ct,"Hash overflow: %I64d  %I64d - %g\n",count1,count2,c1/c2);
   set_info(ct);
   set_result("-");
}


// Figuren-Offsets im Bitmap mit den Schachfiguren ermitteln
int create_grafic_offsets()
{
  int i,j,k,x_off,y_off,erg=0;
  
  char cb1[64],cb2[64];

  j=0;
  for (i=28; i<28+8*12; i+=12)
  {
    for (k=0; k<8;k++)
    {
      cb1[j]=cb.cc[i+k];
      cb2[j]=board_color[i+k];
      j++;
    }
  }

  for (i=0; i<64; i++)
  {
    x_off=0;
    y_off=0;

    if (cb2[i]==0) x_off+=2; // schwarzes Feld
    if ((cb1[i]>6)&&(cb1[i]<14)) x_off+=1; // Schwarze Figur

    if ((cb1[i]==6)||(cb1[i]== 8)) y_off=1;
    if ((cb1[i]==1)||(cb1[i]==13)) y_off=2;
    if ((cb1[i]==2)||(cb1[i]==12)) y_off=3;
    if ((cb1[i]==3)||(cb1[i]==11)) y_off=4;
    if ((cb1[i]==4)||(cb1[i]==10)) y_off=5;
    if ((cb1[i]==5)||(cb1[i]== 9)) y_off=6;

    offset1_x[i]=x_off;
    offset1_y[i]=y_off;

    if ((x_off<0)||(x_off>3)) erg-=1;
    if ((y_off<0)||(x_off>6)) erg=-100;  
  }
  return erg;
}

void get_grafic_offset(int n, int *x, int *y)
{
  *x = offset1_x[n];
  *y = offset1_y[n];
}


/* MOVEPATH */
/***********************************************************************************/

// Anzahl der Stellungen nach n Zuegen ermitteln durch Hash-Eintrag aller Stellungen
// und anschliessendes Zählen
// Spezialvariante fuer mehrere Durchläufe mit partitioniertem Hashfile
void search_perft1d()
{
  int i,j,mp,anzmv,flag; 
  uint64 hpos,h;

  if (overflow_flag==1)
    return;
     
  lwm=pos[g].lwm;
  lsm=pos[g].lsm;
  
  level++;
  make_king();

  mp=mvtop;  
  anzmv = generate_moves();
  
  if (level>=maxlevel)
  {  
    for (i=0; i<anzmv; i++)
    {    
      make_move_h_hashonly(mv1[mp+i],mv2[mp+i]);    // g++
      
      h=pos[g].hashkey;
      if (pos[g].hash_doppel>16)
      {               
        g--;
        make_move(mv1[mp+i],mv2[mp+i]);
        lwm=pos[g].lwm;
        lsm=pos[g].lsm;
        if (generate_moves_ep()>0)
          h^=zobrist[15][pos[g].hash_doppel];
        undo_move();
        g++;
      }
      hpos=h & position_mask;       

      if ((hpos>=step0)&&(hpos<=step1))
      {
        hpos -= step0; // Bereich in den physikalisch kleineren Hashspeicher verschieben
        
        flag=0;
        for (j=0; j<32; j++)
        {        
          if (hash0[(size_t)hpos+j]==0)
          {
            hash0[(size_t)hpos+j]=h;
            flag=1;
            break;
          }
          else
          {
            if (hash0[(size_t)hpos+j]==h)
            {
              flag=1;
              break;
            }
          }
        }

        if (flag==0)        
          overflow_flag=1;
      }
      
      g--; // Nur den Zugindex der Hash-Vorschau zuruecksetzen.      
    }

    level--;
    mvtop=mp;
    return;
  }
  
  for (i=0; i<anzmv; i++)
  {    
    make_move_h(mv1[mp+i],mv2[mp+i]);
    search_perft1d();
    undo_move();
  }
 
  level--;
  mvtop=mp;
  return;
}


// Anzahl der Stellungen nach n Zuegen ermitteln durch Hash-Eintrag aller Stellungen
// und anschliessendes Zählen
// Spezialvariante fuer mehrere Durchläufe mit partitioniertem Hashfile
void search_perft1d_nor()
{
  int i,j,mp,anzmv,flag; 
  uint64 hpos,h;

  if (overflow_flag==1)
    return;
     
  lwm=pos[g].lwm;
  lsm=pos[g].lsm;
  
  level++;
  make_king();

  mp=mvtop;  
  anzmv = generate_moves();
  
  if (level>=maxlevel)
  {  
    for (i=0; i<anzmv; i++)
    {    
      make_move_h_nor_hashonly(mv1[mp+i],mv2[mp+i]);  // g++
      
      h=pos[g].hashkey;
      if (pos[g].hash_doppel>16)
      {               
        g--;
        make_move(mv1[mp+i],mv2[mp+i]);
        lwm=pos[g].lwm;
        lsm=pos[g].lsm;
        if (generate_moves_ep()>0)
          h^=zobrist[15][pos[g].hash_doppel];
        undo_move();
        g++;
      }
      hpos=h & position_mask;       

      if ((hpos>=step0)&&(hpos<=step1))
      {
        hpos -= step0; // Bereich in den physikalisch kleineren Hashspeicher verschieben
        
        flag=0;
        for (j=0; j<32; j++)
        {        
          if (hash0[(size_t)hpos+j]==0)
          {
            hash0[(size_t)hpos+j]=h;
            flag=1;
            break;
          }
          else
          {
            if (hash0[(size_t)hpos+j]==h)
            {
              flag=1;
              break;
            }
          }
        }

        if (flag==0)        
          overflow_flag=1;
      }
      
      g--; // Nur den Zugindex der Hash-Vorschau zuruecksetzen.      
    }

    level--;
    mvtop=mp;
    return;
  }
  
  for (i=0; i<anzmv; i++)
  {    
    make_move_h(mv1[mp+i],mv2[mp+i]);
    search_perft1d_nor();
    undo_move();
  }
 
  level--;
  mvtop=mp;
  return;
}

// Anzahl der Stellungen nach n Zuegen ermitteln durch Hash-Eintrag aller Stellungen
// und anschliessendes Zählen
void search_perft1c()
{
  int i,j,mp,anzmv,flag; 
  unsigned int hpos;
  uint64 h;

  if (overflow_flag==1)
    return;

  lwm=pos[g].lwm;
  lsm=pos[g].lsm;
  
  level++;
  make_king();

  mp=mvtop;  
  anzmv = generate_moves();
  
  if (level>=maxlevel)
  {  
    for (i=0; i<anzmv; i++)
    {
      make_move_h_hashonly(mv1[mp+i],mv2[mp+i]);  // g++      

      h=pos[g].hashkey;
      if (pos[g].hash_doppel>16)
      {               
        g--;
        make_move(mv1[mp+i],mv2[mp+i]);
        lwm=pos[g].lwm;
        lsm=pos[g].lsm;
        if (generate_moves_ep()>0)
          h^=zobrist[15][pos[g].hash_doppel];
        undo_move();
        g++;
      }
      hpos=(unsigned int)(h & hmask);       

      flag=0;
      for (j=0; j<32; j++)
      {        
        if (hash0[(size_t)hpos+j]==0)
        {
          hash0[(size_t)hpos+j]=h;
          flag=1;
          break;
        }
        else
        {
          if (hash0[(size_t)hpos+j]==h)
          {
            flag=1;
            break;
          }
        }
      }

      if (flag==0)
        overflow_flag=1;
      
      g--; // Nur den Zugindex der Hash-Vorschau zuruecksetzen.      
    }

    level--;
    mvtop=mp;
    return;
  }
  
  for (i=0; i<anzmv; i++)
  {    
    make_move_h(mv1[mp+i],mv2[mp+i]);
    search_perft1c();
    undo_move();
  }
 
  level--;
  mvtop=mp;
  return;
}


// Anzahl der Stellungen nach n Zuegen ermitteln durch Hash-Eintrag aller Stellungen
// und anschliessendes Zählen
void search_perft1c_nor()
{
  int i,j,mp,anzmv,flag; 
  unsigned int hpos;
  uint64 h;

  if (overflow_flag==1)
    return;

  lwm=pos[g].lwm;
  lsm=pos[g].lsm;
  
  level++;
  make_king();

  mp=mvtop;  
  anzmv = generate_moves();
  
  if (level>=maxlevel)
  {  
    for (i=0; i<anzmv; i++)
    {
      make_move_h_nor_hashonly(mv1[mp+i],mv2[mp+i]);  // g++

      h=pos[g].hashkey;
      if (pos[g].hash_doppel>16)
      {               
        g--;
        make_move(mv1[mp+i],mv2[mp+i]);
        lwm=pos[g].lwm;
        lsm=pos[g].lsm;
        if (generate_moves_ep()>0)
          h^=zobrist[15][pos[g].hash_doppel];
        undo_move();
        g++;
      }
      hpos=(unsigned int)(h & hmask);       

      flag=0;
      for (j=0; j<32; j++)
      {        
        if (hash0[(size_t)hpos+j]==0)
        {
          hash0[(size_t)hpos+j]=h;
          flag=1;
          break;
        }
        else
        {
          if (hash0[(size_t)hpos+j]==h)
          {
            flag=1;
            break;
          }
        }
      }

      if (flag==0)
        overflow_flag=1;
      
      g--; // Nur den Zugindex der Hash-Vorschau zuruecksetzen.      
    }

    level--;
    mvtop=mp;
    return;
  }
  
  for (i=0; i<anzmv; i++)
  {    
    make_move_h(mv1[mp+i],mv2[mp+i]);
    search_perft1c_nor();
    undo_move();
  }
 
  level--;
  mvtop=mp;
  return;
}


// Anzahl der Stellungen nach n Zuegen mit Hash-Eintragen fur die vorigen Knoten
// Wenn Knoten gefunden, dann nicht weitermachen, da die Eintraege im anderen Hash-Array
// fuer den maxlevel schon gemacht wurden.
uint64 search_perft3_Stellungen()
{
  int i,mp,anzmv,hpos,hpos2; 
  uint64 h,erg,move_count3=0;
  unsigned int *h1;
  unsigned short *h2;

  if ((overflow_flag==1)||(stopflag != 0)) return 0;

  lwm=pos[g].lwm;
  lsm=pos[g].lsm;
      
  // Fuer die Suche nach wiederholten Stellungen den vollstaendigen Hashcode merken
  h=pos[g].hashkey;
  if (pos[g].hash_doppel>16) h^=zobrist[15][pos[g].hash_doppel];
  hpos=(int)(h & hmask1); 

  h1 = (unsigned int *)&(h);
  h2 = (unsigned short *)&(h);  
  
  // Nur Treffer derselben Tiefe verwenden; deren Endstellungen sind bereits eingetragen.
  if ((hash1[hpos].hashcode1 == *(h1+1)) && (hash1[hpos].hashcode2 == *(h2+1)) && (hash1[hpos].level == g))
  {               
    return 0;   
  }
  else
  {
    if ((hash1[hpos+1].hashcode1 == *(h1+1)) && (hash1[hpos+1].hashcode2 == *(h2+1)) && (hash1[hpos+1].level == g))
    {                         
      return 0;
    }
    else
    {
      if ((hash1[hpos+2].hashcode1 == *(h1+1)) && (hash1[hpos+2].hashcode2 == *(h2+1)) && (hash1[hpos+2].level == g))
      {                               
        return 0;        
      }
    }
  }

  level++;
  make_king();
  mp=mvtop;  
  anzmv = generate_moves();
    
  for (i=0; i<anzmv; i++)
  {    
    if ((level==1)&&(anz_step<2)) // Nur fuer Fortschrittsanzeige      
      set_move_erg(mv1[mp+i],mv2[mp+i],-777,1,i+1);
    
    make_move_h(mv1[mp+i],mv2[mp+i]);                     

    if (level<maxlevel-1)       
      erg=search_perft3_Stellungen();    
    else
    {   
      if (anz_step<2) // Ohne Hash-File Partition
      {      
        if ((pos[g].wr1 == 0) &&( pos[g].sr1 == 0))        
          search_perft1c_nor();
        else
          search_perft1c();

      }
      else  // Mit Hash-File Partition
      {
        if ((pos[g].wr1 == 0) &&( pos[g].sr1 == 0))        
          search_perft1d_nor();
        else
          search_perft1d();
      }
    }
    undo_move();
  }
 
  if (hash1[hpos].hashcode1 != 0)
  {
    if (hash1[hpos+1].hashcode1 == 0)    
      hpos+=1;    
    else
    {
      if (hash1[hpos+2].hashcode1 == 0)     
        hpos+=2;      
      else
      {
         hpos2=hpos;                  
         if (hash1[hpos+1].level > hash1[hpos2].level) hpos2=hpos+1;
         if (hash1[hpos+2].level > hash1[hpos2].level) hpos2=hpos+2;                    
         hpos=hpos2;
      }
    }
  }
       
  hash1[hpos].level = g;
  hash1[hpos].hashcode1 = *(h1+1);
  hash1[hpos].hashcode2 = *(h2+1);

  level--;
  mvtop=mp;
  return move_count3;
}


/* PERFT */
/*************************************************************************************/

// Perft-Suche ohne hashen mit domove/undo move im letzten Knoten
uint64 search_perft1b()
{
  int i,mp,anzmv; 
  uint64 move_count3=0;

  lwm=pos[g].lwm;
  lsm=pos[g].lsm;
  
  level++;
  make_king();

  mp=mvtop;  
  anzmv = generate_moves();
  
  if (level>=maxlevel)
  {  
    for (i=0; i<anzmv; i++)
    {    
      make_move(mv1[mp+i],mv2[mp+i]);      
      undo_move();
    }

    level--;
    mvtop=mp;
    return (uint64)anzmv;
  }
  
  for (i=0; i<anzmv; i++)
  {    
    make_move(mv1[mp+i],mv2[mp+i]);
    move_count3+=search_perft1b();
    undo_move();
  }
 
  level--;
  mvtop=mp;
  return move_count3;
}


// Perft-Suche ohne hashen
uint64 search_perft1()
{
  int i,mp,anzmv; 
  uint64 erg,move_count3=0;

  lwm=pos[g].lwm;
  lsm=pos[g].lsm;
  
  level++;
  make_king();

  mp=mvtop;    
  
  if (level>=maxlevel)
  {  
    anzmv = generate_moves_perft();
    level--;
    mvtop=mp;
    return (uint64)anzmv;
  }

  anzmv = generate_moves();

  for (i=0; i<anzmv; i++)
  {    
    make_move_h(mv1[mp+i],mv2[mp+i]);
    erg=search_perft1();
    move_count3 += erg;
    undo_move();

    if (level==1)    
      set_move_erg(mv1[mp+i],mv2[mp+i],erg,1,i+1);
  }
 
  level--;
  mvtop=mp;
  return move_count3;
}


// Sondervariante fuer Beschleunigung der Matt-Suche
int search_perft_min_black(int mp0, int anzmv0, int *mm)
{
  int i,anzmv1,mp1;
  int min_nr=0, min_moves=254, anzg=0;

  for (i=0; i<anzmv0; i++)
  {    
    make_b_move(mv1[mp0+i],mv2[mp0+i]);
               
    lwm=pos[g].lwm;
    lsm=pos[g].lsm; 

    makewking();

    mp1=mvtop;  
     
    anzmv1 = generate_white_perft();
    if (anzmv1 < min_moves)
    {
      min_moves=anzmv1;
      min_nr=i;
    }
    anzg += anzmv1;
  
    mvtop=mp1;

    undo_b_move();
  }
      
  *mm = min_moves;
  return min_nr;  
}


// Sondervariante fuer Beschleunigung der Matt-Suche
// Mit Beruecksichtigung, ob die gezogenen Figur geschlagen werden kann
int search_perft_min_black2(int mp0, int anzmv0, int *mm)
{
  int i,j,anzmv1,mp1,count,pos2;
  int min_nr=0, min_moves=254, anzg=0;
 
  for (i=0; i<anzmv0; i++)
  {    
    pos2 = mv2[mp0+i];
    
    make_b_move(mv1[mp0+i],mv2[mp0+i]);
               
    lwm=pos[g].lwm;
    lsm=pos[g].lsm; 

    makewking();

    mp1=mvtop;  
     
    anzmv1 = generate_white();
    count=0;   
    if (anzmv1>0)
    {    
      for (j=0; j<anzmv1; j++)
      {
        if (mv2[mp1+j]==pos2) 
        {     
          count+=2;
          break;
        }
      }
    }
    anzmv1+=count;
    
    if (anzmv1 < min_moves)
    {
      min_moves=anzmv1;
      min_nr=i;
    }
    anzg += anzmv1;
  
    mvtop=mp1;

    undo_b_move();
  }
      
  *mm = min_moves;
  return min_nr;  
}


// Sondervariante fuer Beschleunigung der Matt-Suche
int search_perft_min_white(int mp0, int anzmv0, int *mm)
{
  int i,anzmv1,mp1;
  int min_nr=0, min_moves=254;

  for (i=0; i<anzmv0; i++)
  {    
    make_w_move(mv1[mp0+i],mv2[mp0+i]);
               
    lwm=pos[g].lwm;
    lsm=pos[g].lsm; 

    makesking();

    mp1=mvtop;  
     
    anzmv1 = generate_black_perft();

    if (anzmv1 < min_moves)
    {
      min_moves=anzmv1;
      min_nr=i;
    }
           
    mvtop=mp1;

    undo_w_move();
  }

  *mm = min_moves;
  return min_nr;  
}


// Sondervariante fuer Beschleunigung der Matt-Suche
int search_perft_min_white2(int mp0, int anzmv0, int *mm)
{
  int i,j,anzmv1,mp1,count,pos2;
  int min_nr=0, min_moves=254;

  for (i=0; i<anzmv0; i++)
  { 
    pos2 = mv2[mp0+i];

    make_w_move(mv1[mp0+i],mv2[mp0+i]);
               
    lwm=pos[g].lwm;
    lsm=pos[g].lsm; 

    makesking();

    mp1=mvtop;  
     
    anzmv1 = generate_black_perft();
    count=0;   
    if (anzmv1>0)
    {    
      for (j=0; j<anzmv1; j++)
      {
        if (mv2[mp1+j]==pos2) 
        {     
          count+=2;
          break;
        }
      }
    }
    anzmv1+=count;

    if (anzmv1 < min_moves)
    {
      min_moves=anzmv1;
      min_nr=i;
    }
           
    mvtop=mp1;

    undo_w_move();
  }

  *mm = min_moves;
  return min_nr;  
}


// Sondervariante fuer Beschleunigung der Matt-Suche
int search_perft_min_black_zw()
{
  int i,anzmv0,anzmv1,anzmv2,mp0,check,mp1,erg;
  int max_nr=0, max_moves=-254;

  lwm=pos[g].lwm;
  lsm=pos[g].lsm; 

  check=makewking();

  mp0 = mvtop;

  anzmv0=generate_white();

  if (anzmv0==0)
  {  
    mvtop=mp0;
    return 0;
  }

  for (i=0; i<anzmv0; i++)
  {    
    make_w_move(mv1[mp0+i],mv2[mp0+i]);
               
    lwm=pos[g].lwm;
    lsm=pos[g].lsm; 

    makesking();

    mp1=mvtop;
    anzmv1=generate_black();
     
    erg = search_perft_min_black2(mp1,anzmv1,&anzmv2);

    mvtop=mp1;
    undo_w_move();
    
    if (anzmv2 > max_moves)
    {
      max_moves=anzmv2;
      max_nr=i;
    
    }           
  }
  
  mvtop=mp0;
  return max_moves;  
}


// Sondervariante fuer Beschleunigung der Matt-Suche
int search_perft_min_black_start(int mp0, int anzmv0, int *mm)
{
  int i,anzmv1;
  int min_nr=0, min_moves=254;

  for (i=0; i<anzmv0; i++)
  {        
    
    make_b_move(mv1[mp0+i],mv2[mp0+i]);
                    
    anzmv1 = search_perft_min_black_zw();
    if (anzmv1 < min_moves)
    {
      min_moves=anzmv1;
      min_nr=i;
    }
      
    undo_b_move();
  }
      
  *mm = min_moves;
  return min_nr;  
}


// Den letzten Knoten ohne hashen
uint64 search_perft2_lastnodes(int mp0, int anzmv0)
{
  int i,anzmv1=0,mp1;

  for (i=0; i<anzmv0; i++)
  {    
     if (g&1)
       make_b_move(mv1[mp0+i],mv2[mp0+i]);
     else
       make_w_move(mv1[mp0+i],mv2[mp0+i]);           
 
     lwm=pos[g].lwm;
     lsm=pos[g].lsm; 

     if (g&1)
       makesking();
     else
       makewking();

     mp1=mvtop;  
     
     if (g&1)
       anzmv1 +=  generate_black_perft();
     else
       anzmv1 +=  generate_white_perft();     
           
     mvtop=mp1;

     if (g&1)
       undo_w_move();
     else
       undo_b_move();
   }

  return (uint64)anzmv1;  
}

/* Prefetch abschalten: PERFT_PREFETCH auf 0 setzen oder als 0 beim Build definieren. */
#ifndef PERFT_PREFETCH
#define PERFT_PREFETCH 1
#endif
#if PERFT_PREFETCH
#include <xmmintrin.h>
/* Hash-only nutzt den Folge-Slot; Brett und Figurenlisten bleiben unveraendert. */
static void prefetch_leaf(int a, int b, int nor)
{
  unsigned char saved_lwm=lwm, saved_lsm=lsm;
  uint64 key;
  if (nor) make_move_h_nor_hashonly(a,b);
  else make_move_h_hashonly(a,b);
  key=pos[g].hashkey;
  if (pos[g].hash_doppel>16) key^=zobrist[15][pos[g].hash_doppel];
  --g; lwm=saved_lwm; lsm=saved_lsm; /* Zugindex und beide Zugmerker restaurieren. */
  _mm_prefetch((const char *)&hash8[key & hmask], _MM_HINT_T0);
}
/* Zwei Zuege voraus laden; der regulaere Tabellenzugriff bleibt unveraendert. */
#define PREFETCH_LEAF(mp,i,n,nor) \
  do { if ((i)+2<(n)) prefetch_leaf(mv1[(mp)+(i)+2],mv2[(mp)+(i)+2],(nor)); } while (0)
#else
#define PREFETCH_LEAF(mp,i,n,nor) ((void)0)
#endif

// Den letzten Knoten mit hashen
short search_perft2_lastnodes_hash()
{
  int i,anzmv1=0,mp1,hpos,mp0,anzmv0;  
  uint64 h;
  short anzmv2;
  unsigned int *h1;
  unsigned short *h2;

  make_king();
  mp0=mvtop;  
  anzmv0 = generate_moves();

  for (i=0; i<anzmv0; i++)
  {
     PREFETCH_LEAF(mp0,i,anzmv0,0);
     if (g&1)
       make_b_move_h(mv1[mp0+i],mv2[mp0+i]);
     else
       make_w_move_h(mv1[mp0+i],mv2[mp0+i]);     
                    
     h=pos[g].hashkey;
     if (pos[g].hash_doppel>16) h^=zobrist[15][pos[g].hash_doppel];   
     hpos=(int)(h & hmask); 
     
     h1 = (unsigned int *)&(h);
     h2 = (unsigned short *)&(h);

     if ((hash8[hpos].hashcode1 == *(h1+1))&&(hash8[hpos].hashcode2 == *(h2+1)))
     {            
       anzmv1 += hash8[hpos].movecount;
 
       if (g&1)
         undo_w_move();
       else
         undo_b_move();

       continue;
     }                 
 
     lwm=pos[g].lwm;
     lsm=pos[g].lsm; 

     if (g&1)
       makesking();
     else
       makewking();

     mp1=mvtop;  
     
     if (g&1)
       anzmv2 =  generate_black_perft();
     else
       anzmv2 =  generate_white_perft();     
           
     anzmv1+=anzmv2;
     mvtop=mp1;

     hash8[hpos].hashcode1 = *(h1+1);
     hash8[hpos].hashcode2 = *(h2+1);
     hash8[hpos].movecount = anzmv2;

     if (g&1)
       undo_w_move();
     else
       undo_b_move();     
  }

  mvtop=mp0;
  return anzmv1;  
}

// Den letzten Knoten mit Hashing, keine Rochade mehr moeglich
short search_perft2_lastnodes_hash_nor()
{
  int i,anzmv1=0,mp1,hpos,mp0,anzmv0;  
  uint64 h;
  short anzmv2;
  unsigned int *h1;
  unsigned short *h2;

  make_king();
  mp0=mvtop;  
  anzmv0 = generate_moves();

  for (i=0; i<anzmv0; i++)
  {
     PREFETCH_LEAF(mp0,i,anzmv0,1);
     if (g&1)
       make_b_move_h_nor(mv1[mp0+i],mv2[mp0+i]);
     else
       make_w_move_h_nor(mv1[mp0+i],mv2[mp0+i]);     
                    
     h=pos[g].hashkey;
     if (pos[g].hash_doppel>16) h^=zobrist[15][pos[g].hash_doppel];   
     hpos=(int)(h & hmask); 
     
     h1 = (unsigned int *)&(h);
     h2 = (unsigned short *)&(h);

     if ((hash8[hpos].hashcode1 == *(h1+1))&&(hash8[hpos].hashcode2 == *(h2+1)))
     {            
       anzmv1 += hash8[hpos].movecount;
 
       if (g&1)
         undo_w_move();
       else
         undo_b_move();

       continue;
     }                 
 
     lwm=pos[g].lwm;
     lsm=pos[g].lsm; 

     if (g&1)
       makesking();
     else
       makewking();

     mp1=mvtop;  
     
     if (g&1)
       anzmv2 =  generate_black_perft();
     else
       anzmv2 =  generate_white_perft();     
           
     anzmv1+=anzmv2;
     mvtop=mp1;

     hash8[hpos].hashcode1 = *(h1+1);
     hash8[hpos].hashcode2 = *(h2+1);
     hash8[hpos].movecount = anzmv2;

     if (g&1)
       undo_w_move();
     else
       undo_b_move();     
  }

  mvtop=mp0;
  return anzmv1;  
}


// Suche mit 3 Hash-Tabellen vorletzter level
short search_perft3b()
{
  int hpos; 
  uint64 h;
  short move_count3=0;
  unsigned int *h1;
  unsigned short *h2;

  lwm=pos[g].lwm;
  lsm=pos[g].lsm;
    
  // Fuer die Suche nach wiederholten Stellungen den vollstaendigen Hashcode merken
  h=pos[g].hashkey;
  if (pos[g].hash_doppel>16) h^=zobrist[15][pos[g].hash_doppel];
  hpos=(int)(h & hmask); 
  
  h1 = (unsigned int *)&(h);
  h2 = (unsigned short *)&(h);  

  if ((hash7[hpos].hashcode1 == *(h1+1))&&(hash7[hpos].hashcode2 == *(h2+1)))
  {     
     return hash7[hpos].movecount;
  }
  else
  {
    if ((hash7[hpos+1].hashcode1 == *(h1+1))&&(hash7[hpos+1].hashcode2 == *(h2+1)))
    {
       return hash7[hpos+1].movecount;
    }
  }

  if ((pos[g].wr1 == 0) &&( pos[g].sr1 == 0))
    move_count3=search_perft2_lastnodes_hash_nor();   
  else
    move_count3=search_perft2_lastnodes_hash();
  
  if (hash7[hpos].hashcode1 != 0)
  {
     if (hash7[hpos+1].hashcode1 == 0) hpos++;
  }
  hash7[hpos].hashcode1 = *(h1+1);
  hash7[hpos].hashcode2 = *(h2+1);
  hash7[hpos].movecount = move_count3;
   
  return move_count3;
}


// Suche mit 3 Hash-Tabellen
uint64 search_perft3()
{
  int i,mp,anzmv,hpos,hpos2; 
  uint64 h,erg,move_count3=0;
  unsigned int *h1;
  unsigned short *h2;

  lwm=pos[g].lwm;
  lsm=pos[g].lsm;

  if (stopflag != 0) return 0;
      
  // Fuer die Suche nach wiederholten Stellungen den vollstaendigen Hashcode merken
  h=pos[g].hashkey;
  if (pos[g].hash_doppel>16) h^=zobrist[15][pos[g].hash_doppel];
  hpos=(int)(h & hmask); 

  h1 = (unsigned int *)&(h);
  h2 = (unsigned short *)&(h);  
  
  if ((hash3[hpos].hashcode1 == *(h1+1)) && (hash3[hpos].hashcode2 == *(h2+1)) && (hash3[hpos].level == g))
  {               
    return hash3[hpos].movecount;   
  }
  else
  {
    if ((hash3[hpos+1].hashcode1 == *(h1+1)) && (hash3[hpos+1].hashcode2 == *(h2+1)) && (hash3[hpos+1].level == g))
    {                         
      return hash3[hpos+1].movecount;      
    }
    else
    {
      if ((hash3[hpos+2].hashcode1 == *(h1+1)) && (hash3[hpos+2].hashcode2 == *(h2+1)) && (hash3[hpos+2].level == g))
      {                               
        return hash3[hpos+2].movecount;        
      }
    }
  }

  level++;
  make_king();
  mp=mvtop;  
  anzmv = generate_moves();
    
  for (i=0; i<anzmv; i++)
  {    
    make_move_h(mv1[mp+i],mv2[mp+i]);                     
    if (level<maxlevel-2)
      erg=search_perft3();    
    else
      erg=search_perft3b();
    move_count3+=erg;
    undo_move();

    if (level==1)
    {
      if (stopflag==0)                
        set_move_erg(mv1[mp+i],mv2[mp+i],erg,1,i+1);
    }
  }
 
  if (hash3[hpos].hashcode1 != 0)
  {
    if (hash3[hpos+1].hashcode1 == 0)    
      hpos+=1;    
    else
    {
      if (hash3[hpos+2].hashcode1 == 0)     
        hpos+=2;      
      else
      {
         hpos2=hpos;                  
         if (hash3[hpos+1].movecount < hash3[hpos2].movecount) hpos2=hpos+1;
         if (hash3[hpos+2].movecount < hash3[hpos2].movecount) hpos2=hpos+2;                    
         hpos=hpos2;
      }
    }
  }
       
  hash3[hpos].level = g;
  hash3[hpos].hashcode1 = *(h1+1);
  hash3[hpos].hashcode2 = *(h2+1);
  hash3[hpos].movecount = move_count3;  

  level--;
  mvtop=mp;
  return move_count3;
}


/* MATE SEARCH */
/******************************************************************************/

int km1[64];
unsigned char km2[64];

int findkiller_w(int a, int b)
{
  unsigned char* mvx;
  unsigned int x; 
  unsigned char y;
  int i,u,v;
  int pos1=a,pos2;

  mvx=mv2;
  x=km1[g];
  y=km2[g];
  
  for (i=a; i<b; i++)
  {
    if ((y==mvx[i])&&(x==mv1[i]))
    {
      u=mv1[a];
      v=mvx[a];
      mv1[a]=x;
      mvx[a]=y;
      mv1[i]=u;
      mvx[i]=v;
      pos1++;
      break;
    }
  }    

  if (pos1>=b) return 0; 
  
  // alle Schlagzuege nach vorne
  pos2=pos1;
  for (i=pos2; i<b; i++)
  {
    if (cb.cc[mvx[i]]!=0)
    {
      u=mv1[pos1];
      v=mvx[pos1];
      mv1[pos1]=mv1[i];
      mvx[pos1]=mv2[i];
      mv1[i]=u;
      mvx[i]=v;
      pos1++;
    }
  }
          
  return 0;
}

int findkiller_s(int a, int b)
{
  unsigned char* mvx;
  unsigned int x; 
  unsigned char y;
  int i,u,v;
  int pos1=a,pos2;

  mvx=mv2;
  x=km1[g];
  y=km2[g];
  
  for (i=a; i<b; i++)
  {
    if ((y==mvx[i])&&(x==mv1[i]))
    {
      u=mv1[a];
      v=mvx[a];
      mv1[a]=x;
      mvx[a]=y;
      mv1[i]=u;
      mvx[i]=v;
      pos1++;
      break;
    }
  }    

  if (pos1>=b) return 0; 
  
  // alle Schlagzuege nach vorne
  pos2=pos1;
  for (i=pos2; i<b; i++)
  {
    if (cb.cc[mvx[i]]!=0)
    {
      u=mv1[pos1];
      v=mvx[pos1];
      mv1[pos1]=mv1[i];
      mvx[pos1]=mv2[i];
      mv1[i]=u;
      mvx[i]=v;
      pos1++;
    }
  }
          
  return 0;
}

// Zuerst den Zug mit den wenigsten Gegenzuegen, danach den Killerzug versuchen.
int findkiller_s2(int a, int b, int m)
{
  unsigned char* mvx;
  unsigned int x; 
  unsigned char y;
  int i,u,v;
  int pos1=a,pos2;

  mvx=mv2;
 
  u=mv1[a];
  v=mvx[a];
  mv1[a]=mv1[m];
  mvx[a]=mvx[m];
  mv1[m]=u;
  mvx[m]=v;
  pos1=a+1;
    
  if ((mv1[a] != km1[g]) && (mv2[a] != km2[g]))
  {
    x=km1[g];
    y=km2[g];

    for (i=pos1+1; i<b; i++)
    {
      if ((y==mvx[i])&&(x==mv1[i]))
      {
        u=mv1[pos1];
        v=mvx[pos1];
        mv1[pos1]=x;
        mvx[pos1]=y;
        mv1[i]=u;
        mvx[i]=v;
        pos1++;
        break;
      }
    }
  }

  if (pos1>=b) return 0; 
  
  // alle Schlagzuege nach vorne
  pos2=pos1;
  for (i=pos2; i<b; i++)
  {
    if (cb.cc[mvx[i]]!=0)
    {
      u=mv1[pos1];
      v=mvx[pos1];
      mv1[pos1]=mv1[i];
      mvx[pos1]=mv2[i];
      mv1[i]=u;
      mvx[i]=v;
      pos1++;
    }
  }
          
  return 0;
}

// Anders als bei der schwarzen Zugsortierung wird der Killerzug zuerst versucht.
int findkiller_w2(int a, int b, int m)
{
  unsigned char* mvx;
  unsigned int x; 
  unsigned char y;
  int i,u,v,lastpos;
  int pos1=a,pos2;

  mvx=mv2;
  
  x=km1[g];
  y=km2[g];

  for (i=pos1+1; i<b; i++)
  {
    if ((y==mvx[i])&&(x==mv1[i]))
    {
      u=mv1[pos1];
      v=mvx[pos1];
      mv1[pos1]=x;
      mvx[pos1]=y;
      mv1[i]=u;
      mvx[i]=v;
      pos1++;
      break;
    }
  }
  
  if (pos1>=b) return 0;
   
  u=mv1[pos1];
  v=mvx[pos1];
  mv1[pos1]=mv1[m];
  mvx[pos1]=mvx[m];
  mv1[m]=u;
  mvx[m]=v;
  pos1++;
 
  if (pos1>=b) return 0; 
  
  // Schlagen der zuletzt gezogenen Figur nach vorne
  lastpos=pos[g-1].b;
  for (i=pos1+1; i<b; i++)
  {
    if (lastpos==mvx[i])
    {
      u=mv1[pos1];
      v=mvx[pos1];
      mv1[pos1]=mv1[i];
      mvx[pos1]=mv2[i];
      mv1[i]=u;
      mvx[i]=v;
      pos1++;
      break;
    }
  }

  // alle anderen Schlagzuege nach vorne
  pos2=pos1;
  for (i=pos2; i<b; i++)
  {
    if (cb.cc[mvx[i]]!=0)
    {
      u=mv1[pos1];
      v=mvx[pos1];
      mv1[pos1]=mv1[i];
      mvx[pos1]=mv2[i];
      mv1[i]=u;
      mvx[i]=v;
      pos1++;
    }
  }
          
  return 0;
}


int gensmove()
{
  int mp,anzmv;

  lwm=pos[g].lwm;
  lsm=pos[g].lsm;
  
  // abgekuerzter Code wegen Aufruf nur in letzem Level
  if (makesking()==0) return 0; 
  mp=mvtop;
  anzmv = generate_black_one_move();
  mvtop=mp;
  if (anzmv>0) return 0;
  return 100-level-1+maxlevel;
}

int gensmove_h();

int genwmove_h()
{
  int i,erg,max; 
  int mp,check,anzmv;
  unsigned int hpos;
  uint64 h;
  unsigned int *h1;
  char move_nr=-1;
  int dist1,dist2;

  if (stopflag != 0) return 0;
    
  h=pos[g].hashkey;
  if (pos[g].hash_doppel>16) h^=zobrist[15][pos[g].hash_doppel];   
  if (g>5)
  {
    if (h == pos[g-4].hashkey2) return 0;
  }
  pos[g].hashkey2=h;
  hpos=(unsigned int)(h & hmask);
  h1 = (unsigned int *)&(h);

  lwm=pos[g].lwm;
  lsm=pos[g].lsm;
  level++;

  if (hash2[hpos].hashcode1 == *(h1+1))
  {                               
    if (hash2[hpos].level==maxlevel-level)        
    {    
      level--;
      return hash2[hpos].score;        
    }        
    if (hash2[hpos].level>maxlevel-level)        
    {    
      if (hash2[hpos].score<=0) // Trotz mehr Zuegen kein Matt
      {      
        level--;
        return hash2[hpos].score;        
      }          
      else
      {
        // Tiefendifferenz mit dem Anteil der Mattbewertung oberhalb von 100 vergleichen.        
        dist1=hash2[hpos].level-(maxlevel-level);
        dist2=hash2[hpos].score-100;

        if (dist2>=dist1)
        {
           level--;
           return hash2[hpos].score; 
        }
      }            
    }  
    if (hash2[hpos].level<maxlevel-level)        
    {    
      if (hash2[hpos].score>0) // Trotz weniger Zuegen Matt
      {      
        level--;
        return hash2[hpos].score;        
      }
    }  
    move_nr = hash2[hpos].move_nr;
  }

  check=makewking();  
  
  if ((level>=maxlevel)&&(check==0))
  {
    level--;
    return 0;
  }

  mp=mvtop; 

  if (level>=maxlevel-1)    
  {
    makesking_check();  
    anzmv = generate_white_check();
  }
  else
  {
    anzmv = generate_white();
  }
  
  if (anzmv==0)
  {
    level--;
    mvtop=mp;
    if (check!=0) return -100+level+1-maxlevel;    
    return 0;
  }
  if (level>=maxlevel)
  {
    level--;
    mvtop=mp;
    return 0;
  }

  max=0; // Nicht -100 wegen Filterung der Zuege, es koennen 0 Zuege werden
       
  if (level<maxlevel-1)
  {
    if (level<maxlevel-3)
    {
      if (move_nr<0)            
        move_nr=search_perft_min_white2(mp,anzmv,&erg);
//      if (erg==0) // Matt in 1 gefunden, kleine Optimierung
//      {
//        max=100;
//        anzmv=0; // Ueberspringen
//      }
//      else      
        findkiller_w2(mp,mvtop,mp+move_nr);
    }
    else
    {        
      findkiller_w(mp,mvtop);
    }
        
    for (i=0; i<anzmv; i++)
    {          
      make_w_move_h(mv1[mp+i],mv2[mp+i]);
      erg=gensmove_h();    
      undo_w_move();

      if (erg>max) max=erg;    
 
      if (max>0)
      {
        km1[g]=mv1[mp+i];
        km2[g]=mv2[mp+i];
        break;
      }
    }
  }
  else
  {    
    for (i=0; i<anzmv; i++)
    {          
      if (check_w_move(mv1[mp+i],mv2[mp+i]) == 0) continue;
      
      make_w_move(mv1[mp+i],mv2[mp+i]);                     
      erg=gensmove();                       
      undo_w_move();

      if (erg>max) max=erg;    
  
      if (max>0)
      {
        km1[g]=mv1[mp+i];
        km2[g]=mv2[mp+i];
        break;
      }
    }
  }

  if (hash2[hpos].hashcode1 != 0)
  {
    if (hash2[hpos].level<=maxlevel-level)
    {    
      hash2[hpos].hashcode1=*(h1+1);
      hash2[hpos].score=max;  
      hash2[hpos].level=maxlevel-level;
      hash2[hpos].move_nr=move_nr;
    }
  }
  else
  {
    hash2[hpos].hashcode1=*(h1+1);
    hash2[hpos].score=max;  
    hash2[hpos].level=maxlevel-level;
    hash2[hpos].move_nr=move_nr;
  }

  level--;
  mvtop=mp;
  return max;
}


int gensmove_h()
{
  int i,erg,min; 
  int mp,check,anzmv;
  unsigned int hpos;
  uint64 h;
  unsigned int *h1;
  int move_nr=-1;

  if (stopflag != 0) return 0;

  h=pos[g].hashkey;
  if (pos[g].hash_doppel>16) h^=zobrist[15][pos[g].hash_doppel];   
  if (g>5)
  {
    if (h == pos[g-4].hashkey2) return 0;
  }
  pos[g].hashkey2=h;
  hpos=(unsigned int)(h & hmask);
  h1 = (unsigned int *)&(h);

  lwm=pos[g].lwm;
  lsm=pos[g].lsm;
  level++;

  if (hash2[hpos].hashcode1 == *(h1+1))
  {                  
    if (hash2[hpos].level==maxlevel-level)        
    {    
      level--;
      return hash2[hpos].score;        
    }    
    if (hash2[hpos].level>maxlevel-level)        
    {    
      if (hash2[hpos].score<=0) // Trotz mehr Zuegen kein Matt
      {      
        level--;
        return hash2[hpos].score;        
      }
    }  
    if (hash2[hpos].level<maxlevel-level)        
    {    
      if (hash2[hpos].score>0) // Trotz weniger Zuegen Matt
      {      
        level--;
        return hash2[hpos].score;        
      }
    }  
    move_nr = hash2[hpos].move_nr;
  }

  check=makesking();  

  if ((level>=maxlevel)&&(check==0))
  {
    level--;
    return 0;
  }

  mp=mvtop;
  anzmv = generate_black();

  if (anzmv==0)
  {
    level--;
    mvtop=mp;
    if (check!=0) return 100-level-1+maxlevel;
    return 0;
  }
  if (level>=maxlevel)
  {
    level--;
    mvtop=mp;
    return 0;
  }

  min=120;

  if (level<maxlevel-3)
  {
    if (move_nr<0)                       
         move_nr=search_perft_min_black2(mp,anzmv,&erg);
      
//  if (erg==0) // Matt in 1 gefunden, kleine Optimierung
//  {
//    min=-100;
//    anzmv=0; // Ueberspringen
//  }
//  else           
    findkiller_s2(mp,mvtop,mp+move_nr);  
  }
  else
  {
    if (level<maxlevel-1) findkiller_s(mp,mvtop);
  }
  for (i=0; i<anzmv; i++)
  {
    make_b_move_h(mv1[mp+i],mv2[mp+i]);    
    erg=genwmove_h();       
    undo_b_move(); 

    if (erg<min) min=erg;

    if (min<=0)
    {
      km1[g]=mv1[mp+i];
      km2[g]=mv2[mp+i];
      break;
    }
  }

  if (hash2[hpos].hashcode1 != 0)
  {
    if (hash2[hpos].level<=maxlevel-level)
    {    
      hash2[hpos].hashcode1=*(h1+1);
      hash2[hpos].score=min;  
      hash2[hpos].level=maxlevel-level;
      hash2[hpos].move_nr=move_nr;
    }
  }
  else
  {
    hash2[hpos].hashcode1=*(h1+1);
    hash2[hpos].score=min;  
    hash2[hpos].level=maxlevel-level;
    hash2[hpos].move_nr=move_nr;
  }

  level--;
  mvtop=mp;
  return min;
}


int genwmove_start()
{
  int i,erg,max; 
  int mp,check,anzmv,sol_count=0,mate_level;
  char ct[128],ct2[128],erg_temp[128];

  lwm=pos[g].lwm;
  lsm=pos[g].lsm;
  level++;
  
  check=makewking();  
  
  mp=mvtop;  
  anzmv = generate_white();
  
  if (anzmv==0)
  {
    level--;
    mvtop=mp;
    if (check!=0) return -100;    
    return 0;
  }

  max=-127;
    
  for (i=0; i<anzmv; i++)
  {
    make_w_move_h(mv1[mp+i],mv2[mp+i]);           
    erg=gensmove_h();    
    undo_w_move();

    if (stopflag != 0) return 0;

    if (erg>max) max=erg;
   
    sprintf(ct,"%3d  ",i+1);
    printmove1(mv1[i],mv2[i],ct2);
    strcat(ct,ct2);
            
    if (erg>0)
    {
      mate_level=0;
      if (erg>100)
        mate_level = (erg-100)/2;
      
      sol_count++;
      strcpy(erg_temp,"Solution:  ");
      strcat(erg_temp,ct2);      
      sprintf(ct2," (mate in %d)",maxlevel/2 - mate_level);
      strcat(erg_temp,ct2);
    }
    
    if (erg==0)    
      sprintf(ct2,"  %d",erg);    

    if (erg<0)
      strcpy(ct2,"  m");

    if (erg>0)
      strcpy(ct2,"  Mate");                
 
    strcat(ct,ct2);
    set_text(ct);
  }

  if (sol_count==0)
  {
    sprintf(ct,"No mate in %d moves",maxlevel/2);
    set_result(ct);
  }
  if (sol_count==1)
    set_result(erg_temp);  
  if (sol_count>1)  
    set_result("Multiple Solutions");
     
  level--;
  mvtop=mp;
  return max;
}

// Wie genwmove_start, ohne Text-Ausgabe
int genwmove_start_bench()
{
  int i,erg,max; 
  int mp,check,anzmv,sol_count=0;

  lwm=pos[g].lwm;
  lsm=pos[g].lsm;
  level++;
  
  check=makewking();  
  
  mp=mvtop;  
  anzmv = generate_white();
  
  if (anzmv==0)
  {
    level--;
    mvtop=mp;
    if (check!=0) return -100;    
    return 0;
  }

  max=-127;
    
  for (i=0; i<anzmv; i++)
  {
    make_w_move_h(mv1[mp+i],mv2[mp+i]);           
    erg=gensmove_h();    
    undo_w_move();

    if (erg>max) max=erg;       
  }
     
  level--;
  mvtop=mp;
  return max;
}



/* Start - Routinen */
/******************************************************************************/

// Optional in mehreren Durchläufen und simuliertem grossen Hashfile
__int64 Anzahl_Stellungen(int max_level, char *fen_txt, int steps)
{
  int j,erg;
  size_t i;
  __int64 count2,count1=0;
  char ct[128];
   
  overflow_flag=0;

  anz_step=steps;
  step0=0;
  step1=hstep-1;
  position_mask=hstep*(uint64)anz_step-1;

  erg = fen_to_board(fen_txt);
  if (erg != 0)
  {
    sprintf(ct,"Fen-Error: %d",erg);
    set_info(ct);
    set_result("");
    return -1;
  }

  printboard();  

  maxlevel=max_level;
  level=0;
  mvtop=0;

  if (anz_step>1)
  {
    start_mytime(1);
    for (j=0; j<anz_step; j++)
    {
      hash2_zero();

      if (maxlevel > 1)        
        search_perft3_Stellungen();
      else
        search_perft1d();

      if (overflow_flag==1)
        break;   

      count2=0;
      
      for (i=0; i<hsize; i++)
      {
        if (hash0[i] != 0)
          count2++;        
      }
      if (stopflag==0)
      {      
        sprintf(ct,"Count %2d:  %12I64d",j,count2);
        set_text(ct);
      }
      count1 += count2;

      step0 += hstep;
      step1 += hstep;
    }
    stop_mytime(1);
  }
  else
  {
    start_mytime(1);
    
    if (maxlevel > 1)        
      search_perft3_Stellungen();
    else
      search_perft1c();
    
    stop_mytime(1);
    
    count1=0;

    if (overflow_flag==0)
    {    
      for (i=0; i<hsize; i++)
      {
        if (hash0[i] != 0)
          count1++;        
      }
    }
  }

  if (overflow_flag==1)
  {
    Fuellgrad();
    hash2_zero();
    return 0;
  }

  sprintf(ct,"Positions: %12I64d",count1);
  set_text("");
  set_text(ct);
   
  sprintf(ct,"%s   (Positions after %d half moves)",dot_print(count1),max_level);
  set_result(ct);

  sprintf(ct,"Time: %s",get_mytime2(1));
  set_info(ct);

  hash2_zero();
  return get_mytime1(1);;
}

// Mit 3 Hash-Files und Routinen fuer den letzten und vorletzten Level
__int64 Anzahl_Pfade(int max_level, char *fen_txt)
{
  int erg;
  uint64 move_count_all;
  char ct[128];
   
  erg = fen_to_board(fen_txt);
  if (erg != 0)
  {
    sprintf(ct,"Fen-Error: %d",erg);
    set_info(ct);
    set_result("");
    return -1;
  }

  printboard();
 
  maxlevel = max_level;  
  level=0;
  mvtop=0;

  start_mytime(0);
  if (maxlevel < 4)
    move_count_all = search_perft1();    
  else
    move_count_all = search_perft3();
  stop_mytime(0);

  sprintf(ct,"Total:   %14I64d",move_count_all);
  set_text("");
  set_text(ct);
  
  sprintf(ct,"%s   (Move pathes after %d half moves)",dot_print(move_count_all),max_level);  
  set_result(ct);

  sprintf(ct,"Time: %s",get_mytime2(0));
  set_info(ct);

  if (maxlevel>3)
    hash1_zero();

  return get_mytime1(0);
}


// Matt-Suche
__int64 Anzahl_Matts(int max_level, char *fen_txt)
{
  int erg;
  char ct[128];
  
  erg = fen_to_board(fen_txt);
  if (erg != 0)
  {
    sprintf(ct,"Fen-Error: %d",erg);
    set_info(ct);
    set_result("");
    return -1;
  }

  if (g != 2)
  {
    set_result("-");
    set_info("Side to move must be white!");
    return -1;
  }

  printboard();  

  maxlevel=max_level;
  level=0;
  mvtop=0;

  start_mytime(1);
  genwmove_start();
/*  
  for (i=4; i<=max_level; i+=2)
  {
      level=0;
      mvtop=0;
      maxlevel=i;
      if (i==max_level) start_mytime(1);           
      genwmove_start();
      if (i==max_level) stop_mytime(1); 
  }
*/  
  stop_mytime(1);
    
  sprintf(ct,"Time: %s",get_mytime2(1));
  set_info(ct);

  hash3_zero();
  return get_mytime1(1);;
}


// Matt-Suche
__int64 Anzahl_Bench()
{
  __int64 time_add=0;
  int i,erg;
  char ct[128];

  hash3_zero();
    
  for (i=0; i<5; i++)
  {  
    erg = fen_to_board(bench_fens[i]);

    printboard();
 
    maxlevel=bench_level[i];
    level=0;
    mvtop=0;

    start_mytime(1);
    genwmove_start_bench(); 
    stop_mytime(1);
    
    time_add = time_add + get_mytime1(1);
    
    sprintf(ct,"Time %d:  %s",i+1,get_mytime2(1));
    set_text(ct);
    hash3_zero();
  }

  sprintf(ct,"Benchmark Time: %s",get_mytime3(time_add));
  set_result(ct);
  
  return time_add;
}

__int64 Anzahl_DoAll(int max_level, char *fen_txt, int steps)
{  
  __int64 count=0;
  
  reset_anzcounter();
  
  set_result("Running...");

  if ((opmode<1)||(opmode>4))
  {
    set_info("Opmode Error");    
    return 0;
  }

  // Preserve a cancellation requested immediately after the worker was started.
  {
    LONG previous;
    do {
      previous = get_stopflag();
      if (previous == 3) return 0;
    } while (InterlockedCompareExchange(&stopflag, 0, previous) != previous);
  }

  if (opmode==1)
    count=Anzahl_Pfade(max_level,fen_txt);

  if (opmode==2)
    count=Anzahl_Stellungen(max_level,fen_txt,steps);

  if (opmode==3)
    count=Anzahl_Matts(max_level*2,fen_txt); // Wegen Matt in 5 = maxlevel 10

  if (opmode==4)
    count=Anzahl_Bench();

  if (stopflag==3)
     reset_anzcounter();

  if (stopflag==0)  
    set_stopflag(2); // Wegen Ausgabe-Stop in Windows

  return count;
}



/* Historical console variant; disabled in the library build. */
#if 0
int main(int argc, char **argv)
{
  char fen_txt[2048];
  int max_level,hsize,steps;

  if (argc<4)
  {
    printf("Usage: mgen board-datei <M,P,S,B> <depth> <hash_size> <anz_steps>\n\n");
    printf("M = Move Path Count\n");
    printf("P = Position Count\n");
    printf("S = Mate Search\n");
    printf("B = Benchmark\n\n");
    printf("anz_steps: Hashfile partitioning for Position Count\n");
    exit(0);
  }

  set_winflag(0);
  init_tables();
  init_mytime();

  read_datei(fen_txt,argv[1]);
  
  max_level = atoi(argv[3]);
  steps=1;
  hsize=-1;

  switch (argv[2][0])
  {
    case 'M':
      set_opmode(1);
      break;
    case 'P':
      set_opmode(2);
      break;
    case 'S':
      set_opmode(3);
      break;
    case 'B':
      set_opmode(4);
      break;
    default:
      printf("Error opmode\n");
      exit(0);
  }

  if (argc>4)
  {
    hsize = atoi(argv[4]);
    hash_set(hsize);
  }

  if (argc>5)
  {
    steps = atoi(argv[5]);
  }

  Anzahl_DoAll(max_level,fen_txt,steps);
}

#endif /* historical console variant */
