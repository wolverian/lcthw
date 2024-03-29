#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "object.h"
#include <assert.h>

void Object_destroy(void *self) {
	assert(self != NULL);

	Object *obj = self;

	if (obj) {
		if (obj->description) free(obj->description);
		free(obj);
	}
}

void Object_describe(void *self) {
	assert(self != NULL);

	Object *obj = self;
	printf("%s.\n", obj->description);
}

bool Object_init(void *self) {
	assert(self != NULL);

	// do nothing really
	return true;
}

void *Object_move(void *self, Direction direction) {
	assert(self != NULL);

	printf("You can't go that direction.\n");
	return NULL;
}

bool Object_attack(void *self, int damage) {
	assert(self != NULL);
	assert(damage >= 0);

	printf("You can't attack that.\n");
	return true;
}

void *Object_new(size_t size, Object proto, char *description) {
	assert(size >= 0);
	assert(description != NULL);

	// setup the default functions in case they aren't set
	if (!proto.init) proto.init = Object_init;
	if (!proto.describe) proto.describe = Object_describe;
	if (!proto.destroy) proto.destroy = Object_destroy;
	if (!proto.attack) proto.attack = Object_attack;
	if (!proto.move) proto.move = Object_move;

	// this seems weird, but we can make a struct of one size,
	// then point a different pointer at it to "cast" it
	Object *el = calloc(1, size);
	*el = proto;

	// copy the description over
	el->description = strdup(description);

	// initialize it with whatever init we were given
	if (!el->init(el)) {
		// looks like it didn't initialize properly
		el->destroy(el);
		return NULL;
	} else {
		// all done, we made an object of any type
		assert(el != NULL);
		return el;
	}
}
