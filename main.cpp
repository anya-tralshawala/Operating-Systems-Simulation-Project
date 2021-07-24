
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <iomanip>
#include <string>
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
//algo go here

//First-come-first-served (FCFS)
void FCFS(std::vector<process> processes, double t_cs)
{

    int total_processes = processes.size();
    int completed = 0;
    int time = 0;
    int context_switch = 0;
    std::vector<process> queue;
    std::vector<process> waitstate;
    std::vector<cpuBurst> emptylist;

    process i = process(0, 0, 0, emptylist, 0, 0, 0, 0);

    cpu aCPU = cpu(i);

    printf("time %dms: Simulator started for FCFS %s\n", time, printQueue(queue).c_str());

    /*start simulation, continue until all processes are completed*/
    while (completed != total_processes && time != 300)
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
        /*check is process is done running on CPU*/
        if (aCPU.checkstate() == true)
        {

            /*something is running on the CPU, check if it is finished*/
            process current = aCPU.getProcess();
            current.setBurst(current.getAllBursts()[0]);
            /*check if the CPU burst of the process is finished*/
            //printf("%f", current.getCurrent().get_CPUtime());
            if (current.getCurrent().get_CPUtime() == time)
            {

                aCPU.setState(false);
                int check_finished = current.getRemainingBursts();
                if (check_finished == 0)
                {

                    /*all CPU bursts completed, update number of completed processes*/
                    completed += 1;
                    aCPU.setProcess(i);

                    printf("time %dms: Process %c terminated %s\n", time, toupper(char(current.getpID())), printQueue(queue).c_str());
                    context_switch += 1;
                }
                else
                {
                    /*CPU burst finished, remove from burst list and switch waiting on IO*/
                    printf("time %dms: Process %c completed a CPU burst; %d bursts to go %s\n", time, toupper(char(current.getpID())), check_finished, printQueue(queue).c_str());

                    waitstate.push_back(current);
                    int waiting_time = time + current.getCurrent().get_IOtime() + t_cs;
                    current.removeBurst(current.getCurrent());
                    printf("time %dms: Process %c switching out of CPU; will block on I/O until time %dms %s\n", time, toupper(char(current.getpID())), waiting_time, printQueue(queue).c_str());
                }
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
                
                process nowrunning = queue.front();
                
                queue.erase(queue.begin());

                time += t_cs / 2;
                printf("time %dms: Process %c started using the CPU for %fms burst %s\n", time, toupper(char(nowrunning.getpID())), nowrunning.getAllBursts()[0].get_CPUtime(), printQueue(queue).c_str());
            }
        }

        /*increment time variable*/
        time += 1;
    }
}
//Shortest job first (SJF)

//Shortest remaining time (SRT)
    //use remaining time attribute (similar to FCFS in terms of structure and checks)

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
            if (j == aBurstList.size() - 1)
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

    double tau_initial = t_cs / 2;

    // create vector of processes
    srand48(seed);
    std::vector<process> processes = create_processes(n, seed, lambda, upper_bound);

    /*
    for(int i = 0; i < processes.size(); i++){
        std::cout<<"in the vector pID is: "<<processes[i].getpID()<<"\n";
        std::cout<<"in the vector pID is: "<<processes[i].getArrivialTime()<<"\n";
        std::cout<<"in the vector pID is: "<<processes[i].getState()<<"\n";


    }*/

    FCFS(processes, t_cs);

    return 0;
}
