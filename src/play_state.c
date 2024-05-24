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

	SDL_Rect undoSize = { ((i*SQUARE_SIZE + X_BORDER) - 15), ((j*SQUARE_SIZE + Y_BORDER) - 15), STONE_SIZE, STONE_SIZE};
	
	SDL_SetTextureBlendMode(p->rep.snap, SDL_BLENDMODE_BLEND);	//colouring a part of the texture transparent. 
	SDL_SetRenderTarget (renderer, p->rep.snap);				
	SDL_SetRenderDrawColor (renderer, 0, 0, 0, 0);
	SDL_RenderFillRect (renderer, &undoSize);
	SDL_SetRenderTarget (renderer, NULL);
	
	
}
	










void group_stuff (int column, int row, struct board *board) {
	
	struct group *group;
	
	struct member *newM;
	
	struct group_op_data data = {board, .n_allies = 0, .n_opps = 0, 
								.move_coord.x = row, .move_coord.y = column, 
								.group_id = &group_id};
	
	//~ struct group *ally_groups[4];
	//~ struct group *opp_groups[4];
	//~ int n_allies = 0, n_opps = 0;
	//~ bool group_matched;
	
	//~ struct group_op_data data;
	
	
	
	
	
			
	
	check_adjacent_spots (ally_stone, check_and_set_outfacing, column, row, &data);
	check_adjacent_spots (ally_stone, get_adj_ally_groups, column, row, &data);
	


			// selecting/creating the ally group to add the new stone to

	if (data.n_allies == 0) {			//new group
		struct group *new_group = malloc(sizeof(struct group));
		new_group->next = board->groups;
		board->groups = new_group;
		
		new_group->number = ++(group_id);
		if (board->mech.state[column][row].colour == 1)
			new_group->colour = b;
		else new_group->colour = w;
		
		new_group->liberties = NULL;
		new_group->members = NULL;
		
		new_group->capturing_move_S_no = 0;
		
		board->mech.state[column][row].group = new_group;	
		group = new_group;
		data.group = new_group;
	}
	else {						//adding to the first ally group in the list of allies in contact.
		board->mech.state[column][row].group = data.ally_groups[0];
		group = data.ally_groups[0];
		data.group = data.ally_groups[0];
	}
		
		
			//adding the stone to the group selected/created.
	
	newM = malloc(sizeof(struct member));
	
	newM->coord.y = column;
	newM->coord.x = row;
	newM->outfacing = TRUE;
	newM->merge = FALSE;
	newM->S_no_on_board = board->mech.state[column][row].S_no;
	
	newM->next = data.group->members;
	data.group->members = newM;
	
				
	if ((column == 18) || (board->mech.state[column+1][row].colour == group->colour + 1)) 
		if ((row == 18) || (board->mech.state[column][row+1].colour == group->colour + 1)) 
			if ((column == 0) || (board->mech.state[column-1][row].colour == group->colour + 1)) 
				if ((row == 0) || ( board->mech.state[column][row-1].colour == group->colour + 1)) 
					newM->outfacing = FALSE;
		
	
	
	
				//adding liberties to the list, 
						//not if it is present already. 
						
	check_adjacent_spots (liberties, add_uncommonLiberties, column, row, &data);					
						
	
	
						//deducting the liberty occupied by this move.
						//this has to be done for each ally group in contact.
	
	
	for (int n = 0; n < data.n_allies; n++) {
		
		struct liberty **prev = &(data.ally_groups[n]->liberties),
						*walk = data.ally_groups[n]->liberties;
		while (walk) {
			if (walk->coord.y == column && walk->coord.x == row) {
				*prev = walk->next;
				free(walk);
				break;
			}
			prev = &(walk->next);
			walk = walk->next;
		}
	}
			
	
	
		
	//====================  merging ally groups that got merged with this move ==================
	
	
	struct liberty *stroll = NULL;		
	if (data.n_allies >= 2) {		//If there are multiple ally groups in contact 
		
		for (int n = 1; n < data.n_allies; n++) {
			
						//changing the group for all moves
			
			for (struct member *walk = data.ally_groups[n]->members; walk; walk = walk->next)
				board->mech.state[walk->coord.y][walk->coord.x].group = data.ally_groups[0];
		
				
			//merging liberties
			
				// removing liberties which are present in the list ally_groups[0]->liberties 
				// from ally_groups[n]->liberties 
				
			for (stroll = data.ally_groups[0]->liberties; stroll; stroll = stroll->next) {
				
				struct liberty **walk = &(data.ally_groups[n]->liberties), *temp;
				for (; *walk; walk = &(*walk)->next)
					if ((*walk)->coord.y == stroll->coord.y && (*walk)->coord.x == stroll->coord.x) {
						temp = *walk;
						*walk = (*walk)->next;
						free(temp);
						break;
					}
			}
			
			if (data.ally_groups[n]->liberties) {
				struct liberty *walk = data.ally_groups[n]->liberties;
				for (; walk->next; walk = walk->next)	
					;
				walk->next = data.ally_groups[0]->liberties;
				data.ally_groups[0]->liberties = data.ally_groups[n]->liberties;
			}
			
						//merging members
			struct member *stroll = data.ally_groups[n]->members;
			for (; stroll->next != NULL; stroll = stroll->next)
				;
			stroll->next = data.ally_groups[0]->members;
			data.ally_groups[0]->members = data.ally_groups[n]->members;
		
		
						//removing the group from the board's groups list. 
			struct group *prev = NULL, *tick = board->groups;
			while (tick->number != data.ally_groups[n]->number) {
				prev = tick;
				tick = tick->next;
			}
			if (prev == NULL)
				board->groups = board->groups->next;
			else prev->next = tick->next;
		
			free(tick);
		}
		
		newM->merge = TRUE;
	}
		
	//===============================================================================================		
	
	
	
	
		//deducting liberties from the opponent groups in contact, capturing them if no liberties left

	check_adjacent_spots (opp_stone, get_adj_opp_groups, column, row, &data);

	for (int n = 0; n < data.n_opps; n++) {
		
		struct liberty **prev = &(data.opp_groups[n]->liberties); 
		struct liberty *walk = data.opp_groups[n]->liberties;
		while (walk) {
			if (walk->coord.y == column && walk->coord.x == row) {
				*prev = walk->next;
				free(walk);
				break;
			}
			prev = &(walk->next);
			walk = walk->next;
		}
		
		if (data.opp_groups[n]->liberties == NULL) {
			
			data.opp_groups[n]->capturing_move_S_no = board->mech.state[column][row].S_no;
			capture_group(board, data.opp_groups[n]);
		}
	}		
}		
			
	


			
void capture_group (struct board *board, struct group *captured_group) {
	
	struct group_op_data data = {board, captured_group, .n_allies = 0, .n_opps = 0,  
								.group_id = &group_id};	
	
	
	
	//moving the captured group from the board's groups list into the board's captured_groups list
	
	for (struct group **ptr = &(board->groups); *ptr; ptr = &(*ptr)->next)
		if (*ptr == captured_group) {
			*ptr = (*ptr)->next;
			break;
		}
						
	captured_group->next = board->captured_groups;
	board->captured_groups = captured_group;
	
	
	
	
	for (struct member *walk = captured_group->members; walk; walk = walk->next) 
		
		if (walk->outfacing) {			//adding the liberty to the capturing groups in contact w/ the outfacing stones 
			
			data.move_coord = walk->coord;
			data.n_opps = 0;
			
			check_adjacent_spots (opp_stone, addback_oppLiberties, data.move_coord.y, data.move_coord.x, &data);		
		}
	
		
	//~ for (struct group **prev = &(board->groups), *walk = board->groups; 
			//~ walk; prev = &(walk->next), walk = walk->next)
		
		//~ if (walk == captured_group) 
			//~ *prev = walk->next;
		  //~ //isn't freed because the group is added to board->captured_groups 
	
	
	
									//removing the stones captured.
				
	for (struct member *walk = captured_group->members; walk; walk = walk->next) {
	
		board->mech.state[walk->coord.y][walk->coord.x].colour = empty;
		board->mech.state[walk->coord.y][walk->coord.x].S_no = 0;
		board->mech.state[walk->coord.y][walk->coord.x].group = NULL;
		
		SDL_Rect captureSize = { ((walk->coord.y*SQUARE_SIZE + X_BORDER) - 15), ((walk->coord.x*SQUARE_SIZE + Y_BORDER) - 15), STONE_SIZE, STONE_SIZE};
	
		SDL_SetTextureBlendMode(board->rep.snap, SDL_BLENDMODE_BLEND);	//colouring a part of the texture transparent. 
		SDL_SetRenderTarget (renderer, board->rep.snap);				
		SDL_SetRenderDrawColor (renderer, 0, 0, 0, 0);
		SDL_RenderFillRect (renderer, &captureSize);
		SDL_SetRenderTarget (renderer, NULL);	
	}
}	
			
		
	
	
	
		
		//undo moves, unmerge, uncapture groups
		
