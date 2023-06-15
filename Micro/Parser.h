#ifndef _PARSER_H_
#define _PARSER_H_

#include "Scanner.h"

namespace Parser {
    void system_goal();
    void program();
    void statement_list();
    void statement();
    void id_list();
    void expr_list();
    void expression();
    void primary();
    void add_op();
    void match(token t);
    token next_token();
    void syntax_error(token t);
};

#endif