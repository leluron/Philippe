#pragma once

#include <memory>
#include <vector>
#include <ostream>

class Exp;
class Stat;

using expp = std::shared_ptr<Exp>;
using expl = std::vector<expp>;
using statp = std::shared_ptr<Stat>;
using block = std::vector<statp>;

using id = std::string;
using arglist = std::vector<id>;

class InterpreterContext;

class Value;
using valp = std::shared_ptr<Value>;

class Stat {
public:
    virtual void print(std::ostream &out, int indent) {}
    virtual void eval(InterpreterContext &c) {}
};


class AssignStat : public Stat {
public:
    AssignStat(expl left, expp right) {
        this->left = left;
        this->right = right;
    }
    expl left;
    expp right;
    virtual void print(std::ostream &out, int indent) override;
    virtual void eval(InterpreterContext &c) override;
};


class FuncCallStat : public Stat {
public:
    FuncCallStat(expp func, expl args) {
        this->func = func;
        this->args = args;
    }
    expp func;
    expl args;
    virtual void print(std::ostream &out, int indent) override;
    virtual void eval(InterpreterContext &c) override;
};


class WhileStat : public Stat {
public:
    WhileStat(expp cond, statp body) {
        this->cond = cond;
        this->body = body;
    }
    expp cond;
    statp body;

    virtual void print(std::ostream &out, int indent) override;
    virtual void eval(InterpreterContext &c) override;
};


class IfStat : public Stat {
public:
    IfStat(expp cond, statp thenbody, statp elsebody) {
        this->cond = cond;
        this->thenbody = thenbody;
        this->elsebody = elsebody;
    }
    expp cond;
    statp thenbody;
    statp elsebody;

    virtual void print(std::ostream &out, int indent) override;
    virtual void eval(InterpreterContext &c) override;
};


class ForStat : public Stat {
public:
    ForStat(id name, expp list, statp body) {
        this->name = name;
        this->list = list;
        this->body = body;
    }
    id name;
    expp list;
    statp body;

    virtual void print(std::ostream &out, int indent) override;
    virtual void eval(InterpreterContext &c) override;
};


class BlockStat : public Stat {
public:
    BlockStat(block stats) {
        this->stats = stats;
    }
    block stats;

    virtual void print(std::ostream &out, int indent) override;
    virtual void eval(InterpreterContext &c) override;
};

class BreakStat : public Stat {
public:
    virtual void print(std::ostream &out, int indent) override;
    virtual void eval(InterpreterContext &c) override;
};

class ReturnStat : public Stat {
public:
    ReturnStat(expp ret) {
        this->ret = ret;
    }
    expp ret;

    virtual void print(std::ostream &out, int indent) override;
    virtual void eval(InterpreterContext &c) override;
};

class EmptyStat : public Stat {
    virtual void print(std::ostream &out, int indent) override;
    virtual void eval(InterpreterContext &c) override;
};

class Exp {
public:
    virtual void print(std::ostream &out, int indent) {}
    virtual valp eval(InterpreterContext &c) = 0;
    virtual valp& leval(InterpreterContext &c) = 0;
};


class NilExp : public Exp {
public:
    virtual void print(std::ostream &out, int indent) override;
    virtual valp eval(InterpreterContext &c) override;
    virtual valp& leval(InterpreterContext &c) override;
};


class BoolExp : public Exp {
public:
    BoolExp(bool val) {
        this->val = val;
    }
    bool val;

    virtual void print(std::ostream &out, int indent) override;
    virtual valp eval(InterpreterContext &c) override;
    virtual valp& leval(InterpreterContext &c) override;
};

class IntExp : public Exp {
public:
    IntExp(long val) {
        this->val = val;        
    }
    long val;

    virtual void print(std::ostream &out, int indent) override;
    virtual valp eval(InterpreterContext &c) override;
    virtual valp& leval(InterpreterContext &c) override;
};

class FloatExp : public Exp {
public:
    FloatExp(double val) {
        this->val = val;
    }
    double val;

    virtual void print(std::ostream &out, int indent) override;
    virtual valp eval(InterpreterContext &c) override;
    virtual valp& leval(InterpreterContext &c) override;
};

class StringExp : public Exp {
public:
    StringExp(std::string val) {
        this->val = val;
    }
    std::string val;

    virtual void print(std::ostream &out, int indent) override;
    virtual valp eval(InterpreterContext &c) override;
    virtual valp& leval(InterpreterContext &c) override;
};

class IdExp : public Exp {
public:
    IdExp(id name) {
        this->name = name;
    }
    id name;

