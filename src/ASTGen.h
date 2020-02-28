#pragma once

#include "AST.h"

#include "parser/PhilippeBaseVisitor.h"

class ASTGen : public PhilippeBaseVisitor {
public:
    block gen(PhilippeParser::ChunkContext *ctx) {
        return visitChunk(ctx).as<block>();
    }

    antlrcpp::Any visitBlock(PhilippeParser::BlockContext *ctx) override {
        block b;
        for (auto s : ctx->stat()) {
          auto st = visit(s);
          b.push_back(st);
        }
        return b;
    }

};