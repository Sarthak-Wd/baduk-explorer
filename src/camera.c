#include "include/camera.h"




void pan_manual (struct board *list, struct list_lines *l, SDL_Event *event, SDL_MouseButtonEvent *pan_start, scaling scale) { 
	
	
	struct whole_coords shift;
	
	
	while (event->type != SDL_MOUSEBUTTONUP) 
		SDL_PollEvent(event);
	
	shift.x = event->button.x - pan_start->x;
	shift.y = event->button.y - pan_start->y;
	
	pan (list, l, scale, shift);
	
} 
	


void pan (struct board *list, struct list_lines *l, scaling scale, struct whole_coords shift) {

	struct whole_coords shift_scaled;
	
	shift_scaled.x = (int)(shift.x/scale.amount);				//calculating things outside of the loop 
	shift_scaled.y = (int)(shift.y/scale.amount);
	
	for (; list != NULL; list = list->next) {
		list->rep.center_off.x += shift_scaled.x; 
		list->rep.center_off.y += shift_scaled.y;
		list->rep.size.x += shift.x;
		list->rep.size.y += shift.y;
	}
	
	for (; l != NULL; l = l->next) {
		l->start.x += shift.x;
		l->start.y += shift.y;
		l->end.x += shift.x;
		l->end.y += shift.y;
	}
}


	
	
	
void zoom_coords (struct board *q, struct list_lines *l, SDL_Event event, scaling *scale) {
	
	
	if (event.wheel.y > 0)	{		//scroll up 	
		if (scale->amount >= 1.56)
			return;
		scale->amount *= SCALE_FACTOR;
	}
	else if (event.wheel.y < 0)   	//scroll down
		scale->amount /= SCALE_FACTOR;
		
	scale->center.x = (WINDOW_WIDTH /(scale->amount) * (0.5));
	scale->center.y = (WINDOW_HEIGHT/(scale->amount) * (0.5));
		
										//updating coordinates
	for (; q != NULL; q = q->next)	{												//boards' coordinates
		
		q->rep.size.x = (scale->center.x + q->rep.center_off.x) * (scale->amount);		
		q->rep.size.y = (scale->center.y + q->rep.center_off.y) * (scale->amount);
	}
	
	for (; l != NULL; l = l->next)	{												//lines' coordinates
		
		l->start.x = l->start_board->rep.size.x + (BOARD_SIZE/2) * (scale->amount);		
		l->start.y = l->start_board->rep.size.y + (BOARD_SIZE/2) * (scale->amount);
		l->end.x = l->end_board->rep.size.x + (BOARD_SIZE/2) * (scale->amount);
		l->end.y = l->end_board->rep.size.y + (BOARD_SIZE/2) * (scale->amount);
		
	}
	
	
}
