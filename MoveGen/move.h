#ifndef _move
#define _move

#include <stdint.h>
typedef uint64_t uint64;

int in_check();
int in_check_w();
int in_check_b();
int make_king();

void make_move_h(int a, int b);
void make_move_h_nor(int a, int b);
void make_move_h_hashonly(int a, int b);
void make_move_h_nor_hashonly(int a, int b);
void make_move(int a, int b);
void undo_move();
void make_null();
void undo_null();

void make_b_move(int a, int b);
void make_w_move(int a, int b);

void make_b_move_h(int a, int b);
void make_w_move_h(int a, int b);

void make_b_move_h_nor(int a, int b);
void make_w_move_h_nor(int a, int b);
 
void undo_b_move();
void undo_w_move();

int makesking();
int makewking();
int makesking_check();

int check_w_move(int a, int b);

#endif // -move
