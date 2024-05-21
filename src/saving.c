#include "include/saving.h"

#define FILENAME_LENGTH 100






bool file_mentioned = FALSE;
int board_number;

char filename[FILENAME_LENGTH + 1];
char filepath[10 + FILENAME_LENGTH + 1];





void write_save (struct board *first_board, int n_boards, scaling *scale, bool new) {
	
	FILE *save;
	
	
	
	if (!file_mentioned || new) {
		printf ("Enter a filename: ");
		scanf("%s", filename);
	
		strcpy (filepath, "saves/");
		strcat (filepath, filename);
		
		file_mentioned = 1;
	}
	
	if ((save = fopen (filepath, "w")) == NULL)
		printf ("error: couldn't create savefile\n");
	
	
	
	
	struct board *walk = first_board;

	
	
	fprintf (save, "%d\n", n_boards);
	fprintf (save, "%lf\n", scale->amount);
	fprintf (save, "%lf %lf\n\n", scale->center.x, scale->center.y);


	while (walk != NULL) {
		
		fprintf (save, "#%d\n", walk->number);
		
		
		fprintf (save, "%d %d %d %d\n",
			walk->rep.center_off.x,
			walk->rep.center_off.y,
			walk->rep.size.x,
			walk->rep.size.y);

		if (walk->first_move)
			fprintf (save, "Y %d %d %d %d\n", 
			walk->first_move->colour,
			walk->first_move->S_no,
			walk->first_move->column,
			walk->first_move->row);
		else fprintf (save, ">\n");
		
		if (walk->last_move)
			fprintf (save, "Y %d %d %d %d\n", 
			walk->last_move->colour,
			walk->last_move->S_no,
			walk->last_move->column,
			walk->last_move->row);
		else fprintf (save, ">\n");
		
		
		
		fprintf (save, "%d\n", walk->mech.total_moves);
					
							
		
		fprintf (save, "\n");

		walk = walk->prev;
	}
	
	
	fprintf(save, "section\n\n");
	
	
	
	
	for (walk = first_board; walk != NULL; walk = walk->prev) {
	
		fprintf (save, "l#%d\n", walk->number);
		
		if (walk->above_board)
			//~ fprintf (save, "Y ");
			fprintf (save, "Y %d\n", walk->above_board->number);
		else fprintf (save, ">\n");
		
		if (walk->below) {
			fprintf (save, "Y ");
			for (struct spawn *stroll = walk->below; stroll != NULL; stroll = stroll->next)
				fprintf(save, "%d ", stroll->board->number);
		}
		fprintf (save, ">\n\n");
	}
		
		
	for (walk = first_board; walk; walk = walk->prev) {
		
		if (walk->first_move) {
			int i, j;
			fprintf (save, "mv%d: ", walk->number);
			for (int count = walk->first_move->S_no; 
					count <= walk->mech.total_moves; count++)
				for (i = 0; i < 19; i++) {
					for (j = 0; j < 19; j++) 
						if (walk->mech.state[i][j].S_no == count) {
							fprintf (save, "Y %d %d %d %d, ", i, j,
									walk->mech.state[i][j].S_no,
									walk->mech.state[i][j].colour);
							break;		
						} 
					if (j < 19)
						break;
				}
			fprintf (save, "/mv\n");
		}
		else fprintf (save, ">\n");
	}
	
	
	
	fprintf (save, "\n\n\n");
	
	for (walk = first_board; walk; walk = walk->prev) {
		fprintf (save, "b%d\n\n", walk->number);
		
		for (struct group *stroll = walk->groups; stroll; stroll = stroll->next) {
			fprintf (save, "g%d %d\n",  stroll->number, stroll->colour);
			fprintf (save, "l: ");
			
			for (struct	liberty *travel = stroll->liberties; travel; travel = travel->next)
				fprintf (save, "Y %d %d, ", travel->coord.y, travel->coord.x);
			
			fprintf (save, ">\nm: ");
			
			for (struct member *travel = stroll->members; travel; travel = travel->next)
				fprintf (save, "Y %d %d %d %d, ", travel->coord.y, travel->coord.x, travel->outfacing,
													walk->mech.state[travel->coord.y][travel->coord.x].merge);
			fprintf (save, ">\n\n");		
			fprintf (save, "CAP\n\n");
			
			for (struct member *travel = stroll->members; travel; travel = travel->next)
				if (walk->mech.state[travel->coord.y][travel->coord.x].captured_groups) 
					print_captured_groups (save, walk, travel, 
									walk->mech.state[travel->coord.y][travel->coord.x].captured_groups); 
			
		}
		
		fprintf (save, "\n\n");
	}
	
	
	fprintf (save, "E");
	
	fclose(save);
	
	printf ("\nwritten to %s\n", filename);
}	



