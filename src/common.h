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


struct move {									
	enum {empty, black, white} colour;
	int S_no;
	//struct whole_coords board_coords;	//what puspose does this serve exactly? There already exists an array, which has its members, this very structure?
};


struct  spawn {
	struct board *board;
	struct spawn *next;
};


struct stone {	
	enum {empt, blck, whte} colour;
	int S_no;
	
	int column;
	int row;
};	





struct board {		
		
	struct {		
		struct move state[19][19];	
		enum {black_s, white_s} turn;	
		int total_moves;				//total moves, all of them, including the stones placed on all the boards above in the branch.
	} mech;		
			
	
	int number;	
		
	struct stone *first_move;		
	struct stone *last_move;		
	struct list_lines *line;		
	
	struct opted *selection;	//what is this for? shouldn't it be just a bool? No, to delete a selection, I'll need it.	
	
			
	struct {		
		struct whole_coords center_off;		//displacement from the center (unscaled)	
		SDL_Texture *snap;	
		SDL_Rect size;	
	} rep;		
			
			
	struct board *above_board;		
	struct spawn *below;		
			
	struct board *prev;		
	struct board *next;		
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
	double amount;
	struct fract_coords center;
} scaling;





#endif







