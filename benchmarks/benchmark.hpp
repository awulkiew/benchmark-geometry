#ifndef BENCHMARK_HPP
#define BENCHMARK_HPP

#include <algorithm>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <string>

namespace benchmark {

template <typename Clock, typename Fun>
double run(Fun fun, size_t count)
{
    auto start = Clock::now();
    for (size_t i = 0; i < count; ++i)
    {
        fun();
    }
    auto end = Clock::now();

    std::chrono::duration<double> elapsed = end - start;
    return elapsed.count();
}

template <typename Fun>
void run(std::string const& name, Fun fun, std::ostream & out = std::cout)
{
    try
    {
        double const test_seconds = 1.0;

        size_t base_count = 1;
        double seconds = 0;
        for (; base_count < 1000000; base_count *= 10)
        {
            seconds = run<std::chrono::high_resolution_clock>(fun, base_count);

            if (seconds >= 0.01)
                break;
        }

        size_t count = (size_t)(std::min)(test_seconds * base_count / seconds, std::numeric_limits<size_t>::max() / 2.0);

        if (count >= 2)
        {
            seconds = run<std::chrono::high_resolution_clock>(fun, count) / count;
        }

        out << name << " " << std::fixed << std::setprecision(12) << seconds << std::endl;
    }
    catch (...)
    {
        // ignore
    }
}

} // namespace benchmark

#endif // BENCHMARK_HPP

