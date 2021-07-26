
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <iomanip>
#include <string>
#include <algorithm>
#include <bits/stdc++.h>
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
    double size = processes.size();
    double avg = 0;
    for (int i = 0; i < size; i++)
    {
        //loop through all processes
        std::vector<cpuBurst> current_bursts = processes[i].getAllBursts();
        double burst_sum = 0;
        double burst_avg = 0;
        for (int j = 0; j < current_bursts.size(); j++)
        {
            //loop through all bursts
            burst_sum += current_bursts[j].get_CPUtime();
        }
        burst_avg = burst_sum / current_bursts.size();
        sum += burst_avg;
    }
    avg = sum / size;
    return avg;
}

double avg_wait_time(std::vector<process> completed_processes)
{
    double sum = 0;
    double size = completed_processes.size();
    for (int i = 0; i < size; i++)
    {
        sum += completed_processes[i].getWait();
    }
    return sum / size;
}

//algo go here

//First-come-first-served (FCFS)
void FCFS(std::vector<process> processes, double t_cs)
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
    process i = process(0, 0, 0, emptylist, 0, 0, 0, 0);

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
            for (int i = 0; i < waitstate.size(); i++)
            {

                process curr = waitstate[i];

                curr.setBurst(aCPU.get_prev());
                //printf("curr: %f\n", curr.getWaitTime());
                //printf("time: %d\n", time);
                if(int(curr.getWaitTime()) == time){
                    curr.setWaitTime(-99);
                    //process completed IO burst, add back to the ready queue
                    queue.push_back(curr);
                    std::vector<process>::iterator itr2 = waitstate.begin() + i;
                    waitstate.erase(itr2);
                    printf("time %dms: Process %c completed I/O; added to ready queue %s\n", time, toupper(char(curr.getpID())), printQueue(queue).c_str());
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

                    printf("time %dms: Process %c terminated %s\n", time, toupper(char(current.getpID())), printQueue(queue).c_str());

                    if (completed == total_processes)
                    {
                        //print stats: CHANGE TO PRINT TO FILE
                        time += t_cs / 2;

                        context_switch += 1;
                        double avg_wait = avg_wait_time(completed_processes);
                        double avg_turnaround = avg_CPU_burst_time + avg_wait + t_cs;
                        double CPU_active = (aCPU.getActive() / time) * 100;
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
                    current.removeBurst(current.getCurrent());
                    waitstate.back().setWaitTime(waiting_time);
                    
                    //aCPU.set_waiting(waiting_time);
                    
                    printf("time %dms: Process %c switching out of CPU; will block on I/O until time %dms %s\n", time, toupper(char(current.getpID())), int(waiting_time), printQueue(queue).c_str());
                }
                context_switch += 1;
            }
        }
        //printf("%s\n", printQueue(queue).c_str());
        /*check if ready queue is not empty, and if CPU is available*/
        if (queue.size() != 0)
        {
            
            if (aCPU.checkstate() == false)
            {
                /*start running process*/

                /*set CPU state and update process running on CPU*/
                aCPU.setState(true);

                aCPU.setProcess(queue.front());
                
                aCPU.setTime(time + t_cs / 2);
                process nowrunning = queue.front();

                queue.erase(queue.begin());

                aCPU.updateActive(nowrunning.getAllBursts()[0].get_CPUtime());
                printf("time %dms: Process %c started using the CPU for %dms burst %s\n", int(time + t_cs / 2), toupper(char(nowrunning.getpID())), int(nowrunning.getAllBursts()[0].get_CPUtime()), printQueue(queue).c_str());
            }

            //update wait times, CPU is not available processes waiting
            for (int k = 0; k < queue.size(); k++)
            {
                queue[k].updateWaitTime();
            }
        }

        /*increment time variable*/
        time += 1;
    }
    printf("time %dms: Simulator ended for FCFS %s\n", int(time), printQueue(queue).c_str());
}
//Shortest job first (SJF)

//Shortest remaining time (SRT)
void SRT(std::vector<process> processes, double t_cs)
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
    int remaining_time[total_processes];
    int shortest = 0;
    int min_time = INT_MAX;
    bool check = false;
    int finish_time;
    process i = process(0, 0, 0, emptylist, 0, 0, 0, 0);

    cpu aCPU = cpu(i);

    printf("time %dms: Simulator started for SRT %s\n", time, printQueue(queue).c_str());

    /*start simulation, continue until all processes are completed*/
    while (completed != total_processes)
    {
        /*check if waitIO queue has anything that finished*/
        if (waitstate.size() != 0)
        {

            //loop through, check if any IO has finished
            for (int i = 0; i < waitstate.size(); i++)
            {

                process curr = waitstate[i];

                curr.setBurst(aCPU.get_prev());

                // if wait time is less than remaining time then add to queue
                if (aCPU.get_waiting() < time)
                {
                    //process completed IO burst, add front of the ready queue
                    queue.insert(queue.begin(), curr);
                    // preemptions ++;
                    std::vector<process>::iterator itr2 = waitstate.begin() + i;
                    waitstate.erase(itr2);
                    printf("time %dms: Process %c completed I/O; added to ready queue %s\n", time, toupper(char(curr.getpID())), printQueue(queue).c_str());
                }
                // if (aCPU.get_waiting() > time)
                // {
                //     //process completed IO burst, add back to the ready queue
                //     queue.push_back(curr);
                //     std::vector<process>::iterator itr3 = waitstate.begin() + i;
                //     waitstate.erase(itr3);
                //     printf("time %dms: Process %c completed I/O; added to ready queue %s\n", time, toupper(char(curr.getpID())), printQueue(queue).c_str());
                // }
            }
        }
        /*check if processes arrive*/
        for (int i = 0; i < total_processes; i++)
        {
            /*Find process with minimum remaining time in the queue*/
            if (processes[i].getRemainingTime() < min_time)
            {
                min_time = processes[i].getRemainingTime();
                shortest = i;
                check = true;

                // insert in the front of queue
                queue.insert(queue.begin(), processes[i]);
                printf("time %dms: Process %c arrived; added to ready queue %s\n", time, toupper(char(processes[i].getpID())), printQueue(queue).c_str());
            }
            // decrease remaining time
            min_time--;
        }

        if (aCPU.checkstate() == true)
        {

            /*something is running on the CPU, check if it is finished*/
            process current = aCPU.getProcess();
            current.setBurst(current.getAllBursts()[0]);

            /*check if the CPU burst of the process is finished*/
            //ISSUE: time needs to be compared to CPUtime + original time it started
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

                    printf("time %dms: Process %c terminated %s\n", time, toupper(char(current.getpID())), printQueue(queue).c_str());

                    if (completed == total_processes)
                    {
                        //print stats: CHANGE TO PRINT TO FILE
                        time += t_cs / 2;

                        context_switch += 1;
                        double avg_wait = avg_wait_time(completed_processes);
                        double avg_turnaround = avg_CPU_burst_time + avg_wait + t_cs;
                        double CPU_active = (aCPU.getActive() / time) * 100;
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
                    printf("time %dms: Process %c completed a CPU burst; %d bursts to go %s\n", time, toupper(char(current.getpID())), check_finished, printQueue(queue).c_str());

                    waitstate.push_back(current);
                    double switch_time = t_cs / 2;
                    double waiting_time = time + current.getCurrent().get_IOtime() + switch_time;
                    
                    aCPU.set_prev(current.getCurrent());
                    current.removeBurst(current.getCurrent());
                    aCPU.set_waiting(waiting_time);

                    printf("time %dms: Process %c switching out of CPU; will block on I/O until time %dms %s\n", time, toupper(char(current.getpID())), int(waiting_time), printQueue(queue).c_str());
                }
                context_switch += 1;
            }
        }

        /*check if ready queue is not empty, and if CPU is available*/
        if (queue.size() != 0)
        {

            if (aCPU.checkstate() == false)
            {
                /*start running process*/

                /*set CPU state and update process running on CPU*/
                aCPU.setState(true);

                aCPU.setProcess(queue.front());
                aCPU.setTime(time + t_cs / 2);
                process nowrunning = queue.front();

                queue.erase(queue.begin());

                aCPU.updateActive(nowrunning.getAllBursts()[0].get_CPUtime());
                printf("time %dms: Process %c started using the CPU for %dms burst %s\n", int(time + t_cs / 2), toupper(char(nowrunning.getpID())), int(nowrunning.getAllBursts()[0].get_CPUtime()), printQueue(queue).c_str());
            }

            //update wait times, CPU is not available processes waiting
            for (int k = 0; k < queue.size(); k++)
            {
                queue[k].updateWaitTime();
            }
        }
        /*increment time variable*/
        time += 1;
    }
    printf("time %dms: Simulator ended for SRT %s\n", int(time), printQueue(queue).c_str());
}
//Round robin (RR)

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

std::vector<process> create_processes(int n, int seed, double lambda, double upper_bound)
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

        process current_process = process(0, alphabet[i], arrival_time, aBurstList, num_bursts, 0, 0, 0);
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

    double tau_initial = 1/lambda;

    // create vector of processes
    srand48(seed);
    std::vector<process> processes = create_processes(n, seed, lambda, upper_bound);

    for(int i = 0; i<processes.size(); i++){
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

    FCFS(processes, t_cs);
    SRT(processes, t_cs);

    return 0;
}
