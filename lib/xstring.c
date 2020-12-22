#include "xstring.h"

string_t *string_new_empty(void)
{
    string_t *s = (string_t *)calloc(1, sizeof(string_t));
    s->cap = STRING_UNIT;
    s->p = (char *)calloc(s->cap, sizeof(char));
    s->len = 0;
    return s;
}

string_t *string_new_len(char *p, int len)
{
    string_t *s = (string_t *)calloc(1, sizeof(string_t));
    s->cap = (((len * 2) / STRING_UNIT) + 1) * STRING_UNIT;
    s->p = (char *)calloc(s->cap, sizeof(char));
    strncpy(s->p, p, len);
    s->p[len] = 0;
    s->len = len;
    return s;
}

string_t *string_new(char *p)
{
    if (!p) {
        return string_new_empty();
    }
    return string_new_len(p, strlen(p));
}

string_t *string_clear(string_t *s)
{
    s->len = 0;
    if (!s->p) {
        s->cap = STRING_UNIT;
        s->p = (char *)calloc(STRING_UNIT, sizeof(char));
    }
    s->p[0] = 0;
    return s;
}

void string_free(string_t *s)
{
    free(s->p);
    free(s);
}

string_t *string_append(string_t *dst, char *s)
{
    int slen = strlen(s);
    int len = dst->len + slen;
    if (dst->cap <= len) {
        int cap = (((len * 2) / STRING_UNIT) + 1) * STRING_UNIT;
        char *np = (char *)realloc(dst->p, cap);
        // np == NULL then error?
        dst->p = np;
        dst->cap = cap;
    }
    strcpy(dst->p + dst->len, s);
    dst->len = len;
    return dst;
}

string_t *string_append_char(string_t *dst, char c)
{
    int len = dst->len + 1;
    if (dst->cap <= len) {
        int cap = (((len * 2) / STRING_UNIT) + 1) * STRING_UNIT;
        char *np = (char *)realloc(dst->p, cap);
        // np == NULL then error?
        dst->p = np;
        dst->cap = cap;
    }
    char *p = dst->p + dst->len;
    *p++ = c;
    *p = 0;
    dst->len = len;
    return dst;
}

string_t *string_append_str(string_t *dst, string_t *s)
{
    int len = dst->len + s->len;
    if (dst->cap <= len) {
        int cap = (((len * 2) / STRING_UNIT) + 1) * STRING_UNIT;
        char *np = (char *)realloc(dst->p, cap);
        // np == NULL then error?
        dst->p = np;
        dst->cap = cap;
    }
    strcpy(dst->p + dst->len, s->p);
    dst->len = len;
    return dst;
}

string_t *string_dup(string_t *dst)
{
    return string_new_len(dst->p, dst->len);
}

#ifdef XSTRING_DEBUG
#include <stdio.h>

int main(void)
{
    string_t *s = string_new("abcabc");
    printf("strlen(%3d) = len:%3d, (cap:%3d)\n", (int)strlen(s->p), s->len, s->cap);
    for (int i = 0; i < 20; ++i) {
        s = string_append(s, "defdef");
        printf("strlen(%3d) = len:%3d, (cap:%3d)\n", (int)strlen(s->p), s->len, s->cap);
    }
    string_free(s);
    return 0;
}
#endif
