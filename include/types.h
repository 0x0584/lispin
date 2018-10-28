#ifndef _SCMIN_TYPES_H
#  define _SCMIN_TYPES_H

typedef enum SCHEME_ERROR serror_t;
typedef struct ERROR error_t;

/* garbage Collector information */
typedef struct GC_INFO gc_info;

typedef double number_t;
typedef char *string_t;
typedef void *object_t;
typedef struct VECTOR vector_t;

typedef struct SCOPE scope_t;
typedef struct BOND bond_t;
typedef struct CONTEXT context_t;

typedef struct TOKEN token_t;
typedef enum TOKEN_TYPE token_type;

typedef enum SYMBOLIC_EXPRESSION_TYPE type_t;
typedef struct PAIR pair_t;
typedef struct LAMBDA_EXPRESSION lambda_t;
typedef struct NATIVE_LAMBDA native_t;
typedef struct SYMBOLIC_EXPRESSION sexpr_t;

/* keyword function */
typedef sexpr_t *(*k_func) (scope_t *, sexpr_t *);
typedef struct KEYWORD keyword_t;

typedef struct BEHAVIOR behavior_t;
#endif				/* _SCMIN_TYPES_H */
