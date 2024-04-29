#include "common.h"

extern SDL_Window *window;
extern SDL_Renderer *renderer;
extern SDL_Surface *display;
extern SDL_Texture 	*texture, *bg_board, *selTex, *branchTex, *splitTex, *modeTex_undo, *blackStone, 
					*whiteStone, *ghost_blackStone, *ghost_whiteStone, *highlight_stone; 
extern TTF_Font *font;


int initialize_window(void);
void destroy_window(void);


//SDL_Window *window, SDL_Renderer *renderer, TTF_Font *font
//SDL_Window *window, SDL_Renderer *renderer, SDL_Texture *texture,  SDL_Surface *pngImg,  SDL_Surface *stoneSur, TTF_Font *font
