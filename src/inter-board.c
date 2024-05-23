#include "include/inter-board.h"









void continue_play (int *n_boards, struct board **infocus, struct board **list, struct list_lines **list_lines, playing_parts *parts, scaling scale) {
	
	
	
	struct board *p;
	if ((p = add_board(n_boards, infocus, scale, list, list_lines)) == NULL)
		return;		
		
	/************************************************************************************************/						  
		
		//copying the config
								
	p->mech = p->above_board->mech;		
	
	p->groups = deep_copy_group(p->above_board->groups);
	
	for (int i = 0; i < 19; i++)
		for (int j = 0; j < 19; j++) 
			if (p->mech.state[i][j].group) 
				
						//setting the move's group pointer to the group on the new board
				for (struct group *walk = p->groups; walk; walk = walk->next) 
					if (p->mech.state[i][j].group->number == walk->number)		//DONT replace w/ group == walk !!
						p->mech.state[i][j].group = walk;
					
											
		
	/**************************************************************************************************/	
		
		
											//placing stones
	for (int column = 0; column < 19 ; column++) 
		for (int row = 0; row < 19; row++)  {
			if (p->mech.state[column][row].colour == 1) 
				place_stone (column, row, p, blackStone);
			else if (p->mech.state[column][row].colour == 2)
				place_stone (column, row, p, whiteStone);
		}
						


	if (p->above_board->last_move)		//if continue_play is called by offshoot
		return;
	
								//autopan:  after the offshoot check since it shouldn't be executed if called by offshoot.
	struct whole_coords shift;					
	shift.x = 0;
	shift.y = (int)(-(BOARD_SIZE + SPACE_BW) * scale.amount);
	pan (*list, *list_lines, scale, shift);

							
					//____Setting the last_move of the above_board____						
									
	struct board *q = p->above_board;		
	q->last_move = malloc(sizeof(struct stone));
					
	int i, j;	
	for (i = 0; i < 19; i++) {
		for (j = 0; j < 19; j++) 
			if (q->mech.state[i][j].S_no == q->mech.total_moves)
				break;
		if (j < 19)
			break;
	}
	
	q->last_move->S_no = q->mech.state[i][j].S_no;
	q->last_move->colour = q->mech.state[i][j].colour;
	q->last_move->column = i;
	q->last_move->row = j;

}								
	



