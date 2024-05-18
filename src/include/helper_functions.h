#ifndef HELPER
#define HELPER

#include "common.h"
#include "init.h"



void place_stone (int column, int row, struct board *board, SDL_Texture *stone);
void put_number (int column, int row, playing_parts *parts);

void coords_from_mouse (SDL_Event event, struct board *board, int *column, int *row, double scale_amount);

bool isin_box (SDL_Rect rect, SDL_MouseButtonEvent button);
void delete_SLL (void *p);


void print_liberties (playing_parts *parts);
void put_liberty (int column, int row, playing_parts *parts);

struct group* deep_copy_group (struct group *ogroups_list);

#endif
