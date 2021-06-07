#ifndef PROCESSOR_H
#define PROCESSOR_H

class Processor {
 public:
  float Utilization();  
  Processor();
 private:
  long prev_active{0};
  long prev_total {0};
};

#endif