#include "common.h"
#include "init.h"


struct board *declare_new_board (int *n_boards, struct board *list, struct board *infocus, scaling scale);
struct list_lines *declare_new_line (struct board *start_item, struct board *end_item, scaling scale);
void fit_in_list (struct board *new_item, struct board **list, struct list_lines **list_lines);

struct board* add_board (int *n_boards, struct board **infocus, scaling scale, struct board **list, struct list_lines **list_lines);
void delete_board (struct board *p, int *n_boards, struct opted **sel, struct board **infocus, struct board **list, struct list_lines **list_lines);

void select_board (struct board *p, struct opted **sel);

void shift_elements (struct opted *sel, SDL_Event *event, SDL_MouseButtonEvent *pan_start, scaling scale);
void shift_one (struct board *p, double scale, int shift_x, int shift_y);
void recur_shift (struct board *p, double scale, int shift_x, int shift_y);

void opt_in (struct board *p, struct opted **optList);

