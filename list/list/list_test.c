#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <errno.h>
#include "list.h"

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

void crashIfFalse(int condition, const char *description)
{
	if (!condition)
	{
		printf("%s", description);
	}
	assert(condition);
}

dllist_t *setUp()
{
	dllist_t *list = list_new();
	assert(list && "Memory error occured");
	data_t array[5] = { 1, 2, 3, 4, 5 };
	list_initWithArray(list, array, 5);
	return list;
}

void testListNew()
{
	dllist_t *list = list_new();

	crashIfFalse(list_isOK(list), "testListNew: list is notOK\n");
	crashIfFalse(list->head == NULL, "testListNew: list is not empty\n");

	list_deleteAll(list);
}

void testListNew_InvalidMalloc()
{
	testingMalloc  = 1;
	dllist_t *list = list_new();
	crashIfFalse(list == NULL, "testListNew invalid malloc: error is not detected\n");
	crashIfFalse(errno != 0, "testListNew invalid malloc: errno is not set\n");
	testingMalloc = 0;
}

void testListDeleteAll()
{
	dllist_t *list = setUp();
	int      res   = list_deleteAll(list);
	crashIfFalse(res == 0, "testListDeleteAll null list: error detected\n");
}

void testListDeleteAll_NullList()
{
	int res = list_deleteAll(NULL);
	crashIfFalse(res == 0, "testListDeleteAll null list: error detected\n");
}
//int list_initWithArray(dllist_t **list, const data_t *array, int n);

void testListInitWithArray_FullList()
{
	dllist_t *list    = setUp();
	data_t   array[5] = { 1, 2, 3, 4, 5 };

	int res     = list_initWithArray(list, array, 5);
	crashIfFalse(list_isOK(list), "list_InitWithArray: list isnot OK\n");
	crashIfFalse(res == 0, "full list_InitWithArray: error occured\n");

	node_t *curr;
	int    i    = 0;
	char   *str = malloc(100 * sizeof(char));
	for (curr = list->head; curr != list->tail; curr = curr->next)
	{
		sprintf(str, "list_initWithArray:  want %d, have %d\n", array[i], curr->value);
		crashIfFalse(array[i++] == curr->value, str);
	}
	sprintf(str, "list_initWithArray:  want %d, have %d\n", array[i], curr->value);
	crashIfFalse(array[i] == curr->value, str);

	free(str);
	list_deleteAll(list);
}

void testListInitWithArray_EmptyList()
{
	dllist_t *list = list_new();
	assert(list && "Memory error occured");
	data_t array[5] = { 1, 2, 3, 4, 5 };

	int res     = list_initWithArray(list, array, 5);
	crashIfFalse(list_isOK(list), "empty list_InitWithArray: list isnot OK\n");
	crashIfFalse(res == 0, "empty list_InitWithArray: error occured\n");

	node_t *curr;
	int    i    = 0;
	char   *str = malloc(100 * sizeof(char));
	for (curr = list->head; curr != list->tail; curr = curr->next)
	{
		sprintf(str, "empty list_initWithArray:  want %d, have %d\n", array[i], curr->value);
		crashIfFalse(array[i++] == curr->value, str);
	}
	sprintf(str, "empty list_initWithArray:  want %d, have %d\n", array[i], curr->value);
	crashIfFalse(array[i] == curr->value, str);

	free(str);
	list_deleteAll(list);
}

void testListInitWithArray_EmptyArray()
{
	dllist_t *list = list_new();
	assert(list && "Memory error occured");
	data_t *array = NULL;
	int    res    = list_initWithArray(list, array, 0);
	crashIfFalse(list_isOK(list), "list_InitWithEmptyArray: list isnot OK\n");
	crashIfFalse(res == 0, "list_InitWithEmptyArray: error occured\n");

	crashIfFalse(list->tail == NULL && list->head == NULL, "list init with empty array: list is not empty");

	list_deleteAll(list);
}

void testListInitWithArray_NDoesntMatchArray()
{
	dllist_t *list = list_new();
	assert(list && "Memory error occured");
	data_t *array = NULL;
	int    res    = list_initWithArray(list, array, 1);
	crashIfFalse(list_isOK(list), "list_InitWithyArray n doesn't match array: list isnot OK\n");
	crashIfFalse(res != 0, "list_InitWithEmptyArray n doesn't match array: error is not detected\n");
	crashIfFalse(errno == EINVAL, "list_InitWithArray n doesn't match array: errno is not set\n");

	list_deleteAll(list);
}

void testListInitWithArray_InvalidN()
{
	dllist_t *list = list_new();
	assert(list && "Memory error occured");
	data_t *array = NULL;
	int    res    = list_initWithArray(list, array, -1);
	crashIfFalse(list_isOK(list), "list_InitWithyArray invalid n: list isnot OK\n");
	crashIfFalse(res != 0, "list_InitWithEmptyArray invalid n: error is not detected\n");
	crashIfFalse(errno == EINVAL, "list_InitWithArray invalid n: errno is not set\n");

	list_deleteAll(list);
}

void testListInitWithArray_NullListPtr()
{
	data_t array[5] = { 1, 2, 3, 4, 5 };

	int res = list_initWithArray(NULL, array, 5);
	crashIfFalse(res != 0, "NULL ptr list_InitWithArray: error isnot detected\n");
	crashIfFalse(errno == EINVAL, "NULL ptr list_InitWithArray: errno is not set\n");
}

