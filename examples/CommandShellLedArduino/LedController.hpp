// Simple non-blocking LED controller with a small state machine
#pragma once

#include <Arduino.h>
#include <string>
#include "CommandTypes.hpp"

class LedController {
public:
  enum class State {
    SteadyOff,
    SteadyOn,
    BlinkOn,
    BlinkOff
  };

  explicit LedController(uint8_t pin);

  void begin();
  void setOn();
  void setOff();
  void toggle();

  // Start continuous blink with provided on/off durations (ms)
  void startBlink(unsigned long onMs, unsigned long offMs);

  // Call frequently from loop() to run the state machine
  void update();

  bool isOn() const;
  State state() const;
  std::string statusText() const;

  // Build the CommandShell component for this LED
  commandshell::ComponentCommands buildCommands();

private:
  static unsigned long elapsedSince(unsigned long start, unsigned long now);

  uint8_t pin_;
  State state_ = State::SteadyOff;
  unsigned long on_ms_ = 500;
  unsigned long off_ms_ = 500;
  unsigned long phase_started_at_ = 0;
};

