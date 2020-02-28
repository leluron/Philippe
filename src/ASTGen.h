#pragma once

#include "AST.h"

#include "parser/PhilippeBaseVisitor.h"

class ASTGen : public PhilippeBaseVisitor {
public:
    block gen(PhilippeParser::ChunkContext *ctx) {
        return visitChunk(ctx).as<block>();
    }

    antlrcpp::Any visitChunk(PhilippeParser::ChunkContext *ctx) override {
        return visit(ctx->block());
    }

    antlrcpp::Any visitBlock(PhilippeParser::BlockContext *ctx) override {
        block b;
        for (auto s : ctx->stat()) {
          b.push_back(visit(s).as<statp>());
        }
        return b;
    }

    antlrcpp::Any visitBreakstat(PhilippeParser::BreakstatContext *ctx) override {
        return statp(new BreakStat());
    }

    antlrcpp::Any visitBlockstat(PhilippeParser::BlockstatContext *ctx) override {
        return statp(new BlockStat(visit(ctx->block()).as<block>()));
    }

    expp exp_option(PhilippeParser::ExpContext *e) {
        if (!e) return expp(new NilExp());
        else return visit(e).as<expp>();
    }

    antlrcpp::Any visitReturnstat(PhilippeParser::ReturnstatContext *ctx) override {
        return statp(new ReturnStat(exp_option(ctx->exp())));
    }

    antlrcpp::Any visitStdassign(PhilippeParser::StdassignContext *ctx) override {
        return statp(new AssignStat(visit(ctx->explist()).as<expl>(),visit(ctx->exp()).as<expp>()));
    }

    antlrcpp::Any visitCompoundassign(PhilippeParser::CompoundassignContext *ctx) override {
        auto op = ctx->op->getText();
        auto left = visit(ctx->exp(0)).as<expp>();
        auto right = visit(ctx->exp(1)).as<expp>();
        BinOp o;
        switch (op[0]) {
            case '+': o = BinOp::Plus;    break;
            case '-': o = BinOp::Minus;   break;
            case '*': o = BinOp::Mul;     break;
            case '/': o = BinOp::Div;     break;
        }
        expp e = expp(new BinOpExp(o, left, right));
        return statp(new AssignStat({left}, e));
    }
 
    antlrcpp::Any visitFunctioncall(PhilippeParser::FunctioncallContext *ctx) override {
        return statp(new FuncCallStat(
            visit(ctx->exp()).as<expp>(),
            visit(ctx->explist()).as<expl>()));
    }

    antlrcpp::Any visitWhile_stat(PhilippeParser::While_statContext *ctx) override {
        return statp(new WhileStat(
            visit(ctx->exp()).as<expp>(),
            visit(ctx->stat()).as<statp>()));
    }

    statp stat_option(PhilippeParser::StatContext *s) {
        if (!s) return statp(new EmptyStat());
        else return visit(s).as<statp>();
    }

    statp visitIfAux(PhilippeParser::If_statContext *ctx, int index) {
        auto e = visit(ctx->exp(index)).as<expp>();
        auto s = visit(ctx->stat(index)).as<statp>();
        if (index == ctx->exp().size()) {
            statp els = stat_option(ctx->els);
            return statp(new IfStat(e,s,els));
        } else {
            return statp(new IfStat(e,s,visitIfAux(ctx, index+1)));
        }
    }

    antlrcpp::Any visitIf_stat(PhilippeParser::If_statContext *ctx) override {
        return visitIfAux(ctx, 0);
    }

    antlrcpp::Any visitFor_stat(PhilippeParser::For_statContext *ctx) override {
        return statp(new ForStat(
            ctx->ID()->getText(),
            visit(ctx->exp()).as<expp>(),
            visit(ctx->stat()).as<statp>()));
    }

    antlrcpp::Any visitNilexp(PhilippeParser::NilexpContext *ctx) override {
        return expp(new NilExp());
    }

