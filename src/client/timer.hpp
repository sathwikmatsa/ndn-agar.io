#pragma once
#include <stdint.h>

class Timer {
public:
  // Initializes variables
  Timer();

  // The various clock actions
  void start();
  void stop();
  void pause();
  void unpause();

  // Gets the timer's time
  uint32_t get_ticks();

  // Checks the status of the timer
  bool is_started();
  bool is_paused();

private:
  // The clock time when the timer started
  uint32_t start_ticks;

  // The ticks stored when the timer was paused
  uint32_t paused_ticks;

  // The timer status
  bool paused;
  bool started;
};
