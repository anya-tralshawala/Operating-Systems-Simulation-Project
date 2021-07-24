#include "process.h"

class cpu
{

public:
    cpu(process i);
    void setProcess(process i);
    bool checkstate(){return isrunning;}
    process getProcess(){return running_process;}
    void setState(bool setthis){isrunning = setthis;}
private:
    /* data */
    process running_process;
    bool isrunning;
};

