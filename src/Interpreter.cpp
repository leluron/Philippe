#include "AST.h"
#include "Interpreter.h"

void evalBlock(InterpreterContext &c, block b) {
    for (auto s : b) {
        s->eval(c);
    }
}

void AssignStat::eval(InterpreterContext &c) {
    auto r = right->eval(c);
    if (left.size() > 1) {
        if (r->kind() != ValueKind::Tuple) throw;
        auto tu = dynamic_cast<TupleValue*>(r.get())->elements;
        if (left.size() != tu.size()) throw;
        for (int i=0;i<left.size();i++) {
            left[i]->leval(c) = tu[i];
        }
    } else {
        left[0]->leval(c) = r;
    }
}

void FuncCallStat::eval(InterpreterContext &c) {
    CallExp(func, args).eval(c);
}

void WhileStat::eval(InterpreterContext &c) {
    while (true) {
        auto v = cond->eval(c);
        if (v->kind() != ValueKind::Bool) throw "While condition must be a bool";
        if (!dynamic_cast<BoolValue*>(v.get())->val) return;
        body->eval(c);
    }
}

void IfStat::eval(InterpreterContext &c) {
    if (cond->eval(c))
        thenbody->eval(c);
    else
        elsebody->eval(c);
}

void ForStat::eval(InterpreterContext &c) {
    auto l = list->eval(c);

    if (l->kind() == ValueKind::List) {
        for (auto i : dynamic_cast<ListValue*>(l.get())->elements) {
            c.newFrame();
            c.add(name, i);
            body->eval(c);
            c.popFrame();
        }
    } else if (l->kind() == ValueKind::Range) {
        auto r = dynamic_cast<RangeValue*>(l.get());
        for (auto i=r->beg.val;i<r->end.val;i++) {
            c.newFrame();
            c.add(name, valp(new IntValue(i)));
            body->eval(c);
            c.popFrame();
        }
    } else throw;

}

void BlockStat::eval(InterpreterContext &c) {
    evalBlock(c, stats);
}

void BreakStat::eval(InterpreterContext &c) {
    
}

void ReturnStat::eval(InterpreterContext &c) {
    
}

void EmptyStat::eval(InterpreterContext &c) {}

valp NilExp::eval(InterpreterContext &c) {
    return valp(new NilValue());
}

valp BoolExp::eval(InterpreterContext &c) {
    return valp(new BoolValue(val));
}

valp IntExp::eval(InterpreterContext &c) {
    return valp(new IntValue(val));
}

valp FloatExp::eval(InterpreterContext &c) {
    return valp(new FloatValue(val));
}

valp StringExp::eval(InterpreterContext &c) {
    return valp(new StringValue(val));
}

valp IdExp::eval(InterpreterContext &c) {
    return c.get(name);
}

valp ObjExp::eval(InterpreterContext &c) {
    auto o = new ObjValue();
    for (auto f : fields) {
        o->add(f.name, f.e->eval(c));
    }
    return valp(o);
}

valp ListExp::eval(InterpreterContext &c) {
    auto l = new ListValue();
    for (auto e : elements) {
        l->add(e->eval(c));
    }
    return valp(l);
}

valp RangeExp::eval(InterpreterContext &c) {
    auto beg = begin->eval(c);
    auto end0 = end->eval(c);

    if (beg->kind() != ValueKind::Int || 
        end0->kind() != ValueKind::Int)
        throw;

    return valp(new RangeValue(
        *dynamic_cast<IntValue*>(beg.get()),
        *dynamic_cast<IntValue*>(end0.get())
        ));
}

valp TupleExp::eval(InterpreterContext &c) {
    auto el = std::vector<valp>();
    for (auto e : elements) {
        el.push_back(e->eval(c));
    }
    return valp(new TupleValue(el));
}

valp FuncExp::eval(InterpreterContext &c) {
    return valp(new FuncValue(*this));
}

