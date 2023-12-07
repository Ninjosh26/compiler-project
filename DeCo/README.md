# DeCo Compiler

## Grammar
This is the grammar description for the DeCo grammar. This is written is Wirth Syntax Notation.

- Terminals are in quotes, e.g., "true", "int"
- Nonterminals are in regular text, and appear on the left-hand side
- (Nonterminal =) is a production rule
- Each production rule ends with a period
- Square brackets ( [ ] ) means at most 1 ( the ? in regex )
- Curly brackets ( { } ) means 0 or more ( the * in regex )

lowercase = "a" | "b" | ... | "z" . <br>
uppercase = "A" | "B" | ... | "Z" . <br>
digit = "0" | "1" | ... | "9" .

powOp = "^" . <br>
mulOp = "\*" | "/" | "%" | "&&" . <br>
addOp = "+" | "-" | "||" . <br>
relOp = "==" | "!=" | "<" | "<=" | ">" | ">=" . <br>
assignOp = "=" | "+=" | "-=" | "\*=" | "/=" | "%=" | "^=" . <br>
unaryOp = "++" | "--" .

type = "bool" | "int" | "float" .

boolLit = "true" | "false" . <br>
integerLit = [ "-" ] digit { digit } . <br>
floatLit = integerLit "." digit { digit } . <br>
literal = boolLit | integerLit | floatLit .

letter = lowercase | uppercase . <br>
ident = letter { "_" | letter | digit } .

designator = ident { "[" relExpr "]" } . <br>
groupExpr = literal | designator | "!" relExpr | relation | funcCall . <br>
powExpr = groupExpr { powOp groupExpr } . <br>
mulExpr = powExpr { mulOp powExpr } . <br>
addExpr = mulExpr { addOp mulExpr } . <br>
relExpr = addExpr { relOp addExpr } .

relation = "(" relExpr ")" .

assign = designator ( ( assignOp relExpr ) | unaryOp ) . <br>
funcCall = "call" ident "(" [ relExpr { "," relExpr } ] ")" . <br>
assignStat = assign ";" . <br>
funcCallStat = funcCall ";" . <br>
ifStat = "if" relation "{" [ statSeq ] "}" [ "else" "{" [ statSeq ] "}" ] . <br>
whileStat = "while" relation "{" [ statSeq ] "}" . <br>
doWhileStat = "do" "{" [ statSeq ] "}" "while" relation ";" . <br>
forStat = "for" "(" [ assign ] ";" [ relExpr ] ";" [ assign ] ")" "{" [ statSeq ] "}" . <br>
repeatStat = "repeat" "{" [ statSeq ] "}" "until" relation ";" . <br>
returnStat = "return" [ relExpr ] ";".

statement = varDecl | assignStat | funcCallStat | ifStat | whileStat | doWhileStat | forStat | repeatStat | returnStat . <br>
statSeq = statement { statement } .

typeDecl = type { "[" integerLit "]" } . <br>
varDecl = typeDecl ident { "," ident } ";" . <br>
paramType = type { "[" "]" } . <br>
paramDecl = paramType ident . <br>
paramList = "(" [ paramDecl { "," paramDecl } ] ")" . <br>
funcBody = "{" [ statSeq ] "}" . <br>
funcDecl = "function" ident paramList ":" ( "void" | type ) funcBody .

declList = ( varDecl | funcDecl ) { varDecl | funcDecl } .

program = [ declList ] "main" "(" ")" ":" "void" "{" [ statSeq ] "}" .

## 1. Lexical Analysis
In this step, the text file is read through, and tokens are constructed based on terminals in the grammar. The scanner (or lexer) builds these tokens based on simplified maximal munch, which attempts to get the longest match possible.

### List of Tokens
Boolean Operators
- AND => `&&`
- OR => `||`
- NOT => `!`

Arithmetic Operators
- ADD => `+`
- SUB => `-`
- MUL => `*`
- DIV => `/`
- MOD => `%`
- POW => `^`

Relational Operators
- EQUAL_TO => `==`
- NOT_EQUAL => `!=`
- LESS_THAN => `<`
- LESS_EQUAL => `<=`
- GREATER_THAN => `>`
- GREATER_EQUAL => `>=`

Assignment Operators
- ASSIGN => `=`
- ADD_ASSIGN => `+=`
- SUB_ASSIGN => `-=`
- MUL_ASSIGN => `*=`
- DIV_ASSIGN => `/=`
- MOD_ASSIGN => `%=`
- POW_ASSIGN => `^=`

Unary Increment/Decrement
- UNI_INC => `++`
- UNI_DEC => `--`

Primitive Types
- VOID => `void`
- BOOL => `bool`
- INT => `int`
- FLOAT => `float`

Boolean Literals
- TRUE => `true`
- FALSE => `false`

Region Delimiters
- OPEN_PAREN => `(`
- CLOSE_PAREN => `)`
- OPEN_BRACE => `{`
- CLOSE_BRACE => `}`
- OPEN_BRACKET => `[`
- CLOSE_BRACKET => `]`

Field/Record Delimiters
- COMMA => `,`
- COLON => `:`
- SEMICOLON => `;`

Control Flow Statements
- IF => `if`
- ELSE => `else`
- WHILE => `while`
- DO => `do`
- FOR => `for`
- REPEAT => `repeat`
- UNTIL => `until`
- CALL => `call`
- RETURN => `return`

Keywords
- MAIN => `main`
- FUNC => `function`

Special Cases - These are defined with regex, or have no lexeme
- INT_VAL => `^-?[0-9]+$`
- FLOAT_VAL => `^-?[0-9]+.[0-9]+$`
- IDENT => `^[a-z][_|[a-z]|[0-9]]*$`
- SCAN_EOF
- ERROR