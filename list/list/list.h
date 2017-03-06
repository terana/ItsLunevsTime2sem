typedef int data_t;

typedef struct node
{
	data_t      value;
	struct node *next;
	struct node *previous;
}           node_t;

//struct dllist;
typedef struct dllist dllist_t;

dllist_t *list_new();

void list_delete(dllist_t *list);

void list_initWithArray(dllist_t *list, const data_t *array, int n);

void list_foreach(dllist_t *list, void (*func)(node_t *curr, void *param), void *param);

dllist_t *list_copy(dllist_t *list);

void list_insertAfter(dllist_t *list, node_t *node);

void list_insertBefore(dllist_t *list, node_t *node);

void list_removeNode(dllist_t *list, node_t *node);

node_t *list_pushFront(dllist_t *list, data_t d);

node_t *list_pushBack(dllist_t *list, data_t d);

data_t list_popFront(dllist_t *list);

data_t list_popBack(dllist_t *list);
