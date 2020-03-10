#include "ASTGen.h"

#include <algorithm>

using namespace std;

bool typesEqual(typep a, typep b);

File ASTGen::gen(PhilippeParser::FileContext *ctx) {
    newSymbolFrame();
    loadStd();
    visit(ctx);
    try {
        //inferTypes();
    } catch (runtime_error e) {
        cout << e.what() << endl;
    }
    popSymbolFrame();
    return ast;
}

void ASTGen::loadStd() {
    newSymbol("printf", typep(
        new TypeFunction(
            {
                typep(new TypeString()),
                typep(new TypeInt())
            },
            typep(new TypeNil())
        ))
    );
    newSymbol("__add", typep(
        new TypeFunction(
            {
                typep(new TypeInt()),
                typep(new TypeInt())
            },
            typep(new TypeInt())
        ))
    );
    newSymbol("__sub", typep(
        new TypeFunction(
            {
                typep(new TypeInt()),
                typep(new TypeInt())
            },
            typep(new TypeInt())
        ))
    );
    newSymbol("__mul", typep(
        new TypeFunction(
            {
                typep(new TypeInt()),
                typep(new TypeInt())
            },
            typep(new TypeInt())
        ))
    );
    newSymbol("__div", typep(
        new TypeFunction(
            {
                typep(new TypeInt()),
                typep(new TypeInt())
            },
            typep(new TypeInt())
        ))
    );
    newSymbol("__mod", typep(
        new TypeFunction(
            {
                typep(new TypeInt()),
                typep(new TypeInt())
            },
            typep(new TypeInt())
        ))
    );
    newSymbol("__lt", typep(
        new TypeFunction(
            {
                typep(new TypeInt()),
                typep(new TypeInt())
            },
            typep(new TypeBool())
        ))
    );
    newSymbol("__lteq", typep(
        new TypeFunction(
            {
                typep(new TypeInt()),
                typep(new TypeInt())
            },
            typep(new TypeBool())
        ))
    );
    newSymbol("__eq", typep(
        new TypeFunction(
            {
                typep(new TypeInt()),
                typep(new TypeInt())
            },
            typep(new TypeBool())
        ))
    );
    newSymbol("__neq", typep(
        new TypeFunction(
            {
                typep(new TypeInt()),
                typep(new TypeInt())
            },
            typep(new TypeBool())
        ))
    );
    newSymbol("__and", typep(
        new TypeFunction(
            {
                typep(new TypeBool()),
                typep(new TypeBool())
            },
            typep(new TypeBool())
        ))
    );
    newSymbol("__or", typep(
        new TypeFunction(
            {
                typep(new TypeBool()),
                typep(new TypeBool())
            },
            typep(new TypeBool())
        ))
    );
    newSymbol("__usub", typep(
        new TypeFunction(
            {
                typep(new TypeInt()),
            },
            typep(new TypeBool())
        ))
    );
    newSymbol("__not", typep(
        new TypeFunction(
            {
                typep(new TypeInt()),
            },
            typep(new TypeBool())
        ))
    );
}

void ASTGen::newSymbolFrame() {
    symbols.push_back({});
}
void ASTGen::popSymbolFrame() {
    symbols.pop_back();
}

typep ASTGen::getSymbol(string name) {
    for (int i = symbols.size()-1; i>=0;i--) {
        auto it = symbols.at(i).find(name);
        if (it != symbols.at(i).end()) return it->second;
    }
    return nullptr;
}

void ASTGen::newSymbol(string name, typep t) {
    symbols.back().insert({name, t});
}

antlrcpp::Any ASTGen::visitFile(PhilippeParser::FileContext *ctx) {
    for (auto d : ctx->def()) {
        visit(d);
    }
    return nullptr;
}

antlrcpp::Any ASTGen::visitFunctiondef(PhilippeParser::FunctiondefContext *ctx) {
    auto name = ctx->ID()->getText();
    if (getSymbol(name)) throw runtime_error("Already used name");

    typep ret;
    if (ctx->type()) ret = visit(ctx->type());
    else ret = typep(new TypeNil());

    vector<Arg> args;
    vector<typep> argst;
    for (auto a : ctx->arg()) {
        Arg a0 = visit(a);
        args.push_back(a0);
        argst.push_back(a0.type);
    }

    newSymbol(name, typep(new TypeFunction(argst, ret)));
    newSymbolFrame();
    for (auto a : args) {
        newSymbol(a.name, a.type);
    }

    toReturn = ret;

    block b;
    for (auto s : ctx->stat()) {
        statp s1 = visit(s);
        if(auto b1 = dynamic_pointer_cast<BlockStat>(s1))
            b.insert(b.end(), b1->stats.begin(), b1->stats.end());
        else
            b.push_back(s1);
    }
    ast.functions.insert({name, {args, ret, b}});
    popSymbolFrame();
    return nullptr;
}

