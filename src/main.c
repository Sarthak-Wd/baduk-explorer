#include <stdio.h>
#include "common.h"
#include "init.h"
#include "camera.h"
#include "helper_functions.h"
#include "routine_functions.h"



bool game_is_running = TRUE;

SDL_Event event;
SDL_MouseButtonEvent pan_start;

int n_boards = 1;

struct list *infocus = NULL;

int frames_rendered = 0;







struct branch *branch 	= NULL; 
struct list *list 	= NULL; 
struct list_lines *list_lines	= NULL;

struct message text;
struct opted *sel 	= NULL;	

playing_parts parts;
scaling scale = {1.0, .center.x = CENTER_X, .center.y = CENTER_Y};
	





//Function Prototypes


void load_setup (void);
bool process_input(void);
void render (struct list *p);



void continue_play (SDL_Texture *blackStone, SDL_Texture *whiteStone);
void branch_window (struct list *p);
void off_shoot (struct list *p, int row, int column, int moveNum, int *n_boards);



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
											printf ("adding board_1\n");
											continue_play(blackStone, whiteStone);
											break;
										}
										if (event.button.button == SDL_BUTTON_LEFT) {
											render(list); 		//to clear any warning messages displayed on last input. //might not need this since will need to render to center the board anyway.
											struct list *p = list;
											for ( ; p != NULL; p = p->next)
												if (isin_box(p->node->board.rep.size, event.button)) {
													
													printf ("\nnumber: %d, total moves: %d\n", p->number, p->node->board.mech.total_moves); 
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
													delete_board(p, &n_boards, &sel, &infocus, &list, &list_lines);
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
						select_board(p, &sel);
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
						shift_elements(sel, &event, &pan_start, scale);
						break;
					}
		
				if (q == NULL) 
				 pan_coords(list, list_lines, &event, &pan_start, scale);
			}
					
			else if (event.type == SDL_MOUSEBUTTONUP) { 
				
				if (event.button.x > (WINDOW_WIDTH - 50)  && event.button.y < 50)
					add_board(&n_boards, &infocus, scale, &list, &list_lines);		//improve, make a button, w/ a rect
					
				else  if (event.button.button == SDL_BUTTON_LEFT) {
						struct list *p = list;
						for ( ; p != NULL; p = p->next)
							if (isin_box(p->node->board.rep.size, event.button)) {
								parts.item = p;
								play_move (event, &parts, scale, &text, &infocus);
								break;
							}	
				}
				else { struct list *p = list;
						for ( ; p != NULL; p = p->next)
							if (isin_box(p->node->board.rep.size, event.button)) {
								undo_move(p, &infocus, &text, FALSE);
								break;
							}
						}	
							
										
			}
			return TRUE;
				
				
		case SDL_MOUSEWHEEL:
			
			zoom_coords (list, list_lines, event, &scale);			
			return TRUE;
	}
	
	return FALSE;
}






//don't need the list parameter? it is provided by add_board anyway?
void continue_play (SDL_Texture *blackStone, SDL_Texture *whiteStone) {
	
	struct list *p;
	if ((p = add_board(&n_boards, &infocus, scale, &list, &list_lines)) == NULL)
		return;							  
		
		
											//copying the state
		
	p->node->board.mech = p->node->above->item->node->board.mech;		
													
		
											//placing stones
	for (int column = 0; column < 19 ; column++) 
		for (int row = 0; row < 19; row++)  {
			if (p->node->board.mech.state[column][row].colour == 1) 
				place_stone (column, row, &(p->node->board), blackStone);
			else if (p->node->board.mech.state[column][row].colour == 2)
				place_stone (column, row, &(p->node->board), whiteStone);
		}
						



	
									//last move of the board above, first move of the board below							
									
	struct list *q = p->node->above->item;		//board->below always points to the last declared spawn. So, no need for a loop.
	
	p->node->above->last_move	 = malloc(sizeof(struct moves));
	q->node->below->first_move   = malloc(sizeof(struct moves));
	q->node->below->first_move->S_no = (q->node->board.mech.total_moves + 1);  //I can do away with this. The first_move is copied entirely when it is played.
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
	
	*(p->node->above->last_move) = q->node->board.mech.state[i][j];		//can just have it as a pointer and won't have to malloc?
}								
	





