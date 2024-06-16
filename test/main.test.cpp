#include <gtest/gtest.h>
#include "Compiler.h"

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);

    unlogic::Compiler::InitializeCompilerRuntime();

    return RUN_ALL_TESTS();
}