void testListInitWithArray_NullList()
{
	data_t   array[5] = { 1, 2, 3, 4, 5 };
	dllist_t *list    = NULL;

	int res = list_initWithArray(list, array, 5);
	crashIfFalse(res != 0, "NULL list_InitWithArray: error isnot detected\n");
	crashIfFalse(errno == EINVAL, "NULL list_InitWithArray: errno is not set\n");
}

void testListInitWithArray_InvalidList()
{
	dllist_t *list      = setUp();
	data_t   array[5]   = { 1, 2, 3, 4, 5 };
	node_t   *list_head = list->head;
	list->head = NULL;

	int res = list_initWithArray(list, array, 5);
	crashIfFalse(res != 0, "list_initWithArray invalid list: error is not detected\n");
	crashIfFalse(errno == EINVAL, "list_initWithArray invalid list: errno is not set\n");

	list->head = list_head;
	list_deleteAll(list);
}

void testListInitWithArray_InvalidMalloc()
{
	dllist_t *list    = setUp();
	data_t   array[5] = { 1, 2, 3, 4, 5 };
	testingMalloc = 1;

	int res = list_initWithArray(list, array, 5);
	crashIfFalse(res != 0, "list_initWithArray invalid malloc: error is not detected\n");
	crashIfFalse(errno != 0, "list_initWithArray invalid malloc: errno is not set\n");
	testingMalloc = 0;
	list_deleteAll(list);
}

//int list_foreach(dllist_t *list, void (*func)(node_t *curr, void *param, int *stop), void *param);

int func(node_t *curr, void *param)
{
	node_t *node = *(node_t **) param;

	crashIfFalse(curr == node, "list_foreach: curr != node\n");

	*(node_t **) param = node->next;

	return 0;
}

void testForeach()
{
	dllist_t *list  = setUp();
	node_t   *param = list->head;
	int      res    = list_foreach(list, func, &param);
	crashIfFalse(list_isOK(list), "list_foreach: list is not OK\n");
	crashIfFalse(res == 0, "list_foreach: error occured\n");

	list_deleteAll(list);
}

int funcStop(node_t *curr, void *param)
{
	node_t *node = *(node_t **) param;

	crashIfFalse(curr == node, "list_foreach stop: curr != node\n");
	crashIfFalse(node->value != 3, "list_foreach stop: should have stoped earlier\n");

	*(node_t **) param = node->next;
	if (node->value == 2)
	{
		return 1;
	}
	return 0;
}

void testForeach_Stop()
{
	dllist_t *list  = setUp();
	node_t   *param = list->head;
	int      res    = list_foreach(list, funcStop, &param);
	crashIfFalse(list_isOK(list), "list_foreach stop: list is not OK\n");
	crashIfFalse(res == 1, "list_foreach stop: error occured\n");

	list_deleteAll(list);
}

void testForeach_NullList()
{
	dllist_t *list = NULL;

	int res = list_foreach(list, func, NULL);
	crashIfFalse(res != 0, "list_foreach null list: error isnot detected\n");
	crashIfFalse(errno == EINVAL, "list_foreach null list: errno is not set\n");
}

void testForeach_EmptyList()
{
	dllist_t *list = list_new();
	assert(list && "Memory error occured");

	int res = list_foreach(list, func, NULL);
	crashIfFalse(res == 0, "list_foreach empty list: error occured\n");
	crashIfFalse(list_isOK(list), "list_foreach empty list: list is not OK\n");
	crashIfFalse((list->head == NULL) && (list->tail == NULL), "list_foreach empty list: list is not empty\n");

	list_deleteAll(list);
}

void testForeach_InvalidList()
{
	dllist_t *list      = setUp();
	list->head->previous = list->head;

	int res = list_foreach(list, func, NULL);
	crashIfFalse(res != 0, "list_foreach invalid list: error isnot detected\n");
	crashIfFalse(errno == EINVAL, "list_foreach invalid list: errno is not set\n");

	list->head->previous = NULL;
	list_deleteAll(list);
}

//dllist_t *list_copy(dllist_t *list);

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
	list_deleteAll(list);
	list_deleteAll(copy);
}

void testCopy_EmptyList()
{
	dllist_t *list = list_new();
	assert(list && "Memory error occured");

	dllist_t *copy = list_copy(list);
	crashIfFalse(list_isOK(copy), "list_copy empty list: copy is not OK\n");
	crashIfFalse(list_isOK(list), "list_copy empty list: list is not OK\n");
	crashIfFalse((copy->head == NULL) && (copy->tail == NULL), "list_copy empty list: copy is notempty\n");

	list_deleteAll(list);
	list_deleteAll(copy);
}

void testCopy_NullList()
{
	dllist_t *list = NULL;

	dllist_t *res = list_copy(list);
	crashIfFalse(res == NULL, "list_copy NULL list: error isnot detected\n");
	crashIfFalse(errno == EINVAL, "list_copy NULL list: errno is not set\n");
}

void testCopy_InvalidList()
{
	dllist_t *list      = setUp();
	list->tail->next = NULL;

	dllist_t *res = list_copy(list);
	crashIfFalse(res == NULL, "list_copy NULL list: error isnot detected\n");
	crashIfFalse(errno != 0, "list_foreach invalid list: errno is not set\n");

	list->tail->next = list->head;
	list_deleteAll(list);
}

