#include "ASTGen.h"

block ASTGen::gen(PhilippeParser::ChunkContext *ctx) {
    return visitChunk(ctx).as<block>();
}

antlrcpp::Any ASTGen::visitChunk(PhilippeParser::ChunkContext *ctx) {
    return visit(ctx->block());
}

antlrcpp::Any ASTGen::visitBlock(PhilippeParser::BlockContext *ctx) {
    block b;
    for (auto s : ctx->stat()) {
      b.push_back(visit(s).as<statp>());
    }
    return b;
}

antlrcpp::Any ASTGen::visitBreakstat(PhilippeParser::BreakstatContext *ctx) {
    return statp(new BreakStat());
}

antlrcpp::Any ASTGen::visitBlockstat(PhilippeParser::BlockstatContext *ctx) {
    return statp(new BlockStat(visit(ctx->block()).as<block>()));
}

expp ASTGen::exp_option(PhilippeParser::ExpContext *e) {
    if (!e) return expp(new NilExp());
    else return visit(e).as<expp>();
}

antlrcpp::Any ASTGen::visitReturnstat(PhilippeParser::ReturnstatContext *ctx) {
    return statp(new ReturnStat(exp_option(ctx->exp())));
}

antlrcpp::Any ASTGen::visitStdassign(PhilippeParser::StdassignContext *ctx) {
    return statp(new AssignStat(visit(ctx->explist()).as<expl>(),visit(ctx->exp()).as<expp>()));
}

