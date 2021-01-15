#ifndef KISS_LIST_H
#define KISS_LIST_H

#include <stdlib.h>

typedef struct listitem_ {
    void *item;
    void (*free)(void *item);
    struct listitem_ *prev;
    struct listitem_ *next;
} listitem_t;

typedef struct list_ {
    listitem_t *head;
    listitem_t *tail;
} list_t;

extern list_t *list_new();
extern void list_free(list_t *list);
extern list_t *list_push(list_t *list, void *item, void (*free)(void *item));
extern list_t *list_unshift(list_t *list, void *item, void (*free)(void *item));
extern list_t *list_remove(list_t *list, void *item);
extern void list_remove_item(list_t *list, listitem_t *l);
extern list_t *list_remove_head(list_t *list);
extern list_t *list_remove_tail(list_t *list);

extern listitem_t *list_get(list_t *list, int index);

#endif /* KISS_LIST_H */
