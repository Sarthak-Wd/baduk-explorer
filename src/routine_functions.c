#include "routine_functions.h"

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
		
		
	undo_groups (i, j, p);	
					
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
	newM->next = group->members;
	group->members = newM;
	
					//can replace this with cascading if same_colour
	if ((column == 18) || (board->mech.state[column+1][row].colour == group->colour + 1)) 
		if ((row == 18) || (board->mech.state[column][row+1].colour == group->colour + 1)) 
			if ((column == 0) || (board->mech.state[column-1][row].colour == group->colour + 1)) 
				if ((row == 0) || ( board->mech.state[column][row-1].colour == group->colour + 1)) 
					newM->outfacing = FALSE;
		
	
	
	
				//adding liberties to the list, 
						//not if it is present already. 
	struct liberty *walk;
	
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
	struct liberty *prev;
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
	struct liberty *stroll;		
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
		if (opp_groups[n]->liberties == NULL)
			capture_group(board, opp_groups[n]);
	}


					
								//if in contact w/ an opp group
		
	//~ if (board->mech.state[column+1][row].colour != empty &&		
		//~ board->mech.state[column+1][row].colour != board->mech.state[column][row].colour)
		//deduct liberty, if zero, capture stones.
		//if the liberty is shared b/w multiple outfacing stones, it will be deducted once, and skipped
		//from then on.
		//if the group is removed just after deducting once, b4 all the conditions are checked, there
		//might be problems? Isn't it better to make a list and then execute. This is easy because:
		//the played stone only occupies one liberty; I would just have to remove just one liberty from 
		//each opponent group in contact.
		
	//Then, recheck the outfacing status of the adjacent ally stones.
		
			
}		
			
		
		//stats like colour and S_no are reverted by undo_move already. should I put this befor undo_move?
