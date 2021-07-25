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

// bool lesort_by()
// {
// }