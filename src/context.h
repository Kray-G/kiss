#include "lexer.h"

extern int yyparse(kiss_parsectx_t *);
extern int yydebug;

typedef struct kiss_context_ {
    node_manager_t nmgr;
    string_set_t smgr;
    kiss_parsectx_t parsectx;

    int (*parse)(struct kiss_context_ *ctx, const char *filename);
    void (*free)(struct kiss_context_ *ctx);
} kiss_context_t;

extern kiss_context_t *new_context(void);
