grammar Bytecode;

code: instr* EOF;
instr
    : label? op
    ;

label: name ':';
op
    : opcode intliteral
    | opcode floatliteral
    | opcode name
    | opcode
    | stringarray
    ;

intliteral: INT | HEX;
floatliteral: FLOAT;
name: ID;
stringarray: STRING;

opcode
    : o=('noop'
    | 'loads'
    | 'loadm'
    | 'store'
    | 'alloc'
    | 'free'
    | 'call'
    | 'return'
    | 'ifjump'
    | 'jump'
    | 'castfi'
    | 'castif'
    | 'not'
    | 'and'
    | 'or'
    | 'usubi'
    | 'usubf'
    | 'powi'
    | 'powf'
    | 'muli'
    | 'mulf'
    | 'divi'
    | 'divf'
    | 'modi'
    | 'addi'
    | 'addf'
    | 'subi'
    | 'subf'
    | 'lteqi'
    | 'lteqf'
    | 'lti'
    | 'ltf'
    | 'gti'
    | 'gtf'
    | 'gteqi'
    | 'gteqf'
    | 'eqi'
    | 'eqf'
    | 'neqi'
    | 'neqf'
    | 'end')
    ;

ID
    : [a-zA-Z] [a-zA-Z_0-9]*
    ;

INT
    : [0-9]+
    ;

FLOAT
    : [0-9]+ '.' [0-9]* 
    | '.' [0-9]+ 
    ;

STRING
  : '"' (~('"'))* '"'
  | '\'' (~('\''))* '\''
  ;

HEX
  : '0' [xX] [0-9a-fA-F]+;

SPACE
    : [ \t\r\n] -> skip
    ;

OTHER
    : . 
    ;