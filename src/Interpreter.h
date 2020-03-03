#pragma once

#include <map>
#include <stack>
#include <exception>

void evalBlock(InterpreterContext &c, block b);

enum class ValueKind {
    Nil, Bool, Int, Float, String, Obj, List, Range, Tuple, Func, NativeFunc
};

class Value {
public:
    virtual ValueKind kind() const = 0;
    virtual valp unaryOp(UnaryOp op) {
        throw "Unsupported unary op";
    }
    virtual valp binOp(BinOp op, valp right) {
        throw "Unsupported binary op";
    }
};


class NilValue : public Value {
public:
    virtual ValueKind kind() const {
        return ValueKind::Nil;
    }
};

class BoolValue : public Value {
public:
    BoolValue(bool val) : val(val) {}
    bool val;
    virtual ValueKind kind() const {
        return ValueKind::Bool;
    }
    virtual valp unaryOp(UnaryOp op) override {
        if (op == UnaryOp::Not) {
            return valp(new BoolValue(!val));
        }
        throw "Unsupported bool operation";
    }
    virtual valp binOp(BinOp op, valp right) override {
        if (right->kind() != ValueKind::Bool) throw "Unsupported bool operation";
        auto v1 = dynamic_cast<BoolValue*>(right.get())->val;
        if (op == BinOp::Eq) return valp(new BoolValue(val == v1));
        else if (op == BinOp::Neq) return valp(new BoolValue(val != v1));
        else if (op == BinOp::And) return valp(new BoolValue(val && v1));
        else if (op == BinOp::Or) return valp(new BoolValue(val || v1));
        else throw "Unsupported bool operation";
    }
};
class IntValue : public Value {
public:
    IntValue(long val) : val(val) {}
    long val;
    virtual ValueKind kind() const {
        return ValueKind::Int;
    }
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
    virtual ValueKind kind() const {
        return ValueKind::Float;
    }
    virtual valp unaryOp(UnaryOp op) override {
        if (op == UnaryOp::Minus)
            return valp(new FloatValue(-val));
        throw;
    }
    virtual valp binOp(BinOp op, valp right) override;
};
class StringValue : public Value {
public:
    StringValue(std::string val) : val(val) {}
    std::string val;
    virtual ValueKind kind() const {
        return ValueKind::String;
    }
    virtual valp binOp(BinOp op, valp right) override {
        if (op == BinOp::Concat && 
            right->kind() == ValueKind::String) {
            auto r = dynamic_cast<StringValue*>(right.get())->val;
            return valp(new StringValue(val + r));
        }
        throw;
    }
};
class ObjValue : public Value {
public:
    ObjValue() {}
    void add(std::string name, valp val) {
        fields[name] = val;
    }
    valp& get(std::string name) {
        auto f = fields.find(name);
        if (f == fields.end()) throw "test";
        return f->second;
    }
    std::map<std::string, valp> fields;
    virtual ValueKind kind() const {
        return ValueKind::Obj;
    }
};
class ListValue : public Value {
public:
    ListValue(std::vector<valp> e) : elements(e) {}
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
    std::vector<valp> elements;
    virtual ValueKind kind() const {
        return ValueKind::List;
    }
};
class RangeValue : public Value {
public:
    RangeValue(IntValue beg, IntValue end) : beg(beg), end(end) {}
    IntValue beg, end;
    virtual ValueKind kind() const {
        return ValueKind::Range;
    }
};
class TupleValue : public Value {
public:
    TupleValue(std::vector<valp> e) : elements(e) {}
    valp& get(int index) {
        if (index >= elements.size()) throw;
        return elements[index];
    }
    std::vector<valp> elements;
    virtual ValueKind kind() const {
        return ValueKind::Tuple;
    }
};
class FuncValue : public Value {
public:
    FuncValue(FuncExp f) : func(f) {}
    FuncExp func;
    virtual ValueKind kind() const {
        return ValueKind::Func;
    }
};

class InterpreterContext;

class NativeFuncValue : public Value {
public:
    virtual ValueKind kind() const {
        return ValueKind::NativeFunc;
    }
    virtual valp exec(InterpreterContext &c, std::vector<valp> args)=0;
};

class NativePrint : public NativeFuncValue {
public:
    virtual valp exec(InterpreterContext &c, std::vector<valp> args) override;
};

class StdLib : public ObjValue {
public:
    StdLib();
};


class InterpreterContext {
public:
    InterpreterContext(std::ostream &out) : out(out) {
        vars.push_back(StdLib());
    }
    valp& get(std::string name) {
        for (int i=vars.size()-1;i>=0;i--) {
            try {
                return vars[i].get(name);
            } catch (...) {
                continue;
            }
        }
        throw;
    }
    valp& getLeftValue(std::string name) {
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
    void add(std::string name, valp val) {
        vars.back().add(name, val);
    }

    std::ostream &out;
    std::vector<ObjValue> vars;
    bool broke = false;
    bool returned = false;
    valp returnval = nullptr;
};