#pragma once

#include "AST.h"

#include "parser/PhilippeBaseVisitor.h"

class ASTGen : public PhilippeBaseVisitor {
public:

    expp exp_option(PhilippeParser::ExpContext *e);
    statp stat_option(PhilippeParser::StatContext *s);
    statp visitIfAux(PhilippeParser::If_statContext *ctx, int index);
    expp visitBinOp(std::string op, PhilippeParser::ExpContext *left, PhilippeParser::ExpContext *right);

    block gen(PhilippeParser::ChunkContext *ctx);
    antlrcpp::Any visitChunk(PhilippeParser::ChunkContext *ctx) override;
    antlrcpp::Any visitBlock(PhilippeParser::BlockContext *ctx) override;
    antlrcpp::Any visitBreakstat(PhilippeParser::BreakstatContext *ctx) override;
    antlrcpp::Any visitBlockstat(PhilippeParser::BlockstatContext *ctx) override;
    antlrcpp::Any visitReturnstat(PhilippeParser::ReturnstatContext *ctx) override;
    antlrcpp::Any visitStdassign(PhilippeParser::StdassignContext *ctx) override;
    antlrcpp::Any visitCompoundassign(PhilippeParser::CompoundassignContext *ctx) override;
    antlrcpp::Any visitFunctioncall(PhilippeParser::FunctioncallContext *ctx) override;
    antlrcpp::Any visitWhile_stat(PhilippeParser::While_statContext *ctx) override;
    antlrcpp::Any visitIf_stat(PhilippeParser::If_statContext *ctx) override;
    antlrcpp::Any visitFor_stat(PhilippeParser::For_statContext *ctx) override;
    antlrcpp::Any visitNilexp(PhilippeParser::NilexpContext *ctx) override;
    antlrcpp::Any visitMemberexp(PhilippeParser::MemberexpContext *ctx) override;
    antlrcpp::Any visitUnaryexp(PhilippeParser::UnaryexpContext *ctx) override;
    antlrcpp::Any visitExponentexp(PhilippeParser::ExponentexpContext *ctx) override;
    antlrcpp::Any visitOrexp(PhilippeParser::OrexpContext *ctx) override;
    antlrcpp::Any visitAdditiveexp(PhilippeParser::AdditiveexpContext *ctx) override;
    antlrcpp::Any visitMultiplicativeexp(PhilippeParser::MultiplicativeexpContext *ctx) override;
    antlrcpp::Any visitIdexp(PhilippeParser::IdexpContext *ctx) override;
    antlrcpp::Any visitComparisonexp(PhilippeParser::ComparisonexpContext *ctx) override;
    antlrcpp::Any visitParenexp(PhilippeParser::ParenexpContext *ctx) override;
    antlrcpp::Any visitFalseexp(PhilippeParser::FalseexpContext *ctx) override;
    antlrcpp::Any visitStringexp(PhilippeParser::StringexpContext *ctx) override;
    antlrcpp::Any visitTernaryexp(PhilippeParser::TernaryexpContext *ctx) override;
    antlrcpp::Any visitFunccallexp(PhilippeParser::FunccallexpContext *ctx) override;
    antlrcpp::Any visitAndexp(PhilippeParser::AndexpContext *ctx) override;
    antlrcpp::Any visitRelationexp(PhilippeParser::RelationexpContext *ctx) override;
    antlrcpp::Any visitStrcatexp(PhilippeParser::StrcatexpContext *ctx) override;
    antlrcpp::Any visitTrueexp(PhilippeParser::TrueexpContext *ctx) override;
    antlrcpp::Any visitIndexexp(PhilippeParser::IndexexpContext *ctx) override;
    antlrcpp::Any visitObjdef(PhilippeParser::ObjdefContext *ctx) override;
    antlrcpp::Any visitFielddef(PhilippeParser::FielddefContext *ctx) override;
    antlrcpp::Any visitListdef(PhilippeParser::ListdefContext *ctx) override;
    antlrcpp::Any visitExplist(PhilippeParser::ExplistContext *ctx) override;
    antlrcpp::Any visitRangedef(PhilippeParser::RangedefContext *ctx) override;
    antlrcpp::Any visitTupledef(PhilippeParser::TupledefContext *ctx) override;
    antlrcpp::Any visitTupleelements(PhilippeParser::TupleelementsContext *ctx) override;
    antlrcpp::Any visitFuncdef(PhilippeParser::FuncdefContext *ctx) override;
    antlrcpp::Any visitArglist(PhilippeParser::ArglistContext *ctx) override;
    antlrcpp::Any visitFuncbody(PhilippeParser::FuncbodyContext *ctx) override;
    antlrcpp::Any visitIntliteral(PhilippeParser::IntliteralContext *ctx) override;
    antlrcpp::Any visitFloatliteral(PhilippeParser::FloatliteralContext *ctx) override;

};