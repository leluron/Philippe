#pragma once

#include <memory>
#include <vector>
#include <ostream>
#include <map>

class Exp;
class Stat;
class Lexp;
class Type;
class Def;

using expp = std::shared_ptr<Exp>;
using expl = std::vector<expp>;
using statp = std::shared_ptr<Stat>;
using block = std::vector<statp>;

using lexpp = std::shared_ptr<Lexp>;
using typep = std::shared_ptr<Type>;
using defp = std::shared_ptr<Def>;

using id = std::string;
using arglist = std::vector<id>;

class Def {
public:
    virtual ~Def() {}
};

class Global {
public:
    typep type;
    expp value;
};

struct Arg {
    std::string name;
    typep type;
};

class ObjDef {
public:
    std::vector<Arg> fields;
};

class File {
public:
    std::map<std::string, Global> globals;
    std::map<std::string, ObjDef> objectDefinitions;
    std::map<std::string, defp> functions;
    std::map<std::string, typep> aliases;
};

class FunctionDef : public Def {
public:
    FunctionDef(std::vector<Arg> args,
        typep ret, block body) : args(args),
        ret(ret), body(body) {}
    std::vector<Arg> args;
    typep ret;
    block body;
};

class NativeFunction {};

class NativeFunctionDef : public Def {
public:
    NativeFunctionDef(std::vector<typep> args, typep ret, std::shared_ptr<NativeFunction> func) : args(args),
        ret(ret), func(func) {}

    std::vector<typep> args;
    typep ret;
    std::shared_ptr<NativeFunction> func;
};


class Stat {
public:
    virtual ~Stat() {}
};


class AssignStat : public Stat {
public:
    
    AssignStat(lexpp left, expp right) {
        this->left = left;
        this->right = right;
    }
    lexpp left;
    expp right;
};


class FuncCallStat : public Stat {
public:
    
    FuncCallStat(lexpp func, expl args) {
        this->func = func;
        this->args = args;
    }
    lexpp func;
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
    
    ObjExp(std::string name, std::vector<FieldDef> fields) {
        this->fields = fields;
    }
    std::string name;
    std::vector<FieldDef> fields;

};

class ListExp : public Exp {
public:
    
    ListExp(expl elements) {
        this->elements = elements;
    }
    expl elements;

};

class TupleExp : public Exp {
public:
    
    TupleExp(expl elements) {
        this->elements = elements;
    }
    expl elements;

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
    
    CallExp(lexpp func, expl args) {
        this->func = func;
        this->args = args;
    }
    lexpp func;
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

enum class BinOp {Mul, Div, Mod, Plus, Minus, Lteq, Lt, Eq, And, Or};

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

class CastExp : public Exp {
public:
    CastExp(expp e, typep t) : e(e), t(t) {}
    expp e;
    typep t;
};


class Lexpsuffix {
public:
    virtual ~Lexpsuffix() {}
};

class IndexSuffix : public Lexpsuffix {
public:
    IndexSuffix(expp i) : i(i) {}
    expp i;
};

class MemberSuffix : public Lexpsuffix {
public:
    MemberSuffix(std::string name) : name(name) {}
    std::string name;
};

class Lexp {
public:
    Lexp(std::string name, std::vector<std::shared_ptr<Lexpsuffix>> suffixes,
        std::shared_ptr<Type> type) : name(name), suffixes(suffixes), type(type) {}
    std::string name;
    std::vector<std::shared_ptr<Lexpsuffix>> suffixes;
    std::shared_ptr<Type> type;
};

class Type {
public:
    virtual ~Type() {}
};

class TypeInt : public Type {};
class TypeFloat : public Type {};
class TypeBool : public Type {};
class TypeString : public Type {};
class TypeNil : public Type {};
class TypeTuple : public Type {
public: std::vector<typep> t;
    TypeTuple(std::vector<typep> t) : t(t) {}
};
class TypeObj : public Type {
public: std::string name;
    TypeObj(std::string name) : name(name) {}
};
class TypeFunction: public Type {
public: std::vector<typep> args; typep ret;
    TypeFunction(std::vector<typep> args, typep ret) : args(args), ret(ret) {}
};
class TypeList: public Type {
public :typep t;
    TypeList(typep t) : t(t) {}
};
class TypeVariable : public Type {};