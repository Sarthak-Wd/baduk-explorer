#include "routine_functions.h"






void play_move (SDL_Event event, playing_parts *parts, scaling scale, struct message *text, struct board **infocus)  {
	
	
	
	if (parts->board->below != NULL) {
		text->str = "Can't make changes to a board which has children boards. Enter the branching mode to create a branch.";
		text->coord.x = 50;
		text->coord.y = 50;
		
		SDL_Color bg_color = {200, 180, 125, 255};
		SDL_Color txt_color = {255, 255, 255, 255};
		text->bg_color = bg_color;
		text->txt_color = txt_color;

		text->to_display = TRUE;
		return;
	}
	
	
	double x, y;
	int column, row;
	
	x 	= 	(event.button.x - (parts->board->rep.size.x + BORDER*scale.amount)) / (SQUARE_SIZE*scale.amount); 
	y 	= 	(event.button.y - (parts->board->rep.size.y + BORDER*scale.amount)) / (SQUARE_SIZE*scale.amount);
	
	
									
	if ((x - (int)x) >= 0.5)
		 column = (int)x + 1;
	else column = (int)x;
	
	if ((y - (int)y) >= 0.5)
		 row = (int)y + 1;
	else row = (int)y;
	
	if (parts->board->mech.state[column][row].colour != empty)
		return;
	
	
	//SDL_Color color;
	//int number;
	if (parts->board->above_board != NULL)		//won't it help to have a variable for the no. of stones placed on the current board?  
		parts->number = (parts->board->mech.total_moves + 1) - parts->board->above_board->last_move->S_no; 
	else parts->number = (parts->board->mech.total_moves + 1);
	
	
	if (parts->board->mech.turn) {
		place_stone (column, row, parts->board, parts->whiteStone);
		parts->font_color.r = 0; parts->font_color.g = 0; parts->font_color.b = 0;
	}
	else  {
		place_stone (column, row, parts->board, parts->blackStone);
		parts->font_color.r = 255; parts->font_color.g = 255; parts->font_color.b = 255;
	}
	put_number(column, row, parts);
	
	
	
	
								//updating the stats of the board.
				
	parts->board->mech.state[column][row].S_no = ++(parts->board->mech.total_moves);
									
	parts->board->mech.state[column][row].colour = parts->board->mech.turn + 1;  // + 1 because the colour enum has "empty" as the first element.  
	(parts->board->mech.turn)++; 
	parts->board->mech.turn %= 2;

	*infocus = parts->board;
	
	
	
						//setting the first_move 					
									
	if (parts->board->first_move == NULL) {		//if it is the first move on this board, (that is if it hasn't been set)
		
		parts->board->first_move = malloc(sizeof(struct stone));
		parts->board->first_move->S_no = parts->board->mech.state[column][row].S_no;
		parts->board->first_move->colour = parts->board->mech.state[column][row].colour;
		parts->board->first_move->column = column;
		parts->board->first_move->row = row;	
	}
	
}





