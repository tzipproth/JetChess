/* Zuege - Generierung  */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "move.h"


extern struct POS1
{
  uint64 hashkey,hashkey2;
  int a,b,p1;
  unsigned char d,z,*pbt2;
  unsigned char hash_doppel;
  unsigned char lwm,lsm,wch1,sch1,wr1,sr1,fesselung;
} pos[72];

extern int g;

static int kingmove[9]={1,-1,11,12,13,-13,-12,-11};
static int knigmove[9]={10,23,25,14,-14,-25,-23,-10};
static int quenmove[9]={11,13,-11,-13,12,1,-1,-12};
static int kingmove_s[9]={1,-1,-13,-12,-11,11,12,13};
static int knigmove_s[9]={-14,-25,-23,-10,10,23,25,14};
static int quenmove_s[9]={-13,-11,13,11,-12,1,-1,12};
extern unsigned int mv1[10240];
extern unsigned char mv2[10240];
extern int mvtop;
extern int fln[16];
extern unsigned char flc[16][12];
extern unsigned char *flp;
extern unsigned char lwm,lsm;

extern char knight_attack[144+144];

extern __int64 bit64[64],rook64[64],bishop64[64];
extern __int64 bit120[120],rook120[120],bishop120[120];
extern __int64 rook_bits,bishop_bits;

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

void mv1wpawn(void);
void mv1wknig(void);
void mv1wbish(void);
void mv1wrook(void);
void mv1wquen(void);
void mv1wking(void);
void mv2wpawn(void);
void mv2wknig(void);
void mv2wbish(void);
void mv2wrook(void);
void mv2wquen(void);

void mv1spawn(void);
void mv1sknig(void);
void mv1sbish(void);
void mv1srook(void);
void mv1squen(void);
void mv1sking(void);
void mv2spawn(void);
void mv2sknig(void);
void mv2sbish(void);
void mv2srook(void);
void mv2squen(void);

void mv1wep(void);
void mv2wep(void);
void mv1sep(void);
void mv2sep(void);



// Check auf Weisser Koenig im Schach fuer Rochade
int in_check_wr(int kpos)
{
  unsigned char *pbt,*pbtx;

  pbt=cb.cc+kpos;   /* Pointer auf Weg-Position des Koenigs */
  pbtx=pbt;
  
  do
  {
    pbt+=12;
  } while (*pbt == 0);
  if (*pbt>11) return 1;

  if ((bishop_bits & bit120[kpos])!=0)
  {  
    pbt=pbtx;
    do
    {
      pbt+=11;
    } while (*pbt == 0);
    if ((*pbt>10)&&(*pbt!=12)) return 1;
    pbt=pbtx;
    do
    {
      pbt+=13;
    } while (*pbt == 0);
    if ((*pbt>10)&&(*pbt!=12)) return 1;
  }
  
  pbt=pbtx;
      
  if (*(pbt+10)==10) return 1;
  if (*(pbt+23)==10) return 1;
  if (*(pbt+25)==10) return 1;
  if (*(pbt+14)==10) return 1;
  
  if (*(pbt+11)==9) return 1;
  if (*(pbt+13)==9) return 1;
    
  return 0;
}


// Check auf Schwarzer Koenig im Schach fuer Rochade
int in_check_br(int kpos)
{
  unsigned char *pbt,*pbtx;

  pbt=cb.cc+kpos;   /* Pointer auf Weg-Position des Koenigs */
  pbtx=pbt;

  do
  {
    pbt-=12;
  } while (*pbt == 0);
  if (*pbt<3) return 1;

  if ((bishop_bits & bit120[kpos])!=0)
  {    
    pbt=pbtx;
    do
    {
      pbt-=11;
    } while (*pbt == 0);
    if ((*pbt<4)&&(*pbt!=2)) return 1;
    pbt=pbtx;
    do
    {
      pbt-=13;
    } while (*pbt == 0);
    if ((*pbt<4)&&(*pbt!=2)) return 1;
  }

  pbt=pbtx;
  
  if (*(pbt-10)==4) return 1;
  if (*(pbt-23)==4) return 1;
  if (*(pbt-25)==4) return 1;
  if (*(pbt-14)==4) return 1;    
  
  if (*(pbt-11)==5) return 1;
  if (*(pbt-13)==5) return 1;
 
  return 0;
}


void mv1wking()
{
  register char *pbt;
  register char *pbtx;
  register int mt;
  signed char *pbt2;
  int i,f;

  mt=mvtop;

  pbt=wb.cc+flc[8][0];
  pbt2=wb.cc;
  *(pbt+1)=7;
  *(pbt-1)=7;
  *(pbt+12)=7;
  *(pbt-12)=7;
  *(pbt+11)=7;
  *(pbt-11)=7;
  *(pbt+13)=7;
  *(pbt-13)=7;

  for (i=0; i<fln[10]; i++)
  {
    pbt=wb.cc+flc[10][i];
    *(pbt+10)=7;
    *(pbt+23)=7;
    *(pbt+25)=7;
    *(pbt+14)=7;
    *(pbt-10)=7;
    *(pbt-23)=7;
    *(pbt-25)=7;
    *(pbt-14)=7;
  }

  *(cb.cc+flc[6][0])=0;
  for (i=0; i<8; i++)
  {
    f=flc[6][0]+kingmove[i];
    pbtx=cb.cc+f;
    if ((*pbtx>0)&&(*pbtx<6)) continue;
    if (*(pbt2+f)==7) continue;

    if (*(pbtx+11)==9) continue;
    if (*(pbtx+13)==9) continue;
    
    if ((rook_bits & bit120[f])!=0)
    {
      pbt=pbtx;
      do
      {
        pbt+=1;
      } while (*pbt == 0);
      if (*pbt>11) continue;
      pbt=pbtx;
      do
      {
        pbt-=1;
      } while (*pbt == 0);
      if (*pbt>11) continue;
      pbt=pbtx;
      do
      {
        pbt+=12;
      } while (*pbt == 0);
      if (*pbt>11) continue;
      pbt=pbtx;
      do
      {
        pbt-=12;
      } while (*pbt == 0);
      if (*pbt>11) continue;
    }

    if ((bishop_bits & bit120[f])!=0)
    {
      pbt=pbtx;
      do
      {
        pbt+=11;
      } while (*pbt == 0);
      if ((*pbt>10)&&(*pbt!=12)) continue;
      pbt=pbtx;
      do
      {
        pbt+=13;
      } while (*pbt == 0);
      if ((*pbt>10)&&(*pbt!=12)) continue;
      pbt=pbtx;
      do
      {
        pbt-=11;
      } while (*pbt == 0);
      if ((*pbt>10)&&(*pbt!=12)) continue;
      pbt=pbtx;
      do
      {
        pbt-=13;
      } while (*pbt == 0);
      if ((*pbt>10)&&(*pbt!=12)) continue;
    }

    mv1[mt]=72;
    mv2[mt]=f;
    mt++;
  }
  *(cb.cc+flc[6][0])=6;
  mvtop=mt;
}



void mv1sking()
{
  register char *pbt;
  register char *pbtx;
  register int mt;
  signed char *pbt2;
  int i,f;

  mt=mvtop;

  pbt=sb.cc+flc[6][0];  // Position weisser Koenig
  pbt2=sb.cc;
  *(pbt+1)=7;
  *(pbt-1)=7;
  *(pbt+12)=7;
  *(pbt-12)=7;
  *(pbt+11)=7;
  *(pbt-11)=7;
  *(pbt+13)=7;
  *(pbt-13)=7;

  for (i=0; i<fln[4]; i++)
  {
    pbt=sb.cc+flc[4][i];
    *(pbt+10)=7;
    *(pbt+23)=7;
    *(pbt+25)=7;
    *(pbt+14)=7;
    *(pbt-10)=7;
    *(pbt-23)=7;
    *(pbt-25)=7;
    *(pbt-14)=7;
  }
  
  *(cb.cc+flc[8][0])=0; // Die Zuege duerfen nicht an der alten Koenigsposition stoppen
  for (i=0; i<8; i++)
  {
    f=flc[8][0]+kingmove_s[i];
    pbtx=cb.cc+f;
    if (*pbtx>6) continue;
    if (*(pbt2+f)==7) continue;

    if (*(pbtx-11)==5) continue;
    if (*(pbtx-13)==5) continue;

    if ((rook_bits & bit120[f])!=0)
    {
      pbt=pbtx;
      do
      {
        pbt+=1;
      } while (*pbt == 0);
      if (*pbt<3) continue;
      pbt=pbtx;
      do
      {
        pbt-=1;
      } while (*pbt == 0);
      if (*pbt<3) continue;
      pbt=pbtx;
      do
      {
        pbt+=12;
      } while (*pbt == 0);
      if (*pbt<3) continue;
      pbt=pbtx;
      do
      {
        pbt-=12;
      } while (*pbt == 0);
      if (*pbt<3) continue;
    }

    if ((bishop_bits & bit120[f])!=0)
    {
      pbt=pbtx;
      do
      {
        pbt+=11;
      } while (*pbt == 0);
      if ((*pbt<4)&&(*pbt!=2)) continue;
      pbt=pbtx;
      do
      {
        pbt+=13;
      } while (*pbt == 0);
      if ((*pbt<4)&&(*pbt!=2)) continue;
      pbt=pbtx;
      do
      {
        pbt-=11;
      } while (*pbt == 0);
      if ((*pbt<4)&&(*pbt!=2)) continue;
      pbt=pbtx;
      do
      {
        pbt-=13;
      } while (*pbt == 0);
      if ((*pbt<4)&&(*pbt!=2)) continue;
    }

    mv1[mt]=96;
    mv2[mt]=f;
    mt++;
  }
  *(cb.cc+flc[8][0])=8;
  mvtop=mt;
}


