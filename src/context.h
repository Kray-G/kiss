#ifndef KISS_CONTEXT_H
#define KISS_CONTEXT_H

#include "xlist.h"
#include "lexer.h"
#include "ast/dump.h"
#include "ast/typep.h"
#include "backend/out_c.h"

extern int yyparse(kiss_parsectx_t *);
extern int yydebug;

typedef struct kiss_context_ {
    string_set_t smgr;
    node_manager_t nmgr;
    kiss_parsectx_t parsectx;

    int (*parse)(struct kiss_context_ *ctx, const char *filename);
    void (*type_ast)(struct kiss_context_ *ctx);
    void (*dump_ast)(struct kiss_context_ *ctx);
    void (*output)(struct kiss_context_ *ctx);
    void (*free)(struct kiss_context_ *ctx);

    list_t *funcs;
} kiss_context_t;

extern kiss_context_t *new_context(void);

#endif /* KISS_CONTEXT_H */
