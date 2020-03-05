#include "Interpreter.h"

#include <map>

using namespace std;

enum class ValueKind {
    Nil, Bool, Int, Float, String, Obj, List, Range, Tuple, Func, NativeFunc
};

class Value;
using valp = shared_ptr<Value>;

class Value {
public:
    virtual ~Value() {}
    virtual valp unaryOp(UnaryOp op) {
        throw "Unsupported unary op";
    }
    virtual valp binOp(BinOp op, valp right) {
        throw "Unsupported binary op";
    }
};


class NilValue : public Value {
public:
    
};

class BoolValue : public Value {
public:
    BoolValue(bool val) : val(val) {}
    bool val;
    
    virtual valp unaryOp(UnaryOp op) override {
        if (op == UnaryOp::Not) {
            return valp(new BoolValue(!val));
        }
        throw "Unsupported bool operation";
    }
    virtual valp binOp(BinOp op, valp right) override {
        if (auto v0 = dynamic_pointer_cast<BoolValue>(right)) {
            auto v1 = v0->val;
            if (op == BinOp::Eq) return valp(new BoolValue(val == v1));
            else if (op == BinOp::Neq) return valp(new BoolValue(val != v1));
            else if (op == BinOp::And) return valp(new BoolValue(val && v1));
            else if (op == BinOp::Or) return valp(new BoolValue(val || v1));
            else throw "Unsupported bool operation";
        } else throw "Unsupported bool operation";
    }
};
class IntValue : public Value {
public:
    IntValue(long val) : val(val) {}
    long val;
    
    virtual valp unaryOp(UnaryOp op) override {
        if (op == UnaryOp::Minus)
            return valp(new IntValue(-val));
        throw "Unsupported int operation";
    }
    virtual valp binOp(BinOp op, valp right) override;
};
class FloatValue : public Value {
public:
    FloatValue(double val) : val(val) {}
    FloatValue(IntValue v) : val(v.val) {}
    double val;
    
    virtual valp unaryOp(UnaryOp op) override {
        if (op == UnaryOp::Minus)
            return valp(new FloatValue(-val));
        throw;
    }
    virtual valp binOp(BinOp op, valp right) override;
};
class StringValue : public Value {
public:
    StringValue(string val) : val(val) {}
    string val;
    
    virtual valp binOp(BinOp op, valp right) override {
        if (op == BinOp::Concat) { 
            if (auto r0 = dynamic_pointer_cast<StringValue>(right)) {
                auto r = r0->val;
                return valp(new StringValue(val + r));
            }
        }
        throw;
    }
};
class ObjValue : public Value {
public:
    ObjValue() {}
    void add(string name, valp val) {
        fields[name] = val;
    }
    valp& get(string name) {
        auto f = fields.find(name);
        if (f == fields.end()) throw "test";
        return f->second;
    }
    map<string, valp> fields;
    
};
class ListValue : public Value {
public:
    ListValue(vector<valp> e) : elements(e) {}
    void add(valp e) {
        if (elements.size()==0) elements.push_back(e);
        else {
            if (typeid(e) == typeid(elements[0]))
                elements.push_back(e);
            else throw;
        }
    }
    ListValue() {}
    valp& get(int index) {
        if (index >= elements.size()) throw;
        return elements[index];
    }
    int size() { return elements.size(); }
    vector<valp> elements;
    
};
class RangeValue : public Value {
public:
    RangeValue(IntValue beg, IntValue end) : beg(beg), end(end) {}
    IntValue beg, end;
    
};
class TupleValue : public Value {
public:
    TupleValue(vector<valp> e) : elements(e) {}
    valp& get(int index) {
        if (index >= elements.size()) throw;
        return elements[index];
    }
    vector<valp> elements;
    
};
class FuncValue : public Value {
public:
    FuncValue(FuncExp f) : func(f) {}
    FuncExp func;
    
};

class InterpreterContext;

class NativeFuncValue : public Value {
public:
    virtual valp exec(InterpreterContext &c, vector<valp> args)=0;
};

class NativePrint : public NativeFuncValue {
public:
    virtual valp exec(InterpreterContext &c, vector<valp> args) override;
};

class StdLib : public ObjValue {
public:
    StdLib();
};