antlrcpp::Any ASTGen::visitObjdef(PhilippeParser::ObjdefContext *ctx) {
    vector<Arg> args;
    vector<typep> types;
    map<string, int> fields;
    for (int i=0;i<ctx->arg().size();i++) {
        Arg val = visit(ctx->arg(i));
        args.push_back(val);
        types.push_back(val.type);
        fields[val.name] = i;
    }

    auto name = ctx->ID()->getText();
    auto it = ast.objectDefinitions.find(name);
    if (it == ast.objectDefinitions.end())
        ast.objectDefinitions.insert({name, {fields, shared_ptr<TypeTuple>(new TypeTuple(types))}});
    else throw runtime_error("can't have objects with the same name");
    return nullptr;
}

antlrcpp::Any ASTGen::visitBreakstat(PhilippeParser::BreakstatContext *ctx) {
    return statp(new BreakStat());
}

antlrcpp::Any ASTGen::visitBlockstat(PhilippeParser::BlockstatContext *ctx) {
    block b;
    for (auto s : ctx->stat()) {
        statp s1 = visit(s);
        if(auto b1 = dynamic_pointer_cast<BlockStat>(s1))
            b.insert(b.end(), b1->stats.begin(), b1->stats.end());
        else
            b.push_back(s1);
    }
    return statp(new BlockStat(b));
}

expp ASTGen::exp_option(PhilippeParser::ExpContext *e) {
    if (!e) return expp(new NilExp());
    else return visit(e).as<expp>();
}

antlrcpp::Any ASTGen::visitReturnstat(PhilippeParser::ReturnstatContext *ctx) {
    auto ret = exp_option(ctx->exp());
    if (!typesEqual(ret->type, toReturn)) throw runtime_error("Can't return this type");
    return statp(new ReturnStat(ret));
}

antlrcpp::Any ASTGen::visitStdassign(PhilippeParser::StdassignContext *ctx) {
    expp e = visit(ctx->exp());
    if (ctx->lexpopttype().size() == 1) {
        lexpp lexp = visit(ctx->lexpopttype(0));
        if (!lexp->type) {
            lexp->type = e->type;
            newSymbol(lexp->name, lexp->type);
        } else {
            if (!typesEqual(lexp->type, e->type))
                throw runtime_error("Can't assign with different types");
        }
        return statp(new AssignStat(lexp, e));
    } else {
        if (auto tu = dynamic_pointer_cast<TypeTuple>(e->type)) {
            block out;
            auto tmp = newtmp();
            out.push_back(statp(new AssignStat(lexpp(new Lexp(tmp, {}, e->type)), e)));
            for (int i=0;i<ctx->lexpopttype().size();i++) {
                lexpp lexp = visit(ctx->lexpopttype(i));
                lexp->type = tu->t[i];
                if (getSymbol(lexp->name)) throw runtime_error("Multiple assignment should happen on new variables");
                newSymbol(lexp->name, lexp->type);
                out.push_back(statp(new AssignStat(
                    lexp, 
                    expp(
                        new IndexExp(
                            expp(new IdExp(lexp->type, tmp)),
                            expp(new IntExp(i))
                        )
                    )
                )));
            }
            return statp(new BlockStat(out));
        } else throw runtime_error("Can't do multiple assignment on non-tuples");
    }
}

expp ASTGen::toRvalue(lexpp l) {
    typep t = getSymbol(l->name);
    if (!t) throw runtime_error("Can't find variable");
    expp b = expp(new IdExp(t, l->name));
    for (auto s : l->suffixes) {
        if (auto ind = dynamic_pointer_cast<ListIndexSuffix>(s)) {
            b = expp(new IndexExp(b, ind->i));
        } else if (auto mem = dynamic_pointer_cast<TupleAccessSuffix>(s)) {
            b = expp(new TupleAccessExp(b, mem->i));
        } else throw;
    }
    return b;

}

