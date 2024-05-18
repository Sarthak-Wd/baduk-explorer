#include "include/main.h"



bool game_is_running = TRUE;

SDL_Event event;
SDL_MouseButtonEvent pan_start;

int n_boards = 1;

struct board *infocus = NULL;

//~ int frames_rendered = 0;







//struct branch *branch 	= NULL; 
struct board *list 	= NULL; 
struct board *first_board = NULL;
struct list_lines *list_lines	= NULL;

struct message text;
struct opted *sel 	= NULL;	

playing_parts parts;
playing_parts liberty_parts;
scaling scale = {1.0, .center.x = CENTER_X, .center.y = CENTER_Y};
	





int main ()
{
	
	
	if (!initialize_window())
		destroy_window();
	
	load_setup(); 
	
	
	while (game_is_running) {
			
		SDL_SetRenderDrawColor(renderer, 110, 170, 50, 255);
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
											continue_play(&n_boards, &infocus, &list, &list_lines, &parts, scale);
											break;
										}
										if (event.button.button == SDL_BUTTON_LEFT) {
											render(list); 		//to clear any warning messages displayed on last input. //might not need this since will need to render to center the board anyway.
											struct board *p = list;
											for ( ; p != NULL; p = p->next)
												if (isin_box(p->rep.size, event.button)) {
													
													//~ printf ("\nnumber: %d, total moves: %d\n", p->number, p->mech.total_moves); 
													branch_window(p, &list, &list_lines, &n_boards, &scale, &infocus, &parts, 0);
													break;
												}
											break;
										}
										
										if (event.type == SDL_KEYUP)
											if (event.key.keysym.sym == SDLK_LALT)
												break;
										
										
									} break;
									
									
			case SDLK_LCTRL:		while (1) {
										SDL_PollEvent(&event);
										if (event.key.keysym.sym == SDLK_LSHIFT) {
											while (1) {
												SDL_PollEvent(&event);
												if (event.type == SDL_KEYUP)
													break;
												if (event.key.keysym.sym == SDLK_s)  {
													write_save(first_board, n_boards, &scale, 1);
													break;
												}
											}
											break;
										}
										if (event.key.keysym.sym == SDLK_s)  {
											write_save(first_board, n_boards, &scale, 0);
											break;
										}
										if (event.key.keysym.sym == SDLK_o)  {
											load_save(&list, &list_lines, &first_board, &n_boards, &scale, &parts);
											break;
										}
										if (event.type == SDL_KEYUP)
											break;
									} break;
									
				
				case SDLK_d:		while (1) {
										while (!SDL_PollEvent(&event))
											;
										
										if (event.type == SDL_MOUSEBUTTONDOWN &&
											event.button.button == SDL_BUTTON_LEFT) {	
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
									} break;
									
									
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
											if (event.key.keysym.sym == SDLK_i)
												break;
									} 
									break;
									
				//~ case SDLK_LSHIFT:	while (1) {

										//~ while (!SDL_PollEvent(&event))
											//~ ;
										//~ if (event.button.button == SDL_BUTTON_LEFT) {	
											//~ struct board *p = list;
											//~ for ( ; p != NULL; p = p->next)
												//~ if (isin_box(p->rep.size, event.button)) { 
													//~ liberty_parts.board = p;
													//~ print_liberties(&liberty_parts);
													//~ break;
												//~ }
											//~ break;
										//~ }
										
										//~ if (event.type == SDL_KEYUP)
											//~ if (event.key.keysym.sym == SDLK_i)
												//~ break;
									//~ } 
									//~ break;
									
									
				case SDLK_g:		while (1) {
										while (!SDL_PollEvent(&event))
											;
										if (event.type == SDL_MOUSEBUTTONDOWN && 
										event.button.button == SDL_BUTTON_LEFT) {	
											struct board *p = list;
											for ( ; p != NULL; p = p->next)
												if (isin_box(p->rep.size, event.button)) { 
													inspect_groups(*p);
													break;
												}
											break;
										}
										
										if (event.type == SDL_KEYUP)
											if (event.key.keysym.sym == SDLK_g)
												break;
									} 
									break;
									
									
				case SDLK_t: 		testing(); break;
				
				case SDLK_c:		while (1) {
										while (!SDL_PollEvent(&event))
											;
										if (event.button.button == SDL_BUTTON_LEFT) {	
											struct board *p = list;
											for ( ; p != NULL; p = p->next)
												if (isin_box(p->rep.size, event.button)) { 
													combine_board(p, &parts, scale, &list_lines);
													break;
												}
											break;
										}
										
										if (event.type == SDL_KEYUP)
											if (event.key.keysym.sym == SDLK_d)
												break;
									} break;
									
				case SDLK_v:		while (1) {
										while (!SDL_PollEvent(&event))
											;
										if (event.button.button == SDL_BUTTON_LEFT) {	
											struct board *p = list;
											for ( ; p != NULL; p = p->next)
												if (isin_box(p->rep.size, event.button)) { 
													split_mode(p, &n_boards, &list, &infocus, &list_lines, &scale, &parts);
													break;
												}
											break;
										}
										
										if (event.type == SDL_KEYUP)
											if (event.key.keysym.sym == SDLK_d)
												break;
									}  break;
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
				 pan_manual (list, list_lines, &event, &pan_start, scale);
			}
					
			else if (event.type == SDL_MOUSEBUTTONUP) { 
				
				if (event.button.x > (WINDOW_WIDTH - 50)  && event.button.y < 50)
					add_board(&n_boards, &infocus, scale, &list, &list_lines);		//improve, make a button, w/ a rect
					
				else  if (event.button.button == SDL_BUTTON_LEFT) {
						struct board *p = list;
						for ( ; p != NULL; p = p->next)
							if (isin_box(p->rep.size, event.button)) {
								if (p->below) {
									show_err(&text);
									break;
								}
								int column, row;
								coords_from_mouse(event, p, &column, &row, scale.amount); 
								parts.board = p;
								play_move (column, row, &parts);
								infocus = parts.board;
								break;
							}	
				}
				else { struct board *p = list;
						for ( ; p != NULL; p = p->next)
							if (isin_box(p->rep.size, event.button)) {
								undo_move(p, &infocus, &text, FALSE, &parts);
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
	
	//~ for (struct board *p = list; p; p = p->next) {
		//~ liberty_parts.board = p;
		//~ print_liberties (&liberty_parts);
	//~ }
		
											//render boards
	for (; p != NULL; p = p->next) {

		p->rep.size.w = BOARD_SIZE*scale.amount;		// can the scaling be moved into the zoom function?
		p->rep.size.h = BOARD_SIZE*scale.amount;
		
		
		SDL_RenderCopy (renderer, bg_board, NULL, &(p->rep.size));
		SDL_RenderCopy (renderer, p->rep.snap, NULL, &(p->rep.size));
		//~ SDL_RenderCopy (renderer, p->liberties, NULL, &(p->rep.size));
		
		q = sel;		//what are these both for?
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
	
	printf ("\nnumber of boards: %d", n_boards);
	
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
	




void inspect_groups (struct board q) {
	
int i, j;
	printf ("    ");
	for (i = 0; i < 19; i++) 
		printf ("%2d ", i);
	printf ("\n");
	printf ("    ");
	for (i = 0; i < 19; i++) 
		printf ("___");
	printf ("\n");
	for (i = 0; i < 19; i++) {
		printf ("%2d |", i);
		for (j = 0; j < 19; j++) {
			if (q.mech.state[j][i].colour == empty)
				printf (" . ");
			else if (q.mech.state[j][i].group != NULL)
				printf ("%2.2d ", q.mech.state[j][i].group->number);
			//~ else printf ("%d ", q.mech.state[j][i].colour);
		}
		printf ("\n");
	}
	printf ("    ");
	for (i = 0; i < 19; i++) 
		printf ("___");
	printf ("\n");
	printf ("    ");
	for (i = 0; i < 19; i++) 
		printf ("%2d ", i);
	
	printf("\n\n\n");	
}


	
	
void show_err (struct message *text) {
	
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
	blackStone = IMG_LoadTexture (renderer, "media/black-stone.png");
	whiteStone = IMG_LoadTexture (renderer, "media/white-stone.png");
	ghost_blackStone = IMG_LoadTexture (renderer, "media/ghost-black-stone.png");
	ghost_whiteStone = IMG_LoadTexture (renderer, "media/ghost-white-stone.png");
	highlight_stone = IMG_LoadTexture (renderer, "media/highlight-stone.png");
	
	parts.blackStone = blackStone; 
	parts.whiteStone = whiteStone;
	parts.ghost_b = ghost_blackStone; 
	parts.ghost_w = ghost_whiteStone; 
	parts.font = font;	
	
	liberty_parts.font = font;
	
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
	SDL_SetRenderDrawColor (renderer, 250, 240, 225, 255);
	SDL_RenderFillRect (renderer, NULL);
	SDL_SetRenderTarget (renderer, NULL);
	
	splitTex = SDL_CreateTexture (renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, BOARD_SIZE, BOARD_SIZE);
	SDL_SetRenderTarget (renderer, splitTex);
	SDL_SetRenderDrawColor (renderer, 255, 150, 240, 255);
	SDL_RenderFillRect (renderer, NULL);
	SDL_SetRenderTarget (renderer, NULL);
	
	
	modeTex_undo = SDL_CreateTexture (renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, BOARD_SIZE, BOARD_SIZE);
	SDL_SetRenderTarget (renderer, modeTex_undo);
	SDL_SetRenderDrawColor (renderer, 100, 200, 150, 255);
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
			board_1->mech.state[i][j].group = NULL;
			board_1->mech.state[i][j].merge = FALSE;
			board_1->mech.state[i][j].captured_groups = NULL;
		}
			
	board_1->mech.turn = 0;						//structures. Still dk for sure.
	board_1->mech.total_moves = 0;

	board_1->number = n_boards;
	board_1->first_move = NULL;
	board_1->last_move = NULL;
	board_1->selection = NULL;
	board_1->line = NULL;
	
	board_1->groups = NULL;
	board_1->num_groups = 0;
	
	

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
	
	board_1->liberties = SDL_CreateTexture (renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, BOARD_SIZE, BOARD_SIZE);
	SDL_SetTextureBlendMode(board_1->liberties, SDL_BLENDMODE_BLEND);
	SDL_SetRenderTarget (renderer, board_1->liberties);
	SDL_SetRenderDrawColor (renderer, 0, 0, 0, 0);
	SDL_RenderClear(renderer);
	SDL_SetRenderTarget (renderer, NULL);
	
	
	
	list = board_1;
	
	first_board = board_1;
	
}




