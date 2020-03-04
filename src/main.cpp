#include <iostream>

#include <antlr4-runtime/antlr4-runtime.h>
#include "parser/PhilippeParser.h"
#include "parser/PhilippeLexer.h"
#include "ASTGen.h"
#include "Printer.h"
#include "Interpreter.h"
#include "VirtualMachine.h"
#include "Assembler.h"

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
    /*
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
        IfJump, 74,

        LoadM, n, // println(n)
        LoadS, 76,
        Call, Printf,

        LoadS, 2, // if n%2==1
        LoadM, n,
        Modi, Noop,
        LoadS, 1,
        Eqi, Noop,
        IfJump, 52,

        LoadS, 2, // n/2
        LoadM, n,
        Divi, Noop,
        Jump, 62,

        LoadS, 3, // 3*n+1, 52
        LoadM, n,
        Muli, Noop,
        LoadS, 1,
        Addi, Noop,

        Store, n, // n assign, i += 1, 62
        LoadS, 1,
        LoadM, i,
        Addi, Noop,
        Store, i,
        Jump, 8,

        End, Noop, // 74
        '%', 'd', '\n', '\0' // printf string, 76
    });
    */

    m.load(assemble(R"(
        loads 28472
        store n
        loads 0
        store i

    loop:
        loadm i
        loads 100
        gti
        loads 1
        loadm n
        neqi
        and
        not
        ifjump endp

        loadm n
        loads str
        call printf

        loads 2
        loadm n
        modi
        loads 1
        eqi 
        ifjump cond1

        loads 2
        loadm n
        divi
        jump after

    cond1:
        loads 3
        loadm n
        muli
        loads 1
        addi

    after:
        store n
        loads 1
        loadm i
        addi
        store i
        jump loop

    endp:
        end

    str:
        "%d\n"

    )",
    {
        {"n", n},
        {"i", i},
    }));

    cout << "VM output : " << endl;
    m.run();

    return 0;
}