void testCopy_InvalidMalloc()
{
	dllist_t *list = setUp();
	testingMalloc = 1;

	dllist_t *res = list_copy(list);
	crashIfFalse(res == NULL, "list_copy invalid malloc: error is not detected\n");
	crashIfFalse(errno != 0, "list_copy invalid malloc: errno is not set\n");
	testingMalloc = 0;
	list_deleteAll(list);
}


//int list_insertAfter(dllist_t *list, node_t *node);

void testInsertAfter()
{
	dllist_t *list = setUp();
	dllist_t *copy = list_copy(list);
	node_t   *node = malloc(sizeof(node_t));
	node->value = 6;

	int res     = list_insertAfter(list, node);
	crashIfFalse(list_isOK(list), "list_insertAfter: list is not OK\n");
	crashIfFalse(res == 0, "list_insertAfter: error occured\n");

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

	list_deleteAll(list);
	list_deleteAll(copy);
	free(str);
}

void testInsertAfter_NullList()
{
	dllist_t *list = NULL;
	node_t   *node = malloc(sizeof(node_t));
	node->value = 6;

	int res = list_insertAfter(list, node);
	crashIfFalse(res != 0, "list_insertAfter: NULL list: error isnot detected\n");
	crashIfFalse(errno == EINVAL, "list_insertAfter: NULL list: errno is not set\n");

	free(node);
}

void testInsertAfter_NullNode()
{
	dllist_t *list = setUp();
	node_t   *node = NULL;

	int res = list_insertAfter(list, node);
	crashIfFalse(res != 0, "list_insertAfter: NULL node: error isnot detected\n");
	crashIfFalse(errno == EINVAL, "list_insertAfter: NULL node: errno is not set\n");

	list_deleteAll(list);
}

void testInsertAfter_EmptyList()
{
	dllist_t *list = list_new();
	assert(list && "Memory error occured");
	node_t *node = malloc(sizeof(node_t));
	node->value = 6;

	int res = list_insertAfter(list, node);
	crashIfFalse(list_isOK(list), "list_insertAfter empty list: list is not OK\n");
	crashIfFalse(res == 0, "list_insertAfter empty list: error occured\n");

	crashIfFalse(list->head == node, "list_insertAfter empty list: head is invalid\n");
	crashIfFalse(list->tail == node, "list_insertAfter empty list: tail is invalid\n");

	list_deleteAll(list);
}

void testInsertAfter_InvalidList()
{
	dllist_t *list      = setUp();
	node_t   *list_head_next = list->head->next;
	list->head->next = NULL;
	node_t *node = malloc(sizeof(node_t));
	node->value = 6;

	int res = list_insertAfter(list, node);
	crashIfFalse(res != 0, "list_insertAfter: NULL node: error isnot detected\n");
	crashIfFalse(errno != 0, "list_insertAfter: NULL node: errno is not set\n");

	list->head->next = list_head_next;
	list_deleteAll(list);
	free(node);
}


//int list_insertBefore(dllist_t *list, node_t *node)

void testInsertBefore()
{
	dllist_t *list = setUp();
	dllist_t *copy = list_copy(list);
	node_t   *node = malloc(sizeof(node_t));
	node->value = 6;

	int res = list_insertBefore(list, node);
	crashIfFalse(list_isOK(list), "list_insertBefore: list is not OK\n");
	crashIfFalse(res == 0, "list_insertBefore: error occured\n");

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

	list_deleteAll(list);
	list_deleteAll(copy);
	free(str);
}

void testInsertBefore_NullList()
{
	dllist_t *list = NULL;
	node_t   *node = malloc(sizeof(node_t));
	node->value = 6;

	int res = list_insertBefore(list, node);
	crashIfFalse(res != 0, "list_insertBefore: NULL list: error isnot detected\n");
	crashIfFalse(errno == EINVAL, "list_insertBefore: NULL list: errno is not set\n");

	free(node);
}

void testInsertBefore_NullNode()
{
	dllist_t *list = setUp();
	node_t   *node = NULL;

	int res = list_insertBefore(list, node);
	crashIfFalse(res != 0, "list_insertBefore: NULL node: error isnot detected\n");
	crashIfFalse(errno == EINVAL, "list_insertBefore: NULL node: errno is not set\n");

	list_deleteAll(list);
}

void testInsertBefore_EmptyList()
{
	dllist_t *list = list_new();
	assert(list && "Memory error occured");
	node_t *node = malloc(sizeof(node_t));
	node->value = 6;

	int res = list_insertBefore(list, node);
	crashIfFalse(list_isOK(list), "list_insertBefore empty list: list is not OK\n");
	crashIfFalse(res == 0, "list_insertBefore empty list: error occured\n");

	crashIfFalse(list->head == node, "list_insertBefore empty list: head is invalid\n");
	crashIfFalse(list->tail == node, "list_insertBefore empty list: tail is invalid\n");

	list_deleteAll(list);
}

void testInsertBefore_InvalidList()
{
	dllist_t *list      = setUp();
	node_t   *list_tail_previous = list->tail->previous;
	list->tail->previous = NULL;
	node_t *node = malloc(sizeof(node_t));
	node->value = 6;

	int res = list_insertBefore(list, node);
	crashIfFalse(res != 0, "list_insertBefore: NULL node: error isnot detected\n");
	crashIfFalse(errno == EINVAL, "list_insertBefore: NULL node: errno is not set\n");

	list->tail->previous = list_tail_previous;
	list_deleteAll(list);
	free(node);
}


//int list_removeNode(dllist_t *list, node_t *node)

