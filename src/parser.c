/**
 * @file parser.c
 *
 * @brief contains the declaration of parsing functionalities
 *
 * the main method here is parse_sexpr()
 *
 * @see vector.h
 * @see token.h
 */

#include "parser.h"

#include "vector.h"
#include "token.h"

#include "pair.h"
#include "sexpr.h"

/**
 * @brief this function is responsible of turning a set of tokens into a
 * s-expression.
 *
 * first it reads a set of tokens and then it parses each one as
 * described below:
 *
 *    + if it was a list, then parse_as_list()
 *    + if it was either a number/string/symbol then parse_as_x()
 *    + if it was was quote then parse_as_quote()
 *    + if the token was and error-token, then stop the process!
 */
sexpr_t *parse_sexpr(vector_t * tokens) {
    sexpr_t *expr = NULL, *value = NULL;
    sexpr_t *head = NULL, *tail = NULL;
    sexpr_t *nil = sexpr_nil();
    token_t *token = NULL;

    while ((token = vector_peek(tokens))) {
#if DEBUG_PARSER == DEBUG_ON
	puts("current token:");
	token_print(token);
	putchar('\n');
#endif

	switch (token->type) {
	case TOK_ERR:
	    token_free(token);
	    goto FAILED;

	case TOK_L_PAREN:
	    value = parse_as_list(tokens);
	    break;

	case TOK_QUOTE:
	    value = parse_as_quote(tokens);
	    break;

	case TOK_NUMBER:
	    value = parse_as_number(token->vbuffer);
	    break;
	case TOK_SYMBOL:
	    value = parse_as_symbol(token->vbuffer);
	    break;
	case TOK_STRING:
	    value = parse_as_string(token->vbuffer);
	    break;

	default:
	    break;
	}

	token_free(token);

	if (tokens->size == 0 && !head) {
	    head = value;
	    break;
	}

	expr = cons(value, nil);

	if (!head)
	    head = expr;
	else
	    set_cdr(tail, expr);

	tail = expr;
    }

    return head;

  FAILED:
    return NULL;
}

vector_t *parse_sexprs(vector_t * vtokens) {
    int i;
    vector_t *v = vector_new(NULL, sexpr_print, NULL);

    for (i = 0; i < vtokens->size; ++i) {
#if DEBUG_PARSER == DEBUG_ON
	puts(" ========== vector of tokens =========== ");
	vector_print(vector_get(vtokens, i));
	puts(" ========== ================ =========== ");
	sexpr_t *tmp =
#endif
	    vector_push(v, parse_sexpr(vector_get(vtokens, i)));

#if DEBUG_PARSER == DEBUG_ON
	printf("parsed sexpr: ");
	sexpr_print(tmp);
#endif
    }

    return vector_compact(v);
}

sexpr_t *parse_as_list(vector_t * tokens) {
    sexpr_t *expr = NULL, *value = NULL;
    sexpr_t *head = NULL, *tail = NULL;
    sexpr_t *nil = sexpr_nil();

    token_t *token = NULL;
    bool isfirstloop = true;

#if DEBUG_PARSER == DEBUG_ON
    puts("list starting");
#endif

    while ((token = vector_peek(tokens))) {
#if DEBUG_PARSER == DEBUG_ON
	puts("current token:");
	token_print(token);
	putchar('\n');
#endif
	if (token->type == TOK_R_PAREN) {
	    token_free(token);

	    if (isfirstloop)	/* '() empty list is like a nil */
		return nil;
	    else		/* reached the end of the list */
		break;
	}

	switch (token->type) {
	case TOK_L_PAREN:
	    value = parse_as_list(tokens);
	    break;

	case TOK_QUOTE:
	    value = parse_as_quote(tokens);
	    break;

	case TOK_NUMBER:
	    value = parse_as_number(token->vbuffer);
	    break;
	case TOK_STRING:
	    value = parse_as_string(token->vbuffer);
	    break;
	case TOK_SYMBOL:
	    value = parse_as_symbol(token->vbuffer);
	    break;

	default:

#if DEBUG_PARSER == DEBUG_ON
	    puts("UNKNOWN SYMBOL");
#endif
	    return NULL;	/* this would cause problems */
	}

	token_free(token);

	expr = cons(value, nil);

	if (!head)
	    head = expr;
	else
	    set_cdr(tail, expr);

	tail = expr;

	isfirstloop = false;
    }

#if DEBUG_PARSER == DEBUG_ON
    sexpr_print(head);
    puts("list ending\n--------------\n");
#endif

    return head;
}

sexpr_t *parse_as_quote(vector_t * tokens) {
    token_t *token;
    sexpr_t *quote = NULL, *value = NULL;

    token = vector_peek(tokens);

    switch (token->type) {
    case TOK_L_PAREN:
	value = parse_as_list(tokens);
	break;

    case TOK_NUMBER:
	value = parse_as_number(token->vbuffer);
	break;
    case TOK_STRING:
	value = parse_as_string(token->vbuffer);
	break;
    case TOK_SYMBOL:
	value = parse_as_symbol(token->vbuffer);
	break;

    default:
	puts("UNKNOWN SYMBOL");
	return NULL;		/* this would cause problems */
    }

    token_free(token);

    if (isnil(value))
	return value;

    quote = sexpr_new(SCMIN_SYMBOL);
    quote->s = strdup("quote");

    value = cons(quote, cons(value, sexpr_nil()));

#if DEBUG_PARSER == DEBUG_ON
    sexpr_print(value);
#endif

    return value;
}

sexpr_t *parse_as_number(string_t value) {
    assert(value != NULL);
    sexpr_t *expr = sexpr_new(SCMIN_NUMBER);
    expr->n = strtod(value, NULL);
    return expr;
}

sexpr_t *parse_as_string(string_t value) {
    assert(value != NULL);
    sexpr_t *expr = sexpr_new(SCMIN_STRING);
    expr->s = strdup(value);
    return expr;
}

sexpr_t *parse_as_symbol(string_t value) {
    sexpr_t *expr = NULL;
    expr = parse_as_string(value);
    expr->type = !strcmp(value, "nil") ? SCMIN_NIL : SCMIN_SYMBOL;
    return expr;
}
