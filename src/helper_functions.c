#include "helper_functions.h"






								//to shift all the boards below a certain board, up or down
void recur_shift (struct board *p, double scale, int shift_x, int shift_y) { 
	
	shift_one (p, scale, shift_x, shift_y); 
	
	if (!p->below)
		return;
	
	for (struct spawn *walk = p->below; walk != NULL; walk = walk->next) 
		recur_shift (walk->board, scale, shift_x, shift_y);
	
	//recur_shift (p->below->board, scale, shift_x, shift_y);
	
} 


				//might not need this. I should dissolve this function if it gets confusing.
void shift_one (struct board *p,  double scale, int shift_x, int shift_y) {
	
	p->rep.center_off.x += (int)(shift_x/scale); 
	p->rep.center_off.y += (int)(shift_y/scale);
	p->rep.size.x += shift_x;
	p->rep.size.y += shift_y;
	
							//adjusting lines on shifting boards
	if (p->above_board != NULL) {					//if it's not the first board: the first board has no above line.	
		p->line->end.x += shift_x;		
		p->line->end.y += shift_y;
		//~ printf ("shift_line\n");
	}	
												 
	for (struct spawn *bl = p->below; bl != NULL; bl = bl->next) {		
		bl->board->line->start.x += shift_x;		//multiple lines can emerge from a single board
		bl->board->line->start.y += shift_y;	
	}
}
	
	
	
	
	




								//can't I fit this in the corresponding 'declare_new'?
void fit_in_list (struct board *new_board, struct board **list, struct list_lines **list_lines) {
	
	new_board->line->next = *list_lines;				//in the list, starting of
	new_board->line->prev = NULL;
	if (*list_lines)						//if this isn't the first line in the linked list
		(*list_lines)->prev = new_board->line;
	*list_lines = new_board->line;
									
									//fitting the board into the universal list
	new_board->next = *list;
	new_board->prev = NULL;
	if (*list)						//no need to use it yet, might in the future
		(*list)->prev = new_board;
	*list = new_board;
}	
	
	




	
	
	
						//Adds a new board, empty, right below the parent board. 
						//does not branch-link w/ infocus.
struct board *declare_new_board (int *n_boards, struct board *list, struct board *infocus, scaling scale) {
	


	struct board   *new_board = malloc (sizeof(struct board));
	
	if (new_board == NULL) {
		printf ("failed to add a board.");
		return NULL;
	}
	
	
								
							//need this. For some reason, setting the value of center_off coords through the rep.size struct does not work.
	struct whole_coords newcoord = {.x = infocus->rep.size.x, 
									.y = infocus->rep.size.y + (BOARD_SIZE + SPACE_BW)*scale.amount};

	
	for (int i = 0; i < 19; i++)
		for (int j = 0; j < 19; j++) {
			new_board->mech.state[i][j].S_no = 0;
			new_board->mech.state[i][j].colour = 0;
			new_board->mech.state[i][j].group = NULL;
			new_board->mech.state[i][j].merge = FALSE;
			new_board->mech.state[i][j].captured_groups = NULL;
		}
		
	new_board->mech.turn = 0,						//structures. Still dk for sure.
	new_board->mech.total_moves = 0,


	new_board->number = list->number + 1;
	new_board->first_move = NULL;
	new_board->last_move = NULL;
	new_board->selection = NULL;
	new_board->line = NULL;

	new_board->groups = NULL;
	new_board->num_groups = 0;

	new_board->rep.size.x = newcoord.x; 
	new_board->rep.size.y = newcoord.y; 
	new_board->rep.size.w = BOARD_SIZE; 
	new_board->rep.size.h = BOARD_SIZE;
			
	new_board->rep.center_off.x = (newcoord.x)/scale.amount - scale.center.x, 
	new_board->rep.center_off.y = (newcoord.y)/scale.amount - scale.center.y,
	new_board->rep.snap = NULL,
	
	
	new_board->above_board = infocus;
	new_board->below = NULL;
	
	new_board->prev = NULL;
	new_board->next = NULL;

							//making rep.snap transparent
	new_board->rep.snap = SDL_CreateTexture (renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, BOARD_SIZE, BOARD_SIZE);
	SDL_SetTextureBlendMode(new_board->rep.snap, SDL_BLENDMODE_BLEND);
	SDL_SetRenderTarget (renderer, new_board->rep.snap);
	SDL_SetRenderDrawColor (renderer, 0, 0, 0, 0);
	SDL_RenderClear(renderer);
	SDL_SetRenderTarget (renderer, NULL);
	
	(*n_boards)++;
	
	return new_board;
}

	
	
	
	
	
	
	
	
						//adds a new line, connecing the start and end boards.