void testRemoveNode_Head()
{
	dllist_t *list = setUp();
	dllist_t *copy = list_copy(list);
	node_t   *node = list->head;
	data_t value = node->value;

	int res = list_removeNode(list, node);
	crashIfFalse(list_isOK(list), "list_removeNode head: list is not OK\n");
	crashIfFalse(res == value, "list_removeNode head: return value is not OK\n");
	char   *str  = malloc(100 * sizeof(char));
	node_t *want = copy->head->next;
	node_t *have = list->head;

	for (; want != copy->head; want = want->next, have = have->next)
	{
		sprintf(str, "list_removeNode head: want->value = %d, have->value = %d\n", want->value, have->value);
		crashIfFalse(want->value == have->value, str);
	}

	crashIfFalse(have == list->head, "list_removeNode head: list is not circled");

	list_deleteAll(list);
	list_deleteAll(copy);
	free(str);
}

void testRemoveNode_Tail()
{
	dllist_t *list = setUp();
	dllist_t *copy = list_copy(list);
	node_t   *node = list->tail;
	data_t value = node->value;

	int res = list_removeNode(list, node);
	crashIfFalse(list_isOK(list), "list_removeNode tail: list is not OK\n");
	crashIfFalse(res == value, "list_removeNode tail: return value is not OK\n");

	char   *str  = malloc(100 * sizeof(char));
	node_t *want = copy->head;
	node_t *have = list->head;

	for (; want != copy->tail; want = want->next, have = have->next)
	{
		sprintf(str, "list_removeNode tail: want->value = %d, have->value = %d\n", want->value, have->value);
		crashIfFalse(want->value == have->value, str);
	}

	crashIfFalse(have == list->head, "list_removeNode tail: have extra value");

	list_deleteAll(list);
	list_deleteAll(copy);
	free(str);
}

void testRemoveNode_Middle()
{
	dllist_t *list = setUp();
	dllist_t *copy = list_copy(list);
	node_t   *node = list->head->next;
	data_t value = node->value;

	int res = list_removeNode(list, node);
	crashIfFalse(list_isOK(list), "list_removeNode middle: list is not OK\n");
	crashIfFalse(res == value, "list_removeNode middle: return value is not OK\n");

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

	list_deleteAll(list);
	list_deleteAll(copy);
	free(str);
}

void testRemoveNode_NullList()
{
	dllist_t *list = NULL;
	node_t   *node = malloc(sizeof(node_t));

	int res = list_removeNode(list, node);
	crashIfFalse(res != 0, "list_removeNode NULL node: error isnot detected\n");
	crashIfFalse(errno == EINVAL, "list_removeNode NULL node: errno is not set\n");

	free(node);
}

void testRemoveNode_NullNode()
{
	dllist_t *list = setUp();
	node_t   *node = NULL;

	int res = list_removeNode(list, node);
	crashIfFalse(res != 0, "list_removeNode NULL node: error isnot detected\n");
	crashIfFalse(errno == EINVAL, "list_removeNode NULL node: errno is not set\n");

	list_deleteAll(list);
}

void testRemoveNode_SingularList()
{
	dllist_t *list = list_new();
	assert(list && "Memory error occured");
	data_t array[1] = { 1 };
	int    res      = list_initWithArray(list, array, 1);
	assert(res == 0 && "Error occured initting list");
	node_t *node = list->head;
	data_t value = node->value;

	res = list_removeNode(list, node);
	crashIfFalse(list_isOK(list), "list_removeNode singular list: list is not OK\n");
	crashIfFalse(res == value, "list_removeNode singularlist: return value is not OK\n");

	list_deleteAll(list);
}

void testRemoveNode_EmptyList()
{
	dllist_t *list = list_new();
	assert(list && "Memory error occurred");
	node_t *node = malloc(sizeof(node_t));
	node->value = 6;

	int res = list_removeNode(list, node);
	crashIfFalse(res != 0, "list_removeNode empty list: error is not detected\n");
	crashIfFalse(errno == EINVAL, "list_removeNode empty list: errno is not set\n");

	list_deleteAll(list);
	free(node);
}

void testRemoveNode_InvalidList()
{
	dllist_t *list      = setUp();
	node_t   *list_head = list->head;
	list->head = NULL;
	node_t *node = malloc(sizeof(node_t));
	node->value = 6;

	int res = list_removeNode(list, node);
	crashIfFalse(res != 0, "list_removeNode invalid list: error isnot detected\n");
	crashIfFalse(errno == EINVAL, "list_removeNode invalid list: errno is not set\n");

	list->head = list_head;
	list_deleteAll(list);
	free(node);
}


//node_t *list_pushFront(dllist_t *list, data_t value)

void testPushFront()
{
	dllist_t *list = setUp();
	dllist_t *copy = list_copy(list);

	data_t d     = 6;
	node_t *node = list_pushFront(list, d);
	crashIfFalse(list_isOK(list), "list_pushFront: list is not OK\n");
	crashIfFalse(node != NULL, "list_pushFront: node is NLL");

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

	list_deleteAll(list);
	list_deleteAll(copy);
	free(str);
}

