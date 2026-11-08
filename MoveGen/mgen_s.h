#ifndef _mgen_s
#define _mgen_s

void mv1wep(void);
void mv2wep(void);
void mv1sep(void);
void mv2sep(void);

int generate_white();
int generate_black();
int generate_moves();
int generate_black_one_move();

int generate_white_perft();
int generate_black_perft();
int generate_moves_perft();

int generate_white_check();
int generate_moves_ep();

#endif // _mgen_s
