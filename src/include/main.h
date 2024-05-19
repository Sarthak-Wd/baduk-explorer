#include <stdio.h>

#include "common.h"
#include "init.h"

#include "camera.h"
#include "helper_functions.h"
#include "play_state.h"
#include "inter-board.h"
#include "object_functions.h"
#include "saving.h"


void load_setup (void);
bool process_input(void);
void render (struct board *p);

void place_objects_on_buffer (struct board *p);


void show_err (struct message *text);
void display_text (struct message text, TTF_Font *font);
void ticker (void);
void testing (void);
void inspect_board (struct board p);
void inspect_groups (struct board q);
