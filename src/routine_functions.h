#include "common.h"
#include "init.h"
#include "helper_functions.h"

enum stone_codes {liberty, ally_stone, opp_stone, group_match};



//~ group_match if ((column < 18) && (board->mech.state[column+1][row].group == group)) 		
//~ liberty 	if ((column < 18) && (board->mech.state[column+1][row].colour == empty))
//~ opp_group 	if ((column+1 == 18) || (board->mech.state[column+1 +1][row].group != group))
//~ (cascading)

//~ ally_stone 	if ((column < 18) && 
				//~ (board->mech.state[column+1][row].colour == board->mech.state[column][row].colour)) 

//~ ally_stone	if ((column+1 == 18) || 
//~ (cascading)		(board->mech.state[column+1 +1][row].colour == board->mech.state[column+1][row].colour))

//~ opp_stone	if (column < 18)
				//~ if (board->mech.state[column+1][row].colour != board->mech.state[column][row].colour 
					//~ && board->mech.state[column+1][row].colour != empty) 
					






	
						 

void play_move (int column, int row, playing_parts *parts);
void undo_move (struct board *p, struct board **infocus, struct message *text, bool branching);

void group_stuff (int column, int row, struct board *board);
void undo_groups (int column, int row, struct board *board);
void capture_group (struct board *board, struct group *group);

void check_adjacent_stones (enum stone_codes, void (*f)(int, int, struct board *), int column, int row, struct board *board);
void set_outfacing (int column, int row, struct board *board);

struct board* add_board (int *n_boards, struct board **infocus, scaling scale, struct board **list, struct list_lines **list_lines);
struct board *split_board (int *n_boards, int moveNum, playing_parts *parts, struct message *text, struct board **infocus, struct board **list, struct list_lines **list_lines, scaling scale);
void delete_board (struct board *p, int *n_boards, struct opted **sel, struct board **infocus, struct board **list, struct list_lines **list_lines);
void select_board (struct board *p, struct opted **sel);
void shift_elements (struct opted *sel, SDL_Event *event, SDL_MouseButtonEvent *pan_start, scaling scale);
