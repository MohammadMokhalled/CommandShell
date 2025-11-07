#ifdef CommandShell
#undef CommandShell
#endif
#include "../inc/CommandShell.hpp"
#include <gtest/gtest.h>
#include <string>

// Test fixture for CommandShell tests
class CommandShellTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup code if needed
    }

    void TearDown() override {
        // Cleanup code if needed
    }
};

// Main function for running tests
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