class InterpreterContext {
public:
    InterpreterContext(ostream &out) : out(out) {
        vars.push_back(StdLib());
    }
    void evalBlock(block b);
    valp eval(expp eb);
    valp& leval(expp eb);
    void eval(statp sb);
    valp& get(string name) {
        for (int i=vars.size()-1;i>=0;i--) {
            try {
                return vars[i].get(name);
            } catch (...) {
                continue;
            }
        }
        throw;
    }
    valp& getLeftValue(string name) {
        for (int i=vars.size()-1;i>=0;i--) {
            try {
                return vars[i].get(name);
            } catch (...) {
                continue;
            }
        }
        vars.back().add(name, valp(new NilValue()));
        return vars.back().get(name);
    }
    void newFrame() {
        vars.push_back(ObjValue());
    }
    void popFrame() {
        vars.pop_back();
    }
    void add(string name, valp val) {
        vars.back().add(name, val);
    }

    ostream &out;
    vector<ObjValue> vars;
    bool broke = false;
    bool returned = false;
    valp returnval = nullptr;
};

void InterpreterContext::evalBlock(block b) {
    for (auto s : b) {
        eval(s);
        if (broke || returned) break;
    }
}

void InterpreterContext::eval(statp sb) {
    if (auto s = dynamic_pointer_cast<AssignStat>(sb)) {
        auto r = eval(s->right);
        if (s->left.size() > 1) {
            if (auto tu0 = dynamic_pointer_cast<TupleValue>(r)) {
                auto tu = tu0->elements;
                if (s->left.size() != tu.size()) throw;
                for (int i=0;i<s->left.size();i++) {
                    leval(s->left[i]) = tu[i];
                }
            }
        } else {
            leval(s->left[0]) = r;
        }
    } else if (auto s = dynamic_pointer_cast<FuncCallStat>(sb)) {
        eval(expp(new CallExp(s->func, s->args)));
    } else if (auto s = dynamic_pointer_cast<WhileStat>(sb)) {
        while (true) {
            auto v = eval(s->cond);
            if (auto v0 = dynamic_pointer_cast<BoolValue>(v)) {
                if (!v0->val) return;
                eval(s->body);
                if (broke || returned) {
                    broke = false;
                    break;
                }
            } else throw "While condition must be a bool";
        } 
    } else if (auto s = dynamic_pointer_cast<IfStat>(sb)) {
        auto v = eval(s->cond);
        if (auto v0 = dynamic_pointer_cast<BoolValue>(v)) {
            if (v0->val) 
                eval(s->thenbody);
            else
                eval(s->elsebody);
        } throw "If condition must be a bool";
    } else if (auto s = dynamic_pointer_cast<ForStat>(sb)) {
        auto l = eval(s->list);
        if (auto l0 = dynamic_pointer_cast<ListValue>(l)) {
            for (auto i : l0->elements) {
                newFrame();
                add(s->name, i);
                eval(s->body);
                popFrame();
                broke = false;
            }
        } else if (auto r = dynamic_pointer_cast<RangeValue>(l)) {
            for (auto i=r->beg.val;i<r->end.val;i++) {
                newFrame();
                add(s->name, valp(new IntValue(i)));
                eval(s->body);
                popFrame();
                broke = false;
            }
        } else throw;
    } else if (auto s = dynamic_pointer_cast<BlockStat>(sb)) {
        evalBlock(s->stats);
    } else if (auto s = dynamic_pointer_cast<BreakStat>(sb)) {
        broke = true;
    } else if (auto s = dynamic_pointer_cast<ReturnStat>(sb)) {
        returned = true;
        returnval = eval(s->ret);
    }
}

