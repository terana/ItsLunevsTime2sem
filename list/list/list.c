#include <stdlib.h>
#include <stdio.h>
#include "list.h"

doubleLinkedList_t *list_new()
{
	doubleLinkedList_t *list = malloc(sizeof(doubleLinkedList_t));
	node_t             *node = malloc(sizeof(node_t));
	node->previous = node;
	node->next     = node;
	node->value    = 0;
	list->head     = node;
	list->tail     = node;
	return list;
}

void list_delete(doubleLinkedList_t *list)
{
	node_t *curr = list->head;
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

void list_initWithArray(doubleLinkedList_t *list, const data_t *array, int n)
{
	if (n < 1)
	{
		return;
	}
	if ((list->head != list->tail) || (list->head == NULL))
	{
		list_delete(list);
		list = list_new();
	}
	list->head->value = array[0];

	int    i;
	node_t *prev      = list->head;
	node_t *curr      = list->head;
	for (i = 1; i < n; i++)
	{
		curr = malloc(sizeof(node_t));
		curr->previous = prev;
		prev->next     = curr;
		curr->value    = array[i];
		prev = curr;
	}
	list->tail           = curr;
	curr->next           = list->head;
	list->head->previous = list->head;
}

void copyFunc(data_t value, void *param)
{
	node_t *prev = *((node_t **) param);
	node_t *curr = malloc(sizeof(node_t));
	curr->previous = prev;
	curr->value    = value;
	if (prev)
	{
		prev->next = curr;
	}
	*((node_t **) param) = curr;
}

doubleLinkedList_t *list_copy(doubleLinkedList_t *list)
{
	doubleLinkedList_t *copy = list_new();
	node_t             *node = NULL;
	list_foreach(list, copyFunc, &(node));

	node_t *curr = node;
	while (curr->previous)
	{
		curr = curr->previous;
	}
	curr->previous = curr;
	node->next     = curr;
	copy->head     = curr;
	copy->tail     = node;
	return copy;
}

void list_insertAfter(doubleLinkedList_t *list, node_t *node)
{
	node->next       = list->head;
	node->previous   = list->tail;
	list->tail->next = node;
}

void list_insertBefore(doubleLinkedList_t *list, node_t *node)
{
	node->previous       = node;
	node->next           = list->head;
	list->head->previous = node;
	list->tail->next     = node;
	list->head           = node;
}

void list_removeNode(doubleLinkedList_t *list, node_t *node)
{
	if (node)
	{
		if (node == list->head)
		{
			list->tail->next     = node->next;
			list->head           = list->head->next;
			list->head->previous = list->head;
			return;
		}
		if (node != list->tail)
		{
			node->next->previous = node->previous;
		}
		else
		{
			list->tail = node->previous;
		}
		node->previous->next = node->next;

		free(node);
	}
}

node_t *list_pushFront(doubleLinkedList_t *list, data_t d);
node_t *list_pushBack(doubleLinkedList_t *list, data_t d);

data_t list_popFront(doubleLinkedList_t *list);
data_t list_popBack(doubleLinkedList_t *list);
data_t list_popNode(node_t *t);

void list_foreach(doubleLinkedList_t *list, void (*func)(data_t d, void *param), void *param)
{
	node_t *node = list->head;
	func(node->value, param);
	node = node->next;
	while (node != list->head)
	{
		func(node->value, param);
		node = node->next;
	}
}