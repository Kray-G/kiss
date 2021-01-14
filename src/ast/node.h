#ifndef KISS_NODE_H
#define KISS_NODE_H

#include <stdlib.h>
#include <stdint.h>
#include "xstring.h"
#include "symbol.h"

enum node_type {
    NODE_UNKNOWN,

    EXPR_INT,
    EXPR_DBL,
    EXPR_VAR,
    EXPR_CALL,
    EXPR_UNARY,
    EXPR_BINARY,
    EXPR_DECL,
    EXPR_CAST,

    STMT_EXPR,
    STMT_BLOCK,
    STMT_BRANCH,
    STMT_PRELOOP,
    STMT_PSTLOOP,
    STMT_RET,
    STMT_FUNC,
};

enum value_type {
    VALTYPE_UNKNOWN,
    VALTYPE_INT,
    VALTYPE_DBL,
    VALTYPE_STR,
    VALTYPE_FUNC,
};

struct node_t_;
typedef struct node_manager_t_ {
    struct node_t_ *node;
    struct node_t_ *root;
    struct node_manager_t_ *head;
} node_manager_t;

typedef struct node_t_ {
    node_manager_t *mgr;            // reference to an allocation manager.
    struct node_t_ *next;           // for statement or expression list.
    struct node_t_ *last;           // for statement or expression list.

    enum node_type ntype;
    enum value_type vtype;          // type of value.
    enum value_type rtype;          // if vtype == VALTYPE_FUNC, this means a return type.
    union {
        int64_t ivalue;             // EXPR_INT
        double dvalue;              // EXPR_DBL
        string_t *name;             // EXPR_VAR
        union {
            struct {                // EXPR_UNARY
                int op;
                struct node_t_ *expr;
            } unary;
            struct {                // EXPR_BINARY
                int op;
                struct node_t_ *lhs;
                struct node_t_ *rhs;
            } binary;
            struct {                // EXPR_CALL
                struct node_t_ *func;
                struct node_t_ *args;
            } call;
            struct {                // EXPR_DECL
                string_t *name;     // variable name.
                struct node_t_ *initializer;
            } decl;
        } e;
        union {
            struct node_t_ *block;  // STMT_BLOCK
            struct node_t_ *expr;   // STMT_EXPR, STMT_DECL
            struct {                // STMT_BRANCH
                struct node_t_ *expr;
                struct node_t_ *then_cloause;
                struct node_t_ *else_cloause;
            } branch;
            struct {                // STMT_PRELOOP, STMT_PSTLOOP
                struct node_t_ *e1; // initializer
                struct node_t_ *e2; // condition
                struct node_t_ *e3; // update
                struct node_t_ *then_cloause;
            } loop;
            struct stmt_ret {       // STMT_RET
                struct node_t_ *expr;
            } ret;
            struct stmt_func {      // STMT_FUNC
                string_t *name;     // function name.
                struct node_t_ *args;
                struct node_t_ *block;
            } func;
        } s;
    } n;

    symbol_table_t *symtbl;
} node_t;

extern void node_free_all(node_manager_t *mgr);
extern node_t *node_connect(node_t *prev, node_t *next);

extern node_t *ast_set_root(node_manager_t *mgr, node_t *root);

extern node_t *ast_value_int(node_manager_t *mgr, int64_t type);
extern node_t *ast_value_dbl(node_manager_t *mgr, double type);
extern node_t *ast_variable(node_manager_t *mgr, string_t *name);
extern node_t *ast_binary(node_manager_t *mgr, int op, node_t *lhs, node_t *rhs);
extern node_t *ast_binary_right(node_manager_t *mgr, int op, node_t *lhs, node_t *rhs);
extern node_t *ast_call(node_manager_t *mgr, node_t *func, node_t *args);
extern node_t *ast_decl_expression(node_manager_t *mgr, string_t *name, int type, node_t *initializer);

extern node_t *ast_expr_statement(node_manager_t *mgr, node_t *expr);
extern node_t *ast_block_statement(node_manager_t *mgr, node_t *stmt);
extern node_t *ast_branch_statement(node_manager_t *mgr, node_t *expr, node_t *then_clause, node_t *else_clause);
extern node_t *ast_for_loop_statement(node_manager_t *mgr, node_t *expr1, node_t *expr2, node_t *expr3, node_t *then_clause);
extern node_t *ast_precond_loop_statement(node_manager_t *mgr, node_t *expr, node_t *then_clause);
extern node_t *ast_postcond_loop_statement(node_manager_t *mgr, node_t *expr, node_t *then_clause);
extern node_t *ast_return_statement(node_manager_t *mgr, node_t *expr, node_t *if_modifier);
extern node_t *ast_function_statement(node_manager_t *mgr, int64_t type, string_t *name, node_t *args, node_t *block);

extern void ast_dump(node_t *root);

#endif /* KISS_NODE_H */
