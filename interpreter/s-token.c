/*
    Very Simple Compiler (Tokenizer part)
    e145713
 */

#include "s-compile.h"

static int isdigit(char);
static int isletter(char);
static int ishex(char);
static int isoct(char);
static int todigit(char);
static int htodigit(char);
static int ret(int);

void error(char *);
int  token();

char *ptr,*last_ptr,*before;
int  value,lvalue;
int  last_token;

int
token()
{
    int c = 0;
    int d = 0;

    last_ptr = ptr;  /* for error position */

    /* skip whitespace */
    if (*ptr == ' ') {
        while (*ptr++ == ' ');
        ptr--;
    }
    c = *ptr;

    /* not exist char */
    if (!c)
        return ret(EOF);

    ptr++;

    /* skip blank chars or comment */ 
    if (c < ' ' || (c == '/' && *ptr == '/')) {
        while (*ptr++);
        ptr--;
        last_ptr = ptr;
        return ret(NIL);
    }

    if (isdigit(c)) {
        /* hex */
        if (c == '0' && *ptr == 'x') {
            ptr++;
            unsigned int h = 0;
            while ( ishex((c = *ptr++)) ) {
                d = htodigit(c);
                h = h * 16 + d;
            }
            if (c != 0) ptr--;
            value = (int)h;
            return ret(HEX);
        }

        /* octal */
        if (c == '0' && isoct(*ptr)) {
            unsigned int o = 0;
            while ( isoct((c = *ptr++)) ) {
                d = todigit(c);
                o = o * 8 + d;
            }
            if (c != 0) ptr--;
            value = (int)o;
            return ret(OCT);
        }

        /* decimal */
        if (c == '0') c = *ptr;
        d = todigit(c);
        while ( isdigit((c = *ptr++)) )
            d = d * 10 + todigit(c);

        if (c != 0) ptr--;

        value = d;

        return ret(NUM);
    }

    /* variable */
    /* return variable reference */
    if (isletter(c)) {
        value = c - 'a';
        return ret(VAR);
    }

    return ret(c);
}

void
error(char *s) {
    fprintf(stderr, "%s on %s\n", s, last_ptr);
}

static int
ret(int n) {
    last_token = n;
    return last_token;
}

static int
isletter(char c) { return 'a' <= c && c <= 'z'; }
static int
isdigit(char c) { return '0' <= c && c <= '9'; }
static int
ishex(char c) { return ('0' <= c && c <= '9') || ('a' <= c && c <= 'f') || ('A' <= c && c <= 'F'); }
static int
isoct(char c) { return '0' <= c && c <= '7'; }
static int
tolower(char c) { return ('A' <= c && c <= 'Z') ? c + 32 : c; }
static int
todigit(char c) { return isdigit(c) ? c - '0' : -1; }

static int
htodigit(char c) {
    c = tolower(c);
    if (isdigit(c))
        return c - '0';

    if ('a' <= c && c <= 'f')
        return c - 'a' + 10;

    return -1;
}



/* end */
