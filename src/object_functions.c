#include "include/object_functions.h"








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
	
	





// add_board and delete_board involve inter-board linking, more than just treating boards and lines as objects.
// But still these involve no board state mechanisms, and fit here the best
			
			
struct board* add_board (int *n_boards, struct board **infocus, scaling scale, struct board **list, struct list_lines **list_lines)  	{
	
	
	if (*infocus == NULL) 
		return NULL;
		
	
	struct board *new_board = declare_new_board(n_boards, *list, *infocus, scale);
	
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
	if (p->above_board) {					//if it's not the first board: the first board has no above line.	
		p->line->end.x += shift_x;		
		p->line->end.y += shift_y;
		//~ printf ("shift_line\n");
	}	
												 
	for (struct spawn *bl = p->below; bl != NULL; bl = bl->next) {		
		bl->board->line->start.x += shift_x;		//multiple lines can emerge from a single board
		bl->board->line->start.y += shift_y;	
	}
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