valp MemberExp::eval(InterpreterContext &c) {
    auto l = left->eval(c);

    if (l->kind() != ValueKind::Obj) throw;

    return dynamic_cast<ObjValue*>(l.get())->get(name);
}

valp IndexExp::eval(InterpreterContext &c) {
    auto l = left->eval(c);
    auto ind = index->eval(c);

    if (ind->kind() != ValueKind::Int) throw;

    int ind0 = dynamic_cast<IntValue*>(ind.get())->val;

    if (l->kind() == ValueKind::List)
        return dynamic_cast<ListValue*>(l.get())->get(ind0);
    else if (l->kind() == ValueKind::Tuple)
        return dynamic_cast<TupleValue*>(l.get())->get(ind0);
    else throw;
}

valp CallExp::eval(InterpreterContext &c) {
    auto f = func->eval(c);

    if (f->kind() == ValueKind::Func) {
        auto f0 = dynamic_cast<FuncValue*>(f.get())->func;

        c.newFrame();
        
        for (int i=0;i<args.size();i++) {
            c.add(f0.args[i], args[i]->eval(c));
        }

        evalBlock(c, f0.body.stats);
        auto ret = f0.body.ret->eval(c);
        c.popFrame();
        return ret;
    } else if (f->kind() == ValueKind::NativeFunc) {
        auto f0 = dynamic_cast<NativeFuncValue*>(f.get());
        std::vector<valp> argsv;
        for (int i=0;i<args.size();i++) {
            argsv.push_back(args[i]->eval(c));
        }
        return f0->exec(c, argsv);
    } else throw "Unsupported type for calling";
}

valp UnaryOpExp::eval(InterpreterContext &c) {
    auto a = e->eval(c);
    return a->unaryOp(op);
}

valp BinOpExp::eval(InterpreterContext &c) {
    auto l = left->eval(c);
    auto r = right->eval(c);
    return l->binOp(op, r);
}

valp TernaryExp::eval(InterpreterContext &c) {
    auto co = cond->eval(c);
    if (co->kind() != ValueKind::Bool) throw;

    auto c0 = dynamic_cast<BoolValue*>(co.get())->val;

    if (c0) return then->eval(c);
    else return els->eval(c);
}

valp& NilExp::leval(InterpreterContext &c) {
    throw;
}

valp& BoolExp::leval(InterpreterContext &c) {
    throw;
}

valp& IntExp::leval(InterpreterContext &c) {
    throw;
}

valp& FloatExp::leval(InterpreterContext &c) {
    throw;
}

valp& StringExp::leval(InterpreterContext &c) {
    throw;
}

valp& IdExp::leval(InterpreterContext &c) {
    return c.getLeftValue(name);
}

valp& ObjExp::leval(InterpreterContext &c) {
    throw;
}

valp& ListExp::leval(InterpreterContext &c) {
    throw;
}

valp& RangeExp::leval(InterpreterContext &c) {
    throw;
}

valp& TupleExp::leval(InterpreterContext &c) {
    throw;
}

valp& FuncExp::leval(InterpreterContext &c) {
    throw;
}

valp& MemberExp::leval(InterpreterContext &c) {
    throw;
}

valp& IndexExp::leval(InterpreterContext &c) {
    throw;
}

valp& CallExp::leval(InterpreterContext &c) {
    throw;
}

valp& UnaryOpExp::leval(InterpreterContext &c) {
    throw;
}

valp& BinOpExp::leval(InterpreterContext &c) {
    throw;
}

valp& TernaryExp::leval(InterpreterContext &c) {
    auto co = cond->eval(c);
    if (co->kind() != ValueKind::Bool) throw;

    auto c0 = dynamic_cast<BoolValue*>(co.get())->val;

    if (c0) return then->leval(c);
    else return els->leval(c);
}

