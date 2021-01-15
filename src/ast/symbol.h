#ifndef KISS_SYMBOL_H
#define KISS_SYMBOL_H

#include <stdint.h>
#include "xstring.h"
#include "xlist.h"

enum value_type {
    VALTYPE_UNKNOWN,
    VALTYPE_INT,
    VALTYPE_DBL,
    VALTYPE_STR,
    VALTYPE_FUNC,
};

typedef struct vtypes_ {
    enum value_type vtype;
    enum value_type rtype;
    list_t *argtypes;
} types_t;

typedef struct symbol_ {
    struct symbol_ *next;
    string_t *name;
    types_t type;
    list_t *argtypes;
} symbol_t;

typedef struct symbol_table_ {
    struct symbol_table_ *parent;
    symbol_t *symbol;
} symbol_table_t;

extern symbol_table_t *symbol_table_new(symbol_table_t *parent);
extern void symbol_table_free(symbol_table_t *symtbl);
extern symbol_t *symbol_add(symbol_table_t *symtbl, const char *name, types_t type);
extern symbol_t *symbol_search(symbol_table_t *symtbl, const char *name);

extern symbol_t *symbol_add_argtype(symbol_t *sym, types_t type);
extern types_t symbol_get_argtype(list_t *argtypes, int index);

#endif /* KISS_SYMBOL_H */