void testListPushFront_EmptyList()
{
	dllist_t *list = list_new();
	assert(list && "Memory error occured");

	data_t d     = 6;
	node_t *node = list_pushFront(list, d);
	crashIfFalse(list_isOK(list), "list_pushFront empty: list is not OK\n");
	crashIfFalse(node != NULL, "list_pushFront empty list: node is NULL");

	char *str = malloc(100 * sizeof(char));
	sprintf(str, "empty list_pushFront: want first value %d have %d\n", d, list->head->value);
	crashIfFalse(list->head->value == d, str);

	sprintf(str, "empty list_pushFront: first node is not equal the return value\n");
	crashIfFalse(list->head == node, str);

	sprintf(str, "empty list_pushFront: last node isnot equalthe return value\n");
	crashIfFalse(list->tail == node, str);

	crashIfFalse(list->head->next == list->head, "empty list_pushFront: list is not singular\n");

	list_deleteAll(list);
	free(str);
}

void testListPushFront_NullList()
{
	dllist_t *list = NULL;
	data_t   value = 6;

	node_t *node = list_pushFront(list, value);
	crashIfFalse(node == NULL, "list_pushFront NULL list: error isnot detected\n");
	crashIfFalse(errno == EINVAL, "list_pushFront NULL list: errno is not set\n");
}

void testListPushFront_InvalidList()
{
	dllist_t *list      = setUp();
	node_t   *list_head = list->head;
	list->head = NULL;
	data_t value = 6;

	node_t *node = list_pushFront(list, value);
	crashIfFalse(node == NULL, "list_pushFront invalid list: error isnot detected\n");
	crashIfFalse(errno != 0, "list_pushFront invalid list: errno is not set\n");

	list->head = list_head;
	list_deleteAll(list);
}

void testPushFront_InvalidMalloc()
{
	dllist_t *list = setUp();
	data_t   value = 6;
	testingMalloc = 1;

	node_t *node = list_pushFront(list, value);
	crashIfFalse(node == NULL, "list_pushFront invalid malloc: error isnot detected\n");
	crashIfFalse(errno != 0, "list_pushFront invalid malloc: errno is not set\n");

	testingMalloc = 0;
	list_deleteAll(list);
}

//node_t *list_pushBack(dllist_t *list, data_t value)

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

	list_deleteAll(list);
	list_deleteAll(copy);
	free(str);
}

void testListPushBack_EmptyList()
{
	dllist_t *list = list_new();
	assert(list && "Memory error occured");

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

	list_deleteAll(list);
	free(str);
}

void testListPushBack_NullList()
{
	dllist_t *list = NULL;
	data_t   value = 6;

	node_t *node = list_pushBack(list, value);
	crashIfFalse(node == NULL, "list_pushBack NULL list: error isnot detected\n");
	crashIfFalse(errno == EINVAL, "list_pushBack NULL list: errno is not set\n");
}

void testListPushBack_InvalidList()
{
	dllist_t *list      = setUp();
	node_t   *list_head = list->head;
	list->head = NULL;
	data_t value = 6;

	node_t *node = list_pushBack(list, value);
	crashIfFalse(node == NULL, "list_pushBack Invalid list: error isnot detected\n");
	crashIfFalse(errno == EINVAL, "list_pushBack Ivalid list: errno is not set\n");

	list->head = list_head;
	list_deleteAll(list);
}

void testPushBack_InvalidMalloc()
{
	dllist_t *list = setUp();
	data_t   value = 6;
	testingMalloc = 1;

	node_t *node = list_pushBack(list, value);
	crashIfFalse(node == NULL, "list_pushBack invalid malloc: error isnot detected\n");
	crashIfFalse(errno != 0, "list_pushBack invalid malloc: errno is not set\n");

	testingMalloc = 0;
	list_deleteAll(list);
}


//data_t list_popFront(dllist_t *list)