// Fuer Matt Suche letzter Level
void mv1sking_one_move()
{
  register char *pbt;
  register char *pbtx;
  register int mt;
  signed char *pbt2;
  int i,f;

  mt=mvtop;

  pbt=sb.cc+flc[6][0];  // Position weisser Koenig
  pbt2=sb.cc;
  *(pbt+1)=7;
  *(pbt-1)=7;
  *(pbt+12)=7;
  *(pbt-12)=7;
  *(pbt+11)=7;
  *(pbt-11)=7;
  *(pbt+13)=7;
  *(pbt-13)=7;

  for (i=0; i<fln[4]; i++)
  {
    pbt=sb.cc+flc[4][i];
    *(pbt+10)=7;
    *(pbt+23)=7;
    *(pbt+25)=7;
    *(pbt+14)=7;
    *(pbt-10)=7;
    *(pbt-23)=7;
    *(pbt-25)=7;
    *(pbt-14)=7;
  }
  
  *(cb.cc+flc[8][0])=0; // Die Zuege duerfen nicht an der alten Koenigsposition stoppen
  for (i=0; i<8; i++)
  {
    f=flc[8][0]+kingmove_s[i];
    pbtx=cb.cc+f;
    if (*pbtx>6) continue;
    if (*(pbt2+f)==7) continue;

    if (*(pbtx-11)==5) continue;
    if (*(pbtx-13)==5) continue;

    if ((rook_bits & bit120[f])!=0)
    {
      pbt=pbtx;
      do
      {
        pbt+=1;
      } while (*pbt == 0);
      if (*pbt<3) continue;
      pbt=pbtx;
      do
      {
        pbt-=1;
      } while (*pbt == 0);
      if (*pbt<3) continue;
      pbt=pbtx;
      do
      {
        pbt+=12;
      } while (*pbt == 0);
      if (*pbt<3) continue;
      pbt=pbtx;
      do
      {
        pbt-=12;
      } while (*pbt == 0);
      if (*pbt<3) continue;
    }

    if ((bishop_bits & bit120[f])!=0)
    {
      pbt=pbtx;
      do
      {
        pbt+=11;
      } while (*pbt == 0);
      if ((*pbt<4)&&(*pbt!=2)) continue;
      pbt=pbtx;
      do
      {
        pbt+=13;
      } while (*pbt == 0);
      if ((*pbt<4)&&(*pbt!=2)) continue;
      pbt=pbtx;
      do
      {
        pbt-=11;
      } while (*pbt == 0);
      if ((*pbt<4)&&(*pbt!=2)) continue;
      pbt=pbtx;
      do
      {
        pbt-=13;
      } while (*pbt == 0);
      if ((*pbt<4)&&(*pbt!=2)) continue;
    }

    mv1[mt]=96;
    mv2[mt]=f;
    mt++;
    break;
  }
  *(cb.cc+flc[8][0])=8;
  mvtop=mt;
}


// Rochade weisser Koenig
void mv1wrochade()
{
  int i,p;
    
  if (flc[6][0]!=32) return;
  
  if ((cb.ii[8]==0x02000006)&&((pos[g].wr1 & 1) > 0))
  {
    if (in_check_wr(33)==0)
    {
      if (in_check_wr(34)==0)
      {
        if (flc[8][0]!=46)
        {        
          p=0;
          for (i=0; i<fln[2]; i++)  // Den Turm suchen
          {
            if (flc[2][i]==35)
            {
              p=24+i;
              break;
            }
          }
          if (p>0) // Der Turm sollte immer gefunden werden
          {
            mv1[mvtop]=p+(120<<8);  // Turmzug eintragen
            mv2[mvtop]=33;
            mvtop++;         
          }
        }
      }
    }
  }

  if ((cb.ii[7]==0x02)&&((pos[g].wr1 & 2) > 0))
  {
    if (in_check_wr(31)==0)
    {
      if (in_check_wr(30)==0)
      {
        if ((flc[8][0]!=41)&&(flc[8][0]!=42))
        {       
          p=0;
          for (i=0; i<fln[2]; i++)  // Den Turm suchen
          {
            if (flc[2][i]==28)
            {
              p=24+i;
              break;
            }
          }
          if (p>0)
          {
            mv1[mvtop]=p+(121<<8);
            mv2[mvtop]=31;           // Turmzug eintragen
            mvtop++;         
          }
        }
      }
    }
  }
}


// Rochade schwarzer Koenig
void mv1srochade()
{
  int i,p;
    
  if (flc[8][0]!=116) return;

  if ((cb.ii[29]==0x0C000008)&&((pos[g].sr1 & 1) > 0))
  {
    if (in_check_br(117)==0)
    {
      if (in_check_br(118)==0)
      {
        if (flc[6][0]!=106)
        {        
          p=0;
          for (i=0; i<fln[12]; i++)  // Den Turm suchen
          {
            if (flc[12][i]==119)
            {
              p=144+i;
              break;
            }
          }
          if (p>0)
          {
            mv1[mvtop]=p+(122<<8);   // Turmzug eintragen
            mv2[mvtop]=117;
            mvtop++;         
          }
        }
      }
    }
  }

  if ((cb.ii[28]==0x0C)&&((pos[g].sr1 & 2) > 0))
  {
    if (in_check_br(115)==0)
    {
      if (in_check_br(114)==0)
      {
        if ((flc[6][0]!=101)&&(flc[6][0]!=102))
        {           
          p=0;
          for (i=0; i<fln[12]; i++)  // Den Turm suchen
          {
            if (flc[12][i]==112)
            {
              p=144+i;
              break;
            }
          }
          if (p>0)
          {
            mv1[mvtop]=p+(123<<8);   // Turmzug eintragen
            mv2[mvtop]=115;
            mvtop++;         
          }
        }
      }
    }
  }
}


/* Fuer EP zaehlt die Koenigssicherheit nach dem Entfernen beider Bauern. */
static int ep_king_attacked(int black)
{
  int king=flc[black ? 8 : 6][0], i, step, sq, piece;
  int knight=black ? 4 : 10, pawn=black ? 5 : 9, delta=black ? -12 : 12;
  for (i=0; i<8; i++)
    if (cb.cc[king+knigmove[i]]==knight) return 1;
  if (cb.cc[king+delta-1]==pawn || cb.cc[king+delta+1]==pawn) return 1;
  for (i=0; i<8; i++)
  {
    int diagonal;
    step=kingmove[i];
    if (cb.cc[king+step]==(black ? 6 : 8)) return 1;
    sq=king+step;
    while (cb.cc[sq]==0) sq+=step;
    piece=cb.cc[sq];
    diagonal=(step==11 || step==-11 || step==13 || step==-13);
    if (piece==(black ? 1 : 13) ||
        piece==(diagonal ? (black ? 3 : 11) : (black ? 2 : 12))) return 1;
  }
  return 0;
}

static int ep_legal(int black, int from, int pawn)
{
  int us=black ? 9 : 5, them=black ? 5 : 9, to=pawn+(black ? -12 : 12);
  int legal;
  if (pawn<28 || pawn>119 || (from!=pawn-1 && from!=pawn+1) ||
      cb.cc[from]!=us || cb.cc[pawn]!=them || cb.cc[to]!=0) return 0;
  cb.cc[from]=cb.cc[pawn]=0;
  cb.cc[to]=(unsigned char)us;
  legal=!ep_king_attacked(black);
  cb.cc[from]=(unsigned char)us;
  cb.cc[pawn]=(unsigned char)them;
  cb.cc[to]=0;
  return legal;
}

void mv1wpawn()
{
  register char *pbt;
  register int mt;
  char * fpt;
  int j,k,f,p;

  mt=mvtop;
  fpt=flc[5];
  for (j=0; j<fln[5];j++)
  {
    k=*fpt++;
    p=j+60;

    f=*(wb.cc+k);
    pbt=cb.cc+k;

    if (k>99)
    {
      p+=256;

      if (f!=0)  // Auf der Umwandlungsreihe sind bei f=32 oder f=+/-12 hier keine Zuege moeglich. 
      {
        if (f==11)  /* Kann in dieser Richtung ziehen */
        {
          mv1[mt]=p;
          mv2[mt]=k+11;
          mt++;
          mv1[mt]=p+768;
          mv2[mt]=k+11;
          mt++;
          mv1[mt]=p+256;
          mv2[mt]=k+11;
          mt++;
          mv1[mt]=p+512;
          mv2[mt]=k+11;
          mt++;
          continue;
        }
        if (f==13)
        {
          mv1[mt]=p;
          mv2[mt]=k+13;
          mt++;
          mv1[mt]=p+768;
          mv2[mt]=k+13;
          mt++;
          mv1[mt]=p+256;
          mv2[mt]=k+13;
          mt++;
          mv1[mt]=p+512;
          mv2[mt]=k+13;
          mt++;
          continue;
        }
        continue;
      }

      if (*(pbt+12)==0)
      {
        mv1[mt]=p;
        mv2[mt]=k+12;
        mt++;
        mv1[mt]=p+768;
        mv2[mt]=k+12;
        mt++;
        mv1[mt]=p+256;
        mv2[mt]=k+12;
        mt++;
        mv1[mt]=p+512;
        mv2[mt]=k+12;
        mt++;
      }
      if (*(pbt+11)>8)
      {
        mv1[mt]=p;
        mv2[mt]=k+11;
        mt++;
        mv1[mt]=p+768;
        mv2[mt]=k+11;
        mt++;
        mv1[mt]=p+256;
        mv2[mt]=k+11;
        mt++;
        mv1[mt]=p+512;
        mv2[mt]=k+11;
        mt++;
      }
      if (*(pbt+13)>8)
      {
        mv1[mt]=p;
        mv2[mt]=k+13;
        mt++;
        mv1[mt]=p+768;
        mv2[mt]=k+13;
        mt++;
        mv1[mt]=p+256;
        mv2[mt]=k+13;
        mt++;
        mv1[mt]=p+512;
        mv2[mt]=k+13;
        mt++;
      }
      continue;
    }

    if (f!=0)
    {
      if ((f==12)||(f==-12))
      {
        if (*(pbt+12)==0)
        {
          mv1[mt]=p;
          mv2[mt]=k+12;
          mt++;
          if (k<48)
          {
            if (*(pbt+24)==0)
            {
              mv1[mt]=p+4096;
              mv2[mt]=k+24;
              mt++;
            }
          }
        }
        continue;
      }

      if (f==11)  /* Kann in dieser Richtung ziehen */
      {
        mv1[mt]=p;
        mv2[mt]=k+11;
        mt++;
        continue;
      }

      if (f==13)
      {
        mv1[mt]=p;
        mv2[mt]=k+13;
        mt++;
        continue;
      }
      continue;
    }

    if (*(pbt+12)==0)
    {
      mv1[mt]=p;
      mv2[mt]=k+12;
      mt++;
      if (k<48)
      {
        if (*(pbt+24)==0)
        {
          mv1[mt]=p+4096;
          mv2[mt]=k+24;
          mt++;
        }
      }
    }
    if (*(pbt+11)>8)
    {
      mv1[mt]=p;
      mv2[mt]=k+11;
      mt++;
    }
    if (*(pbt+13)>8)
    {
      mv1[mt]=p;
      mv2[mt]=k+13;
      mt++;
    }
  }
  mvtop=mt;
}


