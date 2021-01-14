#include "context.h"

int main(int ac, char **av)
{
    #if YYDEBUG == 1
    // yydebug = 1;
    #endif
    if (ac != 2) {
        return 1;
    }

    kiss_context_t *ctx = new_context();
    int r = ctx->parse(ctx, av[1]);

    ast_dump(ctx->nmgr.root);

    ctx->free(ctx);
    return r;
}
