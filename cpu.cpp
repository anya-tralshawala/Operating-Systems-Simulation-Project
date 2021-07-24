#include "cpu.h"

cpu::cpu(process i){
    
    running_process = i;
    isrunning = false;
}

void cpu::setProcess(process i){
    running_process = i;
}
