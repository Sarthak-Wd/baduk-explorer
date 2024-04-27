#include "common.h"

void pan_manual (struct board *p, struct list_lines *l, SDL_Event *event, SDL_MouseButtonEvent *pan_start, scaling scale);
void pan (struct board *list, struct list_lines *l, SDL_Event *event, scaling scale, struct whole_coords shift);
void zoom_coords (struct board *q, struct list_lines *lines, SDL_Event event, scaling *scale);
