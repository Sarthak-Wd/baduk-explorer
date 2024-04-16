#include "camera.h"




void pan_coords (struct list *p, struct list_lines *l, SDL_Event *event, SDL_MouseButtonEvent *pan_start, double scale, double center_x_scaled, double center_y_scaled) { 
	
	
	int shift_x, shift_y, shift_x_scaled, shift_y_scaled;
	
	
	while (event->type != SDL_MOUSEBUTTONUP) 
		SDL_PollEvent(event);
	
	shift_x = event->button.x - pan_start->x;
	shift_y = event->button.y - pan_start->y;
	shift_x_scaled = (int)(shift_x/scale);				//calculating things outside of the loop 
	shift_y_scaled = (int)(shift_y/scale);
	
	for (; p != NULL; p = p->next) {
		p->node->board.rep.center_off.x += shift_x_scaled; 
		p->node->board.rep.center_off.y += shift_y_scaled;
		p->node->board.rep.size.x += shift_x;
		p->node->board.rep.size.y += shift_y;
	}
	
	for (; l != NULL; l = l->next) {
		l->start.x += shift_x;
		l->start.y += shift_y;
		l->end.x += shift_x;
		l->end.y += shift_y;
	}
	
} 
	
	
	
	
void zoom_coords (struct list *q, struct list_lines *l, SDL_Event event, double *scale, double *center_x_scaled, double *center_y_scaled) {
	
	
	if (event.wheel.y > 0)	{		//scroll up 	
		if (*scale >= 1.56)
			return;
		*scale *= SCALE_FACTOR;
	}
	else if (event.wheel.y < 0)   	//scroll down
		*scale /= SCALE_FACTOR;
		
	*center_x_scaled = (WINDOW_WIDTH /(*scale) * (0.5));
	*center_y_scaled = (WINDOW_HEIGHT/(*scale) * (0.5));
		
										//updating coordinates
	for (; q != NULL; q = q->next)	{												//boards' coordinates
		
		q->node->board.rep.size.x = (*center_x_scaled + q->node->board.rep.center_off.x) * (*scale);		
		q->node->board.rep.size.y = (*center_y_scaled + q->node->board.rep.center_off.y) * (*scale);
	}
	
	for (; l != NULL; l = l->next)	{												//lines' coordinates
		
		l->start.x = l->start_board->rep.size.x + (BOARD_SIZE/2) * (*scale);		
		l->start.y = l->start_board->rep.size.y + (BOARD_SIZE/2) * (*scale);
		l->end.x = l->end_board->rep.size.x + (BOARD_SIZE/2) * (*scale);
		l->end.y = l->end_board->rep.size.y + (BOARD_SIZE/2) * (*scale);
		
	}
	
	
}