void branch_window (struct board *p, struct board **list, struct list_lines **list_lines, int *n_boards, scaling *scale, struct board **infocus, playing_parts *parts,  bool shifting) {
	
	

	if (!p->first_move)
		return;
	
	parts->board = p;
	
	int current_move = p->mech.total_moves;
	*infocus = p;						//why?
	struct whole_coords shift;
	int copy_turn = p->mech.turn;    //copy made only for branch mode.

	
										//preserving the actual board state.
						//why not just preserve the entire board struct?				
	SDL_Texture *preserve = SDL_CreateTexture (renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, BOARD_SIZE, BOARD_SIZE);
	SDL_SetRenderTarget (renderer, preserve);
	SDL_RenderCopy(renderer, p->rep.snap, NULL, NULL);
	
	
	
						//back branch
	
	SDL_Texture *back_branch = SDL_CreateTexture (renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, BOARD_SIZE, BOARD_SIZE);
	SDL_SetTextureBlendMode(back_branch, SDL_BLENDMODE_BLEND);	//colouring a part of the texture transparent. 				

	if (p->above_board) {								//if it isn't the first_board
		
		SDL_SetRenderTarget (renderer, back_branch);
		SDL_RenderCopy(renderer, p->rep.snap, NULL, NULL);
		
		for (int k = p->mech.total_moves; k >= p->first_move->S_no; k--)
			for (int i = 0; i < 19; i++)
				for (int j = 0; j < 19; j++)				//do I have to set blend mode each time?
					if (p->mech.state[i][j].S_no == k) {
						SDL_Rect undoSize = { ((i*SQUARE_SIZE + BORDER) - 15), ((j*SQUARE_SIZE + BORDER) - 15), STONE_SIZE, STONE_SIZE};
						SDL_SetRenderDrawColor (renderer, 0, 0, 0, 0);
						SDL_RenderFillRect (renderer, &undoSize);
					}
					
		SDL_Texture *first_stone;
		if (p->above_board->last_move->colour == 2)
			first_stone = ghost_blackStone;
		else first_stone = ghost_whiteStone;
		
		for (struct spawn *b = p->above_board->below; b != NULL; b = b->next)
			if (b->board->first_move) {
				SDL_Rect stoneSize = { 	((b->board->first_move->column*SQUARE_SIZE + BORDER) - 15), 
									    ((b->board->first_move->row*SQUARE_SIZE + BORDER) - 15), STONE_SIZE, STONE_SIZE};
				SDL_RenderCopy(renderer, first_stone, NULL, &stoneSize);
			}	
	}	
		
		
	
	SDL_Texture *forward_branch = SDL_CreateTexture (renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, BOARD_SIZE, BOARD_SIZE);
	SDL_SetTextureBlendMode(forward_branch, SDL_BLENDMODE_BLEND);
	
	if (p->below) {								//if it has spawns
			
		SDL_SetRenderTarget (renderer, forward_branch);
		SDL_RenderCopy(renderer, p->rep.snap, NULL, NULL);
					
		SDL_Texture *first_stone;
		if (p->last_move->colour == 2)
			first_stone = ghost_blackStone;
		else first_stone = ghost_whiteStone;
		
		for (struct spawn *b = p->below; b != NULL; b = b->next)
			if (b->board->first_move) {
				SDL_Rect stoneSize = { 	((b->board->first_move->column*SQUARE_SIZE + BORDER) - 15), 
									    ((b->board->first_move->row*SQUARE_SIZE + BORDER) - 15), STONE_SIZE, STONE_SIZE};
				SDL_RenderCopy(renderer, first_stone, NULL, &stoneSize);
			}	
	}	
	
	
	
	
	
	
	
	
	
	
	if (shifting) {
		
		while (current_move > p->first_move->S_no) {
		
			parts->number = current_move - p->first_move->S_no + 1;	
					  
			int column, row;
			for (column = 0; column < 19; column++) {
				for (row = 0; row < 19; row++) {
					if (p->mech.state[column][row].S_no == current_move) 	
						break;
				}
				if (row < 19)
					break;
			}
			
			current_move--;	
			
			
			if (copy_turn) {
				place_stone(column, row, p, ghost_blackStone);
				parts->font_color.r = 255; parts->font_color.g = 255; parts->font_color.b = 255;
			}
			else { 
				place_stone(column, row, p, ghost_whiteStone);
				parts->font_color.r = 0; parts->font_color.g = 0; parts->font_color.b = 0;
			}
			put_number(column, row, parts);
			
			copy_turn++;
			copy_turn %= 2;
		}
	
	}
	
	
											
				//placing the branching indicator.
	
	SDL_Rect select_indicator = {	p->rep.size.x - (40 * scale->amount),
									p->rep.size.y - (40 * scale->amount),
									(BOARD_SIZE + 80) * scale->amount,
									(BOARD_SIZE + 80) * scale->amount	
								};
	
	SDL_SetRenderTarget (renderer, NULL);									
	

	SDL_RenderCopy (renderer, branchTex, NULL, &select_indicator);
	SDL_RenderCopy (renderer, bg_board, NULL, &(p->rep.size));
	SDL_RenderCopy (renderer, p->rep.snap, NULL, &(p->rep.size));
	SDL_RenderPresent (renderer);


	

	SDL_Event event;
	SDL_MouseButtonEvent pan_start;









	
									//input loop : 			//going back and forth in the branching window
	while (1) {												//branching, if new move
		
		if (current_move == (p->first_move->S_no - 1) && p->above_board) {
			SDL_SetRenderTarget (renderer, NULL);
			SDL_RenderCopy (renderer, bg_board, NULL, &(p->rep.size));
			SDL_RenderCopy (renderer, back_branch, NULL, &(p->rep.size));
			SDL_RenderPresent(renderer);
			
			while (!SDL_PollEvent(&event))
				;
				
			if (event.type == SDL_MOUSEBUTTONDOWN) {
				
				printf ("in--here\n");
				pan_start = event.button;
				while (!SDL_PollEvent(&event))
					;
				
				if (event.type == SDL_MOUSEMOTION)
					goto mousemotion;
				
				if (event.type != SDL_MOUSEBUTTONUP)
					continue;
				
				if (!isin_box((*infocus)->rep.size, event.button))
					continue;
				
				
				
				double x, y;
				int column, row;
				
				x 	= 	(event.button.x - (p->rep.size.x + BORDER*scale->amount)) / (SQUARE_SIZE*scale->amount); 
				y 	= 	(event.button.y - (p->rep.size.y + BORDER*scale->amount)) / (SQUARE_SIZE*scale->amount);
				
															
				if ((x - (int)x) >= 0.5)
					 column = (int)x + 1;
				else column = (int)x;
				
				if ((y - (int)y) >= 0.5)
					 row = (int)y + 1;
				else row = (int)y;
				
				for (struct spawn *b = p->above_board->below; b != NULL; b = b->next) {
					if (b->board->first_move->column != column)
						continue;
					if (b->board->first_move->row != row)
						continue;
					
					
					SDL_RenderCopy (renderer, modeTex_undo, NULL, &select_indicator);
					SDL_RenderCopy (renderer, bg_board, NULL, &(p->rep.size));
					SDL_SetRenderTarget (renderer, p->rep.snap);
					SDL_RenderCopy(renderer, preserve, NULL, NULL);
					SDL_SetRenderTarget (renderer, NULL);
					SDL_RenderCopy (renderer, p->rep.snap, NULL, &(p->rep.size));
					
					SDL_RenderPresent(renderer);
					
					//~ struct whole_coords shift;
					shift.x = p->rep.size.x - b->board->rep.size.x; 
					shift.y = p->rep.size.y - b->board->rep.size.y;
					pan (*list, *list_lines, *scale, shift);
					
					SDL_SetRenderDrawColor(renderer, 100, 200, 150, 255);
					SDL_RenderClear(renderer);
					render(*list);
					
					branch_window(b->board, list, list_lines, n_boards, scale, infocus, parts, 1);
					
					return;
				}
				
				SDL_RenderCopy (renderer, modeTex_undo, NULL, &select_indicator);
				SDL_RenderCopy (renderer, bg_board, NULL, &(p->rep.size));
				SDL_SetRenderTarget (renderer, p->rep.snap);
				SDL_RenderCopy(renderer, preserve, NULL, NULL);
				SDL_SetRenderTarget (renderer, NULL);
				SDL_RenderCopy (renderer, p->rep.snap, NULL, &(p->rep.size));
				
				SDL_RenderPresent(renderer);

				struct whole_coords old_coord = {p->rep.size.x, p->rep.size.y};
				*infocus = p->above_board;			//do I need to fix this? how?
				off_shoot(p, list, list_lines, infocus, parts, *scale, column, row, current_move, n_boards);
				
				shift.x = old_coord.x - (*list)->rep.size.x; 
				shift.y = old_coord.y - (*list)->rep.size.y;
				pan (*list, *list_lines, *scale, shift);
				
				return;
			}
		}
		else if (current_move == p->mech.total_moves && p->below) {
			SDL_SetRenderTarget (renderer, NULL);
			SDL_RenderCopy (renderer, bg_board, NULL, &(p->rep.size));
			SDL_RenderCopy (renderer, forward_branch, NULL, &(p->rep.size));
			SDL_RenderPresent(renderer);
			
			while (!SDL_PollEvent(&event))
				;
				
			if (event.type == SDL_MOUSEBUTTONDOWN) {
				
				printf ("in--here\n");
				pan_start = event.button;
				while (!SDL_PollEvent(&event))
					;
					
				if (event.type == SDL_MOUSEMOTION)
					goto mousemotion;
				
				if (event.type != SDL_MOUSEBUTTONUP)
					continue;
					
				if (!isin_box((*infocus)->rep.size, event.button))
					continue;
				
				double x, y;
				int column, row;
				
				x 	= 	(event.button.x - (p->rep.size.x + BORDER*scale->amount)) / (SQUARE_SIZE*scale->amount); 
				y 	= 	(event.button.y - (p->rep.size.y + BORDER*scale->amount)) / (SQUARE_SIZE*scale->amount);
				
															
				if ((x - (int)x) >= 0.5)
					 column = (int)x + 1;
				else column = (int)x;
				
				if ((y - (int)y) >= 0.5)
					 row = (int)y + 1;
				else row = (int)y;
				
				for (struct spawn *b = p->below; b != NULL; b = b->next) {
					if (!b->board->first_move)		//there are instances of no first_move possible here.
						continue;
					if (b->board->first_move->column != column)
						continue;
					if (b->board->first_move->row != row)
						continue;
					
					
					SDL_RenderCopy (renderer, modeTex_undo, NULL, &select_indicator);
					SDL_RenderCopy (renderer, bg_board, NULL, &(p->rep.size));
					SDL_SetRenderTarget (renderer, p->rep.snap);
					SDL_RenderCopy(renderer, preserve, NULL, NULL);
					SDL_SetRenderTarget (renderer, NULL);
					SDL_RenderCopy (renderer, p->rep.snap, NULL, &(p->rep.size));
					
					SDL_RenderPresent(renderer);
					
					//~ struct whole_coords shift;			
					shift.x = p->rep.size.x - b->board->rep.size.x; 
					shift.y = p->rep.size.y - b->board->rep.size.y;
					pan (*list, *list_lines, *scale, shift);
					
					SDL_SetRenderDrawColor(renderer, 100, 200, 150, 255);
					SDL_RenderClear(renderer);
					render(*list);
					
					branch_window(b->board, list, list_lines, n_boards, scale, infocus, parts, 1);
					
					return;
				}
				
				SDL_RenderCopy (renderer, modeTex_undo, NULL, &select_indicator);
				SDL_RenderCopy (renderer, bg_board, NULL, &(p->rep.size));
				SDL_SetRenderTarget (renderer, p->rep.snap);
				SDL_RenderCopy(renderer, preserve, NULL, NULL);
				SDL_SetRenderTarget (renderer, NULL);
				SDL_RenderCopy (renderer, p->rep.snap, NULL, &(p->rep.size));
				
				SDL_RenderPresent(renderer);

				
				*infocus = p;			//do I need to fix this? how?
				off_shoot(p, list, list_lines, infocus, parts, *scale, column, row, current_move, n_boards);
				
				shift.x = p->rep.size.x - (*list)->rep.size.x; 
				shift.y = p->rep.size.y - (*list)->rep.size.y;
				pan (*list, *list_lines, *scale, shift);
				
				return;
			}
		}
	    else 
			while (!SDL_PollEvent(&event))
				;	
		
		
		SDL_SetRenderTarget (renderer, p->rep.snap);
		
		
		
		if (event.type == SDL_MOUSEBUTTONDOWN) {
				
			pan_start = event.button;					//in case mousemotion
			while (!SDL_PollEvent(&event))
				;
				
				
		
			if (event.type == SDL_MOUSEMOTION) {
				
				
	mousemotion:	
				pan_manual (*list, *list_lines, &event, &pan_start, *scale);
				
				SDL_Rect select_indicator = {	p->rep.size.x - (40 * scale->amount),
												p->rep.size.y - (40 * scale->amount),
												(BOARD_SIZE + 80) * scale->amount,
												(BOARD_SIZE + 80) * scale->amount	
											};
				
				SDL_SetRenderTarget (renderer, NULL);									
				SDL_SetRenderDrawColor(renderer, 100, 200, 150, 255);
				SDL_RenderClear(renderer);

				SDL_RenderCopy (renderer, branchTex, NULL, &select_indicator);
				
				render(*list);
			}
				
				
				
			
			else if (event.type == SDL_MOUSEBUTTONUP && event.button.button == SDL_BUTTON_LEFT) {
				
				
				if (!isin_box((*infocus)->rep.size, event.button))
					continue;
				
				int column, row;
				coords_from_mouse (event, p, &column, &row, scale->amount);
				
									
				if (p->mech.state[column][row].S_no <= current_move)
					if (p->mech.state[column][row].S_no != 0)
						continue;								//if the spot clicked on is occupied by a move already
									
				if (p->mech.state[column][row].S_no != current_move + 1) {		//if the spot is not the next move, that is already played
					*infocus = split_board(n_boards, current_move, parts, &text, infocus, list, list_lines, *scale);
					off_shoot(p, list, list_lines, infocus, parts, *scale, column, row, current_move, n_boards);
					
					
						//this has to be here. can't be in continue_play because the boards are shifted in offshoot.
					struct whole_coords shift;
					shift.x = p->rep.size.x - (*list)->rep.size.x; 
					shift.y = p->rep.size.y - (*list)->rep.size.y;
					
					pan (*list, *list_lines, *scale, shift);
															
					return;
				}
				else {				//if the spot is the next move that is already played
					current_move++;
				
					parts->number = current_move - p->first_move->S_no + 1;
					
					if (copy_turn) {
						place_stone(column, row, p, whiteStone);
						parts->font_color.r = 0; parts->font_color.g = 0; parts->font_color.b = 0;
					}
					else { 
						place_stone(column, row, p, blackStone);
						parts->font_color.r = 255; parts->font_color.g = 255; parts->font_color.b = 255;
					}
					put_number(column, row, parts);
					
					copy_turn++;
					copy_turn %= 2;
					
					SDL_SetRenderTarget (renderer, NULL);	//this is needed of course so that render() renders the entire screen
					SDL_RenderCopy (renderer, bg_board, NULL, &(p->rep.size));
					SDL_RenderCopy (renderer, p->rep.snap, NULL, &(p->rep.size));
					SDL_RenderPresent(renderer);
					
				}
			}
		}
			
			
			
		if ((event.key.keysym.sym == SDLK_LEFT || event.key.keysym.sym == SDLK_a) && event.type == SDL_KEYUP) {
			
			//~ if (p->above_board != NULL) {				
				//~ if (current_move == p->above_board->mech.total_moves)		//disallowing undoing the moves of the parent board on the board below.
					//~ continue;
			//~ }
			//~ else if (current_move == 0) 
					//~ continue;
			
			printf ("left\n");
			
			if (!p->first_move)		//what is this for?
				continue;
			if (current_move == (p->first_move->S_no - 1))
				continue;
			
			parts->number = current_move - p->first_move->S_no + 1;	
			  
			int column, row;
			for (column = 0; column < 19; column++) {
				for (row = 0; row < 19; row++) {
					if (p->mech.state[column][row].S_no == current_move) 	
						break;
				}
				if (row < 19)
					break;
			}
			
			current_move--;	
			
			
			if (copy_turn) {
				place_stone(column, row, p, ghost_blackStone);
				parts->font_color.r = 255; parts->font_color.g = 255; parts->font_color.b = 255;
			}
			else { 
				place_stone(column, row, p, ghost_whiteStone);
				parts->font_color.r = 0; parts->font_color.g = 0; parts->font_color.b = 0;
			}
			put_number(column, row, parts);
			
			copy_turn++;
			copy_turn %= 2;
			
			SDL_SetRenderTarget (renderer, NULL);
			SDL_RenderCopy (renderer, bg_board, NULL, &(p->rep.size));
			SDL_RenderCopy (renderer, p->rep.snap, NULL, &(p->rep.size));
			SDL_RenderPresent(renderer);
		} 
		
		
		
		else if ((event.key.keysym.sym == SDLK_RIGHT || event.key.keysym.sym == SDLK_d) && event.type == SDL_KEYUP) {
			
			if (current_move == p->mech.total_moves)
				continue;
				
			current_move++;			//this has to be b4 the changes, unlike when going backwards.
		/*	
			if (p->node->above != NULL) 		
				parts.number = current_move - p->node->above->last_move->S_no;		
			else parts.number = current_move;
		*/
			parts->number = current_move - p->first_move->S_no + 1;
			
			int column, row;
			for (column = 0; column < 19; column++) {
				for (row = 0; row < 19; row++) {
					if (p->mech.state[column][row].S_no == current_move) 	
						break;
				}
				if (row < 19)
					break;
			}
			
			
			if (copy_turn) {
				place_stone(column, row, p, whiteStone);
				parts->font_color.r = 0; parts->font_color.g = 0; parts->font_color.b = 0;
			}
			else { 
				place_stone(column, row, p, blackStone);
				parts->font_color.r = 255; parts->font_color.g = 255; parts->font_color.b = 255;
			}
			put_number(column, row, parts);
			
			copy_turn++;
			copy_turn %= 2;
						
			SDL_SetRenderTarget (renderer, NULL);	//this is needed of course so that render() renders the entire screen
			SDL_RenderCopy (renderer, bg_board, NULL, &(p->rep.size));
			SDL_RenderCopy (renderer, p->rep.snap, NULL, &(p->rep.size));
			SDL_RenderPresent(renderer);			
		}
			
			
		else if (event.type == SDL_MOUSEWHEEL) {
			
			zoom_coords (*list, *list_lines, event, scale);
			SDL_Rect select_indicator = {	p->rep.size.x - (40 * scale->amount),
									p->rep.size.y - (40 * scale->amount),
									(BOARD_SIZE + 80) * scale->amount,
									(BOARD_SIZE + 80) * scale->amount	
								};
	
			SDL_SetRenderTarget (renderer, NULL);									
			SDL_SetRenderDrawColor(renderer, 100, 200, 150, 255);
			SDL_RenderClear(renderer);
		
			SDL_RenderCopy (renderer, branchTex, NULL, &select_indicator);
			
			render(*list);
		}
		
		
		
		else if (event.key.keysym.sym == SDLK_q) {
			SDL_SetRenderTarget (renderer, p->rep.snap);
			SDL_RenderCopy(renderer, preserve, NULL, NULL);
			SDL_SetRenderTarget (renderer, NULL);
			//p->node->board.mech.turn = preserve_turn;
			break;
		}
		
	}
}
	



