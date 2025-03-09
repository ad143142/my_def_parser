#include "def_info.h"
#include "def_parser.h"
#include "rtree.h"

int main(int argc, char **argv) {
    def_info_t def_info;
    fprintf(stdout, "Start parse def file\n");
    def_parser(argc, argv, def_info);
    fprintf(stdout, "End parse def file\n");
    def_info.print_info();

    fprintf(stdout, "Start build rtree\n");
    std::unordered_map<std::string, rtree_t> rtree_map;
    for (auto &[layer, box] : def_info.get_boxes()) {
        rtree_map[layer] = rtree_t(box.begin(), box.end());
    }
    fprintf(stdout, "End build rtree\n");

    float a_x = 17.1;
    float a_y = 112.608;
    float b_x = 17.172;
    float b_y = 114.048;
    point_t p1 = point_t(a_x*4000, a_y*4000);
    point_t p2 = point_t(b_x*4000, b_y*4000);
    mybox_t target(p1, p2, 1e9);
    std::string query_layer = "M3";

    auto [left_closest_box, right_closest_box] = rtree_map[query_layer].horizontal_nearest_box(target);
    auto [up_closest_box, down_closest_box] = rtree_map[query_layer].vertical_nearest_box(target);
    int left_idx = left_closest_box.first;
    int right_idx = right_closest_box.first;
    int up_idx = up_closest_box.first;
    int down_idx = down_closest_box.first;
    std::unordered_map<std::string, std::vector<mybox_t> > &boxes = def_info.get_boxes();
    mybox_t &left_box = boxes[query_layer][left_idx];
    mybox_t &right_box = boxes[query_layer][right_idx];
    mybox_t &up_box = boxes[query_layer][up_idx];
    mybox_t &down_box = boxes[query_layer][down_idx];
    
    int unit = def_info.get_unit();
    fprintf(stdout, "Left closest distance: %d\n", left_closest_box.second);
    fprintf(stdout, "Right closest distance: %d\n", right_closest_box.second);
    fprintf(stdout, "Up closest distance: %d\n", up_closest_box.second);
    fprintf(stdout, "Down closest distance: %d\n", down_closest_box.second);
    fprintf(stdout, "Left box: (%.3f, %.3f), (%.3f, %.3f)\n", bg::get<0, 0>(left_box.get_box())/(float)unit, bg::get<0, 1>(left_box.get_box())/(float)unit, bg::get<1, 0>(left_box.get_box())/(float)unit, bg::get<1, 1>(left_box.get_box())/(float)unit);
    fprintf(stdout, "Right box: (%.3f, %.3f), (%.3f, %.3f)\n", bg::get<0, 0>(right_box.get_box())/(float)unit, bg::get<0, 1>(right_box.get_box())/(float)unit, bg::get<1, 0>(right_box.get_box())/(float)unit, bg::get<1, 1>(right_box.get_box())/(float)unit);
    fprintf(stdout, "Up box: (%.3f, %.3f), (%.3f, %.3f)\n", bg::get<0, 0>(up_box.get_box())/(float)unit, bg::get<0, 1>(up_box.get_box())/(float)unit, bg::get<1, 0>(up_box.get_box())/(float)unit, bg::get<1, 1>(up_box.get_box())/(float)unit);
    fprintf(stdout, "Down box: (%.3f, %.3f), (%.3f, %.3f)\n", bg::get<0, 0>(down_box.get_box())/(float)unit, bg::get<0, 1>(down_box.get_box())/(float)unit, bg::get<1, 0>(down_box.get_box())/(float)unit, bg::get<1, 1>(down_box.get_box())/(float)unit);    
    
    mybox_t intersect_box = rtree_map[query_layer].find_union(target);
    fprintf(stdout, "Intersect box: (%.3f, %.3f), (%.3f, %.3f)\n", bg::get<0, 0>(intersect_box.get_box())/(float)unit, bg::get<0, 1>(intersect_box.get_box())/(float)unit, bg::get<1, 0>(intersect_box.get_box())/(float)unit, bg::get<1, 1>(intersect_box.get_box())/(float)unit);
    return 0;
}
