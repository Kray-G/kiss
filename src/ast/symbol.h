#ifndef KISS_SYMBOL_H
#define KISS_SYMBOL_H

#include <stdint.h>
#include "xstring.h"

typedef struct symbol_ {
    struct symbol_ *next;
    string_t *name;
    int vtype;
} symbol_t;

typedef struct symbol_table_ {
    struct symbol_table_ *parent;
    symbol_t *symbol;
} symbol_table_t;

extern symbol_table_t *symbol_table_new(symbol_table_t *parent);
extern void symbol_add(symbol_table_t *symtbl, const char *name, int vtype);
extern symbol_t *symbol_search(symbol_table_t *symtbl, const char *name);

#endif /* KISS_SYMBOL_H */
