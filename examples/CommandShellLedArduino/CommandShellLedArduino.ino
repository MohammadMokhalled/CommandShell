/*
  CommandShellLedArduino

  Arduino sketch demonstrating the repository's CommandShell library
  to control the built-in LED via UART. Commands follow the
  "<component> <command> [args]" format parsed by CommandShellIO.

  Commands:
    - led on
    - led off
    - led toggle
    - led status
    - help
    - help led [command]

  Default baud: 115200
*/

#include <Arduino.h>
#include "CommandShell.hpp"
#include "CommandShellIO.hpp"
#include "CommandTypes.hpp"

#if defined(LED_BUILTIN)
  const int LED_PIN = LED_BUILTIN;
#else
  const int LED_PIN = 13; // Fallback for boards without LED_BUILTIN
#endif

static const unsigned long BAUD_RATE = 115200;

using commandshell::CommandShell;
using commandshell::CommandShellIO;
using commandshell::ComponentCommands;
using commandshell::CommandDetails;

// Global shell and IO
CommandShell gShell;
CommandShellIO* gIO = nullptr;

// Simple serial-output callback used by CommandShellIO
static void serialOut(const std::string& s)
{
  if (!s.empty()) {
    Serial.print(s.c_str());
    // ensure output ends with newline for neatness when responses lack it
    if (s.back() != '\n' && s.back() != '\r') {
      Serial.print("\r\n");
    }
  }
}

static std::string ledStatusText()
{
  return std::string("LED: ") + (digitalRead(LED_PIN) == HIGH ? "ON\n" : "OFF\n");
}

static void registerLedComponent()
{
  ComponentCommands led{"led", "Control the built-in LED"};

  led.addCommand(CommandDetails{
      "on",
      "Turn LED on",
      [](const std::vector<std::string>&, const std::vector<std::string>&) -> std::string {
        digitalWrite(LED_PIN, HIGH);
        return std::string("OK: LED ON\n");
      }
  });

  led.addCommand(CommandDetails{
      "off",
      "Turn LED off",
      [](const std::vector<std::string>&, const std::vector<std::string>&) -> std::string {
        digitalWrite(LED_PIN, LOW);
        return std::string("OK: LED OFF\n");
      }
  });

  led.addCommand(CommandDetails{
      "toggle",
      "Toggle LED state",
      [](const std::vector<std::string>&, const std::vector<std::string>&) -> std::string {
        int current = digitalRead(LED_PIN);
        digitalWrite(LED_PIN, current == HIGH ? LOW : HIGH);
        return ledStatusText();
      }
  });

  led.addCommand(CommandDetails{
      "status",
      "Show current LED state",
      [](const std::vector<std::string>&, const std::vector<std::string>&) -> std::string {
        return ledStatusText();
      }
  });

  gShell.registerComponent(led);
}

void setup() {
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  Serial.begin(BAUD_RATE);
  while (!Serial) { /* wait for native USB boards */ }

  registerLedComponent();

  static CommandShellIO io(gShell, /*echoInput*/ false, /*prompt*/ "cmd> ");
  gIO = &io;
  gIO->setOutputCallback(serialOut);

  Serial.println();
  Serial.println(F("CommandShellLed ready."));
  Serial.print(F("Baud: "));
  Serial.println(BAUD_RATE);
  Serial.println(F("Type 'help' or 'help led' for commands."));
  gIO->printPrompt();
}

// Accumulate serial input and forward to CommandShellIO
static const size_t BUF_SZ = 128;
static char lineBuf[BUF_SZ];
static size_t lineLen = 0;

void loop() {
  while (Serial.available() > 0) {
    int ch = Serial.read();
    if (ch == '\r') {
      continue; // normalize to \n
    } else if (ch == '\n') {
      // complete line; pass to IO with newline
      if (lineLen < BUF_SZ - 1) {
        lineBuf[lineLen++] = '\n';
      }
      gIO->input(lineBuf, lineLen);
      lineLen = 0;
      // After processing, CommandShellIO prints prompt via callback
    } else {
      if (lineLen < BUF_SZ - 1) {
        lineBuf[lineLen++] = (char)ch;
      } // else drop extra chars until EOL
    }
  }
}
