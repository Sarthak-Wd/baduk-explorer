#ifndef COMMON_H
#define COMMON_H

#include <stdbool.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mouse.h>
#include "constants.h"
#include <SDL2/SDL2_gfxPrimitives.h>	



struct fract_coords {					//replace coordinate members down in the structs with this
	double x;
	double y;
};

struct whole_coords {
	int x;
	int y;
};


struct moves {									
	enum {empty, black, white} colour;
	int S_no;
	struct whole_coords board_coords;
};


struct board {
	
	struct {
		struct moves state[19][19];
		enum {black_s, white_s} turn;
		int total_moves;				//total moves, all of them, including the stones placed on all the boards above in the branch.
	} mech;
	
	struct {
		struct whole_coords center_off;		//displacement from the center (unscaled)
		SDL_Texture *snap;
		SDL_Rect size;
	} rep;								
};



struct  parent {
	struct list_lines *line;
	struct list *item;
	struct moves *last_move;
};
struct  spawn {
	struct list_lines *line;
	struct list *item;
	struct moves *first_move;
	
	struct spawn *next;
};


	


struct branch {
	struct board board;
	int number;			//probably don't need number. list needs it, but no use here, I think.
	//bool selected;		//maybe it's better if selected is in list. dk
	
	struct parent *above;			//Try removing this. The program can function w/o these, using the last board's total moves. 	
	struct spawn *below;			//Or even just declaring a separate variable for the last move. Will need to use a loop to get 
									// to the coordinates until I implement something other than an array for the board state.
	
	//struct list *next;			//what do I need next for when there is above & below->board? Think about this.
};




struct list {
	struct branch *node;
	int number;
	struct opted *selection;
	
	struct list *prev;
	struct list *next;
};


struct list_lines {
	int number;
	struct board *start_board;				//need these to adjust the lines when zooming in or out.
	struct board *end_board;
	
	struct fract_coords start;
	struct fract_coords end;
	
	struct list_lines *prev;
	struct list_lines *next;
};


struct opted {
	struct list *list;
	struct opted *prev;			//why does this need a prev? to adjust if a selection is unselected.
	struct opted *next;			
};







struct delete_list {
	struct list *todelete;
	struct delete_list *next;
};



struct message {
	struct fract_coords coord;
	char *str;
	SDL_Color bg_color;
	SDL_Color txt_color;
};


#endif