void print_captured_groups (FILE *save, struct board *board, struct member *capturing_member, struct group *cap_group) {
	
	//~ struct group *cap_group = board->mech.state[capturing_member->coord.y][capturing_member->coord.x].captured_groups;
	
	fprintf (save, "c: %d %d\n", capturing_member->coord.y, capturing_member->coord.x);
	
	fprintf (save, "g%d%d\n",  cap_group->number, cap_group->colour);
			fprintf (save, "l: ");
	
	for (struct	liberty *travel = cap_group->liberties; travel; travel = travel->next)
				fprintf (save, "Y %d %d, ", travel->coord.y, travel->coord.x);
	
	fprintf (save, ">\nm: ");
	
	for (struct member *travel = cap_group->members; travel; travel = travel->next)
		fprintf (save, "Y %d %d %d | %d %d, ", travel->coord.y, travel->coord.x, travel->outfacing,
											travel->preserved_move->S_no, travel->preserved_move->merge);
	fprintf (save, ">\n\npcg\n\n");
	
	for (struct member *travel = cap_group->members; travel; travel = travel->next)
		if (travel->preserved_move->captured_groups)
			print_captured_groups (save, board, travel, travel->preserved_move->captured_groups);
											
											
	fprintf (save, "\n\n");				
	fprintf (save, "cap\n\n");
										
										
						//this makes no sense
	for (struct member *travel = cap_group->members; travel; travel = travel->next)
		if (board->mech.state[travel->coord.y][travel->coord.x].captured_groups) 
			print_captured_groups (save, board, travel, board->mech.state[travel->coord.y][travel->coord.x].captured_groups);	
			
	if (cap_group->next)
		print_captured_groups (save, board, capturing_member, cap_group->next);
}
	
	
	
	


