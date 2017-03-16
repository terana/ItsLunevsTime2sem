#include <stdlib.h>
#include <errno.h>
#include "list.h"

/*
    list->head->previous == NULL;
    list->tail->next == list->head;
 */

#define RETURN_ERROR_ON_INVALID_ITERATOR(return_value)    {if (iterator == NULL){\
                                                errno = EINVAL;\
                                                return return_value;\
                                            }\
                                            if (iterator->list == NULL || iterator->curr == NULL){\
                                                errno = EINVAL;\
                                                return return_value;\
                                            }}

struct dllist
{
	node_t *head;
	node_t *tail;
};

struct dllist_iterator
{
	dllist_t *list;
	node_t   *curr;
};

dllist_iterator_t *dllist_iterator_new(dllist_t *list)
{
	if (list == NULL)
	{
		errno = EINVAL;
		return NULL;
	}
	if (list->head == NULL)
	{
		return NULL;
	}

	dllist_iterator_t *iterator = malloc(sizeof(dllist_iterator_t));
	if (iterator != NULL)
	{
		iterator->list = list;
		iterator->curr = list->head;
	}
	return iterator;
}

node_t *dllist_iterator_get(dllist_iterator_t *iterator)
{
	RETURN_ERROR_ON_INVALID_ITERATOR(NULL)

	return iterator->curr;
}

node_t *dllist_iterator_next(dllist_iterator_t *iterator)
{
	RETURN_ERROR_ON_INVALID_ITERATOR(NULL)
	return iterator->curr = iterator->curr->next;
}

node_t *dllist_iterator_previous(dllist_iterator_t *iterator)
{
	RETURN_ERROR_ON_INVALID_ITERATOR(NULL)
	if (iterator->curr == iterator->list->head)
	{
		return NULL;
	}
	return iterator->curr = iterator->curr->previous;
}

int dllist_itertor_isLast(dllist_iterator_t *iterator)
{
	RETURN_ERROR_ON_INVALID_ITERATOR(-1)
	if (iterator->curr == iterator->list->tail)
	{
		return 1;
	}
	return 0;
}

dllist_t *list_new()
{
	dllist_t *list = malloc(sizeof(dllist_t));
	if (list)
	{
		list->head = NULL;
		list->tail = NULL;
	}
	return list;
}

void list_deleteAll(dllist_t *list)
{
	if (list == NULL)
	{
		return;
	}
	node_t *curr;

	do
	{
		curr = list->head;
		if (curr == NULL)
		{
			break;
		}
		list_removeNode(list, curr);
	}
	while (1);

	free(list);
}

