#ifndef COMMAND_SHELL_HPP
#define COMMAND_SHELL_HPP

#include <string>
#include <map>
#include <vector>
#include "CommandTypes.hpp"

namespace commandshell
{
    class CommandShell
    {
    public:
        CommandShell();
        ~CommandShell() = default;

        // Register a component command set
        void registerComponent(const commandshell::ComponentCommands& component);

        // Executes a parsed command and returns the output
        std::string executeCommand(const commandshell::Command &command);

    private:
        // Registered components by name
        std::map<std::string, commandshell::ComponentCommands> mComponents;
    };
} // namespace commandshell
#endif // COMMAND_SHELL_HPP
