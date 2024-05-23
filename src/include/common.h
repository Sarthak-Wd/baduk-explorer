#ifndef COMMON_H
#define COMMON_H

#include <stdbool.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mouse.h>
#include "constants.h"
#include "init.h"
#include <SDL2/SDL2_gfxPrimitives.h>	








struct fract_coords {					//replace coordinate members down in the structs with this
	double x;
	double y;
};

struct whole_coords {
	int x;
	int y;
};






struct  spawn {
	struct board *board;
	struct spawn *next;
};


struct stone {	
	enum {empt, blck, whte} colour;		//why is there empty?
	int S_no;
	
	int column;
	int row;
};	


struct move {									
	enum {empty, black, white} colour;
	int S_no;
	struct group *group;
};





struct board {		
		
	int number;	
		
	struct stone *first_move;		
	struct stone *last_move;		
	struct list_lines *line;		
	
	struct group *groups;
	struct group *captured_groups;
	int num_groups;				//is this needed?

	struct opted *selection;	//what is this for? shouldn't it be just a bool? No, to delete a selection, I'll need it.	
	
			
	struct {		
		struct move state[19][19];	
		enum turn {black_s, white_s} turn;	
		int total_moves;				//total moves, all of them, including the stones placed on all the boards above in the branch.
	} mech;			
			
	struct {		
		struct whole_coords center_off;		//displacement from the center (unscaled)	
		SDL_Texture *snap;	
		SDL_Rect size;	
	} rep;		
		
	SDL_Texture *liberties;	
			
	struct board *above_board;		
	struct spawn *below;		
			
	struct board *prev;		
	struct board *next;		
};			



struct list_lines {
	int number;
	struct board *start_board;				//need these to adjust the lines when zooming in or out.
	struct board *end_board;				//I don't think I need these anymore.
	
	struct fract_coords start;
	struct fract_coords end;
	
	struct list_lines *prev;
	struct list_lines *next;
};










struct liberty {
	struct whole_coords coord;
	struct liberty *next;
};

struct member {
	struct whole_coords coord;
	bool outfacing;
	
	bool merge;
	int S_no_on_board; 	//this needs to be preserved.

	struct member *next;
};

struct group {
	
	int number;
	enum {b, w} colour;
	struct liberty *liberties;
	struct member *members;
	
	int capturing_move_S_no;			//only if this a capturing move 
	
	struct group *next;
};












struct opted {
	struct board *board;
	struct opted *prev;			//why does this need a prev? to adjust if a selection is unselected.
	struct opted *next;			
};



struct delete_list {				//what for?
	struct board *todelete;
	struct delete_list *next;
};



struct message {
	struct fract_coords coord;
	char *str;
	SDL_Color bg_color;
	SDL_Color txt_color;
	bool to_display;
};




typedef struct  {
	SDL_Texture *blackStone;
	SDL_Texture *whiteStone;
	SDL_Texture *ghost_b;
	SDL_Texture *ghost_w;
	
	int number;
	TTF_Font *font;
	SDL_Color font_color;
	
	struct board *board;
} playing_parts;


typedef struct {
	SDL_Texture *setup_moves_menu;
	SDL_Texture *alt_turn_black;
	SDL_Texture *alt_turn_white;
	SDL_Texture *turn_black;
	SDL_Texture *turn_white;
	SDL_Rect alt_turn_button;
	SDL_Rect black_turn_button;
	SDL_Rect white_turn_button;
	SDL_Rect alt_turn_position;
	SDL_Rect black_turn_position;
	SDL_Rect white_turn_position;	
} menu_textures;


typedef struct {
	double amount;
	struct fract_coords center;
} scaling;


struct moveslist {
	struct stone *stone;
	struct moveslist *next;
};



#endif







