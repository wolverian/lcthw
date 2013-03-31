#include <stdio.h>

void print_indexing(char *fmt, int len, char **names, int *ages) {
	for (int i = 0; i < len; i++) {
		printf(fmt, names[i], ages[i]);
	}
}

void print_pointing(char *fmt, int len, char **names, int *ages) {
	char **cur_name = names;
	int *cur_age = ages;
	for (int i = 0; i < len; i++) {
		printf(fmt, *(cur_name+i), *(cur_age+i));
	}
}

void print_pointing_again(char *fmt, int len, char **names, int *ages) {
	for (int i = 0; i < len; i++) {
		printf(fmt, names[i], ages[i]);
	}
}

void print_stupid_complex_way(char *fmt, int len, char **names, int *ages) {
	char **cur_name;
	int *cur_age;
	for (
		cur_name = names,
		cur_age = ages;
		(cur_age - ages) < len;
		cur_name++, cur_age++)
	{
		printf(fmt, *cur_name, *cur_age);
	}
}

int main(int argc, char *argv[]) {
	// create two arrays we care about
	int ages[] = {23, 43, 12, 89, 2};
	char *names[] = {
		"Alan", "Frank",
		"Mary", "John", "Lisa"
	};
	int count = 5;

	print_indexing("%s has %d years alive.\n", count, names, ages);
	printf("---\n");

	print_pointing("%s has %d years alive.\n", count, names, ages);
	printf("---\n");

	print_pointing_again("%s is %d years old again.\n", count, names, ages);
	printf("---\n");

	print_stupid_complex_way("%s is %d years old again.\n", count, names, ages);
	printf("---\n");

	return 0;
}
	
