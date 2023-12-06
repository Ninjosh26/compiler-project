#include "Parser.h"

// First Sets, stored as bit maps
// Can store info on 8 tokens in 1 byte
static const int setSize = (Token::Kind::SIZE / 8) + (Token::Kind::SIZE % 8 ? 1 : 0);
static char firstSet[NonTerminal::SIZE][setSize] = {
    // MUL_OP
    {-31, 0, 0, 0, 0, 0, 0},
    // ADD_OP
    {26, 0, 0, 0, 0, 0, 0},
    // REL_OP
    {0, 126, 0, 0, 0, 0, 0}, 
    // ASSIGN_OP
    {0, -128, 63, 0, 0, 0, 0},
    // UNARY_OP
    {0, 0, -64, 0, 0, 0, 0},
    // TYPE
    {0, 0, 0, 14, 0, 0, 0},
    // BOOL_LIT
    {0, 0, 0, 48, 0, 0, 0},
    // LITERAL
    {0, 0, 0, 48, 0, 0, 12},
    // DESIGNATOR
    {0, 0, 0, 0, 0, 0, 16},
    // GROUP_EXPR
    {4, 0, 0, 112, 0, 64, 28}, 
    // POW_EXPR
    {4, 0, 0, 112, 0, 64, 28},
    // MULT_EXPR
    {4, 0, 0, 112, 0, 64, 28},
    // ADD_EXPR
    {4, 0, 0, 112, 0, 64, 28},
    // REL_EXPR
    {4, 0, 0, 112, 0, 64, 28},
    // RELATION
    {0, 0, 0, 64, 0, 0, 0},
    // ASSIGN
    {0, 0, 0, 0, 0, 0, 16},
    // FUNC_CALL
    {0, 0, 0, 0, 0, 64, 0},
    // ASSIGN_STAT
    {0, 0, 0, 0, 0, 0, 16},
    // FUNC_CALL_STAT
    {0, 0, 0, 0, 0, 64, 0},
    // IF_STAT
    {0, 0, 0, 0, -128, 0, 0},
    // WHILE_STAT
    {0, 0, 0, 0, 0, 2, 0},
    // DO_WHILE_STAT
    {0, 0, 0, 0, 0, 4, 0},
    // FOR_STAT
    {0, 0, 0, 0, 0, 8, 0},
    // REPEAT_STAT
    {0, 0, 0, 0, 0, 16, 0},
    // RETURN_STAT
    {0, 0, 0, 0, 0, -128, 0},
    // STATEMENT
    {0, 0, 0, 14, -128, -34, 16},
    // STAT_SEQ
    {0, 0, 0, 14, -128, -34, 16},
    // TYPE_DECL
    {0, 0, 0, 14, 0, 0, 0},
    // VAR_DECL
    {0, 0, 0, 14, 0, 0, 0},
    // PARAM_TYPE
    {0, 0, 0, 14, 0, 0, 0},
    // PARAM_DECL
    {0, 0, 0, 14, 0, 0, 0},
    // DECL_LIST
    {0, 0, 0, 14, 0, 0, 2},
    // PARAM_LIST
    {0, 0, 0, 64, 0, 0, 0},
    // FUNC_BODY
    {0, 0, 0, 0, 1, 0, 0},
    // FUNC_DECL
    {0, 0, 0, 0, 0, 0, 2},
    // PROGRAM
    {0, 0, 0, 14, 0, 0, 3},
};

