#include "def_parser.h"

// Global variables
FILE *fout;
void *userData;
int line_num_print_interval = 1000;
def_info_t *def_info_ptr;
int numObjs;

void lineNumberCB(long long lineNo) {
    fprintf(stdout, "Parsed %lld number of lines!!\n", lineNo);
}

int unUsedCB(defrCallbackType_e c, void *any, defiUserData ud) {
    fprintf(stdout, "This callback is not used.\n");
    return 0;
}

void printWarning(const char *str) {
    fprintf(stderr, "%s\n", str);
}

void checkType(defrCallbackType_e c) {
    if (c >= 0 && c <= defrDesignEndCbkType) {
        // OK
    }
    else {
        fprintf(stdout, "ERROR: callback type is out of bounds!\n");
    }
}

void dataError() {
    fprintf(stdout, "ERROR: returned user data is not correct!\n");
}

void *mallocCB(size_t size) {
    return malloc(size);
}

void *reallocCB(void *name, size_t size) {
    return realloc(name, size);
}

void freeCB(void *name) {
    free(name);
    return;
}

char *address(const char *in) {
    return ((char *)in);
}

int tname(defrCallbackType_e c, const char *string, defiUserData ud) {
    checkType(c);
    if (ud != userData)
        dataError();
    def_info_ptr->set_tech_name(string);
    return 0;
}

int dname(defrCallbackType_e c, const char *string, defiUserData ud) {
    checkType(c);
    if (ud != userData)
        dataError();
    def_info_ptr->set_design_name(string);
    return 0;
}

int units(defrCallbackType_e c, double d, defiUserData ud) {
    checkType(c);
    if (ud != userData)
        dataError();
    def_info_ptr->set_unit(d);
    return 0;
}

int cs(defrCallbackType_e c, int num, defiUserData ud) {
    char *name;

    checkType(c);

    if (ud != userData)
        dataError();

    switch (c) {
        case defrComponentStartCbkType:
            name = address("COMPONENTS");
            break;
        case defrNetStartCbkType:
            name = address("NETS");
            break;
        case defrStartPinsCbkType:
            name = address("PINS");
            break;
        case defrViaStartCbkType:
            name = address("VIAS");
            break;
        case defrRegionStartCbkType:
            name = address("REGIONS");
            break;
        case defrSNetStartCbkType:
            name = address("SPECIALNETS");
            break;
        case defrGroupsStartCbkType:
            name = address("GROUPS");
            break;
        case defrScanchainsStartCbkType:
            name = address("SCANCHAINS");
            break;
        case defrIOTimingsStartCbkType:
            name = address("IOTIMINGS");
            break;
        case defrFPCStartCbkType:
            name = address("FLOORPLANCONSTRAINTS");
            break;
        case defrTimingDisablesStartCbkType:
            name = address("TIMING DISABLES");
            break;
        case defrPartitionsStartCbkType:
            name = address("PARTITIONS");
            break;
        case defrPinPropStartCbkType:
            name = address("PINPROPERTIES");
            break;
        case defrBlockageStartCbkType:
            name = address("BLOCKAGES");
            break;
        case defrSlotStartCbkType:
            name = address("SLOTS");
            break;
        case defrFillStartCbkType:
            name = address("FILLS");
            break;
        case defrNonDefaultStartCbkType:
            name = address("NONDEFAULTRULES");
            break;
        case defrStylesStartCbkType:
            name = address("STYLES");
            break;
        default:
            name = address("BOGUS");
            return 1;
    }
    fprintf(stdout, "\nhave %d %s \n", num, name);
    numObjs = num;
    return 0;
}