//what is p for here?

void off_shoot (struct board *p, struct board **list, struct list_lines **list_lines, struct board **infocus, playing_parts *parts, scaling scale, int column, int row, int moveNum, int *n_boards) {
	
	
	continue_play(n_boards, infocus, list, list_lines, parts, scale);	//places the common moves					//removes infocus
	
	/*
							//__placing the first move, of the offshoot line__
	if ((*list)->mech.turn) {
		place_stone(column, row, *list, whiteStone);
		parts->font_color.r = 0; parts->font_color.g = 0; parts->font_color.b = 0;
	}
	else { 
		place_stone(column, row, *list, blackStone);
		parts->font_color.r = 255; parts->font_color.g = 255; parts->font_color.b = 255;
	}
	parts->board = *list;
	parts->number = 1;
	put_number(column, row, parts);
	
	(*list)->mech.state[column][row].S_no = ++((*list)->mech.total_moves);							
	(*list)->mech.state[column][row].colour = (*list)->mech.turn + 1;  // + 1 because the colour enum has "empty" as the first element.  
	((*list)->mech.turn)++; 
	(*list)->mech.turn %= 2;
	
						//setting the first_move of the offshoot board.
	
	(*list)->first_move = malloc(sizeof(struct stone));
	(*list)->first_move->S_no = (*list)->mech.state[column][row].S_no;
	(*list)->first_move->colour = (*list)->mech.state[column][row].colour;
	(*list)->first_move->column = column;
	(*list)->first_move->row = row;	
	*/
	
	parts->board = *list;
	play_move(column, row, parts);					
				
				
				
				
				
	/*************************************************************************************************/		
	
											
		//shifting, making space for the new offshoot line
	
	struct spawn *max = (*list)->above_board->below->next;
	int biggest_coord = (*list)->above_board->below->next->board->rep.size.x;
	for (struct spawn *walk = max->next; walk != NULL; walk = walk->next)
		if (walk->board->rep.size.x > biggest_coord) {
			biggest_coord = walk->board->rep.size.x;
			max = walk;
		}
				
	int divide = max->board->rep.size.x + BOARD_SIZE;
	int shift_left = -(int)(((BOARD_SIZE/2) + 25)*scale.amount);
	int shift_right = (int)(((BOARD_SIZE/2) + 25)*scale.amount);
	
	(*list)->rep.size.x = max->board->rep.size.x;
	(*list)->rep.center_off.x = ((*list)->rep.size.x)/scale.amount - scale.center.x;
	(*list)->line->end.x = (*list)->rep.size.x + (BOARD_SIZE/2) * scale.amount;
	shift_one (*list, scale.amount, shift_right, 0);
	
	
	for (struct board *p = *list; p != NULL; p = p->next) {
		if (p->number == (*list)->above_board->number || p->number == (*list)->number)
			continue;
		if (p->rep.size.x < divide) {
			shift_one (p, scale.amount, shift_left, 0);
		}
	}
	
	for (struct board *p = *list; p != NULL; p = p->next) {
		if (p == (*list)->above_board)	//not needed?
			continue;
		if (p->rep.size.x > divide)
			shift_one (p, scale.amount, shift_right, 0);
	}		
}
	
	
		

		
		
