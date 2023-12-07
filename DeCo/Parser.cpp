#include "Parser.h"

// FIRST Sets, stored as bit maps
// Can store info about 8 tokens in 1 byte
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

// ERROR REPORTING ============================================================

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
    std::cout << "--------------------------------------------------------------------" << std::endl;
    for (const std::string& msg : errBuf) {
        std::cout << msg << "\n";
    }
}

bool Parser::hasError() {
    return !errBuf.empty();
}

int Parser::lineNum() { return currToken.lineNumber(); }

int Parser::charPos() { return currToken.charPosition(); }

// PARSING HELPERS ============================================================

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

// GRAMMAR RULES ============================================================

// program = [ declList ] "main" "(" ")" ":" "void" "{" [ statSeq ] "}"
void Parser::program() {
    // Check for declList
    if (have(NonTerminal::DECL_LIST)) {
        declList();
    }

    // Get symbol for main
    Token m = expectRetrieve(Token::Kind::MAIN);

    // TODO: Create symbol for main

    expect(Token::Kind::OPEN_PAREN);
    expect(Token::Kind::CLOSE_PAREN);
    expect(Token::Kind::COLON);
    expect(Token::Kind::VOID);
    expect(Token::Kind::OPEN_BRACE);

    // Check for statSeq
    if (have(NonTerminal::STAT_SEQ)) {
        statSeq();
    }

    expect(Token::Kind::CLOSE_BRACE);
}

// declList = ( varDecl | funcDecl ) { varDecl | funcDecl }
void Parser::declList() {
    do {
        if (have(NonTerminal::VAR_DECL)) {
            varDecl();
        } else {
            funcDecl();
        }
    } while (have(NonTerminal::VAR_DECL) || have(NonTerminal::FUNC_DECL));
}

// statSeq = statement { statement }
void Parser::statSeq() {
    do {
        statement();
    } while (have(NonTerminal::STATEMENT));
}

// varDecl = typeDecl ident { "," ident } ";"
void Parser::varDecl() {
    typeDecl();
    Token ident = expectRetrieve(Token::Kind::IDENT);

    // TODO: Create symbols for each declaration

    // Get as many idents as possible
    while (accept(Token::Kind::COMMA)) {
        ident = expectRetrieve(Token::Kind::IDENT);
    }

    expect(Token::Kind::SEMICOLON);
}

// funcDecl = "function" ident paramList ":" ( "void" | type ) funcBody
void Parser::funcDecl() {
    expect(Token::Kind::FUNC);
    Token ident = expectRetrieve(Token::Kind::IDENT);
    paramList();
    expect(Token::Kind::COLON);

    if (!accept(Token::Kind::VOID)) {
        type();
    }

    // TODO: Create symbol for function

    funcBody();
}

// paramList = "(" [ paramDecl { "," paramDecl } ] ")"
void Parser::paramList() {
    expect(Token::Kind::OPEN_PAREN);

    if (have(NonTerminal::PARAM_DECL)) {
        paramDecl();

        while (accept(Token::Kind::COMMA)) {
            paramDecl();
        }
    }

    expect(Token::Kind::CLOSE_PAREN);
}

// type = "bool" | "int" | "float"
void Parser::type() {
    Token t = expectRetrieve(NonTerminal::TYPE);
}

// funcBody = "{" [ statSeq ] "}"
void Parser::funcBody() {
    expect(Token::Kind::OPEN_BRACE);

    if (have(NonTerminal::STAT_SEQ)) {
        statSeq();
    }

    expect(Token::Kind::CLOSE_BRACE);
}

// paramDecl = paramType ident
void Parser::paramDecl() {
    paramType();
    Token ident = expectRetrieve(Token::Kind::IDENT);

    // TODO: Create symbol for parameter
}

// paramType = type { "[" "]" }
void Parser::paramType() {
    type();

    while (accept(Token::Kind::OPEN_BRACKET)) {
        expect(Token::Kind::CLOSE_BRACKET);
    }
}

// typeDecl = type { "[" integerLit "]" }
void Parser::typeDecl() {
    type();

    while (accept(Token::Kind::OPEN_BRACKET)) {
        Token intLit = expectRetrieve(Token::Kind::INT_VAL);
        expect(Token::Kind::CLOSE_BRACKET);
    }
}

