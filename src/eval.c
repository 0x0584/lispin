/**
 * @file eval.c
 *
 * @brief declarations of the core evaluation functions, those would take
 * a parsed s-expressions and evaluate them into another s-expression.
 *
 * the main method here is eval_sexpr() for a single expression, and
 * eval_sexprs() for a vector of sexprs, both functions require parsed
 * s-expressions. in addition of the expressions to evaluate, a
 * containing scope is required, which would basically hold bonded
 * and/or predefined ones. 
 *
 * @see eval.h
 * @see parser.h
 * @see scope.h
 * @see native.h
 */

#include "../include/eval.h"
#include "../include/scope.h"
#include "../include/native.h"

#include "../include/pair.h"
#include "../include/vector.h"
#include "../include/characters.h"

/**
 * @brief evaluate an expression `expr` within a given `scope` 
 *
 * before evaluating each expression, we need to determine its type, there
 * are native/predefined expression that would be executed directly using
 * a predefined C function. and other expressions that are written in need
 * evaluation.
 *
 * the first thing to do is to determine the type whether it's a normal
 * s-expression or does it has an operator:
 *
 *   + the expression'd be a keyword, if so, we pass the cdr(), i.e. args
 *     to the related function returned by iskeyword() so that it runs
 *     evaluation on it's own and returns an evaluated s-expression.
 *
 *   + the expression'd be bonded to a symbol, if so, resolve the bond
 *     using resolve_bond() and return the result.
 *
 *   + the expression'd be an atom, if so just return it. (if symbol was
 *     not bonded it would be returned laterally)
 *
 * if none of the above situation was true, then it's must has an operator,
 * so get the operator which is the car() of the expression, and then we
 * collect an evaluated version of the args by calling eval_sexpr() on each
 * cadr() until the cdr() of the expression is nil.
 *
 * next, we look to see if the operator was a native one, then we call the
 * related native function passing the arguments. or if the the operator was
 * a bond lambda, we create a new scope (child scope of the current scope)
 * then bind the lambda arguments to the arguments in the child scope and
 * evaluate the lambda's body passing the new child scope. finally, the result
 * is returned
 *
 * @param scope the contaning scope
 * @param expr a s-expreesion to evaluate
 *
 * @return the evaluated s-expression
 *
 * @note this fucntion may call itself recursively
 */
sexpr_t *eval_sexpr(scope_t * scope, sexpr_t * expr) {
    if (expr == NULL)
	return sexpr_nil();

    sexpr_t *result = NULL, *op = NULL;	/* operator */
    k_func kwd_func = iskeyword(car(expr));

    /* ==================== ==================== ==================== */

#if EVALUATOR_DEBUG == DBG_ON
    puts("================ eval start ================");
    sexpr_print(expr);
    puts("============================================");
#endif

    if (kwd_func)		/* symbol was a keyword */
	result = kwd_func(scope, cdr(expr));
    else if (isbonded(scope, expr))	/* symbol was bounded  */
	result = resolve_bond(scope, expr);
    else if (isatom(expr))	/* just an atom/nil */
	result = expr;

    /* ==================== ==================== ==================== */

#if EVALUATOR_DEBUG == DBG_ON
    puts(result ? "we have a result" : "there is no result");
    sexpr_print(result);
#endif

    if (result)			/* we have a result */
	goto RET;
    else if (!(op = eval_sexpr(scope, car(expr))))
	goto FAILED;		/* no operator was found */

#if EVALUATOR_DEBUG == DBG_ON
    puts(op ? "we have an operator " : "there is no operator");
    sexpr_print(op);
#endif

    /* ==================== ==================== ==================== */

    sexpr_t *args = NULL, *tail = NULL;
    sexpr_t *foo = expr, *bar = NULL, *nil = sexpr_nil();

    /* creating a list of arguments */
    while (!isnil(foo = cdr(foo))) {
	bar = cons(eval_sexpr(scope, car(foo)), nil);

	if (!args)
	    args = bar;
	else
	    set_cdr(tail, bar);

	tail = bar;
    }

#if EVALUATOR_DEBUG == DBG_ON
    puts("args: ");
    printf("length: %d \n", sexpr_length(args));
    sexpr_print(args);
#endif

    /* ==================== ==================== ==================== */

    if (op->l->isnative)	/* call the native function */
	result = op->l->native->func(args);
    else {			/* evaluate the lambda's body */
	/* puts("###"); */
	err_raise(ERR_LMBD_ARGS,
		  sexpr_length(args) != sexpr_length(op->l->args));

	if (err_log())
	    goto FAILED;

	scope_t *child = scope_init(scope);

	bind_lambda_args(child, op->l, args);
	result = eval_sexpr(child, op->l->body);
    }

    err_raise(ERR_RSLT_NULL, !result);

    /* FIXME: clean memory after the evaluation ends
     * which would be done using evaluation contexts */
    /* gc_collect(true); */

#if EVALUATOR_DEBUG == DBG_ON
    puts("result: ");
    sexpr_print(result);
#endif

  RET:
    /* puts("$$$$$$$$$$$$"); */
    return result;

  FAILED:
    return NULL;
}

/**
 * @brief just like eval_sexpr(), but with a vector of s-expressions
 * 
 * @param sexprs a vector of s-expressions
 * 
 * @return a vector of the evaluated s-expressions
 *
 * @see eval_sexpr()
 * @see vector.h
 */
vector_t *eval_sexprs(vector_t * sexprs) {
    vector_t *v = vector_new(NULL, sexpr_print, NULL);
    int i;

    for (i = 0; i < sexprs->size; ++i) {
/* #if EVALUATOR_DEBUG == DBG_ON */
	sexpr_print(vector_get(sexprs, i));
	sexpr_t *tmp =
/* #endif */
	    vector_push(v, eval_sexpr(get_global_scope(),
				      vector_get(sexprs, i)));

/* #if EVALUATOR_DEBUG == DBG_ON */
	printf("> ");
	sexpr_print(tmp);
	puts("");
/* #endif */

    }

    return vector_compact(v);
}
