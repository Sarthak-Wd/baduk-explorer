#ifndef PLAY_STATE
#define PLAY_STATE

#include "common.h"
#include "init.h"
#include "helper_functions.h"
#include "object_functions.h"
#include "main.h"

enum stone_codes {no_condition, liberties, ally_stone, opp_stone, group_match};


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

struct member_info {
	bool merge;
	int S_no_on_board;
};


	
						 

void play_move (int column, int row, playing_parts *parts);
void undo_move (struct board *p, struct board **infocus, struct message *text, bool branching, playing_parts *parts);


void group_stuff (int column, int row, struct board *board);
void undo_groups (int column, int row, struct board *board, playing_parts *parts);
void capture_group (struct board *board, struct group *group);



void check_adjacent_spots (enum stone_codes, void (*f)(int, int, struct group_op_data*), int column, int row, struct group_op_data *d);
void set_outfacing 				(int column, int row, struct group_op_data *d);
void add_uncommonLiberties 		(int column, int row, struct group_op_data *d);
void remove_uncommonLiberties 	(int column, int row, struct group_op_data *d);
void addback_oppLiberties 		(int column, int row, struct group_op_data *d);
void divide_group 				(int column, int row, struct group_op_data *d);
void add_toGroup 				(int column, int row, struct group_op_data *d);
void evaluate_group 			(int column, int row, struct group_op_data *d);
void remove_oppLiberties 		(int column, int row, struct group_op_data *d);
void get_adj_ally_groups 		(int column, int row, struct group_op_data *d);
void get_adj_opp_groups 		(int column, int row, struct group_op_data *d);
void check_and_set_outfacing 	(int column, int row, struct group_op_data *d);


void setup_mode (menu_textures *turn_menu, struct board *list, struct list_lines *list_lines, scaling *scale);


#endif
