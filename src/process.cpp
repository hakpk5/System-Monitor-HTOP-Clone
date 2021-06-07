#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>
#include <cassert>

#include "process.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process(int pid) : pid_(pid), cpuusage_ (CpuUtilization()) {} // Defining a CTOR for getting the process id whenever it gets created
// Return this process's ID
int Process::Pid() { return pid_; }

// Return this process's CPU utilization
float Process::CpuUtilization() { 
unsigned long int active_jiff  = LinuxParser::ActiveJiffies(pid_);
unsigned long int start_time = LinuxParser::UpTime(pid_);
unsigned long int uptime = LinuxParser::UpTime();
unsigned long seconds = uptime - start_time;
if (seconds <= 0.0f)
    return 0.0f;
double cpu_usage = double (100 * ((active_jiff/sysconf(_SC_CLK_TCK)) / seconds));
return cpu_usage;   
}

// Return the command that generated this process
string Process::Command() {return LinuxParser::Command(Pid());}

// Return this process's memory utilization
string Process::Ram() { return LinuxParser::Ram(Pid()); }

// Return the user (name) that generated this process
string Process::User() { return LinuxParser::User(Pid()); }

// Return the age of this process (in seconds)
long int Process::UpTime() { return LinuxParser::UpTime(Pid()); }

// Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process& a) { 
return (this->CpuUtilization() > a.CpuUtilization()); }
