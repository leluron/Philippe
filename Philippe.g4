grammar Philippe;

chunk: block EOF;
block: stat*;
stat
  : assignment    #assignstat
  | functioncall  #funccallstat
  | while_stat    #whilestat
  | if_stat       #ifstat
  | for_stat      #forstat
  | '{' block '}' #blockstat
  | 'break'       #breakstat
  | 'return' exp? #returnstat
  ;

assignment
  : explist '=' exp                      #stdassign
  | exp ('+=' | '-=' | '*=' | '/=') exp  #compoundassign
  ;
functioncall: exp '(' explist? ')';
while_stat: 'while' exp stat;
if_stat: 'if' exp stat ('elseif' exp stat)* ('else' stat)?;
for_stat: 'for' ID 'in' exp stat;

exp
  : 'nil'                             #nilexp
  | 'true'                            #trueexp
  | 'false'                           #falseexp
  | intliteral                        #intexp
  | floatliteral                      #floatexp
  | STRING                            #stringexp
  | ID                                #idexp
  | objdef                            #objdefexp
  | listdef                           #listdefexp
  | rangedef                          #rangedefexp
  | tupledef                          #tupledefexp
  | funcdef                           #funcdefexp
  | exp '.' ID                        #memberexp
  | exp '[' exp ']'                   #indexexp
  | exp '(' explist? ')'              #funccallexp
  | '-' exp                           #unaryminusexp
  | 'not' exp                         #notexp
  | <assoc=right> exp '^' exp         #exponentexp
  | exp ('*' | '/' | '%') exp         #multiplicativeexp
  | exp ('+' | '-') exp               #additiveexp
  | <assoc=right> exp '..' exp        #strcatexp
  | exp ('<=' | '<' | '>' | '>=') exp #relationexp
  | exp ('==' | '!=' ) exp            #comparisonexp
  | exp 'and' exp                     #andexp
  | exp 'or' exp                      #orexp
  | exp 'if' exp 'else' exp           #ternaryexp
  | '(' exp ')'                       #parenexp
  ;


objdef: '{' fielddef* '}';
fielddef: ID '=' exp;
listdef: '[' explist? ']';
explist: exp (',' exp)*;
rangedef: '[' exp '..' exp ']';
tupledef: '(' tupleelements ')';
tupleelements: exp (',' exp)+;
funcdef: 'function' '(' arglist? ')' funcbody;
arglist: ID (',' ID)*;
funcbody: '{' block retstat? '}' | retstat?;
retstat: ('return')? exp;

intliteral : (INT | HEX);
floatliteral : FLOAT;

ID
    : [a-zA-Z_] [a-zA-Z_0-9]*
    ;

INT
    : [0-9]+
    ;

FLOAT
    : [0-9]+ '.' [0-9]* 
    | '.' [0-9]+ 
    ;

HEX
  : '0' [xX] [0-9a-fA-F]+;

STRING
  : '"' (~('"'))* '"'
  | '\'' (~('\''))* '\''
  ;

COMMENT
    : '//' ~[\r\n]* -> skip
    ;

SPACE
    : [ \t\r\n] -> skip
    ;

OTHER
    : . 
    ;