#pragma once

#include <chrono>
#include <vector>

#include "../string.h"

namespace atom {

class PerformanceCounters {
public:
  typedef std::chrono::high_resolution_clock::time_point TimePoint;

  struct CounterInfo {
    String    name;
    int       parent;
    TimePoint start_time;
    TimePoint stop_time;

    CounterInfo(const String &counter_name, int counter_parent, TimePoint start, TimePoint stop)
      : name(counter_name)
      , parent(counter_parent)
      , start_time(start)
      , stop_time(stop)
    {}

    TimePoint::duration duration()
    { return stop_time - start_time; }

    std::chrono::microseconds microseconds() const
    { return std::chrono::duration_cast<std::chrono::microseconds>(stop_time - start_time); }
  };

  typedef std::vector<CounterInfo> CounterArray;

  void start(
    const String &counter_name);

  void stop(
    const String &counter_name);

  void clear();

  CounterInfo get_counter(
    const String &counter_name) const;

//  CounterArray::const_iterator begin() const
//  { return my_counters.begin(); }

//  CounterArray::const_iterator end() const
//  { return my_counters.end(); }

  String to_string() const;

private:  // private methods
  void print_and_process_counter(
    int counter_index,
    int indent,
    std::stringstream &output) const;

private:
  CounterArray     my_counters;
  std::vector<int> my_running_counters; ///< zasobnik aktualne beziacich casovacov
};

}
