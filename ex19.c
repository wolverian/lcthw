#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include "ex19.h"
#include "game.h"

Object MapProto = {
	.init = Map_init,
	.move = Map_move,
	.attack = Map_attack
};

int main(int argc, char *argv[]) {
	// simple way to set up randomness
	srand(time(NULL));

	Map *game = NEW(Map, "The Hall of the Minotaur.");

	printf("You enter the ");
	game->location->_(describe)(game->location);

	while (process_input(game)) {}

	return 0;
}
