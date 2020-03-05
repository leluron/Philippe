#include "VirtualMachine.h"

double asfloat(int64_t a) {
    return *(double*)(&a);
}

int64_t asint(double a) {
    return *(int64_t*)(&a);
}

void VirtualMachine::step() {
    if (PC >= memory.size()) return;

    auto instr0 = memory[PC];
    auto instr1 = memory[PC+1];

    switch (instr0) {
        case Noop: break;
        case LoadS: {
            operandStack.push(instr1); 
            PC+=2; break;
        }
        case LoadM: {
            operandStack.push(memory[instr1]);
            PC+=2; break;
        }
        case Store: {
            auto v = operandStack.top();
            operandStack.pop();
            memory[instr1] = v;
            PC += 2; break;
        }
        case Alloc: {
            auto p = RAM;
            RAM += instr1;
            operandStack.push(RAM);
            PC += 2;
            break;
        }
        case Free : PC+= 2; break;
        case Call: {
            if (instr1 >= RESERVED_FUNCS) {
                auto func = stdlib[instr1];
                (this->*func)();
                PC += 2;
            } else {
                addressStack.push(PC);
                PC = instr1;
            }
            break;
        }
        case Return: {
            PC = addressStack.top();
            addressStack.pop();
            break;
        }
        case IfJump: {
            if (operandStack.top()) PC = instr1;
            else PC += 2;
            operandStack.pop();
            break;
        }
        case Jump: {
            PC = instr1; break;
        }
        case Castfi: {
            auto f = asfloat(operandStack.top());
            operandStack.pop();
            operandStack.push((int64_t)f); 
            PC++; break;
        }
        case Castif: {
            auto i = operandStack.top();
            operandStack.pop();
            operandStack.push(asint((double)i)); 
            PC++; break;
        }
        case Not: {
            auto v = operandStack.top();
            operandStack.pop();
            operandStack.push(!v);
            PC++; break;
        }
        case And: {
            auto a = operandStack.top();
            operandStack.pop();
            auto b = operandStack.top();
            operandStack.pop();
            operandStack.push(a && b); 
            PC++; break;
        }
        case Or: {
            auto a = operandStack.top();
            operandStack.pop();
            auto b = operandStack.top();
            operandStack.pop();
            operandStack.push(a || b); 
            PC++; break;
        }
        case Usubi: {
            auto a = operandStack.top();
            operandStack.pop();
            operandStack.push(- a); 
            PC++; break;
        }
        case Usubf: {
            auto a = asfloat(operandStack.top());
            operandStack.pop();
            operandStack.push(asint(-a)); 
            PC++; break;
        }
        case Powi: {
            int64_t a=operandStack.top();
            operandStack.pop();
            int64_t b=operandStack.top();
            operandStack.pop();
            int64_t p = 1;
            for (int i=0;i<b;i++) p *= a;
            operandStack.push(p);
            PC++; 
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
            PC++; 
            break;
        }
        case Muli: {
            int64_t a=operandStack.top();
            operandStack.pop();
            int64_t b=operandStack.top();
            operandStack.pop();
            operandStack.push(a*b); 
            PC++; break;
        }
        case Mulf: {
            double a=asfloat(operandStack.top());
            operandStack.pop();
            double b=asfloat(operandStack.top());
            operandStack.pop();
            operandStack.push(asint(a*b)); 
            PC++; break;
        }
        case Divi: {
            int64_t a=operandStack.top();
            operandStack.pop();
            int64_t b=operandStack.top();
            operandStack.pop();
            operandStack.push(a/b); 
            PC++; break;
        }
        case Divf: {
            double a=asfloat(operandStack.top());
            operandStack.pop();
            double b=asfloat(operandStack.top());
            operandStack.pop();
            operandStack.push(asint(a / b)); 
            PC++; break;
        }
        case Modi: {
            int64_t a=operandStack.top();
            operandStack.pop();
            int64_t b=operandStack.top();
            operandStack.pop();
            operandStack.push(a%b); 
            PC++; break;
        }
        case Addi: {
            int64_t a=operandStack.top();
            operandStack.pop();
            int64_t b=operandStack.top();
            operandStack.pop();
            operandStack.push(a+b); 
            PC++; break;
        }
        case Addf: {
            double a=asfloat(operandStack.top());
            operandStack.pop();
            double b=asfloat(operandStack.top());
            operandStack.pop();
            operandStack.push(asint(a + b)); 
            PC++; break;
        }
        case Subi: {
            int64_t a=operandStack.top();
            operandStack.pop();
            int64_t b=operandStack.top();
            operandStack.pop();
            operandStack.push(a-b); 
            PC++; break;
        }
        case Subf: {
            double a=asfloat(operandStack.top());
            operandStack.pop();
            double b=asfloat(operandStack.top());
            operandStack.pop();
            operandStack.push(asint(a - b)); 
            PC++; break;
        }
        case Lteqi: {
            int64_t a=operandStack.top();
            operandStack.pop();
            int64_t b=operandStack.top();
            operandStack.pop();
            operandStack.push(a<=b); 
            PC++; break;
        }
        case Lteqf: {
            double a=asfloat(operandStack.top());
            operandStack.pop();
            double b=asfloat(operandStack.top());
            operandStack.pop();
            operandStack.push(asint(a <= b)); 
            PC++; break;
        }
        case Lti: {
            int64_t a=operandStack.top();
            operandStack.pop();
            int64_t b=operandStack.top();
            operandStack.pop();
            operandStack.push(a<b); 
            PC++; break;
        }
        case Ltf: {
            double a=asfloat(operandStack.top());
            operandStack.pop();
            double b=asfloat(operandStack.top());
            operandStack.pop();
            operandStack.push(asint(a < b)); 
            PC++; break;
        }
        case Gti: {
            int64_t a=operandStack.top();
            operandStack.pop();
            int64_t b=operandStack.top();
            operandStack.pop();
            operandStack.push(a>b); 
            PC++; break;
        }
        case Gtf: {
            double a=asfloat(operandStack.top());
            operandStack.pop();
            double b=asfloat(operandStack.top());
            operandStack.pop();
            operandStack.push(asint(a > b)); 
            PC++; break;
        }
        case Gteqi: {
            int64_t a=operandStack.top();
            operandStack.pop();
            int64_t b=operandStack.top();
            operandStack.pop();
            operandStack.push(a>=b); 
            PC++; break;
        }
        case Gteqf: {
            double a=asfloat(operandStack.top());
            operandStack.pop();
            double b=asfloat(operandStack.top());
            operandStack.pop();
            operandStack.push(asint(a >= b)); 
            PC++; break;
        }
        case Eqi: {
            int64_t a=operandStack.top();
            operandStack.pop();
            int64_t b=operandStack.top();
            operandStack.pop();
            operandStack.push(a==b); 
            PC++; break;
        }
        case Eqf: {
            double a=asfloat(operandStack.top());
            operandStack.pop();
            double b=asfloat(operandStack.top());
            operandStack.pop();
            operandStack.push(asint(a == b)); 
            PC++; break;
        }
        case Neqi: {
            int64_t a=operandStack.top();
            operandStack.pop();
            int64_t b=operandStack.top();
            operandStack.pop();
            operandStack.push(a!=b); 
            PC++; break;
        }
        case Neqf: {
            double a=asfloat(operandStack.top());
            operandStack.pop();
            double b=asfloat(operandStack.top());
            operandStack.pop();
            operandStack.push(asint(a != b)); 
            PC++; break;
        }
        default: break;
    }
}

void VirtualMachine::run() {
    if (PC >= memory.size()) return;

    while (true) {
        auto instr0 = memory[PC];
        if (instr0 == End) return;
        step();
    }
}

void VirtualMachine::printf() {
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