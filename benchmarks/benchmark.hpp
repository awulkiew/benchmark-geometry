#ifndef BENCHMARK_HPP
#define BENCHMARK_HPP

#include <algorithm>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <string>
#include <fstream>

namespace benchmark {

std::ofstream & dump_ostream()
{
    static std::ofstream f;
    return f;
}

template <typename Clock, typename Fun, typename Dump>
double run(Fun fun, size_t count, Dump & dump)
{
    using result_t = decltype(fun());
    result_t result = result_t();

    auto start = Clock::now();
    for (size_t i = 0; i < count; ++i)
    {
        result += fun();
    }
    auto end = Clock::now();

    dump << result;

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
            seconds = run<std::chrono::high_resolution_clock>(fun, base_count, dump_ostream());

            if (seconds >= 0.01)
                break;
        }

        size_t count = (size_t)(std::min)(test_seconds * base_count / seconds, std::numeric_limits<size_t>::max() / 2.0);

        if (count >= 2)
        {
            seconds = run<std::chrono::high_resolution_clock>(fun, count, dump_ostream()) / count;
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

