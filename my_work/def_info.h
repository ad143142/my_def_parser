#ifndef DEF_INFO_H
#define DEF_INFO_H

#include <string>
#include <unordered_map>
#include "r_tree.h"

class def_info_t {
private:
    int unit;
    std::string design_name;
    std::string tech_name;
    std::unordered_map<std::string, std::vector<box_t>> boxes;
public:
    def_info_t(){

    }
    ~def_info_t(){

    }

    void add_box(std::string &layer, box_t &box){
        boxes[layer].push_back(box);
    }

    void set_unit(int u){
        unit = u;
    }
    void set_design_name( const char *string){
        design_name.assign(string);
    }
    void set_tech_name( const char *string){
        tech_name.assign(string);
    }
    std::string get_design_name(){
        return design_name;
    }
    std::string get_tech_name(){
        return tech_name;
    }
    int get_unit(){
        return unit;
    }

};

#endif