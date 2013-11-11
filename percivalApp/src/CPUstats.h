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
        CPUstats();
        virtual ~CPUstats();
        int getCpuStats(int cpu_no,
                        unsigned int t_old,
                        unsigned int i_old,
                        unsigned int *t_new,
                        unsigned int *i_new,
                        int *usage);

    private:
        std::string statpath_;
        std::ifstream stat_;
        bool isOpen_;
};

#endif // CPUSTATS_H_
