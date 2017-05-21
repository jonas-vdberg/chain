/* Copyright (c) 2017 Jonas van den Berg <jonas.vanen@gmail.com>
 * 
 * Chain is free software; you can redistribute it and/or modify
 * it under the terms of the MIT license. See LICENSE for details.
 */

#ifndef CHAIN_CHAIN_H
#define CHAIN_CHAIN_H

#include <stddef.h>
#include <string.h>

struct chain_node {
	size_t weight; /* Total length of the the following nodes. */
	size_t size; /* Length of this node's string. */
	const char *string;
	struct chain_node *next;
};

struct chain {
	struct chain_node *head;
	struct chain_node *tail;
};

#define chain_node_length(node) ((node) ? ((node)->weight + (node)->size) : 0)
#define chain_length(chain) chain_node_length(chain->head)

struct chain *chain_new(void);
void chain_free(struct chain *chain);

void chain_append_n(struct chain *chain, const char *string, size_t size);
#define chain_append(chain, string) \
	chain_append_n(chain, string, (string) ? strlen(string) : 0)

struct chain_node *chain_node_at(struct chain *chain, size_t index);

char *chain_report(struct chain *chain, size_t index, size_t size);

int chain_equal_n(struct chain *chain, const char *str,
		  size_t index, size_t num);

struct chain *chain_concat(struct chain *first, struct chain *last);

void chain_split(struct chain *chain, size_t index,
		 struct chain **first_ptr, struct chain **last_ptr);

struct chain *chain_delete(struct chain *chain, size_t index, size_t size);

#endif /* CHAIN_CHAIN_H */
