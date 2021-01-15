#include "lexer.h"
#include "kiss.tab.h"

#define lex_is_whitespace(c) ((c) == ' ' || (c) == '\t' || (c) == '\n' || (c) == '\r')
#define lex_is_oct(c) ((c) == '0' || (c) == '1' || (c) == '2' || (c) == '3' || (c) == '4' || (c) == '5' || (c) == '6' || (c) == '7')
#define lex_is_name1(c) (isalpha(c) || (c) == '$' || (c) == '_')
#define lex_is_name2(c) (isalnum(c) || (c) == '$' || (c) == '_')

static int lex_curr(kiss_lexctx_t *lexctx)
{
    return lexctx->ch;
}

static int lex_next(kiss_lexctx_t *lexctx)
{
    if (lexctx->ch == EOF) {
        return EOF;
    }
    int ch = fgetc(lexctx->fp);
    if (ch == EOF) {
        return lexctx->ch = EOF;
    }
    return lexctx->ch = ch;
}

static int get_token_of_keyword(string_t *s)
{
    char *buf = s->p;
    char ch = buf[0];
    if (ch == '_') {
        if (!strcmp(buf, "_printf")) return _PRINTF;
    } else if (ch == 'd') {
        if (!strcmp(buf, "dbl")) return DBL_TYPE;
    } else if (ch == 'e') {
        if (!strcmp(buf, "else")) return ELSE;
    } else if (ch == 'f') {
        if (!strcmp(buf, "for")) return FOR;
        if (!strcmp(buf, "function")) return FUNCTION;
    } else if (ch == 'i') {
        if (!strcmp(buf, "if")) return IF;
        if (!strcmp(buf, "int")) return INT_TYPE;
    } else if (ch == 'r') {
        if (!strcmp(buf, "return")) return RETURN;
    } else if (ch == 'v') {
        if (!strcmp(buf, "var")) return VAR;
    } else if (ch == 'w') {
        if (!strcmp(buf, "while")) return WHILE;
    }
    return NAME;
}

#define LEX_BUFMAX (256)
int yylex(YYSTYPE *yylval, kiss_parsectx_t *parsectx)
{
    kiss_lexctx_t *lexctx = &(parsectx->lexctx);
    int ch = lex_curr(lexctx);

RETRY:;
    // skip whitespaces.
    while (lex_is_whitespace(ch)) {
        ch = lex_next(lexctx);
    }

    if (ch == EOF) {
        return EOF;
    }

    #define LEX_CASE(CH) \
        case CH:\
            lex_next(lexctx);\
            return CH;\
    /**/
    #define LEX_EQCASE(CH, EQVAL) \
        case CH:\
            ch = lex_next(lexctx);\
            if (ch == '=') {\
                lex_next(lexctx);\
                return EQVAL;\
            }\
            return CH;\
    /**/
    switch (ch) {
    LEX_EQCASE('=', EQEQ);
    LEX_EQCASE('!', NEQ);
    LEX_EQCASE('+', ADDEQ);
    LEX_EQCASE('-', SUBEQ);
    LEX_EQCASE('*', MULEQ);
    LEX_EQCASE('%', MODEQ);
    LEX_EQCASE('<', LEQ);
    LEX_EQCASE('>', GEQ);
    LEX_CASE('(');
    LEX_CASE(')');
    case '"': {
        string_t *s = string_clear(parsectx->s);
        ch = lex_next(lexctx);
        while (ch != '"') {
            if (ch == '%') {
                string_append_char(s, '%');
                ch = lex_next(lexctx);
                if (ch == 'd') {
                    string_append(s, PRId64);
                } else {
                    string_append_char(s, ch);
                }
            } else if (ch == '\\') {
                string_append_char(s, '\\');
                ch = lex_next(lexctx);
                string_append_char(s, ch);
            } else {
                string_append_char(s, ch);
            }
            ch = lex_next(lexctx);
        }
        yylval->sv = string_set_insert(parsectx->string_mgr, s);
        lex_next(lexctx);
        return STR_VALUE;
    }
    case '/':
        ch = lex_next(lexctx);
        if (ch == '=') {
            lex_next(lexctx);
            return DIVEQ;
        }
        if (ch == '/') {    // line comment.
            ch = lex_next(lexctx);
            while (ch != '\n' && ch != EOF) {
                ch = lex_next(lexctx);
            }
            ch = lex_next(lexctx);
            goto RETRY;
        }
        if (ch == '*') {    // multi-lines comment.
            int nested = 1;
            ch = lex_next(lexctx);
            while (ch != EOF) {
                int c = ch;
                ch = lex_next(lexctx);
                if (c == '/' && ch == '*') {
                    ++nested;
                } else if (c == '*' && ch == '/') {
                    if (--nested == 0) break;
                }
            }
            ch = lex_next(lexctx);
            goto RETRY;
        }
        return '/';
    default:
        ;
    }
    #undef LEX_EQCASE

    if (isdigit(ch)) {
        string_t *s = string_clear(parsectx->s);
        if (ch == '0') {
            ch = lex_next(lexctx);
            if (ch == 'x' || ch == 'X') {
                ch = lex_next(lexctx);
                while (isxdigit(ch)) {
                    string_append_char(s, ch);
                    ch = lex_next(lexctx);
                }
                yylval->iv = strtoll(s->p, NULL, 16);
                return INT_VALUE;
            } else if (lex_is_oct(ch)) {
                while (lex_is_oct(ch)) {
                    string_append_char(s, ch);
                    ch = lex_next(lexctx);
                }
                yylval->iv = strtoll(s->p, NULL, 8);
                return INT_VALUE;
            }
            // just 0.
            yylval->iv = 0;
            return INT_VALUE;
        }
        while (isdigit(ch)) {
            string_append_char(s, ch);
            ch = lex_next(lexctx);
        }
        if (ch != '.') {
            yylval->iv = strtoll(s->p, NULL, 10);
            return INT_VALUE;
        }
        string_append_char(s, ch);
        ch = lex_next(lexctx);
        while (isdigit(ch)) {
            string_append_char(s, ch);
            ch = lex_next(lexctx);
        }
        if (ch == 'e' || ch == 'E') {
            string_append_char(s, ch);
            ch = lex_next(lexctx);
            if (ch == '+' || ch == '-') {
                string_append_char(s, ch);
                ch = lex_next(lexctx);
            }
            while (isdigit(ch)) {
                string_append_char(s, ch);
                ch = lex_next(lexctx);
            }
        }
        yylval->dv = strtod(s->p, NULL);
        return DBL_VALUE;
    }

    if (lex_is_name1(ch)) {
        string_t *s = string_clear(parsectx->s);
        string_append_char(s, ch);
        ch = lex_next(lexctx);
        while (lex_is_name2(ch)) {
            string_append_char(s, ch);
            ch = lex_next(lexctx);
        }
        yylval->sv = string_set_insert(parsectx->string_mgr, s);
        return get_token_of_keyword(s);
    }

    if (isgraph(ch)) {
        lex_next(lexctx);
        return ch;
    }

    return ERROR;
}
