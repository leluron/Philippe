#include "CodeGen.h"
#include "Assembler.h"

#include <sstream>
#include <set>

using namespace std;

enum Type {
    INT,
    FLOAT,
};

class CodeGenContext {
public:
    string code;
    string constants;
    set<string> vars;
    Type lastType;
    uint64_t lblid = 0;

    std::string newlbl() {
        return "l" + lblid++;
    }

    void genCodeBlock(block b);
    void genCode(statp sb);
    void genCode(expp eb);

    void genCodeBinOpExp(BinOpExp e);
};

std::string genCode(block b) {
    CodeGenContext c;
    genCodeBlock(b, c);
    return c.code + c.constants;
}

void CodeGenContext::genCodeBlock(block b) {
    for (auto s : b) {
        genCode(s);
    }
}

void CodeGenContext::genCode(statp sb) {

}

void CodeGenContext::genCode(expp sb) {
    
}

void AssignStat::codegen(CodeGenContext &c) {
    
}

void FuncCallStat::codegen(CodeGenContext &c) {
    
}

void WhileStat::codegen(CodeGenContext &c) {
    auto startlbl = c.newlbl();
    auto endlbl = c.newlbl();
    c.code += startlbl + ":\n";
    cond->codegen(c);
    c.code += "ifjump " + endlbl + "\n";
    body->codegen(c);
    c.code += "jump " + startlbl + "\n";
    c.code += endlbl + ":\n";
}

void IfStat::codegen(CodeGenContext &c) {
    cond->codegen(c);
    auto thenlbl = c.newlbl();
    auto afterlbl = c.newlbl();
    c.code += "ifjump " + thenlbl + "\n";
    elsebody->codegen(c);
    c.code += "jump " + afterlbl + "\n";
    c.code += thenlbl + ":";
    thenbody->codegen(c);
    c.code += afterlbl + ":\n";
}

void ForStat::codegen(CodeGenContext &c) {
    
}

void BlockStat::codegen(CodeGenContext &c) {
    genCodeBlock(stats, c);
}

void BreakStat::codegen(CodeGenContext &c) {
    
}

void ReturnStat::codegen(CodeGenContext &c) {
    ret->codegen(c);
    c.code += "return\n";
}

void EmptyStat::codegen(CodeGenContext &c) {}

void NilExp::codegen(CodeGenContext &c) {
    c.code += "loads 0\n";
    c.lastType = INT;
}

void BoolExp::codegen(CodeGenContext &c) {
    stringstream ss;
    ss << (val?1:0);
    c.code += "loads " + ss.str() + "\n";
    c.lastType = INT;
}

void IntExp::codegen(CodeGenContext &c) {
    stringstream ss;
    ss << val;
    c.code += "loads " + ss.str() + "\n";
    c.lastType = INT;
}

void FloatExp::codegen(CodeGenContext &c) {
    stringstream ss;
    ss << val;
    c.code += "loads " + ss.str() + "\n";
    c.lastType = FLOAT;
}

void StringExp::codegen(CodeGenContext &c) {
    
}

void IdExp::codegen(CodeGenContext &c) {
    c.vars.insert(name);
    c.code += "loadm " + name + "\n";
}

void ObjExp::codegen(CodeGenContext &c) {
    
}

void ListExp::codegen(CodeGenContext &c) {
    
}

void RangeExp::codegen(CodeGenContext &c) {
    
}

void TupleExp::codegen(CodeGenContext &c) {
    
}

void FuncExp::codegen(CodeGenContext &c) {
    
}

void MemberExp::codegen(CodeGenContext &c) {
    
}

void IndexExp::codegen(CodeGenContext &c) {
    
}

void CallExp::codegen(CodeGenContext &c) {
    
}

void UnaryOpExp::codegen(CodeGenContext &c) {
    e->codegen(c);
    if (op == UnaryOp::Minus) {
        if (c.lastType == INT) c.code += "usubi\n";
        else if (c.lastType == FLOAT) c.code += "usubf\n";
        else throw;
    } else if (op == UnaryOp::Not) {
        if (c.lastType == INT) c.code += "not\n";
        else throw;
    }
}

void CodeGenContext::genCodeBinOpExp(BinOpExp e) {
    genCode(e.left);
    auto lefttype = lastType;
    genCode(e.right);
    auto righttype = lastType;
    auto op = e.op;
    if (op == BinOp::Pow) {
        if (righttype != INT) throw;
        if (lefttype == INT) code += "powi\n";
        else if (lefttype == FLOAT) code += "powf\n";
        else throw;
    } else if (op == BinOp::Mul) {
        code += "muli\n";
    } else if (op == BinOp::Div) {
        code += "divi\n";
    } else if (op == BinOp::Mod) {
        if (righttype != INT) throw;
        code += "modi\n";
    } else if (op == BinOp::Plus) {
        code += "addi\n";
    } else if (op == BinOp::Minus) {
        code += "subi\n";
    } else if (op == BinOp::Lteq) {
        code += "lteqi\n";
    } else if (op == BinOp::Lt) {
        code += "lti\n";
    } else if (op == BinOp::Gt) {
        code += "gti\n";
    } else if (op == BinOp::Gteq) {
        code += "gteqi\n";
    } else if (op == BinOp::Eq) {
        code += "eqi\n";
    } else if (op == BinOp::Neq) {
        code += "neqi\n";
    } else if (op == BinOp::And) {
        if (lefttype != INT || righttype != INT) throw;
        code += "and\n";
    } else if (op == BinOp::Or) {
        if (lefttype != INT || righttype != INT) throw;
        code += "or\n";
    }
}

void TernaryExp::codegen(CodeGenContext &c) {
    cond->codegen(c);
    auto thenlbl = c.newlbl();
    auto afterlbl = c.newlbl();
    c.code += "ifjump " + thenlbl + "\n";
    els->codegen(c);
    c.code += "jump " + afterlbl + "\n";
    c.code += thenlbl + ":";
    then->codegen(c);
    c.code += afterlbl + ":";
}