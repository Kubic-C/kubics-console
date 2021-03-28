// utility fucntions to help with implmentation side of things
#pragma once
#ifndef KUBICS_CONSOLE_UTILITY_H
#define KUBICS_CONSOLE_UTILITY_H

#include "base.hpp"

namespace kconsole
{
    /* halting_conditional_varible
     * does not notify if nothing is waiting
    */
    class halting_conditional_varible
    {
    public:
        halting_conditional_varible();

        // notify ALL waiting 
        bool notify();

        // wait for a notify
        void wait();

        bool waiting;

    private:
        std::condition_variable cv;
    };

    typedef std::chrono::steady_clock::time_point time_p;
    typedef std::chrono::duration<double> duration;

    /* timer
     * a simple timer that measures the time between two points
    */
    class timer
    {
    public:
        void start();

        void end();

        std::string show(int multiplier = 1);

    private:
        time_p start_v;
        time_p end_v;
    };
}

#endif // KUBICS_CONSOLE_UTILITY_H