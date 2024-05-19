#include "include/init.h"



SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
SDL_Surface *display = NULL;
SDL_Texture *texture = NULL, *bg_board = NULL, 
			*selTex = NULL, *branchTex = NULL, *splitTex = NULL, *modeTex_undo = NULL,
			*blackStone = NULL, *whiteStone = NULL, *ghost_blackStone = NULL, *ghost_whiteStone = NULL, 
			*highlight_stone = NULL, *alternate_turn_black = NULL, *alternate_turn_white = NULL; 
TTF_Font *font = NULL, *big_font = NULL;




int initialize_window (void) {
	
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)  {
		fprintf(stderr, "Error initializing SDL.\n");
		return FALSE;
	}
	
	if (IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG)  {
		fprintf(stderr, "Error initializing SDL_Image.\n");
		return FALSE;
	}
	
	if ( TTF_Init() < 0 ) {
	fprintf(stderr, "Error initializing SDL_ttf.\n");
		return FALSE;
	}
	font = TTF_OpenFont("fonts/FreeMonoBold.ttf", 20);
		if (font == NULL)	{
			printf ("Font could not be loaded.\n");
			return FALSE;
		}
		
	big_font = TTF_OpenFont("fonts/FreeMonoBold.ttf", 50);
		if (font == NULL)	{
			printf ("Font could not be loaded.\n");
			return FALSE;
		}
		
		printf ("%s", SDL_GetError());
	
	window = SDL_CreateWindow(
		"Helluewlu",
		SDL_WINDOWPOS_CENTERED,  
		SDL_WINDOWPOS_CENTERED,
		WINDOW_WIDTH,
		WINDOW_HEIGHT,
		SDL_WINDOW_SHOWN         
	);	
	if (!window) {
		fprintf (stderr, "Error creating SDL Window.\n");
		return FALSE;
	}
	

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_TARGETTEXTURE);
	if (!renderer) {
		fprintf (stderr, "Error creating SDL Renderer.\n");
		return FALSE;
	}
	
	return TRUE;
}



void destroy_window (void) {
	
	TTF_CloseFont(font);
	
	SDL_DestroyTexture(texture);
	SDL_DestroyTexture(blackStone);
	SDL_DestroyTexture(whiteStone);
	SDL_FreeSurface(display);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();      //inverse of SDL_Init, in a way
}