void load_save (struct board **list, struct list_lines **list_lines, struct board **first_board, int *n_boards, scaling *scale, playing_parts *parts) {
	
	FILE *save;
	
	
	printf ("Enter a filename: ");
	scanf("%s", filename);
	
	strcpy (filepath, "saves/");
	strcat (filepath, filename);
	
	if ((save = fopen (filepath, "r")) == NULL)
		printf ("error: couldn't load savefile\n");
	
	
	
	*first_board = NULL;
	*list = NULL;
	*list_lines = NULL;
	int ch, i;
	struct board *p;
	
	fscanf (save, "%d\n", n_boards);
	fscanf (save, "%lf\n", &(scale->amount));
	fscanf (save, "%lf %lf\n", &(scale->center.x), &(scale->center.y));
	
	/*************************************************************************************************/
	
	
	
	
	
	//~ Getting the boards and some details
	
	
	while (1) {	
	
		if ((ch = getc(save)) == '#') {
			p = malloc(sizeof(struct board));
			if (p == NULL) {
				printf ("failed to add a board.");
				return;
			}
		}
		else if (ch == 's') {
				printf ("done\n");
				fscanf (save, "ection\n\n");
				break;
		}
		else { 
			printf("%c", ch);
			printf ("error reading save file.\n");
			return;
		}	
		
		
		//~ Initializing to NULL and 0
		
		p->mech.turn = 0;
		
		p->first_move = NULL;
		p->last_move = NULL;
		p->selection = NULL;
		p->line = NULL;
		
		p->groups = NULL;
		p->num_groups = 0;
		
		p->rep.snap = NULL,
	
		p->rep.snap = SDL_CreateTexture (renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, BOARD_SIZE, BOARD_SIZE);
		SDL_SetTextureBlendMode(p->rep.snap, SDL_BLENDMODE_BLEND);
		SDL_SetRenderTarget (renderer, p->rep.snap);
		SDL_SetRenderDrawColor (renderer, 0, 0, 0, 0);
		SDL_RenderClear(renderer);
		SDL_SetRenderTarget (renderer, NULL);
		
	
		p->above_board = NULL;
		p->below = NULL;

		
		
		p->next = *list;
		p->prev = NULL;
		if (*list)
			(*list)->prev = p;
		*list = p;

		
		
		//~ Getting board details: rep and number,   first & last moves,  total_moves
		
		fscanf (save, "%d\n", &(p->number));
		if (p->number == 1)
			*first_board = p;
				
		
		fscanf (save, "%d %d %d %d\n", 
				&(p->rep.center_off.x),
				&(p->rep.center_off.y),
				&(p->rep.size.x),
				&(p->rep.size.y));
				
		if ((ch = getc(save)) == 'Y') {
			p->first_move = malloc(sizeof(struct stone));
			fscanf (save, "%d %d %d %d\n", 
					&i,
					&(p->first_move->S_no),
					&(p->first_move->column),
					&(p->first_move->row));
			p->first_move->colour = i;
		}
		else if ((ch = getc(save)) == '>')
		   fscanf (save, "\n");	
		   
		if ((ch = getc(save)) == 'Y') {
			p->last_move = malloc(sizeof(struct stone));
			fscanf (save, "%d %d %d %d\n", 
					&i,
					&(p->last_move->S_no),
					&(p->last_move->column),
					&(p->last_move->row));
			p->last_move->colour = i;
		}
		else if ((ch = getc(save)) == '>')
		   fscanf (save, "\n");	
			

		fscanf (save, "%d\n", &(p->mech.total_moves));
		
		fscanf (save, "\n");			
	}
	
	/****************************************************************************************************/
	
	
	
	
	
		//~ Linking above and below boards, declaring lines
	
	struct spawn *last_added = NULL, **add_here = NULL;
	
	for (struct board *walk = *first_board; walk != NULL; walk = walk->prev) {
		
		fscanf (save, "l#%d\n", &i);
		if (i != walk->number) {
			printf ("board doesn't match\n");
			return;
		}
			
		if ((ch = getc(save)) == 'Y') {
			fscanf (save, "%d\n", &i);
			for (struct board *stroll = *list; stroll != NULL; stroll = stroll->next)
				if (stroll->number == i) {
					walk->above_board = stroll;
					
					walk->line = declare_new_line(stroll, walk, *scale);		//declaring the line
					walk->line->next = *list_lines;
					walk->line->prev = NULL;
					if (*list_lines)
						(*list_lines)->prev = walk->line;
					*list_lines = walk->line;
					
					break;
				}
				
		}
		else fscanf (save, "\n");
		
		
		if ((ch = getc(save)) ==  'Y') {
			add_here = &(walk->below);
			while (1) {
				if (fscanf (save, "%d ", &i) > 0) {
					
					*add_here =  malloc(sizeof(struct spawn));
					last_added = *add_here;
					add_here = &(last_added->next);
					//~ if (!walk->below) {
						//~ walk->below = malloc(sizeof(struct spawn));
						//~ last_added = walk->below;
					//~ }
					//~ else {
						//~ last_added->next = malloc(sizeof(struct spawn));
						//~ last_added = last_added->next;
					//~ }
					
					for (struct board *stroll = *list; stroll != NULL; stroll = stroll->next) {
						if (stroll->number == i) {	
							last_added->board = stroll;
							last_added->next = NULL;
							break;
						}
					}
				}
				else { 
					fscanf (save, ">\n\n");
					break;
					}
			}
		}
		else fscanf (save, "\n\n");
	}
	
	/*************************************************************************************************/
	
	
	
	
	
	
				//____Reading moves into linked lists____
	
	struct list_moves *list_moves = NULL;
	struct list_moves *new_node = NULL;
	struct read_move *last_read = NULL;
	
	while (1) {
		
		if ((ch = getc(save)) == 'm') {	
				
			if (!list_moves) {
				list_moves = malloc(sizeof(struct list_moves));
				list_moves->next = NULL;
				list_moves->moves = NULL;
				new_node = list_moves;
			}
			else {
				new_node->next = malloc(sizeof(struct list_moves));
				new_node = new_node->next;
				new_node->moves = NULL;
				new_node->next = NULL;
			}		
							
			fscanf (save, "v%d: ", &(new_node->board_number));
			
			while (1) {
				if ((ch = getc(save)) == 'Y') {
					if (!new_node->moves) {
						new_node->moves = malloc(sizeof(struct read_move));
						last_read = new_node->moves;
					}
					else {
						last_read->next = malloc(sizeof(struct read_move));
						last_read = last_read->next;
					}
					fscanf (save, "%d %d %d %d, ", 
						&(last_read->coord.y),
						&(last_read->coord.x),
						&(last_read->S_no),
						&(last_read->colour));
						
					last_read->next = NULL;
				}
				else if (ch  == '/') {
					fscanf (save, "mv\n");
					break;
				}
				else { 
					printf ("error reading moves\n");
					return;
				}
			}
		}
		else if (ch == '>')
			fscanf (save, "\n");
		
		else if (ch == 'E')
			break;
			
	}
	
	/**************************************************************************************************/
	
	
	
	
	
	if (!list_moves) {
		fclose(save);
		printf ("\nloaded: %s\n", filename);
		file_mentioned = 1;
		return;
	}
		
		
		
		
		//Printing moves on the first board, updating stats.  Why not just play moves?
		
		
			//taking the moves list of the first board out of the list_moves of the game. Deallocating
			//the list_moves node and only keeping the read_moves list. 
	struct read_move *walk = list_moves->moves;
	list_moves->moves = NULL;						
	struct list_moves *temp = list_moves;
	list_moves = list_moves->next;
	free(temp);
	
							//This is needed. This is the config that is copied, and if it is dirty, 
							//there'll be random stones throughout the boards except the first board
							//since the first board does not have stones printed from the mech but only
							//the numbered stones that are loaded.
	for (int i = 0; i < 19; i++)
	for (int j = 0; j < 19; j++) {
		(*first_board)->mech.state[i][j].S_no = 0;
		(*first_board)->mech.state[i][j].colour = 0;
		(*first_board)->mech.state[i][j].group = NULL;
	}
	
	parts->board = *first_board;
	
	
	while (walk != NULL) {
	
		parts->number = walk->S_no;
		
		if (walk->colour == 2) {
			place_stone (walk->coord.y, walk->coord.x, parts->board, parts->whiteStone);
			parts->font_color.r = 0; parts->font_color.g = 0; parts->font_color.b = 0;
		}
		else  {
			place_stone (walk->coord.y, walk->coord.x, parts->board, parts->blackStone);
			parts->font_color.r = 255; parts->font_color.g = 255; parts->font_color.b = 255;
		}
		put_number(walk->coord.y, walk->coord.x, parts);
		
		
									//updating the stats of the board.
					
		//~ parts->board->mech.state[walk->coord.y][walk->coord.x].S_no = ++(parts->board->mech.total_moves);
		parts->board->mech.state[walk->coord.y][walk->coord.x].S_no = walk->S_no;
										
		parts->board->mech.state[walk->coord.y][walk->coord.x].colour = parts->board->mech.turn + 1;  // + 1 because the colour enum has "empty" as the first element.  
		(parts->board->mech.turn)++; 
		parts->board->mech.turn %= 2;
		
		
		
		struct read_move *temp = walk;
		walk = walk->next;
		free(temp);
	}
	
	
	/*************************************************************************************************/
	
	
	
	
	
	
		//For debugging

	//~ for (struct list_moves *walk = list_moves; walk != NULL; walk = walk->next) {
		//~ printf ("%d: ", walk->board_number);
		//~ for (struct read_move *stroll = walk->moves; stroll != NULL; stroll = stroll->next) {
			//~ printf ("%d %d %d %d, ", 
				//~ stroll->coord.x,
				//~ stroll->coord.y,
				//~ stroll->S_no,
				//~ stroll->colour);
		//~ }
		//~ printf ("\n");
	//~ }
	
	for (struct spawn *tick = (*first_board)->below; 
			tick; tick = tick->next)
		recur_load_moves(tick->board, list_moves, *list, parts);
		
	
	
	fclose(save);
	printf ("\nloaded: %s\n", filename);
	file_mentioned = TRUE;
}






	
void recur_load_moves (struct board *board, struct list_moves *list_moves, struct board *list, playing_parts *parts) {
	

	board->mech = board->above_board->mech;		//copying the config
								
													
											//placing stones from the board above
	for (int column = 0; column < 19 ; column++) 
		for (int row = 0; row < 19; row++)  {
			if (board->mech.state[column][row].colour == 1) 
				place_stone (column, row, board, blackStone);
			else if (board->mech.state[column][row].colour == 2)
				place_stone (column, row, board, whiteStone);
		}
		
		
			//Getting the moves list for this board and taking out its node from the list_moves 
		
	struct read_move *walk = NULL;
	struct list_moves *prev = NULL, *stroll = list_moves;
	for (; stroll != NULL; prev = stroll, stroll = stroll->next) {
		
		if (stroll->board_number == board->number) {
			
			walk = stroll->moves;
			//~ board_number = stroll->board_number;
			struct list_moves *temp = stroll;
			if (!prev)
				list_moves = stroll->next;
			else prev->next = stroll->next;
			free(temp);
			break;
		}
	}
	
	parts->board = board;
			
	while (walk) {			//printing the moves played on this board, updating the mech
	
		parts->number = walk->S_no - board->above_board->mech.total_moves;
		
		
		if (walk->colour == 2) {
			place_stone (walk->coord.y, walk->coord.x, parts->board, parts->whiteStone);
			parts->font_color.r = 0; parts->font_color.g = 0; parts->font_color.b = 0;
		}
		else  {
			place_stone (walk->coord.y, walk->coord.x, parts->board, parts->blackStone);
			parts->font_color.r = 255; parts->font_color.g = 255; parts->font_color.b = 255;
		}
		put_number(walk->coord.y, walk->coord.x, parts);
		
		
									//updating the stats of the board.
					
		parts->board->mech.state[walk->coord.y][walk->coord.x].S_no = ++(parts->board->mech.total_moves);
										
		parts->board->mech.state[walk->coord.y][walk->coord.x].colour = parts->board->mech.turn + 1;  // + 1 because the colour enum has "empty" as the first element.  
		(parts->board->mech.turn)++; 
		parts->board->mech.turn %= 2;
		
		
		
		struct read_move *temp = walk;
		walk = walk->next;
		free(temp);
		
	}

	
		
	
	
	for (struct spawn *tick2 = board->below; 
			tick2 != NULL; tick2 = tick2->next) 
		recur_load_moves (tick2->board, list_moves, list, parts);
}
	
	
