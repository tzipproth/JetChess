#include "rtf_resources.h"
#include <windows.h>
#include <Richedit.h>
#include <string.h>

static char err1_txt[64];
static size_t agblen;
static size_t resource_length;
static char *agb;

// versorgt das Richedit-Feld auf Anforderung mit Daten
static DWORD CALLBACK EditStreamCallback(DWORD_PTR dwCookie, LPBYTE pbBuff, LONG cb, LONG *pcb)
{
  size_t count = agblen < (size_t)cb ? agblen : (size_t)cb;
  memcpy(pbBuff, agb, count);
  *pcb = (LONG)count;
  agb += count;
  agblen -= count;

  return 0;
};


char *get_resource_string(int res_id)
{
  HRSRC hsc;
  char *agb;
     
  // RTF-Textressource suchen
  hsc=FindResourceW(NULL,MAKEINTRESOURCEW(res_id),MAKEINTRESOURCEW(6));   // IDR_RTF1
  if (hsc==NULL)
  {
    strcpy(err1_txt,"Res Error 1");
    return err1_txt;
  }

  // Zeiger auf den RTF-Text holen
  agb = (char *)LockResource(LoadResource(NULL,hsc));
  if (agb==NULL)
  {
    strcpy(err1_txt,"Res Error 2");
    return err1_txt;
  }

  resource_length = SizeofResource(NULL,hsc);
  return agb;
}


void fill_richtext_window(void *hwndv, char *cpt)
{   
  EDITSTREAM es;
  HWND hwnde;

  hwnde = (HWND)hwndv;
  agblen = (cpt == err1_txt) ? strlen(cpt) : resource_length;
  agb = cpt;

  // Struktur zum Bestuecken eines Richedit Feldes
  es.dwCookie=0;
  es.dwError=0;
  es.pfnCallback=EditStreamCallback;

  SendMessage(hwnde, EM_STREAMIN, (WPARAM) SF_RTF, (LPARAM) &es);
}