struct list_lines *declare_new_line (struct board *start_board, struct board *end_board, scaling scale) {

	struct list_lines *new_line = malloc(sizeof(struct list_lines));
	double offset = (BOARD_SIZE/2) * scale.amount;
	
	new_line->start_board = start_board;		
	new_line->end_board   = end_board;
	
	new_line->start.x = new_line->start_board->rep.size.x + offset;
	new_line->start.y = new_line->start_board->rep.size.y + offset;
	new_line->end.x   = new_line->end_board->rep.size.x   + offset;
	new_line->end.y   = new_line->end_board->rep.size.y   + offset;
	
	new_line->number = end_board->number;
	
	return new_line;
}


		





	
					//opts in a board and all that spawned from it. To mass select, delete, shift.
void opt_in (struct board *p, struct opted **optList) {
	
	
	for (;;) {								//adding boards to the optList.
		
		struct opted *new_item = malloc(sizeof(struct opted));
		
		new_item->board = p;
		new_item->next = *optList;
		if (*optList != NULL)
			(*optList)->prev = new_item;
		new_item->prev = NULL;
		*optList = new_item;		//This is why I need a pointer to a pointer
		
		if (p->below == NULL)
			break;

										//moving horizontal, towards the right  (left?)
		for(struct spawn *q = p->below; q->next != NULL; q = q->next) 
			opt_in(q->next->board, optList);	
				
		
						
										//moving vertical, downwards
		p = p->below->board;
		
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

	
	SDL_SetRenderTarget (renderer, parts->board->rep.snap);
	SDL_RenderCopy(renderer, stoneNo_texture, NULL, &stoneNo_rect);
	SDL_FreeSurface(stoneNo_surface);
	SDL_DestroyTexture(stoneNo_texture);
	SDL_SetRenderTarget (renderer, NULL);
}





void print_liberties (playing_parts *parts) {
	
	//~ if (!parts->board->groups)
		//~ return;
				//resetting 
	SDL_SetTextureBlendMode(parts->board->liberties, SDL_BLENDMODE_BLEND);	//colouring a part of the texture transparent. 
	SDL_SetRenderTarget (renderer, parts->board->liberties);				
	SDL_SetRenderDrawColor (renderer, 0, 0, 0, 0);
	SDL_RenderFillRect (renderer, NULL);
	SDL_SetRenderTarget (renderer, NULL);
	
	
	for (struct group *walk = parts->board->groups; walk; walk = walk->next) {
		
		if (walk->colour == b) {
			parts->font_color.r = 255; parts->font_color.g = 255; parts->font_color.b = 0;
		}
		else {
			parts->font_color.r = 57; parts->font_color.g = 255; parts->font_color.b = 20;
		}
		
		parts->number = walk->number;
		
		for (struct liberty *stroll = walk->liberties; stroll; stroll = stroll->next) 
			put_liberty (stroll->coord.y, stroll->coord.x, parts);
	}
}



void put_liberty (int column, int row, playing_parts *parts) {

	
		
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

	
	SDL_SetRenderTarget (renderer, parts->board->liberties);
	SDL_RenderCopy(renderer, stoneNo_texture, NULL, &stoneNo_rect);
	SDL_FreeSurface(stoneNo_surface);
	SDL_DestroyTexture(stoneNo_texture);
	SDL_SetRenderTarget (renderer, NULL);
}





void coords_from_mouse (SDL_Event event, struct board *board, int *column, int *row, double scale_amount) {
	
	double x, y;
	
	x 	= 	(event.button.x - (board->rep.size.x + BORDER*scale_amount)) / (SQUARE_SIZE*scale_amount); 
	y 	= 	(event.button.y - (board->rep.size.y + BORDER*scale_amount)) / (SQUARE_SIZE*scale_amount);
	
	
									
	if ((x - (int)x) >= 0.5)
		 *column = (int)x + 1;
	else *column = (int)x;
	
	if ((y - (int)y) >= 0.5)
		 *row = (int)y + 1;
	else *row = (int)y;
}





bool isin_box (SDL_Rect rect, SDL_MouseButtonEvent button) {
	
	if (!(rect.x < button.x  &&  button.x < (rect.x + rect.w)))
		return FALSE;
	if (!(rect.y < button.y  &&  button.y < (rect.y + rect.h)))
		return FALSE;
		
	return TRUE;
}


//~ void delete_SLL (void *p) {
	
	//~ void *to_delete = p;
	//~ void *stroll = p->next;
	
	//~ while (1) {
		//~ free(to_delete);
		//~ if (!stroll)
			//~ break;
		//~ to_delete = stroll;
		//~ stroll = stroll->next;
	//~ }
//~ }
		
		