// statement = varDecl | assignStat | funcCallStat | ifStat | whileStat 
//             | doWhileStat | forStat | repeatStat | returnStat
void Parser::statement() {
    if (have(NonTerminal::VAR_DECL)) {
        varDecl();
    } else if (have(NonTerminal::ASSIGN_STAT)) {
        assignStat();
    } else if (have(NonTerminal::FUNC_CALL_STAT)) {
        funcCallStat();
    } else if (have(NonTerminal::IF_STAT)) {
        ifStat();
    } else if (have(NonTerminal::WHILE_STAT)) {
        whileStat();
    } else if (have(NonTerminal::DO_WHILE_STAT)) {
        doWhileStat();
    } else if (have(NonTerminal::FOR_STAT)) {
        forStat();
    } else if (have(NonTerminal::REPEAT_STAT)) {
        repeatStat();
    } else if (have(NonTerminal::RETURN_STAT)) {
        returnStat();
    } else {
        expect(NonTerminal::STATEMENT);
    }
}

// assignStat = assign ";"
void Parser::assignStat() {
    assign();
    expect(Token::Kind::SEMICOLON);
}

// funcCallStat = funcCall ";"
void Parser::funcCallStat() {
    funcCall();
    expect(Token::Kind::SEMICOLON);
}

// ifStat = "if" relation "{" [ statSeq ] "}" [ "else" "{" [statSeq] "}" ]
void Parser::ifStat() {
    expect(Token::Kind::IF);
    relation();
    expect(Token::Kind::OPEN_BRACE);
    
    if (have(NonTerminal::STAT_SEQ)) {
        statSeq();
    }

    expect(Token::Kind::CLOSE_BRACE);

    // Check if there is an else
    if (accept(Token::Kind::ELSE)) {
        expect(Token::Kind::OPEN_BRACE);

        if (have(NonTerminal::STAT_SEQ)) {
            statSeq();
        }

        expect(Token::Kind::CLOSE_BRACE);
    }
}

// whileStat = "while" relation "{" [ statSeq ] "}"
void Parser::whileStat() {
    expect(Token::Kind::WHILE);
    relation();
    expect(Token::Kind::OPEN_BRACE);

    if (have(NonTerminal::STAT_SEQ)) {
        statSeq();
    }

    expect(Token::Kind::CLOSE_BRACE);
}

// doWhileStat = "do" "{" [ statSeq ] "}" "while" relation ";"
void Parser::doWhileStat() {
    expect(Token::Kind::DO);
    expect(Token::Kind::OPEN_BRACE);

    if (have(NonTerminal::STAT_SEQ)) {
        statSeq();
    }

    expect(Token::Kind::CLOSE_BRACE);
    expect(Token::Kind::WHILE);
    relation();
    expect(Token::Kind::SEMICOLON);
}

// forStat = "for" "(" [ assign ] ";" [ relExpr ] ";" [ assign ] ")" "{" [ statSeq ] "}"
void Parser::forStat() {
    expect(Token::Kind::FOR);
    expect(Token::Kind::OPEN_PAREN);

    // Check for initialization
    if (have(NonTerminal::ASSIGN)) {
        assign();
    }

    expect(Token::Kind::SEMICOLON);

    // Check for end condition
    if (have(NonTerminal::REL_EXPR)) {
        relExpr();
    }

    expect(Token::Kind::SEMICOLON);

    // Check for update
    if (have(NonTerminal::ASSIGN)) {
        assign();
    }

    expect(Token::Kind::CLOSE_PAREN);
    expect(Token::Kind::OPEN_BRACE);

    if (have(NonTerminal::STAT_SEQ)) {
        statSeq();
    }

    expect(Token::Kind::CLOSE_BRACE);
}

// repeatStat = "repeat" "{" [ statSeq ] "}" "until" relation ";"
void Parser::repeatStat() {
    expect(Token::Kind::REPEAT);
    expect(Token::Kind::OPEN_BRACE);

    if (have(NonTerminal::STAT_SEQ)) {
        statSeq();
    }

    expect(Token::Kind::CLOSE_BRACE);
    expect(Token::Kind::UNTIL);
    relation();
    expect(Token::Kind::SEMICOLON);
}

// returnStat = "return" [ relExpr ] ";"
void Parser::returnStat() {
    expect(Token::Kind::RETURN);

    if (have(NonTerminal::REL_EXPR)) {
        relExpr();
    }

    expect(Token::Kind::SEMICOLON);
}

