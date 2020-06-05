#include <unistd.h>
#include <cstddef>
#include <set>
#include <string>
#include <vector>

#include "process.h"
#include "processor.h"
#include "system.h"
#include "linux_parser.h"

#include <iostream>

using std::set;
using std::size_t;
using std::string;
using std::vector;

Processor& System::Cpu() { return cpu_; }

System::System()
{
    std::vector<int> pids = LinuxParser::Pids();
    for(auto i:pids) {
        Process currentProcess(i);
        processes_.emplace_back(currentProcess);
    }    
}

vector<Process>& System::Processes() { 
  processes_.clear();
  std::vector<int>pids = LinuxParser::Pids();
  for (auto i:pids){
  	Process currentProcess(i);
    processes_.emplace_back(currentProcess);
  }
  return processes_; }

std::string System::Kernel() { return LinuxParser::Kernel(); }

float System::MemoryUtilization() { return LinuxParser::MemoryUtilization(); }

std::string System::OperatingSystem() { return LinuxParser::OperatingSystem(); }

int System::RunningProcesses() { return LinuxParser::RunningProcesses(); }

int System::TotalProcesses() { return LinuxParser::TotalProcesses(); }

long int System::UpTime() { return LinuxParser::UpTime();}