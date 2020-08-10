#pragma once

#include <memory.h>
#include <atomic>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

static constexpr const char* STR_CPU = "cpu";

namespace Sentinel {
namespace System {

constexpr size_t NUM_CPU_STATES = 10;
enum CpuStates {
  S_USER = 0,
  S_NICE,
  S_SYSTEM,
  S_IDLE,
  S_IOWAIT,
  S_IRQ,
  S_SOFTIRQ,
  S_STEAL,
  S_GUEST,
  S_GUEST_NICE
};

class CpuLoadInfo {
 public:
  double usage[3];
  std::string curProNum;
  size_t curProId;
};

class CpuUsageInfo {
 public:
  std::string cpu;
  int64_t times[10];
};

class SystemStatusListener {
 public:
  SystemStatusListener();
  static SystemStatusListener& GetInstance() {
    static SystemStatusListener* instance = new SystemStatusListener();
    return *instance;
  }

  virtual ~SystemStatusListener() {
    stopListner();
    file_stat_.close();
    file_load_.close();
  }
  void RunCpuListener();
  void Initialize();
  double GetCurLoad() { return cur_load_.load(); }
  double GetCurCpuUsage() { return cur_cpu_usage_.load(); }

 private:
  int64_t GetIdleTime(std::shared_ptr<CpuUsageInfo> p);
  int64_t GetActiveTime(std::shared_ptr<CpuUsageInfo> p);
  void ReadCpuUsageFromProc(std::shared_ptr<CpuUsageInfo> p);
  void UpdateCpuUsage();
  void UpdateSystemLoad();

  std::ifstream file_stat_, file_load_;
  std::shared_ptr<CpuUsageInfo> usage_info_p1_, usage_info_p2_;
  std::shared_ptr<CpuLoadInfo> load_info_p_;

  std::atomic<double> cur_load_{-1};
  std::atomic<double> cur_cpu_usage_{-1};
  std::atomic<bool> stopped_cmd_{false};
  std::atomic<bool> stopped_{false};
  std::atomic<bool> inited_{false};

  // wait until loop in RunCpuListener stop
  void stopListner() {
    stopped_cmd_.store(true);
    while (!stopped_.load())
      ;
  }
};

using SystemStatusListenerSharedPtr = std::shared_ptr<SystemStatusListener>;

}  // namespace System
}  // namespace Sentinel
