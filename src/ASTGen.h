#pragma once

#include "AST.h"

#include "parser/PhilippeBaseVisitor.h"

class ASTGen : public PhilippeBaseVisitor {
public:

    expp exp_option(PhilippeParser::ExpContext *e);
    statp stat_option(PhilippeParser::StatContext *s);
    statp visitIfAux(PhilippeParser::IfstatContext *ctx, int index);
    expp visitBinOp(std::string op, PhilippeParser::ExpContext *left, PhilippeParser::ExpContext *right);

    File gen(PhilippeParser::FileContext *ctx);

    virtual antlrcpp::Any visitFile(PhilippeParser::FileContext *ctx)  override;
    virtual antlrcpp::Any visitGlobaldef(PhilippeParser::GlobaldefContext *ctx)  override;
    virtual antlrcpp::Any visitFunctiondef(PhilippeParser::FunctiondefContext *ctx)  override;
    virtual antlrcpp::Any visitAliasdef(PhilippeParser::AliasdefContext *ctx)  override;
    virtual antlrcpp::Any visitObjdef(PhilippeParser::ObjdefContext *ctx)  override;
    virtual antlrcpp::Any visitBreakstat(PhilippeParser::BreakstatContext *ctx)  override;
    virtual antlrcpp::Any visitBlockstat(PhilippeParser::BlockstatContext *ctx)  override;
    virtual antlrcpp::Any visitReturnstat(PhilippeParser::ReturnstatContext *ctx)  override;
    virtual antlrcpp::Any visitStdassign(PhilippeParser::StdassignContext *ctx)  override;
    virtual antlrcpp::Any visitCompoundassign(PhilippeParser::CompoundassignContext *ctx)  override;
    virtual antlrcpp::Any visitFunccall(PhilippeParser::FunccallContext *ctx)  override;
    virtual antlrcpp::Any visitWhilestat(PhilippeParser::WhilestatContext *ctx)  override;
    virtual antlrcpp::Any visitIfstat(PhilippeParser::IfstatContext *ctx)  override;
    virtual antlrcpp::Any visitForstat(PhilippeParser::ForstatContext *ctx)  override;
    virtual antlrcpp::Any visitForexp(PhilippeParser::ForexpContext *ctx)  override;
    virtual antlrcpp::Any visitArg(PhilippeParser::ArgContext *ctx)  override;
    virtual antlrcpp::Any visitNilexp(PhilippeParser::NilexpContext *ctx)  override;
    virtual antlrcpp::Any visitMemberexp(PhilippeParser::MemberexpContext *ctx)  override;
    virtual antlrcpp::Any visitUnaryexp(PhilippeParser::UnaryexpContext *ctx)  override;
    virtual antlrcpp::Any visitOrexp(PhilippeParser::OrexpContext *ctx)  override;
    virtual antlrcpp::Any visitAdditiveexp(PhilippeParser::AdditiveexpContext *ctx)  override;
    virtual antlrcpp::Any visitMultiplicativeexp(PhilippeParser::MultiplicativeexpContext *ctx)  override;
    virtual antlrcpp::Any visitIdexp(PhilippeParser::IdexpContext *ctx)  override;
    virtual antlrcpp::Any visitComparisonexp(PhilippeParser::ComparisonexpContext *ctx)  override;
    virtual antlrcpp::Any visitParenexp(PhilippeParser::ParenexpContext *ctx)  override;
    virtual antlrcpp::Any visitFalseexp(PhilippeParser::FalseexpContext *ctx)  override;
    virtual antlrcpp::Any visitStringexp(PhilippeParser::StringexpContext *ctx)  override;
    virtual antlrcpp::Any visitTernaryexp(PhilippeParser::TernaryexpContext *ctx)  override;
    virtual antlrcpp::Any visitFunccallexp(PhilippeParser::FunccallexpContext *ctx)  override;
    virtual antlrcpp::Any visitAndexp(PhilippeParser::AndexpContext *ctx)  override;
    virtual antlrcpp::Any visitRelationexp(PhilippeParser::RelationexpContext *ctx)  override;
    virtual antlrcpp::Any visitTrueexp(PhilippeParser::TrueexpContext *ctx)  override;
    virtual antlrcpp::Any visitIndexexp(PhilippeParser::IndexexpContext *ctx)  override;
    virtual antlrcpp::Any visitObjexp(PhilippeParser::ObjexpContext *ctx)  override;
    virtual antlrcpp::Any visitFielddef(PhilippeParser::FielddefContext *ctx)  override;
    virtual antlrcpp::Any visitListexp(PhilippeParser::ListexpContext *ctx)  override;
    virtual antlrcpp::Any visitExplist(PhilippeParser::ExplistContext *ctx)  override;
    virtual antlrcpp::Any visitTupleexp(PhilippeParser::TupleexpContext *ctx)  override;
    virtual antlrcpp::Any visitCastexp(PhilippeParser::CastexpContext *ctx)  override;
    virtual antlrcpp::Any visitFloatexp(PhilippeParser::FloatexpContext *ctx)  override;
    virtual antlrcpp::Any visitIntexp(PhilippeParser::IntexpContext *ctx)  override;
    virtual antlrcpp::Any visitLexp(PhilippeParser::LexpContext *ctx)  override;
    virtual antlrcpp::Any visitLexpopttype(PhilippeParser::LexpopttypeContext *ctx)  override;
    virtual antlrcpp::Any visitLexpsuffix(PhilippeParser::LexpsuffixContext *ctx)  override;
    virtual antlrcpp::Any visitPrimitivetype(PhilippeParser::PrimitivetypeContext *ctx)  override;
    virtual antlrcpp::Any visitTupletype(PhilippeParser::TupletypeContext *ctx)  override;
    virtual antlrcpp::Any visitObjaliastype(PhilippeParser::ObjaliastypeContext *ctx)  override;
    virtual antlrcpp::Any visitFunctype(PhilippeParser::FunctypeContext *ctx)  override;
    virtual antlrcpp::Any visitListtype(PhilippeParser::ListtypeContext *ctx)  override;

    void resolveAliases();
    typep replaceAlias(typep a, bool failOnObj);
    void replaceAliases2(statp s);
    void replaceAliases2(expp s);

private:
    int tmpid = 0;
    std::string newtmp() {
        return "$" + tmpid++;
    }

    File ast;
};