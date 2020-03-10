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


class ObjDef {
public:
    std::map<std::string, int> fields;
    std::shared_ptr<TypeTuple> type;
};

struct Arg {
    std::string name;
    typep type;
};

class FunctionDef {
public:
    FunctionDef(std::vector<Arg> args,
        typep ret, block body) : args(args),
        ret(ret), body(body) {}
    std::vector<Arg> args;
    typep ret;
    block body;
};

class File {
public:
    std::map<std::string, ObjDef> objectDefinitions;
    std::map<std::string, FunctionDef> functions;
};

class Lexpsuffix {
public:
    virtual ~Lexpsuffix() {}
};

class ListIndexSuffix : public Lexpsuffix {
public:
    ListIndexSuffix(expp i) : i(i) {}
    expp i;
};

class TupleAccessSuffix : public Lexpsuffix {
public:
    TupleAccessSuffix(int i) : i(i) {}
    int i;
};

class Lexp {
public:
    Lexp(std::string name, std::vector<std::shared_ptr<Lexpsuffix>> suffixes,
        std::shared_ptr<Type> type) : name(name), suffixes(suffixes), type(type) {}
    std::string name;
    std::vector<std::shared_ptr<Lexpsuffix>> suffixes;
    std::shared_ptr<Type> type;
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
    typep type = nullptr;
};

class NilExp : public Exp {
public:
    NilExp() {type = typep(new TypeNil()); }
};


class BoolExp : public Exp {
public:
    
    BoolExp(bool val) {
        type = typep(new TypeBool());
        this->val = val;
    }
    bool val;

};

class IntExp : public Exp {
public:
    
    IntExp(long val) {
        type = typep(new TypeInt());
        this->val = val;        
    }
    long val;

};

class FloatExp : public Exp {
public:
    
    FloatExp(double val) {
        type = typep(new TypeFloat());
        this->val = val;
    }
    double val;

};

class StringExp : public Exp {
public:
    
    StringExp(std::string val) {
        type = typep(new TypeString());
        this->val = val;
    }
    std::string val;

};

class IdExp : public Exp {
public:
    
    IdExp(typep t, id name) {
        this->type = t;
        this->name = name;
    }
    IdExp(id name) {
        this->type = nullptr;
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

class ListExp : public Exp {
public:
    
    ListExp(expl elements) {
        // TODO more checking
        if (elements.size() == 0) type = typep(new TypeList(nullptr));
        else type = typep(new TypeList(elements[0]->type));
        this->elements = elements;
    }
    expl elements;

};

class TupleExp : public Exp {
public:
    
    TupleExp(expl elements) {
        std::vector<typep> types;
        for (auto e : elements) {
            types.push_back(e->type);
        }
        this->elements = elements;
        this->type = typep(new TypeTuple(types));
    }
    TupleExp(expl elements, std::string obj) {
        this->elements = elements;
        this->type = typep(new TypeObj(obj));
    }
    expl elements;

};

class IndexExp : public Exp {
public:
    
    IndexExp(expp left, expp index) {
        if (!std::dynamic_pointer_cast<TypeInt>(index->type)) throw "index should be an int";
        if (auto li = std::dynamic_pointer_cast<TypeList>(left->type)) {
            this->type = li->t;
        } else throw std::runtime_error("left isn't a list");
        this->left = left;
        this->index = index;
    }
    expp left, index;
};

class TupleAccessExp : public Exp {
public:
    TupleAccessExp(expp left,int index) {
        if (auto li = std::dynamic_pointer_cast<TypeTuple>(left->type)) {
            this->type = li->t[index];
        } else throw std::runtime_error("left isn't a list");
        this->left = left;
        this->index = index;
    }
    TupleAccessExp(expp left,int index, typep t) {
        this->type = t;
        this->left = left;
        this->index = index;
    }
    expp left;
    int index;
};

class CallExp : public Exp {
public:
    CallExp(lexpp func, expl args) {
        this->func = func;
        this->args = args;
        this->type = func->type;
    }
    lexpp func;
    expl args;

};

class TernaryExp : public Exp {
public:
    
    TernaryExp(expp then, expp cond, expp els) {
        if (!std::dynamic_pointer_cast<TypeBool>(cond->type))
            throw std::runtime_error("can't evaluate a non-bool");
        this->then = then;
        this->cond = cond;
        this->els = els;
        this->type = then->type;
    }
    expp then, cond, els;

};

class CastExp : public Exp {
public:
    CastExp(expp e, typep t) : e(e) {
        this->type = t;
    }
    expp e;
};