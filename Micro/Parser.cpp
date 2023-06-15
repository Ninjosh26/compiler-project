#include <stdio.h>
#include "Parser.h"
#include "Scanner.h"

token current_token;
extern char token_buffer[];
extern int curr_buffer_idx;

void Parser::system_goal() {
    /* <system goal> ::= <program> SCANEOF */
    program();
    match(token::SCANEOF);
}

void Parser::program() {
    /* <program> ::= BEGIN <statement list> END */
    match(token::BEGIN);
    statement_list();
    match(token::END);
}

void Parser::statement_list() {
    /* <statement list> ::= <statement> {<statement>} */
    statement();
    while (true) {
        switch (next_token()) {
            case token::ID:
            case token::READ:
            case token::WRITE:
                statement();
                break;
            default:
                return;
        }
    }
}

void Parser::statement() {
    token next = next_token();

    switch (next) {
        case token::ID:
            /* <statement> ::= ID := <expression>; */
            match(token::ID);
            match(token::ASSIGNOP);
            expression();
            match(token::SEMICOLON);
            break;
        case token::READ:
            /* <statement> ::= READ (<id list>); */
            match(token::READ);
            match(token::LPAREN);
            id_list();
            match(token::RPAREN);
            match(token::SEMICOLON);
            break;
        case token::WRITE:
            /* <statement> ::= WRITE (<expr list>); */
            match(token::WRITE);
            match(token::LPAREN);
            expr_list();
            match(token::RPAREN);
            match(token::SEMICOLON);
            break;
        default:
            syntax_error(next);
            break;
    }
}

void Parser::id_list() {
    /* <id list> ::= ID {, ID} */
    match(token::ID);
    while (next_token() == token::COMMA) {
        match(token::COMMA);
        match(token::ID);
    }
}

void Parser::expr_list() {
    /* <expr list> ::= <expression> {, <expression>} */
    expression();
    while (next_token() == token::COMMA) {
        match(token::COMMA);
        expression();
    }
}

void Parser::expression() {
    /* <expression> ::= <primary> {<add op> <primary>} */
    primary();
    while (true) {
        switch(next_token()) {
            case token::PLUSOP:
            case token::MINUSOP:
                add_op();
                primary();
            default:
                return;
        }
    }
}

void Parser::primary() {
    token next = next_token();
    switch (next) {
        case token::LPAREN:
            /* <primary> ::= (<expression>) */
            match(token::LPAREN);
            expression();
            match(token::RPAREN);
            break;
        case token::ID:
            /* <primary> ::= ID */
            match(token::ID);
            break;
        case token::INTLITERAL:
            /* <primary> ::= INTLITERAL */
            match(token::INTLITERAL);
            break;
        default:
            syntax_error(next);
    }
}

void Parser::add_op() {
    token next = next_token();
    switch(next) {
        case token::PLUSOP:
            /* <add op> ::= PLUSOP */
            match(token::PLUSOP);
            break;
        case token::MINUSOP:
            /* <add op> ::= MINUSOP */
            match(token::MINUSOP);
            break;
        default:
            syntax_error(next);
    }
}

void Parser::match(token t) {
    token next = scanner();
    if (t != next) {
        syntax_error(next);
    }
    current_token = next;
}

token Parser::next_token() {
    token next = scanner();

    switch (next) {
        case token::BEGIN:
        case token::END:
        case token::ID:
        case token::WRITE:
        case token::READ:
        case token::INTLITERAL:
            for (int i = curr_buffer_idx - 1; i >= 0; i--) {
                ungetc(token_buffer[i], stdin);
            }
            break;
        case token::ASSIGNOP:
            ungetc('=', stdin); ungetc(':', stdin);
            break; 
        case token::COMMA:
            ungetc(',', stdin);
            break;   
        case token::LPAREN:
            ungetc('(', stdin);
            break;
        case token::MINUSOP:
            ungetc('-', stdin);
            break;
        case token::PLUSOP:
            ungetc('+', stdin);
            break;
        case token::RPAREN:
            ungetc(')', stdin);
            break;
        case token::SEMICOLON:
            ungetc(';', stdin);
            break;
    }

    return next;
}