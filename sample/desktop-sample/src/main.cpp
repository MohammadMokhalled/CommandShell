// Simple desktop sample demonstrating CommandShell + CommandShellIO usage
#include "CommandShell.hpp"
#include "CommandShellIO.hpp"
#include "CommandTypes.hpp"

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using commandshell::CommandShell;
using commandshell::CommandShellIO;
using commandshell::ComponentCommands;
using commandshell::CommandDetails;
using commandshell::OptionDetails;

namespace {
    ComponentCommands makeSampleComponent()
    {
        ComponentCommands comp{"sample", "Sample commands for the desktop demo"};

        // echo: prints arguments (supports -n/--no-newline to suppress trailing newline)
        comp.addOption(OptionDetails{"-n", "--no-newline", "Do not append newline"});
        comp.addCommand(CommandDetails{
            "echo",
            "Echo the provided arguments",
            [](const std::vector<std::string>& args, const std::vector<std::string>& opts) -> std::string {
                bool noNewline = false;
                for (const auto& o : opts) {
                    if (o == "-n" || o == "--no-newline") { noNewline = true; }
                }
                std::ostringstream os;
                for (size_t i = 0; i < args.size(); ++i) {
                    if (i) os << ' ';
                    os << args[i];
                }
                if (!noNewline) os << '\n';
                return os.str();
            }
        });

        // sum: sums integer arguments
        comp.addCommand(CommandDetails{
            "sum",
            "Sum integer arguments and print the total",
            [](const std::vector<std::string>& args, const std::vector<std::string>&) -> std::string {
                long long total = 0;
                for (const auto& a : args) {
                    try {
                        total += std::stoll(a);
                    } catch (...) {
                        return std::string("error: non-integer argument: ") + a + "\n";
                    }
                }
                return std::to_string(total) + "\n";
            }
        });

        // about: prints a static message
        comp.addCommand(CommandDetails{
            "about",
            "Show info about this sample app",
            [](const std::vector<std::string>&, const std::vector<std::string>&) -> std::string {
                return "CommandShell desktop sample. Try: help list, sample help, sample echo hi, sample sum 1 2 3\n";
            }
        });

        return comp;
    }
}

int main()
{
    CommandShell shell;
    shell.registerComponent(makeSampleComponent());

    // Wire IO with echo disabled (we control what to print)
    CommandShellIO io(shell, /*echoInput=*/false);
    io.setOutputCallback([](const std::string& out) {
        std::cout << out;
    });

    std::cout << "CommandShell Desktop Sample\n";
    std::cout << "Type 'exit' or 'quit' to leave.\n";
    std::cout << "Try commands: help list | help sample | sample about\n\n";

    std::string line;
    while (true) {
        io.printPrompt();
        if (!std::getline(std::cin, line)) {
            break; // EOF
        }
        if (line == "exit" || line == "quit") {
            break;
        }
        // Feed line + newline to IO so it parses and executes
        line.push_back('\n');
        io.input(line);
        line.clear();
    }

    std::cout << "Goodbye!\n";
    return 0;
}
