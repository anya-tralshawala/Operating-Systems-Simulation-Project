#include "process.h"

process::process(int aState, int aPid, double aTime, std::vector<cpuBurst> aBurstList, int atotal_bursts, int abursts_completed, int atotal_time, int aremaining_time)

{
    // process id, arrival time, cpu bursts

    all_bursts = aBurstList;
    state = aState;
    arrival_time = aTime;
    pid = aPid;
    total_bursts = atotal_bursts;
    bursts_completed = abursts_completed;
    cpuBurst currently_on = cpuBurst();
    remaining_time = aremaining_time;
    total_time = atotal_time;
    wait_queue = 0;
}
process::process()
{
    std::vector<cpuBurst> emptylist;
    all_bursts = emptylist;
    state = 0;
    arrival_time = 0;
    pid = 0;
    cpuBurst currently_on = cpuBurst(0, 0);
    remaining_time = 0;
    total_time = 0;
    remaining_time = 0;
    total_time = 0;
    wait_queue = 0;
}

process::process(const process &i)
{
    all_bursts = i.all_bursts;
    state = i.state;
    arrival_time = i.arrival_time;
    pid = i.pid;
    total_bursts = i.total_bursts;
    bursts_completed = i.bursts_completed;
    remaining_time = i.remaining_time;
    total_time = i.total_time;
    cpuBurst currently_on = i.currently_on;
    wait_queue = i.wait_queue;
    wait_time = i.wait_time;
    
}

void process::removeBurst(cpuBurst removethis)
{
    std::vector<cpuBurst>::iterator itr;

    for (itr = all_bursts.begin(); itr != all_bursts.end(); ++itr)
    {
        if (itr->get_CPUtime() == removethis.get_CPUtime() && itr->get_IOtime() == removethis.get_IOtime())
        {
            itr = all_bursts.erase(itr);
            break;
        }
    }
}

// bool lesort_by()
// {
// }
