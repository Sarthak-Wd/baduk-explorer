#include "common.h"
#include "init.h"
#include "helper_functions.h"


void play_move (SDL_Event event, playing_parts *parts, scaling scale, struct message *text, struct list **infocus);
void undo_move (struct list *p, struct list **infocus, struct message *text, bool branching);
struct list* add_board (int *n_boards, struct list **infocus, scaling scale, struct list **list, struct list_lines **list_lines);
void delete_board (struct list *p, int *n_boards, struct opted **sel, struct list **infocus, struct list **list, struct list_lines **list_lines);
void select_board (struct list *p, struct opted **sel);
void shift_elements (struct opted *sel, SDL_Event *event, SDL_MouseButtonEvent *pan_start, scaling scale);