valp InterpreterContext::eval(expp eb) {
    if (auto e = dynamic_pointer_cast<NilExp>(eb)) {
        return valp(new NilValue());
    } else if (auto e = dynamic_pointer_cast<BoolExp>(eb)) {
        return valp(new BoolValue(e->val));
    } else if (auto e = dynamic_pointer_cast<IntExp>(eb)) {
        return valp(new IntValue(e->val));
    } else if (auto e = dynamic_pointer_cast<FloatExp>(eb)) {
        return valp(new FloatValue(e->val));
    } else if (auto e = dynamic_pointer_cast<StringExp>(eb)) {
        return valp(new StringValue(e->val));
    } else if (auto e = dynamic_pointer_cast<IdExp>(eb)) {
        return get(e->name);
    } else if (auto e = dynamic_pointer_cast<ObjExp>(eb)) {
        auto o = new ObjValue();
        for (auto f : e->fields) {
            o->add(f.name, eval(f.e));
        }
        return valp(o);
    } else if (auto e = dynamic_pointer_cast<ListExp>(eb)) {
        auto l = new ListValue();
        for (auto el : e->elements) {
            l->add(eval(el));
        }
        return valp(l);
    } else if (auto e = dynamic_pointer_cast<RangeExp>(eb)) {
        auto beg = eval(e->begin);
        auto end0 = eval(e->end);

        auto b1 = dynamic_pointer_cast<IntValue>(beg);
        auto e1 = dynamic_pointer_cast<IntValue>(end0);

        if (!b1 || !e1)
            throw;

        return valp(new RangeValue(*b1,*e1));
    } else if (auto e = dynamic_pointer_cast<TupleExp>(eb)) {
        auto l = vector<valp>();
        for (auto el : e->elements) {
            l.push_back(eval(el));
        }
        return valp(new TupleValue(l));
    } else if (auto e = dynamic_pointer_cast<FuncExp>(eb)) {
        return valp(new FuncValue(*e));
    } else if (auto e = dynamic_pointer_cast<MemberExp>(eb)) {
        auto l = eval(e->left);

        if (auto o = dynamic_pointer_cast<ObjValue>(l))
            return o->get(e->name);
        else throw;
    } else if (auto e = dynamic_pointer_cast<IndexExp>(eb)) {
        auto l = eval(e->left);
        auto ind = eval(e->index);

        if (auto ind1 = dynamic_pointer_cast<IntValue>(ind)) {
            int ind0 = ind1->val;

            if (auto l0 = dynamic_pointer_cast<ListValue>(l))
                return l0->get(ind0);
            else if (auto l0 = dynamic_pointer_cast<TupleValue>(l))
                return l0->get(ind0);
            else throw;
        }
    } else if (auto e = dynamic_pointer_cast<CallExp>(eb)) {
        auto f = eval(e->func);

        if (auto f1 = dynamic_pointer_cast<FuncValue>(f)) {
            auto f0 = f1->func;

            newFrame();
            
            for (int i=0;i<e->args.size();i++) {
                add(f0.args[i], eval(e->args[i]));
            }

            evalBlock(f0.body.stats);
            if (returned) {
                returned = false;
                return returnval;
            }

            auto ret = eval(f0.body.ret);
            popFrame();
            return ret;
        } else if (auto f0 = dynamic_pointer_cast<NativeFuncValue>(f)) {
            vector<valp> argsv;
            for (int i=0;i<e->args.size();i++) {
                argsv.push_back(eval(e->args[i]));
            }
            return f0->exec(*this, argsv);
        } else throw "Unsupported type for calling";
    } else if (auto e = dynamic_pointer_cast<UnaryOpExp>(eb)) {
        auto a = eval(e->e);
        return a->unaryOp(e->op);
    } else if (auto e = dynamic_pointer_cast<BinOpExp>(eb)) {
        auto l = eval(e->left);
        auto r = eval(e->right);
        return l->binOp(e->op, r);
    } else if (auto e = dynamic_pointer_cast<TernaryExp>(eb)) {
        auto co = eval(e->cond);
        if (auto c1 = dynamic_pointer_cast<BoolValue>(co)) {
            if (c1->val) return eval(e->then);
            else return eval(e->els);
        } else throw;
    }
    throw;
}

valp& InterpreterContext::leval(expp eb) {
    if (auto e = dynamic_pointer_cast<IdExp>(eb)) {
        return getLeftValue(e->name);
    } else if (auto e = dynamic_pointer_cast<TernaryExp>(eb)) {
        auto co = eval(e->cond);
        if (auto c1 = dynamic_pointer_cast<BoolValue>(co)) {
            if (c1->val) return leval(e->then);
            else return leval(e->els);
        } else throw;
    } else throw;
}


