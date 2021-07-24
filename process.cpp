#include "process.h"

process::process(int aState, int aPid, double aTime,std::vector<cpuBurst> aBurstList)

{
    // process id, arrival time, cpu bursts
    all_bursts = aBurstList;
    state = aState;
    arrival_time = aTime;
    pid = aPid;
}

// bool lesort_by()
// {
// }
