#include "benchmark.hpp"

#include <cmath>
#include <vector>

#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/geometries/linestring.hpp>
#include <boost/geometry/geometries/polygon.hpp>

#include <boost/math/constants/constants.hpp>

using namespace boost::geometry;

int main()
{
    typedef model::point<double, 2, cs::cartesian> point_t;
    typedef model::linestring<point_t> linestring_t;
    typedef model::polygon<point_t> polygon_t;

    size_t const N = 100000;
    size_t const Z = 200;
    size_t const S = 100;

    double const pi = boost::math::constants::pi<double>();
    double const a_step = 2 * pi / (N * 1.001);
    
    polygon_t polygon, polygon_min, polygon_max, polygon_zero;
    linestring_t linestring, linestring_min, linestring_max, linestring_zero;
    linestring_t linestring_zigzag1, linestring_zigzag2;

    size_t i; double a;
    for (i = 0, a = 0; i < N; ++i, a += a_step)
    {
        double x = cos(a);
        double y = sin(a);
        double x_r = cos(a + a_step / 2);
        double y_r = sin(a + a_step / 2);

        // reference
        point_t pt = { S * x, S * -y };
        polygon.outer().push_back(pt);
        linestring.push_back(pt);
        // moved by x = S
        pt = { S + S * x, S * -y };
        polygon_min.outer().push_back(pt);
        linestring_min.push_back(pt);
        // rotated by a_step/2
        pt = { S * x_r, S * -y_r };
        polygon_max.outer().push_back(pt);
        linestring_max.push_back(pt);
        // moved by x = 3S
        pt = { 3 * S + S * x, S * -y };
        polygon_zero.outer().push_back(pt);
        linestring_zero.push_back(pt);
    }
    polygon.outer().push_back(polygon.outer()[0]);
    polygon_min.outer().push_back(polygon_min.outer()[0]);
    polygon_max.outer().push_back(polygon_max.outer()[0]);
    polygon_zero.outer().push_back(polygon_zero.outer()[0]);

    double l_step = (2.0 * S) / Z; // [-S, S]
    double l = 0;
    for (size_t i = 0; i < Z; ++i, l += l_step)
    {
        linestring_zigzag1.push_back({ -double(S) + l, -double(S) });
        linestring_zigzag1.push_back({ -double(S) + l, double(S) });
        linestring_zigzag2.push_back({ -double(S), -double(S) + l });
        linestring_zigzag2.push_back({ double(S), -double(S) + l });
    }

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
        });

        benchmark::run("get_turns-polygon-min", [&]() {
            turns.clear();

            detail::relate::turns::get_turns
                <
                    polygon_t,
                    polygon_t,
                    detail::get_turns::get_turn_info_type<polygon_t, polygon_t, detail::relate::turns::assign_policy<true> >
                >::apply(turns, polygon, polygon_min);
        });

        benchmark::run("get_turns-polygon-max", [&]() {
            turns.clear();

            detail::relate::turns::get_turns
                <
                    polygon_t,
                    polygon_t,
                    detail::get_turns::get_turn_info_type<polygon_t, polygon_t, detail::relate::turns::assign_policy<true> >
                >::apply(turns, polygon, polygon_max);
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
        });

        benchmark::run("get_turns-linestring-min", [&]() {
            turns.clear();

            detail::relate::turns::get_turns
                <
                    linestring_t,
                    linestring_t,
                    detail::get_turns::get_turn_info_type<linestring_t, linestring_t, detail::relate::turns::assign_policy<true> >
                >::apply(turns, linestring, linestring_min);
        });

        benchmark::run("get_turns-linestring-max", [&]() {
            turns.clear();

            detail::relate::turns::get_turns
                <
                    linestring_t,
                    linestring_t,
                    detail::get_turns::get_turn_info_type<linestring_t, linestring_t, detail::relate::turns::assign_policy<true> >
                >::apply(turns, linestring, linestring_max);
        });

        benchmark::run("get_turns-linestring-zigzag", [&]() {
            turns.clear();

            detail::relate::turns::get_turns
                <
                    linestring_t,
                    linestring_t,
                    detail::get_turns::get_turn_info_type<linestring_t, linestring_t, detail::relate::turns::assign_policy<true> >
                >::apply(turns, linestring_zigzag1, linestring_zigzag2);
        });
    }

    return 0;
}
