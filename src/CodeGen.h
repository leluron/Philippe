#pragma once

#include "AST.h"
#include "VirtualMachine.h"

class CodeGenContext {
public:
    std::string code;
};

void codegen(block &b, CodeGenContext &c);

vmcode genCode(block &b, CodeGenContext &c);