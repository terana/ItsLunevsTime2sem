typedef int data_t;

typedef struct node
{
	data_t      value;
	struct node *next;
	struct node *previous;
}           node_t;

typedef struct dllist dllist_t;

typedef struct iterator
{

} list_iterator_t;

/*
 * Returns pointer on new empty list.
 * If error while allocating memory occures, returns NULL while errno is set to ENOMEM.
 */
dllist_t *list_new();

void list_deleteAll(dllist_t *list);

/*
 * Initiates list with array. If list is not empty, the pointer may change.
 * Returns 0 if everything is OK, returns 1 if error occures and changes errno.
 */
int list_initWithArray(dllist_t **list, const data_t *array, int n);

int list_foreach(dllist_t *list, void (*func)(node_t *curr, void *param, int *stop), void *param);

dllist_t *list_copy(dllist_t *list);

/*
 * Inserts node at the end of the list.
 * Returns 0 if everything is OK, returns 1 if error occures and changes errno.
 */
int list_insertAfter(dllist_t *list, node_t *node);

/*
 * Inserts node at the beginning of the list.
 * Returns 0 if everything is OK, returns 1 if error occures and changes errno.
 */
int list_insertBefore(dllist_t *list, node_t *node);

/*
 * Removes node from list.
 * Returns 0 if everything is OK, returns 1 if error occures and changes errno.
 */
int list_removeNode(dllist_t *list, node_t *node);

/*
 * Creates new node with value val at the beginning of the list.
 *  Returns pointer on created node if everything is OK, returns NULL if error occures and changes errno.
 */
node_t *list_pushFront(dllist_t *list, data_t val);

/*
 * Creates new node with value val at the end of the list.
 *  Returns pointer on created node if everything is OK, returns NULL if error occures and changes errno.
 */
node_t *list_pushBack(dllist_t *list, data_t val);

/*
 * Removes the first node in the list.
 * Returns value of the first node  if everything is OK, returns -1 if error occures and changes errno.
 */
data_t list_popFront(dllist_t *list);

/*
 * Removes the last node in the list.
 * Returns value of the last node  if everything is OK, returns -1 if error occures and changes errno.
 */
data_t list_popBack(dllist_t *list);