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
#include "LedController.hpp"

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
static LedController gLed(LED_PIN);

// Simple serial-output callback used by CommandShellIO
static void serialOut(const std::string& s)
{
  if (!s.empty()) {
    Serial.print(s.c_str());
  }
}

static std::string ledStatusText() { return gLed.statusText(); }

static void registerLedComponent()
{
  gShell.registerComponent(gLed.buildCommands());
}

void setup() {
  gLed.begin();

  // Explicitly set 8 data bits, no parity, 1 stop bit
  Serial.begin(BAUD_RATE, SERIAL_8N1);
  while (!Serial) { /* wait for native USB boards */ }

  registerLedComponent();

  static CommandShellIO io(gShell, /*echoInput*/ true, /*prompt*/ "cmd> ");
  gIO = &io;
  gIO->setOutputCallback(serialOut);

  Serial.println();
  Serial.println(F("CommandShellLed ready."));
  Serial.print(F("Baud: "));
  Serial.println(BAUD_RATE);
  Serial.println(F("Type 'help' or 'help led' for commands."));
  gIO->printPrompt();
}

void loop() {
  // Run LED state machine
  gLed.update();

  while (Serial.available() > 0) {
    char ch = Serial.read();
    gIO->input(&ch, 1);
  }
}
