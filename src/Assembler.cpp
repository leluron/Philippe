#include "Assembler.h"
#include <antlr4-runtime/antlr4-runtime.h>
#include "parser/BytecodeParser.h"
#include "parser/BytecodeLexer.h"
#include "parser/BytecodeBaseVisitor.h"

using namespace std;
using namespace antlr4;

using addressmap = std::map<std::string, uint64_t>;

addressmap stdlib = {
    {"printf", Printf}
};

vector<int64_t> stringArrayToCode(std::string str) {
    vector<int64_t> s;
    for (int i=1;i<str.size()-1;i++) {
        char c = str[i];
        if (c == '\\') {
            c = str[++i];
            if (c == 'n') s.push_back('\n');
            else if (c == '\\') s.push_back('\\');
            else if (c == '"') s.push_back('"');
            else if (c == 'r') s.push_back('\r');
            else if (c == 't') s.push_back('\t');
            else if (c == 'v') s.push_back('\v');
        } else {
            s.push_back(c);
        }
    }
    s.push_back('\0');
    if (s.size()%2 == 1) s.push_back('\0');
    return s;
}

class LabelResolve : BytecodeBaseVisitor {
public:
    virtual antlrcpp::Any visitCode(BytecodeParser::CodeContext *ctx) override {
        a = 0;
        labels.clear();
        visitChildren(ctx);
        return labels;
    }

    virtual antlrcpp::Any visitInstr(BytecodeParser::InstrContext *ctx) override {
        return visitChildren(ctx);
    }

    virtual antlrcpp::Any visitLabel(BytecodeParser::LabelContext *ctx) override {
        labels[ctx->name()->getText()] = a;
        return nullptr;
    }

    virtual antlrcpp::Any visitOpcode(BytecodeParser::OpcodeContext *ctx) override {
        return ctx->o->getText();
    }

    virtual antlrcpp::Any visitOp(BytecodeParser::OpContext *ctx) override {
        if (ctx->stringarray()) return visit(ctx->stringarray());
        else if (ctx->intl || ctx->floatl ) a += 1;
        else {
            string op = visit(ctx->opcode());
            if (op == "loads"
            || op == "loadm"
            || op == "store"
            || op == "alloc"
            || op == "free"
            || op == "call"
            || op == "ifjump"
            || op == "jump") a+=2;
            else a+=1;
        }
        return nullptr;
    }

    virtual antlrcpp::Any visitStringarray(BytecodeParser::StringarrayContext *ctx) override {
        auto str = ctx->STRING()->getText();
        a += stringArrayToCode(str).size();
        return nullptr;
    }
private:
    uint64_t a;
    addressmap labels;
};

class Assembler : BytecodeBaseVisitor {
public:
    vmcode run(std::string assembly) {
        ANTLRInputStream input(assembly);
        BytecodeLexer lexer(&input);
        CommonTokenStream tokens(&lexer);
        BytecodeParser parser(&tokens);    
        BytecodeParser::CodeContext* tree = parser.code();

        auto labels = LabelResolve().visitCode(tree).as<addressmap>();
        this->addresses.insert(labels.begin(), labels.end());
        this->addresses.insert(stdlib.begin(), stdlib.end());
        code.clear();
        visitCode(tree);

        return code;
    }

    virtual antlrcpp::Any visitCode(BytecodeParser::CodeContext *ctx) override {
        return visitChildren(ctx);
    }

    virtual antlrcpp::Any visitInstr(BytecodeParser::InstrContext *ctx) override {
        return visitChildren(ctx);
    }

    virtual antlrcpp::Any visitLabel(BytecodeParser::LabelContext *ctx) override {
        return visitChildren(ctx);
    }

