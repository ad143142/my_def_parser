#ifndef DEF_INFO_H
#define DEF_INFO_H

#include "rtree.h"
#include <string>
#include <unordered_map>
#include <utility>

class def_info_t {
private:
    int unit;
    std::string design_name;
    std::string tech_name;
    std::unordered_map<std::string, std::vector<mybox_t> > boxes;
    
public:
    def_info_t() {
    }
    ~def_info_t() {
    }

    void add_box(std::string &layer, point_t& p1, point_t& p2) {
        unsigned int idx = boxes[layer].size();
        boxes[layer].emplace_back(mybox_t(p1, p2, idx));
    }

    void print_info() {
        fprintf(stdout, "Design name: %s\n", design_name.c_str());
        fprintf(stdout, "Tech name: %s\n", tech_name.c_str());
        fprintf(stdout, "Unit: %d\n", unit);
        for (auto &[layer, box] : boxes) {
            fprintf(stdout, "Layer: %s %llu boxes\n", layer.c_str(), box.size());
        }
    }

    void set_unit(int u) {
        unit = u;
    }
    void set_design_name(const char *string) {
        design_name.assign(string);
    }
    void set_tech_name(const char *string) {
        tech_name.assign(string);
    }

    std::string get_design_name() {
        return design_name;
    }
    std::string get_tech_name() {
        return tech_name;
    }
    int get_unit() {
        return unit;
    }
    std::unordered_map<std::string, std::vector<mybox_t> > &get_boxes() {
        return boxes;
    }
};

#endif