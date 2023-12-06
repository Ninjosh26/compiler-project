#ifndef _PARSER_H_
#define _PARSER_H_

#include "Scanner.h"

enum NonTerminal {
    // Operators (POW_OP not needed since it is unique)
    MUL_OP, ADD_OP, REL_OP, ASSIGN_OP, UNARY_OP,

    // Type
    TYPE,

    // Literals (integer and float handled by Scanner)
    BOOL_LIT, LITERAL,

    // Designator (ident handled by Scanner)
    DESIGNATOR,

    // Expression-related nonterminals
    GROUP_EXPR, POW_EXPR, MULT_EXPR, ADD_EXPR, REL_EXPR, RELATION, 

    // Statements 
    ASSIGN, FUNC_CALL, ASSIGN_STAT, FUNC_CALL_STAT, IF_STAT, WHILE_STAT, 
    DO_WHILE_STAT, FOR_STAT, REPEAT_STAT, RETURN_STAT, STATEMENT, STAT_SEQ,

    // Declarations
    TYPE_DECL, VAR_DECL, PARAM_TYPE, PARAM_DECL, DECL_LIST,

    // Functions
    PARAM_LIST, FUNC_BODY, FUNC_DECL,

    // Program
    PROGRAM,

    // Used for getting size of enum
    SIZE,
};

class Parser {
private:

    Token currToken;

public:

    // Useful for seeing the first sets of each nonterminal
    static void printFirstSets();

private:

    // Check if current token is a specific type
    bool have(Token::Kind kind);
    // Check if current token is in first set
    bool have(NonTerminal nt);
    // Consume token if next
    bool accept(Token::Kind kind);
    // Consume token if in first set
    bool accept(NonTerminal nt);
    // Attempt to consume token, will throw if not provided type
    bool expect(Token::Kind kind);
    // Attempt to consume token, will throw if not in first set
    bool expect(NonTerminal nt);
    // Will retrieve if next token, otherwise will throw
    Token expectRetrieve(Token::Kind kind);
    // Will retrieve if next token in first set, otherwise will throw
    Token expectRetrieve(NonTerminal nt);


    void program();
    void declList();
    void statSeq();
    void decl();
    void varDecl();
    void funcDecl();
    void ident();
    void paramList();
    void type();
    void funcBody();
    void paramDecl();
    void paramType();
    void typeDecl();
    void statement();
    void assignStat();
    void funcCallStat();
    void ifStat();
    void whileStat();
    void doWhileStat();
    void forStat();
    void repeatStat();
    void returnStat();
    void relExpr();
    void relation();
    void assign();
    void funcCall();
    void designator();
    void assignOp();
    void unaryOp();
    void relOp();
    void addOp();
    void mulOp();
    void powOp();
    void literal();
};

#endif