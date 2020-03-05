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

class Stat {
public:
    virtual ~Stat() {}
};


class AssignStat : public Stat {
public:
    
    AssignStat(expl left, expp right) {
        this->left = left;
        this->right = right;
    }
    expl left;
    expp right;
};


class FuncCallStat : public Stat {
public:
    
    FuncCallStat(expp func, expl args) {
        this->func = func;
        this->args = args;
    }
    expp func;
    expl args;
};


class WhileStat : public Stat {
public:
    
    WhileStat(expp cond, statp body) {
        this->cond = cond;
        this->body = body;
    }
    expp cond;
    statp body;

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

};


class BlockStat : public Stat {
public:
    
    BlockStat(block stats) {
        this->stats = stats;
    }
    block stats;

};

class BreakStat : public Stat {
public:
    
};

class ReturnStat : public Stat {
public:
    
    ReturnStat(expp ret) {
        this->ret = ret;
    }
    expp ret;

};

class EmptyStat : public Stat {
    
};

class Exp {
public:
    virtual ~Exp() {}

};


class NilExp : public Exp {
public:
    
};


class BoolExp : public Exp {
public:
    
    BoolExp(bool val) {
        this->val = val;
    }
    bool val;

};

class IntExp : public Exp {
public:
    
    IntExp(long val) {
        this->val = val;        
    }
    long val;

};

class FloatExp : public Exp {
public:
    
    FloatExp(double val) {
        this->val = val;
    }
    double val;

};

class StringExp : public Exp {
public:
    
    StringExp(std::string val) {
        this->val = val;
    }
    std::string val;

};

class IdExp : public Exp {
public:
    
    IdExp(id name) {
        this->name = name;
    }
    id name;

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

};

class ListExp : public Exp {
public:
    
    ListExp(expl elements) {
        this->elements = elements;
    }
    expl elements;

};

class RangeExp : public Exp {
public:
    
    RangeExp(expp begin, expp end) {
        this->begin = begin;
        this->end = end;
    }
    expp begin, end;

};

class TupleExp : public Exp {
public:
    
    TupleExp(expl elements) {
        this->elements = elements;
    }
    expl elements;

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

};

class MemberExp : public Exp {
public:
    
    MemberExp(expp left, id name) {
        this->left = left;
        this->name = name;
    }
    expp left;
    id name;

};

class IndexExp : public Exp {
public:
    
    IndexExp(expp left, expp index) {
        this->left = left;
        this->index = index;
    }
    expp left, index;

};

class CallExp : public Exp {
public:
    
    CallExp(expp func, expl args) {
        this->func = func;
        this->args = args;
    }
    expp func;
    expl args;

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

};

class TernaryExp : public Exp {
public:
    
    TernaryExp(expp then, expp cond, expp els) {
        this->then = then;
        this->cond = cond;
        this->els = els;
    }
    expp then, cond, els;

};
