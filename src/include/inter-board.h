#ifndef INTER
#define INTER

#include "common.h"
#include "init.h"
#include "main.h"
#include "play_state.h"
#include "camera.h"


extern struct message text;

//~ Functions that deal with inter-board links and managing the states while linking the boards.



void continue_play (int *n_boards, struct board **infocus, struct board **list, struct list_lines **list_lines, playing_parts *parts, scaling scale);
void branch_window (struct board *p, struct board **list, struct list_lines **list_lines, int *n_boards, scaling *scale, struct board **infocus, playing_parts *parts,  bool shifting);
void off_shoot (struct board *p, struct board **list, struct list_lines **list_lines, struct board **infocus, playing_parts *parts, scaling scale, int column, int row, int moveNum, int *n_boards);
void combine_board (struct board *p, playing_parts *parts, scaling scale, struct list_lines **list_lines);
void split_mode (struct board *p, int *n_boards, struct board **list, struct board **infocus, struct list_lines **list_lines, scaling *scale, playing_parts *parts);
struct board *split_board (int *n_boards, int moveNum, playing_parts *parts, struct message *text, struct board **infocus, struct board **list, struct list_lines **list_lines, scaling scale);

#endif
