/*
 * CPUstats.cpp
 *
 *  Created on: 8th Nov 2013
 *      Author: mjn19172
 */

#include "CPUstats.h"

#define SUCCESS 0
#define FAILURE -1

int CPUstats::getCpuStats(int cpu_no, trio *previous, int *usage)
{
    int pos = 0;
    int busy = 0;
    int idle = 0;
    int total = 0;

    // Init file stream
    *usage = -1;
    std::ifstream stat("/proc/stat");
    if (stat.fail()) return FAILURE;

    // See procps/proc/sysinfo.c for inspiration (or comparison)...
    char buf[BUFFSIZE];
    buf[BUFFSIZE-1] = 0;
    stat.read(buf, BUFFSIZE-1);

    // Convenient Containers(TM)
    std::string content(buf);
    std::cout << "CONTENT:" << std::endl << content << std::endl;
    std::istringstream tokeniser(content);

    // Look for CPU entry
    while ((pos < content.length()) && tokeniser.good()) {
        pos = content.find("cpu", pos);
        std::cout << "pos = " << pos << std::endl;

        // Check for potential match
        if ((pos + 5) < content.length()) {
            pos = pos + 3;

            // Check for CPU number as opposed to whitespace
            if (content[pos] != ' ') {
                int CPU = atoi(content.substr(pos, 2).c_str());
                pos = pos + 2;

                // Check for CPU number match
                if (CPU == cpu_no) {
                    std::cout << "CPU = " << CPU << ", cpu_no = " << cpu_no << std::endl;
                    if (content[pos] == ' ') pos++;
                    tokeniser.seekg(pos);

                    // Extract user, nice and system times
                    for (int i = 0; i < 3; i++) {
                        tokeniser >> total;
                        busy = busy + total;
                        std::cout << "busy[" << i << "] = " << total << std::endl;
                    }
                    // Extract idle time
                    tokeniser >> idle;
                    // Extract other times for total busy time
                    for (int i = 0; i < 3; i++) {
                        tokeniser >> total;
                        busy = busy + total;
                        std::cout << "busy[" << (i+4) << "] = " << total << std::endl;
                    }
                    total = busy + idle;
                    std::cout << "old_busy = " << previous->busy << std::endl;
                    std::cout << "busy = " << busy << std::endl;
                    std::cout << "old_idle = " << previous->idle << std::endl;
                    std::cout << "idle = " << idle << std::endl;
                    std::cout << "old_total = " << previous->total << std::endl;
                    std::cout << "total = " << total << std::endl;

                    // Convert to percentage, save ticks
                    //*usage = 100.0 * (1.0 - (double)(idle - old_idle) / (double)(total - old_total));
                    std::cout << "(busy-old_busy) = " << (busy-previous->busy) << std::endl;
                    std::cout << "(total-old_total) = " << (total-previous->total) << std::endl;
                    std::cout << "(busy-old_busy)/(total-old_total) = ";
                    std::cout << ((busy-previous->busy)/(total-previous->total)) << std::endl;
                    *usage = 100 * (busy - previous->busy) / (total - previous->total);
                    std::cout << "usage = " << *usage << std::endl;
                    previous->busy = busy;
                    previous->idle = idle;
                    previous->total = total;

                    return SUCCESS;
                }
            }
        }
    }

    // Entry not found
    return FAILURE;
}