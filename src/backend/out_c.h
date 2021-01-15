#ifndef KISS_OUT_C_H
#define KISS_OUT_C_H

#include "xlist.h"
#include "../ast/node.h"

extern void ast_output_c_code(list_t *funcs, node_t *root);

#endif /* KISS_OUT_C_H */
