#ifndef JETCHESS_MGEN6_H
#define JETCHESS_MGEN6_H

#ifdef __cplusplus
extern "C" {
#endif
  void __fastcall set_winflag(int wf);
  void __fastcall init_tables();
  void __fastcall init_mytime();
  
  int  __fastcall set_opmode(int op);
  /* MiB: 1024, 2048, 4096, 8192, 16384 oder 32768; Rueckgabe: aktive Groesse. */
  int  __fastcall hash_set(int size_mb); 
  void __fastcall hash_zero();

  __int64 __fastcall Anzahl_DoAll(int max_level, char *fen_txt, int steps);

  void __fastcall set_stopflag(int sf);
  int  __fastcall get_stopflag();

  /* Vier/sechs Felder, optional ; id; Fehler erhalten die Stellung. Maximal 255 Bytes. */
  int  __fastcall fen_to_board(char *fen);
  int  __fastcall create_grafic_offsets();
  void __fastcall get_grafic_offset(int n, int *x, int *y);
  
  char* __fastcall get_fen(int n); 
  /* Gespeicherten Eingabetext hinzufuegen; -1 bei NULL oder mehr als 255 Bytes. */
  int   __fastcall add_fen(char *fen); 
  
  void  __fastcall reset_anzcounter();
  int   __fastcall get_anzresult();
  int   __fastcall get_anzinfo();
  int   __fastcall get_anztext();
  
  char* __fastcall get_result();
  char* __fastcall get_info();
  char* __fastcall get_text(int n);

#ifdef __cplusplus
}
#endif
#endif
