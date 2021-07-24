#include "process.h"

class cpu
{

public:
    cpu(process i);
    void setProcess(process i){running_process = i;}
    bool checkstate(){return isrunning;}
    process getProcess(){return running_process;}
private:
    /* data */
    process running_process;
    bool isrunning;
};

