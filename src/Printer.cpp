#include "AST.h"

using namespace std;

void print(ostream &out, defp db);
void print(ostream &out, int indent, statp sb);
void print(ostream &out, expp eb);
void print(ostream &out, typep tb);
void print(ostream &out, lexpp lp);

void printexpl(ostream &out, expl l, const char* sep) {
    if (l.size() == 0) return;

    print(out, l[0]);
    for (int i=1;i<l.size();i++) {
        out << sep;
        print(out, l[i]);
    }
}

void ind(ostream &out, int indent) {
    for (int i=0;i<indent;i++) out << "\t";
}

void print(ostream &out, File f) {
    for (auto d : f.defs) {
        print(out, d);
    }
}

void print(ostream &out, defp db) {
    if (auto d = dynamic_pointer_cast<GlobalDef>(db)) {
        out << d->name;
        if (d->type) {
            out << " : ";
            print(out, d->type);
        }
        out << " = ";
        print(out, d->value);
    } else if (auto d = dynamic_pointer_cast<FunctionDef>(db)) {
        out << d->name << " = function(";
        for (auto a : d->args) {
            out << a.name << " : ";
            print(out, a.type);
        }
        out << ") {" << endl;
        for (auto s : d->body) {
            print(out, 1, s);
            out << endl;
        }
        out << "}" << endl;
    } else if (auto d = dynamic_pointer_cast<AliasDef>(db)) {
        out << d->name << " = ";
        print(out, d->t);
    } else if (auto d = dynamic_pointer_cast<ObjDef>(db)) {
        out << "type " << d->name << " = {";
        for (auto a : d->fields) {
            ind(out, 1);
            out << a.name << " : ";
            print(out, a.type);
        }
    }
}

void print(ostream &out, int indent, statp sb) {
    if (auto s = dynamic_pointer_cast<BlockStat>(sb)) {
        out << "{" << endl;
        for (auto s0 : s->stats) {
            print(out, indent+1, s0);
            out << endl;
        }
        ind(out, indent);
        out << "}";
    } else {
        ind(out, indent);
        if (auto s = dynamic_pointer_cast<FuncCallStat>(sb)) {
            print(out, s->func);
            out << "(";
            printexpl(out, s->args, ", ");
            out << ")";
        } else if (auto s = dynamic_pointer_cast<WhileStat>(sb)) {
            out << "while ";
            print(out, s->cond);
            print(out, indent, s->body);
        } else if (auto s = dynamic_pointer_cast<IfStat>(sb)) {
            out << "if ";
            print(out, s->cond);
            print(out, indent, s->thenbody);
            out << "else ";
            print(out, indent, s->elsebody);
        } else if (auto s = dynamic_pointer_cast<AssignStat>(sb)) {
            print(out, s->left);
            out << " = ";
            print(out, s->right);
        } else if (auto s = dynamic_pointer_cast<BreakStat>(sb)) {
            out << "break";
        } else if (auto s = dynamic_pointer_cast<ReturnStat>(sb)) {
            out << "return ";
            print(out, s->ret);
        }
    }
}

void print(ostream &out, expp eb) {
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
        out << e->name << " {";
        for (auto f : e->fields) {
            out << f.name << "=";
            print(out, f.e);
        }
        out << "}";
        
    } else if (auto e = dynamic_pointer_cast<ListExp>(eb)) {
        out << "[";
        printexpl(out, e->elements, ", ");
        out << "]";
        
    } else if (auto e = dynamic_pointer_cast<TupleExp>(eb)) {
        out << "(";
        printexpl(out, e->elements, ", ");
        out << ")";
        
    } else if (auto e = dynamic_pointer_cast<MemberExp>(eb)) {
        print(out, e->left);
        out << "." << e->name;
        
    } else if (auto e = dynamic_pointer_cast<IndexExp>(eb)) {
        print(out, e->left);
        out << "[";
        print(out, e->index);
        out << "]";
        
    } else if (auto e = dynamic_pointer_cast<CallExp>(eb)) {
        print(out, e->func);
        out << "(";
        printexpl(out, e->args, ", ");
        out << ")";
    } else if (auto e = dynamic_pointer_cast<UnaryOpExp>(eb)) {
        out << "(";
        if (e->op == UnaryOp::Minus) out << "-";
        else if (e->op == UnaryOp::Not) out << "not";
        print(out, e->e);
        out << ")";
        
    } else if (auto e = dynamic_pointer_cast<BinOpExp>(eb)) {
        out << "(";
        print(out, e->left);
        if (e->op == BinOp::Mul) out << "*";
        else if (e->op == BinOp::Div) out << "/";
        else if (e->op == BinOp::Mod) out << "%";
        else if (e->op == BinOp::Plus) out << "+";
        else if (e->op == BinOp::Minus) out << "-";
        else if (e->op == BinOp::Lteq) out << "<=";
        else if (e->op == BinOp::Lt) out << "<";
        else if (e->op == BinOp::Eq) out << "==";
        else if (e->op == BinOp::And) out << "and";
        else if (e->op == BinOp::Or) out << "or";
        print(out, e->right);
        out << ")";
        
    } else if (auto e = dynamic_pointer_cast<TernaryExp>(eb)) {
        out << "(";
        print(out, e->then);
        out << " if ";
        print(out, e->cond);
        out << " else ";
        print(out, e->els);
        out << ")";
        
    } else if (auto e = dynamic_pointer_cast<CastExp>(eb)) {
        out << "(";
        print(out, e->e);
        out << " as ";
        print(out, e->t);
        out << ")";
    }
}

void print(ostream &out, typep tb) {
    if (dynamic_pointer_cast<TypeInt>(tb)) out << "int";
    else if (dynamic_pointer_cast<TypeFloat>(tb)) out << "float";
    else if (dynamic_pointer_cast<TypeBool>(tb)) out << "bool";
    else if (dynamic_pointer_cast<TypeString>(tb)) out << "string";
    else if (dynamic_pointer_cast<TypeNil>(tb)) out << "nil";
    else if (auto t = dynamic_pointer_cast<TypeTuple>(tb)) {
        out << "(";
        for (auto t0 : t->t) {
            print(out, t0);
            out << ",";
        }
        out << ")";
    } else if (auto t = dynamic_pointer_cast<TypeObj>(tb)) {
        out << t->name;
    } else if (auto t = dynamic_pointer_cast<TypeFunction>(tb)) {
        out << "function(";
        for (auto t0 : t->args) {
            print(out, t0);
            out << " ";
        }
        out << " -> ";
        print(out, t->ret);
    } else if (auto t = dynamic_pointer_cast<TypeList>(tb)) {
        out << "list ";
        print(out, t->t);
    }
}

void print(ostream &out, lexpp lp) {
    out << lp->name;
    for (auto suf : lp->suffixes) {
        if (auto ind = dynamic_pointer_cast<IndexSuffix>(suf)) {
            out << "[";
            print(out, ind->i);
            out << "]";
        } else if (auto mem = dynamic_pointer_cast<MemberSuffix>(suf)) {
            out << "." << mem->name;
        }
    }
    if (lp->type) {
        out << " : ";
        print(out, lp->type);
    }
}