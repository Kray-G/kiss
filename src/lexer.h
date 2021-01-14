#ifndef KISS_LEXER_H
#define KISS_LEXER_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include "ast/node.h"
#include "xstring.h"
#include "xstring_set.h"

typedef struct kiss_lexctx_t_ {
    int ch;
    FILE *fp;
} kiss_lexctx_t;

typedef struct kiss_parsectx_t_ {
    kiss_lexctx_t lexctx;
    node_manager_t *node_mgr;
    string_set_t *string_mgr;
    string_t *s;
} kiss_parsectx_t;

#endif /* KISS_LEXER_H */