    antlrcpp::Any visitMemberexp(PhilippeParser::MemberexpContext *ctx) override {
        return expp(new MemberExp(
            visit(ctx->exp()).as<expp>(),
            ctx->ID()->getText()));
    }

    UnaryOp getUnaryOp(std::string t) {
        if (t=="-") return UnaryOp::Minus;
        else if (t=="not") return UnaryOp::Not;
        else throw;
    }

    BinOp getBinOp(std::string t) {
        if (t=="^") return BinOp::Pow;
        else if (t=="*") return BinOp::Mul;
        else if (t=="/") return BinOp::Div;
        else if (t=="%") return BinOp::Mod;
        else if (t=="+") return BinOp::Plus;
        else if (t=="-") return BinOp::Minus;
        else if (t=="<=") return BinOp::Lteq;
        else if (t=="<") return BinOp::Lt;
        else if (t==">") return BinOp::Gt;
        else if (t==">=") return BinOp::Gteq;
        else if (t=="==") return BinOp::Eq;
        else if (t=="!=") return BinOp::Neq;
        else if (t=="and") return BinOp::And;
        else if (t=="or") return BinOp::Or;
        else throw;
    }

    expp visitBinOp(std::string op, PhilippeParser::ExpContext *left, PhilippeParser::ExpContext *right) {
        return expp(new BinOpExp(
            getBinOp(op),
            visit(left).as<expp>(),
            visit(right).as<expp>()));
    }

    antlrcpp::Any visitUnaryexp(PhilippeParser::UnaryexpContext *ctx) override {
        auto op = getUnaryOp(ctx->op->getText());
        return expp(new UnaryOpExp(
            getUnaryOp(ctx->op->getText()),
            visit(ctx->exp()).as<expp>()));
    }

    antlrcpp::Any visitExponentexp(PhilippeParser::ExponentexpContext *ctx) override {
        return visitBinOp("^", ctx->exp(0), ctx->exp(1));
    }

    antlrcpp::Any visitOrexp(PhilippeParser::OrexpContext *ctx) override {
        return visitBinOp("or", ctx->exp(0), ctx->exp(1));
    }

    antlrcpp::Any visitAdditiveexp(PhilippeParser::AdditiveexpContext *ctx) override {
        return visitBinOp(ctx->op->getText(), ctx->exp(0), ctx->exp(1));
    }

    antlrcpp::Any visitMultiplicativeexp(PhilippeParser::MultiplicativeexpContext *ctx) override {
        return visitBinOp(ctx->op->getText(), ctx->exp(0), ctx->exp(1));
    }

    antlrcpp::Any visitIdexp(PhilippeParser::IdexpContext *ctx) override {
        return expp(new IdExp(ctx->ID()->getText()));
    }

    antlrcpp::Any visitComparisonexp(PhilippeParser::ComparisonexpContext *ctx) override {
        return visitBinOp(ctx->op->getText(), ctx->exp(0), ctx->exp(1));
    }

    antlrcpp::Any visitParenexp(PhilippeParser::ParenexpContext *ctx) override {
        return visit(ctx->exp());
    }

    antlrcpp::Any visitFalseexp(PhilippeParser::FalseexpContext *ctx) override {
        return expp(new BoolExp(false));
    }

    antlrcpp::Any visitStringexp(PhilippeParser::StringexpContext *ctx) override {
        return expp(new StringExp(ctx->STRING()->getText()));
    }

    antlrcpp::Any visitTernaryexp(PhilippeParser::TernaryexpContext *ctx) override {
        return expp(new TernaryExp(
            visit(ctx->exp(0)).as<expp>(),
            visit(ctx->exp(1)).as<expp>(),
            visit(ctx->exp(2)).as<expp>()));
    }

    antlrcpp::Any visitFunccallexp(PhilippeParser::FunccallexpContext *ctx) override {
        return expp(new CallExp(
            visit(ctx->exp()).as<expp>(),
            visit(ctx->explist()).as<expl>()));
    }

