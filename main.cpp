
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <iomanip>
#include <string>
#include "process.h"

//algo go here

//First-come-first-served (FCFS)

//Shortest job first (SJF)

//Shortest remaining time (SRT)

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
    std::vector<process> return_vector(n);
    char alphabet[26];
    for (int letter = 'a'; letter <= 'z'; letter++)
    {
        alphabet[letter - 'a'] = letter;
    }
    double arrival_time = floor(next_exp(lambda, upper_bound));
    for (int i = 0; i < n; i++)
    {
        // int aState, int aPid, double aTime,std::vector<cpuBurst> aBurstList)
        //number of processes is ceil(drand48() * 100)
        std::vector<cpuBurst> aBurstList(ceil(drand48() * 100));
        //populate burst list
        for (int j = 0; j < aBurstList.size(); j++)
        {
            double CPU_burst_time = ceil(next_exp(lambda, upper_bound));
            double IO_time = ceil(next_exp(lambda, upper_bound)) * 10;
            if (j == aBurstList.size() - 1)
            {
                IO_time = -99;
            }
            cpuBurst current_burst = cpuBurst(CPU_burst_time, IO_time);
            aBurstList.push_back(current_burst);
        }

        process current_process = process(0, alphabet[i], arrival_time, aBurstList);
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

    //create vector of processes
    srand48(seed);
    std::vector<process> processes = create_processes(n, seed, lambda, upper_bound);

    return 0;
}
