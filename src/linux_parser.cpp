#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <cassert>
#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;
using std::ifstream;
using std::ofstream;
// Read data from the filesystem

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
  filestream.close();
return value;
}

// read data from the filesystem
string LinuxParser::Kernel() {
  string os, kernel, version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel ;
  }
  stream.close();
return kernel;
}

// To update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str()); // converts C++ style string to null terminated string having array of bytes
  struct dirent* file; // for opening the file in read  only mode
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

// Read and return the system memory utilization
float LinuxParser::MemoryUtilization() { 
float mem_util, var_val = 0.0f, mem_total  /* adding up the other value associated with key*/;
//----------- Formula used to calculate the mem_util -----------------------------//
//----------- Mem: used =	MemTotal - MemFree - Buffers - Cached - Slab -----------//
string key, value, line;
ifstream stream (kProcDirectory + kMeminfoFilename);
assert(stream.is_open());
    while(std::getline(stream, line)){
    std::istringstream linestream (line);
    linestream >> key >> value;
    if (key == "MemTotal:")
      mem_util = std::stoi(value);
    else if(key == "MemFree:")
      var_val += std::stoi (value);
    else if (key == "Buffers:")
      var_val += std::stoi (value);
    else if (key == "Cached:")
      var_val += std::stoi (value);
    else if (key == "Slab:")
      var_val += std::stoi (value);
  }
mem_total = mem_util;  
mem_util -= var_val / mem_total;
stream.close();
return mem_util; 
}

// : Read and return the system uptime
long LinuxParser::UpTime() { 
  ifstream stream (kProcDirectory + kUptimeFilename);
  string sys_uptime;
  string line;
  if(stream.is_open()){
    std::getline(stream, line, ' ');
    std::istringstream linestream(line);
    linestream >> sys_uptime;
  }
  stream.close();
return std::stol(sys_uptime);
}

// Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { 
return ActiveJiffies() + IdleJiffies();
}

// Read and return the number of active jiffies/processor utilization for a PID
long LinuxParser::ActiveJiffies(int pid) { 
    std::ifstream stream (kProcDirectory + std::to_string (pid) + kStatFilename);
    std::string line;
    std::getline (stream, line);
    stream.close();
    std::istringstream linestream (line);
    std::vector <string> data {std::istream_iterator <string> {linestream}, std::istream_iterator <string> {}};
    long total_time = std::stol(data[13]) + std::stol(data[14]) + std::stol(data[15]) + std::stol(data[16]);  
    stream.close();
return total_time;
}

// Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { 
vector<string> active_jiff = CpuUtilization();
return std::stol(active_jiff[kUser_]) + std::stol(active_jiff[kNice_])
        + std::stol(active_jiff[kSystem_]) + std::stol(active_jiff[kIRQ_]) 
        + std::stol(active_jiff[kSoftIRQ_]) + std::stol(active_jiff[kSteal_]) 
        + std::stol(active_jiff[kGuest_]) + std::stol(active_jiff[kGuestNice_]);       
}

// Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { 
  vector<string> idle_jiff = CpuUtilization();
  return std::stol (idle_jiff[kIdle_]) + std::stol (idle_jiff[kIOwait_]);
}

// Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() { 
  ifstream cpu_file (kProcDirectory + kStatFilename);
  assert(cpu_file.is_open());
  string cpu_line;
  std::getline (cpu_file, cpu_line);
  cpu_file.close();
  std::istringstream cpu_stream (cpu_line);
  vector<string> cpu_data {std::istream_iterator <string> {cpu_stream}, std::istream_iterator <string> {}};
  if (cpu_data[0] == "cpu"){
    cpu_data.erase(cpu_data.begin());
    return cpu_data;
  }
return {};
}

// Read and return the total number of processes
int LinuxParser::TotalProcesses() { 
  string key, all_Processes, line;
  ifstream stream (kProcDirectory + kStatFilename);
  assert(stream.is_open()); // to be replaced with if, left for debugging purposes.
  while (!stream.eof()){
      std::getline(stream, line);
      std::istringstream linestream(line);
      linestream >> key >> all_Processes; 
      if (key == "processes"){
        stream.close();
        return std::stoi(all_Processes); 
      }
    }
return 0;
}

// Read and return the number of running processes
int LinuxParser::RunningProcesses() { 
  string key, procs_running, line;
  ifstream stream (kProcDirectory + kStatFilename);
  assert(stream.is_open()); // to be replaced with if, left for debugging purposes.
  while (std::getline(stream, line)){
      std::istringstream linestream(line);
      linestream >> key >> procs_running;
      if (key == "procs_running"){
        stream.close();
        return std::stoi(procs_running); 
      }
  }
return 0;
}

// Read and return the command associated with a process
string LinuxParser::Command(int pid) { 
  string cmd;
  ifstream stream (kProcDirectory + std::to_string(pid) + kCmdlineFilename);
  assert(stream.is_open());
  std::getline (stream, cmd);
  if (cmd.length() > 0){
    stream.close();
    return cmd; 
  }
return string();
}

// Read and return the memory used by a process
string LinuxParser::Ram(int pid) { 
  ifstream stream (kProcDirectory + std::to_string(pid) + kStatusFilename); // cd /proc/[pid]/status
  string key, value, line;
  long ram_util = 0;  
  assert(stream.is_open());
  while (!stream.eof()){
    std::getline(stream, line);
    std::istringstream linestream (line);
    linestream >> key >> value;
      if (key == "VmSize:"){
        ram_util = std::stol(value);
        stream.close();
        break;
      }
  }
ram_util = ram_util / 1024; // Conerting to mega bytes
value = to_string (ram_util);   
return value; 
}

// Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) { 
  string process_dir = std::to_string (pid);
  ifstream stream (kProcDirectory + process_dir + kStatusFilename); // cd /proc/[pid]/status
  string key, value, line;
  assert(stream.is_open());
  while (!stream.eof()){
    std::getline(stream, line);
    std::istringstream linestream (line);
    linestream >> key >> value;
    if (key == "Uid:"){
      stream.close();
      return value;
    }
  }
return string();
}

// Read and return the user associated with a process
string LinuxParser::User(int pid) { 
  // string process_dir = std::to_string (pid);
  ifstream stream (kPasswordPath); // cd /etc/passwd --> password file/
  string proc_uid = Uid(pid);  
  string proc_owner, is_encrypted, uid, line; // chronic:x:uid
  assert(stream.is_open());
  while(!stream.eof()){
    std::getline(stream, line);
    std::istringstream linestream(line);
    std::getline(linestream, proc_owner, ':');
    std::getline(linestream, is_encrypted, ':');
    std::getline(linestream, uid, ':');
    if(proc_uid == uid){
      stream.close();
      return proc_owner;
    }
  }
  return {};
}
// Read and return the uptime of a process
long LinuxParser::UpTime(int pid) { 
    std::ifstream stream (kProcDirectory + std::to_string (pid) + kStatFilename);
    assert(stream.is_open());
    std::string line;
    std::getline (stream, line);
    stream.close();
    std::istringstream linestream (line);
    std::vector<string> data {std::istream_iterator<string> {linestream}, std::istream_iterator<string>{}};
    long uptime = LinuxParser::UpTime() -  std::stol(data[21]) / sysconf (_SC_CLK_TCK);
return uptime;
}