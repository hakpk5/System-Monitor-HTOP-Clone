#include "processor.h"
#include "linux_parser.h"
Processor::Processor() {
    Utilization();
}
// TODO: Return the aggregate CPU utilization
float Processor::Utilization() {
    float fraction ;
    long active = LinuxParser::ActiveJiffies();
    long total = LinuxParser::Jiffies();
    fraction = float(active- prev_active) / float(total - prev_total);
    prev_active = active;
    prev_total = total;
    return fraction;
}