void undo_move (struct board *p, struct board **infocus, struct message *text, bool branching)  {
	
	if (p->mech.total_moves <= 0)	
		return;
		
	if (p->above_board != NULL) 				//disallowing undoing the moves of the parent board on the bottom boards.
		if (p->mech.total_moves == p->above_board->mech.total_moves)
			return;	
	
	
	if (p->below != NULL && branching == FALSE) {
		text->str = "Can't make changes to a board which has children boards. Enter the branching mode to create a branch.";
		text->coord.x = 50;
		text->coord.y = 50;
		
		SDL_Color bg_color = {200, 180, 125, 255};
		SDL_Color txt_color = {255, 255, 255, 255};
		text->bg_color = bg_color;
		text->txt_color = txt_color;
	
		text->to_display = TRUE;
		return;
	}
		
	
	int i, j;
	
	for (i = 0; i < 19; i++) {
		for (j = 0; j < 19; j++) 	
			if (p->mech.state[i][j].S_no == p->mech.total_moves)
				break;	
		if (j < 19)
			break;
	}
							
								//removing the first move, if it is undone
	if (p->mech.state[i][j].S_no == p->first_move->S_no) {
		free(p->first_move);
		p->first_move = NULL;
	}
		
										//reverting the board state
	p->mech.total_moves--;
	p->mech.state[i][j].S_no = 0;
	p->mech.state[i][j].colour = empty;
	(p->mech.turn)++; 						//shouldn't matter if I increment or decrement, but ++ because -- leads to -1. 
	p->mech.turn %= 2;
	
	
	*infocus = p;

	SDL_Rect undoSize = { ((i*SQUARE_SIZE + BORDER) - 15), ((j*SQUARE_SIZE + BORDER) - 15), STONE_SIZE, STONE_SIZE};
	
	SDL_SetTextureBlendMode(p->rep.snap, SDL_BLENDMODE_BLEND);	//colouring a part of the texture transparent. 
	SDL_SetRenderTarget (renderer, p->rep.snap);				
	SDL_SetRenderDrawColor (renderer, 0, 0, 0, 0);
	SDL_RenderFillRect (renderer, &undoSize);
	SDL_SetRenderTarget (renderer, NULL);
}
	


			
			
			
			
			
struct board* add_board (int *n_boards, struct board **infocus, scaling scale, struct board **list, struct list_lines **list_lines)  	{
	
	
	if (*infocus == NULL) 
		return NULL;
		
	
	struct board *new_board = declare_new_board(n_boards, *infocus, scale);
	
	/*
									//in the branch: linking the parent and the spawn board through the 'above' and 'below' members

	new_board->above_board = *infocus;									//list is any board that was last declared. It is possible to add a board else where. I need something other than list here. So: infocus
	new_board->below = NULL;
	*/
	
	
								
	struct spawn *new_spawn = malloc(sizeof(struct spawn));	
	if (new_spawn == NULL)	
			printf ("couldn't allocate memory for spawn struct");
						
	new_spawn->board = new_board;
	new_spawn->next = (*infocus)->below;
	(*infocus)->below = new_spawn;
	
	
	new_board->line = declare_new_line (*infocus, new_board, scale);
								
	
									//fitting the item and line into the universal lists
	fit_in_list (new_board, list, list_lines);
	
	*infocus = NULL;
	
	return new_board;
}






	
struct board *split_board (int *n_boards, int moveNum, playing_parts *parts, struct message *text, struct board **infocus, struct board **list, struct list_lines **list_lines, scaling scale) {



	struct board *new_board_1 = declare_new_board(n_boards, *infocus, scale);	
	new_board_1->line = declare_new_line (*infocus, new_board_1, scale);
	fit_in_list (new_board_1, list, list_lines);
	//~ new_board_1->above_board = infocus;
	
	
	
	
					//____Adjusting the new board in between the infocus and its spawns____
								
	if ((*infocus)->below != NULL) {
		
		new_board_1->below = (*infocus)->below;				 //this is only needed once, so outside of loop.
		
		for (struct spawn *b = (*infocus)->below; b != NULL; ) {	
			b->board->above_board = new_board_1;			//last_move to be updated
			b->board->line->start_board = new_board_1;
		
			b->board->line->start.x = new_board_1->rep.size.x + (BOARD_SIZE/2) * scale.amount;
			b->board->line->start.y = new_board_1->rep.size.y + (BOARD_SIZE/2) * scale.amount;
							
			if (b->next == NULL)
				break;
			b = b->next; 
		}
	}

	
	if (((*infocus)->below = malloc(sizeof(struct spawn))) == NULL)	//have to allocate new memory or else it would save it in the old location, that is, the below link of the new board. 
		printf ("couldn't allocate memory for spawn struct");	
	(*infocus)->below->board = new_board_1;		
	(*infocus)->below->next = NULL;
	
	
				//shifting the boards
					