void split_mode (struct board *p, int *n_boards, struct board **list, struct board **infocus, struct list_lines **list_lines, scaling *scale, playing_parts *parts) {
	

	if (!p->first_move)
		return;
	
	parts->board = p;
	
	int current_move = p->mech.total_moves;
	*infocus = p;
	//~ struct whole_coords shift;
	//~ int copy_turn = p->mech.turn;    //copy made only for split mode.

	
										//preserving the actual board state.
						//why not just preserve the entire board struct?				
	SDL_Texture *preserve = SDL_CreateTexture (renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, BOARD_SIZE, BOARD_SIZE);
	SDL_SetRenderTarget (renderer, preserve);
	SDL_RenderCopy(renderer, p->rep.snap, NULL, NULL);

	
	
	
		//split mode indicator
	
	SDL_Rect select_indicator = {	p->rep.size.x - (40 * scale->amount),
									p->rep.size.y - (40 * scale->amount),
									(BOARD_SIZE + 80) * scale->amount,
									(BOARD_SIZE + 80) * scale->amount	
								};
	
	SDL_SetRenderTarget (renderer, NULL);									
	

	SDL_RenderCopy (renderer, splitTex, NULL, &select_indicator);
	SDL_RenderCopy (renderer, bg_board, NULL, &(p->rep.size));
	SDL_RenderCopy (renderer, p->rep.snap, NULL, &(p->rep.size));
	SDL_RenderPresent (renderer);





	int column, row;
	for (int walk = p->first_move->S_no; walk <= current_move; walk++)
		for (column = 0; column < 19; column++) {
			for (row = 0; row < 19; row++) {
				if (p->mech.state[column][row].S_no == walk) 	{
					SDL_Rect stoneSize = { ((column*SQUARE_SIZE + BORDER) - 15), ((row*SQUARE_SIZE + BORDER) - 15), STONE_SIZE, STONE_SIZE};	
					SDL_SetRenderTarget (renderer, p->rep.snap);
					SDL_RenderCopy(renderer, highlight_stone, NULL, &stoneSize);	
					break;
				}
			}
			if (row < 19)
				break;
		}
	SDL_SetRenderTarget (renderer, NULL);	
	SDL_RenderCopy (renderer, bg_board, NULL, &(p->rep.size));
	SDL_RenderCopy (renderer, p->rep.snap, NULL, &(p->rep.size));
	SDL_RenderPresent(renderer);
	
	
	SDL_Rect stoneSize = {.w  = STONE_SIZE, .h = STONE_SIZE};  
	SDL_Rect undoSize =  {.w  = STONE_SIZE, .h = STONE_SIZE};  

	SDL_Event event;
	SDL_MouseButtonEvent pan_start;
	
	while(1) {
		
		while (!SDL_PollEvent(&event))
			;
			
		switch (event.type) {
		
		
		
			case SDL_KEYDOWN: 
				printf ("input\n");
		
				switch (event.key.keysym.sym) {
					
						case SDLK_LEFT: case SDLK_a:		if (current_move == p->first_move->S_no)
																continue;
															  
															for (column = 0; column < 19; column++) {
																for (row = 0; row < 19; row++) {
																	if (p->mech.state[column][row].S_no == current_move) 	
																		break;
																}
																if (row < 19)
																	break;
															}
															//~ printf ("left\n");
															
															undoSize.x = (column * SQUARE_SIZE + BORDER) - 15; 
															undoSize.y = (row	 * SQUARE_SIZE + BORDER) - 15;
	
															SDL_SetTextureBlendMode(p->rep.snap, SDL_BLENDMODE_BLEND);	//colouring a part of the texture transparent. 
															SDL_SetRenderTarget (renderer, p->rep.snap);				
															SDL_SetRenderDrawColor (renderer, 0, 0, 0, 0);
															SDL_RenderFillRect (renderer, &undoSize);
															SDL_SetRenderTarget (renderer, NULL);
															
															
															parts->number = current_move - p->first_move->S_no + 1;
															if (p->mech.state[column][row].colour == 1) {
																place_stone(column, row, p, blackStone);
																parts->font_color.r = 255; parts->font_color.g = 255; parts->font_color.b = 255;
															}
															else { 
																place_stone(column, row, p, whiteStone);
																parts->font_color.r = 0; parts->font_color.g = 0; parts->font_color.b = 0;
															}
															put_number(column, row, parts);
															
															current_move--;	
															
															SDL_SetRenderTarget (renderer, NULL);
															SDL_RenderCopy (renderer, bg_board, NULL, &(p->rep.size));
															SDL_RenderCopy (renderer, p->rep.snap, NULL, &(p->rep.size));
															SDL_RenderPresent(renderer);
															
															break;
					
		
						
						case SDLK_RIGHT: case SDLK_d:		if (current_move == p->mech.total_moves)
																continue;
																
															current_move++;			//this has to be b4 the changes, unlike when going backwards.
														
															
															for (column = 0; column < 19; column++) {
																for (row = 0; row < 19; row++) {
																	if (p->mech.state[column][row].S_no == current_move) 	
																		break;
																}
																if (row < 19)
																	break;
															}
															printf ("right\n");
														
															
															stoneSize.x = (column*SQUARE_SIZE + BORDER) - 15; 
															stoneSize.y = (row*SQUARE_SIZE + BORDER) - 15;
															
															SDL_SetRenderTarget (renderer, p->rep.snap);
															SDL_RenderCopy(renderer, highlight_stone, NULL, &stoneSize);
																		
															SDL_SetRenderTarget (renderer, NULL);	//this is needed of course so that render() renders the entire screen
															SDL_RenderCopy (renderer, bg_board, NULL, &(p->rep.size));
															SDL_RenderCopy (renderer, p->rep.snap, NULL, &(p->rep.size));
															SDL_RenderPresent(renderer);	

															break;
									
									
									
						case SDLK_q:						SDL_SetRenderTarget (renderer, p->rep.snap);
															SDL_RenderCopy(renderer, preserve, NULL, NULL);
															SDL_SetRenderTarget (renderer, NULL);
									
															return;
															
															
						case SDLK_LSHIFT:					SDL_SetRenderTarget (renderer, p->rep.snap);
															SDL_RenderCopy(renderer, preserve, NULL, NULL);
															SDL_SetRenderTarget (renderer, NULL);
															
															if (current_move == p->mech.total_moves) {
																printf ("getting out\n");
																return;
															}
															split_board(n_boards, current_move, parts, &text, infocus, list, list_lines, *scale);	
															render (*list);
															return;
				}
				
				
			case SDL_MOUSEBUTTONDOWN: 
				
						pan_start = event.button;					//in case mousemotion
						while (!SDL_PollEvent(&event))
							;
		
						switch (event.type) {
		
								case SDL_MOUSEMOTION:  		pan_manual (*list, *list_lines, &event, &pan_start, *scale);
											
															
															select_indicator.x = p->rep.size.x - (40 * scale->amount);
															select_indicator.y = p->rep.size.y - (40 * scale->amount);
															
															
															SDL_SetRenderTarget (renderer, NULL);									
															SDL_SetRenderDrawColor(renderer, 100, 200, 150, 255);
															SDL_RenderClear(renderer);
											
															SDL_RenderCopy (renderer, splitTex, NULL, &select_indicator);
															
															render(*list);
															break;
															
							}				
						
						break;
						
								
								
			case SDL_MOUSEWHEEL: 		zoom_coords (*list, *list_lines, event, scale);
															
										select_indicator.x = p->rep.size.x - (40 * scale->amount);
										select_indicator.y = p->rep.size.y - (40 * scale->amount);
										select_indicator.w = (BOARD_SIZE + 80) * scale->amount;
										select_indicator.h = (BOARD_SIZE + 80) * scale->amount;
										
										SDL_SetRenderTarget (renderer, NULL);									
										SDL_SetRenderDrawColor(renderer, 100, 200, 150, 255);
										SDL_RenderClear(renderer);
									
										SDL_RenderCopy (renderer, splitTex, NULL, &select_indicator);
										
										render(*list);
										break;
						
																							
											
		}
	}
}




	
struct board *split_board (int *n_boards, int moveNum, playing_parts *parts, struct message *text, struct board **infocus, struct board **list, struct list_lines **list_lines, scaling scale) {


