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

struct board *infocus = NULL;

int frames_rendered = 0;







//struct branch *branch 	= NULL; 
struct board *list 	= NULL; 
struct list_lines *list_lines	= NULL;

struct message text;
struct opted *sel 	= NULL;	

playing_parts parts;
scaling scale = {1.0, .center.x = CENTER_X, .center.y = CENTER_Y};
	





//Function Prototypes


void load_setup (void);
bool process_input(void);
void render (struct board *p);



void continue_play (SDL_Texture *blackStone, SDL_Texture *whiteStone);
void branch_window (struct board *p);
void off_shoot (struct board *p, int row, int column, int moveNum, int *n_boards);



void display_text (struct message text);
void ticker (void);
void testing (void);
void inspect_board (struct board p);




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
											struct board *p = list;
											for ( ; p != NULL; p = p->next)
												if (isin_box(p->rep.size, event.button)) {
													
													printf ("\nnumber: %d, total moves: %d\n", p->number, p->mech.total_moves); 
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
											struct board *p = list;
											for ( ; p != NULL; p = p->next)
												if (isin_box(p->rep.size, event.button)) { 
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
											struct board *p = list;
											for ( ; p != NULL; p = p->next)
												if (isin_box(p->rep.size, event.button)) { 
													inspect_board(*p);
													break;
												}
											break;
										}
										
										if (event.type == SDL_KEYUP)
											if (event.key.keysym.sym == SDLK_d)
												break;
									} 
									break;
				case SDLK_t: 		testing(); break;
            }
            return TRUE;
		

		case SDL_MOUSEBUTTONDOWN:
			
			if (event.button.button == SDL_BUTTON_MIDDLE) {
				for (struct board *p = list; p != NULL; p = p->next)
					if (isin_box(p->rep.size, event.button)) { 
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
					if (isin_box (q->board->rep.size, pan_start)) {
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
						struct board *p = list;
						for ( ; p != NULL; p = p->next)
							if (isin_box(p->rep.size, event.button)) {
								parts.board = p;
								play_move (event, &parts, scale, &text, &infocus);
								break;
							}	
				}
				else { struct board *p = list;
						for ( ; p != NULL; p = p->next)
							if (isin_box(p->rep.size, event.button)) {
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







void continue_play (SDL_Texture *blackStone, SDL_Texture *whiteStone) {
	
	
	
	struct board *p;
	if ((p = add_board(&n_boards, &infocus, scale, &list, &list_lines)) == NULL)
		return;							  
		
		
								
	p->mech = p->above_board->mech;		//copying the config
													
		
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
	





void branch_window (struct board *p) {
	
	
	parts.board = p;
	
	int current_move = p->mech.total_moves;
	infocus = p;
	
	//printf ("turn while entering branch mode: %d\n", infocus->node->board.mech.turn);
	
										//preserving the actual board state.
						//why not just preserve the entire board struct?				
	SDL_Texture *preserve = SDL_CreateTexture (renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, BOARD_SIZE, BOARD_SIZE);
	SDL_SetRenderTarget (renderer, preserve);
	SDL_RenderCopy(renderer, p->rep.snap, NULL, NULL);
	int copy_turn = p->mech.turn;    //copy made only for branch mode.
	
	
	
											
											//placing the branching indicator.
	{
		SDL_Rect select_indicator = {	p->rep.size.x - (40 * scale.amount),
										p->rep.size.y - (40 * scale.amount),
										(BOARD_SIZE + 80) * scale.amount,
										(BOARD_SIZE + 80) * scale.amount	
									};
											
		
		SDL_SetRenderTarget (renderer, NULL);
		SDL_RenderCopy (renderer, branchTex, NULL, &select_indicator);
		render(list);
	}
	
	
	
	
									//input loop : 			//going back and forth in the branching window
	while (1) {												//branching, if new move
		
		SDL_SetRenderTarget (renderer, p->rep.snap);
	
	    
		while (!SDL_PollEvent(&event))
			;	

		
		
		
		
		if (event.type == SDL_MOUSEBUTTONDOWN) {
				
			while (!SDL_PollEvent(&event))
				;
		
			
		
			if (event.type == SDL_MOUSEMOTION)
				while (1) {
					SDL_PollEvent(&event);
					if (event.type == SDL_MOUSEBUTTONUP && event.button.button == SDL_BUTTON_LEFT)
						break;	//there was continue here. What was that supposed to do? hung the program.
				}
			
			else if (event.type == SDL_MOUSEBUTTONUP && event.button.button == SDL_BUTTON_LEFT) {
				
				if (!isin_box(infocus->rep.size, event.button))
					continue;
				
				double x, y;
				int column, row;
				
				x 	= 	(event.button.x - (p->rep.size.x + BORDER*scale.amount)) / (SQUARE_SIZE*scale.amount); 
				y 	= 	(event.button.y - (p->rep.size.y + BORDER*scale.amount)) / (SQUARE_SIZE*scale.amount);
			
													//rounding:  row and column are swapped because that is how an array be.
				if ((x - (int)x) >= 0.5)
					 column = (int)x + 1;
				else column = (int)x;
				
				if ((y - (int)y) >= 0.5)
					 row = (int)y + 1;
				else row = (int)y;
				
				
				if (p->mech.state[column][row].S_no <= current_move)
					if (p->mech.state[column][row].S_no != 0)
						continue;
					
				
				if (p->mech.state[column][row].S_no != current_move + 1) {
					//printf ("turn right b4 entering off_shoot: %d\n", infocus->node->board.mech.turn);
					off_shoot(p, column, row, current_move, &n_boards);
					return;
				}
				else {
					current_move++;
					
					/*
					if (p->node->above != NULL) 			
						parts.number = current_move - p->node->above->last_move->S_no;		
					else parts.number = current_move;
					*/
					parts.number = current_move - p->first_move->S_no + 1;
					
					if (copy_turn) {
						place_stone(column, row, p, parts.whiteStone);
						parts.font_color.r = 0; parts.font_color.g = 0; parts.font_color.b = 0;
					}
					else { 
						place_stone(column, row, p, parts.blackStone);
						parts.font_color.r = 255; parts.font_color.g = 255; parts.font_color.b = 255;
					}
					put_number(column, row, &parts);
					
					copy_turn++;
					copy_turn %= 2;
					
					SDL_SetRenderTarget (renderer, NULL);	//this is needed of course so that render() renders the entire screen
					render(list);
					
					printf ("turn after action in branch mode: %d\n", p->mech.turn);
					
				}
			}
		}
			
			
			
		if (event.key.keysym.sym == SDLK_LEFT && event.type == SDL_KEYUP) {
			
			if (p->above_board != NULL) {				
				if (current_move == p->above_board->mech.total_moves)		//disallowing undoing the moves of the parent board on the board below.
					continue;
			}
			else if (current_move == 0) 
					continue;
			
			parts.number = current_move - p->first_move->S_no + 1;	
			  
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
				parts.font_color.r = 255; parts.font_color.g = 255; parts.font_color.b = 255;
			}
			else { 
				place_stone(column, row, p, ghost_whiteStone);
				parts.font_color.r = 0; parts.font_color.g = 0; parts.font_color.b = 0;
			}
			put_number(column, row, &parts);
			
			copy_turn++;
			copy_turn %= 2;
			
			SDL_SetRenderTarget (renderer, NULL);
			render(list);
			
		//	printf ("turn after action in branch mode: %d\n", p->mech.turn);
		} 
		
		
		
		else if (event.key.keysym.sym == SDLK_RIGHT && event.type == SDL_KEYUP) {
			
			if (current_move == p->mech.total_moves)
				continue;
				
			current_move++;			//this has to be b4 the changes, unlike when going backwards.
		/*	
			if (p->node->above != NULL) 		
				parts.number = current_move - p->node->above->last_move->S_no;		
			else parts.number = current_move;
		*/
			parts.number = current_move - p->first_move->S_no + 1;
			
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
				parts.font_color.r = 0; parts.font_color.g = 0; parts.font_color.b = 0;
			}
			else { 
				place_stone(column, row, p, blackStone);
				parts.font_color.r = 255; parts.font_color.g = 255; parts.font_color.b = 255;
			}
			put_number(column, row, &parts);
			
			copy_turn++;
			copy_turn %= 2;
						
			SDL_SetRenderTarget (renderer, NULL);	//this is needed of course so that render() renders the entire screen
			render(list);
			
			//printf ("turn after action in branch mode: %d\n", p->node->board.mech.turn);
			
		}
			
		
		else if (event.key.keysym.sym == SDLK_q) {
			SDL_SetRenderTarget (renderer, p->rep.snap);
			SDL_RenderCopy(renderer, preserve, NULL, NULL);
			SDL_SetRenderTarget (renderer, NULL);
			//p->node->board.mech.turn = preserve_turn;
			break;
		}
		
		//printf ("turn after action in branch mode: %d\n", p->node->board.mech.turn);
	}
}
	




void off_shoot (struct board *p, int column, int row, int moveNum, int *n_boards) {
	
	/*
	struct board *new_item_1 = declare_new_board(n_boards, infocus, scale);	
	
	new_item_1->above_board = infocus;
	
												//adjusting the new item with all the links below.
	if (infocus->below != NULL) {
		
		new_item_1->below = infocus->below; //this is only needed once, so outside of loop.
		
		for (struct spawn *b = infocus->below; b != NULL; ) {	
			b->board->above_board = new_item_1;			//last_move to be updated
			b->board->line->start_board = new_item_1;
		
			b->board->line->start.x = new_item_1->rep.size.x + (BOARD_SIZE/2) * scale.amount;
			b->board->line->start.y = new_item_1->rep.size.y + (BOARD_SIZE/2) * scale.amount;
							
			if (b->next == NULL)
				break;
			b = b->next; 
		}
	}
	else new_item_1->below = NULL;
	
												//have to allocate new memory or else it would save it in the old location, that is, the below link of the new board. 
	if ((infocus->below = malloc(sizeof(struct spawn))) == NULL)	
		printf ("couldn't allocate memory for spawn struct");	
	infocus->below->board = new_item_1;		
	infocus->below->next = NULL;
	
	
	
	
	
	new_item_1->line = declare_new_line (infocus, new_item_1, scale);
								
								//fitting the item & line into the universal lists	
	fit_in_list (new_item_1, &list, &list_lines);
	recur_shift (new_item_1->below, scale);

	
	 
						// setting moves, board state
						
	new_item_1->mech = infocus->mech;					
			
	//~ printf ("\nturn while assigning the new board mech: %d\n\n", infocus->mech.turn);		
						
	int start = 1, counter;		//start seems to serve no purpose, I can just remove it and set counter to 1 directly.
	
	counter = start;
	
	printf ("\n-----cloning the first board-----\n");
	
	int x, y;
	for (; counter <= moveNum; counter++)
										//moveNum is the number of the last common move.
		for (x = 0; x < 19; x++) {
			for (y = 0; y < 19; y++)
				if (new_item_1->mech.state[x][y].S_no == counter) {
					if (new_item_1->mech.state[x][y].colour == 1) 
						place_stone (x, y, new_item_1, blackStone);
					else if (new_item_1->mech.state[x][y].colour == 2)
						place_stone (x, y, new_item_1, whiteStone);
					break;
				}		
			if (y < 19)
				break;
		}
	
	printf ("Common moves cloned\n");	
	

	
	for (int column, row; counter <= new_item_1->mech.total_moves; counter++)			//moveNum is the number of the last common move.
		for (column = 0; column < 19; column++) { 
			for (row = 0; row < 19; row++)	
				if (new_item_1->mech.state[column][row].S_no == counter) 	{
					if (new_item_1->mech.state[column][row].colour == 1) {
						place_stone (column, row, new_item_1, blackStone);
						parts.font_color.r = 255; parts.font_color.g = 255; parts.font_color.b = 255; 
					}
					else if (new_item_1->mech.state[column][row].colour == 2) {
						place_stone (column, row, new_item_1, whiteStone);
						parts.font_color.r = 0; parts.font_color.g = 0; parts.font_color.b = 0;
					}
					parts.number = counter - moveNum;
					parts.board = new_item_1;
					put_number(column, row, &parts);
					break;
				}		
			if (row < 19)
				break;
		}
	
	//~ infocus->first_move = malloc(sizeof(struct stone));
	new_item_1->first_move = malloc(sizeof(struct stone));
	new_item_1->last_move = malloc(sizeof(struct stone));
	
	
									//first and last moves, new_item_1
									//last move, infocus
	for (int x = 0; x < 19; x++) 
		for (int y = 0; y < 19; y++)	{
			if (infocus->mech.state[x][y].S_no == moveNum) {
				infocus->last_move->S_no = infocus->mech.state[x][y].S_no; 
				infocus->last_move->colour = infocus->mech.state[x][y].colour; 
				infocus->last_move->column = x; 
				infocus->last_move->row = y; 
			}
			if (new_item_1->mech.state[x][y].S_no == moveNum + 1) {
				*(new_item_1->first_move) = new_item_1->mech.state[x][y];
				new_item_1->first_move->S_no = new_item_1->mech.state[x][y].S_no; 
				new_item_1->first_move->colour = new_item_1->mech.state[x][y].colour; 
				new_item_1->first_move->column = x; 
				new_item_1->first_move->row = y; 
			}
			if (new_item_1->below == NULL)
				continue;
			if (new_item_1->mech.state[x][y].S_no == new_item_1->mech.total_moves) {
				*(new_item_1->first_move) = new_item_1->mech.state[x][y];
				new_item_1->last_move->S_no = new_item_1->mech.state[x][y].S_no; 
				new_item_1->last_move->colour = new_item_1->mech.state[x][y].colour; 
				new_item_1->last_move->column = x; 
				new_item_1->last_move->row = y; 
			}
		}
	
	counter--;	
										
	printf ("-----done cloning the first board-----\n\n");									
										
											//undoing moves in the parent board
											
	for ( ; counter > moveNum; counter--) 
		undo_move(infocus, &infocus, &text, TRUE);
	*/			
	
	
	infocus = split_board(n_boards, moveNum, &parts, &text, &infocus, &list, &list_lines, scale);
	
	
					//____OFFSHOOT____
	
	continue_play(blackStone, whiteStone);	//places the common moves					//removes infocus
	
	
							//__placing the first move, of the offshoot line__
	if (list->mech.turn) {
		place_stone(column, row, list, whiteStone);
		parts.font_color.r = 0; parts.font_color.g = 0; parts.font_color.b = 0;
	}
	else { 
		place_stone(column, row, list, blackStone);
		parts.font_color.r = 255; parts.font_color.g = 255; parts.font_color.b = 255;
	}
	parts.board = list;
	parts.number = 1;
	put_number(column, row, &parts);
	
	list->mech.state[column][row].S_no = ++(list->mech.total_moves);							
	list->mech.state[column][row].colour = list->mech.turn + 1;  // + 1 because the colour enum has "empty" as the first element.  
	(list->mech.turn)++; 
	list->mech.turn %= 2;
	
						//setting the first_move of the offshoot board.
	
	list->first_move = malloc(sizeof(struct stone));
	list->first_move->S_no = list->mech.state[column][row].S_no;
	list->first_move->colour = list->mech.state[column][row].colour;
	list->first_move->column = column;
	list->first_move->row = row;	
						
				
				
											
									//shifting, making space for the new offshoot line
	
	int divide = list->rep.size.x + BOARD_SIZE;
	int shift = (int)(BOARD_SIZE/2) + 25;
	int shift_left = -(int)(shift*scale.amount);
	int shift_right = (int)(shift*scale.amount);
	
	
	shift_one (list, shift_right, 0, scale.amount);
	
	for (struct board *p = list; p != NULL; p = p->next) {
		if (p->number == list->above_board->number || p->number == list->number)
			continue;
		if (p->rep.size.x < divide)
			shift_one (p, shift_left, 0, scale.amount);
	}
	
	for (struct board *p = list; p != NULL; p = p->next) {
		if (p == list->above_board)	//not needed?
			continue;
		if (p->rep.size.x > divide)
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

void render (struct board *p) {
	
	
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
		 
		select_indicator.x = q->board->rep.size.x - (40 * scale.amount);
		select_indicator.y = q->board->rep.size.y - (40 * scale.amount);
		SDL_RenderCopy (renderer, selTex, NULL, &select_indicator);
	}
	
		
											//render boards
	for (; p != NULL; p = p->next) {

		p->rep.size.w = BOARD_SIZE*scale.amount;		// can the scaling be moved into the zoom function?
		p->rep.size.h = BOARD_SIZE*scale.amount;
		
		
		SDL_RenderCopy (renderer, bg_board, NULL, &(p->rep.size));
		SDL_RenderCopy (renderer, p->rep.snap, NULL, &(p->rep.size));
		
		
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
	
	printf ("\nlist: ");
	for (struct board *p = list; p != NULL; p = p->next)
			printf("%d ", p->number);
		printf ("\n");
		
	printf ("list_lines: ");
	for (struct list_lines *l = list_lines; l != NULL; l = l->next)
			printf("%d ", l->number);
		printf ("\n");
	
	printf ("selection: ");	
	for (struct opted *s = sel; s != NULL; s = s->next)
		printf("%d ", s->board->number);
	printf ("\n\n");
}




void inspect_board (struct board q) {
	
	int i, j;
	for (i = 0; i < 19; i++) {
		for (j = 0; j < 19; j++)
			printf ("%d ", q.mech.state[j][i].colour);
		printf ("\n");
	}
	
	
	printf ("\n\n||||||||||BOARD STATS||||||||||\n\n");
	
	
	printf ("\n\n_____ABOVE_____\n\n");
	struct board r = q;
	
	if (q.above_board == NULL)
		printf("----------\n");
	else {		
		printf (	
			"\t#%d\n"
			"total moves: %10d\n"
			"last_move-\n  colour: ",
			 r.above_board->number,
			 r.above_board->mech.total_moves);
			 ((r.above_board->last_move->colour == 1)?printf("black\n"):printf("white\n"));
			 
			 printf("  S_no: %d\n", r.above_board->last_move->S_no);	
			 printf ("turn:\t\t");
			 r.above_board->mech.turn?printf("white\n"):printf("black\n");
	}
	
	
	

	
	printf ("\n\n_____CURRENT_____\n\n");
	printf (
		"\t#%d\n"
		"total moves: %10d\n"
		"turn:\t\t",
		q.number,
		q.mech.total_moves);
		q.mech.turn?printf("white\n"):printf("black\n");
	
	printf ("first_move- ");
	if (q.first_move) {
		printf ("\n  colour: ");
		(q.first_move->colour == 1)?printf("black\n"):printf("white\n");
		printf("  S_no: %d\n", q.first_move->S_no);
	}
	else printf ("none\n");	
	
	printf ("last_move- ");
	if (q.last_move) {
		printf ("\n  colour: ");
		(q.last_move->colour == 1)?printf("black\n"):printf("white\n");
		printf("  S_no: %d\n", q.last_move->S_no);
	}
	else printf ("none\n");	
	
	
		
	
	
	
	printf ("\n\n_____BELOW_____\n\n");
	
	if (q.below == NULL)
		printf("----------\n");
	else {
		struct spawn *r = q.below;
		for (; ;) { 
			
			printf (	
				"\t#%d\n"
				"total moves: %10d\n"
				"first_move- ",
				 r->board->number,
				 r->board->mech.total_moves);
			if (r->board->first_move) {
				printf ("\n  colour: ");
				(r->board->first_move->colour == 1)?printf("black\n"):printf("white\n");
				printf("  S_no: %d\n", r->board->first_move->S_no);
			}
			else printf ("none\n");	
			
			
			
			printf ("turn:\t\t");
			r->board->mech.turn?printf("white\n"):printf("black\n");
				
			printf ("\n"); 
			if (r->next == NULL)
				break;
				
			r = r->next;
		}
	}
	
	printf ("\n\n");
}
	
	

/*
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

*/



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
	struct board *board_1 = malloc(sizeof(struct board)); 
	
	
	/*= {.mech.state = {{{0}}},		//the braces because it's an array of 
						.mech.turn = 0,						//structures. Still dk for sure.
						.mech.total_moves = 0,
						
						.number = n_boards;
						.first_move = NULL;
						.last_move = NULL;
						.selection = NULL;
						.line = NULL;
					
						.rep.center_off.x = 10 - CENTER_X, 
						.rep.center_off.y = 10 - CENTER_Y,
						.rep.size = {10, 10, BOARD_SIZE, BOARD_SIZE},
						.rep.snap = NULL,
						
						.above_board = NULL;
						.below = NULL;
						
						.prev = NULL;
						.next = NULL;
					};
	*/
	
	
	for (int i = 0; i < 19; i++)
		for (int j = 0; j < 19; j++) {
			board_1->mech.state[i][j].S_no = 0;
			board_1->mech.state[i][j].colour = 0;
		}
			
	board_1->mech.turn = 0;						//structures. Still dk for sure.
	board_1->mech.total_moves = 0;

	board_1->number = n_boards;
	board_1->first_move = NULL;
	board_1->last_move = NULL;
	board_1->selection = NULL;
	board_1->line = NULL;

	board_1->rep.size.x = 10; 
	board_1->rep.size.y = 10; 
	board_1->rep.size.w = BOARD_SIZE; 
	board_1->rep.size.h = BOARD_SIZE;
			
	board_1->rep.center_off.x = 10 - CENTER_X; 
	board_1->rep.center_off.y = 10 - CENTER_Y;
	board_1->rep.snap = NULL;
	
	board_1->above_board = NULL;
	board_1->below = NULL;
	
	board_1->prev = NULL;
	board_1->next = NULL;
												//the board state texture on a transparent bg
	board_1->rep.snap = SDL_CreateTexture (renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, BOARD_SIZE, BOARD_SIZE);
	SDL_SetTextureBlendMode(board_1->rep.snap, SDL_BLENDMODE_BLEND);
	SDL_SetRenderTarget (renderer, board_1->rep.snap);
	SDL_SetRenderDrawColor (renderer, 0, 0, 0, 0);
	SDL_RenderClear(renderer);
	SDL_SetRenderTarget (renderer, NULL);
	
	/*
	branch = malloc (sizeof(struct branch));		//why is this needed?
	branch->board = board_1;					//because this is a linked list while board above
											//is not one. This is the same as allocating mem
											//for a new_node and pointing first to it.
	
	branch->above = NULL;
	branch->below = NULL;
	
	branch->number = n_boards;
	*/
	//first->next = NULL;
	
	list = board_1;
	
	/*
	malloc (sizeof(struct board));
	list = board_1;
	list->number = n_boards;
	list->selection = NULL;
	list->prev = NULL;
	list->next = NULL;
	*/
	
	
}




