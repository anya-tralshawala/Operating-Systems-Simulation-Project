#include <iostream>
#include <vector>
#include "cpuBurst.h"

class process
{
public:
    process(int aState, int aPid, double aTime,std::vector<cpuBurst> aBurstList);
    // ACCESSORS
    int getState() { return state; }
    int getpID(){return pid;}
    int getArrivialTime() { return arrival_time; }

    // cpuBurst cpuBurst::getBurstAt(int i){return all_bursts[i]}

    // MODIFIERS
    void setWaitTime(double set_time){wait_time = set_time;}
    void setTurnaroundTime(double set_time){turnaround_time = set_time;}
    void setCompletionTime(double set_time){completion_time = set_time;}
    //private member variables
private:
    std::vector<cpuBurst> all_bursts;
    int state;
    double arrival_time;
    int pid;
    double wait_time;
    double turnaround_time;
    double completion_time;

};
//bools we will use in sorting
bool less_names();
