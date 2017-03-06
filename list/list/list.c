#include <stdlib.h>
#include <assert.h>
#include "list.h"

/*
    list->head->previous == NULL;
    list->tail->next == list->head;
 */

struct dllist
{
	node_t *head;
	node_t *tail;
};


dllist_t *list_new()
{
	dllist_t *list = malloc(sizeof(dllist_t));
	list->head = NULL;
	list->tail = NULL;
	return list;
}

void list_delete(dllist_t *list)
{
	if (list == NULL)
	{
		return;
	}

	node_t *curr = list->head;
	if (curr == NULL)
	{
		free(list);
		return;
	}

	node_t *next = (list->head->next);
	while (curr != list->tail)
	{
		free(curr);
		curr = next;
		next = curr->next;
	}
	free(curr);
	free(list);
}

void list_initWithArray(dllist_t *list, const data_t *array, int n)
{
	assert(list && "Invalid argument: list is NULL");
	assert(n >= 0 && "Invalid argument: number of elements in array is less then zero");
	assert(!((array == NULL) && (n != 0)) && "Invalid arguments: array is NULL and n is not zero");

	if (list->head != NULL)
	{
		list_delete(list);
		list = list_new();
	}

	int    i;
	node_t *prev = NULL;
	node_t *curr = NULL;
	for (i = 0; i < n; i++)
	{
		curr = malloc(sizeof(node_t));
		curr->previous = prev;
		curr->value    = array[i];
		if (prev == NULL)
		{
			list->head = curr;
		}
		else
		{
			prev->next = curr;
		}
		prev = curr;
	}
	list->tail = curr;
	if (curr)
	{
		curr->next = list->head;
	}
}

void list_foreach(dllist_t *list, void (*func)(node_t *curr, void *param), void *param)
{
	if (list == NULL)
	{
		return;
	}
	if ((list->head == NULL) && (list->tail == NULL))
	{
		return;
	}
	assert(!((list->head == NULL) || (list->tail == NULL)) && "Invalid argument: list is not OK");

	node_t *node;
	for (node = list->head; node != list->tail; node = node->next)
	{
		func(node, param);
	}
	func(node, param);
}

void copyFunc(node_t *node, void *param)
{
	node_t *prev = *((node_t **) param);
	node_t *curr = malloc(sizeof(node_t));
	curr->previous = prev;
	curr->value    = node->value;
	if (prev)
	{
		prev->next = curr;
	}
	*((node_t **) param) = curr;
}

dllist_t *list_copy(dllist_t *list)
{
	dllist_t *copy = list_new();
	node_t   *node = NULL;
	list_foreach(list, copyFunc, &(node));

	node_t *curr = node;
	while (curr->previous)
	{
		curr = curr->previous;
	}
	copy->head = curr;
	copy->tail = node;
	node->next = curr;
	return copy;
}

void list_insertAfter(dllist_t *list, node_t *node)
{
	node->next       = list->head;
	node->previous   = list->tail;
	list->tail->next = node;
	list->tail       = node;
}

void list_insertBefore(dllist_t *list, node_t *node)
{
	node->previous       = NULL;
	node->next           = list->head;
	list->head->previous = node;
	list->tail->next     = node;
	list->head           = node;
}

void list_removeNode(dllist_t *list, node_t *node)
{
	if (node)
	{
		if (node == list->head)
		{
			list->tail->next     = node->next;
			list->head           = list->head->next;
			list->head->previous = NULL;
			free(node);
			return;
		}
		if (node == list->tail)
		{
			list->tail = node->previous;
		}
		else
		{
			node->next->previous = node->previous;
		}
		node->previous->next = node->next;

		free(node);
	}
}

node_t *list_pushFront(dllist_t *list, data_t value)
{
	assert(list);

	node_t *node = malloc(sizeof(node_t));
	node->value = value;
	if ((list->head == NULL) && (list->tail == NULL))
	{
		list->head     = list->tail = node;
		node->next     = node;
		node->previous = NULL;
		return node;
	}
	assert(!((list->head == NULL) || (list->tail == NULL)) && "Invalid argument: list is not OK");

	node->previous = NULL;
	node->next     = list->head;

	list->head->previous = node;
	list->tail->next     = node;
	list->head           = node;
	return node;
}

node_t *list_pushBack(dllist_t *list, data_t value)
{
	assert(list);

	node_t *node = malloc(sizeof(node_t));
	node->value = value;
	if ((list->head == NULL) && (list->tail == NULL))
	{
		list->head     = list->tail = node;
		node->next     = node;
		node->previous = NULL;
		return node;
	}
	assert(!((list->head == NULL) || (list->tail == NULL)) && "Invalid argument: list is not OK");

	node->next       = list->head;
	node->previous   = list->tail;
	list->tail->next = node;

	list->tail = node;
	return node;
}

data_t list_popFront(dllist_t *list)
{
	assert(list && "Ivalid argument: list is NULL");
	assert(!((list->head == NULL) && (list->tail == NULL)) && "Ivalid argument: list is empty");
	assert(!((list->head == NULL) || (list->tail == NULL)) && "Ivalid argument: list is not OK");

	node_t *node = list->head;
	data_t value = list->head->value;

	list->head           = list->head->next;
	list->head->previous = NULL;
	list->tail->next     = list->head;

	free(node);
	return value;
}

data_t list_popBack(dllist_t *list)
{
	assert(list && "Ivalid argument: list is NULL");
	assert(!((list->head == NULL) && (list->tail == NULL)) && "Ivalid argument: list is empty");
	assert(!((list->head == NULL) || (list->tail == NULL)) && "Ivalid argument: list is not OK");

	node_t *node = list->tail;
	data_t value = list->tail->value;

	list->tail          = list->tail->previous;
	list->tail->next     = list->head;

	free(node);
	return value;
}