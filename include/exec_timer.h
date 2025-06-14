#ifndef EXEC_TIMER_H_INCLUDED
#define EXEC_TIMER_H_INCLUDED

#include <chrono>
#include <string>

using std::chrono::duration_cast;
using std::chrono::high_resolution_clock;
using std::chrono::milliseconds;
using std::chrono::nanoseconds;
using std::chrono::time_point;

class ExecTimer {
 private:
  using _time_point = time_point<high_resolution_clock>;
  std::string caller;
  _time_point startTime;
  _time_point endTime;

 public:
  explicit ExecTimer(const std::string &caller);
  ~ExecTimer();
};

#endif