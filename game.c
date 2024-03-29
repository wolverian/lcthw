#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include "ex19.h"

bool Monster_attack(void *self, int damage) {
	assert(self != NULL);
	assert(damage >= 0);

	Monster *monster = self;

	printf("You attack %s!\n", monster->_(description));

	monster->hit_points -= damage;

	if (monster->hit_points > 0) {
		printf("It is still alive.\n");
		return false;
	} else {
		printf("It is dead!\n");
		return true;
	}
}

bool Monster_init(void *self) {
	assert(self != NULL);

	Monster *monster = self;
	monster->hit_points = 10;
	return true;
}

Object MonsterProto = {
	.init = Monster_init,
	.attack = Monster_attack
};

void *Room_move(void *self, Direction direction) {
	assert(self != NULL);

	Room *room = self;
	Room *next = NULL;

	if (direction == NORTH && room->north) {
		printf("You go north, into:\n");
		next = room->north;
	} else if (direction == SOUTH && room->south) {
		printf("You go south, into:\n");
		next = room->south;
	} else if (direction == EAST && room->east) {
		printf("You go east, into:\n");
		next = room->east;
	} else if (direction == WEST && room->west) {
		printf("You go west, into:\n");
		next = room->west;
	} else {
		printf("You can't go that direction.");
		next = NULL;
	}

	if (next) {
		next->_(describe)(next);
		return next;
	} else {
		return NULL;
	}
}	

bool Room_attack(void *self, int damage) {
	assert(self != NULL);
	assert(damage >= 0);

	Room *room = self;
	Monster *monster = room->bad_guy;

	if (monster) {
		monster->_(attack)(monster, damage);
		return true;
	} else {
		printf("You flail in the air at nothing.\n");
		return false;
	}
}

Object RoomProto = {
	.move = Room_move,
	.attack = Room_attack
};

void *Map_move(void *self, Direction direction) {
	assert(self != NULL);

	Map *map = self;
	Room *location = map->location;
	Room *next = location->_(move)(location, direction);

	if (next) {
		map->location = next;
		return next;
	} else {
		return NULL;
	}
}

bool Map_attack(void *self, int damage) {
	assert(self != NULL);
	assert(damage >= 0);

	Map *map = self;
	Room *location = map->location;

	return location->_(attack)(location, damage);
}

bool Map_init(void *self) {
	assert(self != NULL);

	Map *map = self;

	// make some rooms for a small map
	Room *hall = NEW(Room, "The great hall");
	Room *throne = NEW(Room, "The throne room");
	Room *arena = NEW(Room, "The arena, with the minotaur");
	Room *kitchen = NEW(Room, "Kitchen, you have the knife now");

	// put the bad guy in the arena
	arena->bad_guy = NEW(Monster, "The evil minotaur");

	// set up the map rooms
	hall->north = throne;

	throne->west = arena;
	throne->east = kitchen;
	throne->south = hall;

	arena->east = throne;
	kitchen->west = throne;

	// start the map and the character off in the hall
	map->start = hall;
	map->location = hall;

	return true;
}

bool process_input(Map *game) {
	assert(game != NULL);

	printf("\n> ");

	char ch = getchar();
	getchar(); // eat enter

	int damage = rand() % 4;

	switch (ch) {
		case -1:
			printf("Giving up? :(\n");
			return false;
			break;
		case 'n':
			game->_(move)(game, NORTH);
			break;
		case 's':
			game->_(move)(game, SOUTH);
			break;
		case 'e':
			game->_(move)(game, EAST);
			break;
		case 'w':
			game->_(move)(game, WEST);
			break;
		case 'a':
			game->_(attack)(game, damage);
			break;
		case 'l':
			printf("You can go:\n");
			if (game->location->north) printf("NORTH\n");
			if (game->location->south) printf("SOUTH\n");
			if (game->location->east) printf("EAST\n");
			if (game->location->west) printf("WEST\n");
			break;
		default:
			printf("What?: %d\n", ch);
	}

	return true;
}

