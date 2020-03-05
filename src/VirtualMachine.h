#pragma once

#include <stack>
#include <map>
#include <vector>
#include <iostream>

enum Instruction {
    Noop = 0,
    LoadS, LoadM,
    Store,
    Alloc,
    Free,
    Call,
    Return,
    IfJump,
    Jump,
    Castfi, Castif,
    Not,
    And,
    Or,
    Usubi, Usubf,
    Powi, Powf,
    Muli, Mulf,
    Divi, Divf,
    Modi,
    Addi, Addf,
    Subi, Subf,
    Lteqi, Lteqf,
    Lti, Ltf,
    Gti, Gtf,
    Gteqi, Gteqf,
    Eqi, Eqf,
    Neqi, Neqf,
    End
};

enum ReservedFuncs {
    RESERVED_FUNCS = 0x0ff0000000000000,
    Printf,
};

using vmcode = std::vector<int64_t>;

class VirtualMachine {
public:
    VirtualMachine(std::ostream &o) : out(o) {}
    void setSize(size_t size) {
        this->memory.resize(size);
    }
    void load(vmcode program) {
        auto size = memory.size();
        memory.assign(program.begin(), program.end());
        if (size > program.size()) {
            this->memory.resize(size);
        }
        RAM = program.size();
    }
    
    void step();
    void run();
    

private:
    uint64_t PC = 0;
    uint64_t RAM = 0;
    std::vector<int64_t> memory;
    std::stack<int64_t> operandStack;
    std::stack<uint64_t> addressStack;
    std::map<int64_t, void (VirtualMachine::*)()> stdlib = {
        { Printf, &VirtualMachine::printf},
    };

    std::ostream &out;

    void printf();

};