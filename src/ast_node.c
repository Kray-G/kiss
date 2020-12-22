
#include "ast_node.h"

/*
    Management of nodes
*/
static node_manager_t *node_manager_new(void)
{
    return (node_manager_t *)calloc(1, sizeof(node_manager_t));
}

static node_t *node_new(node_manager_t *mgr)
{
    node_t *n = (node_t *)calloc(1, sizeof(node_t));
    node_manager_t *m = node_manager_new();
    m->node = n;
    m->head = mgr->head;
    mgr->head = m;
    return n;
}

void node_free_all(node_manager_t *mgr)
{
    node_manager_t *m = mgr->head;
    while (m) {
        node_manager_t *next = m->head;
        free(m->node);
        free(m);
        m = next;
    }
}

node_t *node_connect(node_t *prev, node_t *next)
{
    if (!prev->last) {
        prev->next = prev->last = next;
        return prev;
    }
    prev->last->next = next;
    prev->last = next;
    return prev;
}

/*
    Setup root node, this is used at the finalization of building tree.
*/
node_t *ast_set_root(node_manager_t *mgr, node_t *root)
{
    mgr->root = root;
    return root;
}

/*
    Expression.
*/
node_t *ast_value_int(node_manager_t *mgr, int64_t ivalue)
{
    node_t *n = node_new(mgr);
    n->ntype = EXPR_INT;
    n->vtype = VALTYPE_INT;
    n->n.ivalue = ivalue;
    return n;
}

node_t *ast_value_dbl(node_manager_t *mgr, double dvalue)
{
    node_t *n = node_new(mgr);
    n->ntype = EXPR_DBL;
    n->vtype = VALTYPE_DBL;
    n->n.dvalue = dvalue;
    return n;
}

node_t *ast_variable(node_manager_t *mgr, string_t *name)
{
    node_t *n = node_new(mgr);
    n->ntype = EXPR_VAR;
    n->vtype = VALTYPE_UNKNOWN;
    n->n.name = name;
    return n;
}

node_t *ast_call(node_manager_t *mgr, node_t *func, node_t *args)
{
    node_t *n = node_new(mgr);
    n->ntype = EXPR_CALL;
    n->vtype = VALTYPE_UNKNOWN;
    n->n.e.call.func = func;
    n->n.e.call.args = args;
    return n;
}

node_t *ast_binary(node_manager_t *mgr, int op, node_t *lhs, node_t *rhs)
{
    node_t *n = node_new(mgr);
    n->ntype = EXPR_BINARY;
    n->vtype = VALTYPE_UNKNOWN;
    n->n.e.binary.op = op;
    n->n.e.binary.lhs = lhs;
    n->n.e.binary.rhs = rhs;
    return n;
}

node_t *ast_binary_right(node_manager_t *mgr, int op, node_t *lhs, node_t *rhs)
{
    node_t **p = &lhs;
    while ((*p)->n.e.binary.rhs) {
        p = &((*p)->n.e.binary.rhs);
    }
    node_t *n = node_new(mgr);
    n->ntype = EXPR_BINARY;
    n->vtype = VALTYPE_UNKNOWN;
    n->n.e.binary.op = op;
    n->n.e.binary.lhs = *p;
    n->n.e.binary.rhs = rhs;
    *p = n;
    return lhs;
}

node_t *ast_decl_expression(node_manager_t *mgr, string_t *name, int vtype, node_t *initializer)
{
    node_t *n = node_new(mgr);
    n->ntype = EXPR_DECL;
    n->vtype = vtype;
    n->n.e.decl.name = name;
    n->n.e.decl.initializer = initializer;
    return n;
}

/*
    Statement.
*/
node_t *ast_expr_statement(node_manager_t *mgr, node_t *expr)
{
    node_t *n = node_new(mgr);
    n->ntype = STMT_EXPR;
    n->vtype = VALTYPE_UNKNOWN;
    n->n.s.expr = expr;
    return n;
}

node_t *ast_block_statement(node_manager_t *mgr, node_t *stmt)
{
    if (!stmt) {
        /* there is no statement, a block is not necessary. */
        return NULL;
    }
    if (stmt->ntype == STMT_BLOCK && !stmt->next) {
        /* This is a single block in a block. Do not need to wrap it by a block. */
        return stmt;
    }

    node_t *n = node_new(mgr);
    n->ntype = STMT_BLOCK;
    n->vtype = VALTYPE_UNKNOWN;
    n->n.s.block = stmt;
    return n;
}

node_t *ast_branch_statement(node_manager_t *mgr, node_t *expr, node_t *then_clause, node_t *else_clause)
{
    node_t *n = node_new(mgr);
    n->ntype = STMT_BRANCH;
    n->vtype = VALTYPE_UNKNOWN;
    n->n.s.branch.expr = expr;
    n->n.s.branch.then_cloause = ast_block_statement(mgr, then_clause);
    n->n.s.branch.else_cloause = ast_block_statement(mgr, else_clause);
    return n;
}

node_t *ast_for_loop_statement(node_manager_t *mgr, node_t *expr1, node_t *expr2, node_t *expr3, node_t *then_clause)
{
    node_t *n = node_new(mgr);
    n->ntype = STMT_PRELOOP;
    n->vtype = VALTYPE_UNKNOWN;
    n->n.s.loop.e1 = expr1;
    n->n.s.loop.e2 = expr2;
    n->n.s.loop.e3 = expr3;
    n->n.s.loop.then_cloause = ast_block_statement(mgr, then_clause);
    return n;
}

node_t *ast_precond_loop_statement(node_manager_t *mgr, node_t *expr, node_t *then_clause)
{
    node_t *n = node_new(mgr);
    n->ntype = STMT_PRELOOP;
    n->vtype = VALTYPE_UNKNOWN;
    n->n.s.loop.e2 = expr;
    n->n.s.loop.then_cloause = ast_block_statement(mgr, then_clause);
    return n;
}

node_t *ast_postcond_loop_statement(node_manager_t *mgr, node_t *expr, node_t *then_clause)
{
    node_t *n = node_new(mgr);
    n->ntype = STMT_PSTLOOP;
    n->vtype = VALTYPE_UNKNOWN;
    n->n.s.loop.e2 = expr;
    n->n.s.loop.then_cloause = ast_block_statement(mgr, then_clause);
    return n;
}

node_t *ast_return_statement(node_manager_t *mgr, node_t *expr, node_t *if_modifier_expr)
{
    node_t *n = node_new(mgr);
    n->ntype = STMT_RET;
    n->vtype = VALTYPE_UNKNOWN;
    n->n.s.ret.expr = expr;
    if (if_modifier_expr) {
        return ast_branch_statement(mgr, if_modifier_expr, n, NULL);
    }
    return n;
}

node_t *ast_function_statement(node_manager_t *mgr, int64_t rtype, string_t *name, node_t *args, node_t *block)
{
    node_t *n = node_new(mgr);
    n->ntype = STMT_FUNC;
    n->vtype = rtype;
    n->n.s.func.name = name;
    n->n.s.func.args = args;
    n->n.s.func.block = block;
    return n;
}
