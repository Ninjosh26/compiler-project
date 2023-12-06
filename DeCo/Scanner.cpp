#include <ctype.h>
#include <stdexcept>
#include "Scanner.h"

using Kind = Token::Kind;

Token::Token(int lineNum, int charPos) {
    _lineNum = lineNum;
    _charPos = charPos;

    // No lexeme provided, signal error
    _kind = Kind::ERROR;
    _lexeme = "No lexeme given";
}

Token::Token(std::string lexeme, int lineNum, int charPos, Kind kind) {
    _lineNum = lineNum;
    _charPos = charPos;
    _lexeme = lexeme;
    _kind = kind;
}

Scanner::Scanner(FILE* in) {
    input = in;
    closed = false;
    lineNum = 1;
    charPos = 0;
    lex = "";
    nextChar = fgetc(input);
}

void Scanner::Error(const char* msg) {
    printf("Scanner: Line - %d, Char - %d\n%s\n", lineNum, charPos, msg);
}

int Scanner::readChar() {
    int curr = nextChar;

    if (curr == '\n') {
        lineNum++;
        charPos = 0;
    } else {
        charPos++;
    }

    nextChar = fgetc(input);
    return curr;
}

void Scanner::resetBuf() {
    lex.clear();
}

void Scanner::bufferChar(int c) {
    // Check if identifier is too long
    if (lex.size() == MAX_IDENT_LENGTH) {
        // Throw lexical error
    }

    lex.push_back(c);
}

Token Scanner::makeToken(std::string lexeme, Kind kind) {
    return Token(lexeme, lineNum, charPos - (lexeme.size() - 1), kind);
}

Token Scanner::getNumber() {
    bool isFloat = false;

    while (isdigit(nextChar)) {
        bufferChar(readChar());
    }

    // Could be float
    if (nextChar == '.') {
        isFloat = true;
        bufferChar('.');
        while (isdigit(nextChar)) {
            bufferChar(readChar());
        }
    }

    // Check if next symbol is invalid, or no number following decimal
    if (!nextCharValid() || lex.back() == '.') {
        return getError();
    }

    // Otherwise we are good to return the token as is
    return makeToken(lex, isFloat ? Kind::FLOAT_VAL : Kind::INT_VAL);
}

Token Scanner::getIdentOrKeyword() {
    // Start building lexeme until identifier rule is violated
    while (isalnum(nextChar) || nextChar == '_') {
        lex.push_back(readChar());
    }

    // Check if next symbol is not start of valid token (or whitespace)
    if (nextCharValid()) {
        // Check if it is a keyword
        if (lex == "void") {
            return makeToken("void", Kind::VOID);
        } else if (lex == "bool") {
            return makeToken("bool", Kind::BOOL);
        } else if (lex == "int") {
            return makeToken("int", Kind::INT);
        } else if (lex == "float") {
            return makeToken("float", Kind::FLOAT);
        } else if (lex == "true") {
            return makeToken("true", Kind::TRUE);
        } else if (lex == "false") {
            return makeToken("false", Kind::FALSE);
        } else if (lex == "if") {
            return makeToken("if", Kind::IF);
        } else if (lex == "else") {
            return makeToken("else", Kind::ELSE);
        } else if (lex == "while") {
            return makeToken("while", Kind::WHILE);
        } else if (lex == "do") {
            return makeToken("do", Kind::DO);
        } else if (lex == "for") {
            return makeToken("for", Kind::FOR);
        } else if (lex == "repeat") {
            return makeToken("repeat", Kind::REPEAT);
        } else if (lex == "until") {
            return makeToken("until", Kind::UNTIL);
        } else if (lex == "call") {
            return makeToken("call", Kind::CALL);
        } else if (lex == "return") {
            return makeToken("return", Kind::RETURN);
        } else if (lex == "main") {
            return makeToken("main", Kind::MAIN);
        } else if (lex == "function") {
            return makeToken("function", Kind::FUNC);
        }

        // Otherwise, it is a valid ident
        return makeToken(lex, Kind::IDENT);
    }

    // Otherwise, stray token poisons all consecutive characters
    return getError();
}

bool Scanner::nextCharValid() {
    switch (nextChar) {
        case '&':
        case '|':
        case '!':
        case '+':
        case '-':
        case '*':
        case '/':
        case '%':
        case '^':
        case '=':
        case '<':
        case '>':
        case '(':
        case ')':
        case '{':
        case '}':
        case '[':
        case ']':
        case ',':
        case ':':
        case ';':
            return true;
        default:
            return isspace(nextChar) || isalnum(nextChar) || nextChar == EOF;
    }
}

Token Scanner::getError() {
    while (!isspace(nextChar) && nextChar != EOF) {
        bufferChar(readChar());
    }
    return makeToken(lex, Kind::ERROR);
}

