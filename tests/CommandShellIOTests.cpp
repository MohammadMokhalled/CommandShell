// Unit tests for CommandShellIO
#ifdef CommandShell
#undef CommandShell
#endif

#include "../inc/CommandShellIO.hpp"
#include "../inc/CommandShell.hpp"

#include <gtest/gtest.h>
#include <string>
#include <sstream>
#include <vector>
#include <cstring>

using commandshell::CommandShell;
using commandshell::CommandShellIO;
using commandshell::Command;
using commandshell::ComponentCommands;
using commandshell::CommandDetails;

// Simple fixture to provide a CommandShell instance and capture callback output
class CommandShellIOTest : public ::testing::Test {
protected:
    void SetUp() override {
        instanceName = std::string("test-instance");
        shell = &CommandShell::getInstance(instanceName);
        captured.clear();
    }

    void appendCapture(const std::string& s) {
        captured.push_back(s);
    }

    std::string joined() const {
        std::ostringstream os;
        for (const auto& s : captured) os << s;
        return os.str();
    }

    std::string instanceName;
    CommandShell* shell{nullptr};
    std::vector<std::string> captured;
};

// Helper subclass to expose protected methods for testing
class TestableCommandShellIO : public CommandShellIO {
public:
    using CommandShellIO::CommandShellIO; // inherit constructors
    using CommandShellIO::splitInput;     // expose as public
    using CommandShellIO::parseCommand;   // expose as public
};

TEST_F(CommandShellIOTest, EchoesInputViaCallbackSingleChunk) {
    ASSERT_NE(shell, nullptr);
    CommandShellIO io(*shell, /*echoInput=*/true);
    io.setOutputCallback([this](const std::string& s) { appendCapture(s); });

    std::string line = "echo HelloIO\n";
    io.input(line);

    ASSERT_EQ(captured.size(), 1u);
    EXPECT_EQ(captured[0], line);
}

TEST_F(CommandShellIOTest, EchoesInputAcrossMultipleChunksUntilNewline) {
    ASSERT_NE(shell, nullptr);
    CommandShellIO io(*shell, /*echoInput=*/true);
    io.setOutputCallback([this](const std::string& s) { appendCapture(s); });

    std::string p1 = "echo";
    std::string p2 = " ";
    std::string p3 = "World";
    std::string p4 = "\n";

    io.input(p1);
    io.input(p2);
    io.input(p3);
    io.input(p4);

    ASSERT_EQ(captured.size(), 4u);
    EXPECT_EQ(captured[0], p1);
    EXPECT_EQ(captured[1], p2);
    EXPECT_EQ(captured[2], p3);
    EXPECT_EQ(captured[3], p4);
}

TEST_F(CommandShellIOTest, SplitInputBasicAndMultipleSpaces) {
    ASSERT_NE(shell, nullptr);
    TestableCommandShellIO io(*shell, /*echoInput=*/false);

    // Multiple spaces and standard tokens
    std::string s = "comp  cmd   arg1  -v   arg2";
    auto parts = io.splitInput(s);

    ASSERT_EQ(parts.size(), 5u);
    EXPECT_EQ(parts[0], "comp");
    EXPECT_EQ(parts[1], "cmd");
    EXPECT_EQ(parts[2], "arg1");
    EXPECT_EQ(parts[3], "-v");
    EXPECT_EQ(parts[4], "arg2");
}

TEST_F(CommandShellIOTest, SplitInputLeadingTrailingSpacesAndNewline) {
    ASSERT_NE(shell, nullptr);
    TestableCommandShellIO io(*shell, /*echoInput=*/false);

    std::string s = "   comp cmd   \n"; // newline is not treated as a space by splitInput
    auto parts = io.splitInput(s);

    ASSERT_EQ(parts.size(), 3u);
    EXPECT_EQ(parts[0], "comp");
    EXPECT_EQ(parts[1], "cmd");
    // Third token is the trailing newline chunk
    EXPECT_EQ(parts[2], std::string("\n"));
}