void mv1wknig()
{
  register char *pbt;
  register int mt;
  register int f;
  char *fpt;
  int i,j,k,p;

  mt=mvtop;
  fpt=flc[4];
  for (j=0; j<fln[4]; j++)
  {
    k=*fpt++;
    p=j+48;

    if (*(wb.cc+k)!=0) continue;  /* gefesselt, bei Springer immer total */

    pbt=cb.cc+k;

    for (i=0; i<8; i++)
    {
      f=knigmove[i];
      if ((*(pbt+f)==0)||(*(pbt+f)>8))
      {
        mv1[mt]=p;
        mv2[mt]=k+f;
        mt++;
      }
    }
  }
  mvtop=mt;
}


void mv1wbish()
{
  register char *pbt;
  register int mt;
  register int i;
  char *fpt;
  int j,k,f,p,l;

  mt=mvtop;
  fpt=flc[3];
  for (j=0; j<fln[3]; j++)
  {
    k=*fpt++;
    p=j+36;

    f=*(wb.cc+k);
    
    if (f!=0)  /* Kann nur in Fesselrichtung ziehen */
    {
      if (f==32) continue;  /* vollkommen gefesselt */
      pbt=cb.cc;      
      i=k;
      do
      {
        i+=f;
        mv1[mt]=p;
        mv2[mt]=i;
        mt++;
      } while (*(pbt+i)==0);
      i=k-f;
      while (*(pbt+i)==0)
      {
        mv1[mt]=p;
        mv2[mt]=i;
        mt++;
        i-=f;
      }
      continue;
    }

    pbt=cb.cc;

    for (l=0; l<4; l++)
    {
      f=quenmove[l];

      i=k+f;
      while (*(pbt+i)==0)
      {
        mv1[mt]=p;
        mv2[mt]=i;
        mt++;
        i+=f;
      }
      if (*(pbt+i)>8)
      {
        mv1[mt]=p;
        mv2[mt]=i;
        mt++;
      }
    }
  }
  mvtop=mt;
}


void mv1wrook()
{
  register char *pbt;
  register int mt;
  register int i;
  char *fpt;
  int j,k,f,p,l;

  mt=mvtop;
  fpt=flc[2];
  for (j=0; j<fln[2]; j++)
  {
    k=*fpt++;
    p=j+24;

    f=*(wb.cc+k);    

    if (f!=0)  /* Kann nur in Fesselrichtung ziehen */
    {
      if (f==32) continue;  /* vollkommen gefesselt */
      pbt=cb.cc;
      i=k;
      do
      {
        i+=f;
        mv1[mt]=p;
        mv2[mt]=i;
        mt++;
      } while (*(pbt+i)==0);
      i=k-f;
      while (*(pbt+i)==0)
      {
        mv1[mt]=p;
        mv2[mt]=i;
        mt++;
        i-=f;
      }
      continue;
    }

    pbt=cb.cc;

    for (l=4; l<8; l++)
    {
      f=quenmove[l];

      i=k+f;
      while (*(pbt+i)==0)
      {
        mv1[mt]=p;
        mv2[mt]=i;
        mt++;
        i+=f;
      }
      if (*(pbt+i)>8)
      {
        mv1[mt]=p;
        mv2[mt]=i;
        mt++;
      }
    }
  }
  mvtop=mt;
}


void mv1wquen()
{
  register char *pbt;
  register int mt;
  register int i;
  char *fpt;
  int j,k,f,p,l;

  mt=mvtop;
  fpt=flc[1];
  for (j=0; j<fln[1]; j++)
  {
    k=*fpt++;
    p=j+12;

    f=*(wb.cc+k);    

    if (f!=0)  /* Kann nur in Fesselrichtung ziehen */
    {
      pbt=cb.cc;
      i=k;
      do
      {
        i+=f;
        mv1[mt]=p;
        mv2[mt]=i;
        mt++;
      } while (*(pbt+i)==0);
      i=k-f;
      while (*(pbt+i)==0)
      {
        mv1[mt]=p;
        mv2[mt]=i;
        mt++;
        i-=f;
      }
      continue;
    }

    pbt=cb.cc;

    for (l=0; l<8; l++)
    {
      f=quenmove[l];

      i=k+f;
      while (*(pbt+i)==0)
      {
        mv1[mt]=p;
        mv2[mt]=i;
        mt++;
        i+=f;
      }
      if (*(pbt+i)>8)
      {
        mv1[mt]=p;
        mv2[mt]=i;
        mt++;
      }
    }
  }
  mvtop=mt;
}


void mv2wpawn()
{
  register char *pbt;
  register char *pbt2;
  register int mt;
  char *fpt;
  int j,k,p;

  mt=mvtop;
  fpt=flc[5];
  for (j=0; j<fln[5]; j++)
  {
    k=*fpt++;
    p=j+60;

    if (*(wb.cc+k)!=0) continue;  /* vollkommen gefesselt */

    pbt=cb.cc+k;
    pbt2=wb.cc+k;

    if (k>99)
    {
      p+=256;

      if ((*(pbt2+12)==2)&&(*(pbt+12)==0))
      {
        mv1[mt]=p;
        mv2[mt]=k+12;
        mt++;
        mv1[mt]=p+768;
        mv2[mt]=k+12;
        mt++;
        mv1[mt]=p+256;
        mv2[mt]=k+12;
        mt++;
        mv1[mt]=p+512;
        mv2[mt]=k+12;
        mt++;

      }
      if ((*(pbt2+11)==2)&&(*(pbt+11)>8))
      {
        mv1[mt]=p;
        mv2[mt]=k+11;
        mt++;
        mv1[mt]=p+768;
        mv2[mt]=k+11;
        mt++;
        mv1[mt]=p+256;
        mv2[mt]=k+11;
        mt++;
        mv1[mt]=p+512;
        mv2[mt]=k+11;
        mt++;

      }
      if ((*(pbt2+13)==2)&&(*(pbt+13)>8))
      {
        mv1[mt]=p;
        mv2[mt]=k+13;
        mt++;
        mv1[mt]=p+768;
        mv2[mt]=k+13;
        mt++;
        mv1[mt]=p+256;
        mv2[mt]=k+13;
        mt++;
        mv1[mt]=p+512;
        mv2[mt]=k+13;
        mt++;

      }
      continue;
    }

    if (*(pbt+12)==0)
    {
      if (*(pbt2+12)==2)
      {
        mv1[mt]=p;
        mv2[mt]=k+12;
        mt++;
      }
      if (k<48)
      {
        if ((*(pbt2+24)==2)&&(*(pbt+24)==0))
        {
          mv1[mt]=p+4096;
          mv2[mt]=k+24;
          mt++;
        }
      }
    }
    if ((*(pbt2+11)==2)&&(*(pbt+11)>8))
    {
      mv1[mt]=p;
      mv2[mt]=k+11;
      mt++;
    }
    if ((*(pbt2+13)==2)&&(*(pbt+13)>8))
    {
      mv1[mt]=p;
      mv2[mt]=k+13;
      mt++;
    }
  }
  mvtop=mt;
}


void mv2wknig()
{
  register char *pbt2;
  register int mt;
  char *fpt;
  int i,j,k,f,p;

  mt=mvtop;
  fpt=flc[4];
  for (j=0; j<fln[4]; j++)
  {
    k=*fpt++;
    p=j+48;

    if (*(wb.cc+k)!=0) continue;  /* gefesselt, bei Springer immer total */

    pbt2=wb.cc+k;

    for (i=0; i<8; i++)
    {
      f=knigmove[i];
      if (*(pbt2+f)==2)
      {
        mv1[mt]=p;
        mv2[mt]=k+f;
        mt++;
      }
    }
  }
  mvtop=mt;
}


void mv2wbish()
{
  register char *pbt;
  register char *pbt2;
  register int mt;
  register int i;
  char *fpt;
  int j,k,f,p,l;

  mt=mvtop;
  fpt=flc[3];
  for (j=0; j<fln[3]; j++)
  {
    k=*fpt++;
    p=j+36;

    if (*(wb.cc+k)!=0) continue;  /* vollkommen gefesselt */

    pbt=cb.cc;
    pbt2=wb.cc;

    for (l=0; l<4; l++)
    {
      f=quenmove[l];

      i=k;
      do {
        i+=f;
        if (*(pbt2+i)==2)
        {
          mv1[mt]=p;
          mv2[mt]=i;
          mt++;
          break;
        }        
      } while (*(pbt+i)==0);
    }
  }
  mvtop=mt;
}


void mv2wrook()
{
  register char *pbt;
  register char *pbt2;
  register int mt;
  register int i;
  char *fpt;
  int j,k,f,p,l;

  mt=mvtop;
  fpt=flc[2];
  for (j=0; j<fln[2]; j++)
  {
    k=*fpt++;
    p=j+24;

    if (*(wb.cc+k)!=0) continue;  /* vollkommen gefesselt */

    pbt=cb.cc;
    pbt2=wb.cc;

    for (l=4; l<8; l++)
    {
      f=quenmove[l];

      i=k;
      do {
        i+=f;
        if (*(pbt2+i)==2)
        {
          mv1[mt]=p;
          mv2[mt]=i;
          mt++;
          break;
        }        
      } while (*(pbt+i)==0);
    }
  }
  mvtop=mt;
}