// relExpr = addExpr { relOp addExpr }
void Parser::relExpr() {
    addExpr();

    while (have(NonTerminal::REL_OP)) {
        Token op = expectRetrieve(NonTerminal::REL_OP);
        addExpr();
    }
}

// addExpr = multExpr { addOp multExpr }
void Parser::addExpr() {
    mulExpr();

    // May need to check for subtraction read as INT_VAL (4-3 as INT_VAL INT_VAL)
    while (have(NonTerminal::ADD_OP) || have(Token::Kind::INT_VAL) || have(Token::Kind::FLOAT_VAL)) {
        if (have(NonTerminal::ADD_OP)) {
            Token op = expectRetrieve(NonTerminal::ADD_OP);
            mulExpr();
        } else {
            Token val = currToken;

            // Check if it is a negative number
            if (val.lexeme().front() == '-') {
                // Interpret as subtraction
                Token op("-", val.lineNumber(), val.charPosition(), Token::Kind::SUB);

                // Use rest of number as next token
                Token::Kind k = val.kind() == Token::Kind::INT_VAL ? Token::Kind::INT_VAL : Token::Kind::FLOAT_VAL;
                currToken = Token(val.lexeme().substr(1), val.lineNumber(), val.charPosition() + 1, k);
            }

            mulExpr();
        }
    }
}

// multExpr = powExpr { multOp powExpr }
void Parser::mulExpr() {
    powExpr();

    while (have(NonTerminal::MUL_OP)) {
        Token op = expectRetrieve(NonTerminal::MUL_OP);
        powExpr();
    }
}

// powExpr = groupExpr { powOp groupExpr }
void Parser::powExpr() {
    groupExpr();

    while (accept(Token::Kind::POW)) {
        groupExpr();
    }
}

// groupExpr = literal | designator | "!" relExpr | relation | funcCall
void Parser::groupExpr() {
    if (have(NonTerminal::LITERAL)) {
        Token literal = expectRetrieve(NonTerminal::LITERAL);
    } else if (have(NonTerminal::DESIGNATOR)) {
        designator();
    } else if (accept(Token::Kind::NOT)) {
        relExpr();
    } else if (have(NonTerminal::RELATION)) {
        relation();
    } else if (have(NonTerminal::FUNC_CALL)) {
        funcCall();
    } else {
        expect(NonTerminal::GROUP_EXPR);
    }
}

// relation = "(" relExpr ")"
void Parser::relation() {
    expect(Token::Kind::OPEN_PAREN);
    relExpr();
    expect(Token::Kind::CLOSE_PAREN);
}

// assign = designator ( ( assignOp relExpr ) | unaryOp )
void Parser::assign() {
    designator();

    if (have(NonTerminal::ASSIGN_OP)) {
        Token op = expectRetrieve(NonTerminal::ASSIGN_OP);
        relExpr();
    } else if (have(NonTerminal::UNARY_OP)) {
        Token op = expectRetrieve(NonTerminal::UNARY_OP);
    } else {
        expect(NonTerminal::ASSIGN);
    }
}

// funcCall = "call" ident "(" [ relExpr { "," relExpr } ] ")"
void Parser::funcCall() {
    expect(Token::Kind::CALL);
    Token ident = expectRetrieve(Token::Kind::IDENT);
    expect(Token::Kind::OPEN_PAREN);

    // Check for parameters
    if (have(NonTerminal::REL_EXPR)) {
        relExpr();

        while (accept(Token::Kind::COMMA)) {
            relExpr();
        }
    }

    expect(Token::Kind::CLOSE_PAREN);
}

// designator = ident { "[" relExpr "]" }
void Parser::designator() {
    Token ident = expectRetrieve(Token::Kind::IDENT);

    while (accept(Token::Kind::OPEN_BRACKET)) {
        relExpr();
        expect(Token::Kind::CLOSE_BRACKET);
    }
}

// CONSTRUCTOR ============================================================

Parser::Parser(Scanner s): scanner(s), currToken(scanner.next()) {}

// RUN THE PARSER ============================================================

void Parser::parse() {
    try {
        program();
    } catch (QuitParseException e) {
        std::cout << e.what() << std::endl;
    }
}

// DEBUGGING HELPERS ============================================================

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