int list_initWithArray(dllist_t **listPtr, const data_t *array, int n)
{
	if (listPtr == NULL)
	{
		errno = EINVAL;
		return 1;
	}

	dllist_t *list = *listPtr;
	if (list == NULL)
	{
		errno = EINVAL;
		return 1;
	}

	if (n < 0)
	{
		errno = EINVAL;
		return 1;
	}

	if ((array == NULL) && (n != 0))
	{
		errno = EINVAL;
		return 1;
	}

	if (list->head != NULL)
	{
		list_deleteAll(list);
		list = list_new();
		if (list == NULL)
		{
			//errno = ENOMEM;
			return 1;
		}
		*listPtr = list;
	}

	int    i;
	node_t *prev   = NULL;
	node_t *curr   = NULL;
	for (i = 0; i < n; i++)
	{
		curr = malloc(sizeof(node_t));
		if (curr == NULL)
		{
			//errno = ENOMEM;
			return 1;
		}
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
	return 0;
}

int list_foreach(dllist_t *list, void (*func)(node_t *curr, void *param, int *stop), void *param)
{
	if (list == NULL)
	{
		errno = EINVAL;
		return 1;
	}
	if ((list->head == NULL) && (list->tail == NULL))
	{
		return 0;
	}

	if ((list->head == NULL) || (list->tail == NULL))
	{
		errno = EINVAL;
		return 1;
	}

	node_t *node;
	int    stop = 0;
	for (node = list->head; (node != list->tail) && (stop == 0); node = node->next)
	{
		func(node, param, &stop);
	}
	if ((node == list->tail) && (stop == 0))
	{
		func(node, param, &stop);
	}
	return 0;
}

void copyFunc(node_t *node, void *param, int *stop)
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
	if (list == NULL)
	{
		errno = EINVAL;
		return NULL;
	}

	dllist_t *copy = list_new();
	if ((list->head == NULL) && (list->tail == NULL))
	{
		return copy;
	}

	node_t *node = NULL;
	int    res   = list_foreach(list, copyFunc, &(node));
	if (res != 0)
	{
		return NULL;
	}
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

void insertSingleNodeIntoEmptyList(dllist_t *list, node_t *node)
{
	list->head     = node;
	list->tail     = node;
	node->previous = NULL;
	node->next     = node;
}

int list_insertAfter(dllist_t *list, node_t *node)
{
	if (node == NULL || list == NULL)
	{
		errno = EINVAL;
		return 1;
	}
	if ((list->head == NULL) && (list->tail == NULL))
	{
		insertSingleNodeIntoEmptyList(list, node);
		return 0;
	}
	if ((list->head == NULL) || (list->tail == NULL)) //List is not OK
	{
		errno = EINVAL;
		return 1;
	}

	node->next       = list->head;
	node->previous   = list->tail;
	list->tail->next = node;
	list->tail       = node;
	return 0;
}

int list_insertBefore(dllist_t *list, node_t *node)
{
	if (node == NULL || list == NULL)
	{
		errno = EINVAL;
		return 1;
	}
	if ((list->head == NULL) && (list->tail == NULL))
	{
		insertSingleNodeIntoEmptyList(list, node);
		return 0;
	}
	if ((list->head == NULL) || (list->tail == NULL)) //List is not OK
	{
		errno = EINVAL;
		return 1;
	}
	node->previous       = NULL;
	node->next           = list->head;
	list->head->previous = node;
	list->tail->next     = node;
	list->head           = node;
	return 0;
}

int list_removeNode(dllist_t *list, node_t *node)
{
	if (node == NULL || list == NULL)
	{
		errno = EINVAL;
		return 1;
	}

	if ((list->head == NULL) && (list->tail == NULL))// list is empty
	{
		errno = EINVAL;
		return 1;
	}

	if ((list->head == NULL) || (list->tail == NULL)) // list isnot OK
	{
		errno = EINVAL;
		return 1;
	}

	if ((node == list->head) && (node == list->tail))
	{
		free(node);
		list->head = NULL;
		list->tail = NULL;
		return 0;
	}
	if (node == list->head)
	{
		list->tail->next     = node->next;
		list->head           = list->head->next;
		list->head->previous = NULL;
		free(node);
		return 0;
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

	return 0;
}

node_t *list_pushFront(dllist_t *list, data_t value)
{
	if (list == NULL)
	{
		errno = EINVAL;
		return NULL;
	}

	node_t *node = malloc(sizeof(node_t));
	if (node == NULL)
	{
		//errno = ENOMEM
		return NULL;
	}
	node->value = value;
	if ((list->head == NULL) && (list->tail == NULL))
	{
		insertSingleNodeIntoEmptyList(list, node);
		return node;
	}
	if ((list->head == NULL) || (list->tail == NULL))
	{
		errno = EINVAL;
		return NULL;
	}

	node->previous = NULL;
	node->next     = list->head;

	list->head->previous = node;
	list->tail->next     = node;
	list->head           = node;
	return node;
}

node_t *list_pushBack(dllist_t *list, data_t value)
{
	if (list == NULL)
	{
		errno = EINVAL;
		return NULL;
	}

	node_t *node = malloc(sizeof(node_t));
	if (node == NULL)
	{
		//errno = ENOMEM
		return NULL;
	}
	node->value = value;
	if ((list->head == NULL) && (list->tail == NULL))
	{
		insertSingleNodeIntoEmptyList(list, node);
		return node;
	}
	if ((list->head == NULL) || (list->tail == NULL))
	{
		errno = EINVAL;
		return NULL;
	}

	node->next       = list->head;
	node->previous   = list->tail;
	list->tail->next = node;

	list->tail = node;
	return node;
}

data_t list_popFront(dllist_t *list)
{
	if (list == NULL)
	{
		errno = EINVAL;
		return -1;
	}
	if ((list->head == NULL) || (list->tail == NULL)) // list is not OK or empty
	{
		errno = EINVAL;
		return -1;
	}

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
	if (list == NULL)
	{
		errno = EINVAL;
		return -1;
	}
	if ((list->head == NULL) || (list->tail == NULL))
	{
		errno = EINVAL;
		return -1;
	}

	node_t *node = list->tail;
	data_t value = list->tail->value;

	list->tail       = list->tail->previous;
	list->tail->next = list->head;

	free(node);
	return value;
}