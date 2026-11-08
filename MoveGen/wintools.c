#define _CRT_SECURE_NO_DEPRECATE 1

#include <windows.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int high_resolution;
int ptime_start32[32];
int ptime_all32[32];
char ctx[128];

__int64 ptime_fr;  // Timer Resolution QueryPerformanceCounter
__int64 ptime_start64[32];
__int64 ptime_all64[32];

void init_mytime()
{
  LARGE_INTEGER t1;
  BOOL b1;
  
  high_resolution=0;
  ptime_fr=0;

  b1 = QueryPerformanceFrequency(&t1);

  if (b1)  
  {
    ptime_fr = t1.QuadPart;
    if (ptime_fr > 1000)
      high_resolution=1;
  }
}


void start_mytime(int n)
{
  LARGE_INTEGER t1;
  BOOL b1;
    
  ptime_start32[n] = clock();

  if (high_resolution == 1)
  {
    b1 = QueryPerformanceCounter(&t1);
    ptime_start64[n]=0;
    if (b1)
      ptime_start64[n] = t1.QuadPart;
  }
}


void stop_mytime(int n)
{
  LARGE_INTEGER t1;
  __int64 ptime_stop64;
  int ptime_stop32;
  BOOL b1;

  if (high_resolution == 1)
    b1 = QueryPerformanceCounter(&t1);
  
  ptime_stop32 = clock();
    
  ptime_all32[n] = ptime_stop32 - ptime_start32[n];
   
  if (high_resolution == 1)
  {
    ptime_all64[n]=0;
    
    if (b1)
    {
      ptime_stop64 = t1.QuadPart;
      ptime_all64[n] = ptime_stop64 - ptime_start64[n];

      ptime_all64[n] = ptime_all64[n] * 1000 / ptime_fr;
    }
  }
}


__int64 get_mytime1(int n)
{
  if (high_resolution==1)
    return ptime_all64[n];
  else
    return (__int64)ptime_all32[n];
}


char *get_mytime2(int n)
{
  __int64 t1;
  char ct2[32];
  int len;

  memset(ctx,0,128);
  t1 = get_mytime1(n);

  if (t1 < 1000)
  {
    sprintf(ctx,"%I64d ms",t1);
  }
  else
  {
    memset(ct2,0,32);
    sprintf(ct2,"%I64d",t1);
    len=(int)strlen(ct2);

    strncpy(ctx,ct2,len-3);
    strcat(ctx,".");
    strcat(ctx,ct2+len-3);
    strcat(ctx," s");
  }
  return ctx;
}


char *get_mytime3(__int64 t1)
{
  char ct2[32];
  int len;

  memset(ctx,0,128);

  if (t1 < 1000)
  {
    sprintf(ctx,"%I64d ms",t1);
  }
  else
  {
    memset(ct2,0,32);
    sprintf(ct2,"%I64d",t1);
    len=(int)strlen(ct2);

    strncpy(ctx,ct2,len-3);
    strcat(ctx,".");
    strcat(ctx,ct2+len-3);
    strcat(ctx," s");
  }
  return ctx;
}
