#include "common.h"
#include "init.h"
#include "helper_functions.h"


void play_move (int column, int row, playing_parts *parts);
void undo_move (struct board *p, struct board **infocus, struct message *text, bool branching);

void group_stuff (int column, int row, struct board *board);
void capture_group (struct board *board, struct group *group);

struct board* add_board (int *n_boards, struct board **infocus, scaling scale, struct board **list, struct list_lines **list_lines);
struct board *split_board (int *n_boards, int moveNum, playing_parts *parts, struct message *text, struct board **infocus, struct board **list, struct list_lines **list_lines, scaling scale);
void delete_board (struct board *p, int *n_boards, struct opted **sel, struct board **infocus, struct board **list, struct list_lines **list_lines);
void select_board (struct board *p, struct opted **sel);
void shift_elements (struct opted *sel, SDL_Event *event, SDL_MouseButtonEvent *pan_start, scaling scale);