void mv2wquen()
{
  register char *pbt;
  register char *pbt2;
  register int mt;
  register int i;
  char *fpt;
  int j,k,f,p,l;

  mt=mvtop;
  fpt=flc[1];
  for (j=0; j<fln[1]; j++)
  {
    k=*fpt++;
    p=j+12;

    if (*(wb.cc+k)!=0) continue;  /* vollkommen gefesselt */

    pbt=cb.cc;
    pbt2=wb.cc;

    for (l=0; l<8; l++)
    {
      f=quenmove[l];

      i=k;
      do {
        i+=f;
        if (*(pbt2+i)==2)
        {
          mv1[mt]=p;
          mv2[mt]=i;
          mt++;
          break;
        }        
      } while (*(pbt+i)==0);
    }
  }
  mvtop=mt;
}


/**********************************************/

void mv1spawn()
{
  register char *pbt;
  register int mt;
  char *fpt;
  int f,j,k,p;

  mt=mvtop;
  fpt=flc[9];
  for (j=0; j<fln[9];j++)
  {
    k=*fpt++;
    p=j+108;

    f=*(sb.cc+k);    
    pbt=cb.cc+k;

    if (k<48)
    {
      p+=3328;

      if (f!=0)  // Auf der Umwandlungsreihe sind bei f=32 oder f=+/-12 hier keine Zuege moeglich.
      {
        if (f==-13)
         {
           mv1[mt]=p;
           mv2[mt]=k-13;
           mt++;
           mv1[mt]=p-768;
           mv2[mt]=k-13;
           mt++;
           mv1[mt]=p-256;
           mv2[mt]=k-13;
           mt++;
           mv1[mt]=p-512;
           mv2[mt]=k-13;
           mt++;
           continue;
         }
                
        if (f==-11)  /* Kann in dieser Richtung ziehen */
        {
          mv1[mt]=p;
          mv2[mt]=k-11;
          mt++;
          mv1[mt]=p-768;
          mv2[mt]=k-11;
          mt++;
          mv1[mt]=p-256;
          mv2[mt]=k-11;
          mt++;
          mv1[mt]=p-512;
          mv2[mt]=k-11;
          mt++;
          continue;
        }

        continue;
      }

      if (*(pbt-12)==0)
      {
        mv1[mt]=p;
        mv2[mt]=k-12;
        mt++;
        mv1[mt]=p-768;
        mv2[mt]=k-12;
        mt++;
        mv1[mt]=p-256;
        mv2[mt]=k-12;
        mt++;
        mv1[mt]=p-512;
        mv2[mt]=k-12;
        mt++;
      }
      if ((*(pbt-13)!=0)&&(*(pbt-13)<6))
      {
        mv1[mt]=p;
        mv2[mt]=k-13;
        mt++;
        mv1[mt]=p-768;
        mv2[mt]=k-13;
        mt++;
        mv1[mt]=p-256;
        mv2[mt]=k-13;
        mt++;
        mv1[mt]=p-512;
        mv2[mt]=k-13;
        mt++;
      }      
      if ((*(pbt-11)!=0)&&(*(pbt-11)<6))
      {
        mv1[mt]=p;
        mv2[mt]=k-11;
        mt++;
        mv1[mt]=p-768;
        mv2[mt]=k-11;
        mt++;
        mv1[mt]=p-256;
        mv2[mt]=k-11;
        mt++;
        mv1[mt]=p-512;
        mv2[mt]=k-11;
        mt++;
      }
      continue;
    }

    if (f!=0)
    {
      if ((f==12)||(f==-12))
      {
        if (*(pbt-12)==0)
        {
          mv1[mt]=p;
          mv2[mt]=k-12;
          mt++;
          if (k>99)
          {
            if (*(pbt-24)==0)
            {
              mv1[mt]=p+4096;
              mv2[mt]=k-24;
              mt++;
            }
          }
        }
        continue;
      }

      if (f==-13)
      {
        mv1[mt]=p;
        mv2[mt]=k-13;
        mt++;
        continue;
      }
      
      if (f==-11)  /* Kann in dieser Richtung ziehen */
      {
        mv1[mt]=p;
        mv2[mt]=k-11;
        mt++;
        continue;
      }

      continue;
    }

    if (*(pbt-12)==0)
    {
      mv1[mt]=p;
      mv2[mt]=k-12;
      mt++;
      if (k>99)
      {
        if (*(pbt-24)==0)
        {
          mv1[mt]=p+4096;
          mv2[mt]=k-24;
          mt++;
        }
      }
    }
    if ((*(pbt-13)!=0)&&(*(pbt-13)<6))
    {
      mv1[mt]=p;
      mv2[mt]=k-13;
      mt++;
    }    
    if ((*(pbt-11)!=0)&&(*(pbt-11)<6))
    {
      mv1[mt]=p;
      mv2[mt]=k-11;
      mt++;
    }
  }
  mvtop=mt;
}


void mv1sknig()
{
  register char *pbt;
  register int mt;
  char *fpt;
  int i,j,k,f,p;

  mt=mvtop;
  fpt=flc[10];
  for (j=0; j<fln[10];j++)
  {
    k=*fpt++;
    p=j+120;

    if (*(sb.cc+k)!=0) continue;  /* gefesselt, bei Springer immer total */

    pbt=cb.cc+k;

    for (i=0; i<8; i++)
    {
      f=knigmove_s[i];
      if (*(pbt+f)<6)
      {
        mv1[mt]=p;
        mv2[mt]=k+f;
        mt++;
      }
    }
  }
  mvtop=mt;
}


void mv1sbish()
{
  register char *pbt;
  register int mt;
  register int i;
  char *fpt;
  int j,k,f,p,l;

  mt=mvtop;
  fpt=flc[11];
  for (j=0; j<fln[11];j++)
  {
    k=*fpt++;
    p=j+132;

    f=*(sb.cc+k);    

    if (f!=0)  /* Kann nur in Fesselrichtung ziehen */
    {
      if (f==32) continue;  /* vollkommen gefesselt */
      pbt=cb.cc;
      i=k-f;
      while (*(pbt+i)==0)
      {
        mv1[mt]=p;
        mv2[mt]=i;
        mt++;
        i-=f;
      }      
      i=k;
      do
      {
        i+=f;
        mv1[mt]=p;
        mv2[mt]=i;
        mt++;
      } while (*(pbt+i)==0);
      continue;
    }

    pbt=cb.cc;

    for (l=0; l<4; l++)
    {
      f=quenmove_s[l];

      i=k+f;
      while (*(pbt+i)==0)
      {
        mv1[mt]=p;
        mv2[mt]=i;
        mt++;
        i+=f;
      }
      if (*(pbt+i)<6)
      {
        mv1[mt]=p;
        mv2[mt]=i;
        mt++;
      }
    }
  }
  mvtop=mt;
}


void mv1srook()
{
  register char *pbt;
  register int mt;
  register int i;
  char *fpt;
  int j,k,f,p,l;

  mt=mvtop;
  fpt=flc[12];
  for (j=0; j<fln[12];j++)
  {
    k=*fpt++;
    p=j+144;

    f=*(sb.cc+k);
    
    if (f!=0)  /* Kann nur in Fesselrichtung ziehen */
    {
      if (f==32) continue;  /* vollkommen gefesselt */
      pbt=cb.cc;
      i=k;
      do
      {
        i+=f;
        mv1[mt]=p;
        mv2[mt]=i;
        mt++;
      } while (*(pbt+i)==0);
      i=k-f;
      while (*(pbt+i)==0)
      {
        mv1[mt]=p;
        mv2[mt]=i;
        mt++;
        i-=f;
      }           
      continue;
    }

    pbt=cb.cc;

    for (l=4; l<8; l++)
    {
      f=quenmove_s[l];

      i=k+f;
      while (*(pbt+i)==0)
      {
        mv1[mt]=p;
        mv2[mt]=i;
        mt++;
        i+=f;
      }
      if (*(pbt+i)<6)
      {
        mv1[mt]=p;
        mv2[mt]=i;
        mt++;
      }
    }
  }
  mvtop=mt;
}


void mv1squen()
{
  register char *pbt;
  register int mt;
  register int i;
  char *fpt;
  int j,k,f,p,l;

  mt=mvtop;
  fpt=flc[13];
  for (j=0; j<fln[13];j++)
  {
    k=*fpt++;
    p=j+156;

    f=*(sb.cc+k);    

    if (f!=0)  /* Kann nur in Fesselrichtung ziehen */
    {
      pbt=cb.cc;
      i=k-f;
      while (*(pbt+i)==0)
      {
        mv1[mt]=p;
        mv2[mt]=i;
        mt++;
        i-=f;
      }      
      i=k;
      do
      {
        i+=f;
        mv1[mt]=p;
        mv2[mt]=i;
        mt++;
      } while (*(pbt+i)==0);            
      continue;
    }

    pbt=cb.cc;

    for (l=0; l<8; l++)
    {
      f=quenmove_s[l];

      i=k+f;
      while (*(pbt+i)==0)
      {
        mv1[mt]=p;
        mv2[mt]=i;
        mt++;
        i+=f;
      }
      if (*(pbt+i)<6)
      {
        mv1[mt]=p;
        mv2[mt]=i;
        mt++;
      }
    }
  }
  mvtop=mt;
}


