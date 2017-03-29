#include <stdlib.h>
#include <errno.h>
#include "list.h"

/*
    list->head->previous == NULL;
    list->tail->next == list->head;
 */


#define RETURN_ERROR_ON_INVALID_ITERATOR(return_value)    do {if (iterator == NULL){\
                                                errno = EINVAL;\
                                                return return_value;\
                                            }\
                                            if (iterator->list == NULL){\
                                                errno = EINVAL;\
                                                return return_value;\
                                            }}while(0);

void *malloc_for_test(size_t size)
{
	if (testingMalloc)
	{
		errno = ENOMEM;
		return NULL;
	}
	return malloc(size);
}

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

int list_isOK(dllist_t *list)
{
	if (list == NULL)
	{
		return 0;
	}

	if ((list->head == NULL) && (list->tail == NULL))
	{
		return 1;
	}

	if ((list->head == NULL) || (list->tail == NULL))
	{
		return 0;
	}

	if (list->head->previous != NULL)
	{
		return 0;
	}
	if (list->tail->next != list->head)
	{
		return 0;
	}

	node_t *curr = list->head;
	while (curr != list->tail)
	{
		if (curr == NULL)
		{
			return 0;
		}
		curr = curr->next;
	}

	curr = list->tail;
	while (curr != list->head)
	{
		if (curr == NULL)
		{
			return 0;
		}
		curr = curr->previous;
	}
	return 1;
}

dllist_iterator_t *dllist_iterator_new(dllist_t *list)
{
	if (!list_isOK(list))
	{
		errno = EINVAL;
		return NULL;
	}

	dllist_iterator_t *iterator = malloc_for_test(sizeof(dllist_iterator_t));
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
	if (iterator->curr == NULL)
	{
		return NULL;
	}
	return iterator->curr = iterator->curr->next;
}

node_t *dllist_iterator_previous(dllist_iterator_t *iterator)
{
	RETURN_ERROR_ON_INVALID_ITERATOR(NULL)

	if ((iterator->curr == NULL) || (iterator->curr == iterator->list->head))
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
	dllist_t *list = malloc_for_test(sizeof(dllist_t));
	if (list)
	{
		list->head = NULL;
		list->tail = NULL;
	}
	return list;
}

int list_deleteAll(dllist_t *list)
{
	if (list == NULL)
	{
		return 0;
	}
	node_t *curr;
	do
	{
		curr = list->head;
		if (curr == NULL)
		{
			break;
		}
		errno = 0;
		list_removeNode(list, curr);
		if (errno != 0)
		{
			return 1;
		}
	}
	while (1);

	free(list);
	return 0;
}

int list_initWithArray(dllist_t *list, const data_t *array, int n)
{
	if (!list_isOK(list))
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

	int    i;
	node_t *curr = NULL;

	do
	{
		curr = list->head;
		if (curr == NULL)
		{
			break;
		};
		errno = 0;
		list_removeNode(list, curr);
		if (errno != 0)
		{
			return 1;
		}
	}
	while (1);

	for (i = 0; i < n; i++)
	{
		curr = list_pushBack(list, array[i]);
		if (curr == NULL)
		{
			return 1;
		}
	}
	return 0;
}

int list_foreach(dllist_t *list, int (*func)(node_t *curr, void *param), void *param)
{
	if (!list_isOK(list))
	{
		errno = EINVAL;
		return -1;
	}
	if ((list->head == NULL) && (list->tail == NULL))
	{
		return 0;
	}

	node_t *node;
	int    stop = 0;
	for (node = list->head; node != list->tail; node = node->next)
	{
		stop = func(node, param);
		if (stop)
		{
			return 1;
		}
	}
	func(node, param);

	return 0;
}

int copyFunc(node_t *node, void *param)
{
	dllist_t *list = *((dllist_t **) param);
	if (node == NULL || list == NULL)
	{
		return 1;
	}
	node_t *res = list_pushBack(list, node->value);
	if (res == NULL)
	{
		return 1;
	}
	return 0;
}

dllist_t *list_copy(dllist_t *list)
{
	if (!list_isOK(list))
	{
		errno = EINVAL;
		return NULL;
	}

	dllist_t *copy = list_new();
	if (copy == NULL)
	{
		return NULL;
	}
	if ((list->head == NULL) && (list->tail == NULL))
	{
		return copy;
	}

	int res = list_foreach(list, copyFunc, &(copy));
	if (res != 0)
	{
		list_deleteAll(copy);
		return NULL;
	}
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
	if (!list_isOK(list) || node == NULL)
	{
		errno = EINVAL;
		return 1;
	}

	if ((list->head == NULL) && (list->tail == NULL))
	{
		insertSingleNodeIntoEmptyList(list, node);
		return 0;
	}

	node->next       = list->head;
	node->previous   = list->tail;
	list->tail->next = node;
	list->tail       = node;
	return 0;
}

int list_insertBefore(dllist_t *list, node_t *node)
{
	if (!list_isOK(list) || node == NULL)
	{
		errno = EINVAL;
		return 1;
	}

	if ((list->head == NULL) && (list->tail == NULL))
	{
		insertSingleNodeIntoEmptyList(list, node);
		return 0;
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
	if (!list_isOK(list) || node == NULL)
	{
		errno = EINVAL;
		return -1;
	}

	if ((list->head == NULL) && (list->tail == NULL))// list is empty
	{
		errno = EINVAL;
		return -1;
	}
	data_t value = node->value;
	if ((node == list->head) && (node == list->tail))
	{
		list->head = list->tail = NULL;
		free(node);
		return value;
	}

	if (node == list->head)
	{
		list->head           = node->next;
		list->tail->next     = list->head;
		list->head->previous = NULL;
	}
	else
	{
		node->previous->next = node->next;
	}

	if (node == list->tail)
	{
		list->tail = node->previous;
	}
	else
	{
		node->next->previous = node->previous;
	}
	free(node);

	return value;
}

node_t *list_pushFront(dllist_t *list, data_t value)
{
	if (!list_isOK(list))
	{
		errno = EINVAL;
		return NULL;
	}

	node_t *node = malloc_for_test(sizeof(node_t));
	if (node == NULL)
	{
		//errno = ENOMEM
		return NULL;
	}
	node->value = value;
	int res = list_insertBefore(list, node);
	if (res != 0)
	{
		free(node);
		return NULL;
	}
	return node;
}

node_t *list_pushBack(dllist_t *list, data_t value)
{
	if (!list_isOK(list))
	{
		errno = EINVAL;
		return NULL;
	}

	node_t *node = malloc_for_test(sizeof(node_t));
	if (node == NULL)
	{
		//errno = ENOMEM
		return NULL;
	}
	node->value = value;
	int res = list_insertAfter(list, node);
	if (res != 0)
	{
		free(node);
		return NULL;
	}
	return node;
}