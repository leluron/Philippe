#include "CodeGen.h"
#include "Assembler.h"

vmcode genCode(block &b, CodeGenContext &c) {
    codegen(b, c);
    return {};
}

void codegen(block &b, CodeGenContext &c) {
    for (auto s : b) {
        s->codegen(c);
    }
}

void AssignStat::codegen(CodeGenContext &c) {
    
}

void FuncCallStat::codegen(CodeGenContext &c) {
    
}

void WhileStat::codegen(CodeGenContext &c) {
    
}

void IfStat::codegen(CodeGenContext &c) {
    
}

void ForStat::codegen(CodeGenContext &c) {
    
}

void BlockStat::codegen(CodeGenContext &c) {
    
}

void BreakStat::codegen(CodeGenContext &c) {
    
}

void ReturnStat::codegen(CodeGenContext &c) {
    
}

void EmptyStat::codegen(CodeGenContext &c) {
    
}

void NilExp::codegen(CodeGenContext &c) {
    
}

void BoolExp::codegen(CodeGenContext &c) {
    
}

void IntExp::codegen(CodeGenContext &c) {
    
}

void FloatExp::codegen(CodeGenContext &c) {
    
}

void StringExp::codegen(CodeGenContext &c) {
    
}

void IdExp::codegen(CodeGenContext &c) {
    
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
    
}

void BinOpExp::codegen(CodeGenContext &c) {
    
}

void TernaryExp::codegen(CodeGenContext &c) {
    
}