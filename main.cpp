
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <iomanip>
#include <string>

int main(int argc, char * argv[])
{
    std::string  n = argv[1];
    int seed = std::stoi(argv[2]);
    double lambda = std::stod(argv[3]);
    double upper_bound = std::stod(argv[4]);
    double t_cs = std::stod(argv[5]);
    double alpha = std::stod(argv[6]);
    double t_slice = std::stod(argv[7]);

    return 0;
}