void undo_groups (int column, int row, struct board *board, playing_parts *parts) {
	
	
	struct group *group = board->mech.state[column][row].group;
				
	struct group_op_data data = {board, group, .n_allies = 0, .n_opps = 0, 
								.move_coord.x = row, .move_coord.y = column, 
								.group_id = &group_id};			
				
	bool merge; 	//since the member is deleted b4 the operations			
				
					
					
	board->mech.state[column][row].group = NULL;								//removing the move's group pointer
	
						
	for (struct member **prev = &(group->members), *walk = group->members; 		//removing the corresponding member from the group
			walk;  prev = &(walk->next), walk = walk->next)
			
		if ((walk->coord.y == column) && (walk->coord.x == row)) {
			
			merge = walk->merge;
			*prev = walk->next;
			free(walk);
			break;
		}
	
	
	
									//removing the group, deallocating memory, if no members left
	if (group->members == NULL) {
		
								//deallocating the liberties list
		if (group->liberties) {
			struct liberty *to_delete = group->liberties, 
							*stroll = group->liberties->next;
			while (1) {
				free(to_delete);
				if (!stroll)
					break;
				to_delete = stroll;
				stroll = stroll->next;
			}
		}
									//removing from the board's groups list
								
		for (struct group **ptr = &board->groups, *temp; 
				*ptr;   ptr = &(*ptr)->next)
				
			if (*ptr == group) {
				temp = *ptr;
				*ptr = (*ptr)->next;
				free(temp);
				break;
			}
	}
			
			
	
	else {		//if members are left in the group
		
		if (merge) {
			
			check_adjacent_spots (ally_stone, divide_group, column, row, &data);
			
			
			
		//________ passing on the merge and S_no info, since the members are new. _____________
				
			struct member_info pass_on[19][19];
			
			for (struct member *walk = group->members; walk; walk = walk->next) {
				pass_on[walk->coord.y][walk->coord.x].merge 		= walk->merge;
				pass_on[walk->coord.y][walk->coord.x].S_no_on_board = walk->S_no_on_board;
			}
		
			for (int n = 0; n < data.n_allies; n++) 
				for (struct member *walk = data.ally_groups[n]->members; 
						walk; walk = walk->next) {
					walk->merge = pass_on[walk->coord.y][walk->coord.x].merge;
					walk->S_no_on_board = pass_on[walk->coord.y][walk->coord.x].S_no_on_board;
				}
		
			
			
			
		// ________ deleting the unified group ____________________________________________
			struct liberty 	*to_delete = group->liberties, 
							*stroll = group->liberties->next;			
			while(1) {
				free(to_delete);
				if (!stroll)
					break;
				to_delete = stroll;
				stroll = stroll->next;
			}
			
			for (struct group **prev = &(board->groups), *walk = board->groups; walk;
					prev = &(walk->next), walk = walk->next)
				if (walk == group) {
					*prev = walk->next;
					free(walk);
					break;
				}
		}
			
			
		else {		// if not merge
			
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
	
	
	
	
	
	
	//================ If the move captured group(s) =========================
	
																		
	for (struct group *stroll = board->captured_groups; stroll; stroll = board->captured_groups) {				//stroll = board->captured_groups   This, because stroll points to the first in the active boards list by the end of the loop.
		
		if (stroll->capturing_move_S_no != board->mech.state[column][row].S_no)
			break;
		
		for (struct member *walk = stroll->members; walk; walk = walk->next) {
		
			if (walk->outfacing) {			//removing the liberties from the capturing groups in contact w/ the outfacing stones 
				
				data.n_opps = 0;
				
				data.member = walk;
				check_adjacent_spots (opp_stone, remove_oppLiberties, walk->coord.y, walk->coord.x, &data);
			}
		
			board->mech.state[walk->coord.y][walk->coord.x].colour = stroll->colour + 1;
			board->mech.state[walk->coord.y][walk->coord.x].S_no = walk->S_no_on_board;
			board->mech.state[walk->coord.y][walk->coord.x].group = stroll;
			
			
				//printing the move 
				
			if (stroll->colour == b) {
				place_stone (walk->coord.y, walk->coord.x, board, parts->blackStone);
				parts->font_color.r = 255; parts->font_color.g = 255; parts->font_color.b = 255; 
			}
			if (stroll->colour == w) {
				place_stone (walk->coord.y, walk->coord.x, board, parts->whiteStone);
				parts->font_color.r = 0; parts->font_color.g = 0; parts->font_color.b = 0; 
			}
			if (walk->S_no_on_board >= board->first_move->S_no) {		//If the stone was played on the current board
				parts->number = walk->S_no_on_board - (board->first_move->S_no - 1);							//board->mech.state[walk->coord.y][walk->coord.x].S_no - (board->first_move->S_no - 1);
				parts->board = board;
				put_number(walk->coord.y, walk->coord.x, parts);
			}
		}
			
			
			//Adding the liberty occupied by the undone move, to the group uncaptured.
			
		struct liberty *new_liberty = malloc(sizeof(struct liberty));
		new_liberty->coord.x = row;
		new_liberty->coord.y = column;
		
		new_liberty->next = stroll->liberties;
		stroll->liberties = new_liberty; 
		
		
			//getting the uncaptured group out of the captured list and putting it into the main groups list
		
		board->captured_groups = board->captured_groups->next;
		stroll->next = board->groups;
		board->groups = stroll;
	}
	
}
		
		
			
		














//====================================================================================================**/

					/**********____HELPER_GROUP_FUNCTIONS____******************/




void check_adjacent_spots (enum stone_codes n, void (*f)(int, int, struct group_op_data *), int column, int row, struct group_op_data *d) {
	
	switch (n) {
		
		
		case no_condition: 	if (column < 18)
								(*f)(column+1, row, d);
							if (row < 18)
								(*f)(column, row+1, d);
							if (column > 0)
								(*f)(column-1, row, d);
							if (row > 0)
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


void add_uncommonLiberties (int column, int row, struct group_op_data *d) {
	
	struct liberty *walk = d->group->liberties;
	struct liberty *newL;
	
	for (; walk; walk = walk->next) 
		if (walk->coord.y == column && walk->coord.x == row) 
			break;
	if (walk == NULL) {
		newL = malloc(sizeof(struct liberty));
		newL->coord.y = column;
		newL->coord.x = row;
		newL->next = d->group->liberties;
		d->group->liberties = newL;
	}
}
			

void remove_uncommonLiberties (int column, int row, struct group_op_data *d) {
	
	if ((column == 18) || (d->board->mech.state[column +1][row].group != d->group))
		if ((row == 18)  || (d->board->mech.state[column][row +1].group != d->group))
			if ((column == 0) || (d->board->mech.state[column -1][row].group != d->group))
				if ((row == 0) || (d->board->mech.state[column][row -1].group != d->group))
				
					for (struct liberty *prev = NULL, *walk = d->group->liberties; walk;
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

	d->opp_groups[d->n_opps++] = d->board->mech.state[column][row].group;

	struct liberty *new_liberty = malloc(sizeof(struct liberty));
	new_liberty->coord.y = d->move_coord.y;
	new_liberty->coord.x = d->move_coord.x;
	new_liberty->next = d->opp_groups[d->n_opps-1]->liberties;
	d->opp_groups[d->n_opps-1]->liberties = new_liberty;

}


		
void divide_group (int column, int row, struct group_op_data *d) {
	
	int n = 0;
	for (; n < d->n_allies; n++) 
		if (d->board->mech.state[column][row].group == d->ally_groups[n])
			return;	
		
		
	
	
	struct group *new_group = malloc(sizeof(struct group));
	new_group->number = ++(*(d->group_id));
	new_group->colour = d->board->mech.state[column][row].colour - 1;
	new_group->liberties = NULL;
	new_group->members = NULL;
	
	new_group->next = d->board->groups;
	d->board->groups = new_group;
	
	d->ally_groups[d->n_allies++] = new_group;
	
	
				//adding the ally_stone in contact to its new group
	
	d->board->mech.state[column][row].group = new_group;
	
	
	struct member *new_member = malloc(sizeof(struct member));
	
	new_member->coord.x = row;
	new_member->coord.y = column;
	new_member->outfacing = TRUE;			//since it was in contact with the undone move 
	
	new_member->next = new_group->members;
	new_group->members = new_member;
	
	
			//adding the liberty occupied by the move being undone to the group in contact with it.
	struct liberty *new_liberty = malloc(sizeof(struct liberty));
	new_liberty->coord.y = d->move_coord.y;
	new_liberty->coord.x = d->move_coord.x;
	new_liberty->next = new_group->liberties;
	new_group->liberties = new_liberty;
	


							//I am only declaring a new struct and initializing it to pass the new_member
	struct group_op_data new_data = {d->board, new_group, new_member,
									.move_coord.x = d->move_coord.x, .move_coord.y = d->move_coord.y};
	
	check_adjacent_spots (no_condition, evaluate_group, column, row, &new_data);
	
}

	
void evaluate_group (int column, int row, struct group_op_data *d) {
	
	if ((column == d->move_coord.y) && (row == d->move_coord.x)) 	//why is this needed? won't the next condition check for this as well? Nope, this is for the move that is undone. If it is evaluated, the evaluation will pass over to other groups attached, and this won't divide any groups.	
		return;
	
	
	if (d->board->mech.state[column][row].group == d->group)		//If added to the group already
		return;
		
				//If the current spot is not occupied by an ally stone
	
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
	
			//If the current spot is occupied by an ally stone
	
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
	
	
	if ((row == d->move_coord.x) && (column == d->move_coord.y))		//undone move
		return;
	
	
	for (int n = 0; n < d->n_opps; n++)									//so that I don't remove this liberty from the same group twice
		if (d->opp_groups[n] == d->board->mech.state[column][row].group)
			return;
			
	
	
	d->opp_groups[d->n_opps++] = d->board->mech.state[column][row].group;
	
	
	struct liberty **prev = &(d->opp_groups[d->n_opps-1]->liberties), 
					*stroll = d->opp_groups[d->n_opps-1]->liberties;
					
	for (; stroll; prev = &(stroll->next), stroll = stroll->next) 
		if ((stroll->coord.y == d->member->coord.y) 
				&& (stroll->coord.x == d->member->coord.x))  {
			*prev = stroll->next;
			free(stroll);
			break;
		}
	
}
			
		

void get_adj_ally_groups (int column, int row, struct group_op_data *d) {

	int n;
	for (n = 0; n < d->n_allies; n++)
		if (d->ally_groups[n]->number == d->board->mech.state[column][row].group->number) 
			break;
	if (n == d->n_allies) 
		d->ally_groups[d->n_allies++] = d->board->mech.state[column][row].group;
}


void get_adj_opp_groups (int column, int row, struct group_op_data *d) {
	
	int n;
	for (n = 0; n < d->n_opps; n++)
		if (d->opp_groups[n]->number == d->board->mech.state[column][row].group->number) 
			break;
	if (n == d->n_opps) 
		d->opp_groups[d->n_opps++] = d->board->mech.state[column][row].group;
}


void check_and_set_outfacing (int column, int row, struct group_op_data *d) {
			
	if ((column == 18) || (d->board->mech.state[column +1][row].colour == d->board->mech.state[column][row].group->colour + 1)) 
		if ((row == 18) || (d->board->mech.state[column][row +1].colour == d->board->mech.state[column][row].group->colour + 1)) 
			if ((column == 0) || (d->board->mech.state[column -1][row].colour == d->board->mech.state[column][row].group->colour + 1)) 
				if ((row == 0) || (d->board->mech.state[column][row -1].colour == d->board->mech.state[column][row].group->colour + 1)) 
					for(struct member *tick = d->board->mech.state[column][row].group->members; 
							tick; tick = tick->next)
						if (tick->coord.x == row && tick->coord.y == column) {
							tick->outfacing = FALSE;		
							break;
						}
}



// The function isn't used anywhere? wierd
/*
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
	*/


//================================================================================











		
		
		



void setup_mode (menu_textures *turn_menu, struct board *list, struct list_lines *list_lines, scaling *scale) {
	
	SDL_MouseButtonEvent pan_start;
	
	enum {alt, bl, wh} turn_type;
	enum turn turn;
	
	SDL_Rect placement = {WINDOW_WIDTH-200, 0, 200, WINDOW_HEIGHT};
	
	
		
		
	SDL_Event event;
	
	while (1) {
		
		SDL_RenderCopy(renderer, turn_menu->setup_moves_menu, NULL, &placement);
		SDL_RenderPresent(renderer);
		
		
		
		while (!SDL_PollEvent(&event))
			;
		
		switch (event.type)  {
			
			case SDL_KEYDOWN: 
			
				switch (event.key.keysym.sym) {
                
					case SDLK_q: 	SDL_SetRenderTarget (renderer, NULL);
									return;
				}
				
			case SDL_MOUSEBUTTONDOWN : 		pan_start = event.button;					//in case mousemotion
											while (!SDL_PollEvent(&event))
												;
												
												
											if (event.type == SDL_MOUSEMOTION) {
												
												pan_manual (list, list_lines, &event, &pan_start, *scale);
				
												SDL_SetRenderTarget (renderer, NULL);									
												SDL_SetRenderDrawColor(renderer, BG_r, BG_g, BG_b, 255);
												SDL_RenderClear(renderer);
												
												place_objects_on_buffer(list);
											}
											
											if (event.type == SDL_MOUSEBUTTONUP 
												&& event.button.button == SDL_BUTTON_LEFT) {
												
												if (isin_box(turn_menu->black_turn_button, event.button)) {
													SDL_SetRenderTarget(renderer, turn_menu->setup_moves_menu);
													SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
													if (turn_type == wh) {
														SDL_RenderFillRect (renderer, &(turn_menu->white_turn_position));
														SDL_RenderCopy(renderer, turn_menu->turn_white, NULL, &(turn_menu->white_turn_position));
													}
													if (turn_type == alt) {	
														SDL_RenderFillRect (renderer, &(turn_menu->alt_turn_position));
														SDL_RenderCopy(renderer, turn_menu->alt_turn_black, NULL, &(turn_menu->alt_turn_position));
													}
													turn_type = bl;
													turn = black_s;
													
													SDL_SetRenderDrawColor (renderer, 200, 250, 200, 255);
													SDL_RenderFillRect (renderer, &(turn_menu->black_turn_position));
													SDL_RenderCopy(renderer, turn_menu->turn_black, NULL, &(turn_menu->black_turn_position));
													SDL_SetRenderTarget(renderer, NULL);
												}
												else if (isin_box(turn_menu->white_turn_button, event.button)) {
													SDL_SetRenderTarget(renderer, turn_menu->setup_moves_menu);
													SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
													if (turn_type == bl) {
														SDL_RenderFillRect (renderer, &(turn_menu->black_turn_position));
														SDL_RenderCopy(renderer, turn_menu->turn_black, NULL, &(turn_menu->black_turn_position));
													}
													if (turn_type == alt) {	
														SDL_RenderFillRect (renderer, &(turn_menu->alt_turn_position));
														SDL_RenderCopy(renderer, turn_menu->alt_turn_black, NULL, &(turn_menu->alt_turn_position));
													}
													turn_type = wh;
													turn = white_s;
												
													SDL_SetRenderDrawColor (renderer, 200, 250, 200, 255);
													SDL_RenderFillRect (renderer, &(turn_menu->white_turn_position));
													SDL_RenderCopy(renderer, turn_menu->turn_white, NULL, &(turn_menu->white_turn_position));
													SDL_SetRenderTarget(renderer, NULL);
												}
												else if (isin_box(turn_menu->alt_turn_button, event.button)) {
													SDL_SetRenderTarget(renderer, turn_menu->setup_moves_menu);
													SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
													if (turn_type == bl) {
														SDL_RenderFillRect (renderer, &(turn_menu->black_turn_position));
														SDL_RenderCopy(renderer, turn_menu->turn_black, NULL, &(turn_menu->black_turn_position));
													}
													if (turn_type == wh) {	
														SDL_RenderFillRect (renderer, &(turn_menu->white_turn_position));
														SDL_RenderCopy(renderer, turn_menu->turn_white, NULL, &(turn_menu->white_turn_position));
													}
													
													SDL_SetRenderDrawColor (renderer, 200, 250, 200, 255);
													SDL_RenderFillRect (renderer, &(turn_menu->alt_turn_position));
													if (turn_type == alt) {
														turn++;
														turn %= 2;
													}
													turn_type = alt;
													if (turn == black_s)
														SDL_RenderCopy(renderer, turn_menu->alt_turn_black, NULL, &(turn_menu->alt_turn_position));
													else SDL_RenderCopy(renderer, turn_menu->alt_turn_white, NULL, &(turn_menu->alt_turn_position));
													SDL_SetRenderTarget(renderer, NULL);
												}
											}
											break;
			
			
			case SDL_MOUSEWHEEL :			zoom_coords (list, list_lines, event, scale);
											
											SDL_SetRenderTarget (renderer, NULL);									
											SDL_SetRenderDrawColor(renderer, BG_r, BG_g, BG_b, 255);
											SDL_RenderClear(renderer);
											
											place_objects_on_buffer(list);
											
											break; 			
			
		}		
			

	}
}
	


										
											
	



