#include <stdio.h>
#include <stdbool.h>

#include "hashset.h"

enum return_codes {
	SUCCESS = 0,
	INVOCATION_ERROR = 1,
	FILE_ERROR = 2,
	MEMORY_ERROR = 3
};



int main(int argc, char *argv[])
{
	if (argc < 3) {
		fprintf(stderr, "Usage: %s [FILE]...\n", argv[0]);
		return(INVOCATION_ERROR);
	} 

	// Validates that given files are able to be opened; sourced from ws.c
	bool close_flag = false;
	for (int i = 1; i < argc; ++i) {
		FILE *fo = fopen(argv[i], "r");
		if (!fo) {	// If file could not be opened for reading
			close_flag = true;	// Close after all files checked
			fprintf(stderr, "%s could not be opened",
				argv[i]);
			perror(" \b");	// Backspace to format perror string
		} else {
			fclose(fo);
		}
	}
	if (close_flag == true) {
		return (INVOCATION_ERROR);
	}

	hash_set *set = hash_set_create();
	hash_set_add_word(set, "test");
	hash_set_add_word(set, "TEST");
	//hash_set_add_alts(set, "TEST", 1);
	hash_set_add_alts(set, "TeSt", 1);
	hash_set_add_word(set, "new_word");
	hash_set_add_alts(set, "new_WORD", 1);
	printf("num_word = %d\n", set->num_words);
	int i = 0;
	hash_node *node = NULL;
	while(i < set->size) {
		if (set->table[i] != NULL && set->table[i]->counter == 1) {
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
		++i;
	}
	hash_set_destroy(set);

	return(SUCCESS);
}
