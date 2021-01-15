#include <stdarg.h>
#include "out_c.h"
#include "../kiss.tab.h"

typedef struct ast_output_context_ {
    int in_arglist;
} ast_output_context_t;

static const char *get_operator_name(int op)
{
    switch (op) {
    case EQEQ: return "==";
    case NEQ: return "!=";
    case LEQ: return "<";
    case GEQ: return ">";
    case ADDEQ: return "+=";
    case SUBEQ: return "-=";
    case MULEQ: return "*=";
    case DIVEQ: return "/=";
    case MODEQ: return "%=";
    }

    static char b[2] = {0};
    b[0] = op;
    return b;
}

static const char *get_type_name(int vtype)
{
    switch (vtype) {
    case VALTYPE_INT: return "int64_t";
    case VALTYPE_DBL: return "double";
    // case VALTYPE_STR: return "str";
    default:
        ;
    }
    return "";
}

static void print_factor(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vprintf(fmt, ap);
    va_end(ap);
}

static void print_indent(int indent, const char *fmt, ...)
{
    SHOW_INDENT4(indent);
    va_list ap;
    va_start(ap, fmt);
    vprintf(fmt, ap);
    va_end(ap);
}

static void ast_output_c(int indent, node_t *node, ast_output_context_t *outctx)
{
    if (!node) {
        return;
    }

TOP:;
    switch (node->ntype) {
    /* dump expression. */
    case EXPR_INT: {
        print_factor("%lld", node->n.ivalue);
        break;
    }
    case EXPR_DBL: {
        print_factor("%f\n", node->n.dvalue);
        break;
    }
    case EXPR_VAR: {
        print_factor("%s", node->n.name->p);
        break;
    }
    case EXPR_CALL: {
        ast_output_c(indent, node->n.e.call.func, outctx);
        print_factor("(");
        ast_output_c(indent, node->n.e.call.args, outctx);
        node_t *next = node->n.e.call.args->next;
        while (next) {
            ast_output_c(indent, next, outctx);
            next = next->next;
        }
        print_factor(")");
        break;
    }
    case EXPR_BINARY: {
        ast_output_c(indent, node->n.e.binary.lhs, outctx);
        print_factor(" %s ", get_operator_name(node->n.e.binary.op));
        ast_output_c(indent, node->n.e.binary.rhs, outctx);
        break;
    }
    case EXPR_DECL: {
        print_factor("%s %s", get_type_name(node->vtype.vtype), node->n.e.decl.name->p);
        if (outctx->in_arglist) {
            if (node->next) {
                print_factor(", ");
                ast_output_c(indent, node->next, outctx);
            }
        } else {
            if (node->n.e.decl.initializer) {
                print_factor(" = ");
                ast_output_c(indent, node->n.e.decl.initializer, outctx);
            }
            if (node->next) {
                print_factor(";\n");
                ast_output_c(indent, node->next, outctx);
            }
        }
        break;
    }

    /* dump statement, note that a statement can have a next statement. */
    case STMT_EXPR: {
        ast_output_c(indent, node->n.s.expr, outctx);
        CHECKNEXT(node);
        break;
    }
    case STMT_BLOCK: {
        print_indent(indent, "{\n");
        ++indent;
        ast_output_c(indent, node->n.s.block, outctx);
        --indent;
        print_indent(indent, "}\n");
        CHECKNEXT(node);
        break;
    }
    case STMT_BRANCH: {
        print_indent(indent, "if (");
        ast_output_c(indent, node->n.s.branch.expr, outctx);
        print_factor(")\n");
        ast_output_c(indent, node->n.s.branch.then_cloause, outctx);
        if (node->n.s.branch.else_cloause) {
            print_indent(indent, "else\n");
            ast_output_c(indent, node->n.s.branch.else_cloause, outctx);
        }
        print_indent(indent, "\n");
        CHECKNEXT(node);
        break;
    }
    case STMT_PRELOOP: {
        print_indent(indent, "{\n");
        ++indent;
        if (node->n.s.loop.e1) {
            ast_output_c(indent, node->n.s.loop.e1, outctx);
            print_factor(";\n");
        }
        print_indent(indent, "while (");
        if (node->n.s.loop.e2) {
            ast_output_c(indent, node->n.s.loop.e2, outctx);
        }
        print_factor(")\n");
        ++indent;
        ast_output_c(indent, node->n.s.loop.then_cloause, outctx);
        --indent;
        print_indent(indent, "\n");
        if (node->n.s.loop.e3) {
            ast_output_c(indent, node->n.s.loop.e3, outctx);
        }
        --indent;
        print_indent(indent, "}\n");
        CHECKNEXT(node);
        break;
    }
    case STMT_PSTLOOP: {
        print_indent(indent, "do {\n");
        ++indent;
        ast_output_c(indent, node->n.s.loop.e2, outctx);
        --indent;
        print_indent(indent, "} while (\n");
        if (node->n.s.loop.e2) {
            ast_output_c(indent, node->n.s.loop.e2, outctx);
        }
        print_factor(");\n");
        CHECKNEXT(node);
        break;
    }
    case STMT_RET: {
        print_indent(indent, "return ");
        ast_output_c(indent, node->n.s.ret.expr, outctx);
        print_factor(";\n");
        CHECKNEXT(node);
        break;
    }
    case STMT_FUNC: {
        CHECKNEXT(node);
        break;
    }
    default:
        ;
    }
}

static void ast_output_function(list_t *funcs, node_t *node)
{
    ast_output_context_t outctx = {0};

    int index = 0;
    while (1) {
        listitem_t *head = list_get(funcs, index++);
        if (!head) {
            break;
        }
        node_t *func = (node_t *)(head->item);
        print_indent(0, "\n%s %s(", get_type_name(func->vtype.rtype), func->n.s.func.name->p);
        outctx.in_arglist = 1;
        ast_output_c(0, func->n.s.func.args, &outctx);
        outctx.in_arglist = 0;
        printf(")\n");
        ast_output_c(0, func->n.s.func.block, &outctx);
        printf("\n");
    }

    print_indent(0, "int main()\n");
    ast_output_c(0, node, &outctx);
    print_indent(0, "\n");
}

void ast_output_c_code(list_t *funcs, node_t *root)
{
    ast_output_function(funcs, root);
}
