#include <stdio.h>
#include "common.h"
#include "init.h"
#include "camera.h"




SDL_Event event;
SDL_MouseButtonEvent pan_start;
SDL_Rect select_indicator = { 0, 0, (BOARD_SIZE+20), (BOARD_SIZE+20) };
bool game_is_running = TRUE;
double scale = 1.0;
int n_boards = 1;
double center_x_scaled = CENTER_X, center_y_scaled = CENTER_Y;
bool txt_display = FALSE;
struct list *infocus = NULL;

int frames_rendered = 0;







struct branch *branch 	= NULL; 
struct list 	 *list 	= NULL; 
struct opted *sel 	= NULL;
struct list_lines 	 *list_lines	= NULL;
struct message text;
	


		


//Function Prototypes


void load_setup (void);
bool process_input(void);
void render (struct list *p);

void play_move   (SDL_Event event, struct list *p, SDL_Texture *blackStone, SDL_Texture *whiteStone);
void undo_move (struct list *p, bool branching);
struct list* add_board (int *n_boards);
void delete_board (struct list *p, int *n_boards);
void recur_delete (struct list *p, struct delete_list *listof, int *n_boards);
void select_board (struct list *p);
void shift_elements (struct opted *p, SDL_Event *event, SDL_MouseButtonEvent *pan_start);

void continue_play (struct list *p, SDL_Texture *blackStone, SDL_Texture *whiteStone);
void branch_window (struct list *p);
void off_shoot (struct list *p, int row, int column, int moveNum, int *n_boards);

void shift_one (struct list *p, int shift_x, int shift_y, int shift_x_scaled, int shift_y_scaled);
void recur_shift (struct spawn *b);
struct opted *opt_in (struct list *p, struct opted **optList);
void fit_in_list (struct list *new_item, struct list_lines *new_line);
struct list *declare_new_board (int *n_boards);
struct list_lines *declare_new_line (struct list *start_item, struct list *end_item);
void place_stone (int x, int y, struct board *board, int number, SDL_Texture *stone_black, SDL_Texture *stone_white, bool branching_mode);
bool isin_box (SDL_Rect rect, SDL_MouseButtonEvent button);

void display_text (struct message text);
void ticker (void);
void testing (void);
void inspect_board (struct list p);




int main ()
{
	
	
	if (!initialize_window())
		destroy_window();
	
	load_setup(); 
	
	
	while (game_is_running) {
			
		SDL_SetRenderDrawColor(renderer, 100, 200, 150, 255);
		SDL_RenderClear(renderer);
		
		render (list);
	
		while (!process_input()) 
			;
			
		if (!game_is_running)
			break; 
			
		//testing();
	}
	
	destroy_window();
	return 0;
}

		 



bool process_input(void)  {
			
	
		
	while (!SDL_PollEvent(&event))
		;

		
	switch (event.type) {
		
		case SDL_QUIT: 
			game_is_running = FALSE;
			return TRUE;
		
		
		
		case SDL_KEYDOWN: 
		
			switch (event.key.keysym.sym) {
                
                case SDLK_ESCAPE: 	game_is_running = FALSE; break;
               		
				case SDLK_LALT:		while (1) {
										SDL_PollEvent(&event);
										if (event.key.keysym.sym == SDLK_b)  {
											continue_play(list, blackStone, whiteStone);
											break;
										}
										if (event.button.button == SDL_BUTTON_LEFT) {
											render(list); 		//to clear any warning messages displayed on last input. //might not need this since will need to render to center the board anyway.
											struct list *p = list;
											for ( ; p != NULL; p = p->next)
												if (isin_box(p->node->board.rep.size, event.button)) {
													
													printf ("number: %d, total moves: %d\n", p->number, p->node->board.mech.total_moves); 
													branch_window(p);
													break;
												}
											break;
										}
										
										if (event.type == SDL_KEYUP)
											if (event.key.keysym.sym == SDLK_LALT)
												break;
										
										
									} break;
				
				case SDLK_d:		while (1) {
										while (!SDL_PollEvent(&event))
											;
										if (event.button.button == SDL_BUTTON_LEFT) {	
											struct list *p = list;
											for ( ; p != NULL; p = p->next)
												if (isin_box(p->node->board.rep.size, event.button)) { 
													delete_board(p, &n_boards);
													break;
												}
											break;
										}
										
										if (event.type == SDL_KEYUP)
											if (event.key.keysym.sym == SDLK_d)
												break;
									}
									
				case SDLK_i:		while (1) {
										while (!SDL_PollEvent(&event))
											;
										if (event.button.button == SDL_BUTTON_LEFT) {	
											struct list *p = list;
											for ( ; p != NULL; p = p->next)
												if (isin_box(p->node->board.rep.size, event.button)) { 
													inspect_board(*p);
													break;
												}
											break;
										}
										
										if (event.type == SDL_KEYUP)
											if (event.key.keysym.sym == SDLK_d)
												break;
									}
            }
            return TRUE;
		

		case SDL_MOUSEBUTTONDOWN:
			
			if (event.button.button == SDL_BUTTON_MIDDLE) {
				for (struct list *p = list; p != NULL; p = p->next)
					if (isin_box(p->node->board.rep.size, event.button)) { 
						select_board(p);
						break;
					}
				return TRUE;
			}
			
				
			pan_start = event.button;
			

			while (!SDL_PollEvent(&event))
				;			//as long as there is no input
				

			if (event.type == SDL_MOUSEMOTION) {
				
				struct opted *q = sel;	
				for (; q != NULL; q = q->next) 
					if (isin_box (q->list->node->board.rep.size, pan_start)) {
						shift_elements(sel, &event, &pan_start);
						break;
					}
		
				if (q == NULL) 
				 pan_coords(list, list_lines, &event, &pan_start, scale, center_x_scaled, center_y_scaled);
			}
					
			else if (event.type == SDL_MOUSEBUTTONUP) { 
				
				if (event.button.x > (WINDOW_WIDTH - 50)  && event.button.y < 50)
					add_board(&n_boards);		//improve, make a button, w/ a rect
					
				else  if (event.button.button == SDL_BUTTON_LEFT) {
						struct list *p = list;
						for ( ; p != NULL; p = p->next)
							if (isin_box(p->node->board.rep.size, event.button)) {
								play_move (event, p, blackStone, whiteStone);
								break;
							}	
				}
				else { struct list *p = list;
						for ( ; p != NULL; p = p->next)
							if (isin_box(p->node->board.rep.size, event.button)) {
								undo_move(p, FALSE);
								break;
							}
						}	
							
										
			}
			return TRUE;
				
				
		case SDL_MOUSEWHEEL:
			
			zoom_coords (list, list_lines, event, &scale, &center_x_scaled, &center_y_scaled);			
			return TRUE;
	}
	
	return FALSE;
}






