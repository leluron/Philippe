#pragma once

#include <stack>

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
    Concat,
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

class VirtualMachine {
public:
    VirtualMachine(std::ostream &o) : out(o) {
        stdlib[Printf] = &VirtualMachine::printf;
    }
    void setSize(size_t size) {
        this->memory.resize(size);
    }
    void load(std::vector<int64_t> program) {
        auto size = memory.size();
        memory.assign(program.begin(), program.end());
        if (size > program.size()) {
            this->memory.resize(size);
        }
        RAM = program.size();
    }

    double asfloat(int64_t a) {
        return *(double*)(&a);
    }

    int64_t asint(double a) {
        return *(int64_t*)(&a);
    }

    void step() {
        if (PC >= memory.size()) return;

        auto instr0 = memory[PC];
        auto instr1 = memory[PC+1];

        switch (instr0) {
            case Noop: break;
            case LoadS: {
                operandStack.push(instr1); break;
            }
            case LoadM: {
                operandStack.push(memory[instr1]); break;
            }
            case Store: {
                auto v = operandStack.top();
                operandStack.pop();
                memory[instr1] = v; break;
            }
            case Alloc: {
                auto p = RAM;
                RAM += instr1;
                operandStack.push(RAM);
                break;
            }
            case Free : break;
            case Call: {
                if (instr1 >= RESERVED_FUNCS) {
                    auto func = stdlib[instr1];
                    (this->*func)();
                } else {
                    addressStack.push(PC);
                    PC = instr1-2;
                }
                break;
            }
            case Return: {
                PC = addressStack.top()-2;
                addressStack.pop();
                break;
            }
            case IfJump: {
                if (operandStack.top()) PC = instr1-2;
                operandStack.pop();
                break;
            }
            case Jump: {
                PC = instr1-2; break;
            }
            case Castfi: {
                auto f = asfloat(operandStack.top());
                operandStack.pop();
                operandStack.push((int64_t)f); break;
            }
            case Castif: {
                auto i = operandStack.top();
                operandStack.pop();
                operandStack.push(asint((double)i)); break;
            }
            case Not: {
                auto v = operandStack.top();
                operandStack.pop();
                operandStack.push(!v); break;
            }
            case And: {
                auto a = operandStack.top();
                operandStack.pop();
                auto b = operandStack.top();
                operandStack.pop();
                operandStack.push(a && b); break;
            }
            case Or: {
                auto a = operandStack.top();
                operandStack.pop();
                auto b = operandStack.top();
                operandStack.pop();
                operandStack.push(a || b); break;
            }
            case Usubi: {
                auto a = operandStack.top();
                operandStack.pop();
                operandStack.push(- a); break;
            }
            case Usubf: {
                auto a = asfloat(operandStack.top());
                operandStack.pop();
                operandStack.push(asint(-a)); break;
            }
            case Powi: {
                int64_t a=operandStack.top();
                operandStack.pop();
                int64_t b=operandStack.top();
                operandStack.pop();
                int64_t p = 1;
                for (int i=0;i<b;i++) p *= a;
                operandStack.push(p);
                break;
            }
            case Powf: {
                double a=asfloat(operandStack.top());
                operandStack.pop();
                int64_t b=operandStack.top();
                operandStack.pop();
                double p = 1;
                for (int i=0;i<b;i++) p *= a;
                operandStack.push(asint(p));
                break;
            }
            case Muli: {
                int64_t a=operandStack.top();
                operandStack.pop();
                int64_t b=operandStack.top();
                operandStack.pop();
                operandStack.push(a*b); break;
            }
            case Mulf: {
                double a=asfloat(operandStack.top());
                operandStack.pop();
                double b=asfloat(operandStack.top());
                operandStack.pop();
                operandStack.push(asint(a*b)); break;
            }
            case Divi: {
                int64_t a=operandStack.top();
                operandStack.pop();
                int64_t b=operandStack.top();
                operandStack.pop();
                operandStack.push(a/b); break;
            }
            case Divf: {
                double a=asfloat(operandStack.top());
                operandStack.pop();
                double b=asfloat(operandStack.top());
                operandStack.pop();
                operandStack.push(asint(a / b)); break;
            }
            case Modi: {
                int64_t a=operandStack.top();
                operandStack.pop();
                int64_t b=operandStack.top();
                operandStack.pop();
                operandStack.push(a%b); break;
            }
            case Addi: {
                int64_t a=operandStack.top();
                operandStack.pop();
                int64_t b=operandStack.top();
                operandStack.pop();
                operandStack.push(a+b); break;
            }
            case Addf: {
                double a=asfloat(operandStack.top());
                operandStack.pop();
                double b=asfloat(operandStack.top());
                operandStack.pop();
                operandStack.push(asint(a + b)); break;
            }
            case Subi: {
                int64_t a=operandStack.top();
                operandStack.pop();
                int64_t b=operandStack.top();
                operandStack.pop();
                operandStack.push(a-b); break;
            }
            case Subf: {
                double a=asfloat(operandStack.top());
                operandStack.pop();
                double b=asfloat(operandStack.top());
                operandStack.pop();
                operandStack.push(asint(a - b)); break;
            }
            case Concat: {
                break;
            }
            case Lteqi: {
                int64_t a=operandStack.top();
                operandStack.pop();
                int64_t b=operandStack.top();
                operandStack.pop();
                operandStack.push(a<=b); break;
            }
            case Lteqf: {
                double a=asfloat(operandStack.top());
                operandStack.pop();
                double b=asfloat(operandStack.top());
                operandStack.pop();
                operandStack.push(asint(a <= b)); break;
            }
            case Lti: {
                int64_t a=operandStack.top();
                operandStack.pop();
                int64_t b=operandStack.top();
                operandStack.pop();
                operandStack.push(a<b); break;
            }
            case Ltf: {
                double a=asfloat(operandStack.top());
                operandStack.pop();
                double b=asfloat(operandStack.top());
                operandStack.pop();
                operandStack.push(asint(a < b)); break;
            }
            case Gti: {
                int64_t a=operandStack.top();
                operandStack.pop();
                int64_t b=operandStack.top();
                operandStack.pop();
                operandStack.push(a>b); break;
            }
            case Gtf: {
                double a=asfloat(operandStack.top());
                operandStack.pop();
                double b=asfloat(operandStack.top());
                operandStack.pop();
                operandStack.push(asint(a > b)); break;
            }
            case Gteqi: {
                int64_t a=operandStack.top();
                operandStack.pop();
                int64_t b=operandStack.top();
                operandStack.pop();
                operandStack.push(a>=b); break;
            }
            case Gteqf: {
                double a=asfloat(operandStack.top());
                operandStack.pop();
                double b=asfloat(operandStack.top());
                operandStack.pop();
                operandStack.push(asint(a >= b)); break;
            }
            case Eqi: {
                int64_t a=operandStack.top();
                operandStack.pop();
                int64_t b=operandStack.top();
                operandStack.pop();
                operandStack.push(a==b); break;
            }
            case Eqf: {
                double a=asfloat(operandStack.top());
                operandStack.pop();
                double b=asfloat(operandStack.top());
                operandStack.pop();
                operandStack.push(asint(a == b)); break;
            }
            case Neqi: {
                int64_t a=operandStack.top();
                operandStack.pop();
                int64_t b=operandStack.top();
                operandStack.pop();
                operandStack.push(a!=b); break;
            }
            case Neqf: {
                double a=asfloat(operandStack.top());
                operandStack.pop();
                double b=asfloat(operandStack.top());
                operandStack.pop();
                operandStack.push(asint(a != b)); break;
            }
            default: break;
        }
        PC += 2;
    }

    void run() {
        if (PC >= memory.size()) return;

        while (true) {
            auto instr0 = memory[PC];
            if (instr0 == End) return;
            step();
        }
    }
    

private:
    uint64_t PC = 0;
    uint64_t RAM = 0;
    std::vector<int64_t> memory;
    std::stack<uint64_t> operandStack;
    std::stack<uint64_t> addressStack;
    std::map<int64_t, void (VirtualMachine::*)()> stdlib;

    std::ostream &out;

    void printf() {
        auto straddress = operandStack.top();
        operandStack.pop();

        while (true) {
            char c = (char)memory[straddress];
            if (c == '\0') break;
            if (c == '%') {
                straddress++;
                char c1 = (char)memory[straddress];
                if (c1 == 'd' || c1 == 'i') {
                    auto v = operandStack.top();
                    operandStack.pop();
                    out << v;
                } else if (c1 == 'f' || c1 == 'g') {
                    auto v = asfloat(operandStack.top());
                    operandStack.pop();
                    out << v;
                }
            } else {
                out << c;
            }
            straddress++;
        }
    }

};