#include <iostream>

#include <antlr4-runtime/antlr4-runtime.h>
#include "parser/PhilippeParser.h"
#include "parser/PhilippeLexer.h"
#include "ASTGen.h"
#include "Printer.h"
#include "Interpreter.h"
#include "VirtualMachine.h"

using namespace std;
using namespace antlr4;

int main() {

    ifstream stream("test.phil");
    ANTLRInputStream input(stream);
    PhilippeLexer lexer(&input);
    CommonTokenStream tokens(&lexer);
    PhilippeParser parser(&tokens);    
    PhilippeParser::ChunkContext* tree = parser.chunk();

    ASTGen gen;
    auto ast = gen.gen(tree);

    printBlock(cout, 0, ast);

    cout << "Interpreter output : " << endl;
    InterpreterContext ic(cout);
    evalBlock(ic, ast);

    VirtualMachine m(cout);

    m.setSize(0x2000);
    int64_t n = 0x1000;
    int64_t i = 0x1001;
    m.load({
        LoadS, 28472, // Init, 0
        Store, n,
        LoadS, 0,
        Store, i,

        LoadM, i, // While i<100 and n != 1, 8
        LoadS, 100,
        Gti, Noop,
        LoadS, 1,
        LoadM, n,
        Neqi, Noop,
        And, Noop,
        Not, Noop,
        IfJump, 72,

        LoadM, n, // println(n)
        Call, Printlni,

        LoadS, 2, // if n%2==1, 30
        LoadM, n,
        Modi, Noop,
        LoadS, 1,
        Eqi, Noop,
        IfJump, 50,

        LoadS, 2, // n/2, 42
        LoadM, n,
        Divi, Noop,
        Jump, 60,

        LoadS, 3, // 3*n+1, 50
        LoadM, n,
        Muli, Noop,
        LoadS, 1,
        Addi, Noop,

        Store, n, // n assign, i += 1, 60
        LoadS, 1,
        LoadM, i,
        Addi, Noop,
        Store, i,
        Jump, 8,

        End, Noop // 72
    });

    cout << "VM output : " << endl;
    m.run();

    return 0;
}