// Integration tests: input via CommandShellIO flowing through CommandShell
#ifdef CommandShell
#undef CommandShell
#endif

#include "../src/CommandShellIO.hpp"
#include "../src/CommandShell.hpp"

#include <gtest/gtest.h>
#include <string>
#include <vector>

using commandshell::CommandShell;
using commandshell::CommandShellIO;
using commandshell::ComponentCommands;
using commandshell::CommandDetails;

namespace {
    struct Capture {
        std::vector<std::string> out;
        void append(const std::string& s) { out.push_back(s); }
    };
}

TEST(CommandShellIntegrationTests, EndToEndExecutesRegisteredCommand)
{
    CommandShell shell;

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
    shell.registerComponent(sys);

    // Wire IO, disable echo so only command output is captured
    Capture cap;
    CommandShellIO io(shell, /*echoInput=*/false);
    io.setOutputCallback([&cap](const std::string& s) { cap.append(s); });

    std::string line = "sys echo hello world\n";
    io.input(line);

    ASSERT_GE(cap.out.size(), 2u);
    EXPECT_EQ(cap.out[0], std::string("cmd> "));
    EXPECT_EQ(cap.out[1], std::string("hello world\n"));
}

TEST(CommandShellIntegrationTests, EndToEndExecutesWithNoNewlineOption)
{
    CommandShell shell;

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
    shell.registerComponent(sys);

    Capture cap;
    CommandShellIO io(shell, /*echoInput=*/false);
    io.setOutputCallback([&cap](const std::string& s) { cap.append(s); });

    std::string line = "sys echo -n hello\n";
    io.input(line);

    ASSERT_GE(cap.out.size(), 2u);
    EXPECT_EQ(cap.out[0], std::string("cmd> "));
    EXPECT_EQ(cap.out[1], std::string("hello")); // no trailing newline
}
