#include "def_parser.h"
#include "r_tree.h"
#include "def_info.h"

int main(int argc, char **argv)
{
    def_info_t def_info;
    def_parser(argc, argv, def_info);

    return 0;
}