valp IntValue::binOp(BinOp op, valp right) {
    if (right->kind() == ValueKind::Int) {
        auto v1 = dynamic_cast<IntValue*>(right.get())->val; 
        if (op == BinOp::Pow) {
            long l = 1;
            for (int i=0;i<v1;i++) l *= val;
            return valp(new IntValue(l));
        } else if (op == BinOp::Mul) {
            return valp(new IntValue(val*v1));
        } else if (op == BinOp::Div) {
            return valp(new IntValue(val/v1));
        } else if (op == BinOp::Mod) {
            return valp(new IntValue(val%v1));
        } else if (op == BinOp::Plus) {
            return valp(new IntValue(val+v1));
        } else if (op == BinOp::Minus) {
            return valp(new IntValue(val-v1));
        } else if (op == BinOp::Lteq) {
            return valp(new BoolValue(val<=v1));
        } else if (op == BinOp::Lt) {
            return valp(new BoolValue(val<v1));
        } else if (op == BinOp::Gt) {
            return valp(new BoolValue(val>v1));
        } else if (op == BinOp::Gteq) {
            return valp(new BoolValue(val>=v1));
        } else if (op == BinOp::Eq) {
            return valp(new BoolValue(val==v1));
        } else if (op == BinOp::Neq) {
            return valp(new BoolValue(val!=v1));
        } else throw "Unsupported int operation";
    } else if (right->kind() == ValueKind::Float) {
        return FloatValue(*this).binOp(op, right);
    } else throw "Unsupported int operation";
}

valp FloatValue::binOp(BinOp op, valp right) {
    if (right->kind() == ValueKind::Float) {
        auto v1 = dynamic_cast<FloatValue*>(right.get())->val; 
        if (op == BinOp::Pow) {
            double l = 1.0;
            for (int i=0;i<(long)v1;i++) l *= val;
            return valp(new FloatValue(l));
        } else if (op == BinOp::Mul) {
            return valp(new FloatValue(val*v1));
        } else if (op == BinOp::Div) {
            return valp(new FloatValue(val/v1));
        } else if (op == BinOp::Plus) {
            return valp(new FloatValue(val+v1));
        } else if (op == BinOp::Minus) {
            return valp(new FloatValue(val-v1));
        } else if (op == BinOp::Lteq) {
            return valp(new BoolValue(val<=v1));
        } else if (op == BinOp::Lt) {
            return valp(new BoolValue(val<v1));
        } else if (op == BinOp::Gt) {
            return valp(new BoolValue(val>v1));
        } else if (op == BinOp::Gteq) {
            return valp(new BoolValue(val>=v1));
        } else if (op == BinOp::Eq) {
            return valp(new BoolValue(val==v1));
        } else if (op == BinOp::Neq) {
            return valp(new BoolValue(val!=v1));
        } else throw "Unsupported float operation";
    } else if (right->kind() == ValueKind::Int) {
        return binOp(op, valp(new FloatValue(*dynamic_cast<IntValue*>(right.get()))));
    } else throw "Unsupported float operation";
}

valp NativePrint::exec(InterpreterContext &c, std::vector<valp> args) {
    if (args.size() != 1) throw;
    auto arg = args[0];
    if (arg->kind() == ValueKind::String) {
        auto str = dynamic_cast<StringValue*>(arg.get())->val;
        c.out << str << std::endl;
    } else if (arg->kind() == ValueKind::Int) {
        auto str = dynamic_cast<IntValue*>(arg.get())->val;
        c.out << str << std::endl;
    } else if (arg->kind() == ValueKind::Float) {
        auto str = dynamic_cast<FloatValue*>(arg.get())->val;
        c.out << str << std::endl;
    } else if (arg->kind() == ValueKind::Bool) {
        auto str = dynamic_cast<BoolValue*>(arg.get())->val;
        c.out << (str?"true":"false") << std::endl;
    } else {
        throw "Unsupported type for printing";
    }
    return valp(new NilValue());
}

StdLib::StdLib() {
    add("println", valp(new NativePrint()));
}