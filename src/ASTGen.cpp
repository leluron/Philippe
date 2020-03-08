#include "ASTGen.h"
#include "Stdlib.h"

#include <algorithm>

using namespace std;

File ASTGen::gen(PhilippeParser::FileContext *ctx) {
    loadStd();
    visit(ctx);
    resolveAliases();
    return ast;
}

antlrcpp::Any ASTGen::visitFile(PhilippeParser::FileContext *ctx) {
    for (auto d : ctx->def()) {
        visit(d);
    }
    return nullptr;
}

antlrcpp::Any ASTGen::visitGlobaldef(PhilippeParser::GlobaldefContext *ctx) {
    auto value = visit(ctx->exp());

    typep t = nullptr;
    if (ctx->type()) t = visit(ctx->type());

    auto name = ctx->ID()->getText();
    auto it = ast.globals.find(name);
    if (it == ast.globals.end())
        ast.globals.insert({name, {t, value}});
    else throw "can't have globals with the same name";

    return nullptr;
}

antlrcpp::Any ASTGen::visitFunctiondef(PhilippeParser::FunctiondefContext *ctx) {
    typep ret;
    if (ctx->type()) ret = visit(ctx->type());
    else ret = typep(new TypeNil());

    vector<Arg> args;
    for (auto a : ctx->arg()) {
        args.push_back(visit(a));
    }
    block b;
    for (auto s : ctx->stat()) {
        statp s1 = visit(s);
        if(auto b1 = dynamic_pointer_cast<BlockStat>(s1))
            b.insert(b.end(), b1->stats.begin(), b1->stats.end());
        else
            b.push_back(s1);
    }
    auto name = ctx->ID()->getText();
    auto it = ast.functions.find(name);
    if (it == ast.functions.end())
        ast.functions.insert({name, defp(new FunctionDef(
            args, ret, b))});
    else throw "can't have functions with the same name";
    return nullptr;
}

antlrcpp::Any ASTGen::visitAliasdef(PhilippeParser::AliasdefContext *ctx) {
    auto name = ctx->ID()->getText();
    auto it = ast.aliases.find(name);
    auto it2 = ast.objectDefinitions.find(name);
    if (it == ast.aliases.end() && it2 == ast.objectDefinitions.end())
        ast.aliases.insert({name, visit(ctx->type())});
    else throw "can't have aliases and objects with the same name";
    return nullptr;
}

antlrcpp::Any ASTGen::visitObjdef(PhilippeParser::ObjdefContext *ctx) {
    vector<Arg> args;
    for (auto a : ctx->arg()) {
        args.push_back(visit(a));
    }
    auto name = ctx->ID()->getText();
    auto it = ast.objectDefinitions.find(name);
    auto it2 = ast.aliases.find(name);
    if (it == ast.objectDefinitions.end() && it2 == ast.aliases.end())
        ast.objectDefinitions.insert({name, {args}});
    else throw "can't have aliases and objects with the same name";
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
    return statp(new ReturnStat(exp_option(ctx->exp())));
}

antlrcpp::Any ASTGen::visitStdassign(PhilippeParser::StdassignContext *ctx) {
    expp e = visit(ctx->exp());
    if (ctx->lexpopttype().size() == 1) {
        lexpp lexp = visit(ctx->lexpopttype(0));
        return statp(new AssignStat(lexp, visit(ctx->exp())));
    } else {
        block out;
        auto tmp = newtmp();
        out.push_back(statp(new AssignStat(lexpp(new Lexp(tmp, {}, nullptr)), e)));
        for (int i=0;i<ctx->lexpopttype().size();i++) {
            lexpp lexp = visit(ctx->lexpopttype(i));
            out.push_back(statp(new AssignStat(
                lexp, 
                expp(
                    new IndexExp(
                        expp(new IdExp(tmp)),
                        expp(new IntExp(i))
                    )
                )
            )));
        }
        return statp(new BlockStat(out));
    }
}

expp toRvalue(lexpp l) {
    expp b = expp(new IdExp(l->name));
    for (auto s : l->suffixes) {
        if (auto ind = dynamic_pointer_cast<IndexSuffix>(s)) {
            b = expp(new IndexExp(b, ind->i));
        } else if (auto mem = dynamic_pointer_cast<MemberSuffix>(s)) {
            b = expp(new MemberExp(b, mem->name));
        } else throw;
    }
    return b;

}

antlrcpp::Any ASTGen::visitCompoundassign(PhilippeParser::CompoundassignContext *ctx) {
    auto op = ctx->op->getText();
    lexpp left = visit(ctx->lexpopttype());
    expp right = visit(ctx->exp());
    BinOp o;
    switch (op[0]) {
        case '+': o = BinOp::Plus;    break;
        case '-': o = BinOp::Minus;   break;
        case '*': o = BinOp::Mul;     break;
        case '/': o = BinOp::Div;     break;
    }
    expp e = expp(new BinOpExp(o, toRvalue(left), right));
    return statp(new AssignStat(left, e));
}

