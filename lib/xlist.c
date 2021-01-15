#include "xlist.h"

static void listitem_free(listitem_t *l)
{
    if (l->free) {
        l->free(l->item);
    }
    free(l);
}

list_t *list_new()
{
    return (list_t *)calloc(1, sizeof(list_t));
}

void list_free(list_t *list)
{
    listitem_t *l = list->head;
    while (l) {
        listitem_t *next = l->next;
        listitem_free(l);
        l = next;
    }
    free(list);
}

list_t *list_push(list_t *list, void *item, void (*free)(void *item))
{
    listitem_t *nl = calloc(1, sizeof(listitem_t));
    nl->item = item;
    nl->free = free;
    listitem_t *l = list->tail;
    if (l) {
        l->next = nl;
        nl->prev = l;
        list->tail = nl;
    } else {
        list->head = list->tail = nl;
    }
    return list;
}

list_t *list_unshift(list_t *list, void *item, void (*free)(void *item))
{
    listitem_t *nl = calloc(1, sizeof(listitem_t));
    nl->item = item;
    nl->free = free;
    listitem_t *l = list->head;
    if (l) {
        nl->next = l;
        l->prev = nl;
        list->head = nl;
    } else {
        list->head = list->tail = nl;
    }
    return list;
}

list_t *list_remove(list_t *list, void *item)
{
    listitem_t *l = list->head;
    while (l) {
        if (l->item == item) {
            list_remove_item(l);
            break;
        }
        l = l->next;
    }
    return list;
}

void list_remove_item(listitem_t *l)
{
    if (l) {
        listitem_t *prev = l->prev;
        listitem_t *next = l->next;
        listitem_free(l);
        if (prev) {
            prev->next = next;
        }
        if (next) {
            next->prev = prev;
        }
    }
}

list_t *list_remove_head(list_t *list)
{
    list_remove_item(list->head);
    return list;
}

list_t *list_remove_tail(list_t *list)
{
    list_remove_item(list->tail);
    return list;
}

listitem_t *list_get(list_t *list, int index)
{
    listitem_t *l = list->head;
    for (int i = 0; l && i < index; ++i) {
        l = l->next;
    }
    return l;
}
