#include "ASTGen.h"

#include <algorithm>

using namespace std;

bool typesEqual(typep a, typep b);

void ASTGen::resolveAliases() {
    // Propagate alias resolving
    bool cont = true;
    while (cont) {
        cont = false;
        for (auto& it : ast.aliases) {
            auto val = it.second;
            it.second = replaceAlias(val, true);
            if (!cont && !typesEqual(val, it.second)) cont = true;
        }
    }
    // Replace types that are aliased with actual types
    for (auto& f0 : ast.functions) {
        if (auto f = dynamic_pointer_cast<FunctionDef>(f0.second)) {
            for (auto &a : f->args) {
                a.type = replaceAlias(a.type, false);
            }
            f->ret = replaceAlias(f->ret, false);
            for (auto& sb : f->body) {
                replaceAliases2(sb);
            }
        }
    }
    ast.aliases.clear();
}

typep ASTGen::replaceAlias(typep a, bool failOnObj) {
    if (auto a0 = dynamic_pointer_cast<TypeTuple>(a)) {
        vector<typep> t1(a0->t.size());
        transform(a0->t.begin(), a0->t.end(), t1.begin(), [&](auto x) {
            return replaceAlias(x, failOnObj);
        });
        return typep(new TypeTuple(t1));
    } else if (auto a0 = dynamic_pointer_cast<TypeObj>(a)) {
        auto it = ast.aliases.find(a0->name);
        if (it != ast.aliases.end()) {
            return it->second;
        } else {
            if (failOnObj) throw "can't find target for alias";
        }
    } else if (auto a0 = dynamic_pointer_cast<TypeFunction>(a)) {
        vector<typep> args0(a0->args);
        transform(a0->args.begin(), a0->args.end(), args0.begin(), [&](auto x) {
            return replaceAlias(x, failOnObj);
        });
        return typep(new TypeFunction(args0, replaceAlias(a0->ret, failOnObj)));
    } else if (auto a0 = dynamic_pointer_cast<TypeList>(a)) {
        return typep(new TypeList(replaceAlias(a0->t, failOnObj)));
    }
    return a;
}

void ASTGen::replaceAliases2(statp sb) {
    if (auto s = dynamic_pointer_cast<AssignStat>(sb)) {
        s->left->type = replaceAlias(s->left->type, false);
        for (auto a : s->left->suffixes) {
            if (auto a0 = dynamic_pointer_cast<IndexSuffix>(a))
                replaceAliases2(a0->i);
        }
        replaceAliases2(s->right);
    } else if (auto s = dynamic_pointer_cast<FuncCallStat>(sb)) {
        for (auto a : s->func->suffixes) {
            if (auto a0 = dynamic_pointer_cast<IndexSuffix>(a))
                replaceAliases2(a0->i);
        }
        for (auto e : s->args) replaceAliases2(e);
    } else if (auto s = dynamic_pointer_cast<WhileStat>(sb)) {
        replaceAliases2(s->cond);
        replaceAliases2(s->body);
    } else if (auto s = dynamic_pointer_cast<IfStat>(sb)) {
        replaceAliases2(s->cond);
        replaceAliases2(s->thenbody);
        replaceAliases2(s->elsebody);
    } else if (auto s = dynamic_pointer_cast<BlockStat>(sb)) {
        for (auto bsb : s->stats) {
            replaceAliases2(bsb);
        }
    } else if (auto s = dynamic_pointer_cast<ReturnStat>(sb)) {
        replaceAliases2(s->ret);
    }
}

void ASTGen::replaceAliases2(expp eb) {
    if (auto e = dynamic_pointer_cast<ObjExp>(eb)) {
        for (auto &f : e->fields) replaceAliases2(f.e);
    } else if (auto e = dynamic_pointer_cast<ListExp>(eb)) {
        for (auto el : e->elements) replaceAliases2(el);
    } else if (auto e = dynamic_pointer_cast<TupleExp>(eb)) {
        for (auto el : e->elements) replaceAliases2(el);
    } else if (auto e = dynamic_pointer_cast<MemberExp>(eb)) {
        replaceAliases2(e->left);
    } else if (auto e = dynamic_pointer_cast<IndexExp>(eb)) {
        replaceAliases2(e->left);
        replaceAliases2(e->index);
    } else if (auto e = dynamic_pointer_cast<CallExp>(eb)) {
        for (auto a : e->func->suffixes) {
            if (auto a0 = dynamic_pointer_cast<IndexSuffix>(a))
                replaceAliases2(a0->i);
        }
        for (auto el : e->args) replaceAliases2(el);
    } else if (auto e = dynamic_pointer_cast<UnaryOpExp>(eb)) {
        replaceAliases2(e->e);
    } else if (auto e = dynamic_pointer_cast<BinOpExp>(eb)) {
        replaceAliases2(e->left);
        replaceAliases2(e->right);
    } else if (auto e = dynamic_pointer_cast<TernaryExp>(eb)) {
        replaceAliases2(e->then);
        replaceAliases2(e->cond);
        replaceAliases2(e->els);
    } else if (auto e = dynamic_pointer_cast<CastExp>(eb)) {
        replaceAliases2(e->e);
        e->t = replaceAlias(e->t, false);
    }
}
