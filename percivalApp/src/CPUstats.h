/*
 * CPUstats.h
 *
 *  Created on: 8th Nov 2013
 *      Author: mjn19172
 */

#ifndef CPUSTATS_H_
#define CPUSTATS_H_

#include <cstdlib>
#include <fstream>
#include <sstream>
#include <iostream>

class CPUstats
{
    public:
        typedef struct trio { int busy;
                              int idle;
                              int total;
        } trio;

        int getCpuStats(int cpu_no, trio *previous, int *usage);

    private:
        static const unsigned int BUFFSIZE = 64*1024; //Stolen from procps/proc/sysinfo.h
};

#endif // CPUSTATS_H_
