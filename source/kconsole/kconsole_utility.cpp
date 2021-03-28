#include "headers/kconsole_utility.hpp"

namespace kconsole
{
    halting_conditional_varible::halting_conditional_varible()
        : waiting(false), cv()
    {

    }

    bool halting_conditional_varible::notify()
    {
        if (waiting)
        {
            cv.notify_all();
            return true;
        }
        else
            return false;
    }

    void halting_conditional_varible::wait()
    {
        std::mutex mtx;
        std::unique_lock<std::mutex> mtx_u(mtx);
        waiting = true;
        cv.wait(mtx_u);
    }
}

namespace kconsole
{
    void timer::start()
    {
        start_v = std::chrono::steady_clock::now();
    }

    void timer::end()
    {
        end_v = std::chrono::steady_clock::now();
    }

    std::string timer::show(int multiplier)
    {
        return std::to_string(std::chrono::duration_cast
            <std::chrono::microseconds>(end_v - start_v).count());
    }
}