void mv2spawn()
{
  register char *pbt;
  register char *pbt2;
  register int mt;
  char *fpt;
  int j,k,p;

  mt=mvtop;
  fpt=flc[9];
  for (j=0; j<fln[9];j++)
  {
    k=*fpt++;
    p=j+108;

    if (*(sb.cc+k)!=0) continue;  /* vollkommen gefesselt */

    pbt=cb.cc+k;
    pbt2=sb.cc+k;

    if (k<48)
    {
      p+=3328;

      if ((*(pbt2-12)==2)&&(*(pbt-12)==0))
      {
        mv1[mt]=p;
        mv2[mt]=k-12;
        mt++;
        mv1[mt]=p-768;
        mv2[mt]=k-12;
        mt++;
        mv1[mt]=p-256;
        mv2[mt]=k-12;
        mt++;
        mv1[mt]=p-512;
        mv2[mt]=k-12;
        mt++;

      }
      if ((*(pbt2-13)==2)&&(*(pbt-13)!=0)&&(*(pbt-13)<6))
      {
        mv1[mt]=p;
        mv2[mt]=k-13;
        mt++;
        mv1[mt]=p-768;
        mv2[mt]=k-13;
        mt++;
        mv1[mt]=p-256;
        mv2[mt]=k-13;
        mt++;
        mv1[mt]=p-512;
        mv2[mt]=k-13;
        mt++;
      }      
      if ((*(pbt2-11)==2)&&(*(pbt-11)!=0)&&(*(pbt-11)<6)) 
      {
        mv1[mt]=p;
        mv2[mt]=k-11;
        mt++;
        mv1[mt]=p-768;
        mv2[mt]=k-11;
        mt++;
        mv1[mt]=p-256;
        mv2[mt]=k-11;
        mt++;
        mv1[mt]=p-512;
        mv2[mt]=k-11;
        mt++;
      }
      continue;
    }

    if (*(pbt-12)==0)
    {
      if (*(pbt2-12)==2)
      {
        mv1[mt]=p;
        mv2[mt]=k-12;
        mt++;
      }
      if (k>99)
      {
        if ((*(pbt2-24)==2)&&(*(pbt-24)==0))
        {
          mv1[mt]=p+4096;
          mv2[mt]=k-24;
          mt++;
        }
      }
    }
    if ((*(pbt2-13)==2)&&(*(pbt-13)!=0)&&(*(pbt-13)<6))
    {
      mv1[mt]=p;
      mv2[mt]=k-13;
      mt++;
    }    
    if ((*(pbt2-11)==2)&&(*(pbt-11)!=0)&&(*(pbt-11)<6))
    {
      mv1[mt]=p;
      mv2[mt]=k-11;
      mt++;
    }
  }
  mvtop=mt;
}


void mv2sknig()
{
  register char *pbt2;
  register int mt;
  char *fpt;
  int i,j,k,f,p;

  mt=mvtop;
  fpt=flc[10];
  for (j=0; j<fln[10];j++)
  {
    k=*fpt++;
    p=j+120;

    if (*(sb.cc+k)!=0) continue;  /* gefesselt, bei Springer immer total */

    pbt2=sb.cc+k;

    for (i=0; i<8; i++)
    {
      f=knigmove_s[i];
      if (*(pbt2+f)==2)
      {
        mv1[mt]=p;
        mv2[mt]=k+f;
        mt++;
      }
    }
  }
  mvtop=mt;
}


void mv2sbish()
{
  register char *pbt;
  register char *pbt2;
  register int mt;
  register int i;
  char *fpt;
  int j,k,f,p,l;

  mt=mvtop;
  fpt=flc[11];
  for (j=0; j<fln[11];j++)
  {
    k=*fpt++;
    p=j+132;

    if (*(sb.cc+k)!=0) continue;  /* vollkommen gefesselt */

    pbt=cb.cc;
    pbt2=sb.cc;

    for (l=0; l<4; l++)
    {
      f=quenmove_s[l];
      
      i=k;
      do {
        i+=f;
        if (*(pbt2+i)==2)
        {
          mv1[mt]=p;
          mv2[mt]=i;
          mt++;
          break;
        }        
      } while (*(pbt+i)==0);  
    }
  }
  mvtop=mt;
}


void mv2srook()
{
  register char *pbt;
  register int mt;
  register int i;
  char *pbt2;
  char *fpt;
  int j,k,f,p,l;

  mt=mvtop;
  fpt=flc[12];
  for (j=0; j<fln[12];j++)
  {
    k=*fpt++;
    p=j+144;

    if (*(sb.cc+k)!=0) continue;  /* vollkommen gefesselt */

    pbt=cb.cc;
    pbt2=sb.cc;

    for (l=4; l<8; l++)
    {
      f=quenmove_s[l];
      
      i=k;
      do {
        i+=f;
        if (*(pbt2+i)==2)
        {
          mv1[mt]=p;
          mv2[mt]=i;
          mt++;
          break;
        }        
      } while (*(pbt+i)==0);  
    }
  }
  mvtop=mt;
}


void mv2squen()
{
  register char *pbt;
  register char *pbt2;
  register int mt;
  register int i;
  char *fpt;
  int j,k,f,p,l;

  mt=mvtop;
  fpt=flc[13];
  for (j=0; j<fln[13];j++)
  {
    k=*fpt++;
    p=j+156;

    if (*(sb.cc+k)!=0) continue;  /* vollkommen gefesselt */

    pbt=cb.cc;
    pbt2=sb.cc;

    for (l=0; l<8; l++)
    {
      f=quenmove_s[l];

      i=k;
      do {
        i+=f;
        if (*(pbt2+i)==2)
        {
          mv1[mt]=p;
          mv2[mt]=i;
          mt++;
          break;
        }        
      } while (*(pbt+i)==0);            
    }
  }
  mvtop=mt;
}

/**********************************************/


/* Gleiche EP-Pruefung mit und ohne Schach; alte Zugreihenfolge beibehalten. */
static void append_ep(int black)
{
  int pawn=black ? lwm : lsm, piece=black ? 9 : 5, offset;
  if (pawn<=16) return;
  for (offset=1; offset>=-1; offset-=2)
  {
    int from=pawn+offset, index;
    if (!ep_legal(black,from,pawn)) continue;
    for (index=0; index<fln[piece] && flc[piece][index]!=from; index++) {}
    if (index==fln[piece]) continue;
    mv1[mvtop]=piece*12+index+256*pawn;
    mv2[mvtop]=(unsigned char)(pawn+(black ? -12 : 12));
    mvtop++;
  }
}

void mv1wep(void) { append_ep(0); }
void mv2wep(void) { append_ep(0); }
void mv1sep(void) { append_ep(1); }
void mv2sep(void) { append_ep(1); }

// Zugerzeugung fuer Stellungen ohne Fesselungen

void mv1wpawn_nofess()
{
  register char *pbt;
  register int mt;
  char * fpt;
  int j,k,p;

  mt=mvtop;
  fpt=flc[5];
  for (j=0; j<fln[5];j++)
  {
    k=*fpt++;
    p=j+60;
    pbt=cb.cc+k;

    if (k>99)
    {
      p+=256;

      if (*(pbt+12)==0)
      {
        mv1[mt]=p;
        mv2[mt]=k+12;
        mt++;
        mv1[mt]=p+768;
        mv2[mt]=k+12;
        mt++;
        mv1[mt]=p+256;
        mv2[mt]=k+12;
        mt++;
        mv1[mt]=p+512;
        mv2[mt]=k+12;
        mt++;
      }
      if (*(pbt+11)>8)
      {
        mv1[mt]=p;
        mv2[mt]=k+11;
        mt++;
        mv1[mt]=p+768;
        mv2[mt]=k+11;
        mt++;
        mv1[mt]=p+256;
        mv2[mt]=k+11;
        mt++;
        mv1[mt]=p+512;
        mv2[mt]=k+11;
        mt++;
      }
      if (*(pbt+13)>8)
      {
        mv1[mt]=p;
        mv2[mt]=k+13;
        mt++;
        mv1[mt]=p+768;
        mv2[mt]=k+13;
        mt++;
        mv1[mt]=p+256;
        mv2[mt]=k+13;
        mt++;
        mv1[mt]=p+512;
        mv2[mt]=k+13;
        mt++;
      }
      continue;
    }

    if (*(pbt+12)==0)
    {
      mv1[mt]=p;
      mv2[mt]=k+12;
      mt++;
      if (k<48)
      {
        if (*(pbt+24)==0)
        {
          mv1[mt]=p+4096;
          mv2[mt]=k+24;
          mt++;
        }
      }
    }
    if (*(pbt+11)>8)
    {
      mv1[mt]=p;
      mv2[mt]=k+11;
      mt++;
    }
    if (*(pbt+13)>8)
    {
      mv1[mt]=p;
      mv2[mt]=k+13;
      mt++;
    }
  }
  mvtop=mt;
}


void mv1wknig_nofess()
{
  register char *pbt;
  register int mt;
  register int f;
  char *fpt;
  int i,j,k,p;

  mt=mvtop;
  fpt=flc[4];
  for (j=0; j<fln[4]; j++)
  {
    k=*fpt++;
    p=j+48;
    pbt=cb.cc+k;

    for (i=0; i<8; i++)
    {
      f=knigmove[i];
      if ((*(pbt+f)==0)||(*(pbt+f)>8))
      {
        mv1[mt]=p;
        mv2[mt]=k+f;
        mt++;
      }
    }
  }
  mvtop=mt;
}


void mv1wbish_nofess()
{
  register char *pbt;
  register int mt;
  register int i;
  char *fpt;
  int j,k,f,p,l;

  mt=mvtop;
  fpt=flc[3];
  for (j=0; j<fln[3]; j++)
  {
    k=*fpt++;
    p=j+36;
    pbt=cb.cc;

    for (l=0; l<4; l++)
    {
      f=quenmove[l];

      i=k+f;
      while (*(pbt+i)==0)
      {
        mv1[mt]=p;
        mv2[mt]=i;
        mt++;
        i+=f;
      }
      if (*(pbt+i)>8)
      {
        mv1[mt]=p;
        mv2[mt]=i;
        mt++;
      }
    }
  }
  mvtop=mt;
}


