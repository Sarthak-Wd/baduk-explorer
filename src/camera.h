#include "common.h"

void pan_coords (struct board *p, struct list_lines *l, SDL_Event *event, SDL_MouseButtonEvent *pan_start, scaling scale);
void zoom_coords (struct board *q, struct list_lines *lines, SDL_Event event, scaling *scale);