int netf(defrCallbackType_e c, defiNet *net, defiUserData ud) {
    // For net and special net.
    int i, j, k, w, x, y, z, count, newLayer;
    defiPath *p;
    defiSubnet *s;
    int path;
    defiVpin *vpin;
    defiWire *wire;

    checkType(c);
    if (ud != userData)
        dataError();

    // regularWiring
    if (net->numWires()) {
        for (i = 0; i < net->numWires(); i++) {
            newLayer = 0;
            wire = net->wire(i);
            count = 0;
            for (j = 0; j < wire->numPaths(); j++) {
                p = wire->path(j);
                p->initTraverse();
                int num_points = 0;
                bool connect_via = false;
                std::string layer;
                // int width = 0;
                point_t p1, p2; // p1 is left bottom, p2 is right top
                while ((path = (int)p->next()) != DEFIPATH_DONE) {
                    switch (path) {
                        case DEFIPATH_LAYER:
                            if (newLayer == 0) {
                                // fprintf(stdout, "%s ", p->getLayer());
                                newLayer = 1;
                            }
                            else {
                                // fprintf(stdout, "NEW %s ", p->getLayer());
                            }
                            layer = p->getLayer();
                            break;
                        case DEFIPATH_RECT: {
                            p->getViaRect(&w, &x, &y, &z);
                            point_t tmp = p1;
                            bg::assign_values(p1, tmp.get<0>() + w, tmp.get<1>() + x);
                            bg::assign_values(p2, tmp.get<0>() + y, tmp.get<1>() + z);
                            // fprintf(stdout, "RECT ( %d %d %d %d ) ", w, x, y, z);

                            break;
                        }
                        // case DEFIPATH_WIDTH:
                        //     // fprintf(stdout, "%d ", p->getWidth());
                        //     width = p->getWidth();
                        //     break;
                        case DEFIPATH_POINT:
                            p->getPoint(&x, &y);
                            if (num_points == 0) {
                                // for the left bottom point
                                // bg::assign_values(p1, x - width, y - width);
                                bg::assign_values(p1, x, y);
                            }
                            else if (num_points == 1) {
                                // for the right top point
                                // bg::assign_values(p2, x + width, y + width);
                                bg::assign_values(p2, x, y);
                            }
                            else {
                                fprintf(stdout, "ERROR: more than 2 points in a path\n");
                                exit(1);
                            }
                            num_points++;
                            // fprintf(stdout, "( %d %d ) ", x, y);
                            break;
                        case DEFIPATH_FLUSHPOINT:
                            p->getFlushPoint(&x, &y, &z);
                            if (z != 0) {
                                fprintf(stdout, "Error: path has flush point\n");
                            }
                            else {
                                if (num_points == 0) {
                                    // for the left bottom point
                                    // bg::assign_values(p1, x - width, y - width);
                                    bg::assign_values(p1, x, y);
                                }
                                else if (num_points == 1) {
                                    // for the right top point
                                    // bg::assign_values(p2, x + width, y + width);
                                    bg::assign_values(p2, x, y);
                                }
                                else {
                                    fprintf(stdout, "ERROR: more than 2 points in a path\n");
                                    exit(1);
                                }
                                num_points++;
                            }
                            // p->getFlushPoint(&x, &y, &z);
                            // fprintf(stdout, "( %d %d %d ) ", x, y, z);
                            break;
                        case DEFIPATH_VIA:
                            // fprintf(stdout, " %s ", p->getVia());
                            if (num_points == 1)
                                connect_via = true;
                            break;
                    }
                }
                // only metal need to be considered
                // the box of the via is include in the box of the metal
                if (connect_via == 0) {
                    def_info_ptr->add_box(layer, p1, p2);
                }
            }
        }
    }

    return 0;
}

int netpath(defrCallbackType_e c, defiNet *ppath, defiUserData ud) {
    fprintf(stdout, "\n");

    fprintf(stdout, "Callback of partial path for net\n");

    return 0;
}

