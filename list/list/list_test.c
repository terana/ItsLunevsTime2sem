#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include "list.h"

struct dllist
{
	node_t *head;
	node_t *tail;
};

void crashIfFalse(int condition, const char *description)
{
	if (!condition)
	{
		printf("%s", description);
	}
	assert(condition);
}


/*
 * returns 1 if list is OK, 0 if list is not OK
 */
int list_isOK(dllist_t *list)
{
	if (list == NULL)
	{
		return 1;
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

dllist_t *setUp()
{
	dllist_t *list    = list_new();
	data_t   array[5] = { 1, 2, 3, 4, 5 };
	list_initWithArray(list, array, 5);
	return list;
}

void testListNew()
{
	dllist_t *list = list_new();
	crashIfFalse(list_isOK(list), "testListNew: list is notOK\n");
}

void testInitWithArray()
{
	dllist_t *list    = setUp();
	data_t   array[5] = { 1, 2, 3, 4, 5 };

	list_initWithArray(list, array, 5);
	crashIfFalse(list_isOK(list), "list_InitWithArray: list isnot OK\n");

	node_t *curr;
	int    i          = 0;
	char   *str       = malloc(100 * sizeof(char));
	for (curr = list->head; curr != list->tail; curr = curr->next)
	{
		sprintf(str, "list_initWithArray:  want %d, have %d\n", array[i], curr->value);
		crashIfFalse(array[i++] == curr->value, str);
	}
	sprintf(str, "list_initWithArray:  want %d, have %d\n", array[i], curr->value);
	crashIfFalse(array[i] == curr->value, str);

	free(str);
	list_delete(list);
}

void testEmptyListInitWithArray()
{
	dllist_t *list    = list_new();
	data_t   array[5] = { 1, 2, 3, 4, 5 };

	list_initWithArray(list, array, 5);
	crashIfFalse(list_isOK(list), "empty list_InitWithArray: list isnot OK\n");

	node_t *curr;
	int    i          = 0;
	char   *str       = malloc(100 * sizeof(char));
	for (curr = list->head; curr != list->tail; curr = curr->next)
	{
		sprintf(str, "empty list_initWithArray:  want %d, have %d\n", array[i], curr->value);
		crashIfFalse(array[i++] == curr->value, str);
	}
	sprintf(str, "empty list_initWithArray:  want %d, have %d\n", array[i], curr->value);
	crashIfFalse(array[i] == curr->value, str);

	free(str);
	list_delete(list);
}

void testListInitWithEmptyArray()
{
	dllist_t *list  = list_new();
	data_t   *array = NULL;
	list_initWithArray(list, array, 0);
	crashIfFalse(list_isOK(list), "list_InitWithArray: list isnot OK\n");

	crashIfFalse(list->tail == NULL && list->head == NULL, "list init with empty array: list is not empty");

	list_delete(list);
}

struct node_str
{
	node_t *node;
	char   *str;
};

void func(node_t *curr, void *param)
{
	struct node_str *ns   = (struct node_str *) param;
	node_t          *node = ns->node;
	char            *str  = ns->str;

	sprintf(str, "list_foreach: curr != node\n");
	crashIfFalse(curr == node, str);

	ns->node = node->next;

	return;
}

void testForeach()
{
	dllist_t        *list = setUp();
	struct node_str *ns   = malloc(sizeof(struct node_str));
	ns->str  = malloc(100 * sizeof(char));
	ns->node = list->head;
	list_foreach(list, func, ns);
	crashIfFalse(list_isOK(list), "list_foreach: list is not OK\n");

	free(ns->str);
	free(ns);
	list_delete(list);
}

void testCopy()
{
	dllist_t *list = setUp();
	dllist_t *copy = list_copy(list);
	crashIfFalse(list_isOK(copy), "list_copy: copy is not OK\n");
	crashIfFalse(list_isOK(list), "list_copy: list is not OK\n");

	node_t *want;
	node_t *have;

	char *str = malloc(100 * sizeof(char));
	for (want = list->head, have = copy->head; want != list->tail; want = want->next, have = have->next)
	{
		sprintf(str, "list_copy: want->value = %d, have->value = %d\n", want->value, have->value);
		crashIfFalse(want->value == have->value, str);
		crashIfFalse(want != have, "list_copy:want and have are the same nodes\n");
	}
	sprintf(str, "list_copy: want->value = %d, have->value = %d\n", want->value, have->value);
	crashIfFalse(want->value == have->value, str);
	crashIfFalse(want != have, "list_copy:want and have are the same nodes\n");

	free(str);
	list_delete(list);
	list_delete(copy);
}

void testInsertAfter()
{
	dllist_t *list = setUp();
	dllist_t *copy = list_copy(list);
	node_t   *node = malloc(sizeof(node_t));
	node->value = 6;

	list_insertAfter(list, node);
	crashIfFalse(list_isOK(list), "list_insertAfter: list is not OK\n");

	node_t *want;
	node_t *have;
	char   *str = malloc(100 * sizeof(char));

	for (want = copy->head, have = list->head; want != copy->tail; want = want->next, have = have->next)
	{
		sprintf(str, "list_insertAfter: want->value = %d, have->value = %d\n", want->value, have->value);
		crashIfFalse(want->value == have->value, str);
	}
	sprintf(str, "list_insertAfter: want->value = %d, have->value = %d\n", want->value, have->value);
	crashIfFalse(want->value == have->value, str);
	have = have->next;

	want = node;
	crashIfFalse(want == have, "last node is not the same\n");
	crashIfFalse(have == list->tail, "List insert after: invalid tail\n");

	list_delete(list);
	list_delete(copy);
	free(str);
}

void testInsertBefore()
{
	dllist_t *list = setUp();
	dllist_t *copy = list_copy(list);
	node_t   *node = malloc(sizeof(node_t));
	node->value = 6;

	list_insertBefore(list, node);
	crashIfFalse(list_isOK(list), "list_insertBefore: list is not OK\n");

	node_t *want = node;
	node_t *have = list->head;
	char   *str  = malloc(100 * sizeof(char));

	sprintf(str, "list_insertBefore: want first value  %d, have first value %d\n", want->value, have->value);
	crashIfFalse(want->value == have->value, str);
	crashIfFalse(want == have, "list_insertBefore: first node is not the same");

	for (want = copy->head, have = have->next; want != copy->tail; want = want->next, have = have->next)
	{
		sprintf(str, "list_insertBefore: want->value = %d, have->value = %d\n", want->value, have->value);
		crashIfFalse(want->value == have->value, str);
	}
	sprintf(str, "list_insertBefore: want->value = %d, have->value = %d\n", want->value, have->value);
	crashIfFalse(want->value == have->value, str);

	list_delete(list);
	list_delete(copy);
	free(str);
}

void testRemoveNodeHead()
{
	dllist_t *list = setUp();
	dllist_t *copy = list_copy(list);
	node_t   *node = list->head;

	list_removeNode(list, node);
	crashIfFalse(list_isOK(list), "list_removeNode head: list is not OK\n");

	char   *str  = malloc(100 * sizeof(char));
	node_t *want = copy->head->next;
	node_t *have = list->head;

	for (; want != copy->head; want = want->next, have = have->next)
	{
		sprintf(str, "list_removeNode head: want->value = %d, have->value = %d\n", want->value, have->value);
		crashIfFalse(want->value == have->value, str);
	}

	crashIfFalse(have == list->head, "list_removeNode head: list is not circled");

	list_delete(list);
	list_delete(copy);
	free(str);
}

void testRemoveNodeTail()
{
	dllist_t *list = setUp();
	dllist_t *copy = list_copy(list);
	node_t   *node = list->tail;

	list_removeNode(list, node);
	crashIfFalse(list_isOK(list), "list_removeNode tail: list is not OK\n");

	char   *str  = malloc(100 * sizeof(char));
	node_t *want = copy->head;
	node_t *have = list->head;

	for (; want != copy->tail; want = want->next, have = have->next)
	{
		sprintf(str, "list_removeNode tail: want->value = %d, have->value = %d\n", want->value, have->value);
		crashIfFalse(want->value == have->value, str);
	}

	crashIfFalse(have == list->head, "list_removeNode tail: have extra value");

	list_delete(list);
	list_delete(copy);
	free(str);
}

void testRemoveNodeMiddle()
{
	dllist_t *list = setUp();
	dllist_t *copy = list_copy(list);
	node_t   *node = list->head->next;

	list_removeNode(list, node);
	crashIfFalse(list_isOK(list), "list_removeNode middle: list is not OK\n");

	char   *str  = malloc(100 * sizeof(char));
	node_t *want = copy->head;
	node_t *have = list->head;

	sprintf(str, "list_removeNode middle: want->value = %d, have->value = %d\n", want->value, have->value);
	crashIfFalse(want->value == have->value, str);

	want = want->next->next;
	have = have->next;

	for (; want != copy->tail; want = want->next, have = have->next)
	{
		sprintf(str, "list_removeNode tail: want->value = %d, have->value = %d\n", want->value, have->value);
		crashIfFalse(want->value == have->value, str);
	}

	list_delete(list);
	list_delete(copy);
	free(str);
}

void testPushFront()
{
	dllist_t *list = setUp();
	dllist_t *copy = list_copy(list);

	data_t d     = 6;
	node_t *node = list_pushFront(list, d);
	crashIfFalse(list_isOK(list), "list_pushFront: list is not OK\n");

	char *str = malloc(100 * sizeof(char));
	sprintf(str, "list_pushFront: want first value %d have %d\n", d, list->head->value);
	crashIfFalse(list->head->value == d, str);

	sprintf(str, "list_pushFront: first node is not equal the return value\n");
	crashIfFalse(list->head == node, str);

	node_t *want = copy->head;
	node_t *have = list->head->next;
	for (; want != copy->tail; want = want->next, have = have->next)
	{
		sprintf(str, "list_pushFront: want->value = %d, have->value = %d\n", want->value, have->value);
		crashIfFalse(want->value == have->value, str);
	}
	sprintf(str, "list_pushFront: want->value = %d, have->value = %d\n", want->value, have->value);
	crashIfFalse(want->value == have->value, str);

	list_delete(list);
	list_delete(copy);
	free(str);
}

void testEmptyListPushFront()
{
	dllist_t *list = list_new();

	data_t d     = 6;
	node_t *node = list_pushFront(list, d);
	crashIfFalse(list_isOK(list), "empty list_pushFront: list is not OK\n");

	char *str = malloc(100 * sizeof(char));
	sprintf(str, "empty list_pushFront: want first value %d have %d\n", d, list->head->value);
	crashIfFalse(list->head->value == d, str);

	sprintf(str, "empty list_pushFront: first node is not equal the return value\n");
	crashIfFalse(list->head == node, str);

	sprintf(str, "empty list_pushFront: last node isnot equalthe return value\n");
	crashIfFalse(list->tail == node, str);

	crashIfFalse(list->head->next == list->head, "empty list_pushFront: list is not singular\n");

	list_delete(list);
	free(str);
}

void testPushBack()
{
	dllist_t *list = setUp();
	dllist_t *copy = list_copy(list);

	data_t d     = 6;
	node_t *node = list_pushBack(list, d);
	crashIfFalse(list_isOK(list), "list_pushBack: list is not OK\n");

	char *str = malloc(100 * sizeof(char));
	sprintf(str, "list_pushBack: want last value %d have %d\n", d, list->tail->value);
	crashIfFalse(list->tail->value == d, str);

	sprintf(str, "list_pushBack: last node isnot equalthe return value\n");
	crashIfFalse(list->tail == node, str);

	node_t *want = copy->head;
	node_t *have = list->head;
	for (; want != copy->tail; want = want->next, have = have->next)
	{
		sprintf(str, "list_pushBack: want->value = %d, have->value = %d\n", want->value, have->value);
		crashIfFalse(want->value == have->value, str);
	}
	sprintf(str, "list_pushBack: want->value = %d, have->value = %d\n", want->value, have->value);
	crashIfFalse(want->value == have->value, str);

	list_delete(list);
	list_delete(copy);
	free(str);
}

void testEmptyListPushBack()
{
	dllist_t *list = list_new();

	data_t d     = 6;
	node_t *node = list_pushBack(list, d);
	crashIfFalse(list_isOK(list), "empty list_pushBack: list is not OK\n");

	char *str = malloc(100 * sizeof(char));
	sprintf(str, "empty list_pushBack: want first value %d have %d\n", d, list->head->value);
	crashIfFalse(list->head->value == d, str);

	sprintf(str, "empty list_pushBack: first node is not equal the return value\n");
	crashIfFalse(list->head == node, str);

	sprintf(str, "empty list_pushBack: last node is not equal the return value\n");
	crashIfFalse(list->tail == node, str);

	crashIfFalse(list->head->next == list->head, "empty list_pushBack: list is not singular\n");

	list_delete(list);
	free(str);
}

void testPopFront()
{
	dllist_t *list     = setUp();
	dllist_t *copy     = list_copy(list);
	data_t   wantValue = list->head->value;

	data_t haveValue = list_popFront(list);
	crashIfFalse(list_isOK(list), "list_popFront: list is not OK\n");

	char *str = malloc(100 * sizeof(char));
	sprintf(str, "list_popFront: wantValue = %d, haveValue = %d\n", wantValue, haveValue);
	crashIfFalse(wantValue == haveValue, str);

	node_t *want = copy->head->next;
	node_t *have = list->head;

	for (; want != copy->head; want = want->next, have = have->next)
	{
		sprintf(str, "list_popFront: want->value = %d, have->value = %d\n", want->value, have->value);
		crashIfFalse(want->value == have->value, str);
	}

	list_delete(list);
	list_delete(copy);
	free(str);
}

void testPopBack()
{
	dllist_t *list     = setUp();
	dllist_t *copy     = list_copy(list);
	data_t   wantValue = list->tail->value;

	data_t haveValue = list_popBack(list);
	crashIfFalse(list_isOK(list), "list_popBack: list is not OK\n");

	char *str = malloc(100 * sizeof(char));
	sprintf(str, "list_popBackt: wantValue = %d, haveValue = %d\n", wantValue, haveValue);
	crashIfFalse(wantValue == haveValue, str);

	node_t *want = copy->head;
	node_t *have = list->head;

	for (; want != copy->tail; want = want->next, have = have->next)
	{
		sprintf(str, "list_popBack: want->value = %d, have->value = %d\n", want->value, have->value);
		crashIfFalse(want->value == have->value, str);
	}

	crashIfFalse(have == list->head, "list_popBack: have extra value");

	list_delete(list);
	list_delete(copy);
	free(str);
}

int main()
{
	testListNew();
	testInitWithArray();
	testListInitWithEmptyArray();
	testEmptyListInitWithArray();
	testForeach();
	testCopy();
	testInsertAfter();
	testInsertBefore();
	testRemoveNodeHead();
	testRemoveNodeMiddle();
	testRemoveNodeTail();
	testPushFront();
	testEmptyListPushFront();
	testPushBack();
	testEmptyListPushBack();
	testPopFront();
	testPopBack();
	printf("Everithing is OK\n");
	return 0;
}