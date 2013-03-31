#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

struct Node {
	int val;
	struct Node *next;
};

struct Stack {
	struct Node *head;
};

struct Stack *stack_new() {
	struct Stack *stack = malloc(sizeof(struct Stack));
	stack->head = NULL;
	return stack;
}

void stack_free(struct Stack *stack) {
	if (stack) {
		struct Node *cur = stack->head;

		while (cur) {
			struct Node *next = cur->next;
			free(cur);
			cur = next;
		}
	}

	free(stack);
}

void stack_push(struct Stack *stack, int val) {
	struct Node *new_head = malloc(sizeof(struct Stack));

	new_head->val = val;
	new_head->next = stack->head;

	stack->head = new_head;
}

bool stack_is_empty(struct Stack *stack) {
	return stack->head == NULL;
}

bool stack_top(struct Stack *stack, int *val) {
	if (stack_is_empty(stack)) {
		return false;
	} else {
		*val = stack->head->val;
		return true;
	}
}

int stack_pop(struct Stack *stack) {
	if (stack_is_empty(stack)) {
		fputs("Error: Stack underflow\n", stderr);
		abort();
	} else {
		int val = stack->head->val;
		stack->head = stack->head->next;
		return val;
	}
}

int main(int argc, char *argv[]) {
	struct Stack *stack = stack_new();

	stack_push(stack, 0);
	stack_push(stack, 1);
	stack_push(stack, 2);
	stack_push(stack, 3);

	int top;

	if (stack_top(stack, &top)) {
		printf("stack_top: %d should be 3\n", top);
	} else {
		printf("stack_top: error: stack is empty\n");
		return 1;
	}

	for (int i = 0; i < 4; i++) {
		int val = stack_pop(stack);

		printf("stack_pop: %d should be %d\n", val, 3 - i);
	}

	stack_free(stack);

	return 0;
}
