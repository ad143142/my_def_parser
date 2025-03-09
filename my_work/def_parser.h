#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <vector>
#include <utility> 

#ifndef WIN32
#include <unistd.h>
#endif /* not WIN32 */

#include "defrReader.hpp"
#include "defiAlias.hpp"
#include "defrData.hpp"

#include "def_info.h"
#include "rtree.h"

#ifndef DEF_PARSER_H
#define DEF_PARSER_H
int def_parser(int argc, char **argv, def_info_t &def_info);


void freeCB(void *name);
void *reallocCB(void *name, size_t size);
void *mallocCB(size_t size);

void lineNumberCB(long long lineNo);
int unUsedCB(defrCallbackType_e c, void* any, defiUserData ud);

int tname(defrCallbackType_e c, const char* string, defiUserData ud);
int dname(defrCallbackType_e c, const char* string, defiUserData ud);
int units(defrCallbackType_e c, double d, defiUserData ud);
void printWarning(const char *str);
void checkType(defrCallbackType_e c);
char *address(const char *in);
void dataError();
int endfunc(defrCallbackType_e c, void *dummy, defiUserData ud);

int ext(defrCallbackType_e t, const char *c, defiUserData ud);
int cs(defrCallbackType_e c, int num, defiUserData ud);

int netf(defrCallbackType_e c, defiNet *net, defiUserData ud);
int snetf(defrCallbackType_e c, defiNet* net, defiUserData ud);
int netNamef(defrCallbackType_e c, const char *netName, defiUserData ud);
int subnetNamef(defrCallbackType_e c, const char *subnetName, defiUserData ud);
int nondefRulef(defrCallbackType_e c, const char* ruleName, defiUserData ud);
int snetpath(defrCallbackType_e c, defiNet* ppath, defiUserData ud);
int netpath(defrCallbackType_e c, defiNet* ppath, defiUserData ud);// need to check
int snetwire(defrCallbackType_e c, defiNet* ppath, defiUserData ud);
int ndr(defrCallbackType_e c, defiNonDefault *nd, defiUserData ud);
#endif