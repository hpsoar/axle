#include "../core/timer.h"

#include <stdio.h>

#if !defined(SYS_IS_WINDOWS)
#include <sys/time.h>
#else
#include <windows.h>
#endif

namespace ax {

#if !defined(SYS_IS_WINDOWS)
double GetTime() {
  struct timeval time_of_day;
  gettimeofday(&time_of_day, NULL);
  return time_of_day.tv_sec + 
    time_of_day.tv_usec / 1000000.0;
}
#else
double InitTimer() {
  LARGE_INTEGER performance_frequency;
  QueryPerformanceFrequency(&performance_frequency);
  return 1.0/static_cast<double>(performance_frequency.QuadPart);
}
static double second_per_counter = InitTimer();

double GetTime() {
  LARGE_INTEGER performance_counter;
  QueryPerformanceCounter(&performance_counter);
  return static_cast<double>(performance_counter.QuadPart) * 
    second_per_counter;
}
#endif

void SeqTimer::Begin(const std::string& info) {	
  s_info_stack.push(info);
  for (size_t i = 0; i < s_time_stack.size(); ++i)
    printf("  ");
  printf("%s ...\n", info.c_str());
  s_time_stack.push(GetTime());
}

double SeqTimer::End() {
  const double end_time = GetTime();
  const double start_time = s_time_stack.top();
  double elapsed = end_time - start_time;
  const std::string info = s_info_stack.top();
  s_info_stack.pop();
  s_time_stack.pop();
  for (size_t i = 0; i < s_time_stack.size(); ++i)
    printf("  ");
  printf("cost: %f (s)\n", elapsed);
  return elapsed;
}

std::stack<std::string> SeqTimer::s_info_stack;

std::stack<double> SeqTimer::s_time_stack;

void AccumTimer::Accum(const std::string& info) {
  double t = GetTime();
  if (!end_called_) {		
    accums_.push_back(0);
    infos_.push_back(info);
  }
  assert(accum_id_ < accums_.size());
  double elapsed = t - last_time_;
  accums_[accum_id_] += elapsed;
  ++accum_id_;
  last_time_ = GetTime();
}

void AccumTimer::PrintResult() {
  for (size_t i = 0; i < accums_.size(); ++i) {
    printf("%s, cost: %f (s)\n", infos_[i].c_str(), accums_[i]);
  }
}

void AccumTimer::TimerBegin() {	
  assert(!begin_ || end_called_);
  begin_ = true;
  accum_id_ = 0;
  last_time_ = GetTime();
}

void AccumTimer::TimerEnd() {
  assert(begin_);
  end_called_ = true;
}
} // ax

