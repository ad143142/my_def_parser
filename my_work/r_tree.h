#ifndef R_TREE_H
#define R_TREE_H
#include <iostream>

#include <boost/geometry.hpp>
#include <boost/geometry/geometries/box.hpp>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/index/rtree.hpp>

namespace bg = boost::geometry;
namespace bgi = boost::geometry::index;

typedef bg::model::point<int, 2, bg::cs::cartesian> point_t;
typedef bg::model::box<point_t> box_t;

#endif