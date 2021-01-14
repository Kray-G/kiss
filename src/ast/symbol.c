#include "symbol.h"
#include "node.h"

static symbol_t *symbol_new(const char *name, int vtype)
{
    symbol_t *sym = (symbol_t *)calloc(1, sizeof(symbol_t));
    sym->name = string_new(name);
    sym->vtype = vtype;
    return sym;
}

symbol_table_t *symbol_table_new(symbol_table_t *parent)
{
    symbol_table_t *symtbl = (symbol_table_t *)calloc(1, sizeof(symbol_table_t));
    symtbl->parent = parent;
    return symtbl;
}

static symbol_t *symbol_search_one(symbol_table_t *symtbl, const char *name)
{
    symbol_t *sym = symtbl->symbol;
    while (sym) {
        if (!strcmp(sym->name->p, name)) {
            return sym;
        }
    }
    return NULL;
}

void symbol_add(symbol_table_t *symtbl, const char *name, int vtype)
{
    symbol_t *sym = symbol_search_one(symtbl, name);
    if (sym) {
        if (sym->vtype != VALTYPE_UNKNOWN && sym->vtype != vtype) {
            // TODO: error.
            ;
        }
        return;
    }

    sym = symbol_new(name, vtype);
    sym->next = symtbl->symbol;
    symtbl->symbol = sym;
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