antlrcpp::Any ASTGen::visitCompoundassign(PhilippeParser::CompoundassignContext *ctx) {
    auto op = ctx->op->getText();
    lexpp left = visit(ctx->lexpopttype());
    if (left->type == nullptr) throw runtime_error("Can't compound assign on new variables");
    expp right = visit(ctx->exp());
    auto ll = toRvalue(left);
    string funcname;
    switch (op[0]) {
        case '+': funcname = "__add"; break;
        case '-': funcname = "__sub"; break;
        case '*': funcname = "__mul"; break;
        case '/': funcname = "__div"; break;
    }
    typep t = getSymbol(funcname);
    lexpp lexp = idToLexp(funcname);
    lexp->type = t;
    expl args = {ll, right};

    typep ret = validateFuncCall(lexp, args);
    Exp *e = new CallExp(lexp, args);
    e->type = ret;

    return statp(new AssignStat(left, expp(e)));
}

antlrcpp::Any ASTGen::visitFunccall(PhilippeParser::FunccallContext *ctx) {
    lexpp f = visit(ctx->lexp());
    expl args = visit(ctx->explist());
    validateFuncCall(f, args);
    return statp(new FuncCallStat(f, args));
}

typep ASTGen::validateFuncCall(lexpp f, expl args) {
    if (auto f0 = dynamic_pointer_cast<TypeFunction>(f->type)) {
        if (args.size() != f0->args.size())
            throw runtime_error("Not the same number of arguments");
        for (int i=0;i<args.size();i++) {
            if (!typesEqual(args[i]->type, f0->args[i]))
                throw runtime_error("Argument types don't match");
        }
        return f0->ret;
    } else throw runtime_error("Can't call non-function");
}

antlrcpp::Any ASTGen::visitWhilestat(PhilippeParser::WhilestatContext *ctx) {
    expp cond = visit(ctx->exp());

    if (!dynamic_pointer_cast<TypeBool>(cond->type))
        throw runtime_error("Can't evaluate non-bool in while statement"); 

    return statp(new WhileStat(
        cond,
        statp(visit(ctx->stat()).as<statp>())));
}

statp ASTGen::stat_option(PhilippeParser::StatContext *s) {
    if (!s) return statp(new BlockStat({}));
    else return visit(s);
}

statp ASTGen::visitIfAux(PhilippeParser::IfstatContext *ctx, int index) {
    expp e = visit(ctx->exp(index));
    if (!dynamic_pointer_cast<TypeBool>(e->type))
        throw runtime_error("Can't evaluate non-bool in if statement"); 

    statp s = visit(ctx->stat(index));
    if (index == ctx->exp().size()) {
        statp els = stat_option(ctx->els);
        return statp(new IfStat(e,s,els));
    } else {
        return statp(new IfStat(e,s,visitIfAux(ctx, index+1)));
    }
}

antlrcpp::Any ASTGen::visitIfstat(PhilippeParser::IfstatContext *ctx) {
    return visitIfAux(ctx, 0);
}

antlrcpp::Any ASTGen::visitForstat(PhilippeParser::ForstatContext *ctx) {
    return visitChildren(ctx); // To implement
}

antlrcpp::Any ASTGen::visitForexp(PhilippeParser::ForexpContext *ctx) {
    expl l;
    for (auto e : ctx->exp()) {
        l.push_back(visit(e));
    }
    return l;
}

antlrcpp::Any ASTGen::visitArg(PhilippeParser::ArgContext *ctx) {
    return Arg{ctx->ID()->getText(), visit(ctx->type())};
}

antlrcpp::Any ASTGen::visitNilexp(PhilippeParser::NilexpContext *ctx) {
    return expp(new NilExp());
}

antlrcpp::Any ASTGen::visitMemberexp(PhilippeParser::MemberexpContext *ctx) {
    expp l = visit(ctx->exp());

    auto fieldname = ctx->ID()->getText();

    if (auto obj = dynamic_pointer_cast<TypeObj>(l->type)) {
        auto name = obj->name;
        int index = getFieldIndex(name, fieldname);
        auto t = ast.objectDefinitions[name].type->t[index];
        return expp(new TupleAccessExp(l, index, t));
    } else throw runtime_error("can't access member from non-object");
    
}

