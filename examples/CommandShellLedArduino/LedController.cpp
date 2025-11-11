// Implementation file for LedController
#include "LedController.hpp"

using commandshell::ComponentCommands;
using commandshell::CommandDetails;

LedController::LedController(uint8_t pin) : pin_(pin) {}

static inline void setLedPinOn(uint16_t pin) {
  digitalWrite(pin, LOW);
}

static inline void setLedPinOff(uint16_t pin) {
  digitalWrite(pin, HIGH);
}

void LedController::begin() {
  pinMode(pin_, OUTPUT);
  setLedPinOff(pin_);
  state_ = State::SteadyOff;
}

void LedController::setOn() {
  state_ = State::SteadyOn;
  setLedPinOn(pin_);
}

void LedController::setOff() {
  state_ = State::SteadyOff;
  setLedPinOff(pin_);
}

void LedController::toggle() {
  if (state_ == State::SteadyOn) {
    setOff();
  } else if (state_ == State::SteadyOff) {
    setOn();
  } else {
    // If blinking, toggle between steady states and stop blinking
    if (isOn()) {
      setOff();
    } else {
      setOn();
    }
  }
}

void LedController::startBlink(unsigned long onMs, unsigned long offMs) {
  on_ms_ = (onMs == 0 ? 1UL : onMs);
  off_ms_ = (offMs == 0 ? 1UL : offMs);
  state_ = State::BlinkOn; // start with ON phase
  setLedPinOn(pin_);
  phase_started_at_ = millis();
}

void LedController::update() {
  const unsigned long now = millis();
  switch (state_) {
    case State::SteadyOn:
    case State::SteadyOff:
      return; // nothing to do in steady states
    case State::BlinkOn:
      if (elapsedSince(phase_started_at_, now) >= on_ms_) {
        state_ = State::BlinkOff;
        setLedPinOff(pin_);
        phase_started_at_ = now;
      }
      break;
    case State::BlinkOff:
      if (elapsedSince(phase_started_at_, now) >= off_ms_) {
        state_ = State::BlinkOn;
        setLedPinOn(pin_);
        phase_started_at_ = now;
      }
      break;
  }
}

bool LedController::isOn() const { return digitalRead(pin_) == HIGH; }

LedController::State LedController::state() const { return state_; }

std::string LedController::statusText() const {
  std::string base = std::string("LED: ") + (isOn() ? "ON" : "OFF");
  switch (state_) {
    case State::SteadyOn: return base + " (steady)\n";
    case State::SteadyOff: return base + " (steady)\n";
    case State::BlinkOn: return base + " (blinking)\n";
    case State::BlinkOff: return base + " (blinking)\n";
  }
  return base + "\n";
}

unsigned long LedController::elapsedSince(unsigned long start, unsigned long now) {
  return now - start; // unsigned arithmetic handles wrap-around
}

ComponentCommands LedController::buildCommands() {
  ComponentCommands led{"led", "Control the built-in LED"};

  led.addCommand(CommandDetails{
      "on",
      "Turn LED on",
      [this](const std::vector<std::string>&, const std::vector<std::string>&) -> std::string {
        this->setOn();
        return this->statusText();
      }
  });

  led.addCommand(CommandDetails{
      "off",
      "Turn LED off",
      [this](const std::vector<std::string>&, const std::vector<std::string>&) -> std::string {
        this->setOff();
        return this->statusText();
      }
  });

  led.addCommand(CommandDetails{
      "toggle",
      "Toggle LED state",
      [this](const std::vector<std::string>&, const std::vector<std::string>&) -> std::string {
        this->toggle();
        return this->statusText();
      }
  });

  led.addCommand(CommandDetails{
      "blink",
      "Blink LED with durations: led blink [on_ms] [off_ms]",
      [this](const std::vector<std::string>& args, const std::vector<std::string>& options) -> std::string {
        unsigned long onMs = 500;
        unsigned long offMs = 500;
        if (args.size() >= 1) {
          onMs = strtoul(args[0].c_str(), nullptr, 10);
        }
        if (args.size() >= 2) {
          offMs = strtoul(args[1].c_str(), nullptr, 10);
        }
        if (onMs < 1) onMs = 1;
        if (offMs < 1) offMs = 1;
        this->startBlink(onMs, offMs);
        return std::string("OK: blinking ") + std::to_string(onMs) + "ms on, " + std::to_string(offMs) + "ms off\n";
      }
  });

  led.addCommand(CommandDetails{
      "status",
      "Show current LED state",
      [this](const std::vector<std::string>&, const std::vector<std::string>&) -> std::string {
        return this->statusText();
      }
  });

  return led;
}

