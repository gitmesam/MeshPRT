#include "Timer.h"

Timer::Timer() {
  start = 0;
  end = 0;
  time = 0;
}

Timer::~Timer() {
}

void Timer::Start() {
  start = clock();
}

void Timer::Stop() {
  end = clock();
  time = (double) (end-start)/CLOCKS_PER_SEC;
}

void Timer::Stop(WCHAR* output){
  Stop();
  WCHAR msg[120];
  Concat(msg, output, L" took (sec): ");
  PD(msg, time);
}