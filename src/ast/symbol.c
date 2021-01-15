#include "symbol.h"
#include "node.h"

static symbol_t *symbol_new(const char *name, types_t type)
{
    symbol_t *sym = (symbol_t *)calloc(1, sizeof(symbol_t));
    sym->name = string_new(name);
    sym->type = type;
    return sym;
}

static void symbol_free(symbol_t *sym)
{
    if (sym->argtypes) {
        list_free(sym->argtypes);
    }
    if (sym->type.argtypes) {
        list_free(sym->type.argtypes);
    }
    string_free(sym->name);
    free(sym);
}

symbol_table_t *symbol_table_new(symbol_table_t *parent)
{
    symbol_table_t *symtbl = (symbol_table_t *)calloc(1, sizeof(symbol_table_t));
    symtbl->parent = parent;
    return symtbl;
}

void symbol_table_free(symbol_table_t *symtbl)
{
    symbol_t *sym = symtbl->symbol;
    while (sym) {
        symbol_t *next = sym->next;
        symbol_free(sym);
        sym = next;
    }
}

static symbol_t *symbol_search_one(symbol_table_t *symtbl, const char *name)
{
    symbol_t *sym = symtbl->symbol;
    while (sym) {
        if (!strcmp(sym->name->p, name)) {
            return sym;
        }
        sym = sym->next;
    }
    return NULL;
}

symbol_t *symbol_add(symbol_table_t *symtbl, const char *name, types_t type)
{
    symbol_t *sym = symbol_search_one(symtbl, name);
    if (sym) {
        if (sym->type.vtype != VALTYPE_UNKNOWN && sym->type.vtype != type.vtype) {
            // TODO: error.
            ;
        }
        return sym;
    }

    sym = symbol_new(name, type);
    sym->next = symtbl->symbol;
    symtbl->symbol = sym;
    return sym;
}

symbol_t *symbol_search(symbol_table_t *symtbl, const char *name)
{
    symbol_t *sym = symbol_search_one(symtbl, name);
    while (!sym) {
        symtbl = symtbl->parent;
        if (!symtbl) {
            break;
        }
        sym = symbol_search_one(symtbl, name);
    }
    return sym;
}

int symbol_get_argcount(symbol_t *sym)
{
    if (!sym->argtypes) {
        return 0;
    }
    int count = 0;
    listitem_t *l = sym->argtypes->head;
    while (l) {
        ++count;
        l = l->next;
    }
    return count;
}

types_t list_get_argtype(list_t *argtypes, int index)
{
    if (!argtypes) {
        return (types_t){0};
    }

    listitem_t *l1 = list_get(argtypes, index);
    if (!l1) {
        return (types_t){0};
    }

    return *(types_t*)(l1->item);
}

list_t *list_add_argtype(list_t *argtypes, types_t type)
{
    if (!argtypes) {
        return NULL;
    }

    types_t *typep = (types_t *)calloc(1, sizeof(types_t));
    memcpy(typep, &type, sizeof(types_t));
    list_push(argtypes, typep, NULL);
    return argtypes;
}

symbol_t *symbol_add_argtype(symbol_t *sym, types_t type)
{
    if (!sym->argtypes) {
        sym->argtypes = list_new();
    }
    types_t *typep = (types_t *)calloc(1, sizeof(types_t));
    memcpy(typep, &type, sizeof(types_t));
    list_push(sym->argtypes, typep, NULL);
    return sym;
}
