#include "cpuBurst.h"

cpuBurst::cpuBurst(double aCPUT, double anIOTime)
{

    CPU_time = aCPUT;
    IO_time = anIOTime;
}
cpuBurst::cpuBurst()
{
    CPU_time = 0;
    IO_time = 0;
}

cpuBurst::cpuBurst(const cpuBurst &i)
{
    CPU_time = i.CPU_time;
    IO_time = i.IO_time;
}

// bool lesort_by()
// {
// }