void mv1wrook_nofess()
{
  register char *pbt;
  register int mt;
  register int i;
  char *fpt;
  int j,k,f,p,l;

  mt=mvtop;
  fpt=flc[2];
  for (j=0; j<fln[2]; j++)
  {
    k=*fpt++;
    p=j+24;
    pbt=cb.cc;

    for (l=4; l<8; l++)
    {
      f=quenmove[l];

      i=k+f;
      while (*(pbt+i)==0)
      {
        mv1[mt]=p;
        mv2[mt]=i;
        mt++;
        i+=f;
      }
      if (*(pbt+i)>8)
      {
        mv1[mt]=p;
        mv2[mt]=i;
        mt++;
      }
    }
  }
  mvtop=mt;
}


void mv1wquen_nofess()
{
  register char *pbt;
  register int mt;
  register int i;
  char *fpt;
  int j,k,f,p,l;

  mt=mvtop;
  fpt=flc[1];
  for (j=0; j<fln[1]; j++)
  {
    k=*fpt++;
    p=j+12;
    pbt=cb.cc;

    for (l=0; l<8; l++)
    {
      f=quenmove[l];

      i=k+f;
      while (*(pbt+i)==0)
      {
        mv1[mt]=p;
        mv2[mt]=i;
        mt++;
        i+=f;
      }
      if (*(pbt+i)>8)
      {
        mv1[mt]=p;
        mv2[mt]=i;
        mt++;
      }
    }
  }
  mvtop=mt;
}

void mv1spawn_nofess()
{
  register char *pbt;
  register int mt;
  char *fpt;
  int j,k,p;

  mt=mvtop;
  fpt=flc[9];
  for (j=0; j<fln[9];j++)
  {
    k=*fpt++;
    p=j+108;    
    pbt=cb.cc+k;

    if (k<48)
    {
      p+=3328;

      if (*(pbt-12)==0)
      {
        mv1[mt]=p;
        mv2[mt]=k-12;
        mt++;
        mv1[mt]=p-768;
        mv2[mt]=k-12;
        mt++;
        mv1[mt]=p-256;
        mv2[mt]=k-12;
        mt++;
        mv1[mt]=p-512;
        mv2[mt]=k-12;
        mt++;
      }
      if ((*(pbt-13)!=0)&&(*(pbt-13)<6))
      {
        mv1[mt]=p;
        mv2[mt]=k-13;
        mt++;
        mv1[mt]=p-768;
        mv2[mt]=k-13;
        mt++;
        mv1[mt]=p-256;
        mv2[mt]=k-13;
        mt++;
        mv1[mt]=p-512;
        mv2[mt]=k-13;
        mt++;
      }      
      if ((*(pbt-11)!=0)&&(*(pbt-11)<6))
      {
        mv1[mt]=p;
        mv2[mt]=k-11;
        mt++;
        mv1[mt]=p-768;
        mv2[mt]=k-11;
        mt++;
        mv1[mt]=p-256;
        mv2[mt]=k-11;
        mt++;
        mv1[mt]=p-512;
        mv2[mt]=k-11;
        mt++;
      }
      continue;
    }

    if (*(pbt-12)==0)
    {
      mv1[mt]=p;
      mv2[mt]=k-12;
      mt++;
      if (k>99)
      {
        if (*(pbt-24)==0)
        {
          mv1[mt]=p+4096;
          mv2[mt]=k-24;
          mt++;
        }
      }
    }
    if ((*(pbt-13)!=0)&&(*(pbt-13)<6))
    {
      mv1[mt]=p;
      mv2[mt]=k-13;
      mt++;
    }    
    if ((*(pbt-11)!=0)&&(*(pbt-11)<6))
    {
      mv1[mt]=p;
      mv2[mt]=k-11;
      mt++;
    }
  }
  mvtop=mt;
}


void mv1sknig_nofess()
{
  register char *pbt;
  register int mt;
  char *fpt;
  int i,j,k,f,p;

  mt=mvtop;
  fpt=flc[10];
  for (j=0; j<fln[10];j++)
  {
    k=*fpt++;
    p=j+120;
    pbt=cb.cc+k;

    for (i=0; i<8; i++)
    {
      f=knigmove_s[i];
      if (*(pbt+f)<6)
      {
        mv1[mt]=p;
        mv2[mt]=k+f;
        mt++;
      }
    }
  }
  mvtop=mt;
}


void mv1sbish_nofess()
{
  register char *pbt;
  register int mt;
  register int i;
  char *fpt;
  int j,k,f,p,l;

  mt=mvtop;
  fpt=flc[11];
  for (j=0; j<fln[11];j++)
  {
    k=*fpt++;
    p=j+132;
    pbt=cb.cc;

    for (l=0; l<4; l++)
    {
      f=quenmove_s[l];

      i=k+f;
      while (*(pbt+i)==0)
      {
        mv1[mt]=p;
        mv2[mt]=i;
        mt++;
        i+=f;
      }
      if (*(pbt+i)<6)
      {
        mv1[mt]=p;
        mv2[mt]=i;
        mt++;
      }
    }
  }
  mvtop=mt;
}


void mv1srook_nofess()
{
  register char *pbt;
  register int mt;
  register int i;
  char *fpt;
  int j,k,f,p,l;

  mt=mvtop;
  fpt=flc[12];
  for (j=0; j<fln[12];j++)
  {
    k=*fpt++;
    p=j+144;
    pbt=cb.cc;

    for (l=4; l<8; l++)
    {
      f=quenmove_s[l];

      i=k+f;
      while (*(pbt+i)==0)
      {
        mv1[mt]=p;
        mv2[mt]=i;
        mt++;
        i+=f;
      }
      if (*(pbt+i)<6)
      {
        mv1[mt]=p;
        mv2[mt]=i;
        mt++;
      }
    }
  }
  mvtop=mt;
}


void mv1squen_nofess()
{
  register char *pbt;
  register int mt;
  register int i;
  char *fpt;
  int j,k,f,p,l;

  mt=mvtop;
  fpt=flc[13];
  for (j=0; j<fln[13];j++)
  {
    k=*fpt++;
    p=j+156;
    pbt=cb.cc;

    for (l=0; l<8; l++)
    {
      f=quenmove_s[l];

      i=k+f;
      while (*(pbt+i)==0)
      {
        mv1[mt]=p;
        mv2[mt]=i;
        mt++;
        i+=f;
      }
      if (*(pbt+i)<6)
      {
        mv1[mt]=p;
        mv2[mt]=i;
        mt++;
      }
    }
  }
  mvtop=mt;
}

// Ende der Varianten ohne Fesselungen


int generate_white()
{
  int mp1=mvtop;
  
  if (pos[g].wch1==0)
  {
    if (pos[g].fesselung==0)
    {
      if (fln[1] != 0) mv1wquen_nofess();
      if (fln[2] != 0) mv1wrook_nofess();
      if (fln[3] != 0) mv1wbish_nofess();
      if (fln[4] != 0) mv1wknig_nofess();
      if (fln[5] != 0) mv1wpawn_nofess();
    }
    else
    {
      if (fln[1] != 0) mv1wquen();
      if (fln[2] != 0) mv1wrook();
      if (fln[3] != 0) mv1wbish();
      if (fln[4] != 0) mv1wknig();
      if (fln[5] != 0) mv1wpawn();
    }
    if (lsm>16) mv1wep();
    if (pos[g].wr1>0) mv1wrochade();
  }
 
  if (pos[g].wch1==1)
  {
    if (fln[1] != 0) mv2wquen();
    if (fln[2] != 0) mv2wrook();
    if (fln[3] != 0) mv2wbish();
    if (fln[4] != 0) mv2wknig();
    if (fln[5] != 0) mv2wpawn();
    if (lsm>16) mv2wep();
  }
  
  mv1wking();
  return mvtop-mp1;
}

int generate_black()
{
  int mp1=mvtop;
  
  if (pos[g].sch1==0)
  {
    if (pos[g].fesselung==0)
    {
      if (fln[13] != 0) mv1squen_nofess();
      if (fln[12] != 0) mv1srook_nofess();
      if (fln[11] != 0) mv1sbish_nofess();
      if (fln[10] != 0) mv1sknig_nofess();
      if (fln[ 9] != 0) mv1spawn_nofess();
    }
    else
    {
      if (fln[13] != 0) mv1squen();
      if (fln[12] != 0) mv1srook();
      if (fln[11] != 0) mv1sbish();
      if (fln[10] != 0) mv1sknig();
      if (fln[ 9] != 0) mv1spawn();
    }
    if (lwm>16) mv1sep();
    if (pos[g].sr1>0) mv1srochade();
  }

  if (pos[g].sch1==1)
  {
    if (fln[13] != 0) mv2squen();
    if (fln[12] != 0) mv2srook();    
    if (fln[11] != 0) mv2sbish();
    if (fln[10] != 0) mv2sknig();
    if (fln[ 9] != 0) mv2spawn();
    if (lwm>16) mv2sep();
  }

  mv1sking();
  return mvtop-mp1;
}

int generate_moves()
{
  
  if (g&1)
    return generate_black();
  else
    return generate_white();
}


// Fuer Matt-Suche
int generate_black_one_move()
{
  int mp1=mvtop;
  
  if (pos[g].sch1==0)
  {    
    if (pos[g].fesselung==0)
    {
      if (fln[13] != 0) mv1squen_nofess();
      if (fln[12] != 0) mv1srook_nofess();
      if (fln[11] != 0) mv1sbish_nofess();
      if (fln[10] != 0) mv1sknig_nofess();
      if (fln[ 9] != 0) mv1spawn_nofess();
    }
    else
    {
      if (fln[13] != 0) mv1squen();
      if (fln[12] != 0) mv1srook();
      if (fln[11] != 0) mv1sbish();
      if (fln[10] != 0) mv1sknig();
      if (fln[ 9] != 0) mv1spawn();
    }
    if (lwm>16) mv1sep();
    if (pos[g].sr1>0) mv1srochade();

    mv1sking();
    return mvtop-mp1;
  }     

  if (pos[g].sch1==1)
  {
    if (fln[13] != 0) 
    {    
      mv2squen();
      if (mvtop>mp1) return 1;
    }
    if (fln[12] != 0)
    {    
       mv2srook();    
       if (mvtop>mp1) return 1;
    }
    if (fln[11] != 0)
    {    
       mv2sbish();
       if (mvtop>mp1) return 1;
    }
    if (fln[10] != 0)
    {    
       mv2sknig();
       if (mvtop>mp1) return 1;
    }
    if (fln[ 9] != 0)
    {           
      mv2spawn();
      if (mvtop>mp1) return 1;
    }
    if (lwm>16)
    {    
       mv2sep();
       if (mvtop>mp1) return 1;
    }
  }

  mv1sking_one_move();
  
  return mvtop-mp1;
}