	// not just add_board because splitting a board requires a different linking with above and below boards. 
	// The below board isn't just NULL, but the boards below the above board.
	
	struct board *new_board_1 = declare_new_board(n_boards, *list, *infocus, scale);	
	new_board_1->line = declare_new_line (*infocus, new_board_1, scale);
	fit_in_list (new_board_1, list, list_lines);
	
	
	
	
	//______________________Adjusting the new board in between the infocus and its spawns______________________________
								
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
		
	 
	/*************************************************************************************************/
	 
	 
				// copying the config -  copying the group structs since otherwise any changes will reflect everywhere there is a pointer to it, in the previous boards.
	 
	 
	
	new_board_1->mech = (*infocus)->mech;						
	
	new_board_1->groups = deep_copy_group((*infocus)->groups);
	new_board_1->captured_groups = deep_copy_group((*infocus)->captured_groups);


	struct group **ptr = &new_board_1->captured_groups, *temp;
	while (*ptr != NULL && ((*ptr)->capturing_move_S_no > moveNum))		//moveNum is the last move of the first board
		ptr = &(*ptr)->next;
		
	while (*ptr != NULL && ((*ptr)->capturing_move_S_no <= moveNum)) {
		temp = *ptr;
		*ptr = (*ptr)->next;
		free(temp);
	}

	
	for (int i = 0; i < 19; i++)
		for (int j = 0; j < 19; j++) 
			if (new_board_1->mech.state[i][j].group) 
				
