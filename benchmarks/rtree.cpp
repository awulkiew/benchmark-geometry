#include "benchmark.hpp"

#include <vector>
#include <random>

#include <boost/geometry.hpp>
#include <boost/geometry/index/rtree.hpp>

using namespace boost::geometry;

template <typename Rtree, typename Input>
void benchmark_queries(std::string const& name, Rtree tree, Input input, size_t const N)
{
    typedef typename Rtree::value_type value_t;
    typedef typename Rtree::bounds_type box_t;
    typedef typename point_type<box_t>::type point_t;

    std::vector<value_t> res;
    size_t i = 0;
    benchmark::run(name + "-spatial", [&]() {
        res.clear();
        box_t qbox{ { get<0>(input[i].min_corner()) - 10, get<1>(input[i].min_corner()) + 10 },
        { get<0>(input[i].max_corner()) - 10, get<1>(input[i].max_corner()) + 10 } };
        tree.query(index::intersects(qbox), std::back_inserter(res));
        i = (i + 1) % N;
    });

    i = 0;
    benchmark::run(name + "-knn", [&]() {
        res.clear();
        point_t qpt{ get<0>(input[i].min_corner()) + 0.5f, get<1>(input[i].min_corner()) + 0.5f };
        tree.query(index::nearest(qpt, 3), std::back_inserter(res));
        i = (i + 1) % N;
    });
}

void benchmark_rtree()
{
    typedef float coord_t;
    typedef model::point<coord_t, 2, cs::cartesian> point_t;
    typedef model::box<point_t> box_t;
    
    size_t const N = 1000000;
    std::vector<box_t> input;
    input.resize(N);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<coord_t> dis1(-1000, 1000);
    for (size_t i = 0; i < N; ++i)
    {
        coord_t x = dis1(gen);
        coord_t y = dis1(gen);

        input[i] = box_t{ {x - 0.5f, y - 0.5f},{x + 0.5f, y + 0.5f} };
    }
    
    {
        typedef index::rtree<box_t, index::linear<16>> rtree_t;
        rtree_t tree;
        benchmark::run("rtree-pack-create", [&]() {
            rtree_t t(input.begin(), input.end());
            tree = std::move(t);
        });
                
        benchmark_queries("rtree-pack", tree, input, N);
    }

    {
        typedef index::rtree<box_t, index::linear<16>> rtree_t;
        rtree_t tree;

        benchmark::run("rtree-linear-create", [&]() {
            rtree_t t;
            t.insert(input.begin(), input.end());
            tree = std::move(t);
        });

        benchmark_queries("rtree-linear", tree, input, N);
    }

    {
        typedef index::rtree<box_t, index::quadratic<16>> rtree_t;
        rtree_t tree;

        benchmark::run("rtree-quadratic-create", [&]() {
            rtree_t t;
            t.insert(input.begin(), input.end());
            tree = std::move(t);
        });

        benchmark_queries("rtree-quadratic", tree, input, N);
    }

    {
        typedef index::rtree<box_t, index::rstar<16>> rtree_t;
        rtree_t tree;

        benchmark::run("rtree-rstar-create", [&]() {
            rtree_t t;
            t.insert(input.begin(), input.end());
            tree = std::move(t);
        });

        benchmark_queries("rtree-rstar", tree, input, N);
    }
}

int main()
{
    benchmark_rtree();
}