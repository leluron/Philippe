#include "AST.h"

void printexpl(std::ostream &out, int indent, expl l, const char* sep) {
    if (l.size() == 0) return;

    l[0]->print(out, indent);
    for (int i=1;i<l.size();i++) {
        out << sep;
        l[i]->print(out, indent);
    }
}

void ind(std::ostream &out, int indent) {
    for (int i=1;i<indent;i++) out << "\t";
}

void printBlock(std::ostream &out, int indent, block b) {
    for (auto s : b) {
        ind(out, indent);
        s->print(out, indent+1);
        out << std::endl;
    }
}

void AssignStat::print(std::ostream &out, int indent) {
    ind(out, indent);
    printexpl(out, indent, left, ", ");
    out << " = ";
    right->print(out, indent);
}

void FuncCallStat::print(std::ostream &out, int indent) {
    ind(out, indent);
    func->print(out, indent);
    out << "(";
    printexpl(out, indent, args, ", ");
    out << ")";
}

void WhileStat::print(std::ostream &out, int indent) {
    ind(out, indent);
    out << "while ";
    cond->print(out, indent);
    body->print(out, indent);
}

void IfStat::print(std::ostream &out, int indent) {
    ind(out, indent);
    out << "if ";
    cond->print(out, indent);
    thenbody->print(out, indent);
    out << "else ";
    elsebody->print(out, indent);
}

void ForStat::print(std::ostream &out, int indent) {
    ind(out, indent);
    out << "for " << name << " in ";
    list->print(out, indent);
    body->print(out, indent);
}

void BlockStat::print(std::ostream &out, int indent) {
    out << "{" << std::endl;
    printBlock(out, indent, stats);
    out << "}";
}

void BreakStat::print(std::ostream &out, int indent) {
    ind(out, indent);
    out << "break";
}

void ReturnStat::print(std::ostream &out, int indent) {
    ind(out, indent);
    out << "return ";
    ret->print(out, indent);
}

void EmptyStat::print(std::ostream &out, int indent) {
    
}

void NilExp::print(std::ostream &out, int indent) {
    out << "nil";
}

void BoolExp::print(std::ostream &out, int indent) {
    out << (val?"true":"false");
}

void IntExp::print(std::ostream &out, int indent) {
    out << val;
}

void FloatExp::print(std::ostream &out, int indent) {
    out << val;
}

void StringExp::print(std::ostream &out, int indent) {
    out << "\"" << val << "\"";
}

void IdExp::print(std::ostream &out, int indent) {
    out << name;
}

void ObjExp::print(std::ostream &out, int indent) {
    out << "{";
    for (auto f : fields) {
        out << f.name << "=";
        f.e->print(out, indent);
    }
    out << "}";
}

void ListExp::print(std::ostream &out, int indent) {
    out << "[";
    printexpl(out, indent, elements, ", ");
    out << "]";
}

void RangeExp::print(std::ostream &out, int indent) {
    out << "[";
    begin->print(out, indent);
    out << "..";
    end->print(out, indent);
    out << "]";
}

void TupleExp::print(std::ostream &out, int indent) {
    out << "(";
    printexpl(out, indent, elements, ", ");
    out << ")";
}

void FuncExp::print(std::ostream &out, int indent) {
    out << "function (";
    for (auto n : args) {
        out << n << ", ";
    }
    out << ") {";
    printBlock(out, indent, body.stats);
    out << "return ";
    body.ret->print(out, indent);
    out << "}";
}

void MemberExp::print(std::ostream &out, int indent) {
    left->print(out, indent);
    out << "." << name;
}

void IndexExp::print(std::ostream &out, int indent) {
    left->print(out, indent);
    out << "[";
    index->print(out, indent);
    out << "]";
}

void CallExp::print(std::ostream &out, int indent) {
    func->print(out, indent);
    out << "(";
    printexpl(out, indent, args, ", ");
    out << ")";
}

void UnaryOpExp::print(std::ostream &out, int indent) {
    out << "(";
    if (op == UnaryOp::Minus) out << "-";
    else if (op == UnaryOp::Not) out << "not";
    e->print(out, indent);
    out << ")";
}

void BinOpExp::print(std::ostream &out, int indent) {
    out << "(";
    left->print(out, indent);
    if (op == BinOp::Pow) out << "^";
    else if (op == BinOp::Mul) out << "*";
    else if (op == BinOp::Div) out << "/";
    else if (op == BinOp::Mod) out << "%";
    else if (op == BinOp::Plus) out << "+";
    else if (op == BinOp::Minus) out << "-";
    else if (op == BinOp::Concat) out << "..";
    else if (op == BinOp::Lteq) out << "<=";
    else if (op == BinOp::Lt) out << "<";
    else if (op == BinOp::Gt) out << ">";
    else if (op == BinOp::Gteq) out << ">=";
    else if (op == BinOp::Eq) out << "==";
    else if (op == BinOp::Neq) out << "!=";
    else if (op == BinOp::And) out << "and";
    else if (op == BinOp::Or) out << "or";
    right->print(out, indent);
    out << ")";
}

void TernaryExp::print(std::ostream &out, int indent) {
    out << "(";
    then->print(out, indent);
    out << " if ";
    cond->print(out, indent);
    out << " else ";
    els->print(out, indent);
    out << ")";
}
