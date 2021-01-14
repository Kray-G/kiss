#include "context.h"

static void free_context(kiss_context_t *ctx)
{
    string_free(ctx->parsectx.s);
    string_set_free_all(&(ctx->smgr));
    node_free_all(&(ctx->nmgr));
}

static int parse_kiss(kiss_context_t *ctx, const char *filename)
{
    if (!ctx || !filename) {
        return 1;
    }
    ctx->parsectx.lexctx.fp = fopen(filename, "r");
    if (!ctx->parsectx.lexctx.fp) {
        return 1;
    }

    return yyparse(&(ctx->parsectx));
}

kiss_context_t *new_context(void)
{
    kiss_context_t *ctx = calloc(1, sizeof(kiss_context_t));
    ctx->parsectx.node_mgr = &(ctx->nmgr);
    ctx->parsectx.string_mgr = &(ctx->smgr);
    ctx->parsectx.s = string_new(NULL);
    ctx->parsectx.lexctx.ch = ' ';
    ctx->parse = parse_kiss;
    ctx->free = free_context;
    return ctx;
}