    antlrcpp::Any visitAndexp(PhilippeParser::AndexpContext *ctx) override {
        return visitBinOp("and", ctx->exp(0), ctx->exp(1));
    }

    antlrcpp::Any visitRelationexp(PhilippeParser::RelationexpContext *ctx) override {
        return visitBinOp(ctx->op->getText(), ctx->exp(0), ctx->exp(1));
    }

    antlrcpp::Any visitStrcatexp(PhilippeParser::StrcatexpContext *ctx) override {
        return expp(new BinOpExp(
            BinOp::Concat,
            visit(ctx->exp(0)).as<expp>(),
            visit(ctx->exp(1)).as<expp>()));
    }

    antlrcpp::Any visitTrueexp(PhilippeParser::TrueexpContext *ctx) override {
        return expp(new BoolExp(true));
    }

    antlrcpp::Any visitIndexexp(PhilippeParser::IndexexpContext *ctx) override {
        return expp(new IndexExp(
            visit(ctx->exp(0)).as<expp>(),
            visit(ctx->exp(1)).as<expp>()));
    }

    antlrcpp::Any visitObjdef(PhilippeParser::ObjdefContext *ctx) override {
        std::vector<FieldDef> fields;
        for (auto f : ctx->fielddef()) {
            fields.push_back(visit(f).as<FieldDef>());
        }
        return expp(new ObjExp(fields));
    }

    antlrcpp::Any visitFielddef(PhilippeParser::FielddefContext *ctx) override {
        return FieldDef(
            ctx->ID()->getText(),
            visit(ctx->exp()).as<expp>());
    }

    antlrcpp::Any visitListdef(PhilippeParser::ListdefContext *ctx) override {
        return expp(new ListExp(visit(ctx->explist()).as<expl>()));
    }

    antlrcpp::Any visitExplist(PhilippeParser::ExplistContext *ctx) override {
        expl l;
        for (auto e : ctx->exp()) {
            l.push_back(visit(e).as<expp>());
        }
        return l;
    }

    antlrcpp::Any visitRangedef(PhilippeParser::RangedefContext *ctx) override {
        return expp(new RangeExp(
            visit(ctx->exp(0)).as<expp>(),
            visit(ctx->exp(1)).as<expp>()));
    }

    antlrcpp::Any visitTupledef(PhilippeParser::TupledefContext *ctx) override {
        return expp(new TupleExp(
            visit(ctx->tupleelements()).as<expl>()));
    }

    antlrcpp::Any visitTupleelements(PhilippeParser::TupleelementsContext *ctx) override {
        expl l;
        for (auto e : ctx->exp()) {
            l.push_back(visit(e).as<expp>());
        }
        return l;
    }

    antlrcpp::Any visitFuncdef(PhilippeParser::FuncdefContext *ctx) override {
        arglist l = {};
        if (ctx->arglist()) l = visit(ctx->arglist()).as<arglist>();
        return expp(new FuncExp(
            l, visit(ctx->funcbody()).as<FuncBody>()));
    }

    antlrcpp::Any visitArglist(PhilippeParser::ArglistContext *ctx) override {
        arglist l;
        for (auto i : ctx->ID()) {
            l.push_back(i->getText());
        }
        return l;
    }

    antlrcpp::Any visitFuncbody(PhilippeParser::FuncbodyContext *ctx) override {
        return FuncBody(
            visit(ctx->block()).as<block>(),
            visit(ctx->retstat()).as<expp>());
    }

    antlrcpp::Any visitIntliteral(PhilippeParser::IntliteralContext *ctx) override {
        std::stringstream s;
        if (ctx->INT()) s << ctx->INT()->getText();
        else if (ctx->HEX()) s << ctx->HEX()->getText();
        long val;
        s >> val;
        return expp(new IntExp(val));
    }

    antlrcpp::Any visitFloatliteral(PhilippeParser::FloatliteralContext *ctx) override {
        std::stringstream s;
        s << ctx->FLOAT()->getText();
        double val;
        s >> val;
        return expp(new IntExp(val));
    }

};