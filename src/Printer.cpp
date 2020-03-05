#include "AST.h"

using namespace std;

void print(ostream &out, int indent, statp sb);
void print(ostream &out, int indent, expp eb);

void printexpl(ostream &out, int indent, expl l, const char* sep) {
    if (l.size() == 0) return;

    print(out, indent, l[0]);
    for (int i=1;i<l.size();i++) {
        out << sep;
        print(out, indent, l[i]);
    }
}

void ind(ostream &out, int indent) {
    for (int i=1;i<indent;i++) out << "\t";
}

void printBlock(ostream &out, int indent, block b) {
    for (auto s : b) {
        ind(out, indent);
        print(out, indent+1, s);
        out << endl;
    }
}


void print(ostream &out, int indent, statp sb) {
    if (auto s = dynamic_pointer_cast<BlockStat>(sb)) {
        out << "{" << endl;
        printBlock(out, indent, s->stats);
        out << "}";
    } else {
        ind(out, indent);
        if (auto s = dynamic_pointer_cast<FuncCallStat>(sb)) {
            print(out, indent, s->func);
            out << "(";
            printexpl(out, indent, s->args, ", ");
            out << ")";
        } else if (auto s = dynamic_pointer_cast<WhileStat>(sb)) {
            out << "while ";
            print(out, indent, s->cond);
            print(out, indent, s->body);
        } else if (auto s = dynamic_pointer_cast<IfStat>(sb)) {
            out << "if ";
            print(out, indent, s->cond);
            print(out, indent, s->thenbody);
            out << "else ";
            print(out, indent, s->elsebody);
        } else if (auto s = dynamic_pointer_cast<ForStat>(sb)) {
            out << "for " << s->name << " in ";
            print(out, indent, s->list);
            print(out, indent, s->body);
        } else if (auto s = dynamic_pointer_cast<AssignStat>(sb)) {
            printexpl(out, indent, s->left, ", ");
            out << " = ";
            print(out, indent, s->right);
        } else if (auto s = dynamic_pointer_cast<BreakStat>(sb)) {
            out << "break";
        } else if (auto s = dynamic_pointer_cast<ReturnStat>(sb)) {
            out << "return ";
            print(out, indent, s->ret);
        }
    }
}

void print(ostream &out, int indent, expp eb) {
    if (auto e = dynamic_pointer_cast<NilExp>(eb)) {
        out << "nil";

    } else if (auto e = dynamic_pointer_cast<BoolExp>(eb)) {
        out << (e->val?"true":"false");

    } else if (auto e = dynamic_pointer_cast<IntExp>(eb)) {
       out << e->val;
        
    } else if (auto e = dynamic_pointer_cast<FloatExp>(eb)) {
        out << e->val;
        
    } else if (auto e = dynamic_pointer_cast<StringExp>(eb)) {
        out << "\"" << e->val << "\"";
        
    } else if (auto e = dynamic_pointer_cast<IdExp>(eb)) {
        out << e->name;
        
    } else if (auto e = dynamic_pointer_cast<ObjExp>(eb)) {
        out << "{";
        for (auto f : e->fields) {
            out << f.name << "=";
            print(out, indent, f.e);
        }
        out << "}";
        
    } else if (auto e = dynamic_pointer_cast<ListExp>(eb)) {
        out << "[";
        printexpl(out, indent, e->elements, ", ");
        out << "]";
        
    } else if (auto e = dynamic_pointer_cast<RangeExp>(eb)) {
        out << "[";
        print(out, indent, e->begin);
        out << "..";
        print(out, indent, e->end);
        out << "]";
        
    } else if (auto e = dynamic_pointer_cast<TupleExp>(eb)) {
        out << "(";
        printexpl(out, indent, e->elements, ", ");
        out << ")";
        
    } else if (auto e = dynamic_pointer_cast<FuncExp>(eb)) {
        out << "function (";
        for (auto n : e->args) {
            out << n << ", ";
        }
        out << ") {";
        printBlock(out, indent, e->body.stats);
        out << "return ";
        print(out, indent, e->body.ret);
        out << "}";
        
    } else if (auto e = dynamic_pointer_cast<MemberExp>(eb)) {
        print(out, indent, e->left);
        out << "." << e->name;
        
    } else if (auto e = dynamic_pointer_cast<IndexExp>(eb)) {
        print(out, indent, e->left);
        out << "[";
        print(out, indent, e->index);
        out << "]";
        
    } else if (auto e = dynamic_pointer_cast<CallExp>(eb)) {
        print(out, indent, e->func);
        out << "(";
        printexpl(out, indent, e->args, ", ");
        out << ")";
        
    } else if (auto e = dynamic_pointer_cast<UnaryOpExp>(eb)) {
        out << "(";
        if (e->op == UnaryOp::Minus) out << "-";
        else if (e->op == UnaryOp::Not) out << "not";
        print(out, indent, e->e);
        out << ")";
        
    } else if (auto e = dynamic_pointer_cast<BinOpExp>(eb)) {
        out << "(";
        print(out, indent, e->left);
        if (e->op == BinOp::Pow) out << "^";
        else if (e->op == BinOp::Mul) out << "*";
        else if (e->op == BinOp::Div) out << "/";
        else if (e->op == BinOp::Mod) out << "%";
        else if (e->op == BinOp::Plus) out << "+";
        else if (e->op == BinOp::Minus) out << "-";
        else if (e->op == BinOp::Concat) out << "..";
        else if (e->op == BinOp::Lteq) out << "<=";
        else if (e->op == BinOp::Lt) out << "<";
        else if (e->op == BinOp::Gt) out << ">";
        else if (e->op == BinOp::Gteq) out << ">=";
        else if (e->op == BinOp::Eq) out << "==";
        else if (e->op == BinOp::Neq) out << "!=";
        else if (e->op == BinOp::And) out << "and";
        else if (e->op == BinOp::Or) out << "or";
        print(out, indent, e->right);
        out << ")";
        
    } else if (auto e = dynamic_pointer_cast<TernaryExp>(eb)) {
        out << "(";
        print(out, indent, e->then);
        out << " if ";
        print(out, indent, e->cond);
        out << " else ";
        print(out, indent, e->els);
        out << ")";
        
    }
}