				for (struct group *walk = new_board_1->groups; walk; walk = walk->next) 		//making the moves' group pointer point to the group on the new board
					if (new_board_1->mech.state[i][j].group->number == walk->number)
						new_board_1->mech.state[i][j].group = walk;
						
				
			
				
				
	/**************************************************************************************************/	
				
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
		
	/**************************************************************************************************/	
		
		
	counter--;
		
								//removing moves after the split in the parent board
	for ( ; counter > moveNum; --counter) 
		undo_move(*infocus, infocus, text, TRUE, parts);
		
		
	
		
		
		
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
		
	//the split_function could also be called by itself, not being a part of branching. Then, it should set infocus to NULL
	*infocus = NULL;					
	return new_board_1->above_board;
	
}
										
										
	
		
void combine_board (struct board *p, playing_parts *parts, scaling scale, struct list_lines **list_lines) {
	
	
	if (!p->below)
		return;
		
	if (p->below->next)
		return;
	
	struct board *b = p->below->board;
	b->first_move = p->first_move;			//first move of the board below is adjusted
	struct whole_coords old_coord = {p->rep.size.x, p->rep.size.y}; 	//to place the combined board right in the old boards place.

	
	int offset;
	if (p->above_board)
		offset = p->above_board->mech.total_moves;
	else offset = 0;
	
	
	/**************************************************************************************************/

				//making the combined board out of the below board-  by reprinting moves from the 
				//readjusted first move.
	
	for (int column, row, counter = 1; counter + offset <= b->mech.total_moves; counter++)			//moveNum is the number of the last common move.
		for (column = 0; column < 19; column++) { 
			for (row = 0; row < 19; row++)	
				if (b->mech.state[column][row].S_no == counter + offset) 	{
					
					if (b->mech.state[column][row].colour == 1) {
						place_stone (column, row, b, blackStone);
						parts->font_color.r = 255; parts->font_color.g = 255; parts->font_color.b = 255; 
					}
					else if (b->mech.state[column][row].colour == 2) {
						place_stone (column, row, b, whiteStone);
						parts->font_color.r = 0; parts->font_color.g = 0; parts->font_color.b = 0;
					}	
					parts->number = counter;
					parts->board = b;
					put_number(column, row, parts);
					break;
				}		
			if (row < 19)
				break;
		}
		
		
	/**************************************************************************************************/
				
				//branch linking
	
	if (p->above_board) {
		for (struct spawn *walk = p->above_board->below; walk; walk = walk->next)
			if (walk->board->number == p->number) {
				walk->board = b;
				break;
			}
		b->line->start_board = p->above_board;
		b->line->start.x = p->above_board->rep.size.x + (BOARD_SIZE/2) * (scale.amount);
		b->line->start.y = p->above_board->rep.size.y + (BOARD_SIZE/2) * (scale.amount);
		
		if (p->line->prev)							//taking the first board's line out of the list.
			p->line->prev->next = p->line->next;
		if (p->line->next)
			p->line->next->prev = p->line->prev;
		free(p->line);
	}	
	else  {
		if (b->line->prev)
			b->line->prev->next = b->line->next;
		else *list_lines = b->line->next;
		if (b->line->next)
			b->line->next->prev = b->line->prev;
		free(b->line);
		b->line = NULL;
	}
		
	
	b->above_board = p->above_board;
	//~ b->number = p->number;  //need this, otherwise there will be two boards w/ the same number (really? how?). Which creates a problem in offshoot shifting. 
	
	if (p->prev)					//taking the first board out of the list.
		p->prev->next = p->next;
	if (p->next)
		p->next->prev = p->prev;
	free(p);
	

	/**************************************************************************************************/
		
		//shifting all the boards below, upwards.
									
	int shift_x = old_coord.x - b->rep.size.x;	
	int shift_y = old_coord.y - b->rep.size.y;	
	shift_one (b, scale.amount, shift_x, shift_y); 

	for (struct spawn *walk = b->below; walk != NULL; walk = walk->next) {
		recur_shift(walk->board, scale.amount, 0, shift_y);
	}

}
		
		