//void testPopFront()
//{
//	dllist_t *list     = setUp();
//	dllist_t *copy     = list_copy(list);
//	data_t   wantValue = list->head->value;
//
//	errno              = 0;
//	data_t haveValue = list_popFront(list);
//	crashIfFalse(list_isOK(list), "list_popFront: list is not OK\n");
//	crashIfFalse(errno == 0, "list_popFront: error occurred\n");
//
//	char *str = malloc(100 * sizeof(char));
//	sprintf(str, "list_popFront: wantValue = %d, haveValue = %d\n", wantValue, haveValue);
//	crashIfFalse(wantValue == haveValue, str);
//
//	node_t *want = copy->head->next;
//	node_t *have = list->head;
//
//	for (; want != copy->head; want = want->next, have = have->next)
//	{
//		sprintf(str, "list_popFront: want->value = %d, have->value = %d\n", want->value, have->value);
//		crashIfFalse(want->value == have->value, str);
//	}
//
//	list_deleteAll(list);
//	list_deleteAll(copy);
//	free(str);
//}
//
//void testPopFront_NullList()
//{
//	dllist_t *list = NULL;
//
//	errno          = 0;
//	list_popFront(list);
//	crashIfFalse(errno != 0, "list_popFront NULL list: errno is not set\n");
//}
//
//void testPopFront_InvalidList()
//{
//	dllist_t *list      = setUp();
//	node_t   *list_head = list->head;
//	list->head = NULL;
//
//	errno               = 0;
//	list_popFront(list);
//	crashIfFalse(errno != 0, "list_popFront invalid list: errno is not set\n");
//
//	list->head = list_head;
//	list_deleteAll(list);
//}
//
//void testPopFront_EmptyList()
//{
//	dllist_t *list = list_new();
//	assert(list && "Memory error occured");
//
//	errno          = 0;
//	list_popFront(list);
//	crashIfFalse(errno != 0, "list_popFront invalid list: errno is not set\n");
//
//	list_deleteAll(list);
//}
//
//
////data_t list_popBack(dllist_t *list)
//
//void testPopBack()
//{
//	dllist_t *list     = setUp();
//	dllist_t *copy     = list_copy(list);
//	data_t   wantValue = list->tail->value;
//
//	errno              = 0;
//	data_t haveValue = list_popBack(list);
//	crashIfFalse(list_isOK(list), "list_popBack: list is not OK\n");
//	crashIfFalse(errno == 0, "list_popBack: error occurred\n");
//
//	char *str = malloc(100 * sizeof(char));
//	sprintf(str, "list_popBackt: wantValue = %d, haveValue = %d\n", wantValue, haveValue);
//	crashIfFalse(wantValue == haveValue, str);
//
//	node_t *want = copy->head;
//	node_t *have = list->head;
//
//	for (; want != copy->tail; want = want->next, have = have->next)
//	{
//		sprintf(str, "list_popBack: want->value = %d, have->value = %d\n", want->value, have->value);
//		crashIfFalse(want->value == have->value, str);
//	}
//
//	crashIfFalse(have == list->head, "list_popBack: have extra value");
//
//	list_deleteAll(list);
//	list_deleteAll(copy);
//	free(str);
//}
//
//void testPopBack_NullList()
//{
//	dllist_t *list = NULL;
//
//	errno          = 0;
//	list_popBack(list);
//	crashIfFalse(errno != 0, "list_popBack NULL list: errno is not set\n");
//}
//
//void testPopBack_InvalidList()
//{
//	dllist_t *list      = setUp();
//	node_t   *list_head = list->head;
//	list->head = NULL;
//
//	errno               = 0;
//	list_popBack(list);
//	crashIfFalse(errno != 0, "list_popBack invalid list: errno is not set\n");
//
//	list->head = list_head;
//	list_deleteAll(list);
//}
//
//void testPopBack_EmptyList()
//{
//	dllist_t *list = list_new();
//	assert(list && "Memory error occured");
//
//	errno          = 0;
//	list_popBack(list);
//	crashIfFalse(errno != 0, "list_popBack invalid list: errno is not set\n");
//
//	list_deleteAll(list);
//}
//

/*
 * returns 1 if iterator is OK, 0 if iterator is not OK
 */
int iteratorIsOK(dllist_iterator_t *iterator)
{
	if (iterator == NULL)
	{
		return 0;
	}
	if (iterator->list == NULL)
	{
		return 0;
	}
	return 1;
}


void testIteratorNew()
{
	dllist_t          *list     = setUp();
	dllist_iterator_t *iterator = dllist_iterator_new(list);
	crashIfFalse(iteratorIsOK(iterator), "dllist_iterator_new: iterator is not OK\n");

	free(iterator);
	list_deleteAll(list);
}


void testIteratorNew_EmptyList()
{
	dllist_t *list = list_new();
	assert(list && "Errorallocating memory occurred");
	errno          = 0;
	dllist_iterator_t *iterator = dllist_iterator_new(list);
	crashIfFalse(iteratorIsOK(iterator), "dllist_iterator_new empty list: iteratoc is not OK\n");

	list_deleteAll(list);
}

void testIteratorNew_NullList()
{
	dllist_t *list = NULL;
	errno          = 0;
	dllist_iterator_t *iterator = dllist_iterator_new(list);
	crashIfFalse(iterator == NULL, "dllist_iterator_new null list: error is not detected\n");
	crashIfFalse(errno != 0, "dllist_iterator_new null list: errno is not set\n");
}

void testIteratorNew_InvalidMalloc()
{
	dllist_t *list = list_new();
	testingMalloc = 1;
	errno          = 0;
	dllist_iterator_t *iterator = dllist_iterator_new(list);
	crashIfFalse(iterator == NULL, "dllist_iterator_new invalid malloc: error is not detected\n");
	crashIfFalse(errno != 0, "dllist_iterator_new invalid malloc: errno is not set\n");
	testingMalloc = 0;
	list_deleteAll(list);
}


void testIteratorGet()
{
	dllist_t *list = setUp();
	assert(list && "Memory allocation error occurred");
	dllist_iterator_t *iterator = dllist_iterator_new(list);

	errno          = 0;
	node_t *have = dllist_iterator_get(iterator);
	crashIfFalse(iteratorIsOK(iterator), "dllist_iterator_get: iterator is not OK\n");
	crashIfFalse(errno == 0, "dllist_iterator_get: error occurred\n");

	node_t *want = list->head;
	crashIfFalse(have == want, "dllist_iterator_get: got invalidnode\n");

	free(iterator);
	list_deleteAll(list);
}

void testIteratorGet_NullIterator()
{
	dllist_iterator_t *iterator = NULL;

	errno                       = 0;
	node_t *have = dllist_iterator_get(iterator);
	crashIfFalse(have == NULL, "dllist_iterator_get null iterator: error is not detected\n");
	crashIfFalse(errno != 0, "dllist_iterator_get null iterator: errno isnot set\n");
}

void testIteratorGet_InvalidIterator()
{
	dllist_iterator_t *iterator = malloc(sizeof(dllist_iterator_t));
	assert(iterator && "Memory allocation error occurred");
	iterator->list = NULL;
	iterator->curr = NULL;

	errno                       = 0;
	node_t *have = dllist_iterator_get(iterator);
	crashIfFalse(have == NULL, "dllist_iterator_get invalid iterator: error is not detected\n");
	crashIfFalse(errno != 0, "dllist_iterator_get invalid iterator: errno isnot set\n");
	free(iterator);
}