valp IntValue::binOp(BinOp op, valp right) {
    if (auto v0 = dynamic_pointer_cast<IntValue>(right)) {
        auto v1 = v0->val; 
        if (op == BinOp::Pow) {
            long l = 1;
            for (int i=0;i<v1;i++) l *= val;
            return valp(new IntValue(l));
        } else if (op == BinOp::Mul) {
            return valp(new IntValue(val*v1));
        } else if (op == BinOp::Div) {
            return valp(new IntValue(val/v1));
        } else if (op == BinOp::Mod) {
            return valp(new IntValue(val%v1));
        } else if (op == BinOp::Plus) {
            return valp(new IntValue(val+v1));
        } else if (op == BinOp::Minus) {
            return valp(new IntValue(val-v1));
        } else if (op == BinOp::Lteq) {
            return valp(new BoolValue(val<=v1));
        } else if (op == BinOp::Lt) {
            return valp(new BoolValue(val<v1));
        } else if (op == BinOp::Gt) {
            return valp(new BoolValue(val>v1));
        } else if (op == BinOp::Gteq) {
            return valp(new BoolValue(val>=v1));
        } else if (op == BinOp::Eq) {
            return valp(new BoolValue(val==v1));
        } else if (op == BinOp::Neq) {
            return valp(new BoolValue(val!=v1));
        } else throw "Unsupported int operation";
    } else if (auto v0 = dynamic_pointer_cast<FloatValue>(right)) {
        return FloatValue(*this).binOp(op, v0);
    } else throw "Unsupported int operation";
}

valp FloatValue::binOp(BinOp op, valp right) {
    if (auto v0 = dynamic_pointer_cast<FloatValue>(right)) {
        auto v1 = v0->val; 
        if (op == BinOp::Pow) {
            double l = 1.0;
            for (int i=0;i<(long)v1;i++) l *= val;
            return valp(new FloatValue(l));
        } else if (op == BinOp::Mul) {
            return valp(new FloatValue(val*v1));
        } else if (op == BinOp::Div) {
            return valp(new FloatValue(val/v1));
        } else if (op == BinOp::Plus) {
            return valp(new FloatValue(val+v1));
        } else if (op == BinOp::Minus) {
            return valp(new FloatValue(val-v1));
        } else if (op == BinOp::Lteq) {
            return valp(new BoolValue(val<=v1));
        } else if (op == BinOp::Lt) {
            return valp(new BoolValue(val<v1));
        } else if (op == BinOp::Gt) {
            return valp(new BoolValue(val>v1));
        } else if (op == BinOp::Gteq) {
            return valp(new BoolValue(val>=v1));
        } else if (op == BinOp::Eq) {
            return valp(new BoolValue(val==v1));
        } else if (op == BinOp::Neq) {
            return valp(new BoolValue(val!=v1));
        } else throw "Unsupported float operation";
    } else if (auto v0 = dynamic_pointer_cast<IntValue>(right)) {
        return binOp(op, valp(new FloatValue(*v0)));
    } else throw "Unsupported float operation";
}

void printval(ostream &out, valp arg) {
    if (auto a = dynamic_pointer_cast<IntValue>(arg)) {
        out << a->val;
    } else if (auto a = dynamic_pointer_cast<FloatValue>(arg)) {
        out << a->val;
    } else if (auto a = dynamic_pointer_cast<BoolValue>(arg)) {
        out << ((a->val)?"true":"false");
    } else {
        throw "Unsupported type for printing";
    }
}

valp NativePrint::exec(InterpreterContext &co, vector<valp> args) {
    if (args.size() < 1) throw;
    auto arg = args[0];
    if (auto a = dynamic_pointer_cast<StringValue>(arg)) {
        auto str = a->val;
        int argi = 1;

        for (int i=0;i<str.size();i++) {
            char c = str[i];
            if (c == '\\') {
                i++;
                char c1 = str[i];
                if (c1 == '0') break;
                else if (c1 == 'n') co.out << endl;
            }
            else if (c == '%') {
                i++;
                char c1 = str[i];
                if (c1 == 'd' || c1 == 'i') {
                    printval(co.out, args[argi]);
                } else if (c1 == 'f' || c1 == 'g') {
                    printval(co.out, args[argi]);
                }
                argi++;
            } else {
                co.out << c;
            }
        }
    } 
    return valp(new NilValue());
}

StdLib::StdLib() {
    add("printf", valp(new NativePrint()));
}

void interpret(ostream &out, block b) {
    InterpreterContext ic(out);
    ic.evalBlock(b);
}