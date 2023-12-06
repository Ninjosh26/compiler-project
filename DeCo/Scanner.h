#ifndef _SCANNER_H_
#define _SCANNER_H_

#include <iostream>
#include <stdio.h>
#include <string>

// Predefined token types
class Token {
public:

    enum Kind {
        // Boolean operators
        AND, OR, NOT,

        // Arithmetic operators
        ADD, SUB, MUL, DIV, MOD, POW,

        // Relational operators
        EQUAL_TO, NOT_EQUAL, LESS_THAN, LESS_EQUAL, GREATER_THAN, 
        GREATER_EQUAL,

        // Assignment operators
        ASSIGN, ADD_ASSIGN, SUB_ASSIGN, MUL_ASSIGN, DIV_ASSIGN, MOD_ASSIGN, 
        POW_ASSIGN,

        // Unary increment/decrement
        UNI_INC, UNI_DEC,

        // Primitive types
        VOID, BOOL, INT, FLOAT,

        // Boolean literals
        TRUE, FALSE,

        // Region delimiters
        OPEN_PAREN, CLOSE_PAREN, OPEN_BRACE, CLOSE_BRACE, OPEN_BRACKET,
        CLOSE_BRACKET,

        // Field/record delimiters
        COMMA, COLON, SEMICOLON,

        // Control flow statements
        IF, ELSE, WHILE, DO, FOR, REPEAT, UNTIL, CALL, RETURN,

        // Keywords
        MAIN, FUNC,

        // Special cases
        INT_VAL, FLOAT_VAL, IDENT, SCAN_EOF, ERROR,

        // Used for getting size of enum
        SIZE,
    };

private:

    int _lineNum;
    int _charPos;
    Kind _kind;
    std::string _lexeme;

public:
    
    int lineNumber() const { return _lineNum; }
    int charPosition() const { return _charPos; }
    Kind kind() const { return _kind; }
    const std::string& lexeme() const { return _lexeme; }
    bool is(Kind kind) const { return this->_kind == kind; }

    Token(int lineNum, int charPos);
    Token(std::string lexeme, int lineNum, int charPos, Kind kind);
};

// Printer for tokens, good for debugging
std::ostream& operator<<(std::ostream& os, const Token& tok);

const int MAX_IDENT_LENGTH = 32;

Token check_reserved();

// Reads in a token from stdin
Token scanner();

// Throws an error
void lexical_error(int c);

class Scanner {
private:

    FILE* input;            // Stream that file is stored in
    bool closed;                    // Flag for whether input is closed or not

    int lineNum;                    // Current line number
    int charPos;                    // Character offset on current line

    std::string lex;                // Current lexeme being scanned in 
    int nextChar;                   // Contains the next char (-1 == EOF)

    // Read in a single char from input
    int readChar();

    // Reset buffer
    void resetBuf();

    // Add to buffer
    void bufferChar(int c);

    // Makes a token with the lexeme and kind
    Token makeToken(std::string lexeme, Token::Kind kind);

    // Mark all consecutive characters as an error
    Token getError();

    // Extract a number if possible
    Token getNumber();

    // Extract a keyword or num if possible
    Token getIdentOrKeyword();

    // Checks if next character could be in a valid token
    bool nextCharValid();

public:
    // Scan tokens from a given file
    Scanner(FILE* in = stdin);

    // Print scanning error
    void Error(const char* msg = "");

    // Query whether more characters can be read
    bool hasNext();

    // Get next token from file
    Token next();
};

#endif