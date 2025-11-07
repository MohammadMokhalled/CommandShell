#include "CommandShell.hpp"
#include "CommandTypes.hpp"

#include <utility>
#include <sstream>

using namespace commandshell;

namespace {
    std::string renderComponents(const std::map<std::string, commandshell::ComponentCommands>& comps)
    {
        std::ostringstream os;
        os << "Available components:\n";
        for (const auto& kv : comps)
        {
            const auto& comp = kv.second;
            os << "  " << comp.component << " - " << comp.description << "\n";
        }
        return os.str();
    }

    std::string renderCommandHelp(const commandshell::ComponentCommands& comp, const std::string& cmdName)
    {
        std::ostringstream os;
        for (const auto& cd : comp.commands)
        {
            if (cd.command == cmdName)
            {
                os << comp.component << " " << cd.command << ": " << cd.description << "\n";
                return os.str();
            }
        }
        return std::string{};
    }

    std::string renderComponentHelp(const commandshell::ComponentCommands& comp)
    {
        std::ostringstream os;
        os << "Component: " << comp.component << "\n";
        os << comp.description << "\n\n";

        if (!comp.options.empty())
        {
            os << "Options:\n";
            for (const auto& opt : comp.options)
            {
                bool hasShort = !opt.shortOpt.empty();
                bool hasLong = !opt.longOpt.empty();
                os << "  ";
                if (hasShort) os << opt.shortOpt;
                if (hasShort && hasLong) os << ", ";
                if (hasLong) os << opt.longOpt;
                if (hasShort || hasLong) os << "  ";
                os << "- " << opt.description << "\n";
            }
            os << "\n";
        }

        os << "Commands:\n";
        for (const auto& cmd : comp.commands)
        {
            os << "  " << cmd.command << " - " << cmd.description << "\n";
        }
        return os.str();
    }
}

CommandShell::CommandShell()
{
    // Register built-in help component so it appears in listings
    ComponentCommands help{"help", "Show help for components and commands"};
    registerComponent(help);
}

void CommandShell::registerComponent(const ComponentCommands& component)
{
    auto it = mComponents.find(component.component);
    if (it != mComponents.end())
    {
        mComponents.erase(it);
    }
    mComponents.emplace(component.component, component);
}

// Executes a parsed command and returns the output via registered components
std::string CommandShell::executeCommand(const Command &command)
{
    // Built-in help component and per-component help command
    if (command.component == "help")
    {
        // help list | help components -> list all components
        if (command.command == "list" || command.command == "components")
        {
            return renderComponents(mComponents);
        }

        // help <component> [command]
        auto compIt2 = mComponents.find(command.command);
        if (compIt2 == mComponents.end())
        {
            // If asking help for unknown component, return empty
            return std::string{};
        }
        const auto& comp2 = compIt2->second;
        if (!command.arguments.empty())
        {
            auto out = renderCommandHelp(comp2, command.arguments[0]);
            if (!out.empty()) return out;
        }
        return renderComponentHelp(comp2);
    }

    auto compIt = mComponents.find(command.component);
    if (compIt == mComponents.end())
    {
        return std::string{};
    }

    const auto& comp = compIt->second;
    // Per-component help: `<component> help [command]`
    if (command.command == "help")
    {
        if (!command.arguments.empty())
        {
            auto out = renderCommandHelp(comp, command.arguments[0]);
            if (!out.empty()) return out;
        }
        return renderComponentHelp(comp);
    }
    auto maybe = comp.getCommandFunction(command.command);
    if (!maybe.has_value())
    {
        return std::string{};
    }

    const auto& details = maybe.value();
    return details.execute(command.arguments, command.options);
}