void branch_window (struct list *p) {
	
	
	parts.item = p;
	
	int current_move = p->node->board.mech.total_moves;
	infocus = p;
	
	//printf ("turn while entering branch mode: %d\n", infocus->node->board.mech.turn);
	
										//preserving the actual board state.
						//why not just preserve the entire board struct?				
	SDL_Texture *preserve = SDL_CreateTexture (renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, BOARD_SIZE, BOARD_SIZE);
	SDL_SetRenderTarget (renderer, preserve);
	SDL_RenderCopy(renderer, p->node->board.rep.snap, NULL, NULL);
	int copy_turn = p->node->board.mech.turn;    //copy made only for branch mode.
	
	
	
											
											//placing the branching indicator.
	{
		SDL_Rect select_indicator = {	p->node->board.rep.size.x - (40 * scale.amount),
										p->node->board.rep.size.y - (40 * scale.amount),
										(BOARD_SIZE + 80) * scale.amount,
										(BOARD_SIZE + 80) * scale.amount	
									};
											
		
		SDL_SetRenderTarget (renderer, NULL);
		SDL_RenderCopy (renderer, branchTex, NULL, &select_indicator);
		render(list);
	}
	
	
	
	
									//input loop : 			//going back and forth in the branching window
	while (1) {												//branching, if new move
		
		SDL_SetRenderTarget (renderer, p->node->board.rep.snap);
	
	    
		while (!SDL_PollEvent(&event))
			;	

		
		
		
		
		if (event.type == SDL_MOUSEBUTTONDOWN) {
				
			while (!SDL_PollEvent(&event))
				;
		
			
		
			if (event.type == SDL_MOUSEMOTION)
				while (1) {
					SDL_PollEvent(&event);
					if (event.type == SDL_MOUSEBUTTONUP && event.button.button == SDL_BUTTON_LEFT)
						break;	//there was continue here. What was that supposed to do? hanged the program.
				}
			
			else if (event.type == SDL_MOUSEBUTTONUP && event.button.button == SDL_BUTTON_LEFT) {
				
				if (!isin_box(infocus->node->board.rep.size, event.button))
					continue;
				
				double x, y;
				int column, row;
				
				x 	= 	(event.button.x - (p->node->board.rep.size.x + BORDER*scale.amount)) / (SQUARE_SIZE*scale.amount); 
				y 	= 	(event.button.y - (p->node->board.rep.size.y + BORDER*scale.amount)) / (SQUARE_SIZE*scale.amount);
			
													//rounding:  row and column are swapped because that is how an array be.
				if ((x - (int)x) >= 0.5)
					 column = (int)x + 1;
				else column = (int)x;
				
				if ((y - (int)y) >= 0.5)
					 row = (int)y + 1;
				else row = (int)y;
				
				
				if (p->node->board.mech.state[column][row].S_no <= current_move)
					if (p->node->board.mech.state[column][row].S_no != 0)
						continue;
					
				
				if (p->node->board.mech.state[column][row].S_no != current_move + 1) {
					printf ("turn right b4 entering off_shoot: %d\n", infocus->node->board.mech.turn);
					off_shoot(p, column, row, current_move, &n_boards);
					return;
				}
				else {
					current_move++;
					
					if (p->node->above != NULL) 			
						parts.number = current_move - p->node->above->last_move->S_no;		
					else parts.number = current_move;
					
					if (copy_turn) {
						place_stone(column, row, &(p->node->board), parts.whiteStone);
						parts.font_color.r = 0; parts.font_color.g = 0; parts.font_color.b = 0;
					}
					else { 
						place_stone(column, row, &(p->node->board), parts.blackStone);
						parts.font_color.r = 255; parts.font_color.g = 255; parts.font_color.b = 255;
					}
					put_number(column, row, &parts);
					
					copy_turn++;
					copy_turn %= 2;
					
					SDL_SetRenderTarget (renderer, NULL);	//this is needed of course so that render() renders the entire screen
					render(list);
					
					printf ("turn after action in branch mode: %d\n", p->node->board.mech.turn);
					
				}
			}
		}
			
			
			
		if (event.key.keysym.sym == SDLK_LEFT && event.type == SDL_KEYUP) {
			
			if (p->node->above != NULL) { 				
				if (current_move == p->node->above->last_move->S_no)		//disallowing undoing the moves of the parent board on the board below.
					continue;
				parts.number = current_move - p->node->above->last_move->S_no;	
			}
			else {
				if (current_move == 0) 
					continue;
				parts.number = current_move;
			}
	
			  
			int column, row;
			for (column = 0; column < 19; column++) {
				for (row = 0; row < 19; row++) {
					if (p->node->board.mech.state[column][row].S_no == current_move) 	
						break;
				}
				if (row < 19)
					break;
			}
			
			current_move--;	
			
			
			if (copy_turn) {
				place_stone(column, row, &(p->node->board), ghost_blackStone);
				parts.font_color.r = 255; parts.font_color.g = 255; parts.font_color.b = 255;
			}
			else { 
				place_stone(column, row, &(p->node->board), ghost_whiteStone);
				parts.font_color.r = 0; parts.font_color.g = 0; parts.font_color.b = 0;
			}
			put_number(column, row, &parts);
			
			copy_turn++;
			copy_turn %= 2;
			
			SDL_SetRenderTarget (renderer, NULL);
			render(list);
			
			printf ("turn after action in branch mode: %d\n", p->node->board.mech.turn);
		} 
		
		
		
		else if (event.key.keysym.sym == SDLK_RIGHT && event.type == SDL_KEYUP) {
			
			if (current_move == p->node->board.mech.total_moves)
				continue;
				
			current_move++;			//this has to be b4 the changes, unlike when going backwards.
			
			if (p->node->above != NULL) 		
				parts.number = current_move - p->node->above->last_move->S_no;		
			else parts.number = current_move;
			
			int column, row;
			for (column = 0; column < 19; column++) {
				for (row = 0; row < 19; row++) {
					if (p->node->board.mech.state[column][row].S_no == current_move) 	
						break;
				}
				if (row < 19)
					break;
			}
			
			
			if (copy_turn) {
				place_stone(column, row, &(p->node->board), whiteStone);
				parts.font_color.r = 0; parts.font_color.g = 0; parts.font_color.b = 0;
			}
			else { 
				place_stone(column, row, &(p->node->board), blackStone);
				parts.font_color.r = 255; parts.font_color.g = 255; parts.font_color.b = 255;
			}
			put_number(column, row, &parts);
			
			copy_turn++;
			copy_turn %= 2;
						
			SDL_SetRenderTarget (renderer, NULL);	//this is needed of course so that render() renders the entire screen
			render(list);
			
			printf ("turn after action in branch mode: %d\n", p->node->board.mech.turn);
			
		}
			
		
		else if (event.key.keysym.sym == SDLK_q) {
			SDL_SetRenderTarget (renderer, p->node->board.rep.snap);
			SDL_RenderCopy(renderer, preserve, NULL, NULL);
			SDL_SetRenderTarget (renderer, NULL);
			//p->node->board.mech.turn = preserve_turn;
			break;
		}
		
		//printf ("turn after action in branch mode: %d\n", p->node->board.mech.turn);
	}
}
	




