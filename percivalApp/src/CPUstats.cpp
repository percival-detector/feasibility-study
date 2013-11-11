/*
 * CPUstats.cpp
 *
 *  Created on: 8th Nov 2013
 *      Author: mjn19172
 */

#include "CPUstats.h"

#define SUCCESS 0
#define FAILURE -1

CPUstats::CPUstats()
{
    isOpen_ = false;
    statpath_ = "/proc/stat";
    stat_.open(statpath_.c_str(), std::ifstream::in);
    if (stat_.fail()) std::cerr << "CPUstats: error opening file stream for reading." << std::endl;
    else isOpen_ = true;
}

CPUstats::~CPUstats()
{
    if (isOpen_) stat_.close();
}

int CPUstats::getCpuStats(int cpu_no,
                unsigned int t_old,
                unsigned int i_old,
                unsigned int *t_new,
                unsigned int *i_new,
                int *usage)
{
    std::string cpu = "cpu";
    int pos = 0;
    int busy = 0;
    int length;
    *t_new = 0;
    *usage = -1;
    if (!isOpen_) return FAILURE;
    //TODO: implement, see procps/proc/sysinfo.c for inspiration...
    stat_.seekg(0, stat_.end);
    length = stat_.tellg();
    stat_.seekg(0, stat_.beg);
    char buf[length];
    stat_.read(buf, length);
    std::string content(buf);
    std::istringstream tokeniser(content);
    while ((pos < length) && tokeniser.good()) {
        pos = content.find(cpu, pos);
        if ((pos + 5) < length) {
            pos = pos + 3;
            if (content[pos] != ' ') {
                int CPU = atoi(content.substr(pos, 2).c_str());
                pos = pos + 2;
                if (CPU == cpu_no) {
                    if (content[pos] == ' ') pos++;
                    tokeniser.seekg(pos);
                    for (int i = 0; i < 3; i++) {
                        tokeniser >> *t_new;
                        busy = busy + *t_new;
                    }
                    tokeniser >> *i_new;
                    for (int i = 0; i < 3; i++) {
                        tokeniser >> *t_new;
                        busy = busy + *t_new;
                    }
                    *t_new = busy + *i_new;
                    if (*t_new != t_old)
                        *usage = 100.0 * (1.0 - (double)(*i_new - i_old) / (double)(*t_new - t_old));
                    return SUCCESS;
                }
            }
        }
    }
    return FAILURE;
}