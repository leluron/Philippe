#pragma once

#include <memory>
#include <vector>

struct Exp;
struct Stat;

using expp = std::shared_ptr<Exp>;
using expl = std::vector<expp>;
using statp = std::shared_ptr<Stat>;
using block = std::vector<statp>;

using id = std::string;
using arglist = std::vector<id>;


struct Stat {

};


struct AssignStat : public Stat {
    expl left;
    expp right;
};


struct FuncCallStat : public Stat {
    expp func;
    expp args;
};


struct WhileStat : public Stat {
    expp cond;
    statp body;
};


struct IfStat : public Stat {
    expp cond;
    statp thenbody;
    statp elsebody;
};


struct ForStat : public Stat {
    id name;
    expp list;
    statp body;
};


struct BlockStat : public Stat {
    block stats;
};


struct Exp {

};


struct NilExp : public Exp {

};


struct BoolExp : public Exp {
    bool val;
};

struct IntExp : public Exp {
    long val;
};

struct FloatExp : public Exp {
    double val;
};

struct StringExp : public Exp {
    std::string val;
};

struct IdExp : public Exp {
    id name;
};

struct FieldDef {
    id name;
    expp e;
};

struct ObjExp : public Exp {
    std::vector<FieldDef> fields;
};

struct ListExp : public Exp {
    expl elements;
};

struct RangeExp : public Exp {
    expp begin, end;
};

struct TupleExp : public Exp {
    expl elements;
};

struct FuncBody {
    block stats;
    expp ret;
};

struct FuncExp : public Exp {
    arglist args;
    FuncBody body;
};

struct MemberExp : public Exp {
    expp left;
    id name;
};

struct IndexExp : public Exp {
    expp left, index;
};

struct CallExp : public Exp {
    expp func;
    expl args;
};

struct TernaryExp : public Exp {
    expp then, cond, els;
};
