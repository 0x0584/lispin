/*
 * this would contain some native implementation to functions
 * i.e. built-in into the interpreter
 */
#include "../include/native.h"
#include "../include/sexpr.h"
#include "../include/pair.h"

sexpr_t *native_add(sexpr_t * expr) {
    sexpr_t *result = NULL, *tmp = expr, *value;
    number_t n = 0;

    while (!isnil(tmp)) {
	err_raise(ERR_ARG_TYPE, !isnumber(value = car(tmp)));

	if (err_log())
	    return sexpr_err();

	n += value->n;
	tmp = cdr(tmp);
    }

    result = sexpr_new(T_NUMBER);
    result->n = n;

    return result;
}

sexpr_t *native_minus(sexpr_t * expr) {
    number_t n = 0;
    sexpr_t *result = NULL, *tmp = expr, *value;
    bool isfirst = true;

    while (!isnil(tmp)) {
	err_raise(ERR_ARG_TYPE, !isnumber(value = car(tmp)));

	if (err_log())
	    return sexpr_err();

	n -= value->n;

	if (isfirst) {
	    n *= -1;
	    isfirst = false;
	}

	tmp = cdr(tmp);
    }

    result = sexpr_new(T_NUMBER);
    result->n = n;

    return result;
}

sexpr_t *native_times(sexpr_t * expr) {
    number_t n = 1;
    sexpr_t *result = NULL, *tmp = expr, *value;

    while (!isnil(tmp)) {
	err_raise(ERR_ARG_TYPE, !isnumber(value = car(tmp)));

	if (err_log())
	    return sexpr_err();

	n *= value->n;
	tmp = cdr(tmp);
    }

    result = sexpr_new(T_NUMBER);
    result->n = n;

    return result;
}

sexpr_t *native_divid(sexpr_t * expr) {
    sexpr_t *result = NULL, *tmp, *tmp0;

    err_raise(ERR_ARG_COUNT, sexpr_length(expr) > 2);
    err_raise(ERR_ARG_TYPE, !isnumber(tmp = car(expr)));
    err_raise(ERR_ARG_TYPE, !isnumber(tmp0 = car(cdr(expr))));

    if (err_log())
	return sexpr_err();

    err_raise(ERR_ARG_TYPE, !tmp0->n);

    if (err_log())
	return sexpr_err();

    result = sexpr_new(T_NUMBER);
    result->n = (number_t) tmp->n / tmp0->n;;

    return result;
}

/* (or s-exprs) */
sexpr_t *native_or(sexpr_t * expr) {
    sexpr_t *tmp = expr;

    err_raise(ERR_ARG_COUNT, sexpr_length(expr) < 2);

    if (err_log())
	return sexpr_err();

    while (!isnil(tmp))
	if (!isnil(car(tmp)))
	    return car(tmp);
	else
	    tmp = cdr(tmp);

    return tmp;
}

/* (not s-expr) */
sexpr_t *native_not(sexpr_t * expr) {
    err_raise(ERR_ARG_COUNT, sexpr_length(expr) != 1);

    if (err_log())
	return sexpr_err();

    return isnil(expr) ? sexpr_true() : sexpr_nil();
}

/* (and s-exprs) */
sexpr_t *native_and(sexpr_t * expr) {
    sexpr_t *tmp = expr;

    err_raise(ERR_ARG_COUNT, sexpr_length(expr) < 2);

    if (err_log())
	return sexpr_err();

    while (!isnil(tmp))
	if (isnil(car(tmp)))
	    return car(tmp);
	else
	    tmp = cdr(tmp);

    return sexpr_true();
}

/* (pair? sexpr) */
sexpr_t *native_ispair(sexpr_t * expr) {
    err_raise(ERR_ARG_COUNT, sexpr_length(expr) != 1);

    if (err_log())
	return sexpr_err();

    return ispair(expr) ? sexpr_true() : sexpr_nil();
}

/* (eq? sexprs) */
sexpr_t *native_iseq(sexpr_t * expr) {
    sexpr_t *tmp = expr, *tmp0, *tmp1;

    err_raise(ERR_ARG_COUNT, sexpr_length(expr) < 2);

    if (err_log())
	return sexpr_err();

    do {
	tmp0 = car(tmp);
	tmp1 = car(cdr(tmp));

	if (tmp0->type != tmp1->type)
	    return sexpr_nil();

	if (isnumber(tmp0))
	    if (tmp0->n != tmp1->n)
		return sexpr_nil();
	    else
		continue;
	else if (isstring(tmp0))
	    if (tmp0->n != tmp1->n)
		return sexpr_nil();
	    else
		continue;
	else
	    continue;
    } while (!isnil(tmp = cdr(cdr(tmp))));

    return sexpr_true();
}

/* (atom? sexpr) */
sexpr_t *native_isatom(sexpr_t * expr) {
    err_raise(ERR_ARG_COUNT, sexpr_length(expr) != 1);

    if (err_log())
	return sexpr_err();

    return isatom(expr) ? sexpr_true() : sexpr_nil();
}

/* (cons sexpr0 sexpr1) */
sexpr_t *native_cons(sexpr_t * expr) {
    err_raise(ERR_ARG_COUNT, sexpr_length(expr) != 2);

    if (err_log())
	return sexpr_err();

    return cons(car(expr), car(cdr(expr)));
}

/* (car sexpr) */
sexpr_t *native_car(sexpr_t * expr) {
    err_raise(ERR_ARG_COUNT, sexpr_length(expr) != 1);
    err_raise(ERR_ARG_TYPE, !ispair(expr));

    if (err_log())
	return sexpr_err();

    return car(car(expr));
}

/* (cdr sexpr) */
sexpr_t *native_cdr(sexpr_t * expr) {
    err_raise(ERR_ARG_COUNT, sexpr_length(expr) != 1);
    err_raise(ERR_ARG_TYPE, !ispair(expr));

    if (err_log())
	return sexpr_err();

    return cdr(car(expr));
}
