#include "exec_timer.h"
#include <chrono>
#include <format>
#include <iostream>
#include <string>

ExecTimer::ExecTimer(const std::string &caller) {
  this->caller = caller;
  startTime = high_resolution_clock::now();
}

ExecTimer::~ExecTimer() {
  this->endTime = high_resolution_clock::now();
  const auto duration = this->endTime - this->startTime;
  const std::string message = std::format("\'{0}\': {1} = {2}", this->caller,
                                          duration_cast<nanoseconds>(duration),
                                          duration_cast<seconds>(duration));
  std::cout << message << std::endl;
}