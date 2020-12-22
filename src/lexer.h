#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include "ast_node.h"
#include "xstring.h"
#include "xstring_set.h"

typedef struct kxs_lexctx_t_ {
    int ch;
    FILE *fp;
} kxs_lexctx_t;

typedef struct kxs_parsectx_t_ {
    kxs_lexctx_t lexctx;
    node_manager_t *node_mgr;
    string_set_t *string_mgr;
    string_t *s;
} kxs_parsectx_t;

#endif /* LEXER_H */
