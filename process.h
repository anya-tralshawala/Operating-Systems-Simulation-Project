#include <iostream>
#include <vector>
#include "cpuBurst.h"

class process
{
public:
    process(int aState, int aPid, double aTime,std::vector<cpuBurst> aBurstList, int atotal_bursts, int abursts_completed);
    process();
    process(const process &i);

    // ACCESSORS
    int getState() { return state; }
    int getpID(){return pid;}
    int getArrivialTime() { return arrival_time; }
    int getTotalBursts(){ return total_bursts;}
    int getBurstsCompleted(){ return bursts_completed;}
    int getRemainingBursts(){ return all_bursts.size();}
    cpuBurst getCurrent(){return currently_on;}
    
    std::vector<cpuBurst> getAllBursts(){return all_bursts;}
    // cpuBurst cpuBurst::getBurstAt(int i){return all_bursts[i]}

    // MODIFIERS
    void setWaitTime(double set_time){wait_time = set_time;}
    void setTurnaroundTime(double set_time){turnaround_time = set_time;}
    void setCompletionTime(double set_time){completion_time = set_time;}
    void removeBurst(cpuBurst removethis);
    void setBurst(cpuBurst setthis){currently_on = setthis;}
    //private member variables
private:
    std::vector<cpuBurst> all_bursts;
    cpuBurst currently_on;
    int state;
    double arrival_time;
    int pid;
    double wait_time;
    double turnaround_time;
    double completion_time;
    int bursts_completed;
    int total_bursts;

};
//bools we will use in sorting
bool less_names();