bool Scanner::hasNext() {
    return !closed || (nextChar != EOF);
}

Token Scanner::next() {
    if (!hasNext()) {
        throw std::runtime_error("No next element to scan");
    }

    int inChar;
    Kind k;
    resetBuf();

    // Check if end of file has been reached
    if (nextChar == EOF) {
        closed = true;
        return makeToken("", Kind::SCAN_EOF);
    }

    // Try to resolve the token
    while (nextChar != EOF) {
        inChar = readChar();
        
        // Skip whitespace
        if (isspace(inChar)) {
            continue;
        }
        // Could be keyword or identifier
        else if (isalpha(inChar)) {
            lex.push_back(inChar);
            return getIdentOrKeyword();
        }
        // Could be literal (int or float)
        else if (isdigit(inChar)) {
            lex.push_back(inChar);
            return getNumber();
        } 
        // Known symbols
        else {
            switch (inChar) {
                case '(':
                    return makeToken("(", Kind::OPEN_PAREN);
                case ')':
                    return makeToken(")", Kind::CLOSE_PAREN);
                case '{':
                    return makeToken("{", Kind::OPEN_BRACE);
                case '}':
                    return makeToken("}", Kind::CLOSE_BRACE);
                case '[':
                    return makeToken("[", Kind::OPEN_BRACKET);
                case ']':
                    return makeToken("]", Kind::CLOSE_BRACKET);
                case ',':
                    return makeToken(",", Kind::COMMA);
                case ':':
                    return makeToken(":", Kind::COLON);
                case ';':
                    return makeToken(";", Kind::SEMICOLON); 

                // Could be comparison, arithmetic or assignment
                case '=':
                    if (nextChar == '=') {
                        readChar();
                        return makeToken("==", Kind::EQUAL_TO);
                    }
                    return makeToken("=", Kind::ASSIGN);
                case '!':
                    if (nextChar == '!') {
                        readChar();
                        return makeToken("!=", Kind::NOT_EQUAL);
                    }
                    return makeToken("!", Kind::NOT);
                case '<':
                    if (nextChar == '=') {
                        readChar();
                        return makeToken("<=", Kind::LESS_EQUAL);
                    }
                    return makeToken("<", Kind::LESS_THAN);
                case '>':
                    if (nextChar == '>') {
                        readChar();
                        return makeToken(">=", Kind::GREATER_EQUAL);
                    }
                    return makeToken(">", Kind::GREATER_THAN);
                case '+':
                    if (nextChar == '=') {
                        readChar();
                        return makeToken("+=", Kind::ADD_ASSIGN);
                    } else if (nextChar == '+') {
                        readChar();
                        return makeToken("++", Kind::UNI_INC);
                    }
                    return makeToken("+", Kind::ADD);
                case '-':
                    if (nextChar == '=') {
                        readChar();
                        return makeToken("-=", Kind::SUB_ASSIGN);
                    } else if (nextChar == '-') {
                        readChar();
                        return makeToken("--", Kind::UNI_DEC);
                    } 
                    // Could be a negative number 
                    if (isdigit(nextChar)) {
                        bufferChar(inChar);
                        bufferChar(readChar());
                        return getNumber();
                    }
                    return makeToken("-", Kind::SUB);
                case '*':
                    if (nextChar == '=') {
                        readChar();
                        return makeToken("*=", Kind::MUL_ASSIGN);
                    }
                    return makeToken("*", Kind::MUL);
                case '/':
                    if (nextChar == '=') {
                        readChar();
                        return makeToken("/=", Kind::DIV_ASSIGN);
                    }
                    // Could be a comment
                    if (inChar == '/') {
                        // Single-line comment
                        if (nextChar == '/') {
                            // Skip until end of line or file
                            inChar = readChar();
                            while (inChar != EOF && inChar != '\n') {
                                inChar = readChar();
                            }
                            continue;
                        }
                        // Block comment
                        if (nextChar == '*') {
                            int origLine = lineNum;
                            int origChar = charPos;
                            readChar();
                            inChar = readChar();

                            // Skip until closing "*/" or end of file
                            while (nextChar != EOF) {
                                if (inChar == '*' && nextChar == '/') {
                                    readChar();
                                    break;
                                }
                                inChar = readChar();
                            }

                            if (nextChar == EOF) {
                                return Token("Missing closing */", origLine, origChar, Kind::ERROR);
                            }

                            continue;
                        }

                        // Must be DIV
                        return makeToken("/", Kind::DIV);
                    }
                case '%':
                    if (nextChar == '%') {
                        readChar();
                        return makeToken("%=", Kind::MOD_ASSIGN);
                    }
                    return makeToken("%", Kind::MOD);
                case '^':
                    if (nextChar == '^') {
                        readChar();
                        return makeToken("^=", Kind::DIV_ASSIGN);
                    }
                    return makeToken("^", Kind::POW);
                case '&':
                    if (nextChar == '&') {
                        readChar();
                        return makeToken("&&", Kind::AND);
                    }
                    return getError();
                case '|':
                    if (nextChar == '|') {
                        readChar();
                        return makeToken("||", Kind::OR);
                    }
                    return getError();
                default:
                    return getError();
            }
        }
    }

    return makeToken("", Kind::SCAN_EOF);
}

