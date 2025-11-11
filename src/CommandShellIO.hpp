#ifndef COMMANDSHELL_IO_HPP
#define COMMANDSHELL_IO_HPP
#include <string>
#include <functional>
#include <vector>
#include <string_view>
#include "CommandTypes.hpp"
// Forward declaration to avoid heavy include and keep coupling low
namespace commandshell { class CommandShell; }

namespace commandshell {
class CommandShellIO {
public:
    // Constructor
    CommandShellIO(CommandShell& shell, bool echoInput = true, std::string promptText = "cmd> ");

    // Get command input with string prompt
    void input(std::string& promptPart);

    // Get command input with char* prompt
    void input(char* promptPart, size_t size);

    // Set callback for when input is received
    void setOutputCallback(std::function<void(const std::string&)> callback);

    // Print the prompt via output callback (or stdout if none)
    void printPrompt();

protected:
    // Split input into parts
    std::vector<std::string_view> splitInput(const std::string& input);

    // Parse command from input parts
    Command parseCommand(const std::vector<std::string_view>& parts);

private:
    CommandShell& mCommandShell;
    bool mEchoInput;
    std::string mCurrentInput;
    std::function<void(const std::string&)> mOnOutputCallback;
    std::string mPromptText;
};
} // namespace commandshell
#endif // COMMANDSHELL_IO_HPP
