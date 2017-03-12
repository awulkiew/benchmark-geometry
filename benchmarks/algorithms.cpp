// Copyright (c) 2017 Adam Wulkiewicz, Lodz, Poland.

// Use, modification and distribution is subject to the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "benchmark.hpp"
#include "data.hpp"

#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/geometries/polygon.hpp>

using namespace boost::geometry;

int main()
{
    typedef model::point<double, 2, cs::cartesian> point_t;
    typedef model::linestring<point_t> linestring_t;
    typedef model::polygon<point_t> polygon_t;

    size_t const N = 1000000;
    
    polygon_t polygon, polygon_min, polygon_max, polygon_zero;
    linestring_t linestring, linestring_min, linestring_max, linestring_zero;

    data::circles(N, polygon.outer(), polygon_min.outer(), polygon_max.outer(), polygon_zero.outer());
    data::circles(N, linestring, linestring_min, linestring_max, linestring_zero);

    linestring_t linestring_zigzag1, linestring_zigzag2;

    data::zigzags(N, linestring_zigzag1, linestring_zigzag2);

    // relate
    benchmark::run("relate-disjoint-polygon-zero", [&]() {
        return relate(polygon, polygon_zero, de9im::mask("FF*FF****"));
    });
    benchmark::run("relate-disjoint-polygon-min", [&]() {
        return relate(polygon, polygon_min, de9im::mask("FF*FF****"));
    });
    benchmark::run("relate-disjoint-polygon-max", [&]() {
        return relate(polygon, polygon_max, de9im::mask("FF*FF****"));
    });
    benchmark::run("relate-disjoint-linestring-zero", [&]() {
        return relate(linestring, linestring_zero, de9im::mask("FF*FF****"));
    });
    benchmark::run("relate-disjoint-linestring-min", [&]() {
        return relate(linestring, linestring_min, de9im::mask("FF*FF****"));
    });
    benchmark::run("relate-disjoint-linestring-max", [&]() {
        return relate(linestring, linestring_max, de9im::mask("FF*FF****"));
    });
    benchmark::run("relate-disjoint-linestring-zigzag", [&]() {
        return relate(linestring_zigzag1, linestring_zigzag2, de9im::mask("FF*FF****"));
    });

    // relation
    benchmark::run("relation-polygon-zero", [&]() {
        return relation(polygon, polygon_zero)[0];
    });
    benchmark::run("relation-polygon-min", [&]() {
        return relation(polygon, polygon_min)[0];
    });
    benchmark::run("relation-polygon-max", [&]() {
        return relation(polygon, polygon_max)[0];
    });
    benchmark::run("relation-linestring-zero", [&]() {
        return relation(linestring, linestring_zero)[0];
    });
    benchmark::run("relation-linestring-min", [&]() {
        return relation(linestring, linestring_min)[0];
    });
    benchmark::run("relation-linestring-max", [&]() {
        return relation(linestring, linestring_max)[0];
    });
    benchmark::run("relation-linestring-zigzag", [&]() {
        return relation(linestring_zigzag1, linestring_zigzag2)[0];
    });

    // intersects
    benchmark::run("intersects-polygon-zero", [&]() {
        return intersects(polygon, polygon_zero);
    });
    benchmark::run("intersects-polygon-min", [&]() {
        return intersects(polygon, polygon_min);
    });
    benchmark::run("intersects-polygon-max", [&]() {
        return intersects(polygon, polygon_max);
    });
    benchmark::run("intersects-linestring-zero", [&]() {
        return intersects(linestring, linestring_zero);
    });
    benchmark::run("intersects-linestring-min", [&]() {
        return intersects(linestring, linestring_min);
    });
    benchmark::run("intersects-linestring-max", [&]() {
        return intersects(linestring, linestring_max);
    });
    benchmark::run("intersects-linestring-zigzag", [&]() {
        return intersects(linestring_zigzag1, linestring_zigzag2);
    });

    // touches
    benchmark::run("touches-polygon-zero", [&]() {
        return touches(polygon, polygon_zero);
    });
    benchmark::run("touches-polygon-min", [&]() {
        return touches(polygon, polygon_min);
    });
    benchmark::run("touches-polygon-max", [&]() {
        return touches(polygon, polygon_max);
    });

    // self-touches
    benchmark::run("touches-polygon", [&]() {
        return touches(polygon);
    });

    // self-intersects
    benchmark::run("intersects-polygon", [&]() {
        return intersects(polygon);
    });

    // is_valid
    benchmark::run("is_valid-polygon", [&]() {
        return is_valid(polygon);
    });
    
    
    return 0;
}