expp ASTGen::visitBinOp(string op, PhilippeParser::ExpContext *left, PhilippeParser::ExpContext *right) {
    expp l = visit(left);
    expp r = visit(right);
    string funcname;
    expl args;
    if (op=="*") {
        funcname = "__mul"; args = {l,r};
    } else if (op=="/") {
        funcname = "__div"; args = {l,r};
    } else if (op=="%") {
        funcname = "__mod"; args = {l,r};
    } else if (op=="+") {
        funcname = "__add"; args = {l,r};
    } else if (op=="-") {
        funcname = "__sub"; args = {l,r};
    } else if (op=="<=") {
        funcname = "__lteq"; args = {l,r};
    } else if (op=="<") {
        funcname = "__lt"; args = {l,r};
    } else if (op==">") {
        funcname = "__lteq"; args = {r,l};
    } else if (op==">=") {
        funcname = "__lt"; args = {r,l};
    } else if (op=="==") {
        funcname = "__eq"; args = {l,r};
    } else if (op=="!=") {
        funcname = "__neq"; args = {l,r};
    } else if (op=="and") {
        funcname = "__and"; args = {l,r};
    } else if (op=="or") {
        funcname = "__or"; args = {l,r};
    } else throw runtime_error("Unknown binary operation");

    typep t = getSymbol(funcname);
    lexpp lexp = idToLexp(funcname);
    lexp->type = t;

    typep ret = validateFuncCall(lexp, args);
    Exp *e = new CallExp(lexp, args);
    e->type = ret;

    return expp(e);
}

antlrcpp::Any ASTGen::visitUnaryexp(PhilippeParser::UnaryexpContext *ctx) {
    auto op = ctx->op->getText();
    expp e = visit(ctx->exp());
    string funcname;
    if (op == "-") 
        funcname = "__usub";
    else if (op == "not")
        funcname = "__not";
    else throw runtime_error("Unknown unary operation");

    typep t = getSymbol(funcname);
    lexpp lexp = idToLexp(funcname);
    lexp->type = t;
    expl args = {e};

    typep ret = validateFuncCall(lexp, {e});
    Exp *ex = new CallExp(lexp,  {e});
    ex->type = ret;

    return expp(ex);
}

antlrcpp::Any ASTGen::visitOrexp(PhilippeParser::OrexpContext *ctx) {
    return visitBinOp("or", ctx->exp(0), ctx->exp(1));
}

antlrcpp::Any ASTGen::visitAdditiveexp(PhilippeParser::AdditiveexpContext *ctx) {
    return visitBinOp(ctx->op->getText(), ctx->exp(0), ctx->exp(1));
}

antlrcpp::Any ASTGen::visitMultiplicativeexp(PhilippeParser::MultiplicativeexpContext *ctx) {
    return visitBinOp(ctx->op->getText(), ctx->exp(0), ctx->exp(1));
}

antlrcpp::Any ASTGen::visitIdexp(PhilippeParser::IdexpContext *ctx) {
    auto name = ctx->ID()->getText();
    typep t = getSymbol(name);
    if (!t) throw runtime_error("Use of inexistent variable");

    return expp(new IdExp(t, name));
}

antlrcpp::Any ASTGen::visitComparisonexp(PhilippeParser::ComparisonexpContext *ctx) {
    return visitBinOp(ctx->op->getText(), ctx->exp(0), ctx->exp(1));
}

antlrcpp::Any ASTGen::visitParenexp(PhilippeParser::ParenexpContext *ctx) {
    return visit(ctx->exp());
}

antlrcpp::Any ASTGen::visitFalseexp(PhilippeParser::FalseexpContext *ctx) {
    return expp(new BoolExp(false));
}

antlrcpp::Any ASTGen::visitStringexp(PhilippeParser::StringexpContext *ctx) {
    auto str = ctx->STRING()->getText();
    return expp(new StringExp(str.substr(1, str.length()-2)));
}

antlrcpp::Any ASTGen::visitTernaryexp(PhilippeParser::TernaryexpContext *ctx) {
    return expp(new TernaryExp(
        visit(ctx->exp(0)).as<expp>(),
        visit(ctx->exp(1)).as<expp>(),
        visit(ctx->exp(2)).as<expp>()));
}

antlrcpp::Any ASTGen::visitFunccallexp(PhilippeParser::FunccallexpContext *ctx) {
    lexpp f = visit(ctx->lexp());
    expl args = visit(ctx->explist());
    typep ret = validateFuncCall(f, args);
    Exp *e = new CallExp(f, args);
    e->type = ret;
    return expp(e);
}

antlrcpp::Any ASTGen::visitAndexp(PhilippeParser::AndexpContext *ctx) {
    return visitBinOp("and", ctx->exp(0), ctx->exp(1));
}

