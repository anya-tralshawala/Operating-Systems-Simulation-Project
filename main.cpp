
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <iomanip>
#include <string>
#include <algorithm>
#include "cpu.h"
#include <iostream>
#include <fstream>

std::string printQueue(std::vector<process> queue)
{
    if (queue.size() == 0)
    {
        std::string emptyQ = "[Q empty]";
        return emptyQ;
    }
    std::string Q = "[Q ";
    for (int i = 0; i < queue.size(); i++)
    {
        char temp = toupper(char(queue[i].getpID()));
        if (i != queue.size() - 1)
        {
            Q = Q + temp + " ";
        }
        else
        {
            Q = Q + temp;
        }
    }
    Q = Q + "]";
    return Q;
}

double avg_burst(std::vector<process> processes)
{
    //get average CPU burst time of all processes in vector
    double sum = 0;
    double size = 0;
    double avg = 0;
    for (int i = 0; i < processes.size(); i++)
    {
        //loop through all processes
        std::vector<cpuBurst> current_bursts = processes[i].getAllBursts();
        double burst_sum = 0;
        for (int j = 0; j < current_bursts.size(); j++)
        {
            //loop through all bursts
            burst_sum += current_bursts[j].get_CPUtime();
        }
        size += current_bursts.size();
        sum += burst_sum;
    }
    avg = sum / size;
    return avg;
}

double avg_wait_time(std::vector<process> completed_processes)
{
    double sum = 0;
    double size = 0;
    for (int i = 0; i < completed_processes.size(); i++)
    {
        sum += completed_processes[i].getWait();
        size += completed_processes[i].getTotalBursts();
    }

    return sum / size;
}

double avg_turnaround_time(std::vector<process> original, std::vector<process> completed_processes, double t_cs)
{
    double sum = 0;
    double size = 0;
    for (int i = 0; i < completed_processes.size(); i++)
    {
        sum += completed_processes[i].getTurnaroundTime();
        size += completed_processes[i].getTotalBursts();
    }
    return sum / size;
}
//algo go here

//First-come-first-served (FCFS)
void FCFS(std::vector<process> processes, double t_cs, int tau_initial, FILE *fp)
{
    int total_processes = processes.size();
    int completed = 0;
    int time = 0;
    int context_switch = 0;
    int preemptions = 0;
    double avg_CPU_burst_time = avg_burst(processes);
    std::vector<process> queue;
    std::vector<process> waitstate;
    std::vector<cpuBurst> emptylist;
    std::vector<process> completed_processes;
    std::vector<process> copy_process = processes;
    process i = process(0, 0, 0, emptylist, 0, 0, cpuBurst(), 0, 0, tau_initial);

    cpu aCPU = cpu(i);

    printf("time %dms: Simulator started for FCFS %s\n", time, printQueue(queue).c_str());

    /*start simulation, continue until all processes are completed*/
    while (completed != total_processes)
    {
        /*check if processes arrive*/
        for (int i = 0; i < total_processes; i++)
        {
            if (processes[i].getArrivialTime() == time)
            {
                queue.push_back(processes[i]);
                printf("time %dms: Process %c arrived; added to ready queue %s\n", time, toupper(char(processes[i].getpID())), printQueue(queue).c_str());
            }
        }
        /*check if waitIO queue has anything that finished*/
        if (waitstate.size() != 0)
        {

            //loop through, check if any IO has finished
            int save_size = waitstate.size();
            int i = 0;
            std::vector<process> temp_list;
            while (i < waitstate.size())
            {
                process curr = waitstate[i];
                curr.setBurst(aCPU.get_prev());
                //printf("curr: %f\n", curr.getWaitTime());
                //printf("time: %d\n", time);
                if (int(curr.getWaitTime()) == time)
                {
                    curr.setWaitTime(-99);
                    //process completed IO burst, add back to the ready queue
                    temp_list.push_back(curr);
                    //adjust for removal of process
                    save_size = waitstate.size();
                    std::vector<process>::iterator itr2 = waitstate.begin() + i;
                    waitstate.erase(itr2);
                }
                else
                {
                    i++;
                }
            }
            if (temp_list.size() > 0)
            {
                //sort based on pid and add to ready queue
                std::sort(temp_list.begin(), temp_list.end());
                for (int i = 0; i < temp_list.size(); i++)
                {
                    queue.push_back(temp_list[i]);
                    printf("time %dms: Process %c completed I/O; added to ready queue %s\n", time, toupper(char(temp_list[i].getpID())), printQueue(queue).c_str());
                }
            }
        }
        /*check is process is done running on CPU*/
        if (aCPU.checkstate() == true)
        {
            /*something is running on the CPU, check if it is finished*/
            process current = aCPU.getProcess();
            current.setBurst(current.getAllBursts()[0]);

            /*check if the CPU burst of the process is finished*/
            //ISSUE: time needs to be compared to CPUtime + original time it started
            //if(current.getCurrent().get_CPUtime() + aCPU.getTime() == time && aCPU.getContext() == 0)
            if (current.getCurrent().get_CPUtime() + aCPU.getTime() + int(t_cs / 2) == time)
            {

                aCPU.setState(false);
                current.removeBurst(current.getCurrent());
                int check_finished = current.getRemainingBursts();
                if (check_finished == 0)
                {
                    /*all CPU bursts completed, update number of completed processes*/
                    completed += 1;
                    completed_processes.push_back(current);

                    aCPU.setProcess(i);
                    aCPU.updateContext(t_cs / 2);
                    printf("time %dms: Process %c terminated %s\n", time, toupper(char(current.getpID())), printQueue(queue).c_str());

                    if (completed == total_processes)
                    {
                        //print stats: CHANGE TO PRINT TO FILE
                        time += t_cs / 2;

                        context_switch += 1;
                        double avg_wait = avg_wait_time(completed_processes);
                        double avg_turnaround = avg_CPU_burst_time + avg_wait + t_cs;
                        double CPU_active = (aCPU.getActive() / time) * 100;

                        printf("time %dms: Simulator ended for FCFS %s\n", int(time), printQueue(queue).c_str());
                        fprintf(fp, "Algorithm FCFS\n");
                        fprintf(fp, "-- average CPU burst time: %.3f ms\n", avg_CPU_burst_time);
                        fprintf(fp, "-- average wait time: %.3f ms\n", avg_wait);
                        fprintf(fp, "-- average turnaround time: %.3f ms\n", avg_turnaround);
                        fprintf(fp, "-- total number of context switches: %d\n", context_switch);
                        fprintf(fp, "-- total number of preemptions: %d\n", preemptions);
                        fprintf(fp, "-- CPU utilization: %.3f%%\n", CPU_active);

                        return;
                    }
                }
                else
                {
                    /*CPU burst finished, remove from burst list and switch waiting on IO*/
                    printf("time %dms: Process %c completed a CPU burst; %d bursts to go %s\n", time, toupper(char(current.getpID())), check_finished, printQueue(queue).c_str());

                    waitstate.push_back(current);
                    double switch_time = t_cs / 2;
                    double waiting_time = time + current.getCurrent().get_IOtime() + switch_time;
                    aCPU.set_prev(current.getCurrent());
                    waitstate.back().setWaitTime(waiting_time);
                    aCPU.updateContext(t_cs / 2);
                    printf("time %dms: Process %c switching out of CPU; will block on I/O until time %dms %s\n", int(time), toupper(char(current.getpID())), int(waiting_time), printQueue(queue).c_str());
                }
                context_switch += 1;
            }
        }

        /*check if ready queue is not empty, and if CPU is available*/
        if (aCPU.checkstate() == false && aCPU.getContext() == 0 && queue.size() != 0)
        {
            /*start running process*/
            /*set CPU state and update process running on CPU*/
            aCPU.setState(true);
            aCPU.setProcess(queue.front());
            aCPU.setTime(time);
            process nowrunning = queue.front();
            queue.erase(queue.begin());
            aCPU.updateActive(nowrunning.getAllBursts()[0].get_CPUtime());
            aCPU.updateContext(t_cs / 2);
            printf("time %dms: Process %c started using the CPU for %dms burst %s\n", int(time + t_cs / 2), toupper(char(nowrunning.getpID())), int(nowrunning.getAllBursts()[0].get_CPUtime()), printQueue(queue).c_str());
        }
        //update context switch var
        else if (aCPU.getContext() > 0)
        {
            aCPU.updateContext(-1);
        }
        //update wait times, CPU is not available processes waiting
        for (int k = 0; k < queue.size(); k++)
        {
            queue[k].updateWaitTime();
        }
        /*increment time variable*/
        time += 1;
    }
}

//Shortest job first (SJF)

bool comparision(process a, process b)
{

    return (a.getTotalBursts() < b.getTotalBursts());
}

void SJF(std::vector<process> processes, double t_cs, double alpha, int tau_initial, FILE *fp)
{
    int tau_before = tau_initial;
    int tau_new = 0;
    int total_processes = processes.size();
    int completed = 0;
    int time = 0;
    int context_switch = 0;
    int preemptions = 0;
    double avg_CPU_burst_time = avg_burst(processes);
    std::vector<process> queue;
    std::vector<process> waitstate;
    std::vector<cpuBurst> emptylist;
    std::vector<process> completed_processes;
    std::vector<process> copy_process = processes;
    int totalCPUtime = 0;
    process i = process(0, 0, 0, emptylist, 0, 0, cpuBurst(), 0, 0, tau_initial);
    cpu aCPU = cpu(i);

    printf("\n");
    printf("time %dms: Simulator started for SJF %s\n", time, printQueue(queue).c_str());

    /*start simulation, continue until all processes are completed*/
    while (completed != total_processes)
    {
        /*check if processes arrive*/
        /*check if processes arrive*/
        for (int i = 0; i < total_processes; i++)
        {
            if (processes[i].getArrivialTime() == time)
            {
                queue.push_back(processes[i]);
                printf("time %dms: Process %c arrived; added to ready queue %s\n", time, toupper(char(processes[i].getpID())), printQueue(queue).c_str());
            }
        }
        /*check if waitIO queue has anything that finished*/
        if (waitstate.size() != 0)
        {
            //loop through, check if any IO has finished
            int save_size = waitstate.size();
            int i = 0;
            std::vector<process> temp_list;
            while (i < waitstate.size())
            {
                //add to ready queue
                process curr = waitstate[i];
                curr.setBurst(aCPU.get_prev());
                if (int(curr.getWaitTime()) == time)
                {
                    curr.setWaitTime(-99);
                    //process completed IO burst, add back to the ready queue
                    temp_list.push_back(curr);
                    //adjust for removal of process
                    save_size = waitstate.size();
                    std::vector<process>::iterator itr2 = waitstate.begin() + i;
                    waitstate.erase(itr2);
                }
                else
                {
                    i++;
                }
            }
            if (temp_list.size() > 0)
            {
                //sort based on tau and add to ready queue
                //still need to fix tau sorting
                std::sort(temp_list.begin(), temp_list.end());
                for (int i = 0; i < temp_list.size(); i++)
                {
                    queue.push_back(temp_list[i]);
                    printf("time %dms: Process %c (tau %dms) completed I/O; added to ready queue %s\n", time, toupper(char(temp_list[i].getpID())), temp_list[i].getTau(), printQueue(queue).c_str());
                }
            }
        }

        if (aCPU.checkstate() == true)
        {
            /*something is running on the CPU, check if it is finished*/
            process current = aCPU.getProcess();
            current.setBurst(current.getAllBursts()[0]);

            /*check if the CPU burst of the process is finished*/
            if (current.getCurrent().get_CPUtime() + aCPU.getTime() + int(t_cs / 2) == time)
            {
                aCPU.setState(false);
                current.removeBurst(current.getCurrent());
                int check_finished = current.getRemainingBursts();
                if (check_finished == 0)
                {
                    /*all CPU bursts completed, update number of completed processes*/
                    completed += 1;
                    completed_processes.push_back(current);

                    aCPU.setProcess(i);

                    printf("time %dms: Process %c terminated %s\n", time, toupper(char(current.getpID())), printQueue(queue).c_str());

                    if (completed == total_processes)
                    {
                        //print stats: CHANGE TO PRINT TO FILE

                        time += t_cs / 2;
                        context_switch += 1;
                        double avg_wait = avg_wait_time(completed_processes);
                        double avg_turnaround = avg_CPU_burst_time + avg_wait + t_cs;
                        double CPU_active = (aCPU.getActive() / time) * 100;

                        printf("time %dms: Simulator ended for SJF %s\n", int(time), printQueue(queue).c_str());
                        fprintf(fp, "Algorithm SJF\n");
                        fprintf(fp, "-- average CPU burst time: %.3f ms\n", avg_CPU_burst_time);
                        fprintf(fp, "-- average wait time: %.3f ms\n", avg_wait);
                        fprintf(fp, "-- average turnaround time: %.3f ms\n", avg_turnaround);
                        fprintf(fp, "-- total number of context switches: %d\n", context_switch);
                        fprintf(fp, "-- total number of preemptions: %d\n", preemptions);
                        fprintf(fp, "-- CPU utilization: %.3f%%\n", CPU_active);

                        return;
                    }
                }
                else
                {
                    /*CPU burst finished, remove from burst list and switch waiting on IO*/
                    printf("time %dms: Process %c (tau %dms) completed a CPU burst; %d bursts to go %s\n", time, toupper(char(current.getpID())), current.getTau(), check_finished, printQueue(queue).c_str());

                    //CHANGE: recalculate tau for the process
                    int save_tau = current.getTau();
                    current.updateTau(alpha, totalCPUtime);
                    printf("time %dms: Recalculated tau from %dms to %dms for process %s\n", time, save_tau, current.getTau(), printQueue(queue).c_str());
                    waitstate.push_back(current);
                    double switch_time = t_cs / 2;
                    double waiting_time = time + current.getCurrent().get_IOtime() + switch_time;
                    aCPU.set_prev(current.getCurrent());
                    waitstate.back().setWaitTime(waiting_time);
                    aCPU.updateContext(t_cs / 2);
                    printf("time %dms: Process %c switching out of CPU; will block on I/O until time %dms %s\n", int(time), toupper(char(current.getpID())), int(waiting_time), printQueue(queue).c_str());
                }
                context_switch += 1;
            }
        }

        if (aCPU.checkstate() == false && aCPU.getContext() == 0 && queue.size() != 0)
        {
            /*start running process*/
            /*set CPU state and update process running on CPU*/
            aCPU.setState(true);
            aCPU.setProcess(queue.front());
            aCPU.setTime(time);
            process nowrunning = queue.front();
            queue.erase(queue.begin());
            aCPU.updateActive(nowrunning.getAllBursts()[0].get_CPUtime());
            aCPU.updateContext(t_cs / 2);
            totalCPUtime = ceil(int(nowrunning.getAllBursts()[0].get_CPUtime()));
            printf("time %dms: Process %c (tau %dms) started using the CPU for %dms burst %s\n", int(time + t_cs / 2), toupper(char(nowrunning.getpID())), nowrunning.getTau(), totalCPUtime, printQueue(queue).c_str());
        }
        //update context switch var
        else if (aCPU.getContext() > 0)
        {
            aCPU.updateContext(-1);
        }

        //update wait times, CPU is not available processes waiting
        for (int k = 0; k < queue.size(); k++)
        {
            queue[k].updateWaitTime();
        }
        /*increment time variable*/
        time += 1;
    }
}

//Shortest remaining time (SRT)

bool alphaSort(process a, process b)
{
    return (a < b);
}
void SRT(std::vector<process> processes, double t_cs, double alpha, int tau_initial, FILE *fp)
{
    int tau_before = tau_initial;
    int tau_new = 0;
    int total_processes = processes.size();
    int completed = 0;
    int time = 0;
    int context_switch = 0;
    int preemptions = 0;
    double avg_CPU_burst_time = avg_burst(processes);
    std::vector<process> queue;
    std::vector<process> waitstate;
    std::vector<cpuBurst> emptylist;
    std::vector<process> completed_processes;
    std::vector<process> copy_process = processes;
    int totalCPUtime = 0;
    process i = process(0, 0, 0, emptylist, 0, 0, cpuBurst(), 0, 0, tau_initial);
    cpu aCPU = cpu(i);

    printf("\n");
    printf("time %dms: Simulator started for SRT %s\n", time, printQueue(queue).c_str());

    /*start simulation, continue until all processes are completed*/
    while (completed != total_processes)
    {
        /*check if processes arrive*/
        for (int i = 0; i < total_processes; i++)
        {
            if (processes[i].getArrivialTime() == time)
            {

                //check if preemption occurs
                //CHANGE: instead of looking at a processes' arrival time, you should look at elapsed CPU time use
                //also, i think you should look at the process coming in (processes[i]) compared to the processes in the ready queue
                //something like :
                for (int j = 0; j < queue.size(); j++)
                {
                    if (processes[i].getTau() < queue[j].getTau() - queue[j].CPU_use_time())
                    {
                        //preemption occurs
                        queue.insert(queue.begin(), processes[i]);
                        preemptions += 1;
                    }
                }

                if (processes[i].getTau() == (processes[i - 1].getTau() - processes[i].CPU_use_time()))
                {
                    //CHANGE: here need to sort them alphabetically (if they have the same Tau values)
                    std::sort(queue.begin(), queue.end(), alphaSort);
                    queue.push_back(processes[i]);
                }
                else
                {
                    queue.push_back(processes[i]);
                }
                printf("time %dms: Process %c (tau %dms) arrived; added to ready queue %s\n", time, toupper(char(processes[i].getpID())), processes[i].getTau(), printQueue(queue).c_str());
            }
        }
        /*check if waitIO queue has anything that finished*/
        if (waitstate.size() != 0)
        {
            //loop through, check if any IO has finished
            int save_size = waitstate.size();
            int i = 0;
            std::vector<process> temp_list;
            while (i < waitstate.size())
            {
                //add to ready queue
                process curr = waitstate[i];
                curr.setBurst(aCPU.get_prev());
                if (int(curr.getWaitTime()) == time)
                {
                    curr.setWaitTime(-99);
                    //process completed IO burst, add back to the ready queue
                    temp_list.push_back(curr);
                    //adjust for removal of process
                    save_size = waitstate.size();
                    std::vector<process>::iterator itr2 = waitstate.begin() + i;
                    waitstate.erase(itr2);
                }
                else
                {
                    i++;
                }
            }
            if (temp_list.size() > 0)
            {
                //sort based on tau and add to ready queue
                //still need to fix tau sorting
                std::sort(temp_list.begin(), temp_list.end());
                for (int i = 0; i < temp_list.size(); i++)
                {
                    queue.push_back(temp_list[i]);
                    printf("time %dms: Process %c (tau %dms) completed I/O; added to ready queue %s\n", time, toupper(char(temp_list[i].getpID())), temp_list[i].getTau(), printQueue(queue).c_str());
                }
            }
        }

        if (aCPU.checkstate() == true)
        {
            /*something is running on the CPU, check if it is finished*/
            process current = aCPU.getProcess();
            current.setBurst(current.getAllBursts()[0]);

            /*check if the CPU burst of the process is finished*/
            if (current.getCurrent().get_CPUtime() + aCPU.getTime() + int(t_cs / 2) == time)
            {
                aCPU.setState(false);
                current.removeBurst(current.getCurrent());
                int check_finished = current.getRemainingBursts();
                if (check_finished == 0)
                {
                    /*all CPU bursts completed, update number of completed processes*/
                    completed += 1;
                    completed_processes.push_back(current);

                    aCPU.setProcess(i);

                    printf("time %dms: Process %c terminated %s\n", time, toupper(char(current.getpID())), printQueue(queue).c_str());

                    if (completed == total_processes)
                    {
                        //print stats: CHANGE TO PRINT TO FILE
                        time += t_cs / 2;
                        context_switch += 1;
                        double avg_wait = avg_wait_time(completed_processes);
                        double avg_turnaround = avg_CPU_burst_time + avg_wait + t_cs;
                        double CPU_active = (aCPU.getActive() / time) * 100;

                        printf("time %dms: Simulator ended for SRT %s\n", int(time), printQueue(queue).c_str());
                        fprintf(fp, "Algorithm SRT\n");
                        fprintf(fp, "-- average CPU burst time: %.3f ms\n", avg_CPU_burst_time);
                        fprintf(fp, "-- average wait time: %.3f ms\n", avg_wait);
                        fprintf(fp, "-- average turnaround time: %.3f ms\n", avg_turnaround);
                        fprintf(fp, "-- total number of context switches: %d\n", context_switch);
                        fprintf(fp, "-- total number of preemptions: %d\n", preemptions);
                        fprintf(fp, "-- CPU utilization: %.3f%%\n", CPU_active);

                        return;
                    }
                }
                else
                {
                    /*CPU burst finished, remove from burst list and switch waiting on IO*/
                    printf("time %dms: Process %c (tau %dms) completed a CPU burst; %d bursts to go %s\n", time, toupper(char(current.getpID())), current.getTau(), check_finished, printQueue(queue).c_str());
                    //recalculate tau for the process
                    int save_tau = current.getTau();
                    current.updateTau(alpha, totalCPUtime);
                    printf("time %dms: Recalculated tau from %dms to %dms for process %s\n", time, save_tau, current.getTau(), printQueue(queue).c_str());
                    waitstate.push_back(current);
                    double switch_time = t_cs / 2;
                    double waiting_time = time + current.getCurrent().get_IOtime() + switch_time;
                    aCPU.set_prev(current.getCurrent());
                    waitstate.back().setWaitTime(waiting_time);
                    aCPU.updateContext(t_cs / 2);
                    printf("time %dms: Process %c switching out of CPU; will block on I/O until time %dms %s\n", int(time), toupper(char(current.getpID())), int(waiting_time), printQueue(queue).c_str());
                }
                context_switch += 1;
            }
        }

        if (aCPU.checkstate() == false && aCPU.getContext() == 0 && queue.size() != 0)
        {
            /*start running process*/
            /*set CPU state and update process running on CPU*/
            aCPU.setState(true);
            aCPU.setProcess(queue.front());
            aCPU.setTime(time);
            process nowrunning = queue.front();
            queue.erase(queue.begin());
            aCPU.updateActive(nowrunning.getAllBursts()[0].get_CPUtime());
            aCPU.updateContext(t_cs / 2);
            totalCPUtime = ceil(int(nowrunning.getAllBursts()[0].get_CPUtime()));
            printf("time %dms: Process %c (tau %dms) started using the CPU for %dms burst %s\n", int(time + t_cs / 2), toupper(char(nowrunning.getpID())), nowrunning.getTau(), totalCPUtime, printQueue(queue).c_str());
        }
        //update context switch var
        else if (aCPU.getContext() > 0)
        {
            aCPU.updateContext(-1);
        }
        //update wait times, CPU is not available processes waiting
        for (int k = 0; k < queue.size(); k++)
        {
            queue[k].updateWaitTime();
        }
        /*increment time variable*/
        time += 1;
    }
}

//Round robin (RR)
void RR(std::vector<process> processes, double t_cs, double t_slice, int tau_inital, FILE *fp)
{

    int total_processes = processes.size();
    int completed = 0;
    int time = 0;
    int context_switch = 0;
    int preemptions = 0;
    double avg_CPU_burst_time = avg_burst(processes);
    std::vector<process> queue;
    std::vector<process> waitstate;
    std::vector<cpuBurst> emptylist;
    std::vector<process> completed_processes;
    std::vector<process> copy_process = processes;
    process i = process(0, 0, 0, emptylist, 0, 0, cpuBurst(), 0, 0, tau_inital);
    double time_switch = t_slice;
    process current = process();
    cpu aCPU = cpu(i);

    printf("time %dms: Simulator started for RR %s\n", time, printQueue(queue).c_str());

    /*start simulation, continue until all processes are completed*/
    while (completed != total_processes)
    {
        /*check is process is done running on CPU*/
        if (aCPU.checkstate() == true)
        {
            /*something is running on the CPU, check if it is finished*/
            aCPU.getProcess().setTurnaroundTime(aCPU.getProcess().getTurnaroundTime() + 1);
            current = aCPU.getProcess();
            aCPU.getProcess().setBurst(current.getAllBursts()[0]);

            /*check if the CPU burst of the process is finished*/
            /*OR if the time slice is expire*/
            if ((current.getCurrent().get_CPUtime() + aCPU.getTime() + int(t_cs / 2) == time) || (time_switch == time))
            {
                /*case 1 - process finished before the time slice expired */
                if ((current.getCurrent().get_CPUtime() + aCPU.getTime() + int(t_cs / 2) == time))
                {

                    aCPU.setState(false);
                    current.removeBurst(current.getCurrent());
                    int check_finished = current.getRemainingBursts();

                    if (check_finished == 0)
                    {
                        /*all CPU bursts completed, update number of completed processes*/
                        completed += 1;
                        current.setTurnaroundTime(current.getTurnaroundTime() + t_cs / 2);
                        completed_processes.push_back(current);

                        aCPU.setProcess(i);
                        aCPU.updateContext(t_cs / 2);
                        printf("time %dms: Process %c terminated %s\n", time, toupper(char(current.getpID())), printQueue(queue).c_str());

                        if (completed == total_processes)
                        {
                            //print stats: CHANGE TO PRINT TO FILE
                            time += t_cs / 2;

                            context_switch += 1;
                            double avg_wait = avg_wait_time(completed_processes);
                            double avg_turnaround = avg_turnaround_time(copy_process, completed_processes, t_cs);
                            double CPU_active = (aCPU.getActive() / time) * 100;
                            printf("time %dms: Simulator ended for RR %s\n", int(time), printQueue(queue).c_str());
                            fprintf(fp, "Algorithm RR\n");
                            fprintf(fp, "-- average CPU burst time: %.3f ms\n", avg_CPU_burst_time);
                            fprintf(fp, "-- average wait time: %.3f ms\n", avg_wait);
                            fprintf(fp, "-- average turnaround time: %.3f ms\n", avg_turnaround);
                            fprintf(fp, "-- total number of context switches: %d\n", context_switch);
                            fprintf(fp, "-- total number of preemptions: %d\n", preemptions);
                            fprintf(fp, "-- CPU utilization: %.3f%%\n", CPU_active);

                            return;
                        }
                    }
                    else
                    {
                        /*CPU burst finished, remove from burst list and switch waiting on IO*/
                        printf("time %dms: Process %c completed a CPU burst; %d bursts to go %s\n", time, toupper(char(current.getpID())), check_finished, printQueue(queue).c_str());

                        waitstate.push_back(current);
                        double switch_time = t_cs / 2;
                        double waiting_time = time + current.getCurrent().get_IOtime() + switch_time;
                        aCPU.set_prev(current.getCurrent());
                        waitstate.back().setWaitTime(waiting_time);
                        waitstate.back().setTurnaroundTime(waitstate.back().getTurnaroundTime() + t_cs / 2);
                        aCPU.updateContext(t_cs / 2);

                        printf("time %dms: Process %c switching out of CPU; will block on I/O until time %dms %s\n", int(time), toupper(char(current.getpID())), int(waiting_time), printQueue(queue).c_str());
                    }
                    context_switch += 1;
                }
                /*case 2 - time slice expired, preempt process*/
                else if ((time_switch == time) && (queue.size() != 0))
                {

                    //update no. of preemptions
                    preemptions += 1;
                    context_switch += 1;
                    aCPU.setState(false);

                    //preempt process
                    printf("time %dms: Time slice expired; process %c preempted with %dms to go %s\n", time, toupper(char(current.getpID())), int(current.getCurrent().get_CPUtime() - t_slice), printQueue(queue).c_str());
                    //update CPU context wait time, since process is preempted include time to switch out
                    //process and time to switch in new process
                    current.getCurrent().update_CPU(t_slice);
                    current.getAllBursts()[0].update_CPU(t_slice);
                    current.updateSwitches(1);
                    aCPU.setProcess(i);
                    queue.push_back(current);
                    queue.back().decWaitTime(t_cs / 2);
                    aCPU.updateContext(t_cs / 2);
                }
                else if ((time_switch == time) && (queue.size() == 0))
                {
                    //no preemption even though time slice expired, reset time_switch to next slice time
                    time_switch = time + t_slice;
                    //need to set the CPU time for the currently running process -- NOT WORKING
                    aCPU.setTime(aCPU.getTime() + t_slice);
                    aCPU.getProcess().getCurrent().update_CPU(t_slice);
                    aCPU.getProcess().getAllBursts()[0].update_CPU(t_slice);
                    printf("time %dms: Time slice expired; no preemption because ready queue is empty %s\n", time, printQueue(queue).c_str());
                }
            }
        }

        /*check if waitIO queue has anything that finished*/
        if (waitstate.size() != 0)
        {
            //loop through, check if any IO has finished
            int save_size = waitstate.size();
            int i = 0;
            std::vector<process> temp_list;
            while (i < waitstate.size())
            {
                process curr = waitstate[i];
                if (int(curr.getWaitTime()) == time)
                {
                    curr.setWaitTime(-99);
                    //process completed IO burst, add back to the ready queue
                    temp_list.push_back(curr);
                    //adjust for removal of process
                    save_size = waitstate.size();
                    std::vector<process>::iterator itr2 = waitstate.begin() + i;
                    waitstate.erase(itr2);
                }
                else
                {
                    i++;
                }
            }
            if (temp_list.size() > 0)
            {
                //sort based on pid and add to ready queue
                std::sort(temp_list.begin(), temp_list.end());
                for (int i = 0; i < temp_list.size(); i++)
                {
                    queue.push_back(temp_list[i]);
                    printf("time %dms: Process %c completed I/O; added to ready queue %s\n", time, toupper(char(temp_list[i].getpID())), printQueue(queue).c_str());
                }
            }
        }
        /*check if processes arrive*/
        for (int i = 0; i < total_processes; i++)
        {
            if (processes[i].getArrivialTime() == time)
            {
                queue.push_back(processes[i]);
                printf("time %dms: Process %c arrived; added to ready queue %s\n", time, toupper(char(processes[i].getpID())), printQueue(queue).c_str());
            }
        }
        /*check if ready queue is not empty, and if CPU is available*/

        if (aCPU.checkstate() == false && aCPU.getContext() == 0 && queue.size() != 0)
        {
            /*start running process*/
            /*set CPU state and update process running on CPU*/
            aCPU.setState(true);
            aCPU.setProcess(queue.front());
            aCPU.setTime(time);
            process nowrunning = queue.front();
            queue.erase(queue.begin());
            aCPU.updateContext(t_cs / 2);
            if (int(nowrunning.getAllBursts()[0].get_CPUtime()) == int(nowrunning.getAllBursts()[0].getOriginal()))
            {
                printf("time %dms: Process %c started using the CPU for %dms burst %s\n", int(time + t_cs / 2), toupper(char(nowrunning.getpID())), int(nowrunning.getAllBursts()[0].get_CPUtime()), printQueue(queue).c_str());
                aCPU.updateActive(nowrunning.getAllBursts()[0].get_CPUtime());
            }
            else
            {
                printf("time %dms: Process %c started using the CPU for remaining %dms of %dms burst %s\n", int(time + t_cs / 2), toupper(char(nowrunning.getpID())), int(nowrunning.getAllBursts()[0].get_CPUtime()), int(nowrunning.getAllBursts()[0].getOriginal()), printQueue(queue).c_str());
            }
            /*set time slice*/
            time_switch = t_slice + (time + t_cs / 2);
        }
        //update context switch var
        else if (aCPU.getContext() > 0)
        {
            aCPU.updateContext(-1);
        }
        //update wait times, CPU is not available processes waiting
        for (int k = 0; k < queue.size(); k++)
        {
            queue[k].updateWaitTime();
            queue[k].setTurnaroundTime(queue[k].getTurnaroundTime() + 1);
        }

        //increment time
        time += 1;
    }
}

double next_exp(double lambda, double upper_bound)
{
    //generate next exp list pseudo-random number
    double x = 0;
    while (1)
    {
        double r = drand48();
        x = -log(r) / lambda;
        if (x <= upper_bound)
        {
            break;
        }
    }
    return x;
}

std::vector<process> create_processes(int n, int seed, double lambda, double upper_bound, int tau_initial)
{
    std::vector<process> return_vector;
    char alphabet[26];
    for (int letter = 'a'; letter <= 'z'; letter++)
    {
        alphabet[letter - 'a'] = letter;
    }

    for (int i = 0; i < n; i++)
    {
        //number of bursts is ceil(drand48() * 100)
        double arrival_time = floor(next_exp(lambda, upper_bound));
        std::vector<cpuBurst> aBurstList;

        //populate burst list
        int num_bursts = ceil(drand48() * 100);

        for (int j = 0; j < num_bursts; j++)
        {
            double CPU_burst_time = ceil(next_exp(lambda, upper_bound));
            // printf("%c, %f\n", char(alphabet[i]), CPU_burst_time);
            double IO_time;
            if (j == num_bursts - 1)
            {
                IO_time = -99;
            }
            else
            {
                IO_time = ceil(next_exp(lambda, upper_bound)) * 10;
            }

            cpuBurst current_burst = cpuBurst(CPU_burst_time, IO_time);

            aBurstList.push_back(current_burst);
        }

        process current_process = process(0, alphabet[i], arrival_time, aBurstList, num_bursts, 0, cpuBurst(), 0, 0, tau_initial);
        return_vector.push_back(current_process);
    }

    /*For each of these CPU bursts, identify the CPU burst time and the I/O burst time as the “ceiling” 
    of the next two random numbers in the sequence given by next_exp(); multiply the I/O burst time by 
    10 such that I/O burst time is generally an order of magnitude slower than CPU burst time; for the 
    last CPU burst, do not generate an I/O burst time (since each process ends with a final CPU burst)*/

    return return_vector;
}

int main(int argc, char *argv[])
{

    FILE *myfile = fopen("simout.txt", "w");

    int n = std::stoi(argv[1]);
    int seed = std::stoi(argv[2]);
    double lambda = std::stod(argv[3]);
    double upper_bound = std::stod(argv[4]);
    double t_cs = std::stod(argv[5]);
    double alpha = std::stod(argv[6]);
    double t_slice = std::stod(argv[7]);
    double tau_initial = 1 / lambda;

    // create vector of processes
    srand48(seed);
    std::vector<process> processes = create_processes(n, seed, lambda, upper_bound, tau_initial);

    for (int i = 0; i < processes.size(); i++)
    {
        printf("Process %c (arrival time %d ms) %d CPU bursts (tau %dms)\n", toupper(char(processes[i].getpID())), processes[i].getArrivialTime(), processes[i].getTotalBursts(), int(tau_initial));
    }
    printf("\n");
    printf("\n");

    FCFS(processes, t_cs, tau_initial, myfile);
    SJF(processes, t_cs, alpha, tau_initial, myfile);
    SRT(processes, t_cs, alpha, tau_initial, myfile);
    RR(processes, t_cs, t_slice, tau_initial, myfile);
    return 0;
}
