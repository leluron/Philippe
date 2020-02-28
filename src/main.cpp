#include <iostream>

#include <antlr4-runtime/antlr4-runtime.h>
#include "parser/PhilippeParser.h"
#include "parser/PhilippeLexer.h"
#include "ASTGen.h"

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

    return 0;
}