#include "CommandShellIO.hpp"
#include "CommandShell.hpp"

#include <iostream>
#include <string>

using namespace commandshell;

CommandShellIO::CommandShellIO(CommandShell &shell, bool echo, std::string promptText)
    : mCommandShell(shell), mEchoInput(echo), mCurrentInput(""), mOnOutputCallback(nullptr), mPromptText(std::move(promptText)) {}

void CommandShellIO::input(std::string &promptPart)
{
    if(mEchoInput)
    {
        mOnOutputCallback(promptPart);
    }

    mCurrentInput += promptPart;
    bool isEndofLine = false;
    for(auto c : promptPart) {
        if(c == '\n' || c == '\r') {
            isEndofLine = true;
            break;
        }
    }

    if(!isEndofLine)
    {
        return;
    }

    // Trim at first newline for parsing
    size_t eol = mCurrentInput.find_first_of("\r\n");
    auto commandStr = (eol == std::string::npos) ? mCurrentInput : mCurrentInput.substr(0, eol);
    auto commandParts = splitInput(commandStr);

    Command command;
    if(commandParts.size() < 2) {
        // Allow bare `help` to map to `help list`
        if (commandParts.size() == 1 && commandParts[0] == "help") {
            command.component = "help";
            command.command = "list";
        } else {
            mCurrentInput.clear();
            return;
        }
    } else {
        command = parseCommand(commandParts);
    }
    // Execute via CommandShell if a command is registered
    std::string output = mCommandShell.executeCommand(command);
    mCurrentInput.clear();

    if(mOnOutputCallback) {
        mOnOutputCallback(output);
    } 
}

void CommandShellIO::input(char *promptPart, size_t size)
{
    std::string prompt(promptPart, size);
    input(prompt);
}

void CommandShellIO::setOutputCallback(std::function<void(const std::string &)> callback)
{
    mOnOutputCallback = std::move(callback);
    printPrompt();
}

void CommandShellIO::printPrompt()
{
    if (mOnOutputCallback) {
        mOnOutputCallback(mPromptText);
    }
}

/******************** Private methods *******************/

std::vector<std::string_view> CommandShellIO::splitInput(const std::string& input)
{
    std::vector<std::string_view> result;
    size_t start = 0;
    size_t end = 0;

    while((end = input.find(' ', start)) != std::string::npos) {
        if(end != start) {
            result.emplace_back(input.data() + start, end - start);
        }
        start = end + 1;
    }

    if(start < input.size()) {
        result.emplace_back(input.data() + start, input.size() - start);
    }

    return result;
}

commandshell::Command CommandShellIO::parseCommand(const std::vector<std::string_view>& commandParts)
{
    commandshell::Command command;
    command.component = std::string(commandParts[0]);
    command.command = std::string(commandParts[1]);
    for(size_t i = 2; i < commandParts.size(); ++i) {
        const auto& part = commandParts[i];
        if(part.size() > 0 && part[0] == '-') {
            command.options.emplace_back(part);
        } else {
            command.arguments.emplace_back(part);
        }
    }
    return command;
}
