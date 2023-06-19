#include <stdio.h>
#include <ctype.h>
#include <stdexcept>
#include <string.h>
#include "Scanner.h"

char token_buffer[32];
int curr_buffer_idx = 0;

void clear_buffer() {
    for (int i = 0; i < 32; i++) {
        token_buffer[i] = '\0';
    }

    curr_buffer_idx = 0;
}

void buffer_char(int c) {
    if (curr_buffer_idx == 32) { lexical_error(-2); }
    token_buffer[curr_buffer_idx++] = c; 
}

token check_reserved() {
    // begin, end, read, and write are reserved
    if (strcmp(token_buffer, "begin") == 0) { return BEGIN; }
    if (strcmp(token_buffer, "write") == 0) { return WRITE; }
    if (strcmp(token_buffer, "read") == 0) { return READ; }
    if (strcmp(token_buffer, "end") == 0) { return END; }

    return ID;
}

void lexical_error(int c) {
    if (c == -2) { throw std::runtime_error("Identifier is too long"); }
    throw std::runtime_error("Unidentified character: " + char(c));
}

token scanner() {
    int in_char, c;
    clear_buffer();

    if (feof(stdin)) {
        return SCANEOF;
    }

    while ((in_char = getchar()) != EOF) {
        if (isspace(in_char)) { // Skip whitespace
            continue;
        } else if (isalpha(in_char)) {
            /*
             * ID ::= LETTER | ID LETTER
             *               | ID DIGIT
             *               | ID UNDERSCORE
             */
            buffer_char(in_char);

            for (c = getchar(); isalnum(c) || c == '_'; c = getchar()) {
                buffer_char(c);
            }

            ungetc(c, stdin);
            return check_reserved();
        } else if (isdigit(in_char)) {
            /*
             * INTLITERAL ::= DIGIT | INTLITERAL DIGIT
             */
            buffer_char(in_char);

            for (c = getchar(); isdigit(c); c = getchar()) {
                buffer_char(c);
            }

            ungetc(c, stdin);
            return INTLITERAL;
        } else if (in_char == '(') {
            return LPAREN;
        } else if (in_char == ')') {
            return RPAREN;
        } else if (in_char == ';') {
            return SEMICOLON;
        } else if (in_char == ',') {
            return COMMA;
        } else if (in_char == '+') {
            return PLUSOP;
        } else if (in_char == ':') {
            // Looking for ":="
            c = getchar();
            if (c == '=') {
                return ASSIGNOP;
            } else {
                ungetc(c, stdin);
                lexical_error(in_char);
            }
        } else if (in_char == '-') {
            // Looking for --, comment start
            c = getchar();
            if (c == '-') {
                while ((in_char = getchar()) != '\n');
            } else {
                ungetc(c, stdin);
                return MINUSOP;
            }
        } else {
            lexical_error(in_char);
        }
    }

    return SCANEOF;
}