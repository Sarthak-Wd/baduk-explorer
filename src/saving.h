#include "common.h"
#include "helper_functions.h"
#include "object_functions.h"


								//for reading moves
struct read_move {
	struct whole_coords coord;
	int colour;
	int S_no;
	struct read_move *next;
};

struct list_moves {
	int board_number;
	struct read_move *moves;
	struct list_moves *next;
};

void write_save (struct board *first_board, int n_boards, scaling *scale, bool new);
void load_save (struct board **list, struct list_lines **list_lines, struct board **first_board, int *n_boards, scaling *scale, playing_parts *parts);
void recur_load_moves (struct board *board, struct list_moves *list_moves, struct board *list, playing_parts *parts);
