#define _GNU_SOURCE

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

#include "hashset.h"

// This variable is global so that I'm able to use my hash set iterate
// function without breaking the signature
int file_num = 2;

enum return_codes {
	SUCCESS = 0,
	INVOCATION_ERROR = 1,
	FILE_ERROR = 2,
	MEMORY_ERROR = 3
};

static struct {
	bool vertical_print;
	bool punctuation_sensitive;
} options = {false, false};

hash_set *load_words(FILE * fo);
void compare_words(hash_set * set, FILE * fo, size_t file_num);
void default_print(hash_node *node);

int main(int argc, char *argv[])
{
	int opt;
	while ((opt = getopt(argc, argv, "ai")) != -1) {
		switch (opt) {
		case 'a':
			options.vertical_print = true;
			break;
		case 'i':
			options.punctuation_sensitive = true;
			break;
		case '?':
			return(INVOCATION_ERROR);
		}
	}
	argc -= optind;
	argv += optind;

	if (argc < 2) {
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

	while (file_num < argc) {
		FILE *fo = fopen(argv[file_num], "r");
		compare_words(set, fo, file_num - 2);
		fclose(fo);
		++file_num;
	}
	hash_set_to_sorted_list(set);
	hash_node *node = NULL;
	hash_set_iterate(set, default_print);
	/*int i = 0;
	while (i < set->size) {
		if (set->table[i] != NULL) {
			// Safe to cast file_num as unsigned, as it will never
			// be less than 0
			if (set->table[i]->counter == (unsigned)file_num - 2) {
				printf("word[%u] == %s[c:%u]", i,
				       set->table[i]->word,
				       set->table[i]->counter);
				if (set->table[i]->alt_next != NULL) {
					node = set->table[i]->alt_next;
					while (node != NULL) {
						printf(", alt: %s", node->word);
						node = node->alt_next;
					}
				}
				putchar('\n');
			}
		}
		++i;
	}*/
	hash_set_destroy(set);

	return (SUCCESS);
}

hash_set *load_words(FILE * fo)
{
	hash_set *set = hash_set_create();
	if (!set) {
		fprintf(stderr, "Memory allocation error,");
		exit(MEMORY_ERROR);
	}
	char *line_buf = NULL;
	size_t buf_size = 0;
	while (getline(&line_buf, &buf_size, fo) != -1) {
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
	return (set);
}

void compare_words(hash_set * set, FILE * fo, size_t file_count)
{
	char *line_buf = NULL;
	size_t buf_size = 0;
	while (getline(&line_buf, &buf_size, fo) != -1) {
		if (line_buf[0] == '\n') {
			continue;
		}
		char *current_word = strtok(line_buf, " \t\n\v\f\r");
		if (current_word != NULL) {
			hash_set_add_alts(set, current_word, file_count);
		}
		while ((current_word = strtok(NULL, " \t\n\v\f\r")) != NULL) {
			hash_set_add_alts(set, current_word, file_count);
		}
	}
	if (line_buf) {
		free(line_buf);
	}
	return;
}

void default_print(hash_node *node)
{
	// file_num will never be negative, so this is a safe cast
	if (node->counter == (unsigned) file_num - 2) {
		printf("%s\n", node->word);
	}
	return;
}