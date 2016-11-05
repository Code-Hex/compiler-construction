/*
    Very Simple Compiler (Parser part, Tree Genertor)
	$Id$
 */

#include "s-compile.h"

#include <string.h>
#include <stdlib.h>    // for malloc

static int optimize = 0;

#define ADD(type) ((type *)calloc(1, sizeof(type)))
#define NEW(type, size) ((type **)calloc(size, sizeof(type)))
#define RENEW(type, ptr, size) ((type **)realloc(ptr, size * sizeof(type)))

typedef struct node {
    struct node *left;
    struct node *right;
    int type;
    int value;
} node, *node_ptr;

typedef struct nodes {
    node **ptrs;
    int ptr;
    int size;
} nodes;

static node *expr(nodes *);
static node *aexpr(nodes *);
static node *mexpr(nodes *);
static node *term(nodes *);
static node *set_node(nodes *, int, int, node_ptr, node_ptr);
static void free_nodes(nodes *);
static void alloc_nodes(nodes *, int);
static int get_ptr(nodes *);

static int
get_ptr(nodes *ary)
{
	int p = ary->ptr;
	ary->ptr++;
	return p;
}

static void
alloc_nodes(nodes *ary, int size)
{
	if (ary->ptrs == NULL) {
		ary->ptrs = NEW(node, size);
		for (int i = 0; i < size; i++)
			ary->ptrs[i] = ADD(node);
		ary->ptr = 0;
		ary->size = size;
		return;
	}

	if (ary->size < size) {
		ary->ptrs = RENEW(node, ary->ptrs, size);
		for (int i = ary->size; i < size; i++)
			ary->ptrs[i] = ADD(node);
		ary->ptr = 0;
		ary->size = size;
		return;
	}

	ary->ptr = 0;
}

static void
free_nodes(nodes *ary)
{
	if (ary) {
		for (int i = 1; i < ary->size; i++) {
			if (ary->ptrs[i])
				free(ary->ptrs[i]);
		}

		free(ary->ptrs);
	}
}

static node_ptr
set_node(nodes *ary, int type, int value, node_ptr left, node_ptr right) 
{
    node *d;
    if (optimize && (left  &&  left->type =='0') &&
        (right && right->type =='0')) {
		switch(type) {
		case '>':
		    right->value = (left->value > right->value); 
		    free(left); return right;
		case '+':
		    right->value = left->value + right->value; 
		    free(left); return right;
		case '-':
		    right->value = left->value - right->value; 
		    free(left); return right;
		case '*':
		    right->value = right->value * left->value;
		    free(left); return right;
		case '/':
		    if (right->value==0) {
				error("zero divide in compile time");
		    } else {
				right->value = left->value / right->value;
		    }
		    free(left); return right;
		}
    }

    int p = get_ptr(ary);
    d = ary->ptrs[p];
    d->type = type;
    d->value = value;
    d->left = left;
    d->right = right;
    return d;
}

static void
code_generate(node_ptr d)
{
    int assign;
    switch(d->type) {
    case '0':
		emit_value(d->value);
		return;
    case 'v':
		emit_load(d->value);
		return;
    case '=':
		if (!d->left || d->left->type != 'v') {
		    error("Bad assignment");
		    code_generate(d->right);
		    return;
		}
		assign = d->left->value;
		code_generate(d->right);
		emit_store(assign);
		return;
    case '>': 
		code_generate(d->left);
		emit_push();
		code_generate(d->right);
		emit_compare(); 
		break;
    default:   /* calculation */
		code_generate(d->right);
		emit_push();
		code_generate(d->left);
		switch(d->type) {
		case '+': emit_calc(O_ADD); break;
		case '-': emit_calc(O_SUB_R); break;
		case '/': emit_calc(O_DIV_R); break;
		case '*': emit_calc(O_MUL); break;
		default:
		    error("Internal Error, unknown opecode");
		}
	return;
    }
}

static node_ptr 
expr(nodes *ary)
{
    node *d;

    d = aexpr(ary);
    while(last_token!=EOF) {
		switch(last_token) {
		case '>': 
		    d = set_node(ary, '>', 0, d, aexpr(ary)); 
		    break;
		case '=': 
		    d = set_node(ary, '=', 0, d, aexpr(ary)); 
		    break;
		case ')': 
		    return d;
		default:
		    error("Bad expression");
		    return d;
		}
    }
    return d;
}

static node_ptr 
aexpr(nodes *ary)
{
    node *d;

    d = mexpr(ary);
    while(last_token!=EOF) {
		switch(last_token) {
		case '-': 
		    d = set_node(ary, '-', 0, d, mexpr(ary));
		    break;
		case '+': 
		    d = set_node(ary, '+', 0, d, mexpr(ary));
		    break;
		default:
		    return d;
		}
    }
    return d;
}

static node_ptr
mexpr(nodes *ary)
{
    node *d;

    d = term(ary);
    while(last_token!=EOF) {
		switch(last_token) {
		case '*': 
		    d = set_node(ary, '*', 0, d, term(ary));
		    break;
		case '/': 
		    d = set_node(ary, '/', 0, d, term(ary));
		    break;
		default:
		    return d;
		}
    }
    return d;
}

static node_ptr
term(nodes *ary)
{
    node *d;

    lvalue = -1;
    token();
    if (last_token==EOF) {
		error("Term expected");
    }
    switch(last_token) {
    case '0':
		d = set_node(ary, '0', value, NULL, NULL);
		token();
		return d;
    case 'v':
		d = set_node(ary, 'v', value, NULL, NULL);
		token();
		return d;
    case '(':
		d = expr(ary);
		if (last_token != ')') {
		    error("Unbalanced parenthsis");
		} 
		token(); 
		return d;
    default:
		token();
		error("Unknown term");
		return 0;
    }
}

int
main() 
{
	node *d;
    nodes ary;
    char buf[BUFSIZ];

    emit_intro();
    while (fgets(buf,BUFSIZ,stdin)) {
		ptr = buf;
		before = buf;

		alloc_nodes(&ary, strlen(buf) - 1);
		printf("%s %s", comments, buf);
		d = expr(&ary);
		code_generate(d);
		emit_print();
		emit_comment();
    }

    free_nodes(&ary);
    emit_ending();
    return 0;
}


/* end */