int snetpath(defrCallbackType_e c, defiNet *ppath, defiUserData ud) {
    int i, j, x, y, z, count, newLayer;
    char *layerName;
    double dist, left, right;
    defiPath *p;
    defiSubnet *s;
    int path;
    defiShield *shield;
    defiWire *wire;
    int numX, numY, stepX, stepY;

    if (c != defrSNetPartialPathCbkType)
        return 1;
    if (ud != userData)
        dataError();
    // 6494 9697 = 3204 - 208 = 2996
    //  COVER, FIXED, ROUTED or SHIELD
    if (ppath->numWires()) {
        newLayer = 0;
        for (i = 0; i < ppath->numWires(); i++) {
            newLayer = 0;
            wire = ppath->wire(i);
            fprintf(stdout, "  %d ", wire->numPaths());
            for (j = 0; j < wire->numPaths(); j++) {
                p = wire->path(j);
                p->initTraverse();
                int num_points = 0;
                bool connect_via = false;
                std::string layer;
                int width = 0;
                point_t p1, p2; // p1 is left bottom, p2 is right top
                while ((path = (int)p->next()) != DEFIPATH_DONE) {
                    switch (path) {
                        case DEFIPATH_LAYER:
                            if (newLayer == 0) {
                                // fprintf(stdout, "%s ", p->getLayer());
                                newLayer = 1;
                            }
                            else {
                                // fprintf(stdout, "NEW %s ", p->getLayer());
                            }
                            layer = p->getLayer();
                            break;
                        case DEFIPATH_WIDTH:
                            // fprintf(stdout, "%d ", p->getWidth());
                            width = p->getWidth();
                            break;
                        case DEFIPATH_POINT:
                            p->getPoint(&x, &y);
                            if (num_points == 0) {
                                // for the left bottom point
                                bg::assign_values(p1, x - width, y - width);
                            }
                            else if (num_points == 1) {
                                // for the right top point
                                bg::assign_values(p2, x + width, y + width);
                            }
                            else {
                                fprintf(stdout, "ERROR: more than 2 points in a path\n");
                                exit(1);
                            }
                            num_points++;
                            // fprintf(stdout, "( %d %d ) ", x, y);

                            break;
                        case DEFIPATH_VIA:
                            // fprintf(stdout, " %s ", p->getVia());
                            connect_via = true;
                            break;
                        default:
                            break;
                    }
                }

                // only metal need to be considered
                // the box of the via is include in the box of the metal
                if (connect_via == 0) {
                    def_info_ptr->add_box(layer, p1, p2);
                }
            }
        }
    }
    return 0;
}

int snetwire(defrCallbackType_e c, defiNet *ppath, defiUserData ud) {
    int i, j, x, y, z, count = 0, newLayer;
    defiPath *p;
    int path;
    defiWire *wire;
    defiShield *shield;
    int numX, numY, stepX, stepY;

    if (c != defrSNetWireCbkType)
        return 1;
    if (ud != userData)
        dataError();

    if (ppath->numWires()) {
        newLayer = 0;
        for (i = 0; i < ppath->numWires(); i++) {
            newLayer = 0;
            wire = ppath->wire(i);
            fprintf(stdout, "  %d ", wire->numPaths());
            for (j = 0; j < wire->numPaths(); j++) {
                p = wire->path(j);
                p->initTraverse();
                int num_points = 0;
                bool connect_via = false;
                std::string layer;
                int width = 0;
                point_t p1, p2; // p1 is left bottom, p2 is right top
                while ((path = (int)p->next()) != DEFIPATH_DONE) {
                    switch (path) {
                        case DEFIPATH_LAYER:
                            if (newLayer == 0) {
                                // fprintf(stdout, "%s ", p->getLayer());
                                newLayer = 1;
                            }
                            else {
                                // fprintf(stdout, "NEW %s ", p->getLayer());
                            }
                            layer = p->getLayer();
                            break;
                        case DEFIPATH_WIDTH:
                            // fprintf(stdout, "%d ", p->getWidth());
                            width = p->getWidth();
                            break;
                        case DEFIPATH_POINT:
                            p->getPoint(&x, &y);
                            if (num_points == 0) {
                                // for the left bottom point
                                bg::assign_values(p1, x - width, y - width);
                            }
                            else if (num_points == 1) {
                                // for the right top point
                                bg::assign_values(p2, x + width, y + width);
                            }
                            else {
                                fprintf(stdout, "ERROR: more than 2 points in a path\n");
                                exit(1);
                            }
                            num_points++;
                            // fprintf(stdout, "( %d %d ) ", x, y);

                            break;
                        case DEFIPATH_VIA:
                            // fprintf(stdout, " %s ", p->getVia());
                            connect_via = true;
                            break;
                        default:
                            break;
                    }
                }

                // only metal need to be considered
                // the box of the via is include in the box of the metal
                if (connect_via == 0) {
                    def_info_ptr->add_box(layer, p1, p2);
                }
            }
        }
    }

    return 0;
}

