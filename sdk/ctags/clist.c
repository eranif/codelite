#include "clist.h"
#include <stdlib.h>

/*
 *----------------------------------------------------------------
 * private methods
 *----------------------------------------------------------------
 *
 */
static list_node_t *list_new_node()
{
	list_node_t *node = (list_node_t *)malloc(sizeof(list_node_t));
	node->data = 0;
	node->next = 0;
	node->prev = 0;

	return node;
}

/*
 *----------------------------------------------------------------
 * API
 *----------------------------------------------------------------
 *
 */
void list_init(list_t** lst)
{
	*lst = (list_t*)malloc(sizeof(list_t));
	list_t *l = *lst;
	l->head = (list_node_t*)0;
	l->last = (list_node_t*)0;
	l->size = 0;
}

void list_append(list_t* lst, void* data)
{
	list_node_t *node = list_new_node();
	node->data = data;

	if (lst->last) {
		/* we have last item, just append the new one */
		lst->last->next = node;
		node->prev = lst->last;
		lst->last = node;

	} else {
		/* no items in the list */
		lst->last = node;
		lst->head = node;
	}
	lst->size++;
}

void list_destroy(list_t* lst)
{
	list_node_t *node = lst->head;
	while(node) {
		list_node_t *next = node->next;
		free(node);
		node = next;
	}
	lst->size = 0;
	lst->head = 0;
	lst->last = 0;
}
