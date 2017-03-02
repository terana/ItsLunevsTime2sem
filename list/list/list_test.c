#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include "list.h"

void crashIfFalse(int condition, const char *description)
{
	if (!condition)
	{
		printf("%s", description);
	}
	assert(condition);
}

doubleLinkedList_t *setUp()
{
	doubleLinkedList_t *list    = list_new();
	data_t             array[5] = { 1, 2, 3, 4, 5 };
	list_initWithArray(list, array, 5);
	return list;
}

void testInitWithArray()
{
	doubleLinkedList_t *list    = list_new();
	data_t             array[5] = { 1, 2, 3, 4, 5 };
	list_initWithArray(list, array, 5);

	node_t *curr = list->head;
	int    i     = 0;

	char *str = malloc(100 * sizeof(char));
	sprintf(str, "list_initWithArray:  want %d, have %d\n", array[i], curr->value);
	crashIfFalse(array[i++] == curr->value, str);
	curr = curr->next;
	while (curr != list->head)
	{
		sprintf(str, "list_initWithArray:  want %d, have %d\n", array[i], curr->value);
		crashIfFalse(array[i++] == curr->value, str);
		curr = curr->next;
	}
	free(str);
	list_delete(list);
}

void testCopy()
{
	doubleLinkedList_t *list = setUp();
	doubleLinkedList_t *copy = list_copy(list);

	node_t *want = list->head;
	node_t *have = copy->head;
	char   *str  = malloc(100 * sizeof(char));
	sprintf(str, "list_copy: want->value = %d, have->value = %d\n", want->value, have->value);
	crashIfFalse(want->value == have->value, str);
	crashIfFalse(want != have, "list_copy:want and have are the same nodes\n");
	want = want->next;
	have = have->next;
	while (want != list->head)
	{
		sprintf(str, "list_copy: want->value = %d, have->value = %d\n", want->value, have->value);
		crashIfFalse(want->value == have->value, str);
		crashIfFalse(want != have, "list_copy:want and have are the same nodes\n");
		want = want->next;
		have = have->next;
	}

	free(str);
	list_delete(list);
	list_delete(copy);
}

void testInsertAfter()
{
	doubleLinkedList_t *list = setUp();
	doubleLinkedList_t *copy = list_copy(list);
	node_t             *node = malloc(sizeof(node_t));
	node->value = 6;

	list_insertAfter(list, node);

	node_t *want = copy->head;
	node_t *have = list->head;
	char   *str  = malloc(100 * sizeof(char));
	sprintf(str, "list_insertAfter: want->value = %d, have->value = %d\n", want->value, have->value);
	crashIfFalse(want->value == have->value, str);
	want = want->next;
	have = have->next;
	while (want != copy->head)
	{
		sprintf(str, "list_insertAfter: want->value = %d, have->value = %d\n", want->value, have->value);
		crashIfFalse(want->value == have->value, str);
		want = want->next;
		have = have->next;
	}
	want = node;
	sprintf(str, "list_insertAfter: want last value  %d, have last value %d\n", want->value, have->value);
	crashIfFalse(want->value == have->value, str);
	crashIfFalse(want == have, "last node is not the same");

	list_delete(list);
	list_delete(copy);
	free(str);
}

void testInsertBefore()
{
	doubleLinkedList_t *list = setUp();
	doubleLinkedList_t *copy = list_copy(list);
	node_t             *node = malloc(sizeof(node_t));
	node->value = 6;

	list_insertBefore(list, node);

	node_t *want = node;
	node_t *have = list->head;
	char   *str  = malloc(100 * sizeof(char));
	sprintf(str, "list_insertBefore: want first value  %d, have first value %d\n", want->value, have->value);
	crashIfFalse(want->value == have->value, str);
	crashIfFalse(want == have, "list_insertBefore: first node is not the same");

	want = copy->head;
	have = have->next;
	while (want != copy->head)
	{
		sprintf(str, "list_insertBefore: want->value = %d, have->value = %d\n", want->value, have->value);
		crashIfFalse(want->value == have->value, str);
		want = want->next;
		have = have->next;
	}

	list_delete(list);
	list_delete(copy);
	free(str);
}

void testRemoveNodeHead()
{
	doubleLinkedList_t *list = setUp();
	doubleLinkedList_t *copy = list_copy(list);
	node_t             *node = list->head;

	list_removeNode(list, node);

	char   *str  = malloc(100 * sizeof(char));
	node_t *want = copy->head->next;
	node_t *have = list->head;

	while (want != copy->head)
	{
		sprintf(str, "list_removeNode head: want->value = %d, have->value = %d\n", want->value, have->value);
		crashIfFalse(want->value == have->value, str);
		want = want->next;
		have = have->next;
	}

	crashIfFalse(have == list->head, "list_removeNode head: list isnot circled");

	list_delete(list);
	list_delete(copy);
	free(str);
}

void testRemoveNodeTail()
{
	doubleLinkedList_t *list = setUp();
	doubleLinkedList_t *copy = list_copy(list);
	node_t             *node = list->tail;

	list_removeNode(list, node);

	char   *str  = malloc(100 * sizeof(char));
	node_t *want = copy->head;
	node_t *have = list->head;

	while (want != copy->tail)
	{
		sprintf(str, "list_removeNode tail: want->value = %d, have->value = %d\n", want->value, have->value);
		crashIfFalse(want->value == have->value, str);
		want = want->next;
		have = have->next;
	}

	crashIfFalse(have == list->head, "list_removeNode tail: have extra value");

	list_delete(list);
	list_delete(copy);
	free(str);
}

void testRemoveNodeMiddle()
{
	doubleLinkedList_t *list = setUp();
	doubleLinkedList_t *copy = list_copy(list);
	node_t             *node = list->head->next;

	list_removeNode(list, node);

	char   *str  = malloc(100 * sizeof(char));
	node_t *want = copy->head;
	node_t *have = list->head;

	sprintf(str, "list_removeNode middle: want->value = %d, have->value = %d\n", want->value, have->value);
	crashIfFalse(want->value == have->value, str);

	want = want->next->next;
	have = have->next;

	while (want != copy->tail)
	{
		sprintf(str, "list_removeNode tail: want->value = %d, have->value = %d\n", want->value, have->value);
		crashIfFalse(want->value == have->value, str);
		want = want->next;
		have = have->next;
	}

	list_delete(list);
	list_delete(copy);
	free(str);
}

struct node_str
{
	node_t *node;
	char   *str;
};

void func(data_t value, void *param)
{
	struct node_str *ns   = (struct node_str *) param;
	node_t          *node = ns->node;
	char            *str  = ns->str;

	sprintf(str, "list_foreach: value = %d, node->value = %d\n", value, node->value);
	crashIfFalse(value == node->value, str);

	ns->node = node->next;

	return;
}

void testForeach()
{
	doubleLinkedList_t *list = setUp();
	struct node_str    *ns   = malloc(sizeof(struct node_str));
	ns->str  = malloc(100 * sizeof(char));
	ns->node = list->head;
	list_foreach(list, func, ns);
	free(ns->str);
	free(ns);
	list_delete(list);
}

int main()
{
	testInitWithArray();
	testCopy();
	testInsertAfter();
	testInsertBefore();
	testRemoveNodeHead();
	testRemoveNodeMiddle();
	testRemoveNodeTail();
	testForeach();
	printf("Everithing is OK\n");
	return 0;
}