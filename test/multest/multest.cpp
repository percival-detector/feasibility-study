#include <time.h>
#include <iostream>
#include "boost/date_time/posix_time/posix_time.hpp"
#include "boost/chrono.hpp"

int main()
{
    static const int iterations = 1000000;
    boost::posix_time::ptime now;
    long starttime, duration;
    volatile float a = 123.4;
    volatile float b = 234.5;
    volatile float c;
    volatile float d = 345.6;
    volatile float e = 456.7;
    volatile int f = 123;
    volatile int g = 234;
    float h[14] = { 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0,  1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0 };

    asm("nop");

    std::cout << "Testing single multiplication:" << std::endl;
    now = boost::posix_time::microsec_clock::local_time();
    starttime = now.time_of_day().total_microseconds();
    for (int i = 0; i < iterations; i++) {
        c = a * b;
    }
    now = boost::posix_time::microsec_clock::local_time();
    duration = now.time_of_day().total_microseconds() - starttime;
    std::cout << "\tCompleted " << iterations << " iterations in " << duration;
    std::cout << " usec or " << (duration / 1000) << " msec" << std::endl;

    std::cout << "Testing two multiplications:" << std::endl;
    now = boost::posix_time::microsec_clock::local_time();
    starttime = now.time_of_day().total_microseconds();
    for (int i = 0; i < iterations; i++) {
        c = a * b;
        c = a * b;
    }
    now = boost::posix_time::microsec_clock::local_time();
    duration = now.time_of_day().total_microseconds() - starttime;
    std::cout << "\tCompleted " << iterations << " iterations in " << duration;
    std::cout << " usec or " << (duration / 1000) << " msec" << std::endl;

    std::cout << "Testing three multiplications:" << std::endl;
    now = boost::posix_time::microsec_clock::local_time();
    starttime = now.time_of_day().total_microseconds();
    for (int i = 0; i < iterations; i++) {
        c = a * b;
        c = a * b;
        c = a * b;
    }
    now = boost::posix_time::microsec_clock::local_time();
    duration = now.time_of_day().total_microseconds() - starttime;
    std::cout << "\tCompleted " << iterations << " iterations in " << duration;
    std::cout << " usec or " << (duration / 1000) << " msec" << std::endl;

    std::cout << "Testing four multiplications:" << std::endl;
    now = boost::posix_time::microsec_clock::local_time();
    starttime = now.time_of_day().total_microseconds();
    for (int i = 0; i < iterations; i++) {
        c = a * b;
        c = a * b;
        c = a * b;
        c = a * b;
    }
    now = boost::posix_time::microsec_clock::local_time();
    duration = now.time_of_day().total_microseconds() - starttime;
    std::cout << "\tCompleted " << iterations << " iterations in " << duration;
    std::cout << " usec or " << (duration / 1000) << " msec" << std::endl;

    std::cout << "Testing five multiplications:" << std::endl;
    now = boost::posix_time::microsec_clock::local_time();
    starttime = now.time_of_day().total_microseconds();
    for (int i = 0; i < iterations; i++) {
        c = a * b;
        c = a * b;
        c = a * b;
        c = a * b;
        c = a * b;
    }
    now = boost::posix_time::microsec_clock::local_time();
    duration = now.time_of_day().total_microseconds() - starttime;
    std::cout << "\tCompleted " << iterations << " iterations in " << duration;
    std::cout << " usec or " << (duration / 1000) << " msec" << std::endl;

    std::cout << "Testing garbled mess" << std::endl;
    now = boost::posix_time::microsec_clock::local_time();
    starttime = now.time_of_day().total_microseconds();
    for (int i = 0; i < iterations; i++) {
        c = (f >> 2) & 0xde;
        c = (g >> 10) & 0xad;
        c = a * b + d;
        c = (f >> 2) & 0xde;
        c = (g >> 10) & 0xad;
        c = a * b + (d - e) * c;
        c = a * b + c;
    }
    now = boost::posix_time::microsec_clock::local_time();
    duration = now.time_of_day().total_microseconds() - starttime;
    std::cout << "\tCompleted " << iterations << " iterations in " << duration;
    std::cout << " usec or " << (duration / 1000) << " msec" << std::endl;

    std::cout << "Testing lookups" << std::endl;
    now = boost::posix_time::microsec_clock::local_time();
    starttime = now.time_of_day().total_microseconds();
    for (int i = 0; i < iterations; i++) {
        for (int j = 0; j < 14; j++) {
            c = h[j];
        }
    }
    now = boost::posix_time::microsec_clock::local_time();
    duration = now.time_of_day().total_microseconds() - starttime;
    std::cout << "\tCompleted " << iterations << " iterations in " << duration;
    std::cout << " usec or " << (duration / 1000) << " msec" << std::endl;

    return 0;
}