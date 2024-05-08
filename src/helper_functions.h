#include "common.h"
#include "init.h"



void shift_one (struct board *p, double scale, int shift_x, int shift_y);
void recur_shift (struct board *p, double scale, int shift_x, int shift_y);

void opt_in (struct board *p, struct opted **optList);

void fit_in_list (struct board *new_item, struct board **list, struct list_lines **list_lines);
struct board *declare_new_board (int *n_boards, struct board *list, struct board *infocus, scaling scale);
struct list_lines *declare_new_line (struct board *start_item, struct board *end_item, scaling scale);

void place_stone (int column, int row, struct board *board, SDL_Texture *stone);
void put_number (int column, int row, playing_parts *parts);

void coords_from_mouse (SDL_Event event, struct board *board, int *column, int *row, double scale_amount);

bool isin_box (SDL_Rect rect, SDL_MouseButtonEvent button);