antlrcpp::Any ASTGen::visitCompoundassign(PhilippeParser::CompoundassignContext *ctx) {
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

antlrcpp::Any ASTGen::visitFunctioncall(PhilippeParser::FunctioncallContext *ctx) {
    return statp(new FuncCallStat(
        visit(ctx->exp()).as<expp>(),
        visit(ctx->explist()).as<expl>()));
}

antlrcpp::Any ASTGen::visitWhile_stat(PhilippeParser::While_statContext *ctx) {
    return statp(new WhileStat(
        visit(ctx->exp()).as<expp>(),
        visit(ctx->stat()).as<statp>()));
}

statp ASTGen::stat_option(PhilippeParser::StatContext *s) {
    if (!s) return statp(new EmptyStat());
    else return visit(s).as<statp>();
}

statp ASTGen::visitIfAux(PhilippeParser::If_statContext *ctx, int index) {
    auto e = visit(ctx->exp(index)).as<expp>();
    auto s = visit(ctx->stat(index)).as<statp>();
    if (index == ctx->exp().size()) {
        statp els = stat_option(ctx->els);
        return statp(new IfStat(e,s,els));
    } else {
        return statp(new IfStat(e,s,visitIfAux(ctx, index+1)));
    }
}

antlrcpp::Any ASTGen::visitIf_stat(PhilippeParser::If_statContext *ctx) {
    return visitIfAux(ctx, 0);
}

antlrcpp::Any ASTGen::visitFor_stat(PhilippeParser::For_statContext *ctx) {
    return statp(new ForStat(
        ctx->ID()->getText(),
        visit(ctx->exp()).as<expp>(),
        visit(ctx->stat()).as<statp>()));
}

antlrcpp::Any ASTGen::visitNilexp(PhilippeParser::NilexpContext *ctx) {
    return expp(new NilExp());
}

antlrcpp::Any ASTGen::visitMemberexp(PhilippeParser::MemberexpContext *ctx) {
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

expp ASTGen::visitBinOp(std::string op, PhilippeParser::ExpContext *left, PhilippeParser::ExpContext *right) {
    return expp(new BinOpExp(
        getBinOp(op),
        visit(left).as<expp>(),
        visit(right).as<expp>()));
}

antlrcpp::Any ASTGen::visitUnaryexp(PhilippeParser::UnaryexpContext *ctx) {
    auto op = getUnaryOp(ctx->op->getText());
    return expp(new UnaryOpExp(
        getUnaryOp(ctx->op->getText()),
        visit(ctx->exp()).as<expp>()));
}

antlrcpp::Any ASTGen::visitExponentexp(PhilippeParser::ExponentexpContext *ctx) {
    return visitBinOp("^", ctx->exp(0), ctx->exp(1));
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
    return expp(new StringExp(ctx->STRING()->getText()));
}

antlrcpp::Any ASTGen::visitTernaryexp(PhilippeParser::TernaryexpContext *ctx) {
    return expp(new TernaryExp(
        visit(ctx->exp(0)).as<expp>(),
        visit(ctx->exp(1)).as<expp>(),
        visit(ctx->exp(2)).as<expp>()));
}

antlrcpp::Any ASTGen::visitFunccallexp(PhilippeParser::FunccallexpContext *ctx) {
    return expp(new CallExp(
        visit(ctx->exp()).as<expp>(),
        visit(ctx->explist()).as<expl>()));
}

antlrcpp::Any ASTGen::visitAndexp(PhilippeParser::AndexpContext *ctx) {
    return visitBinOp("and", ctx->exp(0), ctx->exp(1));
}

antlrcpp::Any ASTGen::visitRelationexp(PhilippeParser::RelationexpContext *ctx) {
    return visitBinOp(ctx->op->getText(), ctx->exp(0), ctx->exp(1));
}

antlrcpp::Any ASTGen::visitStrcatexp(PhilippeParser::StrcatexpContext *ctx) {
    return expp(new BinOpExp(
        BinOp::Concat,
        visit(ctx->exp(0)).as<expp>(),
        visit(ctx->exp(1)).as<expp>()));
}

antlrcpp::Any ASTGen::visitTrueexp(PhilippeParser::TrueexpContext *ctx) {
    return expp(new BoolExp(true));
}

antlrcpp::Any ASTGen::visitIndexexp(PhilippeParser::IndexexpContext *ctx) {
    return expp(new IndexExp(
        visit(ctx->exp(0)).as<expp>(),
        visit(ctx->exp(1)).as<expp>()));
}

antlrcpp::Any ASTGen::visitObjdef(PhilippeParser::ObjdefContext *ctx) {
    std::vector<FieldDef> fields;
    for (auto f : ctx->fielddef()) {
        fields.push_back(visit(f).as<FieldDef>());
    }
    return expp(new ObjExp(fields));
}

antlrcpp::Any ASTGen::visitFielddef(PhilippeParser::FielddefContext *ctx) {
    return FieldDef(
        ctx->ID()->getText(),
        visit(ctx->exp()).as<expp>());
}

antlrcpp::Any ASTGen::visitListdef(PhilippeParser::ListdefContext *ctx) {
    return expp(new ListExp(visit(ctx->explist()).as<expl>()));
}

antlrcpp::Any ASTGen::visitExplist(PhilippeParser::ExplistContext *ctx) {
    expl l;
    for (auto e : ctx->exp()) {
        l.push_back(visit(e).as<expp>());
    }
    return l;
}

antlrcpp::Any ASTGen::visitRangedef(PhilippeParser::RangedefContext *ctx) {
    return expp(new RangeExp(
        visit(ctx->exp(0)).as<expp>(),
        visit(ctx->exp(1)).as<expp>()));
}

antlrcpp::Any ASTGen::visitTupledef(PhilippeParser::TupledefContext *ctx) {
    return expp(new TupleExp(
        visit(ctx->tupleelements()).as<expl>()));
}

antlrcpp::Any ASTGen::visitTupleelements(PhilippeParser::TupleelementsContext *ctx) {
    expl l;
    for (auto e : ctx->exp()) {
        l.push_back(visit(e).as<expp>());
    }
    return l;
}

antlrcpp::Any ASTGen::visitFuncdef(PhilippeParser::FuncdefContext *ctx) {
    arglist l = {};
    if (ctx->arglist()) l = visit(ctx->arglist()).as<arglist>();
    return expp(new FuncExp(
        l, visit(ctx->funcbody()).as<FuncBody>()));
}

antlrcpp::Any ASTGen::visitArglist(PhilippeParser::ArglistContext *ctx) {
    arglist l;
    for (auto i : ctx->ID()) {
        l.push_back(i->getText());
    }
    return l;
}

antlrcpp::Any ASTGen::visitFuncbody(PhilippeParser::FuncbodyContext *ctx) {
    return FuncBody(
        visit(ctx->block()).as<block>(),
        visit(ctx->retstat()).as<expp>());
}

antlrcpp::Any ASTGen::visitIntliteral(PhilippeParser::IntliteralContext *ctx) {
    std::stringstream s;
    if (ctx->INT()) s << ctx->INT()->getText();
    else if (ctx->HEX()) s << ctx->HEX()->getText();
    long val;
    s >> val;
    return expp(new IntExp(val));
}

antlrcpp::Any ASTGen::visitFloatliteral(PhilippeParser::FloatliteralContext *ctx) {
    std::stringstream s;
    s << ctx->FLOAT()->getText();
    double val;
    s >> val;
    return expp(new IntExp(val));
}