    virtual void print(std::ostream &out, int indent) override;
    virtual valp eval(InterpreterContext &c) override;
    virtual valp& leval(InterpreterContext &c) override;
};

class FieldDef {
public:
    FieldDef() = default;
    FieldDef(id name, expp e) {
        this->name = name;
        this->e = e;
    }
    id name;
    expp e;
};

class ObjExp : public Exp {
public:
    ObjExp(std::vector<FieldDef> fields) {
        this->fields = fields;
    }
    std::vector<FieldDef> fields;

    virtual void print(std::ostream &out, int indent) override;
    virtual valp eval(InterpreterContext &c) override;
    virtual valp& leval(InterpreterContext &c) override;
};

class ListExp : public Exp {
public:
    ListExp(expl elements) {
        this->elements = elements;
    }
    expl elements;

    virtual void print(std::ostream &out, int indent) override;
    virtual valp eval(InterpreterContext &c) override;
    virtual valp& leval(InterpreterContext &c) override;
};

class RangeExp : public Exp {
public:
    RangeExp(expp begin, expp end) {
        this->begin = begin;
        this->end = end;
    }
    expp begin, end;

    virtual void print(std::ostream &out, int indent) override;
    virtual valp eval(InterpreterContext &c) override;
    virtual valp& leval(InterpreterContext &c) override;
};

class TupleExp : public Exp {
public:
    TupleExp(expl elements) {
        this->elements = elements;
    }
    expl elements;

    virtual void print(std::ostream &out, int indent) override;
    virtual valp eval(InterpreterContext &c) override;
    virtual valp& leval(InterpreterContext &c) override;
};

class FuncBody {
public:
    FuncBody() = default;
    FuncBody(block stats, expp ret) {
        this->stats = stats;
        this->ret = ret;
    }
    block stats;
    expp ret;
};

class FuncExp : public Exp {
public:
    FuncExp(arglist args, FuncBody body) {
        this->args = args;
        this->body = body;
    }
    arglist args;
    FuncBody body;

    virtual void print(std::ostream &out, int indent) override;
    virtual valp eval(InterpreterContext &c) override;
    virtual valp& leval(InterpreterContext &c) override;
};

class MemberExp : public Exp {
public:
    MemberExp(expp left, id name) {
        this->left = left;
        this->name = name;
    }
    expp left;
    id name;

    virtual void print(std::ostream &out, int indent) override;
    virtual valp eval(InterpreterContext &c) override;
    virtual valp& leval(InterpreterContext &c) override;
};

class IndexExp : public Exp {
public:
    IndexExp(expp left, expp index) {
        this->left = left;
        this->index = index;
    }
    expp left, index;

    virtual void print(std::ostream &out, int indent) override;
    virtual valp eval(InterpreterContext &c) override;
    virtual valp& leval(InterpreterContext &c) override;
};

class CallExp : public Exp {
public:
    CallExp(expp func, expl args) {
        this->func = func;
        this->args = args;
    }
    expp func;
    expl args;

    virtual void print(std::ostream &out, int indent) override;
    virtual valp eval(InterpreterContext &c) override;
    virtual valp& leval(InterpreterContext &c) override;
};

enum class UnaryOp {Minus, Not};

class UnaryOpExp : public Exp {
public:
    UnaryOpExp(UnaryOp op, expp e) {
        this->op = op;
        this->e = e;
    }
    UnaryOp op;
    expp e;

    virtual void print(std::ostream &out, int indent) override;
    virtual valp eval(InterpreterContext &c) override;
    virtual valp& leval(InterpreterContext &c) override;
};

enum class BinOp {Pow, Mul, Div, Mod, Plus, Minus, Concat, Lteq, Lt, Gt, Gteq, Eq, Neq, And, Or};

class BinOpExp : public Exp {
public:
    BinOpExp(BinOp op, expp left, expp right) {
        this->op = op;
        this->left = left;
        this->right = right;
    }
    BinOp op;
    expp left, right;

    virtual void print(std::ostream &out, int indent) override;
    virtual valp eval(InterpreterContext &c) override;
    virtual valp& leval(InterpreterContext &c) override;
};

class TernaryExp : public Exp {
public:
    TernaryExp(expp then, expp cond, expp els) {
        this->then = then;
        this->cond = cond;
        this->els = els;
    }
    expp then, cond, els;

    virtual void print(std::ostream &out, int indent) override;
    virtual valp eval(InterpreterContext &c) override;
    virtual valp& leval(InterpreterContext &c) override;
};
