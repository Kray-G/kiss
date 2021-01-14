#include "type.h"

#define CHECKNEXT(node) if (node->next) { node = node->next; goto TOP; }

typedef struct ast_type_context_ {
    int lvalue;
    symbol_table_t *symtbl;
} ast_type_context_t;

static void add_symbol(ast_type_context_t *ctx, const char *name, int vtype)
{
    symbol_table_t *symtbl = ctx->symtbl;
    symbol_add(symtbl, name, vtype);
}

static int get_type_from_symbol_table(ast_type_context_t *ctx, const char *name)
{
    symbol_table_t *symtbl = ctx->symtbl;
    symbol_t *sym = symbol_search(symtbl, name);
    return sym ? sym->vtype : VALTYPE_UNKNOWN;
}

static int ast_type_item(node_t *node, ast_type_context_t *ctx)
{
    int type = VALTYPE_UNKNOWN;
    if (!node) {
        return type;
    }

TOP:;
    switch (node->ntype) {
    /* dump expression. */
    case EXPR_INT: {
        type = VALTYPE_INT;
        break;
    }
    case EXPR_DBL: {
        type = VALTYPE_DBL;
        break;
    }
    case EXPR_VAR: {
        if (ctx->lvalue) {
            /* add symbol to the current symbol table. */
            add_symbol(ctx, node->n.name->p, node->vtype);
        } else {
            /* type will be from the symbol in the table. */
            node->vtype = get_type_from_symbol_table(ctx, node->n.name->p);
        }
        type = node->vtype;
        break;
    }
    case EXPR_CALL: {
        (void)ast_type_item(node->n.e.call.func, ctx);
        (void)ast_type_item(node->n.e.call.args, ctx);
        node_t *next = node->n.e.call.args->next;
        while (next) {
            ast_type_item(next, ctx);
            next = next->next;
        }
        if (node->n.e.call.func->ntype == EXPR_VAR) {
            /* value type from symbol table. */
            ;
        }
        break;
    }
    case EXPR_BINARY: {
        ast_type_item(node->n.e.binary.lhs, ctx);
        ast_type_item(node->n.e.binary.rhs, ctx);
        break;
    }
    case EXPR_DECL: {
        int lvale = ctx->lvalue;
        ast_type_item(node->n.e.decl.initializer, ctx);
        ctx->lvalue = lvale;
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
        .lvalue = 0,
        .symtbl = root->symtbl,
    };
    ast_type_item(root, &ctx);
}
