#include "include/play_state.h"

//~ struct group_list {
	//~ struct *group;
	//~ struct group_list *next;
//~ };

int group_id = 0;



	







void play_move (int column, int row, playing_parts *parts)  {
	
	
	if (parts->board->mech.state[column][row].colour != empty)
		return;
	
				 
	if (parts->board->above_board)		//won't it help to have a variable for the no. of stones placed on the current board?  
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

	
	
	
	
						//setting the first_move 					
									
	if (!parts->board->first_move) {		//if it is the first move on this board, (that is, if it hasn't been set)
		
		parts->board->first_move = malloc(sizeof(struct stone));
		parts->board->first_move->S_no = parts->board->mech.state[column][row].S_no;
		parts->board->first_move->colour = parts->board->mech.state[column][row].colour;
		parts->board->first_move->column = column;
		parts->board->first_move->row = row;	
	}
	
	
	group_stuff (column, row, parts->board);
}





void undo_move (struct board *p, struct board **infocus, struct message *text, bool branching, playing_parts *parts)  {
	
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
		
	parts->board = p;	
	undo_groups (i, j, p, parts);	
					
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
	










void group_stuff (int column, int row, struct board *board) {
	
	struct group *group;
	struct liberty *newL;
	struct member *newM;
	
	struct group *ally_groups[4];
	struct group *opp_groups[4];
	int n_allies = 0, n_opps = 0;
	//~ bool group_matched;
	
	//~ struct group_op_data data;
	
	
	
	
	
			//making a list of ally groups in contact
			//and checking the outfacing status of the adjacent ally stones 
	int n;
	struct member *tick;
	
	
	if ((column < 18) && (board->mech.state[column+1][row].colour == board->mech.state[column][row].colour)) {
		ally_groups[n_allies++] = board->mech.state[column+1][row].group;
		if ((column+1 == 18) || (board->mech.state[column+1 +1][row].colour == board->mech.state[column+1][row].colour))
			if ((row == 18) || (board->mech.state[column+1][row +1].colour == board->mech.state[column+1][row].colour))
				if (board->mech.state[column+1 -1][row].colour == board->mech.state[column+1][row].colour) 
					if ((row == 0) || (board->mech.state[column+1][row -1].colour == board->mech.state[column+1][row].colour)) 	
						for(tick = board->mech.state[column+1][row].group->members; tick != NULL; tick = tick->next)
							if (tick->coord.x == row && tick->coord.y == column+1) {
								tick->outfacing = FALSE;		//if it is adjacent to the placed stone, this must have been true.
								break;
							}
	}
		

	if ((row < 18) && (board->mech.state[column][row+1].colour == board->mech.state[column][row].colour)) {	
		for (n = 0; n < n_allies; n++)
			if (ally_groups[n]->number == board->mech.state[column][row+1].group->number) 
				break;
		if (n == n_allies) 
			ally_groups[n_allies++] = board->mech.state[column][row+1].group;
			
		if ((column == 18) || (board->mech.state[column +1][row+1].colour == board->mech.state[column][row+1].group->colour + 1)) 
			if ((row+1 == 18) || (board->mech.state[column][row+1 +1].colour == board->mech.state[column][row+1].group->colour + 1)) 
				if ((column == 0) || (board->mech.state[column -1][row+1].colour == board->mech.state[column][row+1].group->colour + 1)) 
					if (board->mech.state[column][row+1 -1].colour == board->mech.state[column][row+1].group->colour + 1) 
						for(tick = board->mech.state[column][row+1].group->members; 
								tick != NULL; tick = tick->next)
							if (tick->coord.x == row+1 && tick->coord.y == column) {
								tick->outfacing = FALSE;		
								break;
							}
	}
	

	if ((column > 0) && (board->mech.state[column-1][row].colour == board->mech.state[column][row].colour)) {
		for (n = 0; n < n_allies; n++)
			if (ally_groups[n]->number == board->mech.state[column-1][row].group->number)
				break;
		if (n == n_allies) 
			ally_groups[n_allies++] = board->mech.state[column-1][row].group;
		
		if (board->mech.state[column-1 +1][row].colour == board->mech.state[column-1][row].group->colour + 1) 
			if ((row == 18) || (board->mech.state[column-1][row+1].colour == board->mech.state[column-1][row].group->colour + 1)) 
				if ((column-1 == 0) || (board->mech.state[column-1 -1][row].colour == board->mech.state[column-1][row].group->colour + 1)) 
					if ((row == 0) || (board->mech.state[column-1][row-1].colour == board->mech.state[column-1][row].group->colour + 1)) 
						for(tick = board->mech.state[column-1][row].group->members; 
								tick != NULL; tick = tick->next)
							if (tick->coord.x == row && tick->coord.y == column-1 ) {
								tick->outfacing = FALSE;		
								break;
							}

	}
	
				
	if ((row > 0) && (board->mech.state[column][row-1].colour == board->mech.state[column][row].colour)) {
		for (n = 0; n < n_allies; n++)
			if (ally_groups[n]->number == board->mech.state[column][row-1].group->number) 
				break;
		if (n == n_allies) 
			ally_groups[n_allies++] = board->mech.state[column][row-1].group;
			
		if ((column == 18) || (board->mech.state[column+1][row-1].colour == board->mech.state[column][row-1].group->colour + 1)) 
			if (board->mech.state[column][row-1 +1].colour == board->mech.state[column][row-1].group->colour + 1) 
				if ((column == 0) || (board->mech.state[column-1][row-1].colour == board->mech.state[column][row-1].group->colour + 1)) 
					if ((row-1 == 0) || (board->mech.state[column][row-1 -1].colour == board->mech.state[column][row-1].group->colour + 1)) 
						for(tick = board->mech.state[column][row-1].group->members; 
								tick != NULL; tick = tick->next)
							if (tick->coord.x == row-1 && tick->coord.y == column) {
								tick->outfacing = FALSE;		
								break;
							}
	}



			// selecting/creating the ally group to add the new stone to

	if (n_allies == 0) {			//new group
		struct group *new_group = malloc(sizeof(struct group));
		new_group->next = board->groups;
		board->groups = new_group;
		
		new_group->number = ++(group_id);
		if (board->mech.state[column][row].colour == 1)
			new_group->colour = b;
		else new_group->colour = w;
		
		new_group->liberties = NULL;
		new_group->members = NULL;
		
		board->mech.state[column][row].group = new_group;	
		group = new_group;
	}
	else {						//adding to the first ally group in the list of allies in contact.
		board->mech.state[column][row].group = ally_groups[0];
		group = ally_groups[0];
	}
		
		
			//adding the stone to the group selected/created.
	
	newM = malloc(sizeof(struct member));
	newM->coord.y = column;
	newM->coord.x = row;
	newM->outfacing = TRUE;
	newM->preserved_move = NULL;
	newM->next = group->members;
	group->members = newM;
	
					//can replace this with cascading of same_colour
	if ((column == 18) || (board->mech.state[column+1][row].colour == group->colour + 1)) 
		if ((row == 18) || (board->mech.state[column][row+1].colour == group->colour + 1)) 
			if ((column == 0) || (board->mech.state[column-1][row].colour == group->colour + 1)) 
				if ((row == 0) || ( board->mech.state[column][row-1].colour == group->colour + 1)) 
					newM->outfacing = FALSE;
		
	
	
	
				//adding liberties to the list, 
						//not if it is present already. 
	struct liberty *walk = NULL;
	
	if (column < 18) 
		if (board->mech.state[column+1][row].colour == empty) {
			for (walk = group->liberties; walk != NULL; walk = walk->next) 
				if (walk->coord.y == column+1 && walk->coord.x == row) 
					break;
			if (walk == NULL) {
				newL = malloc(sizeof(struct liberty));
				newL->coord.y = column+1;
				newL->coord.x = row;
				newL->next = group->liberties;
				group->liberties = newL;
			}
		}
	if (row < 18) 
		if (board->mech.state[column][row+1].colour == empty) {	
			for (walk = group->liberties; walk != NULL; walk = walk->next) 
				if (walk->coord.y == column && walk->coord.x == row+1) 
					break;
			if (walk == NULL) {
				newL = malloc(sizeof(struct liberty));
				newL->coord.y = column;
				newL->coord.x = row+1;
				newL->next = group->liberties;
				group->liberties = newL;
			}
		}
	if (column > 0)
		if (board->mech.state[column-1][row].colour == empty)  {
			for (walk = group->liberties; walk != NULL; walk = walk->next) 
				if (walk->coord.y == column-1 && walk->coord.x == row) 
					break;
			if (walk == NULL) {
				newL = malloc(sizeof(struct liberty));
				newL->coord.y = column-1;
				newL->coord.x = row;
				newL->next = group->liberties;
				group->liberties = newL;
			}
		}
	if (row > 0)	
		if (board->mech.state[column][row-1].colour == empty)  {
			for (walk = group->liberties; walk != NULL; walk = walk->next) 
				if (walk->coord.y == column && walk->coord.x == row-1) 
					break;
			if (walk == NULL) {
				newL = malloc(sizeof(struct liberty));
				newL->coord.y = column;
				newL->coord.x = row-1;
				newL->next = group->liberties;
				group->liberties = newL;
			}
		}
						//deducting the liberty occupied by this move.
						//this has to be done for each ally group in contact.
	struct liberty *prev = NULL;
	for (int n = 0; n < n_allies; n++) {
		prev = NULL; 
		walk = ally_groups[n]->liberties;
		while (walk) {
			if (walk->coord.y == column && walk->coord.x == row) {
				if (prev == NULL)
					ally_groups[n]->liberties = ally_groups[n]->liberties->next;
				else prev->next = walk->next;
				free(walk);
				break;
			}
			prev = walk;
			walk = walk->next;
		}
	}
			
	
		
				//merging ally groups that got merged with this move.
	struct liberty *stroll = NULL;		
	if (n_allies >= 2) {		//If there are multiple ally groups in contact 
		
		for (int n = 1; n < n_allies; n++) {
			
						//changing the group for all moves
			
			for (struct member *walk = ally_groups[n]->members; walk != NULL; walk = walk->next)
				board->mech.state[walk->coord.y][walk->coord.x].group = ally_groups[0];
			
			//~ board->mech.state[ally_groups[n]->members->coord.y][ally_groups[n]->members->coord.x].ptp_group
				//~ = ally_groups[0];
				
						//merging liberties
			
			for (stroll = ally_groups[0]->liberties; stroll != NULL; stroll = stroll->next)
				for (prev = NULL, walk = ally_groups[n]->liberties; walk != NULL; prev = walk, walk = walk->next)
					if (walk->coord.x == stroll->coord.x && walk->coord.y == stroll->coord.y) {
						if (!prev)
							ally_groups[n]->liberties = ally_groups[n]->liberties->next;
						else prev->next = walk->next;
						free(walk);
						break;
					}
				
			for (walk = ally_groups[n]->liberties; walk->next != NULL; walk = walk->next)	
				;
			walk->next = ally_groups[0]->liberties;
			ally_groups[0]->liberties = ally_groups[n]->liberties;
			
						//merging members
			struct member *stroll = ally_groups[n]->members;
			for (; stroll->next != NULL; stroll = stroll->next)
				;
			stroll->next = ally_groups[0]->members;
			ally_groups[0]->members = ally_groups[n]->members;
		
		
						//removing the group from the board's groups list. 
			struct group *prev = NULL, *tick = board->groups;
			while (tick->number != ally_groups[n]->number) {
				prev = tick;
				tick = tick->next;
			}
			if (prev == NULL)
				board->groups = board->groups->next;
			else prev->next = tick->next;
		
			free(tick);
		}
		
		board->mech.state[column][row].merge = TRUE;
	}
			
	
	
	
		
		//making a list of opponent groups in contact

	if (column < 18)
		if (board->mech.state[column+1][row].colour != board->mech.state[column][row].colour 
				&& board->mech.state[column+1][row].colour != empty) 
			opp_groups[n_opps++] = board->mech.state[column+1][row].group;
		
	if (row < 18)
		if (board->mech.state[column][row+1].colour != board->mech.state[column][row].colour
				&& board->mech.state[column][row+1].colour != empty) {	
			for (n = 0; n < n_opps; n++)
				if (opp_groups[n]->number == board->mech.state[column][row+1].group->number) 
					break;
			if (n == n_opps) 
				opp_groups[n_opps++] = board->mech.state[column][row+1].group;
		}
	
	if (column > 0)		
		if (board->mech.state[column-1][row].colour != board->mech.state[column][row].colour
			&& board->mech.state[column-1][row].colour != empty) {
			for (n = 0; n < n_opps; n++)
				if (opp_groups[n]->number == board->mech.state[column-1][row].group->number) 
					break;
			if (n == n_opps) 
				opp_groups[n_opps++] = board->mech.state[column-1][row].group;
		}
	
	if (row > 0)			
		if (board->mech.state[column][row-1].colour != board->mech.state[column][row].colour
			&& board->mech.state[column][row-1].colour != empty) {
			for (n = 0; n < n_opps; n++)
				if (opp_groups[n]->number == board->mech.state[column][row-1].group->number)
					break;
			if (n == n_opps)
				opp_groups[n_opps++] = board->mech.state[column][row-1].group;
		}


			//deducting liberties from the opponent groups in contact, capturing them if no liberties left

	for (int n = 0; n < n_opps; n++) {
		prev = NULL; 
		walk = opp_groups[n]->liberties;
		while (walk) {
			if (walk->coord.y == column && walk->coord.x == row) {
				if (prev == NULL)
					opp_groups[n]->liberties = opp_groups[n]->liberties->next;
				else prev->next = walk->next;
				free(walk);
				break;
			}
			prev = walk;
			walk = walk->next;
		}
		if (opp_groups[n]->liberties == NULL) {
			
			for (struct group *prev = NULL, *walk = board->groups; 
					walk; prev = walk, walk = walk->next)
				if (walk == opp_groups[n]) {
					if (!prev)
						board->groups = board->groups->next;
					else prev->next = walk->next;
				}
			opp_groups[n]->next = board->mech.state[column][row].captured_groups;
			board->mech.state[column][row].captured_groups = opp_groups[n];
			
			capture_group(board, opp_groups[n]);
		}
	}		
}		
			
		
		//revert the move stats like merge and captured_groups.
		
		
void undo_groups (int column, int row, struct board *board, playing_parts *parts) {
	
	struct group *group = board->mech.state[column][row].group;
				
	struct group_op_data data = {board, group, .n_allies = 0, .n_opps = 0, 
								.move_coord.x = row, .move_coord.y = column, 
								.group_id = &group_id};			
				
				
				
				
					
					//removing the move's group pointer
	board->mech.state[column][row].group = NULL;
	
		
					//removing the corresponding member from the group
				
	for (struct member *prev = NULL, *walk = group->members; walk != NULL; 
			prev = walk, walk = walk->next)
		if ((walk->coord.y == column) && (walk->coord.x = row)) {
			if (prev == NULL)
				group->members = group->members->next;
			else prev->next = walk->next;
			free(walk);
			break;
		}
	
	
	
									//removing the group, deallocating memory, if no members left
	if (group->members == NULL) {
		
		for(struct liberty *to_delete = group->liberties, *stroll = group->liberties->next; ;) {
			free(to_delete);
			if (!stroll)
				break;
			to_delete = stroll;
			stroll = stroll->next;
		}
			
		for (struct group *prev = NULL, *walk = board->groups; walk != NULL;
				prev = walk, walk = walk->next)
			if (walk == group) {
				if (prev == NULL)
					board->groups = board->groups->next;
				else prev->next = walk->next;
				free(walk);
				break;
			}
	}
			
	
	else {		//if members are left in the group
		
		if (board->mech.state[column][row].merge) {
			
			check_adjacent_spots (ally_stone, divide_group, column, row, &data);
			
						//deleting the unified group
						
			for(struct liberty *to_delete = group->liberties, *stroll = group->liberties->next; ;) {
				free(to_delete);
				if (!stroll)
					break;
				to_delete = stroll;
				stroll = stroll->next;
			}
			
			for (struct group *prev = NULL, *walk = board->groups; walk != NULL;
					prev = walk, walk = walk->next)
				if (walk == group) {
					if (prev == NULL)
						board->groups = board->groups->next;
					else prev->next = walk->next;
					free(walk);
					break;
				}
			
			board->mech.state[column][row].merge = FALSE;
		}
			
		else {	
			
			check_adjacent_spots (ally_stone, set_outfacing, column, row, &data);
			
			
						//Adding the liberty which was occupied to the group
					
			struct liberty *new_liberty = malloc(sizeof(struct liberty));
			new_liberty->coord.y = column;
			new_liberty->coord.x = row;
			new_liberty->next = group->liberties;
			group->liberties = new_liberty;
						
			check_adjacent_spots (liberties, remove_uncommonLiberties, column, row, &data);	
		}												
	}
	
	check_adjacent_spots (opp_stone, addback_oppLiberties, column, row, &data);
	
	
	
	
			//If the move captured group(s)
	
	for (struct group *stroll = board->mech.state[column][row].captured_groups;
			stroll; stroll = stroll->next) {
		
		for (struct member *walk = stroll->members; walk != NULL; walk = walk->next) {
		
			if (walk->outfacing) {			//adding the liberty to the capturing groups in contact w/ the outfacing stones 
				
				data.n_opps = 0;
				//~ data.move_coord = walk->coord;
				data.member = walk;
				check_adjacent_spots (opp_stone, remove_oppLiberties, walk->coord.y, walk->coord.x, &data);
			}
		
			board->mech.state[walk->coord.y][walk->coord.x] = *(walk->preserved_move);
			
			if (stroll->colour == b) {
				place_stone (walk->coord.y, walk->coord.x, board, parts->blackStone);
				parts->font_color.r = 255; parts->font_color.g = 255; parts->font_color.b = 255; 
			}
			if (stroll->colour == w) {
				place_stone (walk->coord.y, walk->coord.x, board, parts->whiteStone);
				parts->font_color.r = 0; parts->font_color.g = 0; parts->font_color.b = 0; 
			}
			parts->number = board->mech.state[walk->coord.y][walk->coord.x].S_no - (board->first_move->S_no - 1);
			parts->board = board;
			put_number(walk->coord.y, walk->coord.x, parts);
		}
			
			//Adding the liberty occupied by the undone move.
		struct liberty *new_liberty = malloc(sizeof(struct liberty));
		new_liberty->coord.x = row;
		new_liberty->coord.y = column;
		
		new_liberty->next = stroll->liberties;
		stroll->liberties = new_liberty; 
	}
	
	
	
	
	
	if (board->mech.state[column][row].captured_groups) {
		
				//adding the former captured groups to the board's groups list 
				//setting the capturing move's captured_groups to NULL 
		struct group *walk = board->mech.state[column][row].captured_groups;
		for (;walk->next; walk = walk->next)
				;
		walk->next = board->groups;
		board->groups = board->mech.state[column][row].captured_groups;
		board->mech.state[column][row].captured_groups = NULL;
	}
}
		
		
			
			
void capture_group (struct board *board, struct group *captured_group) {
	
	struct group *opp_groups[4];
	int n_opps = 0;
	
	struct liberty *new_liberty;
	int n = 0;
	
	
	
	for (struct member *walk = captured_group->members; walk != NULL; walk = walk->next) {
		

		if (walk->outfacing) {			//adding the liberty to the capturing groups in contact w/ the outfacing stones 
			
			n_opps = 0;
			
			if (walk->coord.y < 18 
				&& board->mech.state[walk->coord.y+1][walk->coord.x].colour != board->mech.state[walk->coord.y][walk->coord.x].colour
				&& board->mech.state[walk->coord.y+1][walk->coord.x].colour != empty) {
					opp_groups[n_opps++] = board->mech.state[walk->coord.y + 1][walk->coord.x].group;
					new_liberty = malloc(sizeof(struct liberty));
					new_liberty->coord = walk->coord;
					new_liberty->next = board->mech.state[walk->coord.y+1][walk->coord.x].group->liberties;
					board->mech.state[walk->coord.y+1][walk->coord.x].group->liberties = new_liberty;
			}
			if (walk->coord.x < 18
				&& board->mech.state[walk->coord.y][walk->coord.x+1].colour != board->mech.state[walk->coord.y][walk->coord.x].colour
				&& board->mech.state[walk->coord.y][walk->coord.x+1].colour != empty) {
					for (n = 0; n < n_opps; n++)
						if (opp_groups[n] == board->mech.state[walk->coord.y][walk->coord.x+1].group)
							break;
					if (n == n_opps) {
						opp_groups[n_opps++] = board->mech.state[walk->coord.y][walk->coord.x+1].group;
						new_liberty = malloc(sizeof(struct liberty));
						new_liberty->coord = walk->coord;
						new_liberty->next = board->mech.state[walk->coord.y][walk->coord.x+1].group->liberties;
						board->mech.state[walk->coord.y][walk->coord.x+1].group->liberties = new_liberty;
					}
			}
			if (walk->coord.y > 0
				&& board->mech.state[walk->coord.y-1][walk->coord.x].colour != board->mech.state[walk->coord.y][walk->coord.x].colour
				&& board->mech.state[walk->coord.y-1][walk->coord.x].colour != empty) {
					for (n = 0; n < n_opps; n++)
						if (opp_groups[n] == board->mech.state[walk->coord.y-1][walk->coord.x].group)
							break;
					if (n == n_opps) {
						opp_groups[n_opps++] = board->mech.state[walk->coord.y-1][walk->coord.x].group;
						new_liberty = malloc(sizeof(struct liberty));
						new_liberty->coord = walk->coord;
						new_liberty->next = board->mech.state[walk->coord.y-1][walk->coord.x].group->liberties;
						board->mech.state[walk->coord.y-1][walk->coord.x].group->liberties = new_liberty;
					}
			}
			if (walk->coord.x > 0
				&& board->mech.state[walk->coord.y][walk->coord.x-1].colour != board->mech.state[walk->coord.y][walk->coord.x].colour
				&& board->mech.state[walk->coord.y][walk->coord.x-1].colour != empty) {
					for (n = 0; n < n_opps; n++)
						if (opp_groups[n] == board->mech.state[walk->coord.y][walk->coord.x-1].group)
							break;
					if (n == n_opps) {
						opp_groups[n_opps++] = board->mech.state[walk->coord.y][walk->coord.x-1].group;
						new_liberty = malloc(sizeof(struct liberty));
						new_liberty->coord = walk->coord;
						new_liberty->next = board->mech.state[walk->coord.y][walk->coord.x-1].group->liberties;
						board->mech.state[walk->coord.y][walk->coord.x-1].group->liberties = new_liberty;
					}
			}	
		}
	}
							
						//copying the move captured to be preserved.
	for (struct member *walk = captured_group->members; walk; walk = walk->next) {
		walk->preserved_move = malloc(sizeof(struct move));
		*(walk->preserved_move) = board->mech.state[walk->coord.y][walk->coord.x];
	}
		
	for (struct group *prev = NULL, *walk = board->groups; walk; prev = walk, walk = walk->next)
		if (walk == captured_group) {
			if (prev == NULL)
				board->groups = board->groups->next;
			else prev->next = walk->next;
		}  //isn't freed because the group is pointed to by captured_groups member of the capturing move.
	
	
	
									//removing the stones captured.
				
	for (struct member *walk = captured_group->members; walk != NULL; walk = walk->next) {
	
		board->mech.state[walk->coord.y][walk->coord.x].colour = empty;
		board->mech.state[walk->coord.y][walk->coord.x].S_no = 0;
		board->mech.state[walk->coord.y][walk->coord.x].group = NULL;
		
		SDL_Rect captureSize = { ((walk->coord.y*SQUARE_SIZE + BORDER) - 15), ((walk->coord.x*SQUARE_SIZE + BORDER) - 15), STONE_SIZE, STONE_SIZE};
	
		SDL_SetTextureBlendMode(board->rep.snap, SDL_BLENDMODE_BLEND);	//colouring a part of the texture transparent. 
		SDL_SetRenderTarget (renderer, board->rep.snap);				
		SDL_SetRenderDrawColor (renderer, 0, 0, 0, 0);
		SDL_RenderFillRect (renderer, &captureSize);
		SDL_SetRenderTarget (renderer, NULL);	
	}
}	
			
			



void check_adjacent_spots (enum stone_codes n, void (*f)(int, int, struct group_op_data *), int column, int row, struct group_op_data *d) {
	
	switch (n) {
		
		
		case no_condition: 	(*f)(column+1, row, d);
							(*f)(column, row+1, d);
							(*f)(column-1, row, d);
							(*f)(column, row-1, d);
							break;
	
	
		case group_match:	if ((column < 18) && (d->board->mech.state[column+1][row].group == d->group)) 
								(*f)(column+1, row, d);
							if ((row < 18) && (d->board->mech.state[column][row+1].group == d->group)) 
								(*f)(column, row+1, d);
							if ((column > 0) && (d->board->mech.state[column-1][row].group == d->group)) 
								(*f)(column-1, row, d);
							if ((row > 0) && (d->board->mech.state[column][row-1].group == d->group)) 
								(*f)(column, row-1, d);	
							break;
								
								
		case ally_stone: 	if ((column < 18) && (d->board->mech.state[column+1][row].colour == d->board->mech.state[column][row].colour)) 
								(*f)(column+1, row, d);
							if ((row < 18) && (d->board->mech.state[column][row+1].colour == d->board->mech.state[column][row].colour)) 
								(*f)(column, row+1, d);
							if ((column > 0) && (d->board->mech.state[column-1][row].colour == d->board->mech.state[column][row].colour)) 
								(*f)(column-1, row, d);
							if ((row > 0) && (d->board->mech.state[column][row-1].colour == d->board->mech.state[column][row].colour)) 
								(*f)(column, row-1, d);	
							break;
		
		
		case liberties:		if ((column < 18) && (d->board->mech.state[column+1][row].colour == empty))
								(*f)(column+1, row, d);
							if ((row < 18) && (d->board->mech.state[column][row+1].colour == empty)) 
								(*f)(column, row+1, d);
							if ((column > 0) && (d->board->mech.state[column-1][row].colour == empty)) 
								(*f)(column-1, row, d);
							if ((row > 0) && (d->board->mech.state[column][row-1].colour == empty)) 
								(*f)(column, row-1, d);	
							break;
							
							
		case opp_stone:		if ((column < 18) && (d->board->mech.state[column+1][row].colour != d->board->mech.state[column][row].colour) 
									&& (d->board->mech.state[column+1][row].colour != empty)) 
								(*f)(column+1, row, d);
							if ((row < 18) && (d->board->mech.state[column][row+1].colour != d->board->mech.state[column][row].colour) 
									&& (d->board->mech.state[column][row+1].colour != empty)) 
								(*f)(column, row+1, d);
							if ((column > 0) && (d->board->mech.state[column-1][row].colour != d->board->mech.state[column][row].colour) 
									&& (d->board->mech.state[column-1][row].colour != empty)) 
								(*f)(column-1, row, d);
							if ((row > 0) && (d->board->mech.state[column][row-1].colour != d->board->mech.state[column][row].colour) 
									&& (d->board->mech.state[column][row-1].colour != empty)) 
								(*f)(column, row-1, d);
							break;
	}
}




void set_outfacing (int column, int row, struct group_op_data *d) {
	
	for (struct member *walk = d->board->mech.state[column][row].group->members; walk != NULL; walk = walk->next) 
		if ((walk->coord.y == column) && (walk->coord.x == row)) {
			walk->outfacing = TRUE;
			break;
		}
}


void remove_uncommonLiberties (int column, int row, struct group_op_data *d) {
	
	if ((column == 18) || (d->board->mech.state[column +1][row].group != d->group))
		if ((row == 18)  || (d->board->mech.state[column][row +1].group != d->group))
			if ((column == 0) || (d->board->mech.state[column -1][row].group != d->group))
				if ((row == 0) || (d->board->mech.state[column][row -1].group != d->group))
				
					for (struct liberty *prev = NULL, *walk = d->group->liberties; walk != NULL;
							prev = walk, walk = walk->next) 
						if ((walk->coord.y == column) && (walk->coord.x == row)) {
							if (prev == NULL)
								d->group->liberties = d->group->liberties->next;
							else prev->next = walk->next;
							free(walk);
							break;
						}
}




void addback_oppLiberties (int column, int row, struct group_op_data *d) {
		
	int n = 0;
	for (; n < d->n_opps; n++)
		if (d->opp_groups[n] == d->board->mech.state[column][row].group) 
			return;

	struct liberty *new_liberty = malloc(sizeof(struct liberty));
	new_liberty->coord.y = d->move_coord.y;
	new_liberty->coord.x = d->move_coord.x;
	new_liberty->next = d->board->mech.state[column][row].group->liberties;
	d->board->mech.state[column][row].group->liberties = new_liberty;
	d->opp_groups[d->n_opps++] = d->board->mech.state[column][row].group;
	
}


		
void divide_group (int column, int row, struct group_op_data *d) {
	
	int n = 0;
	for (; n < d->n_allies; n++) 
		if (d->board->mech.state[column][row].group == d->ally_groups[n])
			return;	
		//~ for (struct member *walk = d->ally_groups[n]->members; walk != NULL; walk = walk->next)
			//~ if ((walk->coord.y == column) && (walk->coord.x == row))
				//~ return;
		
	
	
	struct group *new_group = malloc(sizeof(struct group));
	new_group->number = ++(*(d->group_id));
	new_group->colour = d->board->mech.state[column][row].colour - 1;
	new_group->liberties = NULL;
	new_group->members = NULL;
	
	new_group->next = d->board->groups;
	d->board->groups = new_group;
	
	d->ally_groups[d->n_allies++] = new_group;
	
	
				//adding the first stone to the new_group
	
	d->board->mech.state[column][row].group = new_group;
	
	struct member *new_member = malloc(sizeof(struct member));
	new_member->coord.x = row;
	new_member->coord.y = column;
	new_member->outfacing = FALSE;
	new_member->next = new_group->members;
	new_group->members = new_member;
	
	
	
							//I am only declaring a new struct and initializing it to pass the new_member
	struct group_op_data new_data = {d->board, new_group, new_member,
									.move_coord.x = d->move_coord.x, .move_coord.y = d->move_coord.y};
	
	check_adjacent_spots (no_condition, evaluate_group, column, row, &new_data);
	
}


void add_toGroup (int column, int row, struct group_op_data *d) {
	
	if ((column == d->move_coord.y) && (row == d->move_coord.x)) 	//why is this needed? won't the next condition check for this as well?
		return;
	
	for (struct member *walk = d->board->groups->members; walk; walk = walk->next)
		if ((walk->coord.x == row) && (walk->coord.y == column))
			return;
		
	d->board->mech.state[column][row].group = d->board->groups;  //this should be fine
	
	struct member *new_member = malloc(sizeof(struct member));
	new_member->coord.x = row;
	new_member->coord.y = column;
	new_member->next = d->board->groups->members;
	d->board->groups->members = new_member;
	
	check_adjacent_spots (ally_stone, add_toGroup, column, row, d);
}
	
	
void evaluate_group (int column, int row, struct group_op_data *d) {
	
	if ((column == d->move_coord.y) && (row == d->move_coord.x)) {	//why is this needed? won't the next condition check for this as well? Nope, this is for the move that is undone. If it is evaluated, the evaluation will pass over to other groups attached, and this won't divide any groups.
		for (struct liberty *walk = d->group->liberties; walk; walk = walk->next)
				if ((walk->coord.x == row) && (walk->coord.y == column))
					return;
		struct liberty *new_liberty = malloc(sizeof(struct liberty));
		new_liberty->coord.y = column;
		new_liberty->coord.x = row;
		new_liberty->next = d->group->liberties;
		d->group->liberties = new_liberty;
		
		d->member->outfacing = TRUE;
		
		return;
	}
	
	if (d->board->mech.state[column][row].group == d->group)
		return;
	
	if (d->board->mech.state[column][row].colour != d->group->colour+1) {
		
		d->member->outfacing = TRUE;
		
		if (d->board->mech.state[column][row].colour == empty) {
			
			for (struct liberty *walk = d->group->liberties; walk; walk = walk->next)
				if ((walk->coord.x == row) && (walk->coord.y == column))
					return;
			struct liberty *new_liberty = malloc(sizeof(struct liberty));
			new_liberty->coord.y = column;
			new_liberty->coord.x = row;
			new_liberty->next = d->group->liberties;
			d->group->liberties = new_liberty;
		}
		return;
	}
	
	
	
	d->board->mech.state[column][row].group = d->group;  
	
	struct member *new_member = malloc(sizeof(struct member));
	new_member->coord.x = row;
	new_member->coord.y = column;
	new_member->outfacing = FALSE;
	new_member->next = d->board->groups->members;
	d->board->groups->members = new_member;
	
	struct group_op_data new_data = {d->board, d->group, new_member,
									.move_coord.x = d->move_coord.x, .move_coord.y = d->move_coord.y};	
	
	
	check_adjacent_spots (no_condition, evaluate_group, column, row, &new_data);
	
}	
		


void remove_oppLiberties (int column, int row, struct group_op_data *d) {
	
	if ((row == d->move_coord.x) && (column == d->move_coord.y))
		return;
	
	int n = 0;
	for (; n < d->n_opps; n++)
		if (d->opp_groups[n] == d->board->mech.state[column][row].group)
			break;
			
	if (n == d->n_opps) {
		d->opp_groups[d->n_opps++] = d->board->mech.state[column][row].group;
		
		for (struct liberty *prev = NULL, *stroll = d->opp_groups[d->n_opps-1]->liberties; stroll;
				prev = stroll, stroll = stroll->next) 
			if ((stroll->coord.y == d->member->coord.y) && (stroll->coord.x == d->member->coord.x))  {
				if (!prev)
					d->opp_groups[d->n_opps-1]->liberties = stroll->next;
				else
					prev->next = stroll->next;
				free(stroll);
				break;
			}
	}
}
			
		
		
		
		
		
	


										
											
	