antlrcpp::Any ASTGen::visitRelationexp(PhilippeParser::RelationexpContext *ctx) {
    return visitBinOp(ctx->op->getText(), ctx->exp(0), ctx->exp(1));
}

antlrcpp::Any ASTGen::visitTrueexp(PhilippeParser::TrueexpContext *ctx) {
    return expp(new BoolExp(true));
}

antlrcpp::Any ASTGen::visitIndexexp(PhilippeParser::IndexexpContext *ctx) {
    expp l = visit(ctx->exp(0));
    expp r = visit(ctx->exp(1));
    return expp(new IndexExp(l, r));
}

antlrcpp::Any ASTGen::visitObjexp(PhilippeParser::ObjexpContext *ctx) {
    auto name = ctx->ID()->getText();

    auto it = ast.objectDefinitions.find(name);
    if (it == ast.objectDefinitions.end()) throw runtime_error("Not an object");
    
    auto indmap = it->second.fields;
    auto t = it->second.type;

    expl values(t->t.size(), nullptr);

    for (auto f : ctx->fielddef()) {
        FieldDef value = visit(f);
        auto it2 = indmap.find(value.name);
        if (it2 == indmap.end()) throw runtime_error("can't find field");
        auto i = it2->second;
        auto e = value.e;
        if (!typesEqual(e->type, t->t[i])) {
            throw runtime_error("field types don't match");
        }
        values[i] = e;
    }
    return expp(new TupleExp(values, name));
}

antlrcpp::Any ASTGen::visitFielddef(PhilippeParser::FielddefContext *ctx) {
    return FieldDef(
        ctx->ID()->getText(),
        visit(ctx->exp()).as<expp>());
}

antlrcpp::Any ASTGen::visitListexp(PhilippeParser::ListexpContext *ctx) {
    return expp(new ListExp(visit(ctx->explist()).as<expl>()));
}

antlrcpp::Any ASTGen::visitExplist(PhilippeParser::ExplistContext *ctx) {
    expl l;
    for (auto e : ctx->exp()) {
        l.push_back(visit(e).as<expp>());
    }
    return l;
}

antlrcpp::Any ASTGen::visitTupleexp(PhilippeParser::TupleexpContext *ctx) {
    expl el;
    for (auto e : ctx->exp()) {
        el.push_back(visit(e));
    }
    return expp(new TupleExp(el));
}

antlrcpp::Any ASTGen::visitCastexp(PhilippeParser::CastexpContext *ctx) {
    return expp(new CastExp(visit(ctx->exp()), visit(ctx->type())));
}

antlrcpp::Any ASTGen::visitFloatexp(PhilippeParser::FloatexpContext *ctx) {
    stringstream s;
    s << ctx->FLOAT()->getText();
    double val;
    s >> val;
    return expp(new FloatExp(val));
}

antlrcpp::Any ASTGen::visitIntexp(PhilippeParser::IntexpContext *ctx) {
    stringstream s;
    if (ctx->INT()) s << ctx->INT()->getText();
    else if (ctx->HEX()) s << ctx->HEX()->getText();
    long val;
    s >> val;
    return expp(new IntExp(val));
}

antlrcpp::Any ASTGen::visitLexp(PhilippeParser::LexpContext *ctx) {
    auto name = ctx->ID()->getText();
    auto t = getSymbol(name);

    // New symbol
    if (!t) {
        if (ctx->lexpsuffix().size() > 0)
            throw runtime_error("Can't index into newly created variable");
        return idToLexp(name);
    }
    // Existing symbol
    vector<shared_ptr<Lexpsuffix>> l;
    for (auto s : ctx->lexpsuffix()) {
        auto suf = visit(s);
        // Index
        if (suf.is<expp>()) {
            expp s0 = suf;
            // TODO separate tuple and list access
            if (auto li = dynamic_pointer_cast<TypeList>(t)) {
                if (!dynamic_pointer_cast<TypeInt>(s0->type)) throw runtime_error("Can't index into list with non-int");
                l.push_back(shared_ptr<Lexpsuffix>(new ListIndexSuffix(s0)));
                t = s0->type;
            } else if (auto tu = dynamic_pointer_cast<TypeTuple>(t)) {
                if (auto i = dynamic_pointer_cast<IntExp>(s0)) {
                    l.push_back(shared_ptr<Lexpsuffix>(new TupleAccessSuffix(i->val)));
                    t = tu->t[i->val];
                } else throw runtime_error("can't index into tuple with non-const, non-int");
            }
        }
        // Member
        else {
            if (auto o = dynamic_pointer_cast<TypeObj>(t)) {
                string id = suf.as<string>();
                int index = getFieldIndex(o->name, suf);
                l.push_back(shared_ptr<Lexpsuffix>(new TupleAccessSuffix(index)));
                t = ast.objectDefinitions[o->name].type->t[index];
            } else throw runtime_error("Can't access member from non-object");

        }
    }
    return lexpp(new Lexp(name, l, t));
}