void off_shoot (struct list *p, int column, int row, int moveNum, int *n_boards) {
	
	
	struct list *new_item_1 = declare_new_board(n_boards, infocus, scale);	
	
	printf ("turn right after entering off_shoot: %d\n", infocus->node->board.mech.turn);
						//putting the board in the branch
	
	if ((new_item_1->node->above = malloc(sizeof(struct parent))) == NULL)	
		printf ("couldn't allocate memory for spawn struct");
	new_item_1->node->above->item = infocus;
	
												//adjusting the new item with all the links below.
	if (infocus->node->below != NULL) {
		
		new_item_1->node->below = infocus->node->below; //this is only needed once, so outside of loop.
		
		for (struct spawn *b = infocus->node->below; b != NULL; ) {	
			b->item->node->above->item = new_item_1;			//last_move to be updated
			b->item->node->above->line->start_board = &(new_item_1->node->board);
		
			b->item->node->above->line->start.x = new_item_1->node->board.rep.size.x + (BOARD_SIZE/2) * scale.amount;
			b->item->node->above->line->start.y = new_item_1->node->board.rep.size.y + (BOARD_SIZE/2) * scale.amount;
							
			if (b->next == NULL)
				break;
			b = b->next; 
		}
	}
	else new_item_1->node->below = NULL;
	
												//have to allocate new memory or else it would save it in the old location, that is, the below link of the new board. 
	if ((infocus->node->below = malloc(sizeof(struct spawn))) == NULL)	
		printf ("couldn't allocate memory for spawn struct");	
	infocus->node->below->item = new_item_1;		//line & first_move to be added
	infocus->node->below->next = NULL;
	
	
	
	
	
	struct list_lines *new_line = declare_new_line (infocus, new_item_1, scale);
								//fitting the line into the branch
	new_item_1->node->above->line = new_line;							
	infocus->node->below->line = new_line;
								//fitting the item & line into the universal lists	
	fit_in_list (new_item_1, new_line, &list, &list_lines);
	recur_shift (new_item_1->node->below, scale);

	
	 
						// setting moves, board state
						
	new_item_1->node->board.mech = infocus->node->board.mech;					
			
	printf ("\nturn while assigning the new board mech: %d\n\n", infocus->node->board.mech.turn);		
						
	int start = 1, counter;		//start seems to serve no purpose, I can just remove it and set counter to 1 directly.
	
	counter = start;
	
	printf ("\n-----cloning the first board-----\n");
	
	int x, y;
	for (; counter <= moveNum; counter++)			//moveNum is the number of the last common move.
		for (x = 0; x < 19; x++) {
			for (y = 0; y < 19; y++)
				if (new_item_1->node->board.mech.state[x][y].S_no == counter) {
					if (new_item_1->node->board.mech.state[x][y].colour == 1) 
						place_stone (x, y, &(new_item_1->node->board), blackStone);
					else if (new_item_1->node->board.mech.state[x][y].colour == 2)
						place_stone (x, y, &(new_item_1->node->board), whiteStone);
					break;
				}		
			if (y < 19)
				break;
		}
	
	printf ("Common moves cloned\n");	
	

	
	for (int column, row; counter <= new_item_1->node->board.mech.total_moves; counter++)			//moveNum is the number of the last common move.
		for (column = 0; column < 19; column++) { 
			for (row = 0; row < 19; row++)	
				if (new_item_1->node->board.mech.state[column][row].S_no == counter) 	{
					if (new_item_1->node->board.mech.state[column][row].colour == 1) {
						place_stone (column, row, &(new_item_1->node->board), blackStone);
						parts.font_color.r = 255; parts.font_color.g = 255; parts.font_color.b = 255; 
					}
					else if (new_item_1->node->board.mech.state[column][row].colour == 2) {
						place_stone (column, row, &(new_item_1->node->board), whiteStone);
						parts.font_color.r = 0; parts.font_color.g = 0; parts.font_color.b = 0;
					}
					parts.number = counter - moveNum;
					parts.item = new_item_1;
					put_number(column, row, &parts);
					break;
				}		
			if (row < 19)
				break;
		}
	
	infocus->node->below->first_move = malloc(sizeof(struct moves));
	new_item_1->node->above->last_move = malloc(sizeof(struct moves));
	
	
									//first and last moves, b/w infocus and new_item_1
	for (int x = 0; x < 19; x++) 
		for (int y = 0; y < 19; y++)	{
			if (new_item_1->node->board.mech.state[x][y].S_no == moveNum) {
				printf ("last move : %d\n", infocus->node->board.mech.state[x][y].S_no); 
				*(new_item_1->node->above->last_move) = infocus->node->board.mech.state[x][y]; }
			else if (new_item_1->node->board.mech.state[x][y].S_no == moveNum + 1)
				*(infocus->node->below->first_move) = new_item_1->node->board.mech.state[x][y];
		}
	
	counter--;	
										
	printf ("-----done cloning the first board-----\n\n");									
										
											//undoing moves in the parent board
											
	for ( ; counter > moveNum; counter--) {
		//for (i = 0; i < 19; i++) {
			//for (j = 0; j < 19; j++)
				//if (p->node->board.mech.state[i][j].S_no == counter) {
					undo_move(infocus, &infocus, &text, TRUE);
					//printf ("undo\n");
					
			//if (j < 19)
				//break;
		}
	
	
											 //offshoot
	
	continue_play(blackStone, whiteStone);		//removes infocus
	
	if (list->node->board.mech.turn) {
		place_stone(column, row, &(list->node->board), whiteStone);
		parts.font_color.r = 0; parts.font_color.g = 0; parts.font_color.b = 0;
	}
	else { 
		place_stone(column, row, &(list->node->board), blackStone);
		parts.font_color.r = 255; parts.font_color.g = 255; parts.font_color.b = 255;
	}
	parts.item = list;
	parts.number = 1;
	put_number(column, row, &parts);
	
	list->node->board.mech.state[column][row].S_no = ++(list->node->board.mech.total_moves);
									
	list->node->board.mech.state[column][row].colour = list->node->board.mech.turn + 1;  // + 1 because the colour enum has "empty" as the first element.  
	(list->node->board.mech.turn)++; 
	list->node->board.mech.turn %= 2;
						
	*(list->node->above->item->node->below->first_move) = list->node->board.mech.state[column][row];				
				
				
											
									//shifting, making space
	
	int divide = list->node->board.rep.size.x + BOARD_SIZE;
	int shift = (int)(BOARD_SIZE/2) + 25;
	int shift_left = -(int)(shift*scale.amount);
	int shift_right = (int)(shift*scale.amount);
	
	
	shift_one (list, shift_right, 0, scale.amount);
	
	for (struct list *p = list; p != NULL; p = p->next) {
		if (p->number == list->node->above->item->number || p->number == list->number)
			continue;
		if (p->node->board.rep.size.x < divide)
			shift_one (p, shift_left, 0, scale.amount);
	}
	
	for (struct list *p = list; p != NULL; p = p->next) {
		if (p == infocus)	//not needed?
			continue;
		if (p->node->board.rep.size.x > divide)
			shift_one (p, shift_right, 0, scale.amount);
	}	
	
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
	
	
														//render select indicators
	struct opted *q = sel;
	
	SDL_Rect select_indicator = {	0, 0,
									(BOARD_SIZE + 80) * scale.amount,
									(BOARD_SIZE + 80) * scale.amount	
								};
	
	for (; q != NULL; q = q->next) 	{	
		 
		select_indicator.x = q->list->node->board.rep.size.x - (40 * scale.amount);
		select_indicator.y = q->list->node->board.rep.size.y - (40 * scale.amount);
		SDL_RenderCopy (renderer, selTex, NULL, &select_indicator);
	}
	
		
											//render boards
	for (; p != NULL; p = p->next) {

		p->node->board.rep.size.w = BOARD_SIZE*scale.amount;		// can the scaling be moved into the zoom function?
		p->node->board.rep.size.h = BOARD_SIZE*scale.amount;
		
		
		SDL_RenderCopy (renderer, bg_board, NULL, &(p->node->board.rep.size));
		SDL_RenderCopy (renderer, p->node->board.rep.snap, NULL, &(p->node->board.rep.size));
		
		
		q = sel;	
		SDL_DestroyTexture (texture);
	}
	
	
	
	//ticker();					//crashing?
	if (text.to_display) {
		display_text(text);
		text.to_display = FALSE;
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
	
	int size = snprintf(NULL, 0, "scale: %4lf", scale.amount);
	char *buffer = malloc(24 + size + 1);
	//buffer[19] = '\0';
    //sprintf (buffer, "%5d, %4d, %4d", z++, first->board.rep.x, first->board.rep.y);		//scale: %4.2lf"
	//sprintf (buffer, "%5d, scale: %4.2lf", z++, scale);
	sprintf (buffer, "%3d, x: %4d, y: %4d,  scale: %4.2lf", ++frames_rendered, list->node->board.rep.size.x, list->node->board.rep.size.y, scale.amount); 
   
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
	
	parts.blackStone = blackStone; 
	parts.whiteStone = whiteStone;
	parts.ghost_b = ghost_blackStone; 
	parts.ghost_w = ghost_whiteStone; 
	parts.font = font;	
	
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