antlrcpp::Any ASTGen::visitFunccall(PhilippeParser::FunccallContext *ctx) {
    return statp(new FuncCallStat(
        visit(ctx->lexp()),
        visit(ctx->explist())
    ));
}

antlrcpp::Any ASTGen::visitWhilestat(PhilippeParser::WhilestatContext *ctx) {
    return statp(new WhileStat(
        visit(ctx->exp()).as<expp>(),
        statp(visit(ctx->stat()).as<statp>())));
}

statp ASTGen::stat_option(PhilippeParser::StatContext *s) {
    if (!s) return statp(new BlockStat({}));
    else return visit(s);
}

statp ASTGen::visitIfAux(PhilippeParser::IfstatContext *ctx, int index) {
    expp e = visit(ctx->exp(index));
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

    return expp(new MemberExp(
        l, ctx->ID()->getText()));
}

UnaryOp getUnaryOp(string t) {
    if (t=="-") return UnaryOp::Minus;
    else if (t=="not") return UnaryOp::Not;
    else throw;
}

expp ASTGen::visitBinOp(string t, PhilippeParser::ExpContext *left, PhilippeParser::ExpContext *right) {
    expp l = visit(left);
    expp r = visit(right);
    Exp* e;
    if (t=="*") {
        e= new BinOpExp(BinOp::Mul, l, r);
    } else if (t=="/") {
        e = new BinOpExp(BinOp::Div, l, r);
    } else if (t=="%") {
        e = new BinOpExp(BinOp::Mod, l, r);
    } else if (t=="+") {
        e = new BinOpExp(BinOp::Plus, l, r);
    } else if (t=="-") {
        e = new BinOpExp(BinOp::Minus, l, r);
    } else if (t=="<=") {
        e = new BinOpExp(BinOp::Lteq, l, r);
    } else if (t=="<") {
        e = new BinOpExp(BinOp::Lt, l, r);
    } else if (t==">") {
        e = new BinOpExp(BinOp::Lteq, r, l);
    } else if (t==">=") {
        e = new BinOpExp(BinOp::Lt, r, l);
    } else if (t=="==") {
        e = new BinOpExp(BinOp::Eq, l, r);
    } else if (t=="!=") {
        e = new UnaryOpExp(UnaryOp::Not, expp(new BinOpExp(BinOp::Eq, l, r)));
    } else if (t=="and") {
        e = new BinOpExp(BinOp::And, l, r);
    } else if (t=="or") {
        e = new BinOpExp(BinOp::Or, l, r);
    }
    return expp(e);
}

antlrcpp::Any ASTGen::visitUnaryexp(PhilippeParser::UnaryexpContext *ctx) {
    auto op = getUnaryOp(ctx->op->getText());
    return expp(new UnaryOpExp(
        getUnaryOp(ctx->op->getText()),
        visit(ctx->exp()).as<expp>()));
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
    return expp(new IdExp(ctx->ID()->getText()));
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
    return expp(new CallExp(
        visit(ctx->lexp()),
        visit(ctx->explist())));
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
    return expp(new IndexExp(
        l, r));
}

antlrcpp::Any ASTGen::visitObjexp(PhilippeParser::ObjexpContext *ctx) {
    vector<FieldDef> fields;
    for (auto f : ctx->fielddef()) {
        fields.push_back(visit(f));
    }
    return expp(new ObjExp(ctx->ID()->getText(), fields));
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
    return expp(new IntExp(val));
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
    vector<shared_ptr<Lexpsuffix>> l;
    for (auto s : ctx->lexpsuffix()) {
        l.push_back(visit(s));
    }
    return lexpp(new Lexp(ctx->ID()->getText(), l, nullptr));
}

antlrcpp::Any ASTGen::visitLexpopttype(PhilippeParser::LexpopttypeContext *ctx) {
    lexpp l = visit(ctx->lexp());
    if (ctx->type()) {
        l->type = visit(ctx->type());
    }
    return l;
}

antlrcpp::Any ASTGen::visitLexpsuffix(PhilippeParser::LexpsuffixContext *ctx) {
    if (ctx->exp()) {
        return shared_ptr<Lexpsuffix>(new IndexSuffix(visit(ctx->exp()).as<expp>()));
    } else {
        return shared_ptr<Lexpsuffix>(new MemberSuffix(ctx->ID()->getText()));
    }
}

antlrcpp::Any ASTGen::visitPrimitivetype(PhilippeParser::PrimitivetypeContext *ctx) {
    auto t = ctx->t->getText();
    if (t == "int") return typep(new TypeInt());
    else if (t == "float") return typep(new TypeFloat());
    else if (t == "bool") return typep(new TypeBool());
    else if (t == "string") return typep(new TypeString());
    else if (t == "nil") return typep(new TypeNil());
    throw "incorrect primitive type";
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
    if (typeid(*a.get()) == typeid(*b.get())) {
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