#include "node.h"
#include "../kiss.tab.h"

static const char *get_type_name(int vtype)
{
    switch (vtype) {
    case VALTYPE_INT: return "int";
    case VALTYPE_DBL: return "dbl";
    case VALTYPE_STR: return "str";
    case VALTYPE_FUNC: return "function";
    default:
        ;
    }
    return "((unknown))";
}

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

static void print_argument_list(list_t *argtypes)
{
    printf(" (");
    for (int i = 0; ; ++i) {
        types_t type = symbol_get_argtype(argtypes, i);
        if (type.vtype == 0 && type.rtype == 0) {
            break;
        }
        if (i > 0) printf(", ");
        if (type.rtype) {
            printf("%s->%s", get_type_name(type.vtype), get_type_name(type.rtype));
            if (type.argtypes) {
                print_argument_list(type.argtypes);
            }
        } else {
            printf("%s", get_type_name(type.vtype));
        }
    }
    printf(")");
}

static void print_symbol_table(symbol_table_t *symtbl)
{
    symbol_t *sym = symtbl->symbol;
    if (!sym) {
        return;
    }
    printf("======== Symbol Table ========\n");
    while (sym) {
        if (sym->type.rtype) {
            printf(" * %s:%s -> %s", sym->name->p, get_type_name(sym->type.vtype), get_type_name(sym->type.rtype));
        } else {
            printf(" * %s:%s", sym->name->p, get_type_name(sym->type.vtype));
        }
        if (sym->argtypes) {
            print_argument_list(sym->argtypes);
        }
        printf("\n");
        sym = sym->next;
    }
    printf("------------------------------\n");
}

static void ast_dump_item(int indent, node_t *node)
{
    if (!node) {
        return;
    }

TOP:;
    SHOW_INDENT(indent);

    switch (node->ntype) {
    /* dump expression. */
    case EXPR_INT: {
        printf("%lld: int\n", node->n.ivalue);
        break;
    }
    case EXPR_DBL: {
        printf("%f: dbl\n", node->n.dvalue);
        break;
    }
    case EXPR_VAR: {
        printf("%s: %s\n", node->n.name->p, get_type_name(node->vtype.vtype));
        break;
    }
    case EXPR_CALL: {
        printf("[call]: %s\n", get_type_name(node->vtype.vtype));
        ast_dump_item(indent + 1, node->n.e.call.func);
        SHOW_INDENT(indent + 1);
        printf("[args]\n");
        ast_dump_item(indent + 2, node->n.e.call.args);
        node_t *next = node->n.e.call.args->next;
        while (next) {
            ast_dump_item(indent + 2, next);
            next = next->next;
        }
        break;
    }
    case EXPR_BINARY: {
        printf("%s: %s\n", get_operator_name(node->n.e.binary.op), get_type_name(node->vtype.vtype));
        ast_dump_item(indent + 1, node->n.e.binary.lhs);
        ast_dump_item(indent + 1, node->n.e.binary.rhs);
        break;
    }
    case EXPR_DECL: {
        printf("[declaration]\n");
        SHOW_INDENT(indent + 1);
        printf("%s: %s\n", node->n.e.decl.name->p, get_type_name(node->vtype.vtype));
        ast_dump_item(indent + 2, node->n.e.decl.initializer);
        if (node->next) {
            ast_dump_item(indent, node->next);
        }
        break;
    }

    /* dump statement, note that a statement can have a next statement. */
    case STMT_EXPR: {
        printf("[expression]\n");
        ast_dump_item(indent + 1, node->n.s.expr);
        CHECKNEXT(node);
        break;
    }
    case STMT_BLOCK: {
        printf("[block]\n");
        print_symbol_table(node->symtbl);
        ast_dump_item(indent + 1, node->n.s.block);
        CHECKNEXT(node);
        break;
    }
    case STMT_BRANCH: {
        printf("[branch]\n");
        SHOW_INDENT(indent + 1);
        printf("[condtion]\n");
        ast_dump_item(indent + 2, node->n.s.branch.expr);
        SHOW_INDENT(indent + 1);
        printf("[then]\n");
        ast_dump_item(indent + 2, node->n.s.branch.then_cloause);
        if (node->n.s.branch.else_cloause) {
            SHOW_INDENT(indent + 1);
            printf("[else]\n");
            ast_dump_item(indent + 2, node->n.s.branch.else_cloause);
        }
        CHECKNEXT(node);
        break;
    }
    case STMT_PRELOOP: {
        printf("[pre-condition-loop]\n");
        if (node->n.s.loop.e1) {
            SHOW_INDENT(indent + 1);
            printf("[initializer]\n");
            ast_dump_item(indent + 2, node->n.s.loop.e1);
        }
        if (node->n.s.loop.e2) {
            SHOW_INDENT(indent + 1);
            printf("[condition]\n");
            ast_dump_item(indent + 2, node->n.s.loop.e2);
        }
        SHOW_INDENT(indent + 1);
        printf("[then]\n");
        ast_dump_item(indent + 2, node->n.s.loop.then_cloause);
        if (node->n.s.loop.e3) {
            SHOW_INDENT(indent + 1);
            printf("[update]\n");
            ast_dump_item(indent + 2, node->n.s.loop.e3);
        }
        CHECKNEXT(node);
        break;
    }
    case STMT_PSTLOOP: {
        printf("[post-condition-loop]\n");
        SHOW_INDENT(indent + 1);
        printf("[then]\n");
        ast_dump_item(indent + 2, node->n.s.loop.e2);
        if (node->n.s.loop.e2) {
            SHOW_INDENT(indent + 1);
            printf("[condition]\n");
            ast_dump_item(indent + 2, node->n.s.loop.e2);
        }
        CHECKNEXT(node);
        break;
    }
    case STMT_RET: {
        printf("[return]\n");
        ast_dump_item(indent + 1, node->n.s.ret.expr);
        CHECKNEXT(node);
        break;
    }
    case STMT_FUNC: {
        printf("[function-definition] %s -> %s\n", node->n.s.func.name->p, get_type_name(node->vtype.rtype));
        print_symbol_table(node->symtbl);
        ast_dump_item(indent + 1, node->n.s.func.args);
        ast_dump_item(indent + 1, node->n.s.func.block);
        CHECKNEXT(node);
        break;
    }
    default:
        ;
    }
}

void ast_dump(node_t *root)
{
    ast_dump_item(0, root);
}
