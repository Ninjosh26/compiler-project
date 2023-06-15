# The Micro Programming Language
This compiler is defined for the "Micro" programming language, which has the following description:
- The only data type is integer
- All identifiers are implicitly declared and are no longer than 32 characters. Identifiers must begin with a letter and are composed of letters, digits, and underscores.
- Literals are strings of digits.
- Comments being with -- and end at the end of the current line.
- Statement types are
Assignment:
&ensp;&ensp;&ensp;&ensp;`ID := Expression;`
&ensp;&ensp;&ensp;&ensp;Expression is an infix expression constructed from identifies, literals, and the operators + and -; parentheses are also allowed.

&ensp;&ensp;&ensp;&ensp;Input/Output:
&ensp;&ensp;&ensp;&ensp;&ensp;&ensp;&ensp;&ensp;**read** (list of IDs);
&ensp;&ensp;&ensp;&ensp;&ensp;&ensp;&ensp;&ensp;**write** (List of Expressions);

- **begin**, **end**, **read**, and **write** are reserved words.
- Each statement is terminated by a semicolon (`;`). The body of a program is delimited by **begin** and **end**.
- A blank is appended to the right end of each source line; thus tokens may not extend across line boundaries.

This is a simple compiler. It is a one-pass type, where no explict intermediate respresentations are used.

## Components
- The *Scanner* reads a source program from a text file and produces a stream of token representations. It is a function that produces token representations one at a time when called by the parser.
- The *Parser* processes tokens until it recognizes a syntactic structure that requires semantic processing. It then directly calls a semantic routine. Some semantic routines use token representation information in their processing.
- The *Semantic Routines* produce output in assembly language for a simple three-address virtual machin. The compiler structure includes no optimizer, and code generation is done by direct calls to appropriate support routines from the semantic routines.
- The *Symbol Table* is used only by the semantic routines. 

## Syntax
The syntax for Micro will be defined using an extended Backus-Naur form (BNF).

1. &lt;program> &ensp;&ensp;&ensp;&ensp;&ensp;&ensp;&ensp;-> **begin** &lt;statement list> **end**
2. &lt;statement list>&ensp;&ensp;&ensp;-> &lt;statement> { &lt;statement> }
3. &lt;statement>&ensp;&ensp;&ensp;&ensp;&ensp;&ensp;-> ID := &lt;expression> ;
4. &lt;statement>&ensp;&ensp;&ensp;&ensp;&ensp;&ensp;-> **read** ( &lt;id list> ) ;
5. &lt;statement>&ensp;&ensp;&ensp;&ensp;&ensp;&ensp;-> **write** ( &lt;expr list> ) ;
6. &lt;id list>&ensp;&ensp;&ensp;&ensp;&ensp;&ensp;&ensp;&ensp;&ensp;&ensp;-> ID { , ID}
7. &lt;expr list>&ensp;&ensp;&ensp;&ensp;&ensp;&ensp;&ensp;&ensp;-> &lt;expression> { , &lt;expression> }
8. &lt;expression>&ensp;&ensp;&ensp;&ensp;&ensp;-> &lt;primary> { &lt;add op> &lt;primary> }
9. &lt;primary>&ensp;&ensp;&ensp;&ensp;&ensp;&ensp;&ensp;&ensp;-> ( &lt;expression> )
10. &lt;primary>&ensp;&ensp;&ensp;&ensp;&ensp;&ensp;&ensp;&ensp;-> ID
11. &lt;primary>&ensp;&ensp;&ensp;&ensp;&ensp;&ensp;&ensp;&ensp;-> INTLITERAL
12. &lt;add op>&ensp;&ensp;&ensp;&ensp;&ensp;&ensp;&ensp;&ensp; -> PLUSOP
13. &lt;add op>&ensp;&ensp;&ensp;&ensp;&ensp;&ensp;&ensp;&ensp; -> MINUSOP
14. &lt;system goal>&ensp;&ensp;&ensp;&ensp;-> &lt;program> SCANEOF

## Target Language
The target language for this compiler is rather simple. It is an assembly language for a three-address machine. The instructions have the following form:

`
OP A,B,C
`

in which OP is an op-code (or pseudo-op), A and B are operands, and C is the result's destination. The operands may be variable names or integer literals. For Micro, all arithmetic operations are assumed to be done on integers.