    virtual antlrcpp::Any visitOp(BytecodeParser::OpContext *ctx) override {
        if (ctx->opcode()) {
            string op = visit(ctx->opcode());
            int64_t i0 = Noop;
            if (op == "noop") i0 = Noop;
            else if (op == "loads") i0 = LoadS;
            else if (op == "loadm") i0 = LoadM;
            else if (op == "store") i0 = Store;
            else if (op == "alloc") i0 = Alloc;
            else if (op == "free") i0 = Free;
            else if (op == "call") i0 = Call;
            else if (op == "return") i0 = Return;
            else if (op == "ifjump") i0 = IfJump;
            else if (op == "jump") i0 = Jump;
            else if (op == "castfi") i0 = Castfi;
            else if (op == "castif") i0 = Castif;
            else if (op == "not") i0 = Not;
            else if (op == "and") i0 = And;
            else if (op == "or") i0 = Or;
            else if (op == "usubi") i0 = Usubi;
            else if (op == "usubf") i0 = Usubf;
            else if (op == "powi") i0 = Powi;
            else if (op == "powf") i0 = Powf;
            else if (op == "muli") i0 = Muli;
            else if (op == "mulf") i0 = Mulf;
            else if (op == "divi") i0 = Divi;
            else if (op == "divf") i0 = Divf;
            else if (op == "modi") i0 = Modi;
            else if (op == "addi") i0 = Addi;
            else if (op == "addf") i0 = Addf;
            else if (op == "subi") i0 = Subi;
            else if (op == "subf") i0 = Subf;
            else if (op == "lteqi") i0 = Lteqi;
            else if (op == "lteqf") i0 = Lteqf;
            else if (op == "lti") i0 = Lti;
            else if (op == "ltf") i0 = Ltf;
            else if (op == "gti") i0 = Gti;
            else if (op == "gtf") i0 = Gtf;
            else if (op == "gteqi") i0 = Gteqi;
            else if (op == "gteqf") i0 = Gteqf;
            else if (op == "eqi") i0 = Eqi;
            else if (op == "eqf") i0 = Eqf;
            else if (op == "neqi") i0 = Neqi;
            else if (op == "neqf") i0 = Neqf;
            else if (op == "end") i0 = End;
            code.push_back(i0);

            if (i0 == LoadS || i0 == LoadM || i0 == Store || i0 == Alloc || i0 == Free || i0 == Call ||
                i0 == IfJump || i0 == Jump) {
                int64_t i1 = Noop;
                if (ctx->intliteral()) i1 = visit(ctx->intliteral());
                else if (ctx->floatliteral()) i1 = visit(ctx->floatliteral());
                else if (ctx->name()) i1 = addresses[visit(ctx->name())];
                else throw;
                code.push_back(i1);
            }

        } else if (ctx->stringarray()) {
            vector<int64_t> s = visit(ctx->stringarray());
            code.insert(code.end(), s.begin(), s.end());
        } else if (ctx->intl) {
            code.push_back(visit(ctx->intl).as<int64_t>());
        } else if (ctx->floatl) {
            code.push_back(visit(ctx->floatl).as<int64_t>());
        }
        return nullptr;
    }

    virtual antlrcpp::Any visitIntliteral(BytecodeParser::IntliteralContext *ctx) override {
        stringstream ss;
        ss << ctx->INT()->getText();
        int64_t val;
        ss >> val;
        return val;
    }

    virtual antlrcpp::Any visitFloatliteral(BytecodeParser::FloatliteralContext *ctx) override {
        stringstream ss;
        ss << ctx->FLOAT()->getText();
        double val;
        ss >> val;
        return *(int64_t*)&val;
    }

    virtual antlrcpp::Any visitName(BytecodeParser::NameContext *ctx) override {
        return ctx->ID()->getText();
    }

    virtual antlrcpp::Any visitOpcode(BytecodeParser::OpcodeContext *ctx) override {
        return ctx->o->getText();
    }

    virtual antlrcpp::Any visitStringarray(BytecodeParser::StringarrayContext *ctx) override {
        auto str = ctx->STRING()->getText();
        return stringArrayToCode(str);
    }

    addressmap addresses;
    vmcode code;

};

vmcode assemble(string assembly) {
    return Assembler().run(assembly);
}