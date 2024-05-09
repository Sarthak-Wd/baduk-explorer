#include <stdio.h>
#include "common.h"
#include "init.h"
#include "camera.h"
#include "helper_functions.h"
#include "routine_functions.h"
#include "saving.h"



bool game_is_running = TRUE;

SDL_Event event;
SDL_MouseButtonEvent pan_start;

int n_boards = 1;

struct board *infocus = NULL;

int frames_rendered = 0;







//struct branch *branch 	= NULL; 
struct board *list 	= NULL; 
struct board *first_board = NULL;
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
void branch_window (struct board *p, bool shifting);
void off_shoot (struct board *p, int row, int column, int moveNum, int *n_boards);
void combine_board (struct board *p);
void split_mode (struct board *p);

void show_err (struct message *text);
void display_text (struct message text);
void ticker (void);
void testing (void);
void inspect_board (struct board p);
void inspect_groups (struct board q);




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
											continue_play(blackStone, whiteStone);
											break;
										}
										if (event.button.button == SDL_BUTTON_LEFT) {
											render(list); 		//to clear any warning messages displayed on last input. //might not need this since will need to render to center the board anyway.
											struct board *p = list;
											for ( ; p != NULL; p = p->next)
												if (isin_box(p->rep.size, event.button)) {
													
													//~ printf ("\nnumber: %d, total moves: %d\n", p->number, p->mech.total_moves); 
													branch_window(p, 0);
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
													combine_board(p);
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
													split_mode(p);
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
	
								//autopan:  after the offshoot check since it shouldn't be executed if called by offshoot.
	struct whole_coords shift;					
	shift.x = 0;
	shift.y = (int)(-(BOARD_SIZE + SPACE_BW) * scale.amount);
	pan (list, list_lines, &event, scale, shift);

							
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
	










void branch_window (struct board *p, bool shifting) {
	
	

	if (!p->first_move)
		return;
	
	parts.board = p;
	
	int current_move = p->mech.total_moves;
	infocus = p;
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
		}
	
	}
	
	
											
				//placing the branching indicator.
	
	SDL_Rect select_indicator = {	p->rep.size.x - (40 * scale.amount),
									p->rep.size.y - (40 * scale.amount),
									(BOARD_SIZE + 80) * scale.amount,
									(BOARD_SIZE + 80) * scale.amount	
								};
	
	SDL_SetRenderTarget (renderer, NULL);									
	

	SDL_RenderCopy (renderer, branchTex, NULL, &select_indicator);
	SDL_RenderCopy (renderer, bg_board, NULL, &(p->rep.size));
	SDL_RenderCopy (renderer, p->rep.snap, NULL, &(p->rep.size));
	SDL_RenderPresent (renderer);







	
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
				
				if (!isin_box(infocus->rep.size, event.button))
					continue;
				
				
				
				double x, y;
				int column, row;
				
				x 	= 	(event.button.x - (p->rep.size.x + BORDER*scale.amount)) / (SQUARE_SIZE*scale.amount); 
				y 	= 	(event.button.y - (p->rep.size.y + BORDER*scale.amount)) / (SQUARE_SIZE*scale.amount);
				
															
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
					pan (list, list_lines, &event, scale, shift);
					
					SDL_SetRenderDrawColor(renderer, 100, 200, 150, 255);
					SDL_RenderClear(renderer);
					render(list);
					
					branch_window(b->board, 1);
					
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
				infocus = p->above_board;			//do I need to fix this? how?
				off_shoot(p, column, row, current_move, &n_boards);
				
				shift.x = old_coord.x - list->rep.size.x; 
				shift.y = old_coord.y - list->rep.size.y;
				pan (list, list_lines, &event, scale, shift);
				
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
					
				if (!isin_box(infocus->rep.size, event.button))
					continue;
				
				double x, y;
				int column, row;
				
				x 	= 	(event.button.x - (p->rep.size.x + BORDER*scale.amount)) / (SQUARE_SIZE*scale.amount); 
				y 	= 	(event.button.y - (p->rep.size.y + BORDER*scale.amount)) / (SQUARE_SIZE*scale.amount);
				
															
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
					pan (list, list_lines, &event, scale, shift);
					
					SDL_SetRenderDrawColor(renderer, 100, 200, 150, 255);
					SDL_RenderClear(renderer);
					render(list);
					
					branch_window(b->board, 1);
					
					return;
				}
				
				SDL_RenderCopy (renderer, modeTex_undo, NULL, &select_indicator);
				SDL_RenderCopy (renderer, bg_board, NULL, &(p->rep.size));
				SDL_SetRenderTarget (renderer, p->rep.snap);
				SDL_RenderCopy(renderer, preserve, NULL, NULL);
				SDL_SetRenderTarget (renderer, NULL);
				SDL_RenderCopy (renderer, p->rep.snap, NULL, &(p->rep.size));
				
				SDL_RenderPresent(renderer);

				
				infocus = p;			//do I need to fix this? how?
				off_shoot(p, column, row, current_move, &n_boards);
				
				shift.x = p->rep.size.x - list->rep.size.x; 
				shift.y = p->rep.size.y - list->rep.size.y;
				pan (list, list_lines, &event, scale, shift);
				
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
				//~ printf ("0-in loop\n");
		
			//~ printf ("1-out of loop\n");
		
			if (event.type == SDL_MOUSEMOTION) {
				
				//~ printf ("2-panning\n");
	mousemotion:	
				pan_manual (list, list_lines, &event, &pan_start, scale);
				
				SDL_Rect select_indicator = {	p->rep.size.x - (40 * scale.amount),
												p->rep.size.y - (40 * scale.amount),
												(BOARD_SIZE + 80) * scale.amount,
												(BOARD_SIZE + 80) * scale.amount	
											};
				
				SDL_SetRenderTarget (renderer, NULL);									
				SDL_SetRenderDrawColor(renderer, 100, 200, 150, 255);
				SDL_RenderClear(renderer);

				SDL_RenderCopy (renderer, branchTex, NULL, &select_indicator);
				
				render(list);
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
					infocus = split_board(&n_boards, current_move, &parts, &text, &infocus, &list, &list_lines, scale);
					off_shoot(p, column, row, current_move, &n_boards);
					
					
						//this has to be here. can't be in continue_play because the boards are shifted in offshoot.
					struct whole_coords shift;
					shift.x = p->rep.size.x - list->rep.size.x; 
					shift.y = p->rep.size.y - list->rep.size.y;
					
					pan (list, list_lines, &event, scale, shift);
															
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
			SDL_RenderCopy (renderer, bg_board, NULL, &(p->rep.size));
			SDL_RenderCopy (renderer, p->rep.snap, NULL, &(p->rep.size));
			SDL_RenderPresent(renderer);			
		}
			
			
		else if (event.type == SDL_MOUSEWHEEL) {
			
			zoom_coords (list, list_lines, event, &scale);
			SDL_Rect select_indicator = {	p->rep.size.x - (40 * scale.amount),
									p->rep.size.y - (40 * scale.amount),
									(BOARD_SIZE + 80) * scale.amount,
									(BOARD_SIZE + 80) * scale.amount	
								};
	
			SDL_SetRenderTarget (renderer, NULL);									
			SDL_SetRenderDrawColor(renderer, 100, 200, 150, 255);
			SDL_RenderClear(renderer);
		
			SDL_RenderCopy (renderer, branchTex, NULL, &select_indicator);
			
			render(list);
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

void off_shoot (struct board *p, int column, int row, int moveNum, int *n_boards) {
	
	
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
	
	//~ list->above_board->below->next->board->rep.size.x + BOARD_SIZE*scale.amount
	
	struct spawn *max = list->above_board->below->next;
	int biggest_coord = list->above_board->below->next->board->rep.size.x;
	for (struct spawn *walk = max->next; walk != NULL; walk = walk->next)
		if (walk->board->rep.size.x > biggest_coord) {
			biggest_coord = walk->board->rep.size.x;
			max = walk;
		}
				
	
	
	
	int divide = max->board->rep.size.x + BOARD_SIZE;
	int shift_left = -(int)(((BOARD_SIZE/2) + 25)*scale.amount);
	int shift_right = (int)(((BOARD_SIZE/2) + 25)*scale.amount);
	
	
	
	
	list->rep.size.x = max->board->rep.size.x;
	list->rep.center_off.x = (list->rep.size.x)/scale.amount - scale.center.x;
	list->line->end.x = list->rep.size.x + (BOARD_SIZE/2) * scale.amount;
	shift_one (list, scale.amount, shift_right, 0);
	
	for (struct board *p = list; p != NULL; p = p->next) {
		if (p->number == list->above_board->number || p->number == list->number)
			continue;
		if (p->rep.size.x < divide) {
			//~ printf("shifting left\n");
			shift_one (p, scale.amount, shift_left, 0);
		}
	}
	
	for (struct board *p = list; p != NULL; p = p->next) {
		if (p == list->above_board)	//not needed?
			continue;
		if (p->rep.size.x > divide)
			shift_one (p, scale.amount, shift_right, 0);
	}	
	
}
	
	
		
		
void combine_board (struct board *p) {
	
	
	if (!p->below)
		return;
		
	if (p->below->next)
		return;
	
	struct board *b = p->below->board;
	b->first_move = p->first_move;
	
	int offset;
	if (p->above_board)
		offset = p->above_board->mech.total_moves;
	else offset = 0;
	
				//making the combined board out of the below board.
	
	for (int column, row, counter = 1; counter + offset <= b->mech.total_moves; counter++)			//moveNum is the number of the last common move.
		for (column = 0; column < 19; column++) { 
			for (row = 0; row < 19; row++)	
				if (b->mech.state[column][row].S_no == counter + offset) 	{
					if (b->mech.state[column][row].colour == 1) {
						place_stone (column, row, b, blackStone);
						parts.font_color.r = 255; parts.font_color.g = 255; parts.font_color.b = 255; 
					}
					else if (b->mech.state[column][row].colour == 2) {
						place_stone (column, row, b, whiteStone);
						parts.font_color.r = 0; parts.font_color.g = 0; parts.font_color.b = 0;
					}
					parts.number = counter;
					parts.board = b;
					put_number(column, row, &parts);
					break;
				}		
			if (row < 19)
				break;
		}
		
		
	
				//branch linking
	
	if (p->above_board) {
		for (struct spawn *walk = p->above_board->below; walk != NULL; walk = walk->next)
			if (walk->board->number == p->number) {
				walk->board = b;
				break;
			}
		b->line->start_board = p->above_board;
		b->line->start.x = p->above_board->rep.size.x + (BOARD_SIZE/2) * (scale.amount);
		b->line->start.y = p->above_board->rep.size.y + (BOARD_SIZE/2) * (scale.amount);
	}	
	else  {
		if (b->line->prev)
			b->line->prev->next = b->line->next;
		else list_lines = b->line->next;
		if (b->line->next)
			b->line->next->prev = b->line->prev;
		free(b->line);
		b->line = NULL;
	}
		
	b->above_board = p->above_board;
	
	
	
	
	
	struct whole_coords old_coord = {p->rep.size.x, p->rep.size.y}; 	//to place the combined board right in the old boards place.
				
				
				//adjusting universal lists
		
	if (p->prev)	
		p->prev->next = p->next;
	if (p->next)
		p->next->prev = p->prev;
	
	if (p->above_board) {
		if (p->line->prev)
			p->line->prev->next = p->line->next;
		if (p->line->next)
			p->line->next->prev = p->line->prev;
		free(p->line);
	}
	
	b->number = p->number;  //need this, otherwise there will be two boards w/ the same number. Which creates a problem in offshoot shifting. 
	free(p);
		
									
	int shift_x = old_coord.x - b->rep.size.x;	
	int shift_y = old_coord.y - b->rep.size.y;	
	shift_one (b, scale.amount, shift_x, shift_y); 

	for (struct spawn *walk = b->below; walk != NULL; walk = walk->next) {
		printf("hello\n");
		recur_shift(walk->board, scale.amount, 0, shift_y);
	}

	printf ("out\n");	
}
		
		
		
		



void split_mode (struct board *p) {
	

	if (!p->first_move)
		return;
	
	parts.board = p;
	
	int current_move = p->mech.total_moves;
	infocus = p;
	//~ struct whole_coords shift;
	//~ int copy_turn = p->mech.turn;    //copy made only for split mode.

	
										//preserving the actual board state.
						//why not just preserve the entire board struct?				
	SDL_Texture *preserve = SDL_CreateTexture (renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, BOARD_SIZE, BOARD_SIZE);
	SDL_SetRenderTarget (renderer, preserve);
	SDL_RenderCopy(renderer, p->rep.snap, NULL, NULL);

	
	
	
		//split mode indicator
	
	SDL_Rect select_indicator = {	p->rep.size.x - (40 * scale.amount),
									p->rep.size.y - (40 * scale.amount),
									(BOARD_SIZE + 80) * scale.amount,
									(BOARD_SIZE + 80) * scale.amount	
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
															
															
															parts.number = current_move - p->first_move->S_no + 1;
															if (p->mech.state[column][row].colour == 1) {
																place_stone(column, row, p, blackStone);
																parts.font_color.r = 255; parts.font_color.g = 255; parts.font_color.b = 255;
															}
															else { 
																place_stone(column, row, p, whiteStone);
																parts.font_color.r = 0; parts.font_color.g = 0; parts.font_color.b = 0;
															}
															put_number(column, row, &parts);
															
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
															split_board(&n_boards, current_move, &parts, &text, &infocus, &list, &list_lines, scale);	
															render (list);
															return;
				}
				
				
			case SDL_MOUSEBUTTONDOWN: 
				
						pan_start = event.button;					//in case mousemotion
						while (!SDL_PollEvent(&event))
							;
		
						switch (event.type) {
		
								case SDL_MOUSEMOTION:  		pan_manual (list, list_lines, &event, &pan_start, scale);
											
															
															select_indicator.x = p->rep.size.x - (40 * scale.amount);
															select_indicator.y = p->rep.size.y - (40 * scale.amount);
															
															
															SDL_SetRenderTarget (renderer, NULL);									
															SDL_SetRenderDrawColor(renderer, 100, 200, 150, 255);
															SDL_RenderClear(renderer);
											
															SDL_RenderCopy (renderer, splitTex, NULL, &select_indicator);
															
															render(list);
															break;
															
							}				
						
						break;
						
								
								
			case SDL_MOUSEWHEEL: 		zoom_coords (list, list_lines, event, &scale);
															
										select_indicator.x = p->rep.size.x - (40 * scale.amount);
										select_indicator.y = p->rep.size.y - (40 * scale.amount);
										select_indicator.w = (BOARD_SIZE + 80) * scale.amount;
										select_indicator.h = (BOARD_SIZE + 80) * scale.amount;
										
										SDL_SetRenderTarget (renderer, NULL);									
										SDL_SetRenderDrawColor(renderer, 100, 200, 150, 255);
										SDL_RenderClear(renderer);
									
										SDL_RenderCopy (renderer, splitTex, NULL, &select_indicator);
										
										render(list);
										break;
						
																							
											
		}
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
	for (i = 0; i < 19; i++) {
		for (j = 0; j < 19; j++) {
			if (q.mech.state[j][i].colour == empty)
				printf (". ");
			else if (q.mech.state[j][i].ptp_group != NULL)
				printf ("%d ", (*(q.mech.state[j][i].ptp_group))->number);
			//~ else printf ("%d ", q.mech.state[j][i].colour);
		}
		printf ("\n");
	}
	
	printf("\n\n");	
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
			board_1->mech.state[i][j].ptp_group = NULL;
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
	
	
	
	list = board_1;
	
	first_board = board_1;
	
}




