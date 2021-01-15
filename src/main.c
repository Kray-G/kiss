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
    if (r == 0) {
        ctx->type_ast(ctx);
        ctx->dump_ast(ctx);
        ctx->output(ctx);
    } else {
        printf("failed: %d\n", r);
    }

    ctx->free(ctx);
    return r;
}
