#include "cpuBurst.h"
#include <iostream>
#include <fstream>
cpuBurst::cpuBurst(double aCPUT, double anIOTime)
{

    CPU_time = aCPUT;
    IO_time = anIOTime;
    starting_CPUtime = aCPUT;
}
cpuBurst::cpuBurst()
{
    CPU_time = 0;
    IO_time = 0;
    starting_CPUtime = 0;
}

cpuBurst::cpuBurst(const cpuBurst &i)
{
    CPU_time = i.CPU_time;
    IO_time = i.IO_time;
    starting_CPUtime = i.starting_CPUtime;
}
void cpuBurst::update_CPU(double time) {
    
    CPU_time -= time;
    
}
bool cpuBurst::isOriginalTime(){
    return (CPU_time == starting_CPUtime);
}


// bool lesort_by()
// {
// }