int snetf(defrCallbackType_e c, defiNet *net, defiUserData ud) {
    // For net and special net.
    int i, j, x, y, z, count, newLayer;
    char *layerName;
    double dist, left, right;
    defiPath *p;
    defiSubnet *s;
    int path;
    defiShield *shield;
    defiWire *wire;
    int numX, numY, stepX, stepY;

    checkType(c);
    if (ud != userData)
        dataError();
    if (c != defrSNetCbkType)
        fprintf(stdout, "BOGUS NET TYPE  ");

    if (net->numWires()) {
        newLayer = 0;
        for (i = 0; i < net->numWires(); i++) {
            newLayer = 0;
            wire = net->wire(i);
            fprintf(stdout, "  %d ", wire->numPaths());
            for (j = 0; j < wire->numPaths(); j++) {
                p = wire->path(j);
                p->initTraverse();
                int num_points = 0;
                bool connect_via = false;
                std::string layer;
                int width = 0;
                point_t p1, p2; // p1 is left bottom, p2 is right top
                while ((path = (int)p->next()) != DEFIPATH_DONE) {
                    switch (path) {
                        case DEFIPATH_LAYER:
                            if (newLayer == 0) {
                                // fprintf(stdout, "%s ", p->getLayer());
                                newLayer = 1;
                            }
                            else {
                                // fprintf(stdout, "NEW %s ", p->getLayer());
                            }
                            layer = p->getLayer();
                            break;
                        case DEFIPATH_WIDTH:
                            // fprintf(stdout, "%d ", p->getWidth());
                            width = p->getWidth();
                            break;
                        case DEFIPATH_POINT:
                            p->getPoint(&x, &y);
                            if (num_points == 0) {
                                // for the left bottom point
                                bg::assign_values(p1, x - width, y - width);
                            }
                            else if (num_points == 1) {
                                // for the right top point
                                bg::assign_values(p2, x + width, y + width);
                            }
                            else {
                                fprintf(stdout, "ERROR: more than 2 points in a path\n");
                                exit(1);
                            }
                            num_points++;
                            // fprintf(stdout, "( %d %d ) ", x, y);

                            break;
                        case DEFIPATH_VIA:
                            // fprintf(stdout, " %s ", p->getVia());
                            connect_via = true;
                            break;
                    }
                }

                // only metal need to be considered
                // the box of the via is include in the box of the metal
                if (connect_via == 0) {
                    def_info_ptr->add_box(layer, p1, p2);
                }
            }
        }
    }
    return 0;
}

int ext(defrCallbackType_e t, const char *c, defiUserData ud) {
    char *name;

    checkType(t);
    if (ud != userData)
        dataError();

    switch (t) {
        case defrNetExtCbkType:
            name = address("net");
            break;
        case defrComponentExtCbkType:
            name = address("component");
            break;
        case defrPinExtCbkType:
            name = address("pin");
            break;
        case defrViaExtCbkType:
            name = address("via");
            break;
        case defrNetConnectionExtCbkType:
            name = address("net connection");
            break;
        case defrGroupExtCbkType:
            name = address("group");
            break;
        case defrScanChainExtCbkType:
            name = address("scanchain");
            break;
        case defrIoTimingsExtCbkType:
            name = address("io timing");
            break;
        case defrPartitionsExtCbkType:
            name = address("partition");
            break;
        default:
            name = address("BOGUS");
            return 1;
    }
    fprintf(stdout, "  %s extension %s\n", name, c);
    return 0;
}