void testIteratorNext()
{
	dllist_t *list = setUp();
	assert(list && "Memory allocation error occurred");
	dllist_iterator_t *iterator = dllist_iterator_new(list);

	errno          = 0;
	node_t *have = dllist_iterator_next(iterator);
	crashIfFalse(iteratorIsOK(iterator), "dllist_iterator_next: iterator is not OK\n");
	crashIfFalse(errno == 0, "dllist_iterator_next: error occurred\n");

	node_t *want = list->head->next;
	crashIfFalse(have == want, "dllist_iterator_next: got invalid node\n");
	crashIfFalse(iterator->curr == want, "dllist_iterator_next: current oterator node is invalid\n");

	free(iterator);
	list_deleteAll(list);
}

void testIteratorNext_NullIterator()
{
	dllist_iterator_t *iterator = NULL;

	errno                       = 0;
	node_t *have = dllist_iterator_next(iterator);
	crashIfFalse(have == NULL, "dllist_iterator_next null iterator: error is not detected\n");
	crashIfFalse(errno != 0, "dllist_iterator_next null iterator: errno isnot set\n");
}

void testIteratorNext_InvalidIterator()
{
	dllist_iterator_t *iterator = malloc(sizeof(dllist_iterator_t));
	assert(iterator && "Memory allocation error occurred");
	iterator->list = NULL;
	iterator->curr = NULL;

	errno                       = 0;
	node_t *have = dllist_iterator_next(iterator);
	crashIfFalse(have == NULL, "dllist_iterator_next invalid iterator: error is not detected\n");
	crashIfFalse(errno != 0, "dllist_iterator_next invalid iterator: errno isnot set\n");
	free(iterator);
}

void testIteratorNext_NullCurrentNode()
{
	dllist_t *list = list_new();
	assert(list && "Memory allocation error occurred");
	dllist_iterator_t *iterator = dllist_iterator_new(list);
	assert(iterator && "Memory allocation error occurred");

	errno          = 0;
	node_t *have = dllist_iterator_next(iterator);
	crashIfFalse(have == NULL, "dllist_iterator_next null current node: invalid return value\n");
	crashIfFalse(errno == 0, "dllist_iterator_next null current node: errno is not null\n");

	free(iterator);
}

void testIteratorPrevious()
{
	dllist_t *list = setUp();
	assert(list && "Memory allocation error occurred");
	dllist_iterator_t *iterator = dllist_iterator_new(list);
	node_t            *want     = iterator->curr;
	iterator->curr = iterator->curr->next;

	errno          = 0;
	node_t *have = dllist_iterator_previous(iterator);
	crashIfFalse(iteratorIsOK(iterator), "dllist_iterator_previous: iterator is not OK\n");
	crashIfFalse(errno == 0, "dllist_iterator_previous: error occurred\n");

	crashIfFalse(have == want, "dllist_iterator_previous: got invalid node\n");
	crashIfFalse(iterator->curr == want, "dllist_iterator_previous: current iterator node is invalid\n");

	free(iterator);
	list_deleteAll(list);
}

void testIteratorPrevious_FirstNode()
{
	dllist_t *list = setUp();
	assert(list && "Memory allocation error occurred");
	dllist_iterator_t *iterator = dllist_iterator_new(list);

	errno          = 0;
	node_t *have = dllist_iterator_previous(iterator);
	crashIfFalse(iteratorIsOK(iterator), "dllist_iterator_previous first node: iterator is not OK\n");
	crashIfFalse(errno == 0, "dllist_iterator_previous: error occurred\n");

	crashIfFalse(have == NULL, "dllist_iterator_previous first node: got invalid node\n");
	crashIfFalse(iterator->curr == list->head, "dllist_iterator_previous first node: current iterator node is invalid\n");

	free(iterator);
	list_deleteAll(list);
}

void testIteratorPrevious_NullIterator()
{
	dllist_iterator_t *iterator = NULL;

	errno                       = 0;
	node_t *have = dllist_iterator_previous(iterator);
	crashIfFalse(have == NULL, "dllist_iterator_previous null iterator: error is not detected\n");
	crashIfFalse(errno != 0, "dllist_iterator_previous null iterator: errno isnot set\n");
}

void testIteratorPrevious_InvalidIterator()
{
	dllist_iterator_t *iterator = malloc(sizeof(dllist_iterator_t));
	assert(iterator && "Memory allocation error occurred");
	iterator->list = NULL;
	iterator->curr = NULL;

	errno                       = 0;
	node_t *have = dllist_iterator_previous(iterator);
	crashIfFalse(have == NULL, "dllist_iterator_previous invalid iterator: error is not detected\n");
	crashIfFalse(errno != 0, "dllist_iterator_previous invalid iterator: errno isnot set\n");
	free(iterator);
}


void testIteratorIsLast()
{
	dllist_t *list = setUp();
	assert(list && "Memory allocation error occurred");
	dllist_iterator_t *iterator = dllist_iterator_new(list);
	iterator->curr = list->tail;

	errno          = 0;
	int res = dllist_itertor_isLast(iterator);
	crashIfFalse(iteratorIsOK(iterator), "dllist_iterator_isLast: iterator is not OK\n");
	crashIfFalse(errno == 0, "dllist_iterator_isLast: error occurred\n");
	crashIfFalse(res == 1, "dllist_iterator_isLast: result is not correct\n");

	list_deleteAll(list);
	free(iterator);
}

