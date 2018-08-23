/*
 * this would contain methods to handle memory using
 * mark-and-sweep garbage collection algorithm
 */
#include "../include/gc.h"

#include "../include/vector.h"
#include "../include/sexpr.h"
#include "../include/pair.h"

static vector_t *gc_allocated_sexprs;

void gc_init(void) {
    gc_allocated_sexprs = vector_new(gc_free_sexpr, sexpr_describe);
}

void gc_clean(void) {
    vector_free(gc_allocated_sexprs);
}

long gc_allocated_size(void) {
    return (gc_allocated_sexprs->size * sizeof(sexpr_t));
}

bool_t gc_has_space_left() {
    assert(GC_FREQUENCY > 0);
    return gc_allocated_size() < GC_RATIO;
}

/* ===================================================================
** FIXME: NOW
 */
void gc_collect(bool_t iscleanup) {
#if GC_DEBUG == DBG_ON
    if (iscleanup)
	printf("%s ", "final");
    else
	puts("collectng");
#endif

    /* ignore garbage collection in case of not surpassing limit */
    if (gc_has_space_left() && !iscleanup) {
	return;
    }

    /* gc_mark_stack_sexprs(gc_allocated_sexprs); */
    gc_sweep_sexprs(gc_allocated_sexprs);

#if GC_DEBUG == DBG_ON
    puts("======================================\n");
#endif

}

void gc_mark_sexpr(sexpr_t * expr) {
    assert(expr != NULL);

    if (expr->gci.ismarked) {
	return;			/* already marked as reachable */
    }

    expr->gci.ismarked = true;

    if (expr->type == T_NIL) {
	return;
    } else if (expr->type == T_PAIR) {
	sexpr_t *head = car(expr), *rest = cdr(expr);

	if (head)
	    gc_mark_sexpr(car(expr));
	if (rest)
	    gc_mark_sexpr(cdr(expr));
    }
}

void gc_mark_stack_sexprs(vector_t * v) {
    int i;

    /* marking all that stacka as reachable */
    for (i = 0; i < v->size; ++i) {
	if (i % 2 == 0)		/* testing */
	    gc_mark_sexpr(vector_get(v, i));
    }
}

void gc_sweep_sexprs(vector_t * v) {
    int i;
    sexpr_t *tmp;

#if GC_DEBUG == DBG_ON
    int freed = 0, size = v->size;

    puts("stack before");
    vector_debug(stderr, v);
#endif

    for (i = 0; i < v->size; ++i) {
	tmp = vector_get(v, i);

	if (!tmp->gci.ismarked) {
	    gc_free_sexpr(tmp);
	    vector_set(v, i, NULL);

#if GC_DEBUG == DBG_ON
	    ++freed;		/* not reachable */
#endif

	} else {
	    /* mark already reachable as not reachable */
	    tmp->gci.ismarked = false;
	}
    }

#if GC_DEBUG == DBG_ON
    puts("stack after");
    vector_debug(stderr, v);
#endif

    vector_compact(v);

#if GC_DEBUG == DBG_ON
    printf("previous: %d - current: %d - added: %d  - freed: %d \n",
	   size, v->size, size - freed, freed);

    puts("final stack");
    vector_debug(stderr, v);
#endif

}

sexpr_t *gc_alloc_sexpr(void) {
    /* FIXME: check this line along while making teh evaluation */

    if (!gc_has_space_left()) {
	gc_collect(true);
    }

    sexpr_t *s = malloc(sizeof *s);

    s->gci.ismarked = false;

    vector_push(gc_allocated_sexprs, s);

    return s;
}

void gc_free_sexpr(object_t o) {
    if (o == NULL)
	return;

    sexpr_t *expr = o;

    sexpr_describe(expr);

    /* because they got an allocated string */
    if (expr->type == T_STRING || expr->type == T_ATOM) {
	free(expr->v.s);
    } else if (expr->type == T_PAIR) {
	gc_free_sexpr(car(expr));
	gc_free_sexpr(cdr(expr));
    }

    free(expr);
}

#if GC_DEBUG == DBG_ON
void gc_debug_memory(void) {
    sexpr_t *s = sexpr_new(T_ATOM);
    s->v.s = strdup("foo");
    sexpr_t *ss = sexpr_new(T_NUMBER);
    ss->v.n = 502;
    sexpr_t *sss = sexpr_new(T_STRING);
    sss->v.s = strdup("this is a string");

    /* making the atom as reachable */
    gc_mark_sexpr(s);

    /* force garbage collection */
    gc_collect(true);

    sexpr_t *ssss = sexpr_new(T_NUMBER);
    ssss->v.n = 775;
}
#endif
