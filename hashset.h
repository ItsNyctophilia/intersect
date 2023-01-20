#ifndef HASH_SET_H
#define HASH_SET_H

typedef struct hash_node{
	char *word;
	unsigned counter;
	struct hash_node *alt_next;
	struct hash_node *next;
} hash_node;

typedef struct hash_set{
	hash_node **table;
	int size;
	int num_words;
} hash_set;

hash_set *hash_set_create(void);

void hash_set_add_word(hash_set *set, const char *word);

int hash_set_contains_word(hash_set *set, const char *word);

void hash_set_destroy(hash_set *set);

#endif