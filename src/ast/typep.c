#include "typep.h"

#define CHECKNEXT(node) if (node->next) { node = node->next; goto TOP; }

typedef struct ast_type_context_ {
    symbol_t *funcdecl;
    symbol_table_t *symtbl;
    list_t *funcs;
} ast_type_context_t;

static symbol_t *add_symbol_to_table(ast_type_context_t *ctx, const char *name, types_t type)
{
    return symbol_add(ctx->symtbl, name, type);
}

static types_t get_type_from_symbol_table(ast_type_context_t *ctx, const char *name)
{
    symbol_table_t *symtbl = ctx->symtbl;
    symbol_t *sym = symbol_search(symtbl, name);
    if (sym) {
        return sym->type;
    }
    return (types_t){0};
}

static types_t ast_type_item(node_t *node, ast_type_context_t *ctx)
{
    types_t type = {0};
    if (!node) {
        return type;
    }

TOP:;
    switch (node->ntype) {
    /* dump expression. */
    case EXPR_INT: {
        type = (types_t){ .vtype = VALTYPE_INT };
        break;
    }
    case EXPR_DBL: {
        type = (types_t){ .vtype = VALTYPE_DBL };
        break;
    }
    case EXPR_VAR: {
        type = node->vtype = get_type_from_symbol_table(ctx, node->n.name->p);
        break;
    }
    case EXPR_CALL: {
        type = ast_type_item(node->n.e.call.func, ctx);
        if (type.vtype == VALTYPE_FUNC) {
            type = node->vtype = (types_t) { .vtype = type.rtype };
        } else {
            node->vtype = type;
        }
        (void)ast_type_item(node->n.e.call.args, ctx);
        node_t *next = node->n.e.call.args->next;
        while (next) {
            ast_type_item(next, ctx);
            next = next->next;
        }
        break;
    }
    case EXPR_BINARY: {
        types_t ltype = ast_type_item(node->n.e.binary.lhs, ctx);
        types_t rtype = ast_type_item(node->n.e.binary.rhs, ctx);
        if (ltype.vtype == rtype.vtype) {
            node->vtype = type = ltype;
        } else {
            // TODO: cast.
        }
        break;
    }
    case EXPR_DECL: {
        symbol_t *sym = add_symbol_to_table(ctx, node->n.e.decl.name->p, node->vtype);
        if (ctx->funcdecl) {
            symbol_add_argtype(ctx->funcdecl, node->vtype);
        }
        ast_type_item(node->n.e.decl.initializer, ctx);
        if (node->next) {
            ast_type_item(node->next, ctx);
        }
        break;
    }

    /* dump statement, note that a statement can have a next statement. */
    case STMT_EXPR: {
        ast_type_item(node->n.s.expr, ctx);
        CHECKNEXT(node);
        break;
    }
    case STMT_BLOCK: {
        ctx->symtbl = node->symtbl = symbol_table_new(ctx->symtbl);
        ast_type_item(node->n.s.block, ctx);
        ctx->symtbl = ctx->symtbl->parent;
        CHECKNEXT(node);
        break;
    }
    case STMT_BRANCH: {
        ast_type_item(node->n.s.branch.expr, ctx);
        ast_type_item(node->n.s.branch.then_cloause, ctx);
        if (node->n.s.branch.else_cloause) {
            ast_type_item(node->n.s.branch.else_cloause, ctx);
        }
        CHECKNEXT(node);
        break;
    }
    case STMT_PRELOOP: {
        if (node->n.s.loop.e1) {
            ast_type_item(node->n.s.loop.e1, ctx);
        }
        if (node->n.s.loop.e2) {
            ast_type_item(node->n.s.loop.e2, ctx);
        }
        ast_type_item(node->n.s.loop.e2, ctx);
        if (node->n.s.loop.e3) {
            ast_type_item(node->n.s.loop.e3, ctx);
        }
        CHECKNEXT(node);
        break;
    }
    case STMT_PSTLOOP: {
        ast_type_item(node->n.s.loop.e2, ctx);
        if (node->n.s.loop.e2) {
            ast_type_item(node->n.s.loop.e2, ctx);
        }
        CHECKNEXT(node);
        break;
    }
    case STMT_RET: {
        ast_type_item(node->n.s.ret.expr, ctx);
        CHECKNEXT(node);
        break;
    }
    case STMT_FUNC: {
        list_push(ctx->funcs, (void*)node, NULL);
        node->sym = add_symbol_to_table(ctx, node->n.s.func.name->p, node->vtype);
        ctx->symtbl = node->symtbl = symbol_table_new(ctx->symtbl);
        symbol_t *fsym = ctx->funcdecl;
        ctx->funcdecl = node->sym;
        ast_type_item(node->n.s.func.args, ctx);
        ctx->funcdecl = fsym;
        ast_type_item(node->n.s.func.block, ctx);
        ctx->symtbl = ctx->symtbl->parent;
        CHECKNEXT(node);
        break;
    }
    default:
        ;
    }

    return type;
}

list_t *ast_type(node_t *root)
{
    ast_type_context_t ctx = { .funcdecl = NULL, .symtbl = NULL, .funcs = list_new() };
    ast_type_item(root, &ctx);
    return ctx.funcs;
}
