#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>

#include "linux_parser.h"

#include <iostream>

using std::stof;
using std::string;
using std::to_string;
using std::vector;

string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

string LinuxParser::Kernel() {
  string os, kernel, version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> kernel >> version;
  }
  return version;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

float LinuxParser::MemoryUtilization() { 
  string line;
  string key;
  string unit;
  float freePercent;
  float memTotal;
  float memFree;
  float memAvailable;
  float memValue;
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  
  if (stream.is_open()){
    for (int i = 0; i < 4 ; i++){
  	std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> key >> memValue >> unit;
    if (key == "MemTotal:"){
      memTotal = memValue;
      continue;
    }
    else if (key == "MemFree:"){
      memFree = memValue;
      continue;
    }
    else if (key == "MemAvailable:"){
      memAvailable = memValue;
      continue;
    }
  } 
  freePercent = (float)((memTotal - memFree) / memTotal);
  return freePercent;
	}
}

long LinuxParser::UpTime() { 
  	std::string line;
  	long uptime;
  
  	std::ifstream stream(kProcDirectory + kUptimeFilename);
  	if (stream.is_open()){
    	std::getline(stream, line);
      	std::istringstream stringStream(line);
        stringStream >> uptime;
    }
  return uptime; 
}

long LinuxParser::Jiffies() { 
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
      std::string line;
      std::getline(filestream, line);
      std::istringstream linestream(line);
      std::string cpu;
      long user;
      long nice;
      long system;
      long idle;
      long iowait;
      long irq;
      long softirq;
      long steal;
      long guess;
      long guessnice; 
      linestream >> cpu >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal >> guess >> guessnice;
      long totalUserTime = user - guess;
      long totalNiceTime = nice - guessnice;
      long totalIdleTime = idle + iowait;
      long totalSystem = system + irq + softirq;
      long totalVirtualTime = guess + guessnice;
      return totalUserTime + totalNiceTime + totalIdleTime + totalSystem + totalVirtualTime;
  } 
}

long LinuxParser::ActiveJiffies(int pid) {
  std::stringstream filename;
  filename << kProcDirectory << "/" << pid << "/" << kStatFilename;
  std::ifstream filestream(filename.str());
  if (filestream.is_open()) {
      std::string line;
      std::getline(filestream, line);
      std::istringstream linestream(line);
      std::string ignore;
      long utime;
      long stime;
      long cutime;
      long cstime;
      long starttime;
      for(int i = 0; i < 13; i++) linestream >> ignore;
      linestream >> utime >> stime >> cutime >> cstime ;
      for(int i = 0; i < 4; i++) linestream >> ignore;
      linestream >> starttime;
      return utime + stime + cutime + cstime +starttime;
  }
  return 0; 
  }

long LinuxParser::ActiveJiffies() {
  std::ifstream filestream(kProcDirectory + kUptimeFilename);
  long upTime = 0;
  long idleTime;
  if (filestream.is_open()) {
      std::string line;
      std::getline(filestream, line);
      std::istringstream linestream(line);
      linestream >> upTime >> idleTime;
  } 
  return 0; }

long LinuxParser::IdleJiffies() { 
  string os, kernel;
  string line;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
      std::string line;
      std::getline(filestream, line);
      std::istringstream linestream(line);
      std::string cpu;
      long user;
      long nice;
      long system;
      long idle;
      long iowait;
      long irq;
      long softirq;
      long steal;
      long guess;
      long guessnice; 
      linestream >> cpu >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal >> guess >> guessnice;
      long totalUserTime = user - guess;
      long totalNiceTime = nice - guessnice;
      long totalIdleTime = idle + iowait;
      long totalSystem = system + irq + softirq;
      long totalVirtualTime = guess + guessnice;
      return totalIdleTime;
  }
  return 0;
 }

 LinuxParser::CpuProcessInfo LinuxParser::GetProcessCpuInfo(int pid) {
    std::stringstream filename;
    filename << kProcDirectory << "/" << pid << "/" << kStatFilename;
    std::ifstream filestream(filename.str());
    LinuxParser::CpuProcessInfo returnInfo;
    if (filestream.is_open()) {
        std::string line;
        std::getline(filestream, line);
        std::istringstream linestream(line);
        std::string ignore;
        long utime;
        long stime;
        long cutime;
        long cstime;
        long starttime;
        for(int i = 0; i < 13; i++) linestream >> ignore;
        linestream >> utime >> stime >> cutime >> cstime ;
        for(int i = 0; i < 4; i++) linestream >> ignore;
        linestream >> starttime;
        returnInfo.seconds = LinuxParser::UpTime() - (starttime/sysconf(_SC_CLK_TCK));
        returnInfo.totalTime = (utime + stime + cutime + cstime)/sysconf(_SC_CLK_TCK);
    }   
    return returnInfo;
  }

float LinuxParser::CpuUtilization(int pid)
  {
    LinuxParser::CpuProcessInfo current = LinuxParser::GetProcessCpuInfo(pid);
    long seconds = current.seconds; 
    long total = current.totalTime;
    return total * 1.0 / seconds;
  }