	int shift_y = (int)((BOARD_SIZE + SPACE_BW)*scale.amount);
	
	for (struct spawn *walk = new_board_1->below; walk != NULL; walk = walk->next) 
		recur_shift (walk->board, scale.amount, 0, shift_y);
	 
	 
	 
	 
	new_board_1->mech = (*infocus)->mech;		// copying the config				
	
	
				
				
				
					//____PLACING STONES____
						
	int counter = 1;
		
	int x, y;								//__common stones__
	for (; counter <= moveNum; counter++)
														//moveNum is the number of the last common move.
		for (x = 0; x < 19; x++) {
			for (y = 0; y < 19; y++)
				if (new_board_1->mech.state[x][y].S_no == counter) {
					if (new_board_1->mech.state[x][y].colour == 1) 
						place_stone (x, y, new_board_1, blackStone);
					else if (new_board_1->mech.state[x][y].colour == 2)
						place_stone (x, y, new_board_1, whiteStone);
					break;
				}		
			if (y < 19)
				break;
		}
	
											//__stones after split__
	
	for (int column, row; counter <= new_board_1->mech.total_moves; counter++)			//moveNum is the number of the last common move.
		for (column = 0; column < 19; column++) { 
			for (row = 0; row < 19; row++)	
				if (new_board_1->mech.state[column][row].S_no == counter) 	{
					if (new_board_1->mech.state[column][row].colour == 1) {
						place_stone (column, row, new_board_1, blackStone);
						parts->font_color.r = 255; parts->font_color.g = 255; parts->font_color.b = 255; 
					}
					else if (new_board_1->mech.state[column][row].colour == 2) {
						place_stone (column, row, new_board_1, whiteStone);
						parts->font_color.r = 0; parts->font_color.g = 0; parts->font_color.b = 0;
					}
					parts->number = counter - moveNum;
					parts->board = new_board_1;
					put_number(column, row, parts);
					break;
				}		
			if (row < 19)
				break;
		}
		
	counter--;
		
								//removing moves after the split in the parent board
	for ( ; counter > moveNum; --counter) 
		undo_move(*infocus, infocus, text, TRUE);
		
		
		
		
		
		
					//____FIRST & LAST MOVES____
		
	if (!(*infocus)->last_move)
		(*infocus)->last_move = malloc(sizeof(struct stone));
	new_board_1->first_move = malloc(sizeof(struct stone));
	if (new_board_1->below)
		new_board_1->last_move = malloc(sizeof(struct stone));
	
	
									//first and last moves, new_board_1
									//last move, infocus
	for (int x = 0; x < 19; x++) 
		for (int y = 0; y < 19; y++)	{
			if ((*infocus)->mech.state[x][y].S_no == moveNum) {
				(*infocus)->last_move->S_no = (*infocus)->mech.state[x][y].S_no; 
				(*infocus)->last_move->colour = (*infocus)->mech.state[x][y].colour; 
				(*infocus)->last_move->column = x; 
				(*infocus)->last_move->row = y; 
			}
			if (new_board_1->mech.state[x][y].S_no == moveNum + 1) {
				new_board_1->first_move->S_no = new_board_1->mech.state[x][y].S_no; 
				new_board_1->first_move->colour = new_board_1->mech.state[x][y].colour; 
				new_board_1->first_move->column = x; 
				new_board_1->first_move->row = y; 
			}
			if (new_board_1->below == NULL)
				continue;
			if (new_board_1->mech.state[x][y].S_no == new_board_1->mech.total_moves) {
				new_board_1->last_move->S_no = new_board_1->mech.state[x][y].S_no; 
				new_board_1->last_move->colour = new_board_1->mech.state[x][y].colour; 
				new_board_1->last_move->column = x; 
				new_board_1->last_move->row = y; 
			}
		}
		
	
	*infocus = NULL;
	return new_board_1->above_board;
	
}
										
										
											
											
	



