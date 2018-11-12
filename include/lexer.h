#ifndef _SCMIN_LEXER_H
#  define _SCMIN_LEXER_H

/**
 * @file lexer.h
 *
 * this file contains declarations of functionalities to verify the grammar
 * of a Scheme-like code and get all tokens in there and then using
 * read_tokes() or read_stream_tokens(), it collects the tokens in a into
 * a Vector
 *
 * @see token.h
 * @see vector.h
 */

#  include "main.h"
#  include "token.h"
#  include "characters.h"

/**
 * read and record all the tokens that are in @p code in a Vector
 * by calling next_token() to get each one
 *
 * @see vector.h
 * @see token.h
 *
 * @return a vector of tokens found in @p code
 */
vector_t *read_tokens(string_t code);

/**
 * read and record all the tokens that are coming from @p stream
 * by first calling file_as_string() to get the code
 * then, it calls read_tokens() to get each one in a Vector
 *
 * @see vector.h
 * @see token.h
 * @see characters.h
 *
 * @return a vector of tokens found in @p code
 */
vector_t *read_stream_tokens(const char *filename);

/**
 * @berief move through all the tokens in a giving @p code.
 *
 * first, it calls clean_comments() and clean_whitespaces() to
 * clean the @p code. after that, using getnc() to keep track on
 * the stream, it gets a character. it calls predict_token_type()
 * to determine the type of the next token. the it calls one of the
 * read functions based on the result.
 *
 * @return a Token
 *
 * @see getnc()
 * @see token.h
 *
 * @note this function modifies the static values in of getnc()
 */
token_t *next_token(char *code);

/**
 * reads the token value from @p code as string
 *
 * @param code a Scheme-like syntax
 *
 * @return value of the token if type matches, or NULL otherwise
 *
 * @see getnc()
 * @see token.h
 *
 * @note this function modifies the static values in of getnc()
 */
string_t read_string(string_t code);

/**
 * reads the token value from @p code as number
 *
 * @param code a Scheme-like syntax
 *
 * @return value of the token if type matches, or NULL otherwise
 *
 * @see getnc()
 * @see token.h
 *
 * @note this function modifies the static values in of getnc()
 */
string_t read_number(string_t code);

/**
 * reads the token value from @p code as symbol
 *
 * @param code a Scheme-like syntax
 *
 * @return value of the token if type matches, or NULL otherwise
 *
 * @see getnc()
 * @see token.h
 *
 * @note this function modifies the static values in of getnc()
 */
string_t read_symbol(string_t code);

void lexer_testing(void);

#endif				/* _SCMIN_LEXER_H */