void undo_groups (int column, int row, struct board *board) {
	
	struct group *group = board->mech.state[column][row].group;
					
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
		
							
							//setting the outfacing status of all ally stones in contact to TRUE
							
		//~ if ((column < 18) && (board->mech.state[column+1][row].group == group)) 
			//~ for (struct member *walk = group->members; walk != NULL; walk = walk->next) 
				//~ if ((walk->coord.y == column+1) && (walk->coord.x == row)) {
					//~ walk->outfacing = TRUE;
					//~ break;
				//~ }
		//~ if ((row < 18) && (board->mech.state[column][row+1].group == group)) 
			//~ for (struct member *walk = group->members; walk != NULL; walk = walk->next) 
				//~ if ((walk->coord.y == column) && (walk->coord.x == row+1)) {
					//~ walk->outfacing = TRUE;
					//~ break;
				//~ }
		//~ if ((column > 0) && (board->mech.state[column-1][row].group == group)) 
			//~ for (struct member *walk = group->members; walk != NULL; walk = walk->next) 
				//~ if ((walk->coord.y == column-1) && (walk->coord.x == row)) {
					//~ walk->outfacing = TRUE;
					//~ break;
				//~ }
		//~ if ((row > 0) && (board->mech.state[column][row-1].group == group)) 
			//~ for (struct member *walk = group->members; walk != NULL; walk = walk->next) 
				//~ if ((walk->coord.y == column) && (walk->coord.x == row-1)) {
					//~ walk->outfacing = TRUE;
					//~ break;
				//~ }	
				
		check_adjacent_stones (ally_stone, set_outfacing, column, row, board);
		
			
		
		
					//Adding the liberty which was occupied to the group
				
		struct liberty *new_liberty = malloc(sizeof(struct liberty));
		new_liberty->coord.y = column;
		new_liberty->coord.x = row;
		new_liberty->next = group->liberties;
		group->liberties = new_liberty;
		
		
			
					//removing liberties (not the shared ones). This won't be needed if it is a merging move.
					//Since that requires a complete overhaul.
					
		if ((column < 18) && (board->mech.state[column+1][row].colour == empty)) 
			
			if ((column+1 == 18) || (board->mech.state[column+1 +1][row].group != group))
				if ((row == 18)  || (board->mech.state[column+1][row +1].group != group))
					if ((column+1 == 0) || (board->mech.state[column+1 -1][row].group != group))
						if ((row == 0) || (board->mech.state[column+1][row -1].group != group))
							for (struct liberty *prev = NULL, *walk = group->liberties; walk != NULL;
									prev = walk, walk = walk->next) 
								if ((walk->coord.y == column+1) && (walk->coord.x == row)) {
									if (prev == NULL)
										group->liberties = group->liberties->next;
									else prev->next = walk->next;
									free(walk);
									break;
								}
								
		if ((row < 18) && (board->mech.state[column][row+1].colour == empty)) 
			
			if ((column == 18) || (board->mech.state[column +1][row+1].group != group))
				if ((row+1 == 18)  || (board->mech.state[column][row+1 +1].group != group))
					if ((column == 0) || (board->mech.state[column -1][row+1].group != group))
						if ((row+1 == 0) || (board->mech.state[column][row+1 -1].group != group))
							for (struct liberty *prev = NULL, *walk = group->liberties; walk != NULL;
									prev = walk, walk = walk->next) 
								if ((walk->coord.y == column) && (walk->coord.x == row+1)) {
									if (prev == NULL)
										group->liberties = group->liberties->next;
									else prev->next = walk->next;
									free(walk);
									break;
								}
			
		if ((column > 0) && (board->mech.state[column-1][row].colour == empty)) 
			
			if ((column-1 == 18) || (board->mech.state[column-1 +1][row].group != group))
				if ((row == 18)  || (board->mech.state[column-1][row +1].group != group))
					if ((column-1 == 0) || (board->mech.state[column-1 -1][row].group != group))
						if ((row == 0) || (board->mech.state[column-1][row -1].group != group))
							for (struct liberty *prev = NULL, *walk = group->liberties; walk != NULL;
									prev = walk, walk = walk->next) 
								if ((walk->coord.y == column-1) && (walk->coord.x == row)) {
									if (prev == NULL)
										group->liberties = group->liberties->next;
									else prev->next = walk->next;
									free(walk);
									break;
								}
			
		if ((row > 0) && (board->mech.state[column][row-1].colour == empty)) 
			
			if ((column == 18) || (board->mech.state[column +1][row-1].group != group))
				if ((row-1 == 18)  || (board->mech.state[column][row-1 +1].group != group))
					if ((column == 0) || (board->mech.state[column -1][row-1].group != group))
						if ((row-1 == 0) || (board->mech.state[column][row-1 -1].group != group))
							for (struct liberty *prev = NULL, *walk = group->liberties; walk != NULL;
									prev = walk, walk = walk->next) 
								if ((walk->coord.y == column) && (walk->coord.x == row-1)) {
									if (prev == NULL)
										group->liberties = group->liberties->next;
									else prev->next = walk->next;
									free(walk);
									break;
								}
	}
	

	
}
		
		
			
			
void capture_group (struct board *board, struct group *group) {
	
	struct group *opp_groups[4];
	int n_opps = 0;
	
	struct liberty *new_liberty;
	int n = 0;
	
	
	
	for (struct member *walk = group->members; walk != NULL; walk = walk->next) {
		

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
	
							//removing the stones captured.
				
	for (struct member *walk = group->members; walk != NULL; walk = walk->next) {
	
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
			
			



void check_adjacent_stones (enum stone_codes n, void (*f)(int, int, struct board *), int column, int row, struct board *board) {
	
	switch (n) {
		
		
		case group_match:	if ((column < 18) && (board->mech.state[column+1][row].group == board->mech.state[column][row].group)) 
								(*f)(column+1, row, board);
							if ((row < 18) && (board->mech.state[column][row+1].group == board->mech.state[column][row].group)) 
								(*f)(column, row+1, board);
							if ((column > 0) && (board->mech.state[column-1][row].group == board->mech.state[column][row].group)) 
								(*f)(column-1, row, board);
							if ((row > 0) && (board->mech.state[column][row-1].group == board->mech.state[column][row].group)) 
								(*f)(column, row-1, board);	
								
								
		case ally_stone: 	if ((column < 18) && (board->mech.state[column+1][row].colour == board->mech.state[column][row].colour)) 
								(*f)(column+1, row, board);
							if ((row < 18) && (board->mech.state[column][row+1].colour == board->mech.state[column][row].colour)) 
								(*f)(column, row+1, board);
							if ((column > 0) && (board->mech.state[column-1][row].colour == board->mech.state[column][row].colour)) 
								(*f)(column-1, row, board);
							if ((row > 0) && (board->mech.state[column][row-1].colour == board->mech.state[column][row].colour)) 
								(*f)(column, row-1, board);	
		
		
		
	}
}




void set_outfacing (int column, int row, struct board *board) {
	for (struct member *walk = board->mech.state[column][row].group->members; walk != NULL; walk = walk->next) 
		if ((walk->coord.y == column) && (walk->coord.x == row)) {
			walk->outfacing = TRUE;
			break;
		}
}





			
			
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






	
struct board *split_board (int *n_boards, int moveNum, playing_parts *parts, struct message *text, struct board **infocus, struct board **list, struct list_lines **list_lines, scaling scale) {



	struct board *new_board_1 = declare_new_board(n_boards, *list, *infocus, scale);	
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


