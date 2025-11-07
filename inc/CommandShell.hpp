#ifndef COMMAND_SHELL_HPP
#define COMMAND_SHELL_HPP

#include <string>
#include <map>
#include <memory>
#include <vector>
#include "CommandTypes.hpp"

namespace commandshell
{
    class CommandShell
    {
    public:
        CommandShell() = default;
        ~CommandShell() = default;

        // Retrieve or create a named instance (simple pooling)
        static CommandShell& getInstance(const std::string& name);

        // Register a component command set
        void registerComponent(const commandshell::ComponentCommands& component);

        // Executes a parsed command and returns the output
        std::string executeCommand(const commandshell::Command &command);



    private:
        // Instance name
        std::string instanceName;

        // Current command input
        std::string currentInput;

        // Static instance map for object pooling
        static std::map<std::string, std::shared_ptr<CommandShell>> instancePool;

        // Registered components by name
        std::map<std::string, commandshell::ComponentCommands> mComponents;
    };
} // namespace commandshell
#endif // COMMAND_SHELL_HPP
