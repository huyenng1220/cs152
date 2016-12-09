grammar FeatherweightJavaScript;


@header { package edu.sjsu.fwjs.parser; }

// Reserved words
IF        : 'if' ;
ELSE      : 'else' ;
WHILE     : 'while';
FUNCTION  : 'function';
VAR       : 'var';
PRINT     : 'print';

// Literals
INT     : [1-9][0-9]* | '0' ;
BOOL    : 'true' | 'false';
NULL    : 'null';

// Symbols
MUL       : '*' ;
DIV       : '/' ;
ADD       : '+';
SUB       : '-';
MOD       : '%';
GT        : '>';
GE        : '>=';
LT        : '<';
LE        : '<=';
EQ        : '==';
SEPARATOR : ';' ;

// Identifiers
ID      : [a-zA-Z][a-zA-Z0-9_]*;

// Whitespace and comments
NEWLINE       : '\r'? '\n' -> skip ;
BLOCK_COMMENT : '/*'.*? '*/' -> skip;
LINE_COMMENT  : '//' ~[\n\r]* -> skip ;
WS            : [ \t]+ -> skip ; // ignore whitespace


// ***Paring rules ***

/** The start rule */
prog: stat+ ;

stat: expr SEPARATOR                                    # bareExpr
    | IF '(' expr ')' block ELSE block                  # ifThenElse
    | IF '(' expr ')' block                             # ifThen
    | WHILE '(' expr ')' block                          # while
    | PRINT '(' expr ')' SEPARATOR                      # print
    | SEPARATOR                                         # empty
    ;

expr: expr args                                         # functionApp
    | FUNCTION params '{' stat* '}'                     # functionDec
    | expr op=( '*' | '/' | '%' ) expr                  # mulDivMod
    | expr op=( '+' | '-' ) expr                        # addSub
    | expr op=( '<' | '<=' | '>' | '>=' | '==' ) expr   # compare
    | VAR ID '=' expr                                   # varDec
    | ID                                                # varRef
    | ID '=' expr                                       # varAssign
    | INT                                               # int
    | BOOL                                              # boolean
    | NULL                                              # null
    | '(' expr ')'                                      # parens
    ;

params: '(' ID (',' ID)* ')'                            # funcParams
    | '(' ')'                                           # funcNoParams
    ;

args: '(' expr (',' expr)* ')'                          # funcArgs
    | '(' ')'                                           # funcNoArgs
    ;

block: '{' stat* '}'                                    # fullBlock
    | stat                                              # simpBlock
    ;