/********************************** perft_white ****************************/

void mv1wknig_perft()
{
  register char *pbt;
  register int mt;
  register int f;
  char *fpt;
  int i,j,k;

  mt=mvtop;
  fpt=flc[4];
  for (j=0; j<fln[4]; j++)
  {
    k=*fpt++;

    if (*(wb.cc+k)!=0) continue;  /* gefesselt, bei Springer immer total */

    pbt=cb.cc+k;

    for (i=0; i<8; i++)
    {
      f=knigmove[i];
      if ((*(pbt+f)==0)||(*(pbt+f)>8))
      {         
        mt++;
      }
    }
  }
  mvtop=mt;
}

void mv1wbish_perft()
{
  register char *pbt;
  register int mt;
  register int i;
  char *fpt;
  int j,k,f,l;

  mt=mvtop;
  fpt=flc[3];
  for (j=0; j<fln[3]; j++)
  {
    k=*fpt++;
    f=*(wb.cc+k);

    if (f!=0)  /* Kann nur in Fesselrichtung ziehen */
    {
      if (f==32) continue;            
      pbt=cb.cc;
      i=k;
      do
      {
        i+=f;
        mt++;
      } while (*(pbt+i)==0);
      i=k-f;
      while (*(pbt+i)==0)
      {
        mt++;
        i-=f;
      }
      continue;
    }

    pbt=cb.cc;

    for (l=0; l<4; l++)
    {
      f=quenmove[l];

      i=k+f;
      while (*(pbt+i)==0)
      {
        mt++;
        i+=f;
      }
      if (*(pbt+i)>8)
      {
        mt++;
      }
    }
  }
  mvtop=mt;
}


void mv1wrook_perft()
{
  register char *pbt;
  register int mt;
  register int i;
  char *fpt;
  int j,k,f,l;

  mt=mvtop;
  fpt=flc[2];
  for (j=0; j<fln[2]; j++)
  {
    k=*fpt++;
    f=*(wb.cc+k);

    if (f!=0)  /* Kann nur in Fesselrichtung ziehen */
    {
      if (f==32) continue;            
      pbt=cb.cc;
      i=k;
      do
      {
        i+=f;
        mt++;
      } while (*(pbt+i)==0);
      i=k-f;
      while (*(pbt+i)==0)
      {
        mt++;
        i-=f;
      }
      continue;
    }

    pbt=cb.cc;

    for (l=4; l<8; l++)
    {
      f=quenmove[l];

      i=k+f;
      while (*(pbt+i)==0)
      {
        mt++;
        i+=f;
      }
      if (*(pbt+i)>8)
      {
        mt++;
      }
    }
  }
  mvtop=mt;
}

void mv1wquen_perft()
{
  register char *pbt;
  register int mt;
  register int i;
  char *fpt;
  int j,k,f,l;

  mt=mvtop;
  fpt=flc[1];
  for (j=0; j<fln[1]; j++)
  {
    k=*fpt++;
    f=*(wb.cc+k);

    if (f!=0)  /* Kann nur in Fesselrichtung ziehen */
    {
      if (f==32) continue;            
      pbt=cb.cc;
      i=k;
      do
      {
        i+=f;
        mt++;
      } while (*(pbt+i)==0);
      i=k-f;
      while (*(pbt+i)==0)
      {
        mt++;
        i-=f;
      }
      continue;
    }

    pbt=cb.cc;

    for (l=0; l<8; l++)
    {
      f=quenmove[l];

      i=k+f;
      while (*(pbt+i)==0)
      {
        mt++;
        i+=f;
      }
      if (*(pbt+i)>8)
      {
        mt++;
      }
    }
  }
  mvtop=mt;
}


void mv1wpawn_perft()
{
  register char *pbt;
  register int mt;
  char * fpt;
  int j,k,f;

  mt=mvtop;
  fpt=flc[5];
  for (j=0; j<fln[5];j++)
  {
    k=*fpt++;
    f=*(wb.cc+k);
    pbt=cb.cc+k;

    if (k>99)
    {
      if (f!=0)
      {
        if (f==11)  /* Kann in dieser Richtung ziehen */
        {
          mt+=4;
          continue;
        }
        if (f==13)
        {
          mt+=4;
          continue;
        }
        continue;
      }

      if (*(pbt+12)==0)
      {
        mt+=4;
      }
      if (*(pbt+11)>8)
      {
        mt+=4;
      }
      if (*(pbt+13)>8)
      {
        mt+=4;
      }
      continue;
    }

    if (f!=0)
    {
      if ((f==12)||(f==-12))
      {
        if (*(pbt+12)==0)
        {
          mt++;
          if (k<48)
          {
            if (*(pbt+24)==0)
            {
              mt++;
            }
          }
        }
        continue;
      }

      if (f==11)  /* Kann in dieser Richtung ziehen */
      {
        mt++;
        continue;
      }

      if (f==13)
      {
        mt++;
        continue;
      }
      continue;
    }

    if (*(pbt+12)==0)
    {
      mt++;
      if (k<48)
      {
        if (*(pbt+24)==0)
        {           
          mt++;
        }
      }
    }
    if (*(pbt+11)>8)
    {
      mt++;
    }
    if (*(pbt+13)>8)
    {
      mt++;
    }
  }
  mvtop=mt;
}

/****************** perft_no_fess_white *********************/

mv1wkbrqp_perft_nofess()
{
  register char *pbt;
  register int mt;
  char *fpt;
  int i,j,k,l,f;

  mt=mvtop;

  // knight

  fpt=flc[4];
  for (j=0; j<fln[4]; j++)
  {
    k=*fpt++;

    pbt=cb.cc+k;

    for (i=0; i<8; i++)
    {
      f=knigmove[i];
      if ((*(pbt+f)==0)||(*(pbt+f)>8))
      {         
        mt++;
      }
    }
  }

  // bishop

  fpt=flc[3];
  for (j=0; j<fln[3]; j++)
  {
    k=*fpt++;

    pbt=cb.cc;

    for (l=0; l<4; l++)
    {
      f=quenmove[l];

      i=k+f;
      while (*(pbt+i)==0)
      {
        mt++;
        i+=f;
      }
      if (*(pbt+i)>8)
      {
        mt++;
      }
    }
  }

  // rook

  fpt=flc[2];
  for (j=0; j<fln[2]; j++)
  {
    k=*fpt++;

    pbt=cb.cc;

    for (l=4; l<8; l++)
    {
      f=quenmove[l];

      i=k+f;
      while (*(pbt+i)==0)
      {
        mt++;
        i+=f;
      }
      if (*(pbt+i)>8)
      {
        mt++;
      }
    }
  }

  // queen

  fpt=flc[1];
  for (j=0; j<fln[1]; j++)
  {
    k=*fpt++;

    pbt=cb.cc;

    for (l=0; l<8; l++)
    {
      f=quenmove[l];

      i=k+f;
      while (*(pbt+i)==0)
      {
        mt++;
        i+=f;
      }
      if (*(pbt+i)>8)
      {
        mt++;
      }
    }
  }

  // pawn

  fpt=flc[5];
  for (j=0; j<fln[5];j++)
  {
    k=*fpt++;

    pbt=cb.cc+k;

    if (k>99)
    {
      if (*(pbt+12)==0)
      {
        mt+=4;
      }
      if (*(pbt+11)>8)
      {
        mt+=4;
      }
      if (*(pbt+13)>8)
      {
        mt+=4;
      }
      continue;
    }

    if (*(pbt+12)==0)
    {
      mt++;
      if (k<48)
      {
        if (*(pbt+24)==0)
        {           
          mt++;
        }
      }
    }
    if (*(pbt+11)>8)
    {
      mt++;
    }
    if (*(pbt+13)>8)
    {
      mt++;
    }
  }
  
  mvtop=mt;
}

/****************** perft_black *********************/

void mv1sknig_perft()
{
  register char *pbt;
  register int mt;
  char *fpt;
  int i,j,k,f;

  mt=mvtop;
  fpt=flc[10];
  for (j=0; j<fln[10];j++)
  {
    k=*fpt++;

    if (*(sb.cc+k)!=0) continue;  /* gefesselt, bei Springer immer total */

    pbt=cb.cc+k;

    for (i=0; i<8; i++)
    {
      f=knigmove_s[i];
      if (*(pbt+f)<6)
      {
        mt++;
      }
    }
  }
  mvtop=mt;
}

void mv1sbish_perft()
{
  register char *pbt;
  register int mt;
  register int i;
  char *fpt;
  int j,k,f,l;

  mt=mvtop;
  fpt=flc[11];
  for (j=0; j<fln[11];j++)
  {
    k=*fpt++;
    f=*(sb.cc+k);

    if (f!=0)  /* Kann nur in Fesselrichtung ziehen */
    {
      if (f==32) continue;
      pbt=cb.cc;
      i=k-f;
      while (*(pbt+i)==0)
      {
        mt++;
        i-=f;
      }      
      i=k;
      do
      {
        i+=f;
        mt++;
      } while (*(pbt+i)==0);
      continue;
    }

    pbt=cb.cc;

    for (l=0; l<4; l++)
    {
      f=quenmove_s[l];

      i=k+f;
      while (*(pbt+i)==0)
      {        
        mt++;
        i+=f;
      }
      if (*(pbt+i)<6)
      { 
        mt++;
      }
    }
  }
  mvtop=mt;
}


