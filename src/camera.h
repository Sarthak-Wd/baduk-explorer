#include "common.h"

void pan_coords (struct list *p, struct list_lines *l, SDL_Event *event, SDL_MouseButtonEvent *pan_start, double scale, double center_x_scaled, double center_y_scaled);
void zoom_coords (struct list *q, struct list_lines *lines, SDL_Event event, double *scale, double *center_x_scaled, double *center_y_scaled);
