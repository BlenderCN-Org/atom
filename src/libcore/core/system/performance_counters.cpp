#include "performance_counters.h"

#include <algorithm>
#include <iomanip>
#include <sstream>
#include "../platform.h"
#include "../log.h"

namespace atom {

void PerformanceCounters::start(const String &counter_name)
{
  assert(std::find_if(my_counters.begin(), my_counters.end(),
    [&counter_name](const CounterInfo &counter_info) -> bool
    { return counter_name == counter_info.name; }) == my_counters.end() && "This counter already exist, you can't start existing counter");
  // ak aktualne bezi nejaky casovac tak ho nastav ako nadradeny

  log::debug(DEBUG_COUNTERS, "Starting counter %s", counter_name.c_str());

  int parent = -1;
  if (!my_running_counters.empty())
    parent = my_running_counters.back();
  // vloz novy zaznam o casovaci a vloz ho do zasobnika aktualne beziacich casovacov
  my_counters.push_back(CounterInfo(counter_name, parent,
    std::chrono::high_resolution_clock::now(), TimePoint()));
  my_running_counters.push_back(my_counters.size() - 1);
}

void PerformanceCounters::stop(const String &counter_name)
{
  auto counter = std::find_if(my_counters.begin(), my_counters.end(),
    [&counter_name](const CounterInfo &counter_info) -> bool
    { return counter_name == counter_info.name; });
  // casovac musi bezat (musi o nom existovat zaznam)
  assert(counter != my_counters.end() && "Trying to stop a nonexistent counter");
  log::debug(DEBUG_COUNTERS, "Stopping counter %s", counter_name.c_str());
  // nastav stop time a odstran ho zo zasobnika beziacich casovacov
  counter->stop_time = std::chrono::high_resolution_clock::now();
  my_running_counters.pop_back();
}

void PerformanceCounters::clear()
{
//  assert(my_running_counters.empty());
  log::debug(DEBUG_COUNTERS, "Clearing counters");
  my_counters.clear();
}

PerformanceCounters::CounterInfo PerformanceCounters::get_counter(
  const String &counter_name) const
{
  auto i = std::find_if(my_counters.begin(), my_counters.end(),
    [&counter_name](const CounterInfo &counter_info)
    { return counter_name == counter_info.name; });
  // over najdenie zaznamu
  assert(i != my_counters.end());
  return *i;
}

String PerformanceCounters::to_string() const
{
//  String results;
//  for (const CounterInfo &counter_info : my_counters) {
//    uint64_t count = duration_cast<milliseconds>(
//      counter_info.stop_time - counter_info.start_time).count();
//    results += counter_info.name + ": " + std::to_string(count) + "ms\n";
//  }
//  return results;

  std::stringstream output;
  int count = my_counters.size();

  for (int i = 0; i < count; ++i) {
    if (my_counters[i].parent < 0)
      print_and_process_counter(i, 0, output);
  }

  return output.str();
}

void PerformanceCounters::print_and_process_counter(
  int counter_index,
  int indent,
  std::stringstream &output) const
{
  for (int i = 0; i < indent; ++i)
    output << "  ";

  float duration = my_counters[counter_index].microseconds().count() / 1000.0;
  output << std::setprecision(2) << my_counters[counter_index].name << ": " << duration << "ms\n";

  int count = my_counters.size();
  for (int i = 0; i < count; ++i) {
    if (my_counters[i].parent == counter_index)
      print_and_process_counter(i, indent + 1, output);
  }
}

}