void testIteratorIsLast_isNotLast()
{
	dllist_t *list = setUp();
	assert(list && "Memory allocation error occurred");
	dllist_iterator_t *iterator = dllist_iterator_new(list);

	errno          = 0;
	int res = dllist_itertor_isLast(iterator);
	crashIfFalse(iteratorIsOK(iterator), "dllist_iterator_isLast: iterator is not OK\n");
	crashIfFalse(errno == 0, "dllist_iterator_isLast: error occurred\n");
	crashIfFalse(res == 0, "dllist_iterator_isLast: result is not correct\n");

	list_deleteAll(list);
	free(iterator);
}

void testIteratorIsLast_NullIterator()
{
	dllist_iterator_t *iterator = NULL;

	errno                       = 0;
	int res = dllist_itertor_isLast(iterator);
	crashIfFalse(res == -1, "dllist_iterator_isLast null iterator: error is not detected\n");
	crashIfFalse(errno != 0, "dllist_iterator_isLast null iterator: errno isnot set\n");
}

void testIteratorIsLast_InvalidIterator()
{
	dllist_iterator_t *iterator = malloc(sizeof(dllist_iterator_t));
	assert(iterator && "Memory allocation error occurred");
	iterator->list = NULL;
	iterator->curr = NULL;

	errno                       = 0;
	int res = dllist_itertor_isLast(iterator);
	crashIfFalse(res == -1, "dllist_iterator_isLast invalid iterator: error is not detected\n");
	crashIfFalse(errno != 0, "dllist_iterator_isLast invalid iterator: errno isnot set\n");
	free(iterator);
}


void testDeleteNodeWithIterator()
{
	dllist_t *list = setUp();
	assert(list && "Memory allocation error occurred");
	dllist_iterator_t *iterator = dllist_iterator_new(list);
	node_t            *node     = dllist_iterator_get(iterator);
	crashIfFalse(node != NULL, "testDeleteNodeWithIterator: error in dllist_iterator_get\n");
	node_t *res = dllist_iterator_next(iterator);
	crashIfFalse(res != NULL, "testDeleteNodeWithIterator: error in dllist_iterator_next\n");
	list_removeNode(list, node);
	crashIfFalse(iteratorIsOK(iterator), "testDeleteNodeWithIterator: iterator is invalid\n");
	crashIfFalse(list_isOK(list), "testDeleteNodeWithIterator: list is invalid\n");
	list_deleteAll(list);
	free(iterator);
}

int main()
{
	testListNew();
	testListNew_InvalidMalloc();
	testListDeleteAll();
	testListDeleteAll_NullList();

	testListInitWithArray_FullList();
	testListInitWithArray_EmptyArray();
	testListInitWithArray_EmptyList();
	testListInitWithArray_NullListPtr();
	testListInitWithArray_NullList();
	testListInitWithArray_InvalidN();
	testListInitWithArray_NDoesntMatchArray();
	testListInitWithArray_InvalidList();
	testListInitWithArray_InvalidMalloc();

	testForeach();
	testForeach_Stop();
	testForeach_NullList();
	testForeach_EmptyList();
	testForeach_InvalidList();

	testCopy();
	testCopy_EmptyList();
	testCopy_NullList();
	testCopy_InvalidList();
	testCopy_InvalidMalloc();

	testInsertAfter();
	testInsertAfter_NullList();
	testInsertAfter_NullNode();
	testInsertAfter_InvalidList();
	testInsertAfter_EmptyList();

	testInsertBefore();
	testInsertBefore_NullList();
	testInsertBefore_NullNode();
	testInsertBefore_InvalidList();
	testInsertBefore_EmptyList();

	testRemoveNode_Head();
	testRemoveNode_Middle();
	testRemoveNode_Tail();
	testRemoveNode_SingularList();
	testRemoveNode_EmptyList();
	testRemoveNode_InvalidList();
	testRemoveNode_NullList();
	testRemoveNode_NullNode();

	testPushFront();
	testListPushFront_EmptyList();
	testListPushFront_NullList();
	testListPushFront_InvalidList();
	testPushFront_InvalidMalloc();

	testPushBack();
	testListPushBack_EmptyList();
	testListPushBack_NullList();
	testListPushBack_InvalidList();
	testPushBack_InvalidMalloc();

//	testPopFront();
//	testPopFront_EmptyList();
//	testPopFront_InvalidList();
//	testPopFront_NullList();
//
//	testPopBack();
//	testPopBack_EmptyList();
//	testPopBack_InvalidList();
//	testPopBack_NullList();

	testIteratorNew();
	testIteratorNew_EmptyList();
	testIteratorNew_NullList();
	testIteratorNew_InvalidMalloc();

	testIteratorGet();
	testIteratorGet_InvalidIterator();
	testIteratorGet_NullIterator();

	testIteratorNext();
	testIteratorNext_InvalidIterator();
	testIteratorNext_NullIterator();
	testIteratorNext_NullCurrentNode();

	testIteratorPrevious();
	testIteratorPrevious_InvalidIterator();
	testIteratorPrevious_NullIterator();
	testIteratorPrevious_FirstNode();

	testIteratorIsLast();
	testIteratorIsLast_isNotLast();
	testIteratorIsLast_NullIterator();
	testIteratorIsLast_InvalidIterator();

	testDeleteNodeWithIterator();
	printf("Everything is OK\n");
	return 0;
}