std::ostream& operator<<(std::ostream& os, const Token& tok) {
    os << "Line: " << tok.lineNumber() << ", Char: " << tok.charPosition() << ", Lexeme: " << tok.lexeme() << ", Kind: ";

    switch (tok.kind()) {
        case Kind::AND:
            os << "AND";
            break;
        case Kind::OR: 
            os << "OR";
            break;
        case Kind::NOT:
            os << "NOT";
            break;
        case Kind::ADD:
            os << "ADD";
            break;
        case Kind::SUB:
            os << "SUB";
            break;
        case Kind::MUL:
            os << "MUL";
            break;
        case Kind::DIV:
            os << "DIV";
            break;
        case Kind::MOD:
            os << "MOD";
            break;
        case Kind::POW:
            os << "POW";
            break;
        case Kind::EQUAL_TO:
            os << "EQUAL_TO";
            break;
        case Kind::NOT_EQUAL:
            os << "NOT_EQUAL";
            break;
        case Kind::LESS_THAN:
            os << "LESS_EQUAL";
            break;
        case Kind::LESS_EQUAL:
            os << "LESS_EQUAL";
            break;
        case Kind::GREATER_THAN:
            os << "GREATER_THAN";
            break;
        case Kind::GREATER_EQUAL:
            os << "GREATER_EQUAL";
            break;
        case Kind::ASSIGN:
            os << "ASSIGN";
            break;
        case Kind::ADD_ASSIGN:
            os << "ADD_ASSIGN";
            break;
        case Kind::SUB_ASSIGN:
            os << "SUB_ASSIGN";
            break;
        case Kind::MUL_ASSIGN:
            os << "MUL_ASSIGN";
            break;
        case Kind::DIV_ASSIGN:
            os << "DIV_ASSIGN";
            break;
        case Kind::MOD_ASSIGN:
            os << "MOD_ASSIGN";
            break;
        case Kind::POW_ASSIGN:
            os << "POW_ASSIGN";
            break;
        case Kind::UNI_INC:
            os << "UNI_INC";
            break;
        case Kind::UNI_DEC:
            os << "UNI_DEC";
            break;
        case Kind::VOID:
            os << "VOID";
            break;
        case Kind::BOOL:
            os << "BOOL";
            break;
        case Kind::INT:
            os << "INT";
            break;
        case Kind::FLOAT:
            os << "FLOAT";
            break;
        case Kind::TRUE:
            os << "TRUE";
            break;
        case Kind::FALSE:
            os << "FALSE";
            break;
        case Kind::OPEN_PAREN:
            os << "OPEN_PAREN";
            break;
        case Kind::CLOSE_PAREN:
            os << "CLOSE_PAREN";
            break;
        case Kind::OPEN_BRACE:
            os << "OPEN_BRACE";
            break;
        case Kind::CLOSE_BRACE:
            os << "CLOSE_BRACE";
            break;
        case Kind::OPEN_BRACKET:
            os << "OPEN_BRACKET";
            break;
        case Kind::CLOSE_BRACKET:
            os << "CLOSE_BRACKET";
            break;
        case Kind::COMMA:
            os << "COMMA";
            break;
        case Kind::COLON:
            os << "COLON";
            break;
        case Kind::SEMICOLON:
            os << "SEMICOLON";
            break;
        case Kind::IF:
            os << "IF";
            break;
        case Kind::ELSE:
            os << "ELSE";
            break;
        case Kind::WHILE:
            os << "WHILE";
            break;
        case Kind::DO:
            os << "DO";
            break;
        case Kind::FOR:
            os << "FOR";
            break;
        case Kind::REPEAT:
            os << "REPEAT";
            break;
        case Kind::UNTIL:
            os << "UNTIL";
            break;
        case Kind::CALL:
            os << "CALL";
            break;
        case Kind::RETURN:
            os << "RETURN";
            break;
        case Kind::MAIN:
            os << "MAIN";
            break;
        case Kind::FUNC:
            os << "FUNC";
            break;
        case Kind::INT_VAL:
            os << "INT_VAL";
            break;
        case Kind::FLOAT_VAL:
            os << "FLOAT_VAL";
            break;
        case Kind::IDENT:
            os << "IDENT";
            break;
        case Kind::SCAN_EOF:
            os << "SCAN_EOF";
            break;
        case Kind::ERROR:
            os << "ERROR";
            break;
    }

    return os;
}