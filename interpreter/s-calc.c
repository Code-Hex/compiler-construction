/*
    Very Simple Calculator
	e145713
 */

#include "s-compile.h"

int  variable[48];

static int expr();
static int asexpr();
static int qexpr();
static int orexpr();
static int andexpr();
static int oexpr();
static int xexpr();
static int amexpr();
static int eqexpr();
static int lgexpr();
static int siexpr();
static int aexpr();
static int mexpr();
static int term();

static int
expr()
{
    int d;

    d = asexpr();
    while (last_token != EOF) {
		switch (last_token) { 
		default:
		    error("Bad expression");
		case NIL:
		case ')':
		    return d;
		}
    }
    return d;
}

/* = */
static int
asexpr()
{
	int d, assign;

	d = qexpr();
	assign = lvalue;
	while (last_token != EOF) {
		if (last_token == '=') {
			if (assign >= 0) {
				d = asexpr(); 
				variable[assign] = d;
		    } else {
				error("Bad assignment");
		    }
			continue;
		}

		break;
	}

	return d;
}

/* a ? b : c */
static int
qexpr()
{
	int a, b, c;
	int d;

	d = orexpr();
	while (last_token != EOF) {
    	if (last_token == '?') {
    		a = d;
    		b = asexpr();
    		if (last_token == ':') {
    			c = asexpr();
    			d = a ? b : c;
    			continue;
    		}
    	}

    	break;
    }

	return d;
}

/* || */
static int
orexpr() {
	int d, r;

	d = andexpr();
    while (last_token != EOF) {
    	if (last_token == '|' && *ptr == '|') {
    		ptr++;
    		r = andexpr();
    		d = (d || r);
    		continue;
    	}

    	break;
    }
    return d;	
}

/* && */
static int
andexpr() {
	int d;

	d = oexpr();
    while (last_token != EOF) {
    	if (last_token == '&' && *ptr == '&') {
    		ptr++;
    		d = (d && oexpr());
    		continue;
    	}

    	break;
    }
    return d;	
}

/* | */
static int
oexpr()
{
    int d;

    d = xexpr();
    while (last_token != EOF) {
    	if (last_token == '|' && *ptr != '|') {
    		d |= xexpr();
    		continue;
    	}

    	break;
    }
    return d;
}

/* ^ */
static int
xexpr()
{
    int d;

    d = amexpr();
    while (last_token != EOF) {
    	if (last_token == '^') {
    		d ^= amexpr();
    		continue;
    	}

    	break;
    }
    return d;
}

/* & */
static int
amexpr()
{
    int d;

    d = eqexpr();
    while (last_token != EOF) {
    	if (last_token == '&' && *ptr != '&') {
    		d &= eqexpr();
    		continue;
    	}

    	break;
    }
    return d;
}

/*  == or !=  */
static int
eqexpr()
{
    int d;

    d = lgexpr();
    while (last_token != EOF) {
    	if (last_token == '=' && *ptr == '=') {
    		ptr++;
    		d = (d == lgexpr());
    		continue;
    	}

    	if (last_token == '!' && *ptr == '=') {
    		ptr++;
    		d = (d != lgexpr());
    		continue;
    	}

    	break;
    }
    return d;
}

/* <, >, <=, >= */
static int
lgexpr()
{
	int d;

	d = siexpr();
	while (last_token != EOF) {
		if (last_token == '<' && *ptr == '=') {
			ptr++;
			d = (d <= siexpr());
			continue;
		}

		if (last_token == '<') {
			d = (d < siexpr());
			continue;
		}

		if (last_token == '>' && *ptr == '=') {
			ptr++;
			d = (d >= siexpr());
			continue;
		}

		if (last_token == '>') {
			d = (d > siexpr());
			continue;
		}

		break;
	}

	return d;
}

/* <<, >> */
static int
siexpr()
{
	int d;

    d = aexpr();
    while (last_token != EOF) {
    	if (last_token == '>' && *ptr == '>') {
    		ptr++;
			d >>= aexpr(); 
			continue;
		}

		if (last_token == '<' && *ptr == '<') {
			ptr++;
			d <<= aexpr();
			continue;
		}

		break;
    }

    return d;
}

/* + or - */
static int
aexpr()
{
    int d;

    d = mexpr();
    while (last_token != EOF) {
    	if (last_token == '-') {
			d -= mexpr(); 
			continue;
		}

		if (last_token == '+') {
			d += mexpr();
			continue;
		}

		break;
    }

    return d;
}

/* * or / */
static int
mexpr()
{
    int d;

    d = term();
    while (last_token != EOF) {
    	if (last_token == '*') {
			d *= term();
			continue;
		}

		if (last_token == '/') {
			d /= term();
			continue;
		}

		if (last_token == '%') {
			d %= term();
			continue;
		}

		break;
    }
    return d;
}

static int 
term()
{
    int d;

    lvalue = -1;
    token();

    switch (last_token) {
    case NUM:
	case HEX:
	case OCT:
		d = value;
		token();
		return d;
    case VAR:
		d = lvalue = value;
		token();
		return variable[d];
	case '+':
		d = expr();
		token();
		return d;
	case '-':
		d = -1 * expr();
		token();
		return d;
    case '(':
		d = expr();
		if (last_token != ')')
		    error("Unbalanced parenthsis");
		token();
		return d;
    default:
		token();
		error("Unknown term");
	case NIL:
		break;
    }

    return 0;
}

int
main() 
{
    int d;
    char buf[BUFSIZ];

    while (fgets(buf, BUFSIZ, stdin)) {
		ptr = buf;
		d = expr();
		if ((buf[0] != '/' && buf[1] != '/') && buf[0] > ' ')
			printf("%s = 0x%08x = %d\n", buf, d, d);
		else
			printf("%s", buf);
		fflush(stdout);
    }
    return 0;
}

/* end */
