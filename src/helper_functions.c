#include "helper_functions.h"











void place_stone (int column, int row, struct board *board, SDL_Texture *stone) {						
	
	SDL_Rect stoneSize = { ((column*SQUARE_SIZE + BORDER) - 15), ((row*SQUARE_SIZE + BORDER) - 15), STONE_SIZE, STONE_SIZE};	
	SDL_SetRenderTarget (renderer, board->rep.snap);
	SDL_RenderCopy(renderer, stone, NULL, &stoneSize);
	SDL_SetRenderTarget (renderer, NULL);
}




						//printing a number on a placed stone.
void put_number (int column, int row, playing_parts *parts) {

		
	char *buffer = malloc(5);
	buffer[5] = '\0';
	sprintf (buffer, "%d", parts->number);
	SDL_Surface *stoneNo_surface = TTF_RenderText_Solid(parts->font, buffer, parts->font_color);
	SDL_Texture *stoneNo_texture = SDL_CreateTextureFromSurface(renderer, stoneNo_surface);
	free(buffer);	


	int texW, texH;
	int x_offset;	//since the coordinates align with the top left of the text
	SDL_QueryTexture(stoneNo_texture, NULL, NULL, &texW, &texH);
	if (parts->number < 10)
		x_offset = 6;
	else x_offset = 11;
	SDL_Rect stoneNo_rect = { ((column*SQUARE_SIZE + BORDER) - x_offset), ((row*SQUARE_SIZE + BORDER) - 9), texW, texH };

	
	SDL_SetRenderTarget (renderer, parts->board->rep.snap);
	SDL_RenderCopy(renderer, stoneNo_texture, NULL, &stoneNo_rect);
	SDL_FreeSurface(stoneNo_surface);
	SDL_DestroyTexture(stoneNo_texture);
	SDL_SetRenderTarget (renderer, NULL);
}





void print_liberties (playing_parts *parts) {
	
	//~ if (!parts->board->groups)
		//~ return;
				//resetting 
	SDL_SetTextureBlendMode(parts->board->liberties, SDL_BLENDMODE_BLEND);	//colouring a part of the texture transparent. 
	SDL_SetRenderTarget (renderer, parts->board->liberties);				
	SDL_SetRenderDrawColor (renderer, 0, 0, 0, 0);
	SDL_RenderFillRect (renderer, NULL);
	SDL_SetRenderTarget (renderer, NULL);
	
	
	for (struct group *walk = parts->board->groups; walk; walk = walk->next) {
		
		if (walk->colour == b) {
			parts->font_color.r = 255; parts->font_color.g = 255; parts->font_color.b = 0;
		}
		else {
			parts->font_color.r = 57; parts->font_color.g = 255; parts->font_color.b = 20;
		}
		
		parts->number = walk->number;
		
		for (struct liberty *stroll = walk->liberties; stroll; stroll = stroll->next) 
			put_liberty (stroll->coord.y, stroll->coord.x, parts);
	}
}



void put_liberty (int column, int row, playing_parts *parts) {

	
		
	char *buffer = malloc(5);
	buffer[5] = '\0';
	sprintf (buffer, "%d", parts->number);
	SDL_Surface *stoneNo_surface = TTF_RenderText_Solid(parts->font, buffer, parts->font_color);
	SDL_Texture *stoneNo_texture = SDL_CreateTextureFromSurface(renderer, stoneNo_surface);
	free(buffer);	


	int texW, texH;
	int x_offset;	//since the coordinates align with the top left of the text
	SDL_QueryTexture(stoneNo_texture, NULL, NULL, &texW, &texH);
	if (parts->number < 10)
		x_offset = 6;
	else x_offset = 11;
	SDL_Rect stoneNo_rect = { ((column*SQUARE_SIZE + BORDER) - x_offset), ((row*SQUARE_SIZE + BORDER) - 9), texW, texH };

	
	SDL_SetRenderTarget (renderer, parts->board->liberties);
	SDL_RenderCopy(renderer, stoneNo_texture, NULL, &stoneNo_rect);
	SDL_FreeSurface(stoneNo_surface);
	SDL_DestroyTexture(stoneNo_texture);
	SDL_SetRenderTarget (renderer, NULL);
}





void coords_from_mouse (SDL_Event event, struct board *board, int *column, int *row, double scale_amount) {
	
	double x, y;
	
	x 	= 	(event.button.x - (board->rep.size.x + BORDER*scale_amount)) / (SQUARE_SIZE*scale_amount); 
	y 	= 	(event.button.y - (board->rep.size.y + BORDER*scale_amount)) / (SQUARE_SIZE*scale_amount);
	
	
									
	if ((x - (int)x) >= 0.5)
		 *column = (int)x + 1;
	else *column = (int)x;
	
	if ((y - (int)y) >= 0.5)
		 *row = (int)y + 1;
	else *row = (int)y;
}





bool isin_box (SDL_Rect rect, SDL_MouseButtonEvent button) {
	
	if (!(rect.x < button.x  &&  button.x < (rect.x + rect.w)))
		return FALSE;
	if (!(rect.y < button.y  &&  button.y < (rect.y + rect.h)))
		return FALSE;
		
	return TRUE;
}


//~ void delete_SLL (void *p) {
	
	//~ void *to_delete = p;
	//~ void *stroll = p->next;
	
	//~ while (1) {
		//~ free(to_delete);
		//~ if (!stroll)
			//~ break;
		//~ to_delete = stroll;
		//~ stroll = stroll->next;
	//~ }
//~ }
		
		