// Used for getting string for NonTerminal and Token enum types
const char* enumName[] = {
    "MUL_OP", "ADD_OP", "REL_OP", "ASSIGN_OP", "UNARY_OP", "TYPE",
    "BOOL_LIT", "LITERAL", "DESIGNATOR", "GROUP_EXPR", "POW_EXPR", 
    "MULT_EXPR", "ADD_EXPR", "REL_EXPR", "RELATION", "ASSIGN", "FUNC_CALL", 
    "ASSIGN_STAT", "FUNC_CALL_STAT", "IF_STAT", "WHILE_STAT", "DO_WHILE_STAT", 
    "FOR_STAT", "REPEAT_STAT", "RETURN_STAT", "STATEMENT", "STAT_SEQ", 
    "TYPE_DECL", "VAR_DECL", "PARAM_TYPE", "PARAM_DECL", "DECL_LIST", 
    "PARAM_LIST", "FUNC_BODY", "FUNC_DECL", "PROGRAM", "&&", "||", "!", 
    "+", "-", "*", "/", "%", "^", "==", "!=", "<", "<=", ">", ">=", "=", 
    "+=", "-=", "*=", "/=", "%=", "^=", "++", "--", "void", "bool", "int", 
    "float", "true", "false", "(", ")", "{", "}", "[", "]", ",", ":", ";", 
    "if", "else", "while", "do", "for", "repeat", "until", "call", "return", 
    "main", "function", "INT_VAL", "FLOAT_VAL", "IDENT", "EOF", "ERROR"
};

std::string Parser::reportSyntaxError(Token::Kind kind) {
    std::string msg = "SyntaxError(" + std::to_string(lineNum()) + "," + std::to_string(charPos()) + ")[Expected "
        + enumName[kind + NonTerminal::SIZE] + " but got " + enumName[currToken.kind() + NonTerminal::SIZE] + ".]"; 
    errBuf.push_back(msg);
    return msg;
}

std::string Parser::reportSyntaxError(NonTerminal nt) {
    std::string msg = "SyntaxError(" + std::to_string(lineNum()) + "," + std::to_string(charPos()) + ")[Expected a token from " 
        + enumName[nt] + " but got " + enumName[currToken.kind() + NonTerminal::SIZE] + ".]";
    errBuf.push_back(msg);
    return msg;
}

void Parser::printErrorReport() {
    std::cout << "ERROR REPORT:" << std::endl;
    std::cout << "---------------------------------------------------" << std::endl;
    for (const std::string& msg : errBuf) {
        std::cout << msg << "\n";
    }
}

bool Parser::hasError() {
    return !errBuf.empty();
}

int Parser::lineNum() { return currToken.lineNumber(); }

int Parser::charPos() { return currToken.charPosition(); }

bool Parser::have(Token::Kind kind) {
    return currToken.kind() == kind;
}

bool Parser::have(NonTerminal nt) {
    return firstSet[nt][currToken.kind() / 8] & (0x1 << (currToken.kind() % 8));
}

bool Parser::accept(Token::Kind kind) {
    if (have(kind)) {
        currToken = scanner.next();
        return true;
    }
    return false;
}

bool Parser::accept(NonTerminal nt) {
    if (have(nt)) {
        currToken = scanner.next();
        return true;
    }
    return false;
}

bool Parser::expect(Token::Kind kind) {
    if (accept(kind)) {
        return true;
    }
    std::string msg = reportSyntaxError(kind);
    throw QuitParseException(msg);
}

bool Parser::expect(NonTerminal nt) {
    if (accept(nt)) {
        return true;
    }
    std::string msg = reportSyntaxError(nt);
    throw new QuitParseException(msg);
}

Token Parser::expectRetrieve(Token::Kind kind) {
    Token tok = currToken;
    if (accept(kind)) {
        return tok;
    }
    std::string msg = reportSyntaxError(kind);
    throw new QuitParseException(msg);
}

Token Parser::expectRetrieve(NonTerminal nt) {
    Token tok = currToken;
    if (accept(nt)) {
        return tok;
    }
    std::string msg = reportSyntaxError(nt);
    throw new QuitParseException(msg);
}

Parser::Parser(Scanner s): scanner(s), currToken(scanner.next()) {}

void Parser::printFirstSets() {
    for (int nt = 0; nt < NonTerminal::SIZE; nt++) {
        std::cout << "FIRST set for " << enumName[nt] << ": { ";
        for (int tok = 0; tok < Token::Kind::SIZE; tok++) {
            if (firstSet[nt][tok / 8] & (0x1 << (tok % 8))) {
                std::cout << enumName[tok + NonTerminal::SIZE] << " ";
            }
        }
        std::cout << "}" << std::endl;
    } 
}
