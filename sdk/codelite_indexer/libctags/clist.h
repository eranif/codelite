#ifndef __CLIST_H__
#define __CLIST_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/types.h>

typedef struct list_node {
	struct list_node *next;
	struct list_node *prev;
	void *data;
} list_node_t;

typedef struct tag_list {
	list_node_t *head;
	list_node_t *last;
	size_t size;
} list_t;

/**
 * \brief initialize a list
 * \param lst
 */
extern void list_init(list_t **lst);

/**
 * \brief append an iten to the end of the list
 */
extern void list_append(list_t *lst, void *data);

/**
 * \brief destroy the list and free all allocated resources
 */
extern void list_destroy(list_t *lst);

#ifdef __cplusplus
}
#endif

#endif /*__CLIST_H__*/
