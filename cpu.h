#include "process.h"

class cpu
{

public:
    cpu(process i);
    void setProcess(process i);
    bool checkstate(){return isrunning;}
    process getProcess(){return running_process;}
    void setState(bool setthis){isrunning = setthis;}
    void setTime(double time);
    double getTime(){return current_time;}
private:
    /* data */
    process running_process;
    bool isrunning;
    double current_time;
};

