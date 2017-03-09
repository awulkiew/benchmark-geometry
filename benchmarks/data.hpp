// Copyright (c) 2017 Adam Wulkiewicz, Lodz, Poland.

// Use, modification and distribution is subject to the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef DATA_HPP
#define DATA_HPP

#include <cmath>

#include <boost/math/constants/constants.hpp>

namespace data {

template <typename Cont>
inline void circles(size_t N, Cont & circle,
                              Cont & circle_min,
                              Cont & circle_max,
                              Cont & circle_zero)
{
    if (N < 3)
        N = 3;

    double const S = 100;

    double const pi = boost::math::constants::pi<double>();
    double const a_step = 2 * pi / (N * 1.001);

    size_t i; double a;
    for (i = 0, a = 0; i < N; ++i, a += a_step)
    {
        double x = cos(a);
        double y = sin(a);
        double x_r = cos(a + a_step / 2);
        double y_r = sin(a + a_step / 2);

        // reference
        circle.push_back({ S * x, S * -y });
        // moved by x = S, min overlap
        circle_min.push_back({ S + S * x, S * -y });
        // rotated by a_step/2, max overlap
        circle_max.push_back({ S * x_r, S * -y_r });
        // moved by x = 3S, zero overlap
        circle_zero.push_back({ 3 * S + S * x, S * -y });
    }

    circle.push_back(circle[0]);
    circle_min.push_back(circle_min[0]);
    circle_max.push_back(circle_max[0]);
    circle_zero.push_back(circle_zero[0]);
}

template <typename Cont>
inline void zigzags(size_t N, Cont & zigzag1,
                              Cont & zigzag2)
{
    if (N < 3)
        N = 3;

    double const S = 100;
    double const Z = 200;

    double l_step = (2.0 * S) / Z; // [-S, S]
    double l = 0;
    for (size_t i = 0; i < Z; ++i, l += l_step)
    {
        zigzag1.push_back({ -S + l, -S });
        zigzag1.push_back({ -S + l, S });
        zigzag2.push_back({ -S, -S + l });
        zigzag2.push_back({ S, -S + l });
    }
}

} // namespace data

#endif // DATA_HPP
