#include "helper_functions.h"






								//to shift all the boards below to fit one in the branch
void recur_shift (struct spawn *b, scaling scale) { 
	
	if (!b)
		return;
	
	if (b->next != NULL)
		recur_shift (b->next, scale);
	
	int shift_y = (int)((BOARD_SIZE + SPACE_BW)*scale.amount);	
	
	shift_one (b->item, 0, shift_y, scale.amount); 
	
	if (b->item->node->below != NULL) 
		recur_shift (b->item->node->below, scale);
	
} 


				//might not need this. I should dissolve this function if it gets confusing.
void shift_one (struct list *p, int shift_x, int shift_y, double scale) {
	
	p->node->board.rep.center_off.x += (int)(shift_x/scale); 
	p->node->board.rep.center_off.y += (int)(shift_y/scale);
	p->node->board.rep.size.x += shift_x;
	p->node->board.rep.size.y += shift_y;
	
							//adjusting lines on shifting boards
	if (p->node->above != NULL) {					//if it's not the first board: the first board has no above.	
		p->node->above->line->end.x += shift_x;		//boards have only one line above
		p->node->above->line->end.y += shift_y;
	}	
												 
	for (struct spawn *bl = p->node->below; bl != NULL; bl = bl->next) {		
		bl->line->start.x += shift_x;		//multiple lines can emerge from a single board
		bl->line->start.y += shift_y;	
	}
}
	
	
	
	
	




								//can't I fit this in the corresponding 'declare_new'?
void fit_in_list (struct list *new_item, struct list_lines *new_line, struct list **list, struct list_lines **list_lines) {
	
	new_line->next = *list_lines;				//in the list, starting of
	new_line->prev = NULL;
	if (*list_lines != NULL)						//if this isn't the first line in the linked list
		(*list_lines)->prev = new_line;
	*list_lines = new_line;
									
									//fitting the board into the universal list
	new_item->next = *list;
	new_item->prev = NULL;
	if (*list != NULL)						//no need to use it yet, might in the future
		(*list)->prev = new_item;
	*list = new_item;
}	


struct list *declare_new_board (int *n_boards, struct list *infocus, scaling scale) {
	
	struct list   *new_item = malloc (sizeof(struct list));
	struct branch *new_node = malloc (sizeof(struct branch));
	
	if (new_node == NULL || new_item == NULL) {
		printf ("failed to add a board.");
		return NULL;
	}
	

	++(*n_boards);
	new_node->number = *n_boards;
	new_item->number = *n_boards;
	new_item->selection = NULL;
	
								
							//need this. For some reason, setting the value of center_off coords through the rep.size struct does not work.
	struct whole_coords newcoord = {.x = infocus->node->board.rep.size.x, .y = infocus->node->board.rep.size.y + (BOARD_SIZE + SPACE_BW)*scale.amount};

	struct board new_board = {.mech.state = {{{0}}},		//the braces because it's an array of 
							.mech.turn = 0,						//structures. Still dk for sure.
							.mech.total_moves = 0,
					
							.rep.size = {newcoord.x, newcoord.y, BOARD_SIZE, BOARD_SIZE},		
							.rep.center_off.x = (newcoord.x)/scale.amount - scale.center.x, 
							.rep.center_off.y = (newcoord.y)/scale.amount - scale.center.y,
							.rep.snap = NULL,
						};
	
	new_board.rep.snap = SDL_CreateTexture (renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, BOARD_SIZE, BOARD_SIZE);
	SDL_SetTextureBlendMode(new_board.rep.snap, SDL_BLENDMODE_BLEND);
	SDL_SetRenderTarget (renderer, new_board.rep.snap);
	SDL_SetRenderDrawColor (renderer, 0, 0, 0, 0);
	SDL_RenderClear(renderer);
	SDL_SetRenderTarget (renderer, NULL);
	
	new_node->board = new_board;
	new_item->node = new_node;
	
	return new_item;
}


struct list_lines *declare_new_line (struct list *start_item, struct list *end_item, scaling scale) {

	struct list_lines *new_line = malloc(sizeof(struct list_lines));
	double offset = (BOARD_SIZE/2) * scale.amount;
	
	new_line->start_board = &(start_item->node->board);		
	new_line->end_board   = &(end_item->node->board);
	
	new_line->start.x = new_line->start_board->rep.size.x + offset;
	new_line->start.y = new_line->start_board->rep.size.y + offset;
	new_line->end.x   = new_line->end_board->rep.size.x   + offset;
	new_line->end.y   = new_line->end_board->rep.size.y   + offset;
	
	return new_line;
}


		






					//opt in boards to mass select, delete, shift
void opt_in (struct list *p, struct opted **optList) {
	
	
	for (;;) {								//adding boards to the optList.
		
		struct opted *new_item = malloc(sizeof(struct opted));
		
		new_item->list = p;
		new_item->next = *optList;
		if (*optList != NULL)
			(*optList)->prev = new_item;
		new_item->prev = NULL;
		*optList = new_item;		//I think this is why I need a pointer to a pointer
		
		if (p->node->below == NULL)
			break;

										//moving horizontal, towards the right  (left?)
		for(struct spawn *q = p->node->below; q->next != NULL; q = q->next) 
			opt_in(q->next->item, optList);	
				
		
						
										//moving vertical, downwards
		p = p->node->below->item;
		
	}
}










void place_stone (int column, int row, struct board *board, SDL_Texture *stone) {						
	
	SDL_Rect stoneSize = { ((column*SQUARE_SIZE + BORDER) - 15), ((row*SQUARE_SIZE + BORDER) - 15), STONE_SIZE, STONE_SIZE};	
	SDL_SetRenderTarget (renderer, board->rep.snap);
	SDL_RenderCopy(renderer, stone, NULL, &stoneSize);
	SDL_SetRenderTarget (renderer, NULL);
}




						//printing a number on a placed stone.
void put_number (int column, int row, playing_parts *parts) {

		
		char *buffer = malloc(5);
		buffer[5] = '\0';
		sprintf (buffer, "%d", parts->number);
		SDL_Surface *stoneNo_surface = TTF_RenderText_Solid(parts->font, buffer, parts->font_color);
		SDL_Texture *stoneNo_texture = SDL_CreateTextureFromSurface(renderer, stoneNo_surface);
		free(buffer);	
	
	
		int texW, texH;
		int x_offset;	//since the coordinates align with the top left of the text
		SDL_QueryTexture(stoneNo_texture, NULL, NULL, &texW, &texH);
		if (parts->number < 10)
			x_offset = 6;
		else x_offset = 11;
		SDL_Rect stoneNo_rect = { ((column*SQUARE_SIZE + BORDER) - x_offset), ((row*SQUARE_SIZE + BORDER) - 9), texW, texH };
	
		
		SDL_SetRenderTarget (renderer, parts->item->node->board.rep.snap);
		SDL_RenderCopy(renderer, stoneNo_texture, NULL, &stoneNo_rect);
		SDL_FreeSurface(stoneNo_surface);
		SDL_DestroyTexture(stoneNo_texture);
		SDL_SetRenderTarget (renderer, NULL);
}










bool isin_box (SDL_Rect rect, SDL_MouseButtonEvent button) {
	
	if (!(rect.x < button.x  &&  button.x < (rect.x + rect.w)))
		return FALSE;
	if (!(rect.y < button.y  &&  button.y < (rect.y + rect.h)))
		return FALSE;
		
	return TRUE;
}
		
