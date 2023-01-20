#ifndef HASH_SET_H
#define HASH_SET_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct hash_node {
	char *word;
	unsigned counter;
	struct hash_node *alt_next;
	struct hash_node *next;
} hash_node;

typedef struct hash_set {
	hash_node **table;
	int size;
	int num_words;
} hash_set;

hash_set *hash_set_create(void);

void hash_set_add_word(hash_set * set, const char *word);

void hash_set_add_alts(hash_set * set, const char *word, size_t file_num);

void hash_set_to_sorted_list(hash_set * set);

void hash_set_iterate(hash_set *set, void (*function)(hash_node *node));

void hash_set_destroy(hash_set * set);

#endif
