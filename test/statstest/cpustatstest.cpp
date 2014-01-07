#include <iostream>
#include "CPUstats.h"

int main()
{
    CPUstats stats;
    int usage = -2;
    int status = 1;

    status = stats.getCpuStats(0, &usage);
    std::cout << "cpustatstest: test for CPU 0 returned usage of " << usage << "% and status ";
    std::cout << ((status == 0) ? "SUCCESS" : "FAILURE") << "." << std::endl;

    status = stats.getCpuStats(1, &usage);
    std::cout << "cpustatstest: test for CPU 1 returned usage of " << usage << "% and status ";
    std::cout << ((status == 0) ? "SUCCESS" : "FAILURE") << "." << std::endl;

    status = stats.getCpuStats(2, &usage);
    std::cout << "cpustatstest: test for CPU 2 returned usage of " << usage << "% and status ";
    std::cout << ((status == 0) ? "SUCCESS" : "FAILURE") << "." << std::endl;

    status = stats.getCpuStats(3, &usage);
    std::cout << "cpustatstest: test for CPU 3 returned usage of " << usage << "% and status ";
    std::cout << ((status == 0) ? "SUCCESS" : "FAILURE") << "." << std::endl << std::endl;

    return 0;
}