antlrcpp::Any ASTGen::visitLexpopttype(PhilippeParser::LexpopttypeContext *ctx) {
    lexpp l = visit(ctx->lexp());
    if (ctx->type()) {
        typep t = visit(ctx->type());
        if (!typesEqual(t, l->type))
            throw runtime_error("Unmatched types");
    }
    return l;
}

antlrcpp::Any ASTGen::visitLexpsuffix(PhilippeParser::LexpsuffixContext *ctx) {
    if (ctx->exp()) {
        return visit(ctx->exp()).as<expp>();
    } else {
        return ctx->ID()->getText();
    }
}

antlrcpp::Any ASTGen::visitPrimitivetype(PhilippeParser::PrimitivetypeContext *ctx) {
    auto t = ctx->t->getText();
    if (t == "int") return typep(new TypeInt());
    else if (t == "float") return typep(new TypeFloat());
    else if (t == "bool") return typep(new TypeBool());
    else if (t == "string") return typep(new TypeString());
    else if (t == "nil") return typep(new TypeNil());
    throw runtime_error("incorrect primitive type");
}

antlrcpp::Any ASTGen::visitTupletype(PhilippeParser::TupletypeContext *ctx) {
    vector<typep> t;
    for (auto a : ctx->typeaux()) {
        t.push_back(visit(a));
    }
    return typep(new TypeTuple(t));
}

antlrcpp::Any ASTGen::visitObjaliastype(PhilippeParser::ObjaliastypeContext *ctx) {
    return typep(new TypeObj(ctx->ID()->getText()));
}

antlrcpp::Any ASTGen::visitFunctype(PhilippeParser::FunctypeContext *ctx) {
    typep ret = nullptr;
    if (ctx->ret) ret = visit(ctx->ret);

    vector<typep> t;
    for (auto a : ctx->typeaux()) {
        t.push_back(visit(a));
    }
    return typep(new TypeFunction(t, ret));
}

antlrcpp::Any ASTGen::visitListtype(PhilippeParser::ListtypeContext *ctx) {
    return typep(new TypeList(visit(ctx->typeaux()).as<typep>()));
}

bool typesEqual(typep a, typep b) {
    if (typeid(*a) == typeid(*b)) {
        if (auto a0 = dynamic_pointer_cast<TypeTuple>(a)) {
            auto b0 = dynamic_pointer_cast<TypeTuple>(b);
            for (int i=0;i<a0->t.size();i++) {
                if (!typesEqual(a0->t[i], b0->t[i])) return false;
            }
            return true;
        } else if (auto a0 = dynamic_pointer_cast<TypeObj>(a)) {
            auto b0 = dynamic_pointer_cast<TypeObj>(b);
            return a0->name == b0->name;
        } else if (auto a0 = dynamic_pointer_cast<TypeFunction>(a)) {
            auto b0 = dynamic_pointer_cast<TypeFunction>(b);
            for (int i=0;i<a0->args.size();i++) {
                if (!typesEqual(a0->args[i], b0->args[i])) return false;
            }
            return typesEqual(a0->ret, b0->ret);
        } else if (auto a0 = dynamic_pointer_cast<TypeList>(a)) {
            auto b0 = dynamic_pointer_cast<TypeList>(b);
            return typesEqual(a0->t, b0->t);
        }

        return true;
    }
    return false;
}

lexpp ASTGen::idToLexp(string name) {
    return lexpp(new Lexp(name, {}, nullptr));
}

int ASTGen::getFieldIndex(string obj, string name) {
    auto it = ast.objectDefinitions.find(obj);
    if (it == ast.objectDefinitions.end()) throw runtime_error("Object definition does not exist");
    auto od = it->second;
    auto it2 = od.fields.find(name);
    if (it2 == od.fields.end()) throw runtime_error("Can't find field");
    return it2->second;
}