void play_move   (SDL_Event event, struct list *p, SDL_Texture *blackStone, SDL_Texture *whiteStone)  {
	
	if (p->node->below != NULL) {
		text.str = "Can't make changes to a board which has children boards. Enter the branching mode to create a branch.";
		text.coord.x = 50;
		text.coord.y = 50;
		
		SDL_Color bg_color = {200, 180, 125, 255};
		SDL_Color txt_color = {255, 255, 255, 255};
		text.bg_color = bg_color;
		text.txt_color = txt_color;

		txt_display = TRUE;
		return;
	}
	
	
	
	double x, y;
	int column, row;
	
	x 	= 	(event.button.x - (p->node->board.rep.size.x + BORDER*scale)) / (SQUARE_SIZE*scale); 
	y 	= 	(event.button.y - (p->node->board.rep.size.y + BORDER*scale)) / (SQUARE_SIZE*scale);
	
	
										//rounding:   row and column are swapped because that is how an array be.
	if ((x - (int)x) >= 0.5)
		 row = (int)x + 1;
	else row = (int)x;
	
	if ((y - (int)y) >= 0.5)
		 column = (int)y + 1;
	else column = (int)y;
	
	if (p->node->board.mech.state[row][column].colour != empty)
		return;
	
	p->node->board.mech.state[row][column].S_no = ++(p->node->board.mech.total_moves);
	
	
	int number;
	if (p->node->above != NULL)
		number = p->node->board.mech.total_moves - p->node->above->last_move->S_no; 
	else number = p->node->board.mech.total_moves;
	
	place_stone (row, column, &(p->node->board), number, blackStone, whiteStone, FALSE);
	
	infocus = p;
	
	
						//putting the first move of the new board in the below link of the board above it.					
	
	if (p->node->above == NULL)		//if it's the first board, it won't have a board above.
		return;
									
	if (p->node->above->last_move->S_no == (p->node->board.mech.total_moves - 1)) {		//if it is the first move on this board.
		struct list *q = p->node->above->item;
		*(q->node->below->first_move) = p->node->board.mech.state[column][row];
		
		//   ->board_coords.y = column;
		//q->node->below->first_move->board_coords.x 	= row;
	}
}





