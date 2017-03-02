typedef int data_t;

typedef struct node
{
	data_t      value;
	struct node *next;
	struct node *previous;
}           node_t;

typedef struct doubleLinkedList
{
	node_t *head;
	node_t *tail;
}           doubleLinkedList_t;

doubleLinkedList_t *list_new();
void list_delete(doubleLinkedList_t *list);
void list_initWithArray(doubleLinkedList_t *list, const data_t *array, int n);

doubleLinkedList_t *list_copy(doubleLinkedList_t *list);

void list_print(doubleLinkedList_t *list);
void list_printReverce(doubleLinkedList_t *list);

void list_insertAfter(doubleLinkedList_t *list, node_t *node);
void list_insertBefore(doubleLinkedList_t *list, node_t *node);

void list_removeNode(doubleLinkedList_t *list, node_t *node);

node_t *list_pushFront(doubleLinkedList_t *list, data_t d);
node_t *list_pushBack(doubleLinkedList_t *list, data_t d);

data_t list_popFront(doubleLinkedList_t *list);
data_t list_popBack(doubleLinkedList_t *list);
data_t list_popNode(node_t *t);

void list_foreach(doubleLinkedList_t *list, void (*func)(data_t d, void *param), void *param);