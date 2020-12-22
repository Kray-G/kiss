#include "xstring_set.h"
#include <limits.h>

static int string_set_hash(char *s)
{
    unsigned v;
    for (v = 0; *s != '\0'; s++) {
        v = ((v << CHAR_BIT) + *s) % STRING_SET_HASHSIZE;
    }
    return (int)v;
}

void string_set_dump(int indent, string_set_node_t *p)
{
    if (!p) return;
    for (int i = 0; i < indent; ++i) printf("  ");
    printf("%s\n", p->key->p);
    string_set_dump(indent + 1, p->left);
    string_set_dump(indent + 1, p->right);
}

void string_set_free_node(string_set_node_t *p)
{
    if (!p) return;
    string_set_free_node(p->left);
    string_set_free_node(p->right);
    free(p->key);
    free(p);
}

void string_set_free_all(string_set_t *sset)
{
    for (int i = 0; i < STRING_SET_HASHSIZE; i++) {
        if (sset->hashtable[i]) {
            string_set_free_node(sset->hashtable[i]);
        }
    }
}

static char *string_set_search_str(string_set_t *sset, char *s)
{
    int cmp;
    string_set_node_t **p;
    string_set_node_t *q;

    p = &(sset->hashtable[string_set_hash(s)]);
    while (*p && (*p)->key && (cmp = strcmp(s, (*p)->key->p)) != 0) {
        if (cmp < 0) {
            p = &((*p)->left);
        } else {
            p = &((*p)->right);
        }
    }
    if (*p) {
        return (*p)->key->p;
    }
    return NULL;
}

static string_t *string_set_search_impl(string_set_t *sset, string_t *s, int insert)
{
    int cmp;
    string_set_node_t **p;
    string_set_node_t *q;

    p = &(sset->hashtable[string_set_hash(s->p)]);
    while (*p && (cmp = strcmp(s->p, (*p)->key->p)) != 0) {
        if (cmp < 0) {
            p = &((*p)->left);
        } else {
            p = &((*p)->right);
        }
    }
    if (*p) {
        return (*p)->key;
    }
    if (!insert) {
        return NULL;
    }

    sset->count++;
    if ((q = malloc(sizeof *q)) == NULL) {
        exit(1);
    }
    q->key = string_dup(s);
    q->left = NULL;
    q->right = *p;
    *p = q;
    return q->key;
}

string_t *string_set_search(string_set_t *sset, string_t *s)
{
    return string_set_search_impl(sset, s, 0);
}

string_t *string_set_insert(string_set_t *sset, string_t *s)
{
    return string_set_search_impl(sset, s, 1);
}

#ifdef XSTRING_SET_DEBUG
#include <stdio.h>

string_set_t sset = {0};
string_t word = {0};
void getword(void)
{
    int c;

    string_clear(&word);
    do {
        if ((c = getchar()) == EOF) return;
    } while (! isalpha(c));
    do {
        string_append_char(&word, tolower(c));
        c = getchar();
    } while (isalpha(c));
}

int main(void)
{
    int i;

    for (i = 0; i < STRING_SET_HASHSIZE; i++) sset.hashtable[i] = NULL;
    while (getword(), word.len > 0) {
        string_set_insert(&sset, string_dup(&word));
        char *str = string_set_search_str(&sset, word.p);
        if (str) printf("Found: %s\n", str);
        else     printf("NOT Found: %s\n", word.p);
    }
    printf("hold %lu different words\n", sset.count);
    for (i = 0; i < STRING_SET_HASHSIZE; i++) {
        if (sset.hashtable[i]) {
            printf("hashtable[%d]\n", i);
            string_set_dump(1, sset.hashtable[i]);
        }
    }
    return 0;
}
#endif