void undo_move (struct list *p, bool branching)  {
	
	if (p->node->board.mech.total_moves <= 0)	
		return;
		
	if (p->node->above != NULL) 				//disallowing undoing the moves of the parent board on the bottom boards.
		if (p->node->board.mech.total_moves == p->node->above->last_move->S_no)
			return;	
	
	
	if (p->node->below != NULL && branching == FALSE) {
		text.str = "Can't make changes to a board which has children boards. Enter the branching mode to create a branch.";
		text.coord.x = 50;
		text.coord.y = 50;
		
		SDL_Color bg_color = {200, 180, 125, 255};
		SDL_Color txt_color = {255, 255, 255, 255};
		text.bg_color = bg_color;
		text.txt_color = txt_color;
	
		txt_display = TRUE;
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
	(p->node->board.mech.turn)--; 
	p->node->board.mech.turn %= 2;
	
	infocus = p;

	SDL_Rect undoSize = { ((i*SQUARE_SIZE + BORDER) - 15), ((j*SQUARE_SIZE + BORDER) - 15), STONE_SIZE, STONE_SIZE};
	
	SDL_SetTextureBlendMode(p->node->board.rep.snap, SDL_BLENDMODE_BLEND);	//colouring a part of the texture transparent. 
	SDL_SetRenderTarget (renderer, p->node->board.rep.snap);				
	SDL_SetRenderDrawColor (renderer, 0, 0, 0, 0);
	SDL_RenderFillRect (renderer, &undoSize);
	SDL_SetRenderTarget (renderer, NULL);
}
	


			
struct list* add_board (int *n_boards)  	{
	
	
	if (infocus == NULL)
		return NULL;
	
	struct list *new_item = declare_new_board(n_boards);
	
	
									//in the branch: linking the parent and the spawn board through the 'above' and 'below' members
	
	if ((new_item->node->above = malloc(sizeof(struct parent))) == NULL)	//declared 'above' as a pointer in the list struct for consistency. It could just be a regular member since parent is not a linked list.
		printf ("couldn't allocate memory for spawn struct");
	new_item->node->above->item = infocus;									//list is any board that was last declared. It is possible to add a board else where. I need something other than list here. So: infocus
	new_item->node->below = NULL;
	
	
			//This is messed up. This way there cannot be more than two below links for any board.
	/*
	if (infocus->node->below != NULL) {
		if ((infocus->node->below->next = malloc(sizeof(struct spawn))) == NULL)	
			printf ("couldn't allocate memory for spawn struct");
		infocus->node->below->next->item = new_item;
		infocus->node->below->next->next = NULL;
	}
	else {
		if ((infocus->node->below = malloc(sizeof(struct spawn))) == NULL)	
			printf ("couldn't allocate memory for spawn struct");	
		infocus->node->below->item = new_item;
		infocus->node->below->next = NULL;	
	}
	*/
	
								//instead
	struct spawn *new_spawn = malloc(sizeof(struct spawn));	
	if (new_spawn == NULL)	
			printf ("couldn't allocate memory for spawn struct");
						
	new_spawn->item = new_item;
	new_spawn->next = infocus->node->below;
	infocus->node->below = new_spawn;
	
	
									//line initialization
	struct list_lines *new_line = declare_new_line (infocus, new_item);
	
	
									//fitting the line into the branch
									
	new_item->node->above->line = new_line;			//in the branch						
	for(struct spawn *b = new_item->node->above->item->node->below;
		b != NULL; b = b->next)
		if (b->item->number == new_item->number) {
			b->line = new_line;
			break;
		}
									
									//fitting the item and line into the universal lists
	fit_in_list (new_item, new_line);
	
	infocus = NULL;
	
	return list;
}





void delete_board (struct list *p, int *n_boards) {	
	
	
	//struct delete_list *listof = NULL;
	struct spawn *temp = p->node->above->item->node->below;
	struct spawn *prev = NULL;
	 		 
	 		 
									//This is to delete the below item of the board above the first board to be deleted. 
									//And then adjusting the below linked list of the board above.		
	for (; temp != NULL && temp->item->node->number != p->node->number; 
			prev = temp, temp = temp->next )
			;
		
	if (temp == NULL)	//board wasn't found? Can't happen but just for safety.
		return;
		
	if (prev == NULL)	
		p->node->above->item->node->below = temp->next;
		
	else 
		prev->next = temp->next;
		
	free(temp);
	
	//recur_delete(p, listof, n_boards);
	
	struct opted *optList = NULL;
	
	opt_in(p, &optList);
	//for (; optList != NULL; optList = optList->next)
	
											//adjusting pointers and deleting
	for (struct list_lines *q; optList != NULL; optList = optList->next) {

								// *** adjusting pointers of the linked lists ***
		p = optList->list;		
										// * lines *
		q = p->node->above->line;
		if (q->next != NULL)				//if q is not the only in list_lines
			q->next->prev = q->prev;
		if (q->prev != NULL)				//if q is not the last in list_lines 
			q->prev->next = q->next;
		else list_lines = q->next;
		
										// * selection indicators *
		if (p->selection != NULL) 	{
			if (p->selection->prev == NULL) 		//if it is the first board in the list (last added?)
				sel = p->selection->next;
			else p->selection->prev->next = p->selection->next;
			
			if (p->selection->next != NULL)		//if the selection is not the last in the list
				p->selection->next->prev = p->selection->prev;
			
			free (p->selection);
			p->selection = NULL;
		}
										// * boards *
		if (p->next != NULL)				//if p is not the only in list
			p->next->prev = p->prev;
		if (p->prev != NULL)				//if p is not the last in list
			p->prev->next = p->next;
		else list = p->next;
									
		free(p->node->above->line);
		free(p);
		--(*n_boards);
		printf ("deleting\n");
	}
		
	infocus = NULL;			//need to do this. Otherwise, if I delete an infocus board, and don't equate this to null, bug: a board, randomly filled will appear on the left edge. 
	
	
}
		

/*
void recur_delete (struct list *p, struct delete_list *listof, int *n_boards) {
	 
	
	for (;;) {								//adding boards to the todelete list.
		
		struct delete_list *new_item = malloc(sizeof(struct delete_list));
		
		new_item->todelete = p;
		new_item->next = listof;
		listof = new_item;
		
		if (p->node->below == NULL)
			break;
				
		for(struct list *q = p; 
			q->node->below->next != NULL; 
			q = q->node->below->next->item) 
				recur_delete(p->node->below->next->item, listof, n_boards);	
					
		p = p->node->below->item;
	}
	
											//adjusting pointers and deleting
	for (struct list_lines *q; listof != NULL; listof = listof->next) {

								// *** adjusting pointers of the linked lists ***
		p = listof->todelete;		
										// * lines *
		q = p->node->above->line;
		if (q->next != NULL)				//if q is not the only in list_lines
			q->next->prev = q->prev;
		if (q->prev != NULL)				//if q is not the last in list_lines 
			q->prev->next = q->next;
		else list_lines = q->next;
		
										// * opted indicators *
		if (p->opted != NULL) 	{
			if (p->opted->prev == NULL) 		//if it is the first board in the list (last added?)
				sel = p->opted->next;
			else p->opted->prev->next = p->opted->next;
			
			if (p->opted->next != NULL)		//if the opted is not the last in the list
				p->opted->next->prev = p->opted->prev;
			
			free (p->opted);
			p->opted = NULL;
		}
										// * boards *
		if (p->next != NULL)				//if p is not the only in list
			p->next->prev = p->prev;
		if (p->prev != NULL)				//if p is not the last in list
			p->prev->next = p->next;
		else list = p->next;
									
		free(p->node->above->line);
		free(p);
		--(*n_boards);
	}
		
	infocus = NULL;			//need to do this. Otherwise, if I delete an infocus board, and don't equate this to null, bug: a board, randomly filled will appear on the left edge. 
	
}
*/





void select_board (struct list *p) {
	
	
	if (p->selection != NULL) 	{
		if (p->selection->prev == NULL) 		//if it is the first board in the list (last added?)
			sel = p->selection->next;
		else p->selection->prev->next = p->selection->next;
		
		if (p->selection->next != NULL)		//if the selection is not the last in the list
			p->selection->next->prev = p->selection->prev;
		
		free (p->selection);
		p->selection = NULL;
		return;
	}
		
	
	struct opted *new_node = malloc(sizeof(struct opted)); 
	new_node->list = p;
	
	new_node->next = sel;
	new_node->prev = NULL;
	if (sel != NULL)   		//if the list is not empty.
		sel->prev = new_node;
	sel = new_node; 
	
	p->selection = new_node;
	
}
 
		
			
void shift_elements (struct opted *p, SDL_Event *event, SDL_MouseButtonEvent *pan_start)  {
	
	int shift_x, shift_y, shift_x_unscaled, shift_y_unscaled;
	
	
	while (event->type != SDL_MOUSEBUTTONUP) 
		SDL_PollEvent(event);
		

	shift_x = event->button.x - pan_start->x;
	shift_y = event->button.y - pan_start->y;
	shift_x_unscaled = (int)(shift_x/scale);				//calculating things outside of the loop 
	shift_y_unscaled = (int)(shift_y/scale);
	
	for (; p != NULL; p = p->next)
		shift_one (p->list, shift_x, shift_y, shift_x_unscaled, shift_y_unscaled);
	
}





//don't need the list parameter? it is provided by add_board anyway?
void continue_play (struct list *p, SDL_Texture *blackStone, SDL_Texture *whiteStone) {
	
	
	if ((p = add_board(&n_boards)) == NULL)
		return;							  
										//first to the new board??
	
	p->node->board.mech = p->node->above->item->node->board.mech;		//the parent and spawns can just be accessed through the above and below members of the current board. 
													
	printf ("%d, %d", p->node->board.mech.turn, p->node->above->item->node->board.mech.turn);		
	
								
													//placing stones upto the parent board state
	SDL_SetRenderTarget (renderer, p->node->board.rep.snap);
								
	for (int i = 0; i < 19 ; i++) 
		for (int j = 0; j < 19; j++) {
			if (p->node->board.mech.state[i][j].colour != empty) {
				
				place_stone (i, j, &(p->node->board), 0, blackStone, whiteStone, FALSE);
			}
		}
						
	SDL_SetRenderTarget (renderer, NULL);
	
	printf ("%d, %d", p->node->board.mech.turn, p->node->above->item->node->board.mech.turn);
	
									//adding a spawn link, putting in details							
									
	struct list *q = p->node->above->item;		//board->below always points to the last declared spawn. So, no need for a loop.
	
	p->node->above->last_move	 = malloc(sizeof(struct moves));
	q->node->below->first_move   = malloc(sizeof(struct moves));
	q->node->below->first_move->S_no = (q->node->board.mech.total_moves + 1);
	//the column and row are assigned in the play_move function since the first stone of the brancing window has not been played yet.  
	
	
					//setting the last_move of the board in the above member of the new board
	int i, j;
	
	for (i = 0; i < 19; i++) {
		for (j = 0; j < 19; j++) 
			if (q->node->board.mech.state[i][j].S_no == q->node->board.mech.total_moves)
				break;
		if (j < 19)
			break;
	}
	
	*(p->node->above->last_move) = q->node->board.mech.state[i][j];
	
	/*
	p->node->above->last_move->S_no = q->node->board.mech.total_moves;
	p->node->above->last_move->board_coords.y = j;
	p->node->above->last_move->board_coords.x = i;
	*/
}								
	





void branch_window (struct list *p) {
	
	
	int current_move = p->node->board.mech.total_moves;
	infocus = p;
	
										//preserving the actual board state.
										
	SDL_Texture *preserve = SDL_CreateTexture (renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, BOARD_SIZE, BOARD_SIZE);
	SDL_SetRenderTarget (renderer, preserve);
	SDL_RenderCopy(renderer, p->node->board.rep.snap, NULL, NULL);
	bool preserve_turn = p->node->board.mech.turn;
	
	
										//placing the branching indicator.
										
	select_indicator.w = (BOARD_SIZE + 80) * scale;
	select_indicator.h = (BOARD_SIZE + 80) * scale;
	
	select_indicator.x = p->node->board.rep.size.x - (40 * scale);
	select_indicator.y = p->node->board.rep.size.y - (40 * scale);
	
	SDL_SetRenderTarget (renderer, NULL);
	SDL_RenderCopy (renderer, branchTex, NULL, &select_indicator);
	render(list);
	
	
	
									//input loop : 			//going back and forth in the branching window
	while (1) {												//branching, if new move
		
		SDL_SetRenderTarget (renderer, p->node->board.rep.snap);
	
	    
		while (!SDL_PollEvent(&event))
			;	

		/*
		int i, j;
		for (i = 0; i < 19; i++) {
			for (j = 0; j < 19; j++)
				printf ("%d ", list->node->board.mech.state[j][i].colour);
			putchar('\n');
		}
		*/
		
		if (event.type == SDL_MOUSEBUTTONDOWN) {
			
			
			while (!SDL_PollEvent(&event))
				;
		
			if (event.type == SDL_MOUSEMOTION)
				while (1) {
					SDL_PollEvent(&event);
					if (event.type == SDL_MOUSEBUTTONUP && event.button.button == SDL_BUTTON_LEFT)
						continue;
				}
			
			else if (event.type == SDL_MOUSEBUTTONUP && event.button.button == SDL_BUTTON_LEFT) {
				
				if (!isin_box(infocus->node->board.rep.size, event.button))
					continue;
				
				int number;
				double x, y;
				int column, row;
				
				x 	= 	(event.button.x - (p->node->board.rep.size.x + BORDER*scale)) / (SQUARE_SIZE*scale); 
				y 	= 	(event.button.y - (p->node->board.rep.size.y + BORDER*scale)) / (SQUARE_SIZE*scale);
			
													//rounding:  row and column are swapped because that is how an array be.
				if ((x - (int)x) >= 0.5)
					 row = (int)x + 1;
				else row = (int)x;
				
				if ((y - (int)y) >= 0.5)
					 column = (int)y + 1;
				else column = (int)y;
				
				
				
				if (p->node->board.mech.state[row][column].S_no != current_move + 1) {
					off_shoot(p, row, column, current_move, &n_boards);
					return;
				}
				else {
					current_move++;
					
					if (p->node->above != NULL) 			
						number = current_move - p->node->above->last_move->S_no;		
					else number = current_move;
					
					place_stone (row, column, &(p->node->board), number, blackStone, whiteStone, FALSE);
					
					SDL_SetRenderTarget (renderer, NULL);	//this is needed of course so that render() renders the entire screen
					render(list);
					
				}
			}
		}
			
			
			
		if (event.key.keysym.sym == SDLK_LEFT && event.type == SDL_KEYUP) {
			
			int number;
			if (p->node->above != NULL) { 				
				if (current_move == p->node->above->last_move->S_no)		//disallowing undoing the moves of the parent board on the board below.
					continue;
				number = current_move - p->node->above->last_move->S_no;	
			}
			else {
				if (current_move == 0) 
					continue;
				number = current_move;
			}
			 
			//printf ("%d\n", current_move);
	
			  
			int i, j;
			for (i = 0; i < 19; i++) {
				for (j = 0; j < 19; j++) {
					if (p->node->board.mech.state[i][j].S_no == current_move) 	
						break;
				}
				if (j < 19)
					break;
			}
			
			current_move--;	
			
			
			place_stone (i, j, &(p->node->board), number, ghost_blackStone, ghost_whiteStone, TRUE);
			
			SDL_SetRenderTarget (renderer, NULL);
			render(list);
		} 
		
		
		else if (event.key.keysym.sym == SDLK_RIGHT && event.type == SDL_KEYUP) {
			
			if (current_move == p->node->board.mech.total_moves)
				continue;
				
			current_move++;			//this has to be b4 the changes, unlike when going backwards.
			
			int number;
			if (p->node->above != NULL) 			//disallowing undoing the moves of the parent board on the board below.
				number = current_move - p->node->above->last_move->S_no;		
			else number = current_move;
			
			int i, j;
			for (i = 0; i < 19; i++) {
				for (j = 0; j < 19; j++) {
					if (p->node->board.mech.state[i][j].S_no == current_move) 	
						break;
				}
				if (j < 19)
					break;
			}
			
			place_stone (i, j, &(p->node->board), number, blackStone, whiteStone, FALSE);
			
			SDL_SetRenderTarget (renderer, NULL);	//this is needed of course so that render() renders the entire screen
			render(list);
			
		}
			
		
		else if (event.key.keysym.sym == SDLK_q) {
			SDL_SetRenderTarget (renderer, p->node->board.rep.snap);
			SDL_RenderCopy(renderer, preserve, NULL, NULL);
			SDL_SetRenderTarget (renderer, NULL);
			p->node->board.mech.turn = preserve_turn;
			break;
		}
	}
}
	




void off_shoot (struct list *p, int row, int column, int moveNum, int *n_boards) {
	
	struct list *new_item_1 = declare_new_board(n_boards);
	
	
	
						//putting the board in the branch
	
	if ((new_item_1->node->above = malloc(sizeof(struct parent))) == NULL)	
		printf ("couldn't allocate memory for spawn struct");
	new_item_1->node->above->item = infocus;
	
												//adjusting the new item with all the links below.
	if (infocus->node->below != NULL) {
		new_item_1->node->below = infocus->node->below; //this is only needed once, so outside of loop.
		for (struct list *q = infocus, *p = new_item_1; q != NULL; ) {
			
			q->node->below->item->node->above->item = new_item_1;	//last_move to be updated
			q->node->below->item->node->above->line->start_board = &(p->node->board);
		
			q->node->below->item->node->above->line->start.x = p->node->board.rep.size.x + (BOARD_SIZE/2) * scale;
			q->node->below->item->node->above->line->start.y = p->node->board.rep.size.y + (BOARD_SIZE/2) * scale;
							
			if (q->node->below->next == NULL)
				break;
			q = q->node->below->next->item; 
		}
	}
	else new_item_1->node->below = NULL;
	
												//have to allocate new memory or else it would save it in the old location, that is, the below link of the new board. 
	if ((infocus->node->below = malloc(sizeof(struct spawn))) == NULL)	
		printf ("couldn't allocate memory for spawn struct");	
	infocus->node->below->item = new_item_1;		//line & first_move to be added
	infocus->node->below->next = NULL;
	
	
	
	
	
	struct list_lines *new_line = declare_new_line (infocus, new_item_1);
	
	
									//fitting the line into the branch
	new_item_1->node->above->line = new_line;							
	infocus->node->below->line = new_line;

									
									//fitting the item & line into the universal lists	
	fit_in_list (new_item_1, new_line);
	
	recur_shift (new_item_1->node->below);

	
	
	 
	 
	 
	 
	 
	 
	 
						// setting moves, board state
						
	new_item_1->node->board.mech = infocus->node->board.mech;					
						
	int start, counter;
	
	/*
	if (infocus->node->above != NULL)					//remove this, no need
		start = infocus->node->above->last_move->S_no + 1;
	else 
	*/
	start = 1;
	
	counter = start;
	
	int i, j;
	for (; counter <= moveNum; counter++)			//moveNum is the number of the last common move.
		for (i = 0; i < 19; i++) {
			for (j = 0; j < 19; j++)
				if (p->node->board.mech.state[i][j].S_no == counter) {
					place_stone(i, j, &(new_item_1->node->board), 0, blackStone, whiteStone, FALSE);
					break;
				}		
			if (j < 19)
				break;
		}
		
	infocus->node->below->first_move = malloc(sizeof(struct moves));
	new_item_1->node->above->last_move = malloc(sizeof(struct moves));
	
	*(new_item_1->node->above->last_move) = infocus->node->board.mech.state[i][j];
	/*
	new_item_1->node->above->last_move->S_no = moveNum;
	new_item_1->node->above->last_move->board_coords.y = j;
	new_item_1->node->above->last_move->board_coords.x = i;
	*/
	
	for (int i, j; counter <= new_item_1->node->board.mech.total_moves; counter++)
		for (i = 0; i < 19; i++) {
			for (j = 0; j < 19; j++)
				if (p->node->board.mech.state[i][j].S_no == counter) {
					if (counter == moveNum + 1)
						*(infocus->node->below->first_move) = new_item_1->node->board.mech.state[i][j];
					
					place_stone(i, j, &(new_item_1->node->board), counter - moveNum, blackStone, whiteStone, FALSE);
					break;
				}		
			if (j < 19)
				break;
		}
		
	counter--;
										
										
										
											//undoing moves in the parent board
											
	for ( ; counter > moveNum; counter--) {
		//for (i = 0; i < 19; i++) {
			//for (j = 0; j < 19; j++)
				//if (p->node->board.mech.state[i][j].S_no == counter) {
					undo_move(infocus, TRUE);
					printf ("undo\n");
					
			//if (j < 19)
				//break;
		}
	
	
											 //offshoot
	
	continue_play(infocus, blackStone, whiteStone);		//removes infocus
	place_stone(row, column, &(list->node->board), 1, blackStone, whiteStone, FALSE);
	
	
	
	
											//shifting, making space
	
	int divide = list->node->board.rep.size.x + BOARD_SIZE;
	int shift = (int)(BOARD_SIZE/2) + 25;
	int shift_left = -(int)(shift*scale);
	int shift_left_unscaled = -shift;
	int shift_right = (int)(shift*scale);
	int shift_right_unscaled = shift;
	
	shift_one (list, shift_right, 0, shift_right_unscaled, 0);
	
	for (struct list *p = list; p != NULL; p = p->next) {
		if (p->number == list->node->above->item->number || p->number == list->number)
			continue;
		if (p->node->board.rep.size.x < divide)
			shift_one (p, shift_left, 0, shift_left_unscaled, 0);
	}
	
	for (struct list *p = list; p != NULL; p = p->next) {
		if (p == infocus)	//not needed?
			continue;
		if (p->node->board.rep.size.x > divide)
			shift_one (p, shift_right, 0, shift_right_unscaled, 0);
	}	
	
}
	
	

	
void recur_shift (struct spawn *b) { 
	
	if (!b)
		return;
	
	if (b->next != NULL)
		recur_shift (b->next);
	
	int shift_y = (int)((BOARD_SIZE + SPACE_BW)*scale);
	int shift_y_unscaled = (int)(BOARD_SIZE + SPACE_BW);		
	
	shift_one (b->item, 0, shift_y, 0, shift_y_unscaled); 
	
	if (b->item->node->below != NULL) 
		recur_shift (b->item->node->below);
	
	
} 


				//might not need this. I should dissolve this function if it gets confusing.
void shift_one (struct list *p, int shift_x, int shift_y, int shift_x_unscaled, int shift_y_unscaled) {
	
	p->node->board.rep.center_off.x += shift_x_unscaled; 
	p->node->board.rep.center_off.y += shift_y_unscaled;
	p->node->board.rep.size.x += shift_x;
	p->node->board.rep.size.y += shift_y;
	
							//adjusting lines on shifting boards
	if (p->node->above != NULL) {					//the first board has no above.	
		p->node->above->line->end.x += shift_x;		//only one line from above (should be)
		p->node->above->line->end.y += shift_y;
	}	
												 
	for (struct spawn *bl = p->node->below; bl != NULL; bl = bl->next) {		
		bl->line->start.x += shift_x;		//multiple lines can emerge from a single board
		bl->line->start.y += shift_y;	
	}
	
	printf ("shift\n");
}
	
	
	
	
	
	
void fit_in_list (struct list *new_item, struct list_lines *new_line) {
	
	new_line->next = list_lines;				//in the list, starting of
	new_line->prev = NULL;
	if (list_lines != NULL)						//if this isn't the first line in the linked list
		list_lines->prev = new_line;
	list_lines = new_line;
									
									//fitting the board into the universal list
	new_item->next = list;
	new_item->prev = NULL;
	list->prev = new_item;
	list = new_item;
}	


struct list *declare_new_board (int *n_boards) {
	
	struct list   *new_item = malloc (sizeof(struct list));
	struct branch *new_node = malloc (sizeof(struct branch));
	
	if (new_node == NULL || new_item == NULL) {
		printf ("failed to add a board.");
		return NULL;
	}
	

	++(*n_boards);
	new_node->number = *n_boards;
	new_item->number = *n_boards;
	new_item->selection = NULL;
	
								
							//need this. For some reason, setting the value of center_off coords through the rep.size struct does not work.
	struct whole_coords newcoord = {.x = infocus->node->board.rep.size.x, .y = infocus->node->board.rep.size.y + (BOARD_SIZE + SPACE_BW)*scale};

	struct board new_board = {.mech.state = {{{0}}},		//the braces because it's an array of 
							.mech.turn = 0,						//structures. Still dk for sure.
							.mech.total_moves = 0,
					
							.rep.size = {newcoord.x, newcoord.y, BOARD_SIZE, BOARD_SIZE},		
							.rep.center_off.x = (newcoord.x)/scale - center_x_scaled, 
							.rep.center_off.y = (newcoord.y)/scale - center_y_scaled,
							.rep.snap = NULL,
						};
	
	new_board.rep.snap = SDL_CreateTexture (renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, BOARD_SIZE, BOARD_SIZE);
	SDL_SetTextureBlendMode(new_board.rep.snap, SDL_BLENDMODE_BLEND);
	SDL_SetRenderTarget (renderer, new_board.rep.snap);
	SDL_SetRenderDrawColor (renderer, 0, 0, 0, 0);
	SDL_RenderClear(renderer);
	SDL_SetRenderTarget (renderer, NULL);
	
	new_node->board = new_board;
	new_item->node = new_node;
	
	return new_item;
}


struct list_lines *declare_new_line (struct list *start_item, struct list *end_item) {

	struct list_lines *new_line = malloc(sizeof(struct list_lines));
	
	new_line->start_board = &(start_item->node->board);		
	new_line->end_board   = &(end_item->node->board);
	
	new_line->start.x = new_line->start_board->rep.size.x + (BOARD_SIZE/2) * scale;
	new_line->start.y = new_line->start_board->rep.size.y + (BOARD_SIZE/2) * scale;
	new_line->end.x   = new_line->end_board->rep.size.x   + (BOARD_SIZE/2) * scale;
	new_line->end.y   = new_line->end_board->rep.size.y   + (BOARD_SIZE/2) * scale;
	
	return new_line;
}

		//this is doing three types of placing stones. I am using conditions to make it act one
		//of the three ways. Is it complicated? sort of. Is it worth it? not sure.

		
					//opt in boards to mass select, delete, shift
struct opted *opt_in (struct list *p, struct opted **optList) {
	
	int count = 0, tick1 = 0;
	
	for (;;) {								//adding boards to the todelete list.
		
		 tick1 = 0;
		struct opted *new_item = malloc(sizeof(struct opted));
		
		new_item->list = p;
		new_item->next = *optList;
		if (*optList != NULL)
			(*optList)->prev = new_item;
		new_item->prev = NULL;
		*optList = new_item;
		
		if (p->node->below == NULL)
			break;
		printf ("in here jdkfj\n");
										//moving horizontal, towards the right
		for(struct spawn *q = p->node->below; 
			q->next != NULL; q = q->next) 
				opt_in(q->next->item, optList);	
				
		
						
										//moving vertical, downwards
		p = p->node->below->item;
		
		printf ("count: %d, tick1: %d\n", count, tick1);
	}
	
	return *optList;
}


			//this is doing too much. 3 types of placing stones. simple it down maybe.
			
void place_stone (int x, int y, struct board *board, int number, SDL_Texture *stone_black, SDL_Texture *stone_white, bool ghost_stones) {						
	
	
	
	SDL_Rect stoneSize = { ((x*SQUARE_SIZE + BORDER) - 15), ((y*SQUARE_SIZE + BORDER) - 15), STONE_SIZE, STONE_SIZE};	
	SDL_SetRenderTarget (renderer, board->rep.snap);
	
	bool condition;
	SDL_Color color;
	
	
	if (number) {
		if (ghost_stones)
			condition = board->mech.turn;
		else condition = !board->mech.turn;
	}
	else condition = !(board->mech.state[x][y].colour - 1);
	
	if (condition) {
		SDL_RenderCopy(renderer, stone_black, NULL, &stoneSize);
		color.r = 255; color.g = 255; color.b = 255;
	}
	else  {
		SDL_RenderCopy(renderer, stone_white, NULL, &stoneSize);
		color.r = 0; color.g = 0; color.b = 0;
	}
		


	if (number) {
		
		char *buffer = malloc(5);
		buffer[5] = '\0';
		sprintf (buffer, "%d", number);
		SDL_Surface *stoneNo_surface = TTF_RenderText_Solid(font, buffer, color);
		SDL_Texture *stoneNo_texture = SDL_CreateTextureFromSurface(renderer, stoneNo_surface);
		free(buffer);	
	
		int texW, texH;
		int x_offset;
		SDL_QueryTexture(stoneNo_texture, NULL, NULL, &texW, &texH);
		if (number < 10)
			x_offset = 6;
		else x_offset = 11;
		SDL_Rect stoneNo_rect = { ((x*SQUARE_SIZE + BORDER) - x_offset), ((y*SQUARE_SIZE + BORDER) - 9), texW, texH };
		
		SDL_RenderCopy(renderer, stoneNo_texture, NULL, &stoneNo_rect);
		
		SDL_FreeSurface(stoneNo_surface);
		SDL_DestroyTexture(stoneNo_texture);
	}
	
	SDL_SetRenderTarget (renderer, NULL);
		
	if(!number)			//if called by continue_play
		return;
	
							//recording changes in the board state.
	
	if(ghost_stones == FALSE)													
		board->mech.state[x][y].colour = board->mech.turn + 1;  // + 1 because the colour enum has "empty" as the first element.  
		
	(board->mech.turn)++; 
	board->mech.turn %= 2;
	
}



bool isin_box (SDL_Rect rect, SDL_MouseButtonEvent button) {
	
	if (!(rect.x < button.x  &&  button.x < (rect.x + rect.w)))
		return FALSE;
	if (!(rect.y < button.y  &&  button.y < (rect.y + rect.h)))
		return FALSE;
		
	return TRUE;
}
		
		
		
	
void display_text (struct message text) {
	
	SDL_Surface *text_sur = TTF_RenderText_Blended_Wrapped(font, text.str, text.txt_color, 200);
	SDL_Texture *text_texture = SDL_CreateTextureFromSurface(renderer, text_sur);

	int texW = 0, texH = 0;										//initializing a text_rect
	SDL_QueryTexture(text_texture, NULL, NULL, &texW, &texH);
	SDL_Rect text_rect = { text.coord.x, text.coord.y, texW, texH };
	
	SDL_SetRenderDrawColor(renderer, text.bg_color.r, text.bg_color.g, text.bg_color.b, text.bg_color.a);	
	SDL_RenderFillRect(renderer, &text_rect);
	SDL_RenderCopy(renderer, text_texture, NULL, &text_rect);
	
	SDL_RenderPresent(renderer);
	SDL_FreeSurface(text_sur);
	SDL_DestroyTexture(text_texture);
}







	
	//moved the texture inside each list struct or board struct. The textures copy to the renderer using the dimensions in the board struct.

void render (struct list *p) {
	
	
														//render lines
	for (struct list_lines *l = list_lines; l != NULL; l = l->next)
		thickLineRGBA (renderer, l->start.x, l->start.y, l->end.x, l->end.y, 5, 150, 100, 200, 255); 
	
	
														//render opted indicators
	struct opted *q = sel;
	
	select_indicator.w = (BOARD_SIZE + 80) * scale;
	select_indicator.h = (BOARD_SIZE + 80) * scale;
	
	for (; q != NULL; q = q->next) 	{	
		 
		//printf ("%d, %d", q->list->node->board.rep.x, q->list->node->board.rep.y);
		select_indicator.x = q->list->node->board.rep.size.x - (40 * scale);
		select_indicator.y = q->list->node->board.rep.size.y - (40 * scale);
		//printf ("rendering: %d, %d\n", p->board.rep.x, p->board.rep.y);  check
		SDL_RenderCopy (renderer, selTex, NULL, &select_indicator);
	}
	
		
											//render boards
	for (; p != NULL; p = p->next) {

		p->node->board.rep.size.w = BOARD_SIZE*scale;		// can the scaling be moved into the zoom function?
		p->node->board.rep.size.h = BOARD_SIZE*scale;
		
		
		SDL_RenderCopy (renderer, bg_board, NULL, &(p->node->board.rep.size));
		SDL_RenderCopy (renderer, p->node->board.rep.snap, NULL, &(p->node->board.rep.size));
		
		
		q = sel;	
		SDL_DestroyTexture (texture);
	}
	
	
	
	//ticker();					//crashing?
	if (txt_display) {
		display_text(text);
		txt_display = FALSE;
	}
	
	SDL_RenderPresent(renderer);
	

}










void testing (void) {
	
	printf ("list: ");
	for (struct list *p = list; p != NULL; p = p->next)
			printf("%d ", p->number);
		printf ("\n");
	
	printf ("selection: ");	
	for (struct opted *s = sel; s != NULL; s = s->next)
		printf("%d ", s->list->number);
	printf ("\n");
}




void inspect_board (struct list q) {
	
	int i, j;
	for (i = 0; i < 19; i++) {
		for (j = 0; j < 19; j++)
			printf ("%d ", q.node->board.mech.state[j][i].colour);
		printf ("\n");
	}
	
	
	printf ("\n\n||||||||||BOARD STATS||||||||||\n\n");
	
	
	printf ("\n\n_____ABOVE_____\n\n");
	struct list r = q;
	
	if (q.node->above == NULL)
		printf("----------\n");
	else {		
		printf (	
			"\t#%d\n"
			"total moves: %10d\n"
			"last_move:\t",
			 r.node->above->item->number,
			 r.node->above->item->node->board.mech.total_moves);
			 (r.node->above->last_move->colour == 1)?printf("black\n"):printf("white\n");	
			 printf ("turn:\t\t");
			 r.node->above->item->node->board.mech.turn?printf("white\n"):printf("black\n");
	}
	
	
	

	
	printf ("\n\n_____CURRENT_____\n\n");
	printf (
		"\t#%d\n"
		"total moves: %10d\n"
		"turn:\t\t",
		q.number,
		q.node->board.mech.total_moves);
		q.node->board.mech.turn?printf("white\n"):printf("black\n");
	
	
	
		
	
	
	
	printf ("\n\n_____BELOW_____\n\n");
	
	if (q.node->below == NULL)
		printf("----------\n");
	else {
		struct spawn *r = q.node->below;
		for (; ;) { 
			
			printf (	
				"\t#%d\n"
				"total moves: %10d\n"
				"first_move:\t",
				 r->item->number,
				 r->item->node->board.mech.total_moves);
				 (r->first_move->colour == 1)?printf("black\n"):printf("white\n");	
				 printf ("turn:\t\t");
				 r->item->node->board.mech.turn?printf("white\n"):printf("black\n");
				
			printf ("\n"); 
			if (r->next == NULL)
				break;
				
			r = r->next;
		}
	}
	
	printf ("\n\n");
}
	
	


void ticker (void) {
	
	int size = snprintf(NULL, 0, "scale: %4lf", scale);
	char *buffer = malloc(24 + size + 1);
	//buffer[19] = '\0';
    //sprintf (buffer, "%5d, %4d, %4d", z++, first->board.rep.x, first->board.rep.y);		//scale: %4.2lf"
	//sprintf (buffer, "%5d, scale: %4.2lf", z++, scale);
	sprintf (buffer, "%3d, x: %4d, y: %4d,  scale: %4.2lf", ++frames_rendered, list->node->board.rep.size.x, list->node->board.rep.size.y, scale); 
   
	SDL_Color color = { 255, 255, 255 };
	SDL_Surface *ticker_surface = TTF_RenderText_Solid(font, buffer, color);
	SDL_Texture *ticker_texture = SDL_CreateTextureFromSurface(renderer, ticker_surface);
	free(buffer);	

	int texW = 0, texH = 0;
	
	SDL_QueryTexture(ticker_texture, NULL, NULL, &texW, &texH);
	SDL_Rect ticker_rect = { (WINDOW_WIDTH-550), 0, texW, texH };
	
	//SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);	
	SDL_RenderFillRect(renderer, &ticker_rect);
	SDL_RenderCopy(renderer, ticker_texture, NULL, &ticker_rect);
	
	SDL_FreeSurface(ticker_surface);
	SDL_DestroyTexture(ticker_texture);
}





void load_setup (void) {
	
	
	bg_board = IMG_LoadTexture (renderer, "media/thick-lines-scaled-down.png");
	blackStone = IMG_LoadTexture (renderer,"media/black-stone.png");
	whiteStone = IMG_LoadTexture (renderer,"media/white-stone.png");
	ghost_blackStone = IMG_LoadTexture (renderer,"media/ghost-black-stone.png");
	ghost_whiteStone = IMG_LoadTexture (renderer,"media/ghost-white-stone.png");
	
	
	/*	//what use are these?
	SDL_SetTextureBlendMode(blackStone, SDL_BLENDMODE_BLEND);
	SDL_SetTextureBlendMode(whiteStone, SDL_BLENDMODE_BLEND);
	*/
	
							//opted Indicator: declaring and colouring
	selTex = SDL_CreateTexture (renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, BOARD_SIZE, BOARD_SIZE);
	SDL_SetRenderTarget (renderer, selTex);
	SDL_SetRenderDrawColor (renderer, 250, 220, 240, 255);
	SDL_RenderFillRect (renderer, NULL);
	SDL_SetRenderTarget (renderer, NULL);
	
	branchTex = SDL_CreateTexture (renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, BOARD_SIZE, BOARD_SIZE);
	SDL_SetRenderTarget (renderer, branchTex);
	SDL_SetRenderDrawColor (renderer, 250, 240, 200, 255);
	SDL_RenderFillRect (renderer, NULL);
	SDL_SetRenderTarget (renderer, NULL);
	
	
	
							//First board:   malloc for it and declare it as a node? or keep  it like this?
							//Not a linked list (see below)
	struct board board_1 = {.mech.state = {{{0}}},		//the braces because it's an array of 
						.mech.turn = 0,						//structures. Still dk for sure.
						.mech.total_moves = 0,
					
						.rep.center_off.x = 10 - CENTER_X, 
						.rep.center_off.y = 10 - CENTER_Y,
						.rep.size = {10, 10, BOARD_SIZE, BOARD_SIZE},
						.rep.snap = NULL,
					};
												//the board state texture on a transparent bg
	board_1.rep.snap = SDL_CreateTexture (renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, BOARD_SIZE, BOARD_SIZE);
	SDL_SetTextureBlendMode(board_1.rep.snap, SDL_BLENDMODE_BLEND);
	SDL_SetRenderTarget (renderer, board_1.rep.snap);
	SDL_SetRenderDrawColor (renderer, 0, 0, 0, 0);
	SDL_RenderClear(renderer);
	SDL_SetRenderTarget (renderer, NULL);
	
	
	branch = malloc (sizeof(struct branch));		//why is this needed?
	branch->board = board_1;					//because this is a linked list while board above
											//is not one. This is the same as allocating mem
											//for a new_node and pointing first to it.
	
	branch->above = NULL;
	branch->below = NULL;
	
	branch->number = n_boards;
	
	//first->next = NULL;
	
	list = malloc (sizeof(struct list));
	list->node = branch;
	list->number = n_boards;
	list->selection = NULL;
	list->prev = NULL;
	list->next = NULL;
	
	
	
}




