#include "routine_functions.h"






void play_move (SDL_Event event, playing_parts *parts, scaling scale, struct message *text, struct list **infocus)  {
	
	
	
	if (parts->item->node->below != NULL) {
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
	
	x 	= 	(event.button.x - (parts->item->node->board.rep.size.x + BORDER*scale.amount)) / (SQUARE_SIZE*scale.amount); 
	y 	= 	(event.button.y - (parts->item->node->board.rep.size.y + BORDER*scale.amount)) / (SQUARE_SIZE*scale.amount);
	
	
									
	if ((x - (int)x) >= 0.5)
		 column = (int)x + 1;
	else column = (int)x;
	
	if ((y - (int)y) >= 0.5)
		 row = (int)y + 1;
	else row = (int)y;
	
	if (parts->item->node->board.mech.state[column][row].colour != empty)
		return;
	
	
	//SDL_Color color;
	//int number;
	if (parts->item->node->above != NULL)		//won't it help to have a variable for the no. of stones placed on the current board?  
		parts->number = (parts->item->node->board.mech.total_moves + 1) - parts->item->node->above->last_move->S_no; 
	else parts->number = (parts->item->node->board.mech.total_moves + 1);
	
	
	if (parts->item->node->board.mech.turn) {
		place_stone (column, row, &(parts->item->node->board), parts->whiteStone);
		parts->font_color.r = 0; parts->font_color.g = 0; parts->font_color.b = 0;
	}
	else  {
		place_stone (column, row, &(parts->item->node->board), parts->blackStone);
		parts->font_color.r = 255; parts->font_color.g = 255; parts->font_color.b = 255;
	}
	put_number(column, row, parts);
	
	
	
	
								//updating the stats of the board.
				
	parts->item->node->board.mech.state[column][row].S_no = ++(parts->item->node->board.mech.total_moves);
									
	parts->item->node->board.mech.state[column][row].colour = parts->item->node->board.mech.turn + 1;  // + 1 because the colour enum has "empty" as the first element.  
	(parts->item->node->board.mech.turn)++; 
	parts->item->node->board.mech.turn %= 2;

	*infocus = parts->item;
	
	
	
						//putting the first move of the new board in the below link of the board above it.					
	
	if (parts->item->node->above == NULL)		//if it's the first board, it won't have a board above.
		return;
									
	if (parts->item->node->above->last_move->S_no == (parts->item->node->board.mech.total_moves - 1)) {		//if it is the first move on this board.
		struct list *q = parts->item->node->above->item;
		*(q->node->below->first_move) = parts->item->node->board.mech.state[column][row];
	}
	
}





void undo_move (struct list *p, struct list **infocus, struct message *text, bool branching)  {
	
	if (p->node->board.mech.total_moves <= 0)	
		return;
		
	if (p->node->above != NULL) 				//disallowing undoing the moves of the parent board on the bottom boards.
		if (p->node->board.mech.total_moves == p->node->above->last_move->S_no)
			return;	
	
	
	if (p->node->below != NULL && branching == FALSE) {
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
			if (p->node->board.mech.state[i][j].S_no == p->node->board.mech.total_moves)
				break;	
		if (j < 19)
			break;
	}
		
										//reverting the board state
	p->node->board.mech.total_moves--;
	p->node->board.mech.state[i][j].S_no = 0;
	p->node->board.mech.state[i][j].colour = empty;
	(p->node->board.mech.turn)++; 						//shouldn't matter if I increment or decrement, but ++ because -- leads to -1. 
	p->node->board.mech.turn %= 2;
	
	printf ("turn after undo: %d\n\n", p->node->board.mech.turn);
	
	*infocus = p;

	SDL_Rect undoSize = { ((i*SQUARE_SIZE + BORDER) - 15), ((j*SQUARE_SIZE + BORDER) - 15), STONE_SIZE, STONE_SIZE};
	
	SDL_SetTextureBlendMode(p->node->board.rep.snap, SDL_BLENDMODE_BLEND);	//colouring a part of the texture transparent. 
	SDL_SetRenderTarget (renderer, p->node->board.rep.snap);				
	SDL_SetRenderDrawColor (renderer, 0, 0, 0, 0);
	SDL_RenderFillRect (renderer, &undoSize);
	SDL_SetRenderTarget (renderer, NULL);
}
	


			
			
			
			
			
struct list* add_board (int *n_boards, struct list **infocus, scaling scale, struct list **list, struct list_lines **list_lines)  	{
	
	
	if (*infocus == NULL) 
		return NULL;
		
	
	struct list *new_item = declare_new_board(n_boards, *infocus, scale);
	
	
									//in the branch: linking the parent and the spawn board through the 'above' and 'below' members
	
	if ((new_item->node->above = malloc(sizeof(struct parent))) == NULL)	//declared 'above' as a pointer in the list struct for consistency. It could just be a regular member since parent is not a linked list.
		printf ("couldn't allocate memory for spawn struct");
	new_item->node->above->item = *infocus;									//list is any board that was last declared. It is possible to add a board else where. I need something other than list here. So: infocus
	new_item->node->below = NULL;
	
	
	
								
	struct spawn *new_spawn = malloc(sizeof(struct spawn));	
	if (new_spawn == NULL)	
			printf ("couldn't allocate memory for spawn struct");
						
	new_spawn->item = new_item;
	new_spawn->next = (*infocus)->node->below;
	(*infocus)->node->below = new_spawn;
	
	
	
	
											//declaring a line, fitting it into branch
	struct list_lines *new_line = declare_new_line (*infocus, new_item, scale);						
									
	new_item->node->above->line = new_line;								
	for(struct spawn *b = new_item->node->above->item->node->below;
		b != NULL; b = b->next)
		if (b->item->number == new_item->number) {
			b->line = new_line;
			break;
		}
									
									//fitting the item and line into the universal lists
	fit_in_list (new_item, new_line, list, list_lines);
	
	*infocus = NULL;
	
	return *list;
}





void delete_board (struct list *p, int *n_boards, struct opted **sel, struct list **infocus, struct list **list, struct list_lines **list_lines) {	
	
	
	
									//removing the below item of the parent board corresponding to the first board to be deleted, from the below list. 
	
	struct spawn *walk = p->node->above->item->node->below;
	struct spawn *prev = NULL;
	 		 									
	for (; walk != NULL && walk->item->node->number != p->node->number; 
			prev = walk, walk = walk->next )
			;
		
	if (walk == NULL)	//board wasn't found? Can't happen but just for safety.
		return;
		
	if (prev == NULL)	
		p->node->above->item->node->below = walk->next;	
	else 
		prev->next = walk->next;
		
	free(walk);
	
	
	
	struct opted *optList = NULL;
	
	opt_in(p, &optList);
	
	
											//adjusting pointers and deleting
	for (struct list_lines *q; optList != NULL; optList = optList->next) {

								// *** removing elements from the linked lists ***
		p = optList->list;		
										// * lines *
		q = p->node->above->line;
		if (q->next != NULL)				//if q is not the only in list_lines
			q->next->prev = q->prev;
		if (q->prev != NULL)				//if q is not the last in list_lines 
			q->prev->next = q->next;
		else *list_lines = q->next;
		
										// * selection indicators *
		if (p->selection != NULL) 	{
			if (p->selection->prev == NULL) 		//if it is the first board in the list (last added?)
				*sel = p->selection->next;
			else p->selection->prev->next = p->selection->next;
			
			if (p->selection->next != NULL)		//if the selection is not the last in the list
				p->selection->next->prev = p->selection->prev;
		}
										// * boards *
		if (p->next != NULL)				//if p is not the only in list
			p->next->prev = p->prev;
		if (p->prev != NULL)				//if p is not the last in list
			p->prev->next = p->next;
		else *list = p->next;
		
								//freeing memory
		free (p->selection);							
		free(p->node->above->line);
		free(p);
		--(*n_boards);
		printf ("deleting\n");
	}
		
	*infocus = NULL;			//need to do this. Otherwise, if I delete an infocus board, and don't equate this to null, bug: a board, randomly filled will appear on the left edge. 
	
	
}
		







void select_board (struct list *p, struct opted **sel) {
	
							
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
	new_node->list = p;
	
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
		shift_one (sel->list, shift_x, shift_y, scale.amount);
	
}


