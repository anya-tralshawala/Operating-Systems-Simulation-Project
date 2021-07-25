#include "cpu.h"

cpu::cpu(process i)
{

    running_process = i;
    isrunning = false;
    current_time = 0;
    prev_burst = cpuBurst();
    waiting_time = 0;
    time_active = 0;
}

void cpu::setProcess(process i)
{
    running_process = i;
}
void cpu::setTime(double time)
{
    current_time = time;
}
void cpu::set_prev(cpuBurst i)
{
    prev_burst = i;
}
