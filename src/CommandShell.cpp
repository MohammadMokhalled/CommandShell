#include "CommandShell.hpp"
#include "CommandTypes.hpp"

#include <utility>

using namespace commandshell;

// Static instance pool definition
std::map<std::string, std::shared_ptr<CommandShell>> CommandShell::instancePool{};

CommandShell& CommandShell::getInstance(const std::string& name)
{
    auto it = instancePool.find(name);
    if (it != instancePool.end())
    {
        return *(it->second);
    }

    auto created = std::shared_ptr<CommandShell>(new CommandShell());
    created->instanceName = name;
    instancePool.emplace(name, created);
    return *created;
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
    auto compIt = mComponents.find(command.component);
    if (compIt == mComponents.end())
    {
        return std::string{};
    }

    const auto& comp = compIt->second;
    auto maybe = comp.getCommandFunction(command.command);
    if (!maybe.has_value())
    {
        return std::string{};
    }

    const auto& details = maybe.value();
    return details.execute(command.arguments, command.options);
}
