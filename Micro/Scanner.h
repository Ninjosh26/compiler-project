#ifndef _SCANNER_H_
#define _SCANNER_H_

// Predefined token types
enum token_types {
    BEGIN, END, READ, WRITE, ID, INTLITERAL, LPAREN, RPAREN, SEMICOLON,
    COMMA, ASSIGNOP, PLUSOP, MINUSOP, SCANEOF
};

// Refer to any instance of token_types as a token
using token = token_types;

// Buffer functions
void clear_buffer();
void buffer_char(int c);
token check_reserved();

token scanner();

void lexical_error(int c);

#endif