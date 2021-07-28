#include "process.h"

class cpu
{

public:
    cpu(process i);
    void setProcess(process i);
    bool checkstate() { return isrunning; }
    process& getProcess() { return running_process; }
    void setState(bool setthis) { isrunning = setthis; }
    void setTime(double time);
    double getTime() { return current_time; }
    void set_prev(cpuBurst i);
    cpuBurst& get_prev() { return prev_burst; }
    void set_waiting(double time) { waiting_time = time; }
    double get_waiting() { return waiting_time; }
    void updateActive(double time) { time_active += time; }
    double getActive() { return time_active; }
    void updateContext(double time){context_time += time;}
    double getContext(){return context_time;}

private:
    /* data */
    process running_process;
    bool isrunning;
    double current_time;
    cpuBurst prev_burst;
    double waiting_time;
    double time_active;
    double context_time;
};
