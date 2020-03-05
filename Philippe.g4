grammar Philippe;

file: def* EOF;

def
  : ID (':' type)? '=' exp #globaldef
  | ID '=' 'function' ('(' (arg (',' arg)*)? ')')? ('->' type)? '{' stat* '}' #functiondef
  | 'type' ID '=' type #aliasdef
  | 'type' ID '=' '{' arg+ '}' #objdef
  ;

stat
  : lexpopttype (',' lexpopttype)* '=' exp                 #stdassign
  | lexpopttype op=('+=' | '-=' | '*=' | '/=') exp  #compoundassign
  | lexp '(' explist? ')'  #funccall
  | 'while' exp stat    #whilestat
  | 'if' exp stat ('elseif' exp stat)* ('else' els=stat)?       #ifstat
  | 'for' ID 'in' forexp stat      #forstat
  | '{' stat* '}' #blockstat
  | 'break'       #breakstat
  | 'return' exp? #returnstat
  ;

forexp
  : exp
  | '[' exp '..' exp ']'
  ;

arg: ID ':' type;
type
  : compositetype
  | basictype
  ;
basictype
  : t=('int'
  | 'float'
  | 'bool'
  | 'string'
  | 'nil') #primitivetype
  | '(' typeaux (',' typeaux)+ ')' #tupletype
  | ID #objaliastype
  ;
compositetype
  : 'function' typeaux* ('->' ret=typeaux)? #functype
  | 'list ' typeaux #listtype
  ;
typeaux
  : basictype
  | '(' compositetype ')'
  ;


lexp : ID lexpsuffix*;
lexpopttype : lexp (':' type)?;

lexpsuffix
  : '[' exp ']'
  | '.' ID
  ;

exp
  : 'nil'                             #nilexp
  | 'true'                            #trueexp
  | 'false'                           #falseexp
  | (INT | HEX)                       #intexp
  | FLOAT                             #floatexp
  | STRING                            #stringexp
  | ID '{' fielddef+ '}'              #objexp
  | ID                                #idexp
  | '[' explist? ']'                  #listexp              
  | '(' exp (',' exp)+ ')'            #tupleexp
  | exp '.' ID                        #memberexp
  | exp '[' exp ']'                   #indexexp
  | lexp '(' explist? ')'              #funccallexp
  | op=('-' | 'not') exp              #unaryexp
  | exp op=('*' | '/' | '%') exp      #multiplicativeexp
  | exp op=('+' | '-') exp            #additiveexp
  | exp op=('<=' | '<' | '>' | '>=') exp #relationexp
  | exp op=('==' | '!=' ) exp            #comparisonexp
  | exp 'and' exp                     #andexp
  | exp 'or' exp                      #orexp
  | exp 'if' exp 'else' exp           #ternaryexp
  | exp 'as' type                     #castexp
  | '(' exp ')'                       #parenexp
  ;

fielddef: ID '=' exp;
explist: exp (',' exp)*;


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