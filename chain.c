/* Copyright (c) 2017 Jonas van den Berg <jonas.vanen@gmail.com>
 * 
 * Chain is free software; you can redistribute it and/or modify
 * it under the terms of the MIT license. See LICENSE for details.
 */

#include <stdlib.h>

#include "chain.h"
#include "ealloc.h"

struct chain *chain_new(void)
{
	struct chain *chain = ecalloc(1, sizeof(struct chain));
	return chain;
}

void chain_free(struct chain *chain)
{
	if (!chain)
		return;
	struct chain_node *current = chain->head;
	while (current) {
		struct chain_node *next = current->next;
		free(current);
		current = next;
	}
	free(chain);
}

struct chain_node *chain_node_at(struct chain *chain, size_t index)
{
	if (!chain->head)
		return NULL;

	struct chain_node *current = chain->head;
	while (index < current->weight)
		current = current->next;
	if (index - current->weight >= current->size)
		return NULL;

	return current;
}

void chain_append_n(struct chain *chain, const char *string, size_t size)
{
	struct chain_node *node = emalloc(1, sizeof(struct chain_node));
	node->weight = chain_length(chain);
	node->size = size;
	node->string = string;
	node->next = chain->head;
	chain->head = node;
	if (!chain->tail)
		chain->tail = node;
}

char *chain_report(struct chain *chain, size_t index, size_t size)
{
	if (!size || index + size > chain_length(chain))
		return NULL;

	char *string = emalloc(size + 1, sizeof(char));
	string[size] = 0;

	struct chain_node *current = chain_node_at(chain, index + size - 1);
	while (size) {
		if (size <= current->weight)
			current = current->next;
		size_t i = index > current->weight ? (index - current->weight) : 0;
		size_t s = size + index - current->weight - i;
		strncpy(string + (size -= s), current->string + i, s);
	}

	return string;
}

int chain_equal_n(struct chain *chain, const char *str,
		  size_t index, size_t num)
{
	if (!num || index + num > chain_length(chain))
		return 0;

	size_t i = index + num - 1;
	struct chain_node *current = chain_node_at(chain, i);
	while (1) {
		if (i < current->weight)
			current = current->next;
		if (current->string[i - current->weight] != str[i - index])
			return 0;
		if (i-- <= index)
			break;
	}

	return 1;
}

struct chain *chain_concat(struct chain *first, struct chain *last)
{
	if (!last->head) {
		free(last);
		return first;
	}
	if (!first->head) {
		free(first);
		return last;
	}

	size_t length = chain_length(first);
	struct chain_node *current = last->head;
	while (current) {
		current->weight += length;
		current = current->next;
	}

	last->tail->next = first->head;
	free(first);

	return last;
}

void chain_split(struct chain *chain, size_t index,
		 struct chain **first_ptr, struct chain **last_ptr)
{
	if (index >= chain_length(chain)) {
		*first_ptr = chain;
		*last_ptr = ecalloc(1, sizeof(struct chain));
		return;
	}

	struct chain_node *target = chain_node_at(chain, index);
	index -= target->weight;

	if (!index) {
		struct chain *first = emalloc(1, sizeof(struct chain));
		first->head = target->next;
		first->tail = chain->tail;

		target->next = NULL;
		chain->tail = target;

		struct chain_node *current = chain->head;
		while (current) {
			current->weight -= target->weight;
			current = current->next;
		}

		*first_ptr = first;
		*last_ptr = chain;
	}
	else {
		struct chain *first = emalloc(1, sizeof(struct chain));
		struct chain_node *node = emalloc(1, sizeof(struct chain_node));
		node->string = target->string;
		node->size = index;
		node->next = target->next;
		node->weight = target->next ?
			target->next->weight + target->next->size : 0;

		first->head = node;
		first->tail = chain->tail;

		target->string += index;
		target->size -= index;
		target->next = NULL;
		chain->tail = target;

		struct chain_node *current = chain->head;
		while (current) {
			current->weight -= (target->weight + index);
			current = current->next;
		}

		target->weight = 0;

		*first_ptr = first;
		*last_ptr = chain;
	}
}

struct chain *chain_delete(struct chain *chain, size_t index, size_t size)
{
	struct chain *first_1, *first_2, *last_1, *last_2;
	chain_split(chain, index, &first_1, &last_1);
	chain_split(chain, size, &first_2, &last_2);
	chain_free(first_2);
	return chain_concat(first_1, last_2);
}
