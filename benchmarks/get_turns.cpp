// Copyright (c) 2017 Adam Wulkiewicz, Lodz, Poland.

// Use, modification and distribution is subject to the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "benchmark.hpp"
#include "data.hpp"

#include <vector>

#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/geometries/linestring.hpp>
#include <boost/geometry/geometries/polygon.hpp>

using namespace boost::geometry;

int main()
{
    typedef model::point<double, 2, cs::cartesian> point_t;
    typedef model::linestring<point_t> linestring_t;
    typedef model::polygon<point_t> polygon_t;

    size_t const N = 100000;
    
    polygon_t polygon, polygon_min, polygon_max, polygon_zero;
    linestring_t linestring, linestring_min, linestring_max, linestring_zero;
    
    data::circles(N, polygon.outer(), polygon_min.outer(), polygon_max.outer(), polygon_zero.outer());
    data::circles(N, linestring, linestring_min, linestring_max, linestring_zero);

    linestring_t linestring_zigzag1, linestring_zigzag2;

    data::zigzags(N, linestring_zigzag1, linestring_zigzag2);

    
    {
        typedef detail::relate::turns::get_turns<polygon_t, polygon_t>::turn_info turn_type;    
        std::vector<turn_type> turns;

        benchmark::run("get_turns-polygon-zero", [&]() {
            turns.clear();

            detail::relate::turns::get_turns
                <
                    polygon_t,
                    polygon_t,
                    detail::get_turns::get_turn_info_type<polygon_t, polygon_t, detail::relate::turns::assign_policy<true> >
                >::apply(turns, polygon, polygon_zero);

            return turns.size();
        });

        benchmark::run("get_turns-polygon-min", [&]() {
            turns.clear();

            detail::relate::turns::get_turns
                <
                    polygon_t,
                    polygon_t,
                    detail::get_turns::get_turn_info_type<polygon_t, polygon_t, detail::relate::turns::assign_policy<true> >
                >::apply(turns, polygon, polygon_min);

            return turns.size();
        });

        benchmark::run("get_turns-polygon-max", [&]() {
            turns.clear();

            detail::relate::turns::get_turns
                <
                    polygon_t,
                    polygon_t,
                    detail::get_turns::get_turn_info_type<polygon_t, polygon_t, detail::relate::turns::assign_policy<true> >
                >::apply(turns, polygon, polygon_max);

            return turns.size();
        });
    }

    {
        typedef detail::relate::turns::get_turns<linestring_t, linestring_t>::turn_info turn_type;
        std::vector<turn_type> turns;

        benchmark::run("get_turns-linestring-zero", [&]() {
            turns.clear();

            detail::relate::turns::get_turns
                <
                    linestring_t,
                    linestring_t,
                    detail::get_turns::get_turn_info_type<linestring_t, linestring_t, detail::relate::turns::assign_policy<true> >
                >::apply(turns, linestring, linestring_zero);

            return turns.size();
        });

        benchmark::run("get_turns-linestring-min", [&]() {
            turns.clear();

            detail::relate::turns::get_turns
                <
                    linestring_t,
                    linestring_t,
                    detail::get_turns::get_turn_info_type<linestring_t, linestring_t, detail::relate::turns::assign_policy<true> >
                >::apply(turns, linestring, linestring_min);

            return turns.size();
        });

        benchmark::run("get_turns-linestring-max", [&]() {
            turns.clear();

            detail::relate::turns::get_turns
                <
                    linestring_t,
                    linestring_t,
                    detail::get_turns::get_turn_info_type<linestring_t, linestring_t, detail::relate::turns::assign_policy<true> >
                >::apply(turns, linestring, linestring_max);

            return turns.size();
        });

        benchmark::run("get_turns-linestring-zigzag", [&]() {
            turns.clear();

            detail::relate::turns::get_turns
                <
                    linestring_t,
                    linestring_t,
                    detail::get_turns::get_turn_info_type<linestring_t, linestring_t, detail::relate::turns::assign_policy<true> >
                >::apply(turns, linestring_zigzag1, linestring_zigzag2);

            return turns.size();
        });
    }

    return 0;
}
