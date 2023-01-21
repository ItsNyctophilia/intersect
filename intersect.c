#define _GNU_SOURCE

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

#include "hashset.h"

// This variable is global so that I'm able to use my hash set iterate
// function without breaking the signature
int file_num = 0;

enum return_codes {
	SUCCESS = 0,
	INVOCATION_ERROR = 1,
	FILE_ERROR = 2,
	MEMORY_ERROR = 3
};

static struct {
	bool vertical_print;
	bool punctuation_insensitive;
} options = { false, false };

hash_set *load_words(FILE * fo);
void compare_words(hash_set * set, FILE * fo, size_t file_num);
void default_print(hash_node * node);
void vertical_print(hash_node * node);
void insensitive_print(hash_node * node);
void all_print(hash_node * node);

int main(int argc, char *argv[])
{
	int opt;
	while ((opt = getopt(argc, argv, "ai")) != -1) {
		switch (opt) {
		case 'a':
			options.vertical_print = true;
			break;
		case 'i':
			options.punctuation_insensitive = true;
			break;
		case '?':
			return (INVOCATION_ERROR);
		}
	}
	if (argc < 3) {
		fprintf(stderr, "Usage: %s [FILE]...\n", argv[0]);
		return (INVOCATION_ERROR);
	}

	argc -= optind;
	argv += optind;
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
	FILE *file_1 = fopen(argv[0], "r");
	hash_set *set = load_words(file_1);
	fclose(file_1);

	while (file_num < argc) {
		FILE *fo = fopen(argv[file_num], "r");
		// (file_num - 2) == zero-indexed value that is the current file
		// being processed
		compare_words(set, fo, file_num);
		fclose(fo);
		++file_num;
	}
	hash_set_to_sorted_list(set);
	if (options.vertical_print && options.punctuation_insensitive) {
		// "-ai"
		hash_set_iterate(set, all_print);
	} else if (options.vertical_print) {
		// "-a"
		hash_set_iterate(set, vertical_print);
	} else if (options.punctuation_insensitive) {
		// "-i"
		hash_set_iterate(set, insensitive_print);
	} else {
		// No options
		hash_set_iterate(set, default_print);
	}
	hash_set_destroy(set);

	return (SUCCESS);
}

hash_set *load_words(FILE * fo)
// Loads words from a given file (specifically the first )
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
// Compares the words found in every file after the first to the
// hash set. If an entry is found for a given word with a different
// capitalization or with different punctuation, a new entry is added,
// otherwise, the passed word is ignored.
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

void default_print(hash_node * node)
// Prints nodes as they were found in the first file that matched across
// all files.
{
	// file_num will never be negative, so this is a safe cast
	hash_node *root = node;
	if (node->counter == (unsigned)file_num) {
		printf("%s\n", node->word);
		if (node->alt_next != NULL) {
			node = node->alt_next;
			while (node != NULL) {
				if (!(strcasecmp(root->word, node->word) == 0)) {
					printf("%s\n", node->word);
				}
				node = node->alt_next;
			}
		}
	}
	return;
}

void vertical_print(hash_node * node)
// Prints nodes as they were found in the first file at the start of each
// line, followed by all alternate-case versions of that word in the order
// that they appeared in each file.
{
	// file_num will never be negative, so this is a safe cast
	hash_node *root = node;
	if (node->counter == (unsigned)file_num) {
		printf("%s", node->word);
		if (node->alt_next != NULL) {
			node = node->alt_next;
			while (node != NULL) {
				printf(" %s", node->word);
				node = node->alt_next;
			}
		}
		putchar('\n');
	}
	node = root;
	return;
}

void insensitive_print(hash_node * node)
// Prints exclusively the topmost node (first found) for all nodes found
// across every file, excluding punctuation-only words.
{
	hash_node *root = node;
	if (node->counter == (unsigned)file_num) {
		if (strpbrk
		    (node->word,
		     "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ") !=
		    NULL) {
			printf("%s\n", node->word);
		}
	}
	node = root;
	return;
}

void all_print(hash_node * node)
// Prints nodes as they were found in the first file, matched across all
// files, with different-case duplicates found on the same line and 
// trailing/leading punctuation words that are overwise the same omitted.
{
	size_t old_psl = 0;	// psl == "punctuation start length"

	hash_node *root = node;
	for (size_t i = 0; i < strlen(root->word); ++i) {
		if (ispunct(root->word[i])) {
			++old_psl;
			continue;
		}
		break;
	}
	int old_span = strspn(root->word,
			      "abcdefghijklmnopqrstuvwxyzABCEDFGHIJKLMNOPQRSTUVWXYZ");
	if (node->counter == (unsigned)file_num) {
		printf("%s", node->word);
		if (node->alt_next != NULL) {
			node = node->alt_next;
			while (node != NULL) {
				size_t new_psl = 0;
				for (size_t i = 0; i < strlen(node->word); ++i) {
					if (ispunct(node->word[i])) {
						++new_psl;
						continue;
					}
					break;
				}
				int new_span = strspn(node->word,
						      "abcdefghijklmnopqrstuvwxyzABCEDFGHIJKLMNOPQRSTUVWXYZ");
				if (strncmp
				    (root->word + old_psl, node->word + new_psl,
				     new_span >
				     old_span ? new_span : old_span) != 0) {
					printf(" %s", node->word);
				}
				node = node->alt_next;
			}
		}
		putchar('\n');
	}
	node = root;
	return;
}
