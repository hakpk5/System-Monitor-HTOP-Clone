#include <string>

#include "format.h"

using std::string;

string Format::ElapsedTime(long seconds[[maybe_unused]]) { 
long sec = seconds % 60;
long mins = (seconds / 60) % 60;
long hrs = (seconds / 60) / 60;

std::string hrs_str = std::to_string(hrs).size() == 1 ? "0" + std::to_string (hrs) : std::to_string(hrs);
std::string min_str = std::to_string(mins).size() == 1 ? "0" + std::to_string (mins) : std::to_string(mins);
std::string sec_str = std::to_string(sec).size() == 1 ? "0" + std::to_string (sec) : std::to_string(sec);

return (hrs_str + ":" + min_str + ":" + sec_str); 
}