int endfunc(defrCallbackType_e c, void *dummy, defiUserData ud) {
    checkType(c);
    if (ud != userData)
        dataError();
    return 0;
}

int def_parser(int argc, char **argv, def_info_t &def_info) {
    // for debugging
    setbuf(stdout, NULL);

    def_info_ptr = &def_info;
    char *inFile[6];
    char *outFile;
    FILE *f;
    int res;
    fout = stdout;
    int numInFile = 0;
    int fileCt = 0;

    argc--;
    argv++;
    if (argc == 0) {
        fprintf(stderr, "Type 'defrw --help' for the help.\n");
        return 2;
    }

    while (argc--) {
        if (strcmp(*argv, "-o") == 0) {
            argc--;
            argv++;
            outFile = *argv;
            if ((fout = fopen(outFile, "w")) == 0) {
                fprintf(stderr, "ERROR: could not open output file\n");
                return 2;
            }
        }
        else if (strcmp(*argv, "-i") == 0) {
            argv++;
            argc--;
            line_num_print_interval = atoi(*argv);
        }
        else if (argv[0][0] != '-') {
            if (numInFile >= 6) {
                fprintf(stderr, "ERROR: too many input files, max = 6.\n");
                return 2;
            }
            inFile[numInFile++] = *argv;
        }
        else if ((strcmp(*argv, "-h") == 0) || (strcmp(*argv, "--help") == 0)) {
            fprintf(stderr, "Usage: defrw (<option>|<file>)* \n");
            fprintf(stderr, "Files:\n");
            fprintf(stderr, "Options:\n");
            fprintf(stderr, "\t-i <num_lines> -- sets processing msg interval (default: 50 lines).\n");
            fprintf(stderr, "\t-o <out_file>  -- write output to the file.\n");
            return 2;
        }
        else {
            fprintf(stderr, "ERROR: Illegal command line option: '%s'\n", *argv);
            return 2;
        }

        argv++;
    }

    defrInitSession(1);

    // setting callback functions
    defrSetWarningLogFunction(printWarning);
    defrSetUserData((void *)3);
    defrSetDesignCbk(dname);
    defrSetTechnologyCbk(tname);
    defrSetUnitsCbk(units);

    // TODO:檢查有沒有SUBNET SHEILD
    // TODO:net 可能沒有width的選項

    defrSetNetCbk(netf); /////////////////////FIXME://////////////////////////////////////////////////
    defrSetNetPartialPathCbk(netpath); /////////////////////////FIXME://////////////////////////////////////////////

    // 3個callback都需要有，且給不同的subroutine，但內容雷同
    // 否則會回傳bad status
    // call的順序是snetpath->snetwire->snetf
    // snetpath會一次讀1000行
    // snetwire會把最後一輪不到1000行的讀完
    // snetf總結所有的path
    defrSetSNetPartialPathCbk(snetpath); ///////////////////////FIXME:////////////////////////////////////////////////
    defrSetSNetWireCbk(snetwire);
    defrSetSNetCbk(snetf);

    defrSetAddPathToNet(); /////////////////must activate to add path to net

    // may not need
    // defrSetNonDefaultCbk(ndr);

    defrSetComponentStartCbk(cs);
    defrSetPinPropStartCbk(cs);
    defrSetNetStartCbk(cs);
    defrSetStartPinsCbk(cs);
    defrSetViaStartCbk(cs);
    defrSetRegionStartCbk(cs);
    defrSetSNetStartCbk(cs);
    defrSetGroupsStartCbk(cs);
    defrSetScanchainsStartCbk(cs);
    defrSetIOTimingsStartCbk(cs);
    defrSetFPCStartCbk(cs);
    defrSetTimingDisablesStartCbk(cs);
    defrSetPartitionsStartCbk(cs);
    defrSetBlockageStartCbk(cs);
    defrSetSlotStartCbk(cs);
    defrSetFillStartCbk(cs);
    defrSetNonDefaultStartCbk(cs);
    defrSetStylesStartCbk(cs);

    // may not need
    defrSetNetExtCbk(ext);
    defrSetNetConnectionExtCbk(ext);

    // may not need
    defrSetNetEndCbk(endfunc);
    defrSetSNetEndCbk(endfunc);

    defrSetMallocFunction(mallocCB);
    defrSetReallocFunction(reallocCB);
    defrSetFreeFunction(freeCB);

    defrSetAssertionWarnings(3);
    defrSetBlockageWarnings(3);
    defrSetCaseSensitiveWarnings(3);
    defrSetComponentWarnings(3);
    defrSetConstraintWarnings(0);
    defrSetDefaultCapWarnings(3);
    defrSetGcellGridWarnings(3);
    defrSetIOTimingWarnings(3);
    defrSetNetWarnings(3);
    defrSetNonDefaultWarnings(3);
    defrSetPinExtWarnings(3);
    defrSetPinWarnings(3);
    defrSetRegionWarnings(3);
    defrSetRowWarnings(3);
    defrSetScanchainWarnings(3);
    defrSetSNetWarnings(3);
    defrSetStylesWarnings(3);
    defrSetTrackWarnings(3);
    defrSetUnitsWarnings(3);
    defrSetVersionWarnings(3);
    defrSetViaWarnings(3);

    defrSetLongLineNumberFunction(lineNumberCB);
    defrSetDeltaNumberLines(line_num_print_interval);

    (void)defrSetOpenLogFileAppend();

    for (fileCt = 0; fileCt < numInFile; fileCt++) {
        if (strcmp(inFile[fileCt], "STDIN") == 0) {
            f = stdin;
        }
        else if ((f = fopen(inFile[fileCt], "r")) == 0) {
            fprintf(stderr, "Couldn't open input file '%s'\n", inFile[fileCt]);
            return (2);
        }
        // Set case sensitive to 0 to start with, in History & PropertyDefinition
        // reset it to 1.

        res = defrRead(f, inFile[fileCt], userData, 1);
        //  defrData *defData;
        //  defiNet *net = &(defData->Net);
        //  defData->PathObj.print(stdout);
        //  net->print(stdout);
        //  net->wire(0)->path(0)->print(stdout);
        //  net->wire(0)->path(1)->print(stdout);
        //  net->wire(0)->path(2)->print(stdout);
        if (res)
            fprintf(stderr, "Reader returns bad status.\n", inFile[fileCt]);

        // Testing the aliases API.
        defrAddAlias("alias1", "aliasValue1", 1);

        defiAlias_itr aliasStore;
        const char *alias1Value = NULL;

        while (aliasStore.Next()) {
            if (strcmp(aliasStore.Key(), "alias1") == 0) {
                alias1Value = aliasStore.Data();
            }
        }

        if (!alias1Value || strcmp(alias1Value, "aliasValue1")) {
            fprintf(stderr, "ERROR: Aliases don't work\n");
        }

        (void)defrPrintUnusedCallbacks(fout);
        (void)defrReleaseNResetMemory();
    }
    (void)defrUnsetCallbacks();
    (void)defrSetUnusedCallbacks(unUsedCB);

    // Unset all the callbacks
    defrUnsetArrayNameCbk();
    defrUnsetAssertionCbk();
    defrUnsetAssertionsStartCbk();
    defrUnsetAssertionsEndCbk();
    defrUnsetBlockageCbk();
    defrUnsetBlockageStartCbk();
    defrUnsetBlockageEndCbk();
    defrUnsetBusBitCbk();
    defrUnsetCannotOccupyCbk();
    defrUnsetCanplaceCbk();
    defrUnsetCaseSensitiveCbk();
    defrUnsetComponentCbk();
    defrUnsetComponentExtCbk();
    defrUnsetComponentStartCbk();
    defrUnsetComponentEndCbk();
    defrUnsetConstraintCbk();
    defrUnsetConstraintsStartCbk();
    defrUnsetConstraintsEndCbk();
    defrUnsetDefaultCapCbk();
    defrUnsetDesignCbk();
    defrUnsetDesignEndCbk();
    defrUnsetDieAreaCbk();
    defrUnsetDividerCbk();
    defrUnsetExtensionCbk();
    defrUnsetFillCbk();
    defrUnsetFillStartCbk();
    defrUnsetFillEndCbk();
    defrUnsetFPCCbk();
    defrUnsetFPCStartCbk();
    defrUnsetFPCEndCbk();
    defrUnsetFloorPlanNameCbk();
    defrUnsetGcellGridCbk();
    defrUnsetGroupCbk();
    defrUnsetGroupExtCbk();
    defrUnsetGroupMemberCbk();
    defrUnsetComponentMaskShiftLayerCbk();
    defrUnsetGroupNameCbk();
    defrUnsetGroupsStartCbk();
    defrUnsetGroupsEndCbk();
    defrUnsetHistoryCbk();
    defrUnsetIOTimingCbk();
    defrUnsetIOTimingsStartCbk();
    defrUnsetIOTimingsEndCbk();
    defrUnsetIOTimingsExtCbk();
    defrUnsetNetCbk();
    defrUnsetNetNameCbk();
    defrUnsetNetNonDefaultRuleCbk();
    defrUnsetNetConnectionExtCbk();
    defrUnsetNetExtCbk();
    defrUnsetNetPartialPathCbk();
    defrUnsetNetSubnetNameCbk();
    defrUnsetNetStartCbk();
    defrUnsetNetEndCbk();
    defrUnsetNonDefaultCbk();
    defrUnsetNonDefaultStartCbk();
    defrUnsetNonDefaultEndCbk();
    defrUnsetPartitionCbk();
    defrUnsetPartitionsExtCbk();
    defrUnsetPartitionsStartCbk();
    defrUnsetPartitionsEndCbk();
    defrUnsetPathCbk();
    defrUnsetPinCapCbk();
    defrUnsetPinCbk();
    defrUnsetPinEndCbk();
    defrUnsetPinExtCbk();
    defrUnsetPinPropCbk();
    defrUnsetPinPropStartCbk();
    defrUnsetPinPropEndCbk();
    defrUnsetPropCbk();
    defrUnsetPropDefEndCbk();
    defrUnsetPropDefStartCbk();
    defrUnsetRegionCbk();
    defrUnsetRegionStartCbk();
    defrUnsetRegionEndCbk();
    defrUnsetRowCbk();
    defrUnsetScanChainExtCbk();
    defrUnsetScanchainCbk();
    defrUnsetScanchainsStartCbk();
    defrUnsetScanchainsEndCbk();
    defrUnsetSiteCbk();
    defrUnsetSlotCbk();
    defrUnsetSlotStartCbk();
    defrUnsetSlotEndCbk();
    defrUnsetSNetWireCbk();
    defrUnsetSNetCbk();
    defrUnsetSNetStartCbk();
    defrUnsetSNetEndCbk();
    defrUnsetSNetPartialPathCbk();
    defrUnsetStartPinsCbk();
    defrUnsetStylesCbk();
    defrUnsetStylesStartCbk();
    defrUnsetStylesEndCbk();
    defrUnsetTechnologyCbk();
    defrUnsetTimingDisableCbk();
    defrUnsetTimingDisablesStartCbk();
    defrUnsetTimingDisablesEndCbk();
    defrUnsetTrackCbk();
    defrUnsetUnitsCbk();
    defrUnsetVersionCbk();
    defrUnsetVersionStrCbk();
    defrUnsetViaCbk();
    defrUnsetViaExtCbk();
    defrUnsetViaStartCbk();
    defrUnsetViaEndCbk();

    fclose(fout);

    // Release allocated singleton data.
    defrClear();

    return res;
}