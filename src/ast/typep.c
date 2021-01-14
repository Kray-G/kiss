#include "typep.h"

#define CHECKNEXT(node) if (node->next) { node = node->next; goto TOP; }

typedef struct ast_type_context_ {
    symbol_table_t *symtbl;
} ast_type_context_t;

typedef struct vtypes_ {
    int vtype;
    int rtype;
} types_t;

static void add_symbol_to_table(ast_type_context_t *ctx, const char *name, int vtype, int rtype)
{
    symbol_table_t *symtbl = ctx->symtbl;
    symbol_add(symtbl, name, vtype, rtype);
}

static types_t get_type_from_symbol_table(ast_type_context_t *ctx, const char *name)
{
    symbol_table_t *symtbl = ctx->symtbl;
    symbol_t *sym = symbol_search(symtbl, name);
    if (sym) {
        return (types_t) {
            .vtype = sym->vtype,
            .rtype = sym->rtype,
        };
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
        type = get_type_from_symbol_table(ctx, node->n.name->p);
        node->vtype = type.vtype;
        node->rtype = type.rtype;
        break;
    }
    case EXPR_CALL: {
        type = ast_type_item(node->n.e.call.func, ctx);
        if (type.vtype == VALTYPE_FUNC) {
            node->vtype = type.rtype;
            node->rtype = VALTYPE_UNKNOWN;
        } else {
            node->vtype = type.vtype;
            node->rtype = type.rtype;
        }
        type = (types_t){ .vtype = node->vtype, .rtype = node->rtype };
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
            type = ltype;
            node->vtype = type.vtype;
            node->rtype = type.rtype;
        } else {
            // TODO: cast.
        }
        break;
    }
    case EXPR_DECL: {
        add_symbol_to_table(ctx, node->n.e.decl.name->p, node->vtype, VALTYPE_UNKNOWN);
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
        add_symbol_to_table(ctx, node->n.s.func.name->p, node->vtype, node->rtype);
        ctx->symtbl = node->symtbl = symbol_table_new(ctx->symtbl);
        ast_type_item(node->n.s.func.args, ctx);
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

void ast_type(node_t *root)
{
    root->symtbl = symbol_table_new(NULL);
    ast_type_context_t ctx = {
        .symtbl = root->symtbl,
    };
    ast_type_item(root, &ctx);
}
