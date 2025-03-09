#ifndef RTREE_H
#define RTREE_H
#include <iostream>

#include <boost/geometry.hpp>
#include <boost/geometry/geometries/box.hpp>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/index/rtree.hpp>

namespace bg = boost::geometry;
namespace bgi = boost::geometry::index;

typedef bg::model::point<int, 2, bg::cs::cartesian> point_t;

class mybox_t {
private:
    bg::model::box<point_t> box;
    unsigned int index;

public:
    mybox_t(): box(point_t(-1, -1), point_t(-1, -1)), index(-1) {
    }

    mybox_t(point_t &p1, point_t &p2, unsigned int idx): box(p1, p2), index(idx) {
    }

    const bg::model::box<point_t> &get_box() const {
        return box;
    }

    unsigned int get_index() const {
        return index;
    }
};

namespace boost {
namespace geometry {
    namespace index {
        template <> struct indexable<mybox_t> {
            typedef bg::model::box<point_t> result_type;
            result_type const &operator()(const mybox_t &v) const {
                return v.get_box();
            }
        };
    }
}
} // namespace boost::geometry::index

// typedef bgi::rtree<std::pair<box_t, unsigned int>, bgi::rstar<16> > rtree_t;
class rtree_t : public bgi::rtree<mybox_t, bgi::rstar<16> > {
private:
    int distance(mybox_t &p1, mybox_t &p2) {
        return bg::distance(p1.get_box(), p2.get_box());
    }

public:
    rtree_t(): bgi::rtree<mybox_t, bgi::rstar<16> >() {
    }
    rtree_t(std::vector<mybox_t>::iterator first, std::vector<mybox_t>::iterator last)
        : bgi::rtree<mybox_t, bgi::rstar<16> >(first, last) {
    }
    std::pair<std::pair<unsigned int, unsigned int>, std::pair<unsigned int, unsigned int> > horizontal_nearest_box(
        mybox_t &target) {
        std::vector<mybox_t> candidates;
        this->query(bgi::satisfies([&](const mybox_t &b) {
            const bg::model::box<point_t> &b_box = b.get_box(); // 取得 Box
            const bg::model::box<point_t> &target_box = target.get_box(); // 從 target 取得 Box

            return !(bg::get<1, 1>(b_box) < bg::get<0, 1>(target_box) || // y 範圍檢查：b 完全在 target 下方
                bg::get<0, 1>(b_box) > bg::get<1, 1>(target_box)); // b 完全在 target 上方
        }),
            std::back_inserter(candidates));
        // bg::get<1, 0>(box) < bg::get<0, 0>(target_box) && // 線段在 target 左側

        std::pair<unsigned int, unsigned int> right_closest_box = std::make_pair(-1, 1e9);
        std::pair<unsigned int, unsigned int> left_closest_box = std::make_pair(-1, 1e9);
        for (auto &c : candidates) {
            int dis = distance(c, target);
            // c 在 target 左側
            if (bg::get<1, 0>(c.get_box()) < bg::get<0, 0>(target.get_box())) {
                if (dis != 0 && dis < left_closest_box.second) {
                    left_closest_box = std::make_pair(c.get_index(), dis);
                }
            }
            // c 在 target 右側
            else if(bg::get<0, 0>(c.get_box()) > bg::get<1, 0>(target.get_box())) {
                if (dis != 0 && dis < right_closest_box.second) {
                    right_closest_box = std::make_pair(c.get_index(), dis);
                }
            }
        }
        return std::make_pair(left_closest_box, right_closest_box);
    }
    std::pair<std::pair<unsigned int, unsigned int>, std::pair<unsigned int, unsigned int> > vertical_nearest_box(
        mybox_t &target) {
        std::vector<mybox_t> candidates;
        this->query(bgi::satisfies([&](const mybox_t &b) {
            const bg::model::box<point_t> &b_box = b.get_box(); // 取得 Box
            const bg::model::box<point_t> &target_box = target.get_box(); // 從 target 取得 Box

            return !(bg::get<1, 0>(b_box) < bg::get<0, 0>(target_box) || // x 範圍檢查：b 完全在 target 左方
                bg::get<0, 0>(b_box) > bg::get<1, 0>(target_box)); // b 完全在 target 右方
        }),
            std::back_inserter(candidates));
        // bg::get<1, 1>(box) < bg::get<0, 1>(target_box) || // y 範圍檢查：b 完全在 target 下方
        // bg::get<0, 1>(box) > bg::get<1, 1>(target_box)); // b 完全在 target 上方

        std::pair<unsigned int, unsigned int> up_closest_box = std::make_pair(-1, 1e9);
        std::pair<unsigned int, unsigned int> down_closest_box = std::make_pair(-1, 1e9);
        for (auto &c : candidates) {
            int dis = distance(c, target);
            // c 在 target 上方
            if (bg::get<1, 1>(c.get_box()) > bg::get<0, 1>(target.get_box())) {
                if (dis != 0 && dis < up_closest_box.second) {
                    up_closest_box = std::make_pair(c.get_index(), dis);
                }
            }
            // c 在 target 下方
            else if(bg::get<0, 1>(c.get_box()) < bg::get<1, 1>(target.get_box())) {
                if (dis != 0 && dis < down_closest_box.second) {
                    down_closest_box = std::make_pair(c.get_index(), dis);
                }
            }
        }
        return std::make_pair(up_closest_box, down_closest_box);
    }
    
    mybox_t find_union(mybox_t &target) {
        // 初始化 union_box 為 target 自身的 box
        bg::model::box<point_t> union_box = target.get_box();

        std::queue<mybox_t> q;
        std::set<unsigned int> visited;
        q.push(target);
        visited.insert(target.get_index());

        while (!q.empty()) {
            mybox_t current = q.front();
            q.pop();

            // 查詢與 current 交疊的所有 box
            std::vector<mybox_t> results;
            this->query(bgi::intersects(current.get_box()), std::back_inserter(results));
            for (const auto &candidate : results) {
                if (visited.find(candidate.get_index()) == visited.end()) {
                    // 擴展 union_box 使其包含 candidate 的 box
                    bg::expand(union_box, candidate.get_box());
                    q.push(candidate);
                    visited.insert(candidate.get_index());
                }
            }
        }
        // 以 union_box 的 min 與 max 角點建立一個新的 mybox_t
        point_t min_pt = union_box.min_corner();
        point_t max_pt = union_box.max_corner();
        mybox_t result(min_pt, max_pt, target.get_index()); // index 可依需求設定
        return result;
    }
};

#endif