vector<LinuxParser::CpuKPI> LinuxParser::CpuUtilPercentage() {
  std::ifstream filestream(kProcDirectory + kStatFilename);
  vector<LinuxParser::CpuKPI> returnVector;
  if (filestream.is_open()) {
      std::string line;
      while (std::getline(filestream, line)) {
          std::istringstream linestream(line);
          std::string cpu;
          long user;
          long nice;
          long system;
          long idle;
          long iowait;
          long irq;
          long softirq;
          long steal;
          long guess;
          long guessnice; 
          linestream >> cpu >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal >> guess >> guessnice;
          if (cpu.substr(0,3) != "cpu")
              return returnVector;
        
          long totalIdleTime = idle + iowait;
          long totalNoIdleTime = user + nice + system + irq + softirq;
          CpuKPI current;
          current.idleTime = totalIdleTime;
          current.totalTime = totalIdleTime + totalNoIdleTime;
          returnVector.emplace_back(current);
      }     
      return returnVector;
  }
}

vector<string> LinuxParser::CpuUtilization() { 
  std::vector<LinuxParser::CpuKPI> previousVector = LinuxParser::CpuUtilPercentage(); 
  sleep(1);
  std::vector<LinuxParser::CpuKPI> currentVector = LinuxParser::CpuUtilPercentage(); 
  vector<std::string> returnCpu;
  for(int i = 0; i < currentVector.size(); i++) {
      std::ostringstream oCpuStream;
      long totalDelta = currentVector[i].totalTime - previousVector[i].totalTime ;
      long idleDelta = currentVector[i].idleTime - previousVector[i].idleTime ;
      oCpuStream << (totalDelta - idleDelta)*1.0/totalDelta*1.0;
      returnCpu.emplace_back(oCpuStream.str());
  }
  return returnCpu;
  }

int LinuxParser::TotalProcesses() { 
  int totalProcesses = 0;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()){
    std::string line;
    while (std::getline(stream, line)){
      std::istringstream linestream(line);
      std::string key;
      linestream >> key;
      if (key == "processes"){
      	linestream >> totalProcesses;
        break;
      }
    }
   }
  return totalProcesses;
}

int LinuxParser::RunningProcesses() { 
	std::ifstream stream(kProcDirectory + kStatFilename);
   	int runningProcesses = 0;
    if (stream.is_open()){
        std::string line;
        
        while (std::getline(stream, line)) {
          std::istringstream linestream(line);
          std::string key;
          linestream >> key;
          if (key == "procs_running")
          {          
              	linestream >> runningProcesses;
            	break;
          }
        }

    }
    return runningProcesses;   
}

string LinuxParser::Command(int pid) { 
 std::stringstream filename;
  filename << kProcDirectory << "/" << pid << "/" << kCmdlineFilename;
  std::ifstream filestream(filename.str());
  std::string cmdLine ;
  if (filestream.is_open()) {
      std::getline(filestream, cmdLine);
  }
  return cmdLine;
 }

string LinuxParser::Ram(int pid) { 
  std::stringstream filename;
  filename << kProcDirectory << "/" << pid << "/" << kStatusFilename;
  std::ifstream filestream(filename.str());
  long memory ;
  std::string unit;
  if (filestream.is_open()) {
      std::string line;
      while (std::getline(filestream, line)) {
        std::istringstream linestream(line);
        std::string key;
        linestream >> key;
        if (key == "VmSize:") {
          linestream >> memory >> unit;
          break;
        }

      }
  }
  std::ostringstream ostream;
  ostream << memory/1024 ;
  return ostream.str(); 
  }

string LinuxParser::Uid(int pid) { 
  std::stringstream filename;
  filename << kProcDirectory << "/" << pid << "/" << kStatusFilename;
  std::ifstream filestream(filename.str());
  string uid ;
  if (filestream.is_open()) {
      std::string line;
      while (std::getline(filestream, line)) {
        std::istringstream linestream(line);
        std::string key;
        linestream >> key;
        if (key == "Uid:") {
          	linestream >> uid;
        	break;
        }
      }
  }
  return uid; 
 }

string LinuxParser::User(int pid) { 
  std::string uid = Uid(pid);
  std::string userName;
  std::ifstream stream(kPasswordPath);
   long runningProcesses = 0;
  if (stream.is_open()) {
      std::string line;      
      while (std::getline(stream, line)) {
        std::replace(line.begin(), line.end(), ' ', '_');
        std::replace(line.begin(), line.end(), ':', ' ');
        std::istringstream linestream(line);
        std::string pwd;
        std::string currentUid;
        linestream >> userName >> pwd >> currentUid;
        if (currentUid == uid) {break;} 
      }
  }
  return userName; 
  }

long LinuxParser::UpTime(int pid) { 
  std::stringstream filename;
  filename << kProcDirectory << "/" << pid << "/" << kStatFilename;
  std::ifstream stream(filename.str());
  long starttime = 0;
  if (stream.is_open()) {
      std::string line;
      std::getline(stream, line);
      std::istringstream linestream(line);
      std::string ignore;
      for(int i = 0; i < 21; i++) linestream >> ignore;
      linestream >> starttime; 
      std::time_t now = std::time(0);
      std::time_t elapsedTime = LinuxParser::UpTime() - (starttime/sysconf(_SC_CLK_TCK));
      return elapsedTime;
  }
  return starttime; 
  }