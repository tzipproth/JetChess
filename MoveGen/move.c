#include "move.h"
#include "stdlib.h"
      
struct POS1
{
  uint64 hashkey,hashkey2;
  int a,b,p1;
  unsigned char d,z,*pbt2;
  unsigned char hash_doppel;
  unsigned char lwm,lsm,wch1,sch1,wr1,sr1,fesselung;
} pos[72];

int g;

int fln[16];
unsigned char flc[16][12];
unsigned char *flp;
unsigned char lwm,lsm;

char knight_attack[144+144];

uint64 zobrist[16][128];
uint64 zobrist_rochade[4];

__int64 bit64[64],rook64[64],bishop64[64];
__int64 bit120[120],rook120[120],bishop120[120];
__int64 rook_bits,bishop_bits;

static int quenmove_c[9]={-13,-11,13,11,-12,1,-1,12};


union board1
{
  unsigned char cc[148];
  long ii[37];
} cb;

union board2
{
  signed char cc[148];
  long ii[37];
} sb,wb,sb2;


// Weisser Koenig im Schach 
int in_check_w()
{
  unsigned char *pbt,*pbtx;

  pbt=cb.cc+flc[6][0];   /* Pointer auf Position des Koenigs */
  pbtx=pbt;

  if (fln[13]+fln[12]>0)
  {
    pbt=pbtx;
    do
    {
      pbt+=1;
    } while (*pbt == 0);
    if (*pbt>11) return 1;
    pbt=pbtx;
    do
    {
      pbt-=1;
    } while (*pbt == 0);
    if (*pbt>11) return 1;
    pbt=pbtx;
    do
    {
      pbt+=12;
    } while (*pbt == 0);
    if (*pbt>11) return 1;
    pbt=pbtx;
    do
    {
      pbt-=12;
    } while (*pbt == 0);
    if (*pbt>11) return 1;
  }

  if (fln[13]+fln[11]>0)
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
    pbt=pbtx;
    do
    {
      pbt-=11;
    } while (*pbt == 0);
    if ((*pbt>10)&&(*pbt!=12)) return 1;
    pbt=pbtx;
    do
    {
      pbt-=13;
    } while (*pbt == 0);
    if ((*pbt>10)&&(*pbt!=12)) return 1;
  }
  
  pbt=pbtx;

  if (pos[g].lsm==10)
  {      
    if (*(pbt+10)==10) return 1;
    if (*(pbt+23)==10) return 1;
    if (*(pbt+25)==10) return 1;
    if (*(pbt+14)==10) return 1;
    if (*(pbt-10)==10) return 1;
    if (*(pbt-23)==10) return 1;
    if (*(pbt-25)==10) return 1;
    if (*(pbt-14)==10) return 1;
  }

  if (*(pbt+11)==9) return 1;
  if (*(pbt+13)==9) return 1;
    
  return 0;
}