void delete_board (struct board *p, int *n_boards, struct opted **sel, struct board **infocus, struct board **list, struct list_lines **list_lines) {	
	
	if (!p->above_board)
		return;
					//__removing the below link of the parent board corresponding to the first board to be deleted, from the below list.__ 
	
	struct spawn *walk = p->above_board->below;
	struct spawn *prev = NULL;
	 		 									
	for (; walk != NULL && walk->board->number != p->number; 
			prev = walk, walk = walk->next )
			;
		
	if (walk == NULL)									//board wasn't found? Can't happen but just for safety.
		return;
		
	if (prev == NULL)	
		p->above_board->below = walk->next;	
	else 
		prev->next = walk->next;
		
	free(walk);
	
	if (!p->above_board->below) {
		free(p->above_board->last_move);
		p->above_board->last_move = NULL;
	}
	
	
	
					//__removing all elements below, starting from the board opted__
	
	struct opted *optList = NULL;
	
	opt_in(p, &optList);
	
	
											//adjusting pointers and deleting
	for (struct list_lines *q; optList != NULL; optList = optList->next) {

								// *** removing elements from the linked lists ***
		p = optList->board;		
										// * lines *
		q = p->line;
		if (q->next != NULL)							//if q is not the only in list_lines
			q->next->prev = q->prev;
		if (q->prev != NULL)							//if q is not the last in list_lines 
			q->prev->next = q->next;
		else *list_lines = q->next;
		
										// * selection indicators *
		if (p->selection != NULL) 	{
			if (p->selection->prev == NULL) 			//if it is the first board in the list (last added?)
				*sel = p->selection->next;
			else p->selection->prev->next = p->selection->next;
			
			if (p->selection->next != NULL)				//if the selection is not the last in the list
				p->selection->next->prev = p->selection->prev;
		}
										// * boards *
		if (p->next != NULL)							//if p is not the only in list
			p->next->prev = p->prev;
		if (p->prev != NULL)							//if p is not the last in list
			p->prev->next = p->next;
		else *list = p->next;
		
								//freeing memory
		free (p->selection);							
		free(p->line);
		free(p);
		--(*n_boards);
		printf ("deleting\n");
	}
		
	*infocus = NULL;			//need to do this. Otherwise, if I delete an infocus board, and don't equate this to null, bug: a board, randomly filled will appear on the left edge. 
	
	
}
		







void select_board (struct board *p, struct opted **sel) {
	
							
							//if the board is already selected, remove it from the selection
	if (p->selection != NULL) 	{
		if (p->selection->prev == NULL) 		//if it is the first board in the list (last added?)
			*sel = p->selection->next;
		else p->selection->prev->next = p->selection->next;
		
		if (p->selection->next != NULL)		//if the selection is not the last in the list
			p->selection->next->prev = p->selection->prev;
		
		free (p->selection);
		p->selection = NULL;
		return;
	}
		
	
	struct opted *new_node = malloc(sizeof(struct opted)); 
	new_node->board = p;
	
	new_node->next = *sel;
	new_node->prev = NULL;
	if (*sel != NULL)   		//if the list is not empty.
		(*sel)->prev = new_node;
	(*sel) = new_node; 
	
	p->selection = new_node;	
}
 
		





						//shifts selected boards
void shift_elements (struct opted *sel, SDL_Event *event, SDL_MouseButtonEvent *pan_start, scaling scale)  {
	
	int shift_x, shift_y;
	
	
	while (event->type != SDL_MOUSEBUTTONUP) 
		SDL_PollEvent(event);
		

	shift_x = event->button.x - pan_start->x;
	shift_y = event->button.y - pan_start->y;
	
	
	for (; sel != NULL; sel = sel->next)
		shift_one (sel->board, scale.amount, shift_x, shift_y);
	
}


