#define _GNU_SOURCE

#include <stdio.h>
#include <stdbool.h>

#include "hashset.h"

enum return_codes {
	SUCCESS = 0,
	INVOCATION_ERROR = 1,
	FILE_ERROR = 2,
	MEMORY_ERROR = 3
};

hash_set *load_words(FILE *fo);
void compare_words(hash_set *set, FILE *fo, size_t file_num);

int main(int argc, char *argv[])
{
	if (argc < 3) {
		fprintf(stderr, "Usage: %s [FILE]...\n", argv[0]);
		return (INVOCATION_ERROR);
	}
	// Validates that given files are able to be opened; sourced from ws.c
	bool close_flag = false;
	for (int i = 1; i < argc; ++i) {
		FILE *fo = fopen(argv[i], "r");
		if (!fo) {	// If file could not be opened for reading
			close_flag = true;	// Close after all files checked
			fprintf(stderr, "%s could not be opened", argv[i]);
			perror(" \b");	// Backspace to format perror string
		} else {
			fclose(fo);
		}
	}
	if (close_flag == true) {
		return (INVOCATION_ERROR);
	}
	FILE *file_1 = fopen(argv[1], "r");
	hash_set *set = load_words(file_1);
	fclose(file_1);

	int file_num = 2;
	while(file_num < argc) {
		FILE *fo = fopen(argv[file_num], "r");
		compare_words(set, fo, file_num - 2);
		fclose(fo);
		++file_num;
	}
	hash_node *node = NULL;

	int i = 0;
	while(i < set->size) {
		if (set->table[i] != NULL) {
			// Safe to cast file_num as unsigned, as it will never
			// be less than 0
			if(set->table[i]->counter == (unsigned) file_num - 2) {
				printf("word[%u] == %s", i, set->table[i]->word);
				if (set->table[i]->alt_next != NULL) {
					node = set->table[i]->alt_next;
					while(node != NULL) {
						printf(", alt: %s", node->word);
						node = node->alt_next;
					}
				}
				putchar('\n');
			}
		}
		++i;
	}
	hash_set_destroy(set);

	return (SUCCESS);
}

hash_set *load_words(FILE *fo)
{
	hash_set *set = hash_set_create();
	if (!set) {
		fprintf(stderr, "Memory allocation error,");
		exit(MEMORY_ERROR);
	}
	char *line_buf = NULL;
	size_t buf_size = 0;
	while(getline(&line_buf, &buf_size, fo) != -1) {
		if (line_buf[0] == '\n') {
			continue;
		}
		char *current_word = strtok(line_buf, " \t\n\v\f\r");
		if (current_word != NULL) {
			hash_set_add_word(set, current_word);
		}
		while ((current_word = strtok(NULL, " \t\n\v\f\r")) != NULL) {
			hash_set_add_word(set, current_word);
		}
	}
	if (line_buf) {
		free(line_buf);
	}
	return(set);
}

void compare_words(hash_set *set, FILE *fo, size_t file_num)
{
	char *line_buf = NULL;
	size_t buf_size = 0;
	while(getline(&line_buf, &buf_size, fo) != -1) {
		if (line_buf[0] == '\n') {
			continue;
		}
		char *current_word = strtok(line_buf, " \t\n\v\f\r");
		if (current_word != NULL) {
			hash_set_add_alts(set, current_word, file_num);
		}
		while ((current_word = strtok(NULL, " \t\n\v\f\r")) != NULL) {
			hash_set_add_alts(set, current_word, file_num);
		}
	}
	if (line_buf) {
		free(line_buf);
	}
	return;
}