// Schwarzer Koenig im Schach
int in_check_b()
{
  unsigned char *pbt, *pbtx;

  pbt=cb.cc+flc[8][0];   /* Pointer auf Position des Koenigs */
  pbtx=pbt;

  if (fln[1]+fln[2]>0)
  {
    pbt=pbtx;
    do
    {
      pbt+=1;
    } while (*pbt == 0);
    if (*pbt<3) return 1;
    pbt=pbtx;
    do
    {
      pbt-=1;
    } while (*pbt == 0);
    if (*pbt<3) return 1;
    pbt=pbtx;
    do
    {
      pbt+=12;
    } while (*pbt == 0);
    if (*pbt<3) return 1;
    pbt=pbtx;
    do
    {
      pbt-=12;
    } while (*pbt == 0);
    if (*pbt<3) return 1;
  }

  if (fln[1]+fln[3]>0)
  {
    pbt=pbtx;
    do
    {
      pbt+=11;
    } while (*pbt == 0);
    if ((*pbt<4)&&(*pbt!=2)) return 1;
    pbt=pbtx;
    do
    {
      pbt+=13;
    } while (*pbt == 0);
    if ((*pbt<4)&&(*pbt!=2)) return 1;
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
    
  if (pos[g].lwm==4)
  {
    if (*(pbt+10)==4) return 1;
    if (*(pbt+23)==4) return 1;
    if (*(pbt+25)==4) return 1;
    if (*(pbt+14)==4) return 1;
    if (*(pbt-10)==4) return 1;
    if (*(pbt-23)==4) return 1;
    if (*(pbt-25)==4) return 1;
    if (*(pbt-14)==4) return 1;    
  }

  if (*(pbt-11)==5) return 1;
  if (*(pbt-13)==5) return 1;

  return 0;
}


int makewking()
{
  long *ii;
  register unsigned char *pbt;
  unsigned char *pbtx,fesselung;
  register signed char *pbt2;
  signed char *pbt2x;
  int i,wch1;

  fesselung = 0;
  rook_bits=0;
  bishop_bits=0;

  if (fln[11]>0)
  {
    for (i=0; i<fln[11]; i++)
      bishop_bits =  bishop_bits | bishop120[flc[11][i]];
  }
  if (fln[12]>0)
  {
    for (i=0; i<fln[12]; i++)
      rook_bits =  rook_bits | rook120[flc[12][i]];
  }
  if (fln[13]>0)
  {
    for (i=0; i<fln[13]; i++)
    {    
      bishop_bits =  bishop_bits | bishop120[flc[13][i]];
      rook_bits =  rook_bits | rook120[flc[13][i]];
    }
  }
  
  pbt=cb.cc+flc[6][0];   /* Pointer auf Position des Koenigs */
  pbt2=wb.cc+flc[6][0];
  pbtx=pbt;
  pbt2x=pbt2;
  wch1=0;

  ii=wb.ii;
  ii[ 7]=0; ii[ 8]=0;
  ii[10]=0; ii[11]=0;
  ii[13]=0; ii[14]=0;
  ii[16]=0; ii[17]=0;
  ii[19]=0; ii[20]=0;
  ii[22]=0; ii[23]=0;
  ii[25]=0; ii[26]=0;
  ii[28]=0; ii[29]=0;
    
  /* Die Geraden absuchen */
  if ((rook_bits & bit120[flc[6][0]])!=0)
  {
    pbt=pbtx;
    do
    {
      pbt+=1;
    } while (*pbt == 0);      // Erste Figur in Zugrichtung
    if (*pbt<6)               // Weisse Figur
    {
      do
      {
        pbt+=1;
      } while (*pbt == 0);    // Erste Figur nach der weissen 
      if (*pbt>11)            //  Turm oder Dame fesselt
      {
        fesselung=1;
        pbt=pbtx;pbt2=pbt2x;  // Erste Weisse wieder suchen
        do
        {
          pbt+=1; pbt2+=1;
        } while (*pbt == 0);
        *pbt2=1;               // Fesselungs-Richtung merken
        if ((*pbt==5)||(*pbt==3)) *pbt2=32; // Figur kann gar nicht ziehen
      }
    }
    else if (*pbt>11)          // Schwarzer Turm, Dame gibt Schach
    {
      pbt=pbtx;pbt2=pbt2x+1;
      do
      {
        *pbt2=2; pbt+=1; pbt2+=1;
      } while (*pbt == 0);     // Schachlinie merken
      wch1++;
    }

    pbt=pbtx;
    do
    {
      pbt-=1;
    } while (*pbt == 0);
    if (*pbt<6)
    {
      do
      {
        pbt-=1;
      } while (*pbt == 0);
      if (*pbt>11)
      {
        fesselung=1;
        pbt=pbtx;pbt2=pbt2x;
        do
        {
          pbt-=1; pbt2-=1;
        } while (*pbt == 0);
        *pbt2=-1;
        if ((*pbt==5)||(*pbt==3)) *pbt2=32;
      }
    }
    else if (*pbt>11)
    {
      pbt=pbtx;pbt2=pbt2x-1;
      do
      {
        *pbt2=2; pbt-=1; pbt2-=1;
      } while (*pbt == 0);
      wch1++;
    }

    pbt=pbtx;
    do
    {
      pbt+=12;
    } while (*pbt == 0);
    if (*pbt<6)
    {
      do
      {
        pbt+=12;
      } while (*pbt == 0);
      if (*pbt>11)
      {
        fesselung=1;
        pbt=pbtx;pbt2=pbt2x;
        do
        {
          pbt+=12; pbt2+=12;
        } while (*pbt == 0);
        *pbt2=12;
        if (*pbt==3) *pbt2=32;
      }
    }
    else if (*pbt>11)
    {
      pbt=pbtx;pbt2=pbt2x+12;
      do
      {
        *pbt2=2; pbt+=12; pbt2+=12;
      } while (*pbt == 0);
      wch1++;
    }

    pbt=pbtx;
    do
    {
      pbt-=12;
    } while (*pbt == 0);
    if (*pbt<6)
    {
      do
      {
        pbt-=12;
      } while (*pbt == 0);
      if (*pbt>11)
      {
        fesselung=1;
        pbt=pbtx;pbt2=pbt2x;
        do
        {
          pbt-=12; pbt2-=12;
        } while (*pbt == 0);
        *pbt2=-12;
        if (*pbt==3) *pbt2=32;
      }
    }
    else if (*pbt>11)
    {
      pbt=pbtx;pbt2=pbt2x-12;
      do
      {
        *pbt2=2; pbt-=12; pbt2-=12;
      } while (*pbt == 0);
      wch1++;
    }
  }

  // Die Diagonalen absuchen
  if ((bishop_bits & bit120[flc[6][0]])!=0)
  {
    pbt=pbtx;
    do
    {
      pbt+=11;
    } while (*pbt == 0);
    if (*pbt<6)
    {
      do
      {
        pbt+=11;
      } while (*pbt == 0);
      if ((*pbt>10)&&(*pbt!=12))
      {
        fesselung=1;
        pbt=pbtx;pbt2=pbt2x;
        do
        {
          pbt+=11; pbt2+=11;
        } while (*pbt == 0);
        *pbt2=11;
        if (*pbt==2) *pbt2=32;
        if ((*pbt==5)&&(*(pbt+11)==0)) *pbt2=32;
      }
    }
    else if ((*pbt>10)&&(*pbt!=12))
    {
      pbt=pbtx;pbt2=pbt2x+11;
      do
      {
        *pbt2=2; pbt+=11; pbt2+=11;
      } while (*pbt == 0);
      wch1++;
    }

    pbt=pbtx;
    do
    {
      pbt+=13;
    } while (*pbt == 0);
    if (*pbt<6)
    {
      do
      {
        pbt+=13;
      } while (*pbt == 0);
      if ((*pbt>10)&&(*pbt!=12))
      {
        fesselung=1;
        pbt=pbtx;pbt2=pbt2x;
        do
        {
          pbt+=13; pbt2+=13;
        } while (*pbt == 0);
        *pbt2=13;
        if (*pbt==2) *pbt2=32;
        if ((*pbt==5)&&(*(pbt+13)==0)) *pbt2=32;
      }
    }
    else if ((*pbt>10)&&(*pbt!=12))
    {
      pbt=pbtx;pbt2=pbt2x+13;
      do
      {
        *pbt2=2; pbt+=13; pbt2+=13;
      } while (*pbt == 0);
      wch1++;
    }

    pbt=pbtx;
    do
    {
      pbt-=11;
    } while (*pbt == 0);
    if (*pbt<6)
    {
      do
      {
        pbt-=11;
      } while (*pbt == 0);
      if ((*pbt>10)&&(*pbt!=12))
      {
        fesselung=1;
        pbt=pbtx;pbt2=pbt2x;
        do
        {
          pbt-=11; pbt2-=11;
        } while (*pbt == 0);
        *pbt2=-11;
        if ((*pbt==2)||(*pbt==5)) *pbt2=32;
      }
    }
    else if ((*pbt>10)&&(*pbt!=12))
    {
      pbt=pbtx;pbt2=pbt2x-11;
      do
      {
        *pbt2=2; pbt-=11; pbt2-=11;
      } while (*pbt == 0);
      wch1++;
    }

    pbt=pbtx;
    do
    {
      pbt-=13;
    } while (*pbt == 0);
    if (*pbt<6)
    {
      do
      {
        pbt-=13;
      } while (*pbt == 0);
      if ((*pbt>10)&&(*pbt!=12))
      {
        fesselung=1;
        pbt=pbtx;pbt2=pbt2x;
        do
        {
          pbt-=13; pbt2-=13;
        } while (*pbt == 0);
        *pbt2=-13;
        if ((*pbt==2)||(*pbt==5)) *pbt2=32;
      }
    }
    else if ((*pbt>10)&&(*pbt!=12))
    {
      pbt=pbtx;pbt2=pbt2x-13;
      do
      {
        *pbt2=2; pbt-=13; pbt2-=13;
      } while (*pbt == 0);
      wch1++;
    }
  }
  pbt=pbtx;pbt2=pbt2x;

  if (lsm==10)
  {
  // Alternative Springerpruefung; im damaligen Test ohne deutlichen Zeitgewinn.
  //  if (knight_attack[pos[g-1].b-wk+144]!=0)
  //  {
  //    wb.cc[pos[g-1].b]=2;
  //    wch1++;
  //  }
      
    if (*(pbt+10)==10)
    {
      *(pbt2+10)=2;
      wch1++;
    }
    if (*(pbt+23)==10)
    {
      *(pbt2+23)=2;
      wch1++;
    }
    if (*(pbt+25)==10)
    {
      *(pbt2+25)=2;
      wch1++;
    }
    if (*(pbt+14)==10)
    {
      *(pbt2+14)=2;
      wch1++;
    }
    if (*(pbt-10)==10)
    {
      *(pbt2-10)=2;
      wch1++;
    }
    if (*(pbt-23)==10)
    {
      *(pbt2-23)=2;
      wch1++;
    }
    if (*(pbt-25)==10)
    {
      *(pbt2-25)=2;
      wch1++;
    }
    if (*(pbt-14)==10)
    {
      *(pbt2-14)=2;
      wch1++;
    }
  }

  if (*(pbtx+11)==9)
  {
    *(pbt2+11)=2;
    wch1++;
  }
  if (*(pbtx+13)==9)
  {
    *(pbt2+13)=2;
    wch1++;
  }

  pos[g].fesselung=fesselung;
  pos[g].wch1=wch1;
  return wch1;
}

int makesking()
{
  long *ii;
  register unsigned char *pbt;
  unsigned char *pbtx,fesselung;
  register signed char *pbt2;
  signed char *pbt2x;
  int i,sch1;
  
  fesselung=0;
  rook_bits=0;
  bishop_bits=0;

  if (fln[3]>0)
  {
    for (i=0; i<fln[3]; i++)
      bishop_bits =  bishop_bits | bishop120[flc[3][i]];
  }
  if (fln[2]>0)
  {
    for (i=0; i<fln[2]; i++)
      rook_bits =  rook_bits | rook120[flc[2][i]];
  }
  if (fln[1]>0)
  {
    for (i=0; i<fln[1]; i++)
    {    
      bishop_bits =  bishop_bits | bishop120[flc[1][i]];
      rook_bits =  rook_bits | rook120[flc[1][i]];
    }
  }
  
  pbt=cb.cc+flc[8][0];   /* Pointer auf Position des Koenigs */
  pbt2=sb.cc+flc[8][0];
  pbtx=pbt;
  pbt2x=pbt2;
  sch1=0;

  ii=sb.ii;
  ii[ 7]=0; ii[ 8]=0;
  ii[10]=0; ii[11]=0;
  ii[13]=0; ii[14]=0;
  ii[16]=0; ii[17]=0;
  ii[19]=0; ii[20]=0;
  ii[22]=0; ii[23]=0;
  ii[25]=0; ii[26]=0;
  ii[28]=0; ii[29]=0;

  /* Die Geraden absuchen */
  if ((rook_bits & bit120[flc[8][0]])!=0)
  {
    pbt=pbtx;
    do
    {
      pbt+=1;
    } while (*pbt == 0);    /* Erste Figur in Zugrichtung */
    if (*pbt>8)                       /* Schwarze Figur */
    {
      do
      {
        pbt+=1;
      } while (*pbt == 0);  /* Erste Figur nach der Schwarzen */
      if (*pbt<3)                     /* Turm oder Dame fesselt */
      {
        fesselung=1;
        pbt=pbtx;pbt2=pbt2x;          /* Erste Schwarze wieder suchen */
        do
        {
          pbt+=1; pbt2+=1;
        } while (*pbt == 0);
        *pbt2=1;                      /* Fesselungs-Richtung merken */
        if ((*pbt==9)||(*pbt==11)) *pbt2=32; /* Figur kann gar nicht ziehen */
      }
    }
    else if (*pbt<3)                 /* Weisser Turm, Dame gibt Schach */
    {
      pbt=pbtx;pbt2=pbt2x+1;
      do
      {
        *pbt2=2; pbt+=1; pbt2+=1;
      } while (*pbt == 0); /* Schachlinie merken */
      sch1++;
    }

    pbt=pbtx;
    do
    {
      pbt-=1;
    } while (*pbt == 0);
    if (*pbt>8)
    {
      do
      {
        pbt-=1;
      } while (*pbt == 0);
      if (*pbt<3)
      {
        fesselung=1;
        pbt=pbtx;pbt2=pbt2x;
        do
        {
          pbt-=1; pbt2-=1;
        } while (*pbt == 0);
        *pbt2=-1;
        if ((*pbt==9)||(*pbt==11)) *pbt2=32;
      }
    }
    else if (*pbt<3)
    {
      pbt=pbtx;pbt2=pbt2x-1;
      do
      {
        *pbt2=2; pbt-=1; pbt2-=1;
      } while (*pbt == 0);
      sch1++;
    }

    pbt=pbtx;
    do
    {
      pbt+=12;
    } while (*pbt == 0);
    if (*pbt>8)
    {
      do
      {
        pbt+=12;
      } while (*pbt == 0);
      if (*pbt<3)
      {
        fesselung=1;
        pbt=pbtx;pbt2=pbt2x;
        do
        {
          pbt+=12; pbt2+=12;
        } while (*pbt == 0);
        *pbt2=12;
        if (*pbt==11) *pbt2=32;
      }
    }
    else if (*pbt<3)
    {
      pbt=pbtx;pbt2=pbt2x+12;
      do
      {
        *pbt2=2; pbt+=12; pbt2+=12;
      } while (*pbt == 0);
      sch1++;
    }

    pbt=pbtx;
    do
    {
      pbt-=12;
    } while (*pbt == 0);
    if (*pbt>8)
    {
      do
      {
        pbt-=12;
      } while (*pbt == 0);
      if (*pbt<3)
      {
        fesselung=1;
        pbt=pbtx;pbt2=pbt2x;
        do
        {
          pbt-=12; pbt2-=12;
        } while (*pbt == 0);
        *pbt2=-12;
        if (*pbt==11) *pbt2=32;
      }
    }
    else if (*pbt<3)
    {
      pbt=pbtx;pbt2=pbt2x-12;
      do
      {
        *pbt2=2; pbt-=12; pbt2-=12;
      } while (*pbt == 0);
      sch1++;
    }
  }
  /* Die Diagonalen absuchen */
  if ((bishop_bits & bit120[flc[8][0]])!=0)
  {
    pbt=pbtx;
    do
    {
      pbt+=11;
    } while (*pbt == 0);
    if (*pbt>8)
    {
      do
      {
        pbt+=11;
      } while (*pbt == 0);
      if ((*pbt<4)&&(*pbt!=2))
      {
        fesselung=1;
        pbt=pbtx;pbt2=pbt2x;
        do
        {
          pbt+=11; pbt2+=11;
        } while (*pbt == 0);
        *pbt2=11;
        if ((*pbt==12)||(*pbt==9)) *pbt2=32;
      }
    }
    else if ((*pbt<4)&&(*pbt!=2))
    {
      pbt=pbtx;pbt2=pbt2x+11;
      do
      {
        *pbt2=2; pbt+=11; pbt2+=11;
      } while (*pbt == 0);
      sch1++;
    }

    pbt=pbtx;
    do
    {
      pbt+=13;
    } while (*pbt == 0);
    if (*pbt>8)
    {
      do
      {
        pbt+=13;
      } while (*pbt == 0);
      if ((*pbt<4)&&(*pbt!=2))
      {
        fesselung=1;
        pbt=pbtx;pbt2=pbt2x;
        do
        {
          pbt+=13; pbt2+=13;
        } while (*pbt == 0);
        *pbt2=13;
        if ((*pbt==12)||(*pbt==9)) *pbt2=32;
      }
    }
    else if ((*pbt<4)&&(*pbt!=2))
    {
      pbt=pbtx;pbt2=pbt2x+13;
      do
      {
        *pbt2=2; pbt+=13; pbt2+=13;
      } while (*pbt == 0);
      sch1++;
    }

    pbt=pbtx;
    do
    {
      pbt-=11;
    } while (*pbt == 0);
    if (*pbt>8)
    {
      do
      {
        pbt-=11;
      } while (*pbt == 0);
      if ((*pbt<4)&&(*pbt!=2))
      {
        fesselung=1;
        pbt=pbtx;pbt2=pbt2x;
        do
        {
          pbt-=11; pbt2-=11;
        } while (*pbt == 0);
        *pbt2=-11;
        if (*pbt==12) *pbt2=32;
        if ((*pbt==9)&&(*(pbt-11)==0)) *pbt2=32;
      }
    }
    else if ((*pbt<4)&&(*pbt!=2))
    {
      pbt=pbtx;pbt2=pbt2x-11;
      do
      {
        *pbt2=2; pbt-=11; pbt2-=11;
      } while (*pbt == 0);
      sch1++;
    }

    pbt=pbtx;
    do
    {
      pbt-=13;
    } while (*pbt == 0);
    if (*pbt>8)
    {
      do
      {
        pbt-=13;
      } while (*pbt == 0);
      if ((*pbt<4)&&(*pbt!=2))
      {
        fesselung=1;
        pbt=pbtx;pbt2=pbt2x;
        do
        {
          pbt-=13; pbt2-=13;
        } while (*pbt == 0);
        *pbt2=-13;
        if (*pbt==12) *pbt2=32;
        if ((*pbt==9)&&(*(pbt-13)==0)) *pbt2=32;
      }
    }
    else if ((*pbt<4)&&(*pbt!=2))
    {
      pbt=pbtx;pbt2=pbt2x-13;
      do
      {
        *pbt2=2; pbt-=13; pbt2-=13;
      } while (*pbt == 0);
      sch1++;
    }
  }
  pbt=pbtx;pbt2=pbt2x;

  if (lwm==4)
  {
  // Alternative Springerpruefung; im damaligen Test ohne deutlichen Zeitgewinn.
  //  if (knight_attack[pos[g-1].b-sk+144]!=0)
  //  {
  //    sb.cc[pos[g-1].b]=2;
  //    sch1++;
  //  }

    if (*(pbt+10)==4)
    {
      *(pbt2+10)=2;
      sch1++;
    }
    if (*(pbt+23)==4)
    {
      *(pbt2+23)=2;
      sch1++;
    }
    if (*(pbt+25)==4)
    {
      *(pbt2+25)=2;
      sch1++;
    }
    if (*(pbt+14)==4)
    {
      *(pbt2+14)=2;
      sch1++;
    }
    if (*(pbt-10)==4)
    {
      *(pbt2-10)=2;
      sch1++;
    }
    if (*(pbt-23)==4)
    {
      *(pbt2-23)=2;
      sch1++;
    }
    if (*(pbt-25)==4)
    {
      *(pbt2-25)=2;
      sch1++;
    }
    if (*(pbt-14)==4)
    {
      *(pbt2-14)=2;
      sch1++;
    }   
  }

  if (*(pbtx-11)==5)
  {
    *(pbt2-11)=2;
    sch1++;
  }
  if (*(pbtx-13)==5)
  {
    *(pbt2-13)=2;
    sch1++;
  }

  pos[g].fesselung=fesselung;
  pos[g].sch1=sch1;
  return sch1;
}

// Zur Vorbereitung der Ermittlung, ob weisse Figuren:
// 1.) Schachlinien aufdecken koennen
// 2.) Selber schachbieten koennen
int makesking_check()
{
  long *ii;
  register unsigned char *pbt;
  unsigned char *pbtx;
  register signed char *pbt2;
  signed char *pbt2x;
  int i,f,sch1;
    
  pbt=cb.cc+flc[8][0];   /* Pointer auf Position des Koenigs */
  pbt2=sb2.cc+flc[8][0];
  pbtx=pbt;
  pbt2x=pbt2;
  sch1=0;

  ii=sb2.ii;
  ii[ 7]=0; ii[ 8]=0;
  ii[10]=0; ii[11]=0;
  ii[13]=0; ii[14]=0;
  ii[16]=0; ii[17]=0;
  ii[19]=0; ii[20]=0;
  ii[22]=0; ii[23]=0;
  ii[25]=0; ii[26]=0;
  ii[28]=0; ii[29]=0;

  for (i=4; i<8; i++)
  {
    f=quenmove_c[i];

    pbt=pbtx;
    pbt2 = pbt2x;
  
    do
    {
      pbt+=f;
      *pbt2=1;              // Hier moegliches Turm/Dame Schach
      pbt2+=f;
    } while (*pbt == 0);    // Erste Figur in Zugrichtung
  
    if (*pbt<7)            // Weisse Figur
    {
      do
      {
        pbt+=f;
      } while (*pbt == 0);  // Figur nach der ersten Figur      

      if (*pbt<3)           // Turm, Dame
      {
        *pbt2=5;            // Erste weisse Figur kann durch wegziehen Schach geben
      }
    }
    else
    {
      if (*pbt>7) *pbt2=1;  // Schach durch Schlagen dieser schwarzen Figur moeglich              
    }
  }

  for (i=0; i<4; i++)
  {
    f=quenmove_c[i];

    pbt=pbtx;
    pbt2 = pbt2x;
  
    do
    {
      pbt+=f;
      *pbt2=2;              // Hier moegliches Lauefer/Dame Schach
      pbt2+=f;
    } while (*pbt == 0);    // Erste Figur in Zugrichtung
  
    if (*pbt<7)            // Weisse Figur
    {
      do
      {
        pbt+=f;
      } while (*pbt == 0);  // Figur nach der ersten Figur      

      if ((*pbt<4)&&(*pbt!=2))           // Lauefer, Dame
      {
        *pbt2=5;            // Erste weisse Figur kann durch wegziehen Schach geben
      }
    }
    else
    {
      if (*pbt>7) *pbt2=2;  // Schach durch Schlagen dieser schwarzen Figur moeglich              
    }
  }
  
  if (fln[4]>0)
  {
    pbt2=pbt2x;

    *(pbt2+10)=7;
    *(pbt2+23)=7;
    *(pbt2+25)=7;
    *(pbt2+14)=7;
    *(pbt2-10)=7;
    *(pbt2-23)=7;
    *(pbt2-25)=7;
    *(pbt2-14)=7;
  }

  return 0;    
}

// Ueberpruefen, ob ein weisser Zug (wahrscheinlich) Schach gibt
int check_w_move(int p1, int b)
{
  int a;
  unsigned char c,d,z,*pbt;  
    
  z=p1>>8;
  if ((z>0)&&(z!=16)) return 1; // Spezial-Zug, en passant, Umwandlung, Rochade
                                // aber kein Bauer - Doppelschritt
  pbt=sb2.cc;

  p1=p1&255;                    /* char z aus int p1 entfernen */
  a=*(flp+p1);                  /* Brett-Position aus Figurenliste */

  if (*(pbt+a)==5) return 1;    // Figur kann durch wegziehen Schach geben

  c=cb.cc[a];                   /* Figurenart */
  d=*(pbt+b);                   /* sb2-Zielfeld */
  
  if (c<4)
  {
    if (c==1)
    {
      if ((d==1)||(d==2)) return 1;           
    }
    else
    {
      if (c==2)
      {
        if (d==1) return 1;
      }
      else
      {
        if (d==2) return 1;
      }
    }        
  }
  else
  {
    if (c==4) // Springer
    {
      if (d==7) return 1;
    }
    else  // Bauer
    {
      pbt = cb.cc + b; // Zeiger auf Zielfeld Bauernzug (keine Umwandlung)
            
      if (*(pbt+11)==8) return 1;
      if (*(pbt+13)==8) return 1;    
    }
  } 
  
  return 0;
}


/*****************************************************************************/

void make_w_move_h(int p1, int b)
{
  int a,g1;
  unsigned char c,d,z,*pbt,*pbt2;
                                        
  g1 = g+1;
  pos[g1].hash_doppel=0;       // Keinen Doppelschritt hashen
  pbt=cb.cc;
  
  z=p1>>8;                      /* Kennzeichen fuer Umwandlung ,ep */
  p1=p1&255;                    /* char z aus int p1 entfernen */
  a=*(flp+p1);                  /* Brett-Position aus Figurenliste */
  *(flp+p1)=b;                  /* Neue Position der Figur */
  c=*(pbt+a);                   /* Figurenart */
  *(pbt+a)=0;                   /* Nach Zug ist das Feld leer */
  d=*(pbt+b);                   /* Inhalt des Ziel-Feldes */
  *(pbt+b)=c;                   /* Ziel-Feld = ziehende Figurenart */
  lwm=c;                        /* Figurenart merken */    

  pos[g1].hashkey = pos[g].hashkey ^ zobrist[c][a] ^ zobrist[c][b] ^ (uint64)-1;
 
  pos[g1].wr1=pos[g].wr1;  
  if (pos[g1].wr1!=0)
  {    
    if ((c==6)||(z>119))
    {        
        if (((pos[g1].wr1) & (unsigned char)2) > 0)
          pos[g1].hashkey = pos[g1].hashkey ^ zobrist_rochade[1];        
        if (((pos[g1].wr1) & (unsigned char)1) > 0)
          pos[g1].hashkey = pos[g1].hashkey ^ zobrist_rochade[0];        
        pos[g1].wr1=0;    // Koenigszug, Rochade
    }
    else
    {
      if (a==28)
      {
        if (((pos[g1].wr1) & (unsigned char)2) > 0)
        {          
          pos[g1].wr1 &= 0xFD; // 2: Grosse Rochade ausblenden
          pos[g1].hashkey = pos[g1].hashkey ^ zobrist_rochade[1];
        }
      }
      if (a==35)
      {
        if (((pos[g1].wr1) & (unsigned char)1) > 0)
        {          
          pos[g1].wr1 &= 0xFE; // 1: Kleine Rochade ausblenden
          pos[g1].hashkey = pos[g1].hashkey ^ zobrist_rochade[0];
        }
      }
    }
  }

  pos[g1].sr1=pos[g].sr1;  
  if (pos[g1].sr1!=0)
  { 
    if (b==112)
    {
      if (((pos[g1].sr1) & (unsigned char)2) > 0)
      {
        pos[g1].sr1 &= 0xFD; // 2: Grosse Rochade ausblenden
        pos[g1].hashkey = pos[g1].hashkey ^ zobrist_rochade[3];
      }  
    }
    if (b==119)
    {
      if (((pos[g1].sr1) & (unsigned char)1) > 0)
      {
        pos[g1].sr1 &= 0xFE; // 1: Kleine Rochade ausblenden
        pos[g1].hashkey = pos[g1].hashkey ^ zobrist_rochade[2];
      }
    }
  }

  if (z!=0)
  {
    if (z==16)
    {
      z=0;
      lwm=b;                    /* Endposition des Doppelschrittes */
      if ((*(pbt+b+1)==9)||(*(pbt+b-1)==9)) pos[g1].hash_doppel=lwm;   
    }
    else
    {
      if (z<16)
      {
        lwm=z;                    /* Figurenart merken */
        *(pbt+b)=z;               /* Umwandlungsfigur auf Feld */
        fln[5]--;                 /* Anzahl Bauern minus 1 */
        *(flp+p1)=flc[5][fln[5]]; /* Letzten Bauern an Stelle des umgew. */
        flc[z][fln[z]]=b;         /* Neue Figur an Liste anfuegen */
        fln[z]++;                 /* Anzahl Figuren der Neuen erhoehen */
        pos[g1].hashkey = pos[g1].hashkey ^ zobrist[5][b] ^ zobrist[z][b];
      }
      if (z>27)
      {
        if (z>119)                 // Rochade Koenigszug
        {
          *(pbt+32)=0;
          if (z==120)
          {
            *(pbt+34)=6;
            flc[6][0]=34;
          }
          else
          {
            *(pbt+30)=6;
            flc[6][0]=30;
          }
        
          pos[g1].hashkey = pos[g1].hashkey ^ zobrist[6][32] ^ zobrist[6][flc[6][0]];
        }
        else
        {
          fln[9]--;                  /* schwarzer Bauer ep geschlagen */
          pbt2=flc[9];               /* Ende Liste schwarzer Bauern */
          while (*pbt2 != z) pbt2++; /* geschlagenen Bauern suchen */
          *pbt2=*(flc[9]+fln[9]);    /* Letzten Bauern an Pos. des geschl. */
          *(pbt+b-12)=0;             /* Feld des geschl. Bauern loeschen */
          pos[g].pbt2=pbt2;
          pos[g1].hashkey ^= zobrist[9][b-12];
        }
      }
    }
  }
  
  if (d !=0)
  {
    fln[d]--;                          // geschlagene Figurenart -1
    pbt2=flc[d];
    while (*pbt2 != b) pbt2++;        // geschlagende Figur suchen
    *pbt2=*(flc[d]+fln[d]);
    pos[g].pbt2=pbt2;                  // Urspruengliche Listenposition der geschlagenen Figur merken
    pos[g1].hashkey ^= zobrist[d][b];
  }

  pos[g].a=a;
  pos[g].b=b;
  pos[g].d=d;
  pos[g].z=z;
  pos[g].p1=p1;
  g++;
  pos[g].lwm=lwm;
}


void make_w_move_h_nor(int p1, int b)
{
  int a,g1;
  unsigned char c,d,z,*pbt,*pbt2;
                                        
  g1 = g+1;
  pos[g1].hash_doppel=0;       // Keinen Doppelschritt hashen
  pbt=cb.cc;
  
  z=p1>>8;                      /* Kennzeichen fuer Umwandlung ,ep */
  p1=p1&255;                    /* char z aus int p1 entfernen */
  a=*(flp+p1);                  /* Brett-Position aus Figurenliste */
  *(flp+p1)=b;                  /* Neue Position der Figur */
  c=*(pbt+a);                   /* Figurenart */
  *(pbt+a)=0;                   /* Nach Zug ist das Feld leer */
  d=*(pbt+b);                   /* Inhalt des Ziel-Feldes */
  *(pbt+b)=c;                   /* Ziel-Feld = ziehende Figurenart */
  lwm=c;                        /* Figurenart merken */    

  pos[g1].hashkey = pos[g].hashkey ^ zobrist[c][a] ^ zobrist[c][b] ^ (uint64)-1;
 
  pos[g1].sr1=0;
  pos[g1].wr1=0;  
  if (z!=0)
  {
    if (z==16)
    {
      z=0;
      lwm=b;                    /* Endposition des Doppelschrittes */
      if ((*(pbt+b+1)==9)||(*(pbt+b-1)==9)) pos[g1].hash_doppel=lwm;   
    }
    else
    {
      if (z<16)
      {
        lwm=z;                    /* Figurenart merken */
        *(pbt+b)=z;               /* Umwandlungsfigur auf Feld */
        fln[5]--;                 /* Anzahl Bauern minus 1 */
        *(flp+p1)=flc[5][fln[5]]; /* Letzten Bauern an Stelle des umgew. */
        flc[z][fln[z]]=b;         /* Neue Figur an Liste anfuegen */
        fln[z]++;                 /* Anzahl Figuren der Neuen erhoehen */
        pos[g1].hashkey = pos[g1].hashkey ^ zobrist[5][b] ^ zobrist[z][b];
      }
      if (z>27)
      {        
        fln[9]--;                  /* schwarzer Bauer ep geschlagen */
        pbt2=flc[9];               /* Ende Liste schwarzer Bauern */
        while (*pbt2 != z) pbt2++; /* geschlagenen Bauern suchen */
        *pbt2=*(flc[9]+fln[9]);    /* Letzten Bauern an Pos. des geschl. */
        *(pbt+b-12)=0;             /* Feld des geschl. Bauern loeschen */
        pos[g].pbt2=pbt2;
        pos[g1].hashkey ^= zobrist[9][b-12];        
      }
    }
  }
  
  if (d !=0)
  {
    fln[d]--;                          // geschlagene Figurenart -1
    pbt2=flc[d];
    while (*pbt2 != b) pbt2++;        // geschlagende Figur suchen
    *pbt2=*(flc[d]+fln[d]);
    pos[g].pbt2=pbt2;                  // Urspruengliche Listenposition der geschlagenen Figur merken
    pos[g1].hashkey ^= zobrist[d][b];
  }

  pos[g].a=a;
  pos[g].b=b;
  pos[g].d=d;
  pos[g].z=z;
  pos[g].p1=p1;
  g++;
  pos[g].lwm=lwm;
}


void make_w_move_h_hashonly(int p1, int b)
{
  int a,g1;
  unsigned char c,d,z,*pbt;
                                        
  g1 = g+1;
  pos[g1].hash_doppel=0;       // Keinen Doppelschritt hashen
  pbt=cb.cc;
  
  z=p1>>8;                      /* Kennzeichen fuer Umwandlung ,ep */
  p1=p1&255;                    /* char z aus int p1 entfernen */
  a=*(flp+p1);                  /* Brett-Position aus Figurenliste */
  c=*(pbt+a);                   /* Figurenart */
  d=*(pbt+b);                   /* Inhalt des Ziel-Feldes */
  lwm=c;                        /* Figurenart merken */    

  pos[g1].hashkey = pos[g].hashkey ^ zobrist[c][a] ^ zobrist[c][b] ^ (uint64)-1;
 
  pos[g1].wr1=pos[g].wr1;  
  if (pos[g1].wr1!=0)
  {    
    if ((c==6)||(z>119))
    {        
        if (((pos[g1].wr1) & (unsigned char)2) > 0)
          pos[g1].hashkey = pos[g1].hashkey ^ zobrist_rochade[1];        
        if (((pos[g1].wr1) & (unsigned char)1) > 0)
          pos[g1].hashkey = pos[g1].hashkey ^ zobrist_rochade[0];        
    }
    else
    {
      if (a==28)
      {
        if (((pos[g1].wr1) & (unsigned char)2) > 0)
        {          
          pos[g1].hashkey = pos[g1].hashkey ^ zobrist_rochade[1];
        }
      }
      if (a==35)
      {
        if (((pos[g1].wr1) & (unsigned char)1) > 0)
        {          
          pos[g1].hashkey = pos[g1].hashkey ^ zobrist_rochade[0];
        }
      }
    }
  }

  pos[g1].sr1=pos[g].sr1;  
  if (pos[g1].sr1!=0)
  { 
    if (b==112)
    {
      if (((pos[g1].sr1) & (unsigned char)2) > 0)
      {
        pos[g1].hashkey = pos[g1].hashkey ^ zobrist_rochade[3];
      }  
    }
    if (b==119)
    {
      if (((pos[g1].sr1) & (unsigned char)1) > 0)
      {
        pos[g1].hashkey = pos[g1].hashkey ^ zobrist_rochade[2];
      }
    }
  }

  if (z!=0)
  {
    if (z==16)
    {
      z=0;
      lwm=b;                    /* Endposition des Doppelschrittes */
      if ((*(pbt+b+1)==9)||(*(pbt+b-1)==9)) pos[g1].hash_doppel=lwm;   
    }
    else
    {
      if (z<16)
      {
        lwm=z;                    /* Figurenart merken */
        pos[g1].hashkey = pos[g1].hashkey ^ zobrist[5][b] ^ zobrist[z][b];
      }
      if (z>27)
      {
        if (z>119)                 // Rochade Koenigszug
        {
          if (z==120)
          {
            pos[g1].hashkey = pos[g1].hashkey ^ zobrist[6][32] ^ zobrist[6][34];
          }
          else
          {
            pos[g1].hashkey = pos[g1].hashkey ^ zobrist[6][32] ^ zobrist[6][30];
          }        
        }
        else
        {
           pos[g1].hashkey ^= zobrist[9][b-12];
        }
      }
    }
  }
  
  if (d !=0)
    pos[g1].hashkey ^= zobrist[d][b];

  g++;
}


void make_w_move_h_nor_hashonly(int p1, int b)
{
  int a,g1;
  unsigned char c,d,z,*pbt;
                                        
  g1 = g+1;
  pos[g1].hash_doppel=0;       // Keinen Doppelschritt hashen
  pbt=cb.cc;
  
  z=p1>>8;                      /* Kennzeichen fuer Umwandlung ,ep */
  p1=p1&255;                    /* char z aus int p1 entfernen */
  a=*(flp+p1);                  /* Brett-Position aus Figurenliste */
  c=*(pbt+a);                   /* Figurenart */
  d=*(pbt+b);                   /* Inhalt des Ziel-Feldes */
  lwm=c;                        /* Figurenart merken */    

  pos[g1].hashkey = pos[g].hashkey ^ zobrist[c][a] ^ zobrist[c][b] ^ (uint64)-1;
 
  pos[g1].sr1=0;
  pos[g1].wr1=0;  
  if (z!=0)
  {
    if (z==16)
    {
      z=0;
      lwm=b;                    /* Endposition des Doppelschrittes */
      if ((*(pbt+b+1)==9)||(*(pbt+b-1)==9)) pos[g1].hash_doppel=lwm;   
    }
    else
    {
      if (z<16)
      {
        lwm=z;                    /* Figurenart merken */
        pos[g1].hashkey = pos[g1].hashkey ^ zobrist[5][b] ^ zobrist[z][b];
      }
      if (z>27)
      {        
        pos[g1].hashkey ^= zobrist[9][b-12];        
      }
    }
  }
  
  if (d !=0)  
    pos[g1].hashkey ^= zobrist[d][b];

  g++;
}


void make_w_move(int p1, int b)
{
  int a,g1;
  unsigned char c,d,z,*pbt,*pbt2;
                                        
  g1 = g+1;
  pbt=cb.cc;
  
  z=p1>>8;                      /* Kennzeichen fuer Umwandlung ,ep */
  p1=p1&255;                    /* char z aus int p1 entfernen */
  a=*(flp+p1);                  /* Brett-Position aus Figurenliste */
  *(flp+p1)=b;                  /* Neue Position der Figur */
  c=*(pbt+a);                   /* Figurenart */
  *(pbt+a)=0;                   /* Nach Zug ist das Feld leer */
  d=*(pbt+b);                   /* Inhalt des Ziel-Feldes */
  *(pbt+b)=c;                   /* Ziel-Feld = ziehende Figurenart */
  lwm=c;                        /* Figurenart merken */    
 
  pos[g1].wr1=pos[g].wr1;  
  if (pos[g1].wr1!=0)
  {    
    if ((c==6)||(z>119))
    {        
      pos[g1].wr1=0;    // Koenigszug, Rochade
    }
    else
    {
      if (a==28)
      {
        if (((pos[g1].wr1) & (unsigned char)2) > 0)                 
          pos[g1].wr1 &= 0xFD; // 2: Grosse Rochade ausblenden                 
      }
      if (a==35)
      {
        if (((pos[g1].wr1) & (unsigned char)1) > 0)
          pos[g1].wr1 &= 0xFE; // 1: Kleine Rochade ausblenden
      }
    }
  }

  pos[g1].sr1=pos[g].sr1;  
  if (pos[g1].sr1!=0)
  { 
    if (b==112)
    {
      if (((pos[g1].sr1) & (unsigned char)2) > 0)
        pos[g1].sr1 &= 0xFD; // 2: Grosse Rochade ausblenden
    }
    if (b==119)
    {
      if (((pos[g1].sr1) & (unsigned char)1) > 0)      
        pos[g1].sr1 &= 0xFE; // 1: Kleine Rochade ausblenden
    }
  }

  if (z!=0)
  {
    if (z==16)
    {
      z=0;
      lwm=b;                    /* Endposition des Doppelschrittes */
    }
    else
    {
      if (z<16)
      {
        lwm=z;                    /* Figurenart merken */
        *(pbt+b)=z;               /* Umwandlungsfigur auf Feld */
        fln[5]--;                 /* Anzahl Bauern minus 1 */
        *(flp+p1)=flc[5][fln[5]]; /* Letzten Bauern an Stelle des umgew. */
        flc[z][fln[z]]=b;         /* Neue Figur an Liste anfuegen */
        fln[z]++;                 /* Anzahl Figuren der Neuen erhoehen */
      }
      if (z>27)
      {
        if (z>119)                 // Rochade Koenigszug
        {
          *(pbt+32)=0;
          if (z==120)
          {
            *(pbt+34)=6;
            flc[6][0]=34;
          }
          else
          {
            *(pbt+30)=6;
            flc[6][0]=30;
          }
        }
        else
        {
          fln[9]--;                  /* schwarzer Bauer ep geschlagen */
          pbt2=flc[9];               /* Ende Liste schwarzer Bauern */
          while (*pbt2 != z) pbt2++; /* geschlagenen Bauern suchen */
          *pbt2=*(flc[9]+fln[9]);    /* Letzten Bauern an Pos. des geschl. */
          *(pbt+b-12)=0;             /* Feld des geschl. Bauern loeschen */
          pos[g].pbt2=pbt2;
        }
      }
    }
  }
  
  if (d !=0)
  {
    fln[d]--;                          // geschlagene Figurenart -1
    pbt2=flc[d];
    while (*pbt2 != b) pbt2++;        // geschlagende Figur suchen
    *pbt2=*(flc[d]+fln[d]);
    pos[g].pbt2=pbt2;                  // Urspruengliche Listenposition der geschlagenen Figur merken
  }

  pos[g].a=a;
  pos[g].b=b;
  pos[g].d=d;
  pos[g].z=z;
  pos[g].p1=p1;
  g++;
  pos[g].lwm=lwm;
}


void undo_w_move()
{
  int a,b;
  unsigned char d,z,*pbt;
  
  pbt=cb.cc;
  
  g--;
  a=pos[g].a;
  b=pos[g].b;
  d=pos[g].d;
  z=pos[g].z;

  *(flp+pos[g].p1)=a;           // Alte Position in Figurenliste eintragen
  *(pbt+a)= *(pbt+b);

  if (z!=0)
  {
    if (z>27)
    {
      if (z>119)                // Koenigszug Rochade zuruecknehmen
      {
        *(pbt+flc[6][0])=0;
        *(pbt+32)=6;
        flc[6][0]=32;
      }
      else
      {        
        *(pos[g].pbt2)=z;       // ep-schlagen zuruecknehmen
        fln[9]++;               // Anzahl schwarzer Bauern erhoehen
        *(pbt+b-12)=9;          // geschlagenen Bauern wieder setzen
      }
    }
    else
    {
      fln[5]++;                 // Anzahl Bauern +1, Liste blieb korrekt
      fln[z]--;                 // Anzahl Figuren Umwandlungsart -1
      *(pbt+a)=5;               // Umwandlung des Bauern zuruecknehmen
    }
  }                             // Der urspruengliche Listeneintrag wird oben ueber pos[g].p1 wiederhergestellt.  
  
  if (d != 0)
  {
    *(flc[d]+fln[d]) = *(pos[g].pbt2);   // Veraenderte Listenposition wieder nach hinten
    *(pos[g].pbt2)=b;                    // Position der geschlagenen Figur
    fln[d]++;                            // geschlagene Figurenart +1                       
  }

  *(pbt+b)=d;                   // Altes Brett - Zielfeld
}


void make_b_move_h(int p1, int b)
{ 
  int a,g1;
  unsigned char c,d,z,*pbt,*pbt2;

  g1=g+1;
  pos[g1].hash_doppel=0;
  pbt=cb.cc;

  z=p1>>8;
  p1=p1&255;
  a=*(flp+p1);
  *(flp+p1)=b;
  c=*(pbt+a);
  *(pbt+a)=0;
  d=*(pbt+b);
  *(pbt+b)=c;
  lsm=c;

  pos[g1].hashkey = pos[g].hashkey ^ zobrist[c][a] ^ zobrist[c][b] ^ (uint64)-1;

  pos[g1].sr1=pos[g].sr1;
  if (pos[g1].sr1!=0)
  {        
    if ((c==8)||(z>119))
    {    
      if (((pos[g1].sr1) & (unsigned char)2) > 0)
        pos[g1].hashkey = pos[g1].hashkey ^ zobrist_rochade[3];
      if (((pos[g1].sr1) & (unsigned char)1) > 0)
        pos[g1].hashkey = pos[g1].hashkey ^ zobrist_rochade[2];
      pos[g1].sr1=0;    // Koenigszug, Rochade

    }
    else
    {
      if ((a==112)&&(c==12))
      {                                      
        if (((pos[g1].sr1) & (unsigned char)2) > 0)
        {
          pos[g1].sr1 &= 0xFD; // 2: Grosse Rochade ausblenden
          pos[g1].hashkey = pos[g1].hashkey ^ zobrist_rochade[3];
        }        
      }
      if ((a==119)&&(c==12))
      {                
        if (((pos[g1].sr1) & (unsigned char)1) > 0)
        {
          pos[g1].sr1 &= 0xFE; // 1: Kleine Rochade ausblenden
          pos[g1].hashkey = pos[g1].hashkey ^ zobrist_rochade[2];
        }
      }
    }
  }

  pos[g1].wr1=pos[g].wr1;
  if (pos[g1].wr1!=0)
  {
    if (b==28)
    {
      if (((pos[g1].wr1) & (unsigned char)2) > 0)
      {
        pos[g1].wr1 &= 0xFD; // 2: Grosse Rochade ausblenden
        pos[g1].hashkey = pos[g1].hashkey ^ zobrist_rochade[1];
      }
    }
    if (b==35)
    {
      if (((pos[g1].wr1) & (unsigned char)1) > 0)
      {
        pos[g1].wr1 &= 0xFE; // 1: Kleine Rochade ausblenden
        pos[g1].hashkey = pos[g1].hashkey ^ zobrist_rochade[0];
      }
    }
  }

  if (z!=0)
  {
    if (z==16)
    {
      z=0;
      lsm=b;
      if ((*(pbt+b+1)==5)||(*(pbt+b-1)==5)) pos[g1].hash_doppel=lsm;
    }
    else
    {
      if (z<16)
      {
        lsm=z;
        *(pbt+b)=z;               /* Umwandlungsfigur auf Feld */
        fln[9]--;                 /* Anzahl Bauern minus 1 */
        *(flp+p1)=flc[9][fln[9]]; /* Letzten Bauern an Stelle des umgew. */
        flc[z][fln[z]]=b;         /* Neue Figur an Liste anfuegen */
        fln[z]++;                 /* Anzahl Figuren der Neuen erhohen */
        pos[g1].hashkey = pos[g1].hashkey ^ zobrist[9][b] ^ zobrist[z][b];
      }
      if (z>27)
      {
        if (z>119)                  // Rochade Koenigszug
        {
          *(pbt+116)=0;
          if (z==122)
          {
            *(pbt+118)=8;
            flc[8][0]=118;
          }
          else
          {
            *(pbt+114)=8;
            flc[8][0]=114;
          }        
        
          pos[g1].hashkey = pos[g1].hashkey ^ zobrist[8][116] ^ zobrist[8][flc[8][0]];
        }
        else
        {          
          fln[5]--;                  // weisser Bauer ep geschlagen
          pbt2=flc[5];               // Ende Liste weisser Bauern
          while (*pbt2 != z) pbt2++; // geschlagenen Bauern suchen
          *pbt2=*(flc[5]+fln[5]);    // Letzten Bauern an Pos. des geschl.
          *(pbt+b+12)=0;             // Feld des geschl. Bauern loeschen
          pos[g].pbt2=pbt2;
          pos[g1].hashkey ^= zobrist[5][b+12];
        }
      }
    }
  }
  
  if (d !=0)
  {
    fln[d]--;
    pbt2=flc[d];
     while (*pbt2 != b) pbt2++;
    *pbt2=*(flc[d]+fln[d]);
    pos[g].pbt2=pbt2;
    pos[g1].hashkey ^= zobrist[d][b];
  }

  pos[g].a=a;
  pos[g].b=b;
  pos[g].d=d;
  pos[g].z=z;
  pos[g].p1=p1;
  g++;
  pos[g].lsm=lsm;
}


void make_b_move_h_nor(int p1, int b)
{ 
  int a,g1;
  unsigned char c,d,z,*pbt,*pbt2;

  g1=g+1;
  pos[g1].hash_doppel=0;
  pbt=cb.cc;

  z=p1>>8;
  p1=p1&255;
  a=*(flp+p1);
  *(flp+p1)=b;
  c=*(pbt+a);
  *(pbt+a)=0;
  d=*(pbt+b);
  *(pbt+b)=c;
  lsm=c;

  pos[g1].hashkey = pos[g].hashkey ^ zobrist[c][a] ^ zobrist[c][b] ^ (uint64)-1;

  pos[g1].sr1=0;
  pos[g1].wr1=0;
  if (z!=0)
  {
    if (z==16)
    {
      z=0;
      lsm=b;
      if ((*(pbt+b+1)==5)||(*(pbt+b-1)==5)) pos[g1].hash_doppel=lsm;
    }
    else
    {
      if (z<16)
      {
        lsm=z;
        *(pbt+b)=z;               /* Umwandlungsfigur auf Feld */
        fln[9]--;                 /* Anzahl Bauern minus 1 */
        *(flp+p1)=flc[9][fln[9]]; /* Letzten Bauern an Stelle des umgew. */
        flc[z][fln[z]]=b;         /* Neue Figur an Liste anfuegen */
        fln[z]++;                 /* Anzahl Figuren der Neuen erhohen */
        pos[g1].hashkey = pos[g1].hashkey ^ zobrist[9][b] ^ zobrist[z][b];
      }
      if (z>27)
      {
        fln[5]--;                  // weisser Bauer ep geschlagen
        pbt2=flc[5];               // Ende Liste weisser Bauern
        while (*pbt2 != z) pbt2++; // geschlagenen Bauern suchen
        *pbt2=*(flc[5]+fln[5]);    // Letzten Bauern an Pos. des geschl.
        *(pbt+b+12)=0;             // Feld des geschl. Bauern loeschen
        pos[g].pbt2=pbt2;
        pos[g1].hashkey ^= zobrist[5][b+12];        
      }
    }
  }
  
  if (d !=0)
  {
    fln[d]--;
    pbt2=flc[d];
     while (*pbt2 != b) pbt2++;
    *pbt2=*(flc[d]+fln[d]);
    pos[g].pbt2=pbt2;
    pos[g1].hashkey ^= zobrist[d][b];
  }

  pos[g].a=a;
  pos[g].b=b;
  pos[g].d=d;
  pos[g].z=z;
  pos[g].p1=p1;
  g++;
  pos[g].lsm=lsm;
}


void make_b_move_h_hashonly(int p1, int b)
{ 
  int a,g1;
  unsigned char c,d,z,*pbt;

  g1=g+1;
  pos[g1].hash_doppel=0;
  pbt=cb.cc;

  z=p1>>8;
  p1=p1&255;
  a=*(flp+p1);
  c=*(pbt+a);
  d=*(pbt+b);
  lsm=c;

  pos[g1].hashkey = pos[g].hashkey ^ zobrist[c][a] ^ zobrist[c][b] ^ (uint64)-1;

  pos[g1].sr1=pos[g].sr1;
  if (pos[g1].sr1!=0)
  {        
    if ((c==8)||(z>119))
    {    
      if (((pos[g1].sr1) & (unsigned char)2) > 0)
        pos[g1].hashkey = pos[g1].hashkey ^ zobrist_rochade[3];
      if (((pos[g1].sr1) & (unsigned char)1) > 0)
        pos[g1].hashkey = pos[g1].hashkey ^ zobrist_rochade[2];
    }
    else
    {
      if ((a==112)&&(c==12))
      {                                      
        if (((pos[g1].sr1) & (unsigned char)2) > 0)
        {
          pos[g1].hashkey = pos[g1].hashkey ^ zobrist_rochade[3];
        }        
      }
      if ((a==119)&&(c==12))
      {                
        if (((pos[g1].sr1) & (unsigned char)1) > 0)
        {
          pos[g1].hashkey = pos[g1].hashkey ^ zobrist_rochade[2];
        }
      }
    }
  }

  pos[g1].wr1=pos[g].wr1;
  if (pos[g1].wr1!=0)
  {
    if (b==28)
    {
      if (((pos[g1].wr1) & (unsigned char)2) > 0)
      {
        pos[g1].hashkey = pos[g1].hashkey ^ zobrist_rochade[1];
      }
    }
    if (b==35)
    {
      if (((pos[g1].wr1) & (unsigned char)1) > 0)
      {
        pos[g1].hashkey = pos[g1].hashkey ^ zobrist_rochade[0];
      }
    }
  }

  if (z!=0)
  {
    if (z==16)
    {
      z=0;
      lsm=b;
      if ((*(pbt+b+1)==5)||(*(pbt+b-1)==5)) pos[g1].hash_doppel=lsm;
    }
    else
    {
      if (z<16)
      {
        lsm=z;
        pos[g1].hashkey = pos[g1].hashkey ^ zobrist[9][b] ^ zobrist[z][b];
      }
      if (z>27)
      {
        if (z>119)                  // Rochade Koenigszug
        {
          if (z==122)
          {               
            pos[g1].hashkey = pos[g1].hashkey ^ zobrist[8][116] ^ zobrist[8][118];
          }
          else
          {
            pos[g1].hashkey = pos[g1].hashkey ^ zobrist[8][116] ^ zobrist[8][114];
          }        
        
          pos[g1].hashkey = pos[g1].hashkey ^ zobrist[8][116] ^ zobrist[8][flc[8][0]];
        }
        else
        {                     
          pos[g1].hashkey ^= zobrist[5][b+12];
        }
      }
    }
  }
  
  if (d !=0)
    pos[g1].hashkey ^= zobrist[d][b];

  g++;
}


void make_b_move_h_nor_hashonly(int p1, int b)
{ 
  int a,g1;
  unsigned char c,d,z,*pbt;

  g1=g+1;
  pos[g1].hash_doppel=0;
  pbt=cb.cc;

  z=p1>>8;
  p1=p1&255;
  a=*(flp+p1);
  c=*(pbt+a);
  d=*(pbt+b);
  lsm=c;

  pos[g1].hashkey = pos[g].hashkey ^ zobrist[c][a] ^ zobrist[c][b] ^ (uint64)-1;

  pos[g1].sr1=0;
  pos[g1].wr1=0;
  if (z!=0)
  {
    if (z==16)
    {
      z=0;
      lsm=b;
      if ((*(pbt+b+1)==5)||(*(pbt+b-1)==5)) pos[g1].hash_doppel=lsm;
    }
    else
    {
      if (z<16)
      {
        lsm=z;
        pos[g1].hashkey = pos[g1].hashkey ^ zobrist[9][b] ^ zobrist[z][b];
      }
      if (z>27)
      {         
        pos[g1].hashkey ^= zobrist[5][b+12];        
      }
    }
  }
  
  if (d !=0)
    pos[g1].hashkey ^= zobrist[d][b];

  g++;
}


void make_b_move(int p1, int b)
{ 
  int a,g1;
  unsigned char c,d,z,*pbt,*pbt2;

  g1=g+1;
  pbt=cb.cc;

  z=p1>>8;
  p1=p1&255;
  a=*(flp+p1);
  *(flp+p1)=b;
  c=*(pbt+a);
  *(pbt+a)=0;
  d=*(pbt+b);
  *(pbt+b)=c;
  lsm=c;

  pos[g1].sr1=pos[g].sr1;
  if (pos[g1].sr1!=0)
  {        
    if ((c==8)||(z>119))
    {          
      pos[g1].sr1=0;    // Koenigszug, Rochade
    }
    else
    {
      if ((a==112)&&(c==12))
      {                                      
        if (((pos[g1].sr1) & (unsigned char)2) > 0)
          pos[g1].sr1 &= 0xFD; // 2: Grosse Rochade ausblenden          
      }
      if ((a==119)&&(c==12))
      {                
        if (((pos[g1].sr1) & (unsigned char)1) > 0)
          pos[g1].sr1 &= 0xFE; // 1: Kleine Rochade ausblenden
      }
    }
  }

  pos[g1].wr1=pos[g].wr1;
  if (pos[g1].wr1!=0)
  {
    if (b==28)
    {
      if (((pos[g1].wr1) & (unsigned char)2) > 0)
        pos[g1].wr1 &= 0xFD; // 2: Grosse Rochade ausblenden        
    }
    if (b==35)
    {
      if (((pos[g1].wr1) & (unsigned char)1) > 0)
        pos[g1].wr1 &= 0xFE; // 1: Kleine Rochade ausblenden
    }
  }

  if (z!=0)
  {
    if (z==16)
    {
      z=0;
      lsm=b;
    }
    else
    {
      if (z<16)
      {
        lsm=z;
        *(pbt+b)=z;               /* Umwandlungsfigur auf Feld */
        fln[9]--;                 /* Anzahl Bauern minus 1 */
        *(flp+p1)=flc[9][fln[9]]; /* Letzten Bauern an Stelle des umgew. */
        flc[z][fln[z]]=b;         /* Neue Figur an Liste anfuegen */
        fln[z]++;                 /* Anzahl Figuren der Neuen erhohen */
      }
      if (z>27)
      {
        if (z>119)                  // Rochade Koenigszug
        {
          *(pbt+116)=0;
          if (z==122)
          {
            *(pbt+118)=8;
            flc[8][0]=118;
          }
          else
          {
            *(pbt+114)=8;
            flc[8][0]=114;
          }                
        }
        else
        {          
          fln[5]--;                  // weisser Bauer ep geschlagen
          pbt2=flc[5];               // Ende Liste weisser Bauern
          while (*pbt2 != z) pbt2++; // geschlagenen Bauern suchen
          *pbt2=*(flc[5]+fln[5]);    // Letzten Bauern an Pos. des geschl.
          *(pbt+b+12)=0;             // Feld des geschl. Bauern loeschen
          pos[g].pbt2=pbt2;
          pos[g1].hashkey ^= zobrist[5][b+12];
        }
      }
    }
  }
  
  if (d !=0)
  {
    fln[d]--;
    pbt2=flc[d];
     while (*pbt2 != b) pbt2++;
    *pbt2=*(flc[d]+fln[d]);
    pos[g].pbt2=pbt2;
  }

  pos[g].a=a;
  pos[g].b=b;
  pos[g].d=d;
  pos[g].z=z;
  pos[g].p1=p1;
  g++;
  pos[g].lsm=lsm;
}



void undo_b_move()
{
  int a,b;
  unsigned char d,z,*pbt;

  pbt=cb.cc;  
  
  g--;
  a=pos[g].a;
  b=pos[g].b;
  d=pos[g].d;
  z=pos[g].z;  
    
  *(flp+pos[g].p1)=a;
  *(pbt+a)= *(pbt+b);
  
  if (z!=0)
  {
    if (z>27)
    {
      if (z>119)                 // Koenigszug Rochade zuruecknehmen
      {
        *(pbt+flc[8][0])=0;
        *(pbt+116)=8;
        flc[8][0]=116;      
      }
      else
      {              
        *(pos[g].pbt2)=z;         // ep-schlagen zurueck
        fln[5]++;                 // Anzahl schwarzer Bauern erhoehen
        *(pbt+b+12)=5;
      }
    }
    else
    {
      fln[9]++;
      fln[z]--;
      *(pbt+a)=9;               // Umwandlung des Bauern zuruecknehmen
    }
  }

  if (d != 0)
  {
    *(flc[d]+fln[d]) = *(pos[g].pbt2);
    *(pos[g].pbt2)=b;            
    fln[d]++;
  }

  *(pbt+b)=d;
}



void make_move_h(int a, int b)
{  
  if (g&1)
     make_b_move_h(a,b);  
  else
     make_w_move_h(a,b);
}

// Keine Rochade mehr moeglich
void make_move_h_nor(int a, int b)
{  
  if (g&1)
     make_b_move_h_nor(a,b);  
  else
     make_w_move_h_nor(a,b);
}

void make_move_h_hashonly(int a, int b)
{  
  if (g&1)
     make_b_move_h_hashonly(a,b);  
  else
     make_w_move_h_hashonly(a,b);
}

// Keine Rochade mehr moeglich
void make_move_h_nor_hashonly(int a, int b)
{  
  if (g&1)
     make_b_move_h_nor_hashonly(a,b);  
  else
     make_w_move_h_nor_hashonly(a,b);
}


void make_move(int a, int b)
{
  if (g&1)
    make_b_move(a,b);
  else
    make_w_move(a,b);
}


void undo_move()
{
  if (g&1)
    undo_w_move();
  else
    undo_b_move();
}


void make_null()
{
  pos[g].lsm=lsm; // Zugmerker fuer undo_null() sichern.
  pos[g].lwm=lwm;  
  pos[g].d=0;
  pos[g].p1=0;
  g++;
  
  pos[g].hash_doppel=0;
  pos[g].wr1=pos[g-1].wr1;
  pos[g].sr1=pos[g-1].sr1;  
  
  lsm=0;
  lwm=0;
  pos[g].lsm=0;
  pos[g].lwm=0;
  pos[g].hashkey = pos[g-1].hashkey^(uint64)-1;
}


void undo_null()
{
  --g;
  lsm=pos[g].lsm;  // Zugmerker der urspruenglichen Stellung wiederherstellen.
  lwm=pos[g].lwm;
}


int in_check()
{
  if (g&1)
    return in_check_b();
  else
    return in_check_w();
}


int make_king()
{
  if (g&1)
    return makesking();
  else
    return makewking();
}
