// Unit tests for CommandShell (direct execution and help rendering)
#include "../inc/CommandShell.hpp"
#include "../inc/CommandTypes.hpp"

#include <gtest/gtest.h>
#include <string>

using commandshell::CommandShell;
using commandshell::Command;
using commandshell::ComponentCommands;
using commandshell::CommandDetails;
using commandshell::OptionDetails;

namespace {
    ComponentCommands makeSysComponent()
    {
        ComponentCommands sys{"sys", "System commands"};

        // Add an option to demonstrate option rendering in help
        sys.addOption(OptionDetails{"-n", "--no-newline", "Do not append newline"});

        // Echo command used for testing
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
        return sys;
    }
}

TEST(CommandShellTests, ExecuteRegisteredCommand)
{
    CommandShell shell;
    auto sys = makeSysComponent();
    shell.registerComponent(sys);

    Command cmd;
    cmd.component = "sys";
    cmd.command = "echo";
    cmd.arguments = {"hello", "world"};

    auto out = shell.executeCommand(cmd);
    EXPECT_EQ(out, std::string("hello world\n"));
}

TEST(CommandShellTests, HelpListComponents)
{
    CommandShell shell;
    auto sys = makeSysComponent();
    shell.registerComponent(sys);

    Command help;
    help.component = "help";
    help.command = "list"; // or "components"

    auto out = shell.executeCommand(help);
    // Expect header and both components: sys and built-in help
    EXPECT_NE(out.find("Available components:\n"), std::string::npos);
    EXPECT_NE(out.find("sys - System commands\n"), std::string::npos);
    EXPECT_NE(out.find("help - Show help for components and commands\n"), std::string::npos);
}

TEST(CommandShellTests, HelpComponentsAlias)
{
    CommandShell shell;
    shell.registerComponent(makeSysComponent());

    Command help;
    help.component = "help";
    help.command = "components";

    auto out = shell.executeCommand(help);
    EXPECT_NE(out.find("Available components:"), std::string::npos);
}

TEST(CommandShellTests, HelpForComponent)
{
    CommandShell shell;
    shell.registerComponent(makeSysComponent());

    Command help;
    help.component = "help";
    help.command = "sys"; // help <component>

    auto out = shell.executeCommand(help);
    EXPECT_NE(out.find("Component: sys\n"), std::string::npos);
    EXPECT_NE(out.find("System commands\n"), std::string::npos);
    EXPECT_NE(out.find("Options:\n  -n, --no-newline  - Do not append newline\n"), std::string::npos);
    EXPECT_NE(out.find("Commands:\n  echo - Echo arguments like /bin/echo\n"), std::string::npos);
}

TEST(CommandShellTests, HelpForSpecificCommand)
{
    CommandShell shell;
    shell.registerComponent(makeSysComponent());

    Command help;
    help.component = "help";
    help.command = "sys";
    help.arguments = {"echo"};

    auto out = shell.executeCommand(help);
    EXPECT_EQ(out, std::string("sys echo: Echo arguments like /bin/echo\n"));
}

TEST(CommandShellTests, PerComponentHelpAndCommandHelp)
{
    CommandShell shell;
    shell.registerComponent(makeSysComponent());

    // sys help
    Command helpComp;
    helpComp.component = "sys";
    helpComp.command = "help";
    auto outComp = shell.executeCommand(helpComp);
    EXPECT_NE(outComp.find("Component: sys\n"), std::string::npos);

    // sys help echo
    Command helpCmd;
    helpCmd.component = "sys";
    helpCmd.command = "help";
    helpCmd.arguments = {"echo"};
    auto outCmd = shell.executeCommand(helpCmd);
    EXPECT_EQ(outCmd, std::string("sys echo: Echo arguments like /bin/echo\n"));
}

