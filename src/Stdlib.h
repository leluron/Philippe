#pragma once 
#include "AST.h"

class PrintfFunc : public NativeFunction {};

File loadStd() {
    File f;
    f.functions["printf"] = defp(
        new NativeFunctionDef(
            {
                typep(new TypeString()),
                typep(new TypeVariable())
            },
            typep(new TypeNil()),
            std::shared_ptr<NativeFunction>(new PrintfFunc())
        )
    );
    return f;
}