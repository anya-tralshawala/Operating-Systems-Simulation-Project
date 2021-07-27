
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <iomanip>
#include <string>
#include <algorithm>
//#include <bits/stdc++.h>
#include "cpu.h"

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

//algo go here

//First-come-first-served (FCFS)
void FCFS(std::vector<process> processes, double t_cs, int tau_initial)
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
    process i = process(0, 0, 0, emptylist, 0, 0, 0, 0, tau_initial);

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

                    printf("time %dms: Process %c completed I/O; added to ready queue %s\n", time, toupper(char(curr.getpID())), printQueue(queue).c_str());
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
                        printf("Algorithm FCFS\n");
                        printf("-- average CPU burst time: %.3f ms\n", avg_CPU_burst_time);
                        printf("-- average wait time: %.3f ms\n", avg_wait);
                        printf("-- average turnaround time: %.3f ms\n", avg_turnaround);
                        printf("-- total number of context switches: %d\n", context_switch);
                        printf("-- total number of preemptions: %d\n", preemptions);
                        printf("-- CPU utilization: %.3f%%\n", CPU_active);

                        return;
                    }
                }
                else
                {
                    /*CPU burst finished, remove from burst list and switch waiting on IO*/
                    printf("time %dms: Process %c completed a CPU burst; %d bursts to go %s\n", time, toupper(char(current.getpID())), check_finished, printQueue(queue).c_str());

                    waitstate.push_back(current);
                    //printf("incpu: %f\n", current.getCurrent().get_IOtime());
                    double switch_time = t_cs / 2;
                    double waiting_time = time + current.getCurrent().get_IOtime() + switch_time;
                    //printf("%d, %f, %f\n", time, current.getCurrent().get_IOtime(), switch_time);
                    aCPU.set_prev(current.getCurrent());
                    //current.removeBurst(current.getCurrent());
                    waitstate.back().setWaitTime(waiting_time);
                    //aCPU.setTime(time + t_cs/2);
                    //aCPU.set_waiting(waiting_time);
                    aCPU.updateContext(t_cs / 2);
                    printf("time %dms: Process %c switching out of CPU; will block on I/O until time %dms %s\n", int(time), toupper(char(current.getpID())), int(waiting_time), printQueue(queue).c_str());
                }
                context_switch += 1;
            }
        }
        //printf("%s\n", printQueue(queue).c_str());
        /*check if ready queue is not empty, and if CPU is available*/
        //if (queue.size() != 0)
        //{

        //printf("CPU: %f\n",aCPU.getContext());
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

        //}

        /*increment time variable*/
        time += 1;
    }
}
//Shortest job first (SJF)

//Shortest remaining time (SRT)
void SRT(std::vector<process> processes, double t_cs, double alpha, int tau_initial)
{
    int tau_before = tau_initial;
    int tau_added = 0;
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
    int remaining_time[total_processes];
    int shortest = 0;
    int min_time = INT_MAX;
    bool check = false;
    int finish_time;
    process i = process(0, 0, 0, emptylist, 0, 0, 0, 0, tau_initial);

    int tau[2] = {tau_initial, tau_initial};
    tau[0] = tau_before;
    tau[1] = tau_added;

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
                for (int j = 0; j < queue.size(); j++)
                {
                    // do ordering of queue here
                }
                //tau calculation [still need to work on]
                if (processes[i].getTau() < processes[i - 1].getTau() - processes[i].getRemainingTime())
                {
                    queue.insert(queue.begin(), processes[i]);
                }
                else
                {
                    queue.push_back(processes[i]);
                }
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
                //check if preempt process is running
                // if it preempts then do not remove current cpu burst and add to queue
                // if (processes[i].getTau() < processes[i - 1].getTau() - processes[i].getRemainingTime())
                // {
                //     process nowrunning = queue.front();
                //     tau_added = tau[i + 1];
                //     tau_added = (alpha * int(nowrunning.getAllBursts()[0].get_CPUtime())) + ((1 - alpha) * tau[i]);
                //     printf("time %dms: Recalculated tau from %dms to %dms for process %s\n", time, tau_before, tau_added, printQueue(queue).c_str());
                //     tau_added = tau_before;
                // }

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

                    printf("time %dms: Process %c (tau %dms) completed I/O; added to ready queue %s\n", time, toupper(char(curr.getpID())), tau_initial, printQueue(queue).c_str());
                }

                else
                {
                    i++;
                }
            }
            if (temp_list.size() > 0)
            {
                std::sort(temp_list.begin(), temp_list.end());
                for (int i = 0; i < temp_list.size(); i++)
                {
                    if (temp_list[i].getRemainingTime() < time)
                    {
                        min_time = temp_list[i].getRemainingTime();
                        shortest = i;
                        check = true;

                        // insert in the front of queue
                        queue.insert(queue.begin(), temp_list[i]);
                    }
                    else
                    {
                        queue.push_back(temp_list[i]);
                    }
                }
            }
        }

        if (aCPU.checkstate() == true)
        {

            /*something is running on the CPU, check if it is finished*/
            process current = aCPU.getProcess();
            current.setBurst(current.getAllBursts()[0]);

            /*check if the CPU burst of the process is finished*/
            if (current.getCurrent().get_CPUtime() + aCPU.getTime() == time)
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

                    printf("time %dms: Process %c (tau %dms) terminated %s\n", time, toupper(char(current.getpID())), tau_before, printQueue(queue).c_str());

                    if (completed == total_processes)
                    {
                        //print stats: CHANGE TO PRINT TO FILE
                        time += t_cs / 2;
                        context_switch += 1;
                        double avg_wait = avg_wait_time(completed_processes);
                        double avg_turnaround = avg_CPU_burst_time + avg_wait + t_cs;
                        double CPU_active = (aCPU.getActive() / time) * 100;
                        printf("time %dms: Simulator ended for SRT %s\n", int(time), printQueue(queue).c_str());
                        printf("Algorithm SRT\n");
                        printf("-- average CPU burst time: %.3f ms\n", avg_CPU_burst_time);
                        printf("-- average wait time: %.3f ms\n", avg_wait);
                        printf("-- average turnaround time: %.3f ms\n", avg_turnaround);
                        printf("-- total number of context switches: %d\n", context_switch);
                        printf("-- total number of preemptions: %d\n", preemptions);
                        printf("-- CPU utilization: %.3f%%\n", CPU_active);
                        return;
                    }
                }
                else
                {
                    /*CPU burst finished, remove from burst list and switch waiting on IO*/
                    printf("time %dms: Process %c (tau %dms) completed a CPU burst; %d bursts to go %s\n", time, toupper(char(current.getpID())), tau_before, check_finished, printQueue(queue).c_str());

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
            else
            {
                //recaluculate tau
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
            printf("time %dms: Process %c (tau %dms) started using the CPU for %dms burst %s\n", int(time + t_cs / 2), toupper(char(nowrunning.getpID())), tau_before, int(nowrunning.getAllBursts()[0].get_CPUtime()), printQueue(queue).c_str());
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
void RR(std::vector<process> processes, double t_cs, double t_slice){
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
    process i = process(0, 0, 0, emptylist, 0, 0, 0, 0);
    double time_switch = t_slice;
    cpu aCPU = cpu(i);

    printf("time %dms: Simulator started for RR %s\n", time, printQueue(queue).c_str());

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

                    printf("time %dms: Process %c completed I/O; added to ready queue %s\n", time, toupper(char(curr.getpID())), printQueue(queue).c_str());
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
            /*OR if the time slice is expire*/
            
            if ((current.getCurrent().get_CPUtime() + aCPU.getTime() + int(t_cs / 2) == time)|| (time_switch == time))

            {
                
                /*case 1 - process finished before the time slice expired OR time slice expired but queue is empty*/
                if((current.getCurrent().get_CPUtime() + aCPU.getTime() + int(t_cs / 2) == time)){
                    
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
                            printf("time %dms: Simulator ended for RR %s\n", int(time), printQueue(queue).c_str());
                            printf("Algorithm FCFS\n");
                            printf("-- average CPU burst time: %.3f ms\n", avg_CPU_burst_time);
                            printf("-- average wait time: %.3f ms\n", avg_wait);
                            printf("-- average turnaround time: %.3f ms\n", avg_turnaround);
                            printf("-- total number of context switches: %d\n", context_switch);
                            printf("-- total number of preemptions: %d\n", preemptions);
                            printf("-- CPU utilization: %.3f%%\n", CPU_active);

                            return;
                        }
                    }
                    else
                    {
                        /*CPU burst finished, remove from burst list and switch waiting on IO*/
                        printf("time %dms: Process %c completed a CPU burst; %d bursts to go %s\n", time, toupper(char(current.getpID())), check_finished, printQueue(queue).c_str());

                        waitstate.push_back(current);
                        //printf("incpu: %f\n", current.getCurrent().get_IOtime());
                        double switch_time = t_cs / 2;
                        double waiting_time = time + current.getCurrent().get_IOtime() + switch_time;
                        //printf("%d, %f, %f\n", time, current.getCurrent().get_IOtime(), switch_time);
                        aCPU.set_prev(current.getCurrent());
                        //current.removeBurst(current.getCurrent());
                        waitstate.back().setWaitTime(waiting_time);
                        //aCPU.setTime(time + t_cs/2);
                        //aCPU.set_waiting(waiting_time);
                        aCPU.updateContext(t_cs / 2);

                        printf("time %dms: Process %c switching out of CPU; will block on I/O until time %dms %s\n", int(time), toupper(char(current.getpID())), int(waiting_time), printQueue(queue).c_str());
                        
                    }
                    context_switch += 1;
                }
                /*case 2 - time slice expired, preempt process*/
                else if((time_switch == time) && (queue.size() != 0)){
                    //update no. of preemptions
                    preemptions += 1;
                    aCPU.setState(false);
                    //not working
                    
                    aCPU.setProcess(i);
                    //preempt process
                    printf("%dms: Time slice expired; process %c preempted with %d to go %s\n", time, toupper(char(current.getpID())), int(current.getCurrent().get_CPUtime() - t_slice), printQueue(queue).c_str());
                    //update CPU context wait time, since process is preempted include time to switch out
                    //process and time to switch in new process
                    queue.push_back(current);
                    aCPU.updateContext(t_cs/2);
                    //update current process remaining CPU time -- NOT WORKING
                    queue.back().getCurrent().update_CPU(t_slice);

                    printf("new:%f\n",queue.back().getCurrent().get_CPUtime());
                }
                else if((time_switch == time) && (queue.size() == 0)){
                    //no preemption even though time slice expired
                    //QUESTION: reset time slice here?
                    
                    printf("%dms: Time slice expired; no preemption because ready queue is empty %s\n", time, printQueue(queue).c_str());
                   
                }
                
            }
        }
        //printf("%s\n", printQueue(queue).c_str());
        /*check if ready queue is not empty, and if CPU is available*/
        //if (queue.size() != 0)
        //{

        //printf("CPU: %f\n",aCPU.getContext());
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
            /*set time slice*/
            time_switch = t_slice + (time + t_cs/2);
            
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

        //increment time
        time += 1;
    }

}
//Skipping preemptions in RR

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
            printf("%c, %f\n", char(alphabet[i]), CPU_burst_time);
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

        process current_process = process(0, alphabet[i], arrival_time, aBurstList, num_bursts, 0, 0, 0, tau_initial);
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
    /*
    for(int i = 0; i < processes.size(); i++){
        std::cout<<"in the vector pID is: "<<processes[i].getpID()<<"\n";
        std::cout<<"in the vector pID is: "<<processes[i].getArrivialTime()<<"\n";
        std::cout<<"in the vector pID is: "<<processes[i].getState()<<"\n";
    }*/

    FCFS(processes, t_cs, tau_initial);
    SRT(processes, t_cs, tau_initial, alpha);
    RR(processes, t_cs, t_slice);
    return 0;
}
