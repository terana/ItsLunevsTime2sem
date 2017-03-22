int testingMalloc;

typedef int data_t;

typedef struct node
{
	data_t      value;
	struct node *next;
	struct node *previous;
}           node_t;

typedef struct dllist dllist_t;

typedef struct dllist_iterator dllist_iterator_t;

/*
 * Returns new iterator on list with current value equal to the first node of the list.
 * If error occurs returns NULL and changes errno.
 */
dllist_iterator_t *dllist_iterator_new(dllist_t *list);

/*
 * Returns current node of iterator.
 * If error occurs returns NULL and changes errno.
 */
node_t *dllist_iterator_get(dllist_iterator_t *iterator);

/*
 * Returns next node of iterator. Sets the iterator current node to next node.
 * If error occurs returns NULL and changes errno.
 */
node_t * dllist_iterator_next(dllist_iterator_t *iterator);

/*
 * Returns next node of iterator. Sets the iterator current node to next node.
 * If current value is the first returns NULL and doesn't change the iterator.
 * If error occurs returns NULL and changes errno.
 */
node_t * dllist_iterator_previous(dllist_iterator_t *iterator);

/*
 * Returns 0 if current value of iterator is not last, 1 if it is last, -1 if error occurs.
 */
int dllist_itertor_isLast(dllist_iterator_t *iterator);

/*
 * Returns pointer on new empty list.
 * If error while allocating memory occures, returns NULL while errno is set to ENOMEM.
 */
dllist_t *list_new();

int list_deleteAll(dllist_t *list);

/*
 * returns 1 if list is OK, 0 if list is not OK
 */
int list_isOK(dllist_t *list);

/*
 * Initiates list with array. If list is not empty, the pointer may change.
 * Returns 0 if everything is OK, returns 1 if error occures and changes errno.
 */
int list_initWithArray(dllist_t *list, const data_t *array, int n);

/*
* Returns 0 if everything is OK, returns 1 if it stops before list tail, returns-1 if error occures and changes errno.
*/
int list_foreach(dllist_t *list, int (*func)(node_t *curr, void *param), void *param);

/*
* Returns copy oflist if everything is OK, returns NULL if error occures and changes errno.
*/
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