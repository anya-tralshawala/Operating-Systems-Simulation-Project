#include "cpu.h"

cpu::cpu(process i){
    
    running_process = i;
    isrunning = false;
    current_time = 0;
}

void cpu::setProcess(process i){
    running_process = i;
}
void cpu::setTime(double time){
    current_time = time;
}
