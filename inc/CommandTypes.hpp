#ifndef COMMAND_TYPES_HPP
#define COMMAND_TYPES_HPP
#include <string>
#include <vector>
#include <functional>
#include <optional>

namespace commandshell {
    struct Command
    {
        std::string component;
        std::string command;
        std::vector<std::string> arguments;
        std::vector<std::string> options;
    };

    /* Details of a single command
    *  How to define a command:
    *  CommandDetails myCommand = {
    *     "myCommand",
    *     "Description of myCommand",
    *     {"--option1", "--option2"},
    *       [](const std::vector<std::string>& args, const std::vector<std::string>& opts) -> std::string {
    *           // Implementation of myCommand
    *           return "Command executed";
    *       }
    *  };
    */
    struct CommandDetails
    {
        const std::string command;
        const std::string description;

        // Function to execute the command (arguments, options) -> output
        std::function<std::string(const std::vector<std::string>&, const std::vector<std::string>&)> execute;
    };

    struct OptionDetails
    {
        std::string shortOpt;
        std::string longOpt;
        std::string description;
    };

    /* Command set for a specific component 
    *  How to define commands for a component:
    *  ComponentCommands myComponentCommands = {
    *      "MyComponent",
    *      "Description of MyComponent commands",
    *      {
    *         {
    *           "myCommand",
    *           "Description of myCommand",
    *           [](const std::vector<std::string>& args, const std::vector<std::string>& opts) -> std::string {
    *            // Implementation of myCommand
    *           return "Command executed";
    *         }
    *      }
    *    }
    */
    struct ComponentCommands
    {
        std::string component;
        std::string description;
        std::vector<CommandDetails> commands;
        std::vector<OptionDetails> options;

        ComponentCommands(const std::string& comp, const std::string& desc)
            : component(comp), description(desc) {};

        // Add a command to the component
        void addCommand(const CommandDetails& cmd) {
            commands.push_back(cmd);
        }

        // Add an option to the component
        void addOption(const OptionDetails& opt) {
            options.push_back(opt);
        }

        std::optional<CommandDetails> getCommandFunction(const std::string& cmdName) const {
            for (const auto& cmd : commands) {
                if (cmd.command == cmdName) {
                    return cmd;
                }
            }
            return std::nullopt;
        }
    };
}

#endif // COMMAND_TYPES_HPP