TEST_F(CommandShellIOTest, ParseCommandParsesArgumentsAndOptions) {
    ASSERT_NE(shell, nullptr);
    TestableCommandShellIO io(*shell, /*echoInput=*/false);

    std::string s = "sys echo hello -n world";
    auto parts = io.splitInput(s);
    ASSERT_GE(parts.size(), 2u);

    Command cmd = io.parseCommand(parts);
    EXPECT_EQ(cmd.component, "sys");
    EXPECT_EQ(cmd.command, "echo");

    // arguments should be ["hello", "world"] and options ["-n"]
    ASSERT_EQ(cmd.arguments.size(), 2u);
    EXPECT_EQ(cmd.arguments[0], "hello");
    EXPECT_EQ(cmd.arguments[1], "world");
    ASSERT_EQ(cmd.options.size(), 1u);
    EXPECT_EQ(cmd.options[0], "-n");
}

TEST_F(CommandShellIOTest, ParseCommandWithOnlyComponentAndCommand) {
    ASSERT_NE(shell, nullptr);
    TestableCommandShellIO io(*shell, /*echoInput=*/false);

    std::string s = "tool run";
    auto parts = io.splitInput(s);
    ASSERT_EQ(parts.size(), 2u);

    Command cmd = io.parseCommand(parts);
    EXPECT_EQ(cmd.component, "tool");
    EXPECT_EQ(cmd.command, "run");
    EXPECT_TRUE(cmd.arguments.empty());
    EXPECT_TRUE(cmd.options.empty());
}

TEST_F(CommandShellIOTest, CharPointerPromptOverloadEchoesInput) {
    ASSERT_NE(shell, nullptr);
    CommandShellIO io(*shell, /*echoInput=*/true);
    io.setOutputCallback([this](const std::string& s) { appendCapture(s); });

    const char* part = "echo Zed\n";
    io.input(const_cast<char*>(part), std::strlen(part));

    ASSERT_EQ(captured.size(), 1u);
    EXPECT_EQ(captured[0], std::string(part));
}

TEST_F(CommandShellIOTest, EchoDisabledDoesNotInvokeCallback) {
    ASSERT_NE(shell, nullptr);
    CommandShellIO io(*shell, /*echoInput=*/false);

    // Set a callback; it should not be triggered because echo is disabled
    io.setOutputCallback([this](const std::string& s) { appendCapture(s); });

    std::string part1 = "no-echo";
    std::string part2 = "\n";
    io.input(part1);
    io.input(part2);

    EXPECT_TRUE(captured.empty());
}

TEST_F(CommandShellIOTest, EmptyInputWithEchoStillInvokesCallback) {
    ASSERT_NE(shell, nullptr);
    CommandShellIO io(*shell, /*echoInput=*/true);
    io.setOutputCallback([this](const std::string& s) { appendCapture(s); });

    std::string empty = "";
    io.input(empty);
    // Follow with newline to simulate submit; still, we only expect echo of what was provided
    std::string nl = "\n";
    io.input(nl);

    ASSERT_EQ(captured.size(), 2u);
    EXPECT_EQ(captured[0], "");
    EXPECT_EQ(captured[1], "\n");
}

// End-to-end demonstration: register a component/command and execute via IO
TEST_F(CommandShellIOTest, EndToEndExecutesRegisteredCommand) {
    ASSERT_NE(shell, nullptr);

    // Register sample component and echo command
    ComponentCommands sys{"sys", "System commands"};
    CommandDetails echoCmd{
        "echo",
        "Echo arguments like /bin/echo",
        [](const std::vector<std::string>& args, const std::vector<std::string>& opts) -> std::string {
            bool noNewline = false;
            for (const auto& o : opts) {
                if (o == "-n" || o == "--no-newline") { noNewline = true; }
            }
            std::string out;
            for (size_t i = 0; i < args.size(); ++i) {
                if (i) out += ' ';
                out += args[i];
            }
            if (!noNewline) out += '\n';
            return out;
        }
    };
    sys.addCommand(echoCmd);
    shell->registerComponent(sys);

    // Wire IO, disable echo so only command output is captured
    CommandShellIO io(*shell, /*echoInput=*/false);
    io.setOutputCallback([this](const std::string& s) { appendCapture(s); });

    std::string line = "sys echo hello world\n";
    io.input(line);

    ASSERT_EQ(captured.size(), 1u);
    EXPECT_EQ(captured[0], std::string("hello world\n"));
}
