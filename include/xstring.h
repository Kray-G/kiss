#ifndef KISS_STRING_H
#define KISS_STRING_H

#include <stdlib.h>
#include <string.h>

typedef struct string_t_ {
    char *p;
    int len;
    int cap;
} string_t;

#define STRING_UNIT (16)

extern string_t *string_new_empty(void);
extern string_t *string_new_len(char *p, int len);
extern string_t *string_new(char *p);
extern string_t *string_clear(string_t *s);
extern void string_free(string_t *s);
extern string_t *string_append(string_t *dst, char *s);
extern string_t *string_append_char(string_t *dst, char c);
extern string_t *string_append_str(string_t *dst, string_t *s);
extern string_t *string_dup(string_t *dst);

#endif /* KISS_STRING_H */
