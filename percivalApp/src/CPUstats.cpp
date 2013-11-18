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
    std::istringstream tokeniser(content);

    // Look for CPU entry
    while ((pos < content.length()) && tokeniser.good()) {
        pos = content.find("cpu", pos);

        // Check for potential match
        if ((pos + 5) < content.length()) {
            pos = pos + 3;

            // Check for CPU number as opposed to whitespace
            if (content[pos] != ' ') {
                int CPU = atoi(content.substr(pos, 2).c_str());
                pos = pos + 2;

                // Check for CPU number match
                if (CPU == cpu_no) {
                    if (content[pos] == ' ') pos++;
                    tokeniser.seekg(pos);

                    // Extract user, nice and system times
                    for (int i = 0; i < 3; i++) {
                        tokeniser >> total;
                        busy = busy + total;
                    }
                    // Extract idle time
                    tokeniser >> idle;
                    // Extract other times for total busy time
                    for (int i = 0; i < 3; i++) {
                        tokeniser >> total;
                        busy = busy + total;
                    }
                    total = busy + idle;

                    // Convert to percentage, save ticks
                    *usage = 100 * (busy - previous->busy) / (total - previous->total);
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
