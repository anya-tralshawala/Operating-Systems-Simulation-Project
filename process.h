#include <iostream>
#include <vector>
#include "cpuBurst.h"

class process
{
public:
    process(int aState, int aPid, double aTime, std::vector<cpuBurst> aBurstList, int atotal_bursts, int abursts_completed, cpuBurst acurrently_on, int atotal_time, int aremaining_time, int atau);
    process();
    process(const process &i);
    int pid;
    // ACCESSORS
    int getState() { return state; }
    int getpID() { return pid; }
    int getArrivialTime() { return arrival_time; }
    int getTotalBursts() { return total_bursts; }
    int getBurstsCompleted() { return bursts_completed; }
    int getRemainingBursts() { return all_bursts.size(); }
    int getRemainingTime() { return total_time - remaining_time; }
    int getTau() { return tau; }
    cpuBurst &getCurrent();
    double getWait() { return wait_queue; }
    std::vector<cpuBurst> &getAllBursts() { return all_bursts; }
    double getTurnaroundTime() { return turnaround_time; }
    double getCompletionTime() { return completion_time; }
    double getWaitTime() { return wait_time; }
    bool operator<(const process &other) const { return pid < other.pid; }
    double getSwitches(){return num_switches;}
    
    // cpuBurst cpuBurst::getBurstAt(int i){return all_bursts[i]}

    // MODIFIERS
    void setWaitTime(double set_time) { wait_time = set_time; }
    void setTurnaroundTime(double set_time) { turnaround_time = set_time; }
    void setCompletionTime(double set_time) { completion_time = set_time; }
    void removeBurst(cpuBurst removethis);
    void setBurst(cpuBurst &setthis) { currently_on = setthis; }
    void updateWaitTime() { wait_queue += 1; }
    void decWaitTime(double time) {wait_queue -= time;}
    void updateTau(double al, double time) {tau = (al * time) + ((1-al) * tau); }   
    void updateSwitches(double num){num_switches += num;} 
    //private member variables
private:
    std::vector<cpuBurst> all_bursts;
    cpuBurst currently_on;
    int state;
    double arrival_time;
    double num_switches;
    double wait_time;
    double turnaround_time;
    double completion_time;
    int bursts_completed;
    int total_bursts;
    int remaining_time;
    int total_time;
    double wait_queue;
    int tau;
};
//bools we will use in sorting
bool less_names();