void mv1srook_perft()
{
  register char *pbt;
  register int mt;
  register int i;
  char *fpt;
  int j,k,f,l;

  mt=mvtop;
  fpt=flc[12];
  for (j=0; j<fln[12];j++)
  {
    k=*fpt++;
    f=*(sb.cc+k);

    if (f!=0)  /* Kann nur in Fesselrichtung ziehen */
    {
      if (f==32) continue;
      pbt=cb.cc;
      i=k;
      do
      {
        i+=f;
        mt++;
      } while (*(pbt+i)==0);
      i=k-f;
      while (*(pbt+i)==0)
      {
        mt++;
        i-=f;
      }           
      continue;
    }

    pbt=cb.cc;

    for (l=4; l<8; l++)
    {
      f=quenmove_s[l];

      i=k+f;
      while (*(pbt+i)==0)
      {
        mt++;
        i+=f;
      }
      if (*(pbt+i)<6)
      {
        mt++;
      }
    }
  }
  mvtop=mt;
}

void mv1squen_perft()
{
  register char *pbt;
  register int mt;
  register int i;
  char *fpt;
  int j,k,f,l;

  mt=mvtop;
  fpt=flc[13];
  for (j=0; j<fln[13];j++)
  {
    k=*fpt++;
    f=*(sb.cc+k);

    if (f!=0)  /* Kann nur in Fesselrichtung ziehen */
    {
      if (f==32) continue;      
      pbt=cb.cc;
      i=k-f;
      while (*(pbt+i)==0)
      {
        mt++;
        i-=f;
      }      
      i=k;
      do
      {
        i+=f;
        mt++;
      } while (*(pbt+i)==0);            
      continue;
    }

    pbt=cb.cc;

    for (l=0; l<8; l++)
    {
      f=quenmove_s[l];

      i=k+f;
      while (*(pbt+i)==0)
      {
        mt++;
        i+=f;
      }
      if (*(pbt+i)<6)
      {
        mt++;
      }
    }
  }
  mvtop=mt;
}


void mv1spawn_perft()
{
  register char *pbt;
  register int mt;
  char *fpt;
  int f,j,k;

  mt=mvtop;
  fpt=flc[9];
  for (j=0; j<fln[9];j++)
  {
    k=*fpt++;
    f=*(sb.cc+k);
    pbt=cb.cc+k;

    if (k<48)
    {
      if (f!=0)
      {
        if (f==-13)
        {
          mt+=4;
          continue;
        }
                
        if (f==-11)  /* Kann in dieser Richtung ziehen */
        {
          mt+=4;
          continue;
        }

        continue;
      }

      if (*(pbt-12)==0)
      {
        mt+=4;
      }
      if ((*(pbt-13)!=0)&&(*(pbt-13)<6))
      {
        mt+=4;
      }      
      if ((*(pbt-11)!=0)&&(*(pbt-11)<6))
      {
        mt+=4;
      }
      continue;
    }

    if (f!=0)
    {
      if ((f==12)||(f==-12))
      {
        if (*(pbt-12)==0)
        {
          mt++;
          if (k>99)
          {
            if (*(pbt-24)==0)
            {               
              mt++;
            }
          }
        }
        continue;
      }

      if (f==-13)
      {
        mt++;
        continue;
      }
      
      if (f==-11)  /* Kann in dieser Richtung ziehen */
      {
        mt++;
        continue;
      }

      continue;
    }

    if (*(pbt-12)==0)
    {
      mt++;
      if (k>99)
      {
        if (*(pbt-24)==0)
        {           
          mt++;
        }
      }
    }
    if ((*(pbt-13)!=0)&&(*(pbt-13)<6))
    {
      mt++;
    }    
    if ((*(pbt-11)!=0)&&(*(pbt-11)<6))
    {
      mt++;
    }
  }
  mvtop=mt;
}

/****************** perft_no_fess_black *********************/

void mv1skbrqp_perf_nofess()
{
  register char *pbt;
  register int mt;
  char *fpt;
  int i,j,k,l,f;

  mt=mvtop;

  // knight
  fpt=flc[10];
  for (j=0; j<fln[10];j++)
  {
    k=*fpt++;
    pbt=cb.cc+k;

    for (i=0; i<8; i++)
    {
      f=knigmove_s[i];
      if (*(pbt+f)<6)
      {
        mt++;
      }
    }
  }
  
  // bishop

  fpt=flc[11];
  for (j=0; j<fln[11];j++)
  {
    k=*fpt++;
    pbt=cb.cc;

    for (l=0; l<4; l++)
    {
      f=quenmove_s[l];

      i=k+f;
      while (*(pbt+i)==0)
      {        
        mt++;
        i+=f;
      }
      if (*(pbt+i)<6)
      { 
        mt++;
      }
    }
  }

  // rook

  fpt=flc[12];
  for (j=0; j<fln[12];j++)
  {
    k=*fpt++;
    pbt=cb.cc;

    for (l=4; l<8; l++)
    {
      f=quenmove_s[l];

      i=k+f;
      while (*(pbt+i)==0)
      {
        mt++;
        i+=f;
      }
      if (*(pbt+i)<6)
      {
        mt++;
      }
    }
  }

  // queen
  
  fpt=flc[13];
  for (j=0; j<fln[13];j++)
  {
    k=*fpt++;
    pbt=cb.cc;

    for (l=0; l<8; l++)
    {
      f=quenmove_s[l];

      i=k+f;
      while (*(pbt+i)==0)
      {
        mt++;
        i+=f;
      }
      if (*(pbt+i)<6)
      {
        mt++;
      }
    }
  }

  // pawn

  fpt=flc[9];
  for (j=0; j<fln[9];j++)
  {
    k=*fpt++;
    pbt=cb.cc+k;

    if (k<48)
    {
      if (*(pbt-12)==0)
      {
        mt+=4;
      }
      if ((*(pbt-13)!=0)&&(*(pbt-13)<6))
      {
        mt+=4;
      }      
      if ((*(pbt-11)!=0)&&(*(pbt-11)<6))
      {
        mt+=4;
      }
      continue;
    }

    if (*(pbt-12)==0)
    {
      mt++;
      if (k>99)
      {
        if (*(pbt-24)==0)
        {           
          mt++;
        }
      }
    }

    if ((*(pbt-13)!=0)&&(*(pbt-13)<6))
    {
      mt++;
    }    
    if ((*(pbt-11)!=0)&&(*(pbt-11)<6))
    {
      mt++;
    }
  }

  mvtop=mt;
}


int generate_white_perft()
{
  int mp1=mvtop;
  
  if (pos[g].wch1==0)
  {
    if (pos[g].fesselung==0)
    {    
      mv1wkbrqp_perft_nofess();
    }
    else
    {
      if (fln[1] != 0) mv1wquen_perft();
      if (fln[2] != 0) mv1wrook_perft();
      if (fln[3] != 0) mv1wbish_perft();
      if (fln[4] != 0) mv1wknig_perft();
      if (fln[5] != 0) mv1wpawn_perft();
    }
    if (lsm>16) mv1wep();
    if (pos[g].wr1>0) mv1wrochade();
  }
 
  if (pos[g].wch1==1)
  {
    if (fln[1] != 0) mv2wquen();
    if (fln[2] != 0) mv2wrook();
    if (fln[3] != 0) mv2wbish();
    if (fln[4] != 0) mv2wknig();
    if (fln[5] != 0) mv2wpawn();
    if (lsm>16) mv2wep();
  }
  
  mv1wking();
  return mvtop-mp1;
}


int generate_black_perft()
{
  int mp1=mvtop;
  
  if (pos[g].sch1==0)
  {
    if (pos[g].fesselung==0)
    {
      mv1skbrqp_perf_nofess();                
    }
    else
    {
      if (fln[13] != 0) mv1squen_perft();
      if (fln[12] != 0) mv1srook_perft();
      if (fln[11] != 0) mv1sbish_perft();
      if (fln[10] != 0) mv1sknig_perft();
      if (fln[ 9] != 0) mv1spawn_perft();
    }
    if (lwm>16) mv1sep();
    if (pos[g].sr1>0) mv1srochade();
  }

  if (pos[g].sch1==1)
  {
    if (fln[13] != 0) mv2squen();
    if (fln[12] != 0) mv2srook();    
    if (fln[11] != 0) mv2sbish();
    if (fln[10] != 0) mv2sknig();
    if (fln[ 9] != 0) mv2spawn();
    if (lwm>16) mv2sep();
  }

  mv1sking();
  return mvtop-mp1;
}


// (Fast) Nur Schach-Zuege generieren
int generate_white_check()
{
  int mp1=mvtop;
  
  if (pos[g].wch1==0)
  {
    if (pos[g].fesselung==0)
    {
      if (fln[1] != 0) mv1wquen_nofess();
      if (fln[2] != 0) mv1wrook_nofess();
      if (fln[3] != 0) mv1wbish_nofess();
      if (fln[4] != 0) mv1wknig_nofess();
      if (fln[5] != 0) mv1wpawn_nofess();
    }
    else
    {
      if (fln[1] != 0) mv1wquen();
      if (fln[2] != 0) mv1wrook();
      if (fln[3] != 0) mv1wbish();
      if (fln[4] != 0) mv1wknig();
      if (fln[5] != 0) mv1wpawn();
    }
    if (lsm>16) mv1wep();
    if (pos[g].wr1>0) mv1wrochade();
  }
 
  if (pos[g].wch1==1)
  {
    if (fln[1] != 0) mv2wquen();
    if (fln[2] != 0) mv2wrook();
    if (fln[3] != 0) mv2wbish();
    if (fln[4] != 0) mv2wknig();
    if (fln[5] != 0) mv2wpawn();
    if (lsm>16) mv2wep();
  }
  
  if (sb2.cc[flc[6][0]]==5)   // Koenig kann durch wegziehen Schach geben
    mv1wking();
  
  return mvtop-mp1;
}


int generate_moves_perft()
{ 
  if (g&1)
    return generate_black_perft();
  else
    return generate_white_perft();
}


/* Nur ein tatsaechlich legaler EP-Schlag unterscheidet Endstellungen. */
int generate_moves_ep()
{
  int black=g&1, pawn=black ? lwm : lsm;
  return pawn>16 && (ep_legal(black,pawn+1,pawn) || ep_legal(black,pawn-1,pawn));
}
