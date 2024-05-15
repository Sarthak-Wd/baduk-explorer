#include "common.h"
#include "init.h"
#include "helper_functions.h"

enum stone_codes {no_condition, liberties, ally_stone, opp_stone, group_match};



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
					

struct group_op_data {					//data for group operations 
	struct board *board;
	struct group *group;
	struct member *member;
	struct whole_coords move_coord;  //coordinate of the move played/undone
	struct group *ally_groups[4];
	struct group *opp_groups[4];
	int n_allies;
	int n_opps;
	//~ enum colour;
	int *group_id;
};




	
						 

void play_move (int column, int row, playing_parts *parts);
void undo_move (struct board *p, struct board **infocus, struct message *text, bool branching);

void group_stuff (int column, int row, struct board *board);
void undo_groups (int column, int row, struct board *board);
void capture_group (struct board *board, struct group *group);

void check_adjacent_spots (enum stone_codes, void (*f)(int, int, struct group_op_data*), int column, int row, struct group_op_data *d);
void set_outfacing (int column, int row, struct group_op_data *d);
void remove_uncommonLiberties (int column, int row, struct group_op_data *d);
void addback_oppLiberties (int column, int row, struct group_op_data *d);
void divide_group (int column, int row, struct group_op_data *d);
void add_toGroup (int column, int row, struct group_op_data *d);
void evaluate_group (int column, int row, struct group_op_data *d);

struct board* add_board (int *n_boards, struct board **infocus, scaling scale, struct board **list, struct list_lines **list_lines);
struct board *split_board (int *n_boards, int moveNum, playing_parts *parts, struct message *text, struct board **infocus, struct board **list, struct list_lines **list_lines, scaling scale);
void delete_board (struct board *p, int *n_boards, struct opted **sel, struct board **infocus, struct board **list, struct list_lines **list_lines);
void select_board (struct board *p, struct opted **sel);
void shift_elements (struct opted *sel, SDL_Event *event, SDL_MouseButtonEvent *pan_start, scaling scale);
