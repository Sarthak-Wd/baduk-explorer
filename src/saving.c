#include "saving.h"
#define FILENAME_LENGTH 100

bool file_mentioned = 0;
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
		
		
	for (walk = first_board; walk != NULL; walk = walk->prev) {
		
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
	
	fprintf (save, "E");
	
	fclose(save);
	
	printf ("\nwritten to %s\n", filename);
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
	
	while (1) {	
	
		if ((ch = getc(save)) == '#') {
			printf ("%c\n", ch); 
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
	
	
	
	
	struct spawn *last_added;
	
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
					
					walk->line = declare_new_line(stroll, walk, *scale);
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
			while (1) {
				if (fscanf (save, "%d ", &i) > 0) {
					
					if (!walk->below) {
						walk->below = malloc(sizeof(struct spawn));
						last_added = walk->below;
					}
					else {
						last_added->next = malloc(sizeof(struct spawn));
						last_added = last_added->next;
					}
					
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
	
	
	if (!list_moves) {
		fclose(save);
		printf ("\nloaded: %s\n", filename);
		file_mentioned = 1;
		return;
	}
	
	if (list_moves->board_number == 1) {
		
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
			(*first_board)->mech.state[i][j].ptp_group = NULL;
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
	}
	
	
	for (struct list_moves *walk = list_moves; walk != NULL; walk = walk->next) {
		printf ("%d: ", walk->board_number);
		for (struct read_move *stroll = walk->moves; stroll != NULL; stroll = stroll->next) {
			printf ("%d %d %d %d, ", 
				stroll->coord.x,
				stroll->coord.y,
				stroll->S_no,
				stroll->colour);
		}
		printf ("\n");
	}
	
	for (struct spawn *tick = (*first_board)->below; tick != NULL;
			tick = tick->next)
		recur_load_moves(tick->board, list_moves, *list, parts);
		
	
	
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
	
	
	fclose(save);
	printf ("\nloaded: %s\n", filename);
	file_mentioned = 1;
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
			
			while (walk) {
			
				for (struct board *tick = list; tick != NULL; tick = tick->next)
					if (board->number == tick->number)
						parts->board = tick;
				
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
	
	
