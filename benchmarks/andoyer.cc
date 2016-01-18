#include "benchmark.hpp"

#include <cmath>
#include <vector>

#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/algorithms/detail/andoyer_inverse.hpp>

using namespace boost::geometry;

int main()
{
    typedef model::point<double, 2, cs::geographic<degree>> point_t;

    point_t p0 = { -45, -45 };
    point_t p1 = { 45, 45 };
    srs::spheroid<double> spheroid;

    extern double d1;
    extern double d2;
    
    benchmark::run("andoyer-formula", [&]() {      
        detail::andoyer_inverse<double, true, false> f;
        d1 = f.apply(get_as_radian<0>(p0), get_as_radian<1>(p0), get_as_radian<0>(p1), get_as_radian<1>(p1), spheroid).distance;
    });

    benchmark::run("andoyer-strategy", [&]() {
        strategy::distance::andoyer<srs::spheroid<double>> s(spheroid);
        d2 = s.apply(p0, p1);
    });

    return 0;
}

double d1;
double d2;