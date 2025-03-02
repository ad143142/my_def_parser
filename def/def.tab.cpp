/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30802

/* Bison version string.  */
#define YYBISON_VERSION "3.8.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1


/* Substitute the variable and function names.  */
#define yyparse         defyyparse
#define yylex           defyylex
#define yyerror         defyyerror
#define yydebug         defyydebug
#define yynerrs         defyynerrs
#define yylval          defyylval
#define yychar          defyychar

/* First part of user prologue.  */
#line 58 "def.y"

#include <stdlib.h>
#include <string.h>
#include "defrReader.hpp"
#include "defiUser.hpp"
#include "defrCallBacks.hpp"
#include "lex.h"

#define DEF_MAX_INT 2147483647
#define YYDEBUG 1     // this is temp fix for pcr 755132 
// TX_DIR:TRANSLATION ON


#include "defrData.hpp"
#include "defrSettings.hpp"
#include "defrCallBacks.hpp"

BEGIN_LEFDEF_PARSER_NAMESPACE

extern int errors;        // from lex.cpph
extern int defInPropDef;  // from lex.cpph, tracking if insided propDef

// From def_keywords.cpp 
extern char* ringCopy(const char* string);

extern void pathIsDone(int shield, int reset, int netOsnet, int *needCbk);

// Macro to describe how we handle a callback.
// If the function was set then call it.
// If the function returns non zero then there was an error
// so call the error routine and exit.
//
#define CALLBACK(func, typ, data) \
    if (!defData->errors) {\
      if (func) { \
        if ((defData->defRetVal = (*func)(typ, data, defSettings->UserData)) == PARSE_OK) { \
        } else if (defData->defRetVal == STOP_PARSE) { \
          return defData->defRetVal; \
        } else { \
          defError(6010, "An error has been reported in callback."); \
          return defData->defRetVal; \
        } \
      } \
    }

#define CHKERR() \
    if (defData->errors > 20) {\
      defError(6011, "Too many syntax defData->errors have been reported."); \
      defData->errors = 0; \
      return 1; \
    }

#define CHKPROPTYPE(propType, propName, name) \
    if (propType == 'N') { \
       defData->warningMsg = (char*)defMalloc(strlen(propName)+strlen(name)+40); \
       sprintf(defData->warningMsg, "The PropName %s is not defined for %s.", \
               propName, name); \
       defWarning(7010, defData->warningMsg); \
       defFree(defData->warningMsg); \
    }

int validateMaskInput(int input, int warningIndex, int getWarningsIndex) 
{
    if (defData->VersionNum < 5.8 && input > 0) {
      if (warningIndex++ < getWarningsIndex) {
          defData->defMsg = (char*)defMalloc(1000);
          sprintf (defData->defMsg,
             "The MASK statement is available in version 5.8 and later.\nHowever, your DEF file is defined with version %g", defData->VersionNum);
          defError(7415, defData->defMsg);
          defFree(defData->defMsg);
          CHKERR();
          
          return 0;
        }
    }   
    
    return 1; 
}

double convert_defname2num(char *versionName)
{
    char majorNm[80];
    char minorNm[80];
    char *subMinorNm = NULL;
    char *versionNm = strdup(versionName);

    double major = 0, minor = 0, subMinor = 0;
    double version;

    sscanf(versionNm, "%[^.].%s", majorNm, minorNm);
    
    char *p1 = strchr(minorNm, '.');
    if (p1) {
       subMinorNm = p1+1;
       *p1 = '\0';
    }
    major = atof(majorNm);
    minor = atof(minorNm);
    if (subMinorNm)
       subMinor = atof(subMinorNm);

    version = major;

    if (minor > 0)
       version = major + minor/10;

    if (subMinor > 0)
       version = version + subMinor/1000;

    free(versionNm);
    return version;
}

int numIsInt (char* volt) {
    if (strchr(volt, '.'))  // a floating point
       return 0;
    else
       return 1;
}

int defValidNum(int values) {
    char *outMsg;
    switch (values) {
        case 100:
        case 200:
        case 1000:
        case 2000:
                return 1;
        case 400:
        case 800:
        case 4000:
        case 8000:
        case 10000:
        case 20000:
             if (defData->VersionNum < 5.6) {
                if (defCallbacks->UnitsCbk) {
                  if (defData->unitsWarnings++ < defSettings->UnitsWarnings) {
                    outMsg = (char*)defMalloc(1000);
                    sprintf (outMsg,
                    "An error has been found while processing the DEF file '%s'\nUnit %d is a 5.6 or later syntax. Define the DEF file as 5.6 and then try again.",
                    defSettings->FileName, values);
                    defError(6501, outMsg);
                    defFree(outMsg);
                  }
                }
                
                return 0;
             } else {
                return 1;
             }
    }
    if (defCallbacks->UnitsCbk) {
      if (defData->unitsWarnings++ < defSettings->UnitsWarnings) {
        outMsg = (char*)defMalloc(10000);
        sprintf (outMsg,
          "The value %d defined for DEF UNITS DISTANCE MICRON is invalid\n. The valid values are 100, 200, 400, 800, 1000, 2000, 4000, 8000, 10000, or 20000. Specify a valid value and then try again.", values);
        defError(6502, outMsg);
        defFree(outMsg);
        CHKERR();
      }
    }
    return 0;
}

#define FIXED 1
#define COVER 2
#define PLACED 3
#define UNPLACED 4

#line 248 "def.tab.c"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

#include "def.tab.h"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_QSTRING = 3,                    /* QSTRING  */
  YYSYMBOL_T_STRING = 4,                   /* T_STRING  */
  YYSYMBOL_SITE_PATTERN = 5,               /* SITE_PATTERN  */
  YYSYMBOL_NUMBER = 6,                     /* NUMBER  */
  YYSYMBOL_K_HISTORY = 7,                  /* K_HISTORY  */
  YYSYMBOL_K_NAMESCASESENSITIVE = 8,       /* K_NAMESCASESENSITIVE  */
  YYSYMBOL_K_DESIGN = 9,                   /* K_DESIGN  */
  YYSYMBOL_K_VIAS = 10,                    /* K_VIAS  */
  YYSYMBOL_K_TECH = 11,                    /* K_TECH  */
  YYSYMBOL_K_UNITS = 12,                   /* K_UNITS  */
  YYSYMBOL_K_ARRAY = 13,                   /* K_ARRAY  */
  YYSYMBOL_K_FLOORPLAN = 14,               /* K_FLOORPLAN  */
  YYSYMBOL_K_SITE = 15,                    /* K_SITE  */
  YYSYMBOL_K_CANPLACE = 16,                /* K_CANPLACE  */
  YYSYMBOL_K_CANNOTOCCUPY = 17,            /* K_CANNOTOCCUPY  */
  YYSYMBOL_K_DIEAREA = 18,                 /* K_DIEAREA  */
  YYSYMBOL_K_PINS = 19,                    /* K_PINS  */
  YYSYMBOL_K_DEFAULTCAP = 20,              /* K_DEFAULTCAP  */
  YYSYMBOL_K_MINPINS = 21,                 /* K_MINPINS  */
  YYSYMBOL_K_WIRECAP = 22,                 /* K_WIRECAP  */
  YYSYMBOL_K_TRACKS = 23,                  /* K_TRACKS  */
  YYSYMBOL_K_GCELLGRID = 24,               /* K_GCELLGRID  */
  YYSYMBOL_K_DO = 25,                      /* K_DO  */
  YYSYMBOL_K_BY = 26,                      /* K_BY  */
  YYSYMBOL_K_STEP = 27,                    /* K_STEP  */
  YYSYMBOL_K_LAYER = 28,                   /* K_LAYER  */
  YYSYMBOL_K_ROW = 29,                     /* K_ROW  */
  YYSYMBOL_K_RECT = 30,                    /* K_RECT  */
  YYSYMBOL_K_COMPS = 31,                   /* K_COMPS  */
  YYSYMBOL_K_COMP_GEN = 32,                /* K_COMP_GEN  */
  YYSYMBOL_K_SOURCE = 33,                  /* K_SOURCE  */
  YYSYMBOL_K_WEIGHT = 34,                  /* K_WEIGHT  */
  YYSYMBOL_K_EEQMASTER = 35,               /* K_EEQMASTER  */
  YYSYMBOL_K_FIXED = 36,                   /* K_FIXED  */
  YYSYMBOL_K_COVER = 37,                   /* K_COVER  */
  YYSYMBOL_K_UNPLACED = 38,                /* K_UNPLACED  */
  YYSYMBOL_K_PLACED = 39,                  /* K_PLACED  */
  YYSYMBOL_K_FOREIGN = 40,                 /* K_FOREIGN  */
  YYSYMBOL_K_REGION = 41,                  /* K_REGION  */
  YYSYMBOL_K_REGIONS = 42,                 /* K_REGIONS  */
  YYSYMBOL_K_NETS = 43,                    /* K_NETS  */
  YYSYMBOL_K_START_NET = 44,               /* K_START_NET  */
  YYSYMBOL_K_MUSTJOIN = 45,                /* K_MUSTJOIN  */
  YYSYMBOL_K_ORIGINAL = 46,                /* K_ORIGINAL  */
  YYSYMBOL_K_USE = 47,                     /* K_USE  */
  YYSYMBOL_K_STYLE = 48,                   /* K_STYLE  */
  YYSYMBOL_K_PATTERN = 49,                 /* K_PATTERN  */
  YYSYMBOL_K_PATTERNNAME = 50,             /* K_PATTERNNAME  */
  YYSYMBOL_K_ESTCAP = 51,                  /* K_ESTCAP  */
  YYSYMBOL_K_ROUTED = 52,                  /* K_ROUTED  */
  YYSYMBOL_K_NEW = 53,                     /* K_NEW  */
  YYSYMBOL_K_SNETS = 54,                   /* K_SNETS  */
  YYSYMBOL_K_SHAPE = 55,                   /* K_SHAPE  */
  YYSYMBOL_K_WIDTH = 56,                   /* K_WIDTH  */
  YYSYMBOL_K_VOLTAGE = 57,                 /* K_VOLTAGE  */
  YYSYMBOL_K_SPACING = 58,                 /* K_SPACING  */
  YYSYMBOL_K_NONDEFAULTRULE = 59,          /* K_NONDEFAULTRULE  */
  YYSYMBOL_K_NONDEFAULTRULES = 60,         /* K_NONDEFAULTRULES  */
  YYSYMBOL_K_N = 61,                       /* K_N  */
  YYSYMBOL_K_S = 62,                       /* K_S  */
  YYSYMBOL_K_E = 63,                       /* K_E  */
  YYSYMBOL_K_W = 64,                       /* K_W  */
  YYSYMBOL_K_FN = 65,                      /* K_FN  */
  YYSYMBOL_K_FE = 66,                      /* K_FE  */
  YYSYMBOL_K_FS = 67,                      /* K_FS  */
  YYSYMBOL_K_FW = 68,                      /* K_FW  */
  YYSYMBOL_K_GROUPS = 69,                  /* K_GROUPS  */
  YYSYMBOL_K_GROUP = 70,                   /* K_GROUP  */
  YYSYMBOL_K_SOFT = 71,                    /* K_SOFT  */
  YYSYMBOL_K_MAXX = 72,                    /* K_MAXX  */
  YYSYMBOL_K_MAXY = 73,                    /* K_MAXY  */
  YYSYMBOL_K_MAXHALFPERIMETER = 74,        /* K_MAXHALFPERIMETER  */
  YYSYMBOL_K_CONSTRAINTS = 75,             /* K_CONSTRAINTS  */
  YYSYMBOL_K_NET = 76,                     /* K_NET  */
  YYSYMBOL_K_PATH = 77,                    /* K_PATH  */
  YYSYMBOL_K_SUM = 78,                     /* K_SUM  */
  YYSYMBOL_K_DIFF = 79,                    /* K_DIFF  */
  YYSYMBOL_K_SCANCHAINS = 80,              /* K_SCANCHAINS  */
  YYSYMBOL_K_START = 81,                   /* K_START  */
  YYSYMBOL_K_FLOATING = 82,                /* K_FLOATING  */
  YYSYMBOL_K_ORDERED = 83,                 /* K_ORDERED  */
  YYSYMBOL_K_STOP = 84,                    /* K_STOP  */
  YYSYMBOL_K_IN = 85,                      /* K_IN  */
  YYSYMBOL_K_OUT = 86,                     /* K_OUT  */
  YYSYMBOL_K_RISEMIN = 87,                 /* K_RISEMIN  */
  YYSYMBOL_K_RISEMAX = 88,                 /* K_RISEMAX  */
  YYSYMBOL_K_FALLMIN = 89,                 /* K_FALLMIN  */
  YYSYMBOL_K_FALLMAX = 90,                 /* K_FALLMAX  */
  YYSYMBOL_K_WIREDLOGIC = 91,              /* K_WIREDLOGIC  */
  YYSYMBOL_K_MAXDIST = 92,                 /* K_MAXDIST  */
  YYSYMBOL_K_ASSERTIONS = 93,              /* K_ASSERTIONS  */
  YYSYMBOL_K_DISTANCE = 94,                /* K_DISTANCE  */
  YYSYMBOL_K_MICRONS = 95,                 /* K_MICRONS  */
  YYSYMBOL_K_END = 96,                     /* K_END  */
  YYSYMBOL_K_IOTIMINGS = 97,               /* K_IOTIMINGS  */
  YYSYMBOL_K_RISE = 98,                    /* K_RISE  */
  YYSYMBOL_K_FALL = 99,                    /* K_FALL  */
  YYSYMBOL_K_VARIABLE = 100,               /* K_VARIABLE  */
  YYSYMBOL_K_SLEWRATE = 101,               /* K_SLEWRATE  */
  YYSYMBOL_K_CAPACITANCE = 102,            /* K_CAPACITANCE  */
  YYSYMBOL_K_DRIVECELL = 103,              /* K_DRIVECELL  */
  YYSYMBOL_K_FROMPIN = 104,                /* K_FROMPIN  */
  YYSYMBOL_K_TOPIN = 105,                  /* K_TOPIN  */
  YYSYMBOL_K_PARALLEL = 106,               /* K_PARALLEL  */
  YYSYMBOL_K_TIMINGDISABLES = 107,         /* K_TIMINGDISABLES  */
  YYSYMBOL_K_THRUPIN = 108,                /* K_THRUPIN  */
  YYSYMBOL_K_MACRO = 109,                  /* K_MACRO  */
  YYSYMBOL_K_PARTITIONS = 110,             /* K_PARTITIONS  */
  YYSYMBOL_K_TURNOFF = 111,                /* K_TURNOFF  */
  YYSYMBOL_K_FROMCLOCKPIN = 112,           /* K_FROMCLOCKPIN  */
  YYSYMBOL_K_FROMCOMPPIN = 113,            /* K_FROMCOMPPIN  */
  YYSYMBOL_K_FROMIOPIN = 114,              /* K_FROMIOPIN  */
  YYSYMBOL_K_TOCLOCKPIN = 115,             /* K_TOCLOCKPIN  */
  YYSYMBOL_K_TOCOMPPIN = 116,              /* K_TOCOMPPIN  */
  YYSYMBOL_K_TOIOPIN = 117,                /* K_TOIOPIN  */
  YYSYMBOL_K_SETUPRISE = 118,              /* K_SETUPRISE  */
  YYSYMBOL_K_SETUPFALL = 119,              /* K_SETUPFALL  */
  YYSYMBOL_K_HOLDRISE = 120,               /* K_HOLDRISE  */
  YYSYMBOL_K_HOLDFALL = 121,               /* K_HOLDFALL  */
  YYSYMBOL_K_VPIN = 122,                   /* K_VPIN  */
  YYSYMBOL_K_SUBNET = 123,                 /* K_SUBNET  */
  YYSYMBOL_K_XTALK = 124,                  /* K_XTALK  */
  YYSYMBOL_K_PIN = 125,                    /* K_PIN  */
  YYSYMBOL_K_SYNTHESIZED = 126,            /* K_SYNTHESIZED  */
  YYSYMBOL_K_DEFINE = 127,                 /* K_DEFINE  */
  YYSYMBOL_K_DEFINES = 128,                /* K_DEFINES  */
  YYSYMBOL_K_DEFINEB = 129,                /* K_DEFINEB  */
  YYSYMBOL_K_IF = 130,                     /* K_IF  */
  YYSYMBOL_K_THEN = 131,                   /* K_THEN  */
  YYSYMBOL_K_ELSE = 132,                   /* K_ELSE  */
  YYSYMBOL_K_FALSE = 133,                  /* K_FALSE  */
  YYSYMBOL_K_TRUE = 134,                   /* K_TRUE  */
  YYSYMBOL_K_EQ = 135,                     /* K_EQ  */
  YYSYMBOL_K_NE = 136,                     /* K_NE  */
  YYSYMBOL_K_LE = 137,                     /* K_LE  */
  YYSYMBOL_K_LT = 138,                     /* K_LT  */
  YYSYMBOL_K_GE = 139,                     /* K_GE  */
  YYSYMBOL_K_GT = 140,                     /* K_GT  */
  YYSYMBOL_K_OR = 141,                     /* K_OR  */
  YYSYMBOL_K_AND = 142,                    /* K_AND  */
  YYSYMBOL_K_NOT = 143,                    /* K_NOT  */
  YYSYMBOL_K_SPECIAL = 144,                /* K_SPECIAL  */
  YYSYMBOL_K_DIRECTION = 145,              /* K_DIRECTION  */
  YYSYMBOL_K_RANGE = 146,                  /* K_RANGE  */
  YYSYMBOL_K_FPC = 147,                    /* K_FPC  */
  YYSYMBOL_K_HORIZONTAL = 148,             /* K_HORIZONTAL  */
  YYSYMBOL_K_VERTICAL = 149,               /* K_VERTICAL  */
  YYSYMBOL_K_ALIGN = 150,                  /* K_ALIGN  */
  YYSYMBOL_K_MIN = 151,                    /* K_MIN  */
  YYSYMBOL_K_MAX = 152,                    /* K_MAX  */
  YYSYMBOL_K_EQUAL = 153,                  /* K_EQUAL  */
  YYSYMBOL_K_BOTTOMLEFT = 154,             /* K_BOTTOMLEFT  */
  YYSYMBOL_K_TOPRIGHT = 155,               /* K_TOPRIGHT  */
  YYSYMBOL_K_ROWS = 156,                   /* K_ROWS  */
  YYSYMBOL_K_TAPER = 157,                  /* K_TAPER  */
  YYSYMBOL_K_TAPERRULE = 158,              /* K_TAPERRULE  */
  YYSYMBOL_K_VERSION = 159,                /* K_VERSION  */
  YYSYMBOL_K_DIVIDERCHAR = 160,            /* K_DIVIDERCHAR  */
  YYSYMBOL_K_BUSBITCHARS = 161,            /* K_BUSBITCHARS  */
  YYSYMBOL_K_PROPERTYDEFINITIONS = 162,    /* K_PROPERTYDEFINITIONS  */
  YYSYMBOL_K_STRING = 163,                 /* K_STRING  */
  YYSYMBOL_K_REAL = 164,                   /* K_REAL  */
  YYSYMBOL_K_INTEGER = 165,                /* K_INTEGER  */
  YYSYMBOL_K_PROPERTY = 166,               /* K_PROPERTY  */
  YYSYMBOL_K_BEGINEXT = 167,               /* K_BEGINEXT  */
  YYSYMBOL_K_ENDEXT = 168,                 /* K_ENDEXT  */
  YYSYMBOL_K_NAMEMAPSTRING = 169,          /* K_NAMEMAPSTRING  */
  YYSYMBOL_K_ON = 170,                     /* K_ON  */
  YYSYMBOL_K_OFF = 171,                    /* K_OFF  */
  YYSYMBOL_K_X = 172,                      /* K_X  */
  YYSYMBOL_K_Y = 173,                      /* K_Y  */
  YYSYMBOL_K_COMPONENT = 174,              /* K_COMPONENT  */
  YYSYMBOL_K_MASK = 175,                   /* K_MASK  */
  YYSYMBOL_K_MASKSHIFT = 176,              /* K_MASKSHIFT  */
  YYSYMBOL_K_COMPSMASKSHIFT = 177,         /* K_COMPSMASKSHIFT  */
  YYSYMBOL_K_SAMEMASK = 178,               /* K_SAMEMASK  */
  YYSYMBOL_K_PINPROPERTIES = 179,          /* K_PINPROPERTIES  */
  YYSYMBOL_K_TEST = 180,                   /* K_TEST  */
  YYSYMBOL_K_COMMONSCANPINS = 181,         /* K_COMMONSCANPINS  */
  YYSYMBOL_K_SNET = 182,                   /* K_SNET  */
  YYSYMBOL_K_COMPONENTPIN = 183,           /* K_COMPONENTPIN  */
  YYSYMBOL_K_REENTRANTPATHS = 184,         /* K_REENTRANTPATHS  */
  YYSYMBOL_K_SHIELD = 185,                 /* K_SHIELD  */
  YYSYMBOL_K_SHIELDNET = 186,              /* K_SHIELDNET  */
  YYSYMBOL_K_NOSHIELD = 187,               /* K_NOSHIELD  */
  YYSYMBOL_K_VIRTUAL = 188,                /* K_VIRTUAL  */
  YYSYMBOL_K_ANTENNAPINPARTIALMETALAREA = 189, /* K_ANTENNAPINPARTIALMETALAREA  */
  YYSYMBOL_K_ANTENNAPINPARTIALMETALSIDEAREA = 190, /* K_ANTENNAPINPARTIALMETALSIDEAREA  */
  YYSYMBOL_K_ANTENNAPINGATEAREA = 191,     /* K_ANTENNAPINGATEAREA  */
  YYSYMBOL_K_ANTENNAPINDIFFAREA = 192,     /* K_ANTENNAPINDIFFAREA  */
  YYSYMBOL_K_ANTENNAPINMAXAREACAR = 193,   /* K_ANTENNAPINMAXAREACAR  */
  YYSYMBOL_K_ANTENNAPINMAXSIDEAREACAR = 194, /* K_ANTENNAPINMAXSIDEAREACAR  */
  YYSYMBOL_K_ANTENNAPINPARTIALCUTAREA = 195, /* K_ANTENNAPINPARTIALCUTAREA  */
  YYSYMBOL_K_ANTENNAPINMAXCUTCAR = 196,    /* K_ANTENNAPINMAXCUTCAR  */
  YYSYMBOL_K_SIGNAL = 197,                 /* K_SIGNAL  */
  YYSYMBOL_K_POWER = 198,                  /* K_POWER  */
  YYSYMBOL_K_GROUND = 199,                 /* K_GROUND  */
  YYSYMBOL_K_CLOCK = 200,                  /* K_CLOCK  */
  YYSYMBOL_K_TIEOFF = 201,                 /* K_TIEOFF  */
  YYSYMBOL_K_ANALOG = 202,                 /* K_ANALOG  */
  YYSYMBOL_K_SCAN = 203,                   /* K_SCAN  */
  YYSYMBOL_K_RESET = 204,                  /* K_RESET  */
  YYSYMBOL_K_RING = 205,                   /* K_RING  */
  YYSYMBOL_K_STRIPE = 206,                 /* K_STRIPE  */
  YYSYMBOL_K_FOLLOWPIN = 207,              /* K_FOLLOWPIN  */
  YYSYMBOL_K_IOWIRE = 208,                 /* K_IOWIRE  */
  YYSYMBOL_K_COREWIRE = 209,               /* K_COREWIRE  */
  YYSYMBOL_K_BLOCKWIRE = 210,              /* K_BLOCKWIRE  */
  YYSYMBOL_K_FILLWIRE = 211,               /* K_FILLWIRE  */
  YYSYMBOL_K_BLOCKAGEWIRE = 212,           /* K_BLOCKAGEWIRE  */
  YYSYMBOL_K_PADRING = 213,                /* K_PADRING  */
  YYSYMBOL_K_BLOCKRING = 214,              /* K_BLOCKRING  */
  YYSYMBOL_K_BLOCKAGES = 215,              /* K_BLOCKAGES  */
  YYSYMBOL_K_PLACEMENT = 216,              /* K_PLACEMENT  */
  YYSYMBOL_K_SLOTS = 217,                  /* K_SLOTS  */
  YYSYMBOL_K_FILLS = 218,                  /* K_FILLS  */
  YYSYMBOL_K_PUSHDOWN = 219,               /* K_PUSHDOWN  */
  YYSYMBOL_K_NETLIST = 220,                /* K_NETLIST  */
  YYSYMBOL_K_DIST = 221,                   /* K_DIST  */
  YYSYMBOL_K_USER = 222,                   /* K_USER  */
  YYSYMBOL_K_TIMING = 223,                 /* K_TIMING  */
  YYSYMBOL_K_BALANCED = 224,               /* K_BALANCED  */
  YYSYMBOL_K_STEINER = 225,                /* K_STEINER  */
  YYSYMBOL_K_TRUNK = 226,                  /* K_TRUNK  */
  YYSYMBOL_K_FIXEDBUMP = 227,              /* K_FIXEDBUMP  */
  YYSYMBOL_K_FENCE = 228,                  /* K_FENCE  */
  YYSYMBOL_K_FREQUENCY = 229,              /* K_FREQUENCY  */
  YYSYMBOL_K_GUIDE = 230,                  /* K_GUIDE  */
  YYSYMBOL_K_MAXBITS = 231,                /* K_MAXBITS  */
  YYSYMBOL_K_PARTITION = 232,              /* K_PARTITION  */
  YYSYMBOL_K_TYPE = 233,                   /* K_TYPE  */
  YYSYMBOL_K_ANTENNAMODEL = 234,           /* K_ANTENNAMODEL  */
  YYSYMBOL_K_DRCFILL = 235,                /* K_DRCFILL  */
  YYSYMBOL_K_OXIDE1 = 236,                 /* K_OXIDE1  */
  YYSYMBOL_K_OXIDE2 = 237,                 /* K_OXIDE2  */
  YYSYMBOL_K_OXIDE3 = 238,                 /* K_OXIDE3  */
  YYSYMBOL_K_OXIDE4 = 239,                 /* K_OXIDE4  */
  YYSYMBOL_K_CUTSIZE = 240,                /* K_CUTSIZE  */
  YYSYMBOL_K_CUTSPACING = 241,             /* K_CUTSPACING  */
  YYSYMBOL_K_DESIGNRULEWIDTH = 242,        /* K_DESIGNRULEWIDTH  */
  YYSYMBOL_K_DIAGWIDTH = 243,              /* K_DIAGWIDTH  */
  YYSYMBOL_K_ENCLOSURE = 244,              /* K_ENCLOSURE  */
  YYSYMBOL_K_HALO = 245,                   /* K_HALO  */
  YYSYMBOL_K_GROUNDSENSITIVITY = 246,      /* K_GROUNDSENSITIVITY  */
  YYSYMBOL_K_HARDSPACING = 247,            /* K_HARDSPACING  */
  YYSYMBOL_K_LAYERS = 248,                 /* K_LAYERS  */
  YYSYMBOL_K_MINCUTS = 249,                /* K_MINCUTS  */
  YYSYMBOL_K_NETEXPR = 250,                /* K_NETEXPR  */
  YYSYMBOL_K_OFFSET = 251,                 /* K_OFFSET  */
  YYSYMBOL_K_ORIGIN = 252,                 /* K_ORIGIN  */
  YYSYMBOL_K_ROWCOL = 253,                 /* K_ROWCOL  */
  YYSYMBOL_K_STYLES = 254,                 /* K_STYLES  */
  YYSYMBOL_K_POLYGON = 255,                /* K_POLYGON  */
  YYSYMBOL_K_PORT = 256,                   /* K_PORT  */
  YYSYMBOL_K_SUPPLYSENSITIVITY = 257,      /* K_SUPPLYSENSITIVITY  */
  YYSYMBOL_K_VIA = 258,                    /* K_VIA  */
  YYSYMBOL_K_VIARULE = 259,                /* K_VIARULE  */
  YYSYMBOL_K_WIREEXT = 260,                /* K_WIREEXT  */
  YYSYMBOL_K_EXCEPTPGNET = 261,            /* K_EXCEPTPGNET  */
  YYSYMBOL_K_FILLWIREOPC = 262,            /* K_FILLWIREOPC  */
  YYSYMBOL_K_OPC = 263,                    /* K_OPC  */
  YYSYMBOL_K_PARTIAL = 264,                /* K_PARTIAL  */
  YYSYMBOL_K_ROUTEHALO = 265,              /* K_ROUTEHALO  */
  YYSYMBOL_266_ = 266,                     /* ';'  */
  YYSYMBOL_267_ = 267,                     /* '-'  */
  YYSYMBOL_268_ = 268,                     /* '+'  */
  YYSYMBOL_269_ = 269,                     /* '('  */
  YYSYMBOL_270_ = 270,                     /* ')'  */
  YYSYMBOL_271_ = 271,                     /* '*'  */
  YYSYMBOL_272_ = 272,                     /* ','  */
  YYSYMBOL_YYACCEPT = 273,                 /* $accept  */
  YYSYMBOL_def_file = 274,                 /* def_file  */
  YYSYMBOL_version_stmt = 275,             /* version_stmt  */
  YYSYMBOL_276_1 = 276,                    /* $@1  */
  YYSYMBOL_case_sens_stmt = 277,           /* case_sens_stmt  */
  YYSYMBOL_rules = 278,                    /* rules  */
  YYSYMBOL_rule = 279,                     /* rule  */
  YYSYMBOL_design_section = 280,           /* design_section  */
  YYSYMBOL_design_name = 281,              /* design_name  */
  YYSYMBOL_282_2 = 282,                    /* $@2  */
  YYSYMBOL_end_design = 283,               /* end_design  */
  YYSYMBOL_tech_name = 284,                /* tech_name  */
  YYSYMBOL_285_3 = 285,                    /* $@3  */
  YYSYMBOL_array_name = 286,               /* array_name  */
  YYSYMBOL_287_4 = 287,                    /* $@4  */
  YYSYMBOL_floorplan_name = 288,           /* floorplan_name  */
  YYSYMBOL_289_5 = 289,                    /* $@5  */
  YYSYMBOL_history = 290,                  /* history  */
  YYSYMBOL_prop_def_section = 291,         /* prop_def_section  */
  YYSYMBOL_292_6 = 292,                    /* $@6  */
  YYSYMBOL_property_defs = 293,            /* property_defs  */
  YYSYMBOL_property_def = 294,             /* property_def  */
  YYSYMBOL_295_7 = 295,                    /* $@7  */
  YYSYMBOL_296_8 = 296,                    /* $@8  */
  YYSYMBOL_297_9 = 297,                    /* $@9  */
  YYSYMBOL_298_10 = 298,                   /* $@10  */
  YYSYMBOL_299_11 = 299,                   /* $@11  */
  YYSYMBOL_300_12 = 300,                   /* $@12  */
  YYSYMBOL_301_13 = 301,                   /* $@13  */
  YYSYMBOL_302_14 = 302,                   /* $@14  */
  YYSYMBOL_303_15 = 303,                   /* $@15  */
  YYSYMBOL_property_type_and_val = 304,    /* property_type_and_val  */
  YYSYMBOL_305_16 = 305,                   /* $@16  */
  YYSYMBOL_306_17 = 306,                   /* $@17  */
  YYSYMBOL_opt_num_val = 307,              /* opt_num_val  */
  YYSYMBOL_units = 308,                    /* units  */
  YYSYMBOL_divider_char = 309,             /* divider_char  */
  YYSYMBOL_bus_bit_chars = 310,            /* bus_bit_chars  */
  YYSYMBOL_canplace = 311,                 /* canplace  */
  YYSYMBOL_312_18 = 312,                   /* $@18  */
  YYSYMBOL_cannotoccupy = 313,             /* cannotoccupy  */
  YYSYMBOL_314_19 = 314,                   /* $@19  */
  YYSYMBOL_orient = 315,                   /* orient  */
  YYSYMBOL_die_area = 316,                 /* die_area  */
  YYSYMBOL_317_20 = 317,                   /* $@20  */
  YYSYMBOL_pin_cap_rule = 318,             /* pin_cap_rule  */
  YYSYMBOL_start_def_cap = 319,            /* start_def_cap  */
  YYSYMBOL_pin_caps = 320,                 /* pin_caps  */
  YYSYMBOL_pin_cap = 321,                  /* pin_cap  */
  YYSYMBOL_end_def_cap = 322,              /* end_def_cap  */
  YYSYMBOL_pin_rule = 323,                 /* pin_rule  */
  YYSYMBOL_start_pins = 324,               /* start_pins  */
  YYSYMBOL_pins = 325,                     /* pins  */
  YYSYMBOL_pin = 326,                      /* pin  */
  YYSYMBOL_327_21 = 327,                   /* $@21  */
  YYSYMBOL_328_22 = 328,                   /* $@22  */
  YYSYMBOL_329_23 = 329,                   /* $@23  */
  YYSYMBOL_pin_options = 330,              /* pin_options  */
  YYSYMBOL_pin_option = 331,               /* pin_option  */
  YYSYMBOL_332_24 = 332,                   /* $@24  */
  YYSYMBOL_333_25 = 333,                   /* $@25  */
  YYSYMBOL_334_26 = 334,                   /* $@26  */
  YYSYMBOL_335_27 = 335,                   /* $@27  */
  YYSYMBOL_336_28 = 336,                   /* $@28  */
  YYSYMBOL_337_29 = 337,                   /* $@29  */
  YYSYMBOL_338_30 = 338,                   /* $@30  */
  YYSYMBOL_339_31 = 339,                   /* $@31  */
  YYSYMBOL_340_32 = 340,                   /* $@32  */
  YYSYMBOL_341_33 = 341,                   /* $@33  */
  YYSYMBOL_pin_layer_mask_opt = 342,       /* pin_layer_mask_opt  */
  YYSYMBOL_pin_via_mask_opt = 343,         /* pin_via_mask_opt  */
  YYSYMBOL_pin_poly_mask_opt = 344,        /* pin_poly_mask_opt  */
  YYSYMBOL_pin_layer_spacing_opt = 345,    /* pin_layer_spacing_opt  */
  YYSYMBOL_pin_poly_spacing_opt = 346,     /* pin_poly_spacing_opt  */
  YYSYMBOL_pin_oxide = 347,                /* pin_oxide  */
  YYSYMBOL_use_type = 348,                 /* use_type  */
  YYSYMBOL_pin_layer_opt = 349,            /* pin_layer_opt  */
  YYSYMBOL_350_34 = 350,                   /* $@34  */
  YYSYMBOL_end_pins = 351,                 /* end_pins  */
  YYSYMBOL_row_rule = 352,                 /* row_rule  */
  YYSYMBOL_353_35 = 353,                   /* $@35  */
  YYSYMBOL_354_36 = 354,                   /* $@36  */
  YYSYMBOL_row_do_option = 355,            /* row_do_option  */
  YYSYMBOL_row_step_option = 356,          /* row_step_option  */
  YYSYMBOL_row_options = 357,              /* row_options  */
  YYSYMBOL_row_option = 358,               /* row_option  */
  YYSYMBOL_359_37 = 359,                   /* $@37  */
  YYSYMBOL_row_prop_list = 360,            /* row_prop_list  */
  YYSYMBOL_row_prop = 361,                 /* row_prop  */
  YYSYMBOL_tracks_rule = 362,              /* tracks_rule  */
  YYSYMBOL_363_38 = 363,                   /* $@38  */
  YYSYMBOL_track_start = 364,              /* track_start  */
  YYSYMBOL_track_type = 365,               /* track_type  */
  YYSYMBOL_track_opts = 366,               /* track_opts  */
  YYSYMBOL_track_mask_statement = 367,     /* track_mask_statement  */
  YYSYMBOL_same_mask = 368,                /* same_mask  */
  YYSYMBOL_track_layer_statement = 369,    /* track_layer_statement  */
  YYSYMBOL_370_39 = 370,                   /* $@39  */
  YYSYMBOL_track_layers = 371,             /* track_layers  */
  YYSYMBOL_track_layer = 372,              /* track_layer  */
  YYSYMBOL_gcellgrid = 373,                /* gcellgrid  */
  YYSYMBOL_extension_section = 374,        /* extension_section  */
  YYSYMBOL_extension_stmt = 375,           /* extension_stmt  */
  YYSYMBOL_via_section = 376,              /* via_section  */
  YYSYMBOL_via = 377,                      /* via  */
  YYSYMBOL_via_declarations = 378,         /* via_declarations  */
  YYSYMBOL_via_declaration = 379,          /* via_declaration  */
  YYSYMBOL_380_40 = 380,                   /* $@40  */
  YYSYMBOL_381_41 = 381,                   /* $@41  */
  YYSYMBOL_layer_stmts = 382,              /* layer_stmts  */
  YYSYMBOL_layer_stmt = 383,               /* layer_stmt  */
  YYSYMBOL_384_42 = 384,                   /* $@42  */
  YYSYMBOL_385_43 = 385,                   /* $@43  */
  YYSYMBOL_386_44 = 386,                   /* $@44  */
  YYSYMBOL_387_45 = 387,                   /* $@45  */
  YYSYMBOL_388_46 = 388,                   /* $@46  */
  YYSYMBOL_389_47 = 389,                   /* $@47  */
  YYSYMBOL_layer_viarule_opts = 390,       /* layer_viarule_opts  */
  YYSYMBOL_391_48 = 391,                   /* $@48  */
  YYSYMBOL_firstPt = 392,                  /* firstPt  */
  YYSYMBOL_nextPt = 393,                   /* nextPt  */
  YYSYMBOL_otherPts = 394,                 /* otherPts  */
  YYSYMBOL_pt = 395,                       /* pt  */
  YYSYMBOL_mask = 396,                     /* mask  */
  YYSYMBOL_via_end = 397,                  /* via_end  */
  YYSYMBOL_regions_section = 398,          /* regions_section  */
  YYSYMBOL_regions_start = 399,            /* regions_start  */
  YYSYMBOL_regions_stmts = 400,            /* regions_stmts  */
  YYSYMBOL_regions_stmt = 401,             /* regions_stmt  */
  YYSYMBOL_402_49 = 402,                   /* $@49  */
  YYSYMBOL_403_50 = 403,                   /* $@50  */
  YYSYMBOL_rect_list = 404,                /* rect_list  */
  YYSYMBOL_region_options = 405,           /* region_options  */
  YYSYMBOL_region_option = 406,            /* region_option  */
  YYSYMBOL_407_51 = 407,                   /* $@51  */
  YYSYMBOL_region_prop_list = 408,         /* region_prop_list  */
  YYSYMBOL_region_prop = 409,              /* region_prop  */
  YYSYMBOL_region_type = 410,              /* region_type  */
  YYSYMBOL_comps_maskShift_section = 411,  /* comps_maskShift_section  */
  YYSYMBOL_comps_section = 412,            /* comps_section  */
  YYSYMBOL_start_comps = 413,              /* start_comps  */
  YYSYMBOL_layer_statement = 414,          /* layer_statement  */
  YYSYMBOL_maskLayer = 415,                /* maskLayer  */
  YYSYMBOL_comps_rule = 416,               /* comps_rule  */
  YYSYMBOL_comp = 417,                     /* comp  */
  YYSYMBOL_comp_start = 418,               /* comp_start  */
  YYSYMBOL_comp_id_and_name = 419,         /* comp_id_and_name  */
  YYSYMBOL_420_52 = 420,                   /* $@52  */
  YYSYMBOL_comp_net_list = 421,            /* comp_net_list  */
  YYSYMBOL_comp_options = 422,             /* comp_options  */
  YYSYMBOL_comp_option = 423,              /* comp_option  */
  YYSYMBOL_comp_extension_stmt = 424,      /* comp_extension_stmt  */
  YYSYMBOL_comp_eeq = 425,                 /* comp_eeq  */
  YYSYMBOL_426_53 = 426,                   /* $@53  */
  YYSYMBOL_comp_generate = 427,            /* comp_generate  */
  YYSYMBOL_428_54 = 428,                   /* $@54  */
  YYSYMBOL_opt_pattern = 429,              /* opt_pattern  */
  YYSYMBOL_comp_source = 430,              /* comp_source  */
  YYSYMBOL_source_type = 431,              /* source_type  */
  YYSYMBOL_comp_region = 432,              /* comp_region  */
  YYSYMBOL_comp_pnt_list = 433,            /* comp_pnt_list  */
  YYSYMBOL_comp_halo = 434,                /* comp_halo  */
  YYSYMBOL_435_55 = 435,                   /* $@55  */
  YYSYMBOL_halo_soft = 436,                /* halo_soft  */
  YYSYMBOL_comp_routehalo = 437,           /* comp_routehalo  */
  YYSYMBOL_438_56 = 438,                   /* $@56  */
  YYSYMBOL_comp_property = 439,            /* comp_property  */
  YYSYMBOL_440_57 = 440,                   /* $@57  */
  YYSYMBOL_comp_prop_list = 441,           /* comp_prop_list  */
  YYSYMBOL_comp_prop = 442,                /* comp_prop  */
  YYSYMBOL_comp_region_start = 443,        /* comp_region_start  */
  YYSYMBOL_comp_foreign = 444,             /* comp_foreign  */
  YYSYMBOL_445_58 = 445,                   /* $@58  */
  YYSYMBOL_opt_paren = 446,                /* opt_paren  */
  YYSYMBOL_comp_type = 447,                /* comp_type  */
  YYSYMBOL_maskShift = 448,                /* maskShift  */
  YYSYMBOL_placement_status = 449,         /* placement_status  */
  YYSYMBOL_weight = 450,                   /* weight  */
  YYSYMBOL_end_comps = 451,                /* end_comps  */
  YYSYMBOL_nets_section = 452,             /* nets_section  */
  YYSYMBOL_start_nets = 453,               /* start_nets  */
  YYSYMBOL_net_rules = 454,                /* net_rules  */
  YYSYMBOL_one_net = 455,                  /* one_net  */
  YYSYMBOL_net_and_connections = 456,      /* net_and_connections  */
  YYSYMBOL_net_start = 457,                /* net_start  */
  YYSYMBOL_458_59 = 458,                   /* $@59  */
  YYSYMBOL_net_name = 459,                 /* net_name  */
  YYSYMBOL_460_60 = 460,                   /* $@60  */
  YYSYMBOL_461_61 = 461,                   /* $@61  */
  YYSYMBOL_net_connections = 462,          /* net_connections  */
  YYSYMBOL_net_connection = 463,           /* net_connection  */
  YYSYMBOL_464_62 = 464,                   /* $@62  */
  YYSYMBOL_465_63 = 465,                   /* $@63  */
  YYSYMBOL_466_64 = 466,                   /* $@64  */
  YYSYMBOL_conn_opt = 467,                 /* conn_opt  */
  YYSYMBOL_net_options = 468,              /* net_options  */
  YYSYMBOL_net_option = 469,               /* net_option  */
  YYSYMBOL_470_65 = 470,                   /* $@65  */
  YYSYMBOL_471_66 = 471,                   /* $@66  */
  YYSYMBOL_472_67 = 472,                   /* $@67  */
  YYSYMBOL_473_68 = 473,                   /* $@68  */
  YYSYMBOL_474_69 = 474,                   /* $@69  */
  YYSYMBOL_475_70 = 475,                   /* $@70  */
  YYSYMBOL_476_71 = 476,                   /* $@71  */
  YYSYMBOL_477_72 = 477,                   /* $@72  */
  YYSYMBOL_478_73 = 478,                   /* $@73  */
  YYSYMBOL_479_74 = 479,                   /* $@74  */
  YYSYMBOL_480_75 = 480,                   /* $@75  */
  YYSYMBOL_net_prop_list = 481,            /* net_prop_list  */
  YYSYMBOL_net_prop = 482,                 /* net_prop  */
  YYSYMBOL_netsource_type = 483,           /* netsource_type  */
  YYSYMBOL_vpin_stmt = 484,                /* vpin_stmt  */
  YYSYMBOL_485_76 = 485,                   /* $@76  */
  YYSYMBOL_vpin_begin = 486,               /* vpin_begin  */
  YYSYMBOL_487_77 = 487,                   /* $@77  */
  YYSYMBOL_vpin_layer_opt = 488,           /* vpin_layer_opt  */
  YYSYMBOL_489_78 = 489,                   /* $@78  */
  YYSYMBOL_vpin_options = 490,             /* vpin_options  */
  YYSYMBOL_vpin_status = 491,              /* vpin_status  */
  YYSYMBOL_net_type = 492,                 /* net_type  */
  YYSYMBOL_paths = 493,                    /* paths  */
  YYSYMBOL_new_path = 494,                 /* new_path  */
  YYSYMBOL_495_79 = 495,                   /* $@79  */
  YYSYMBOL_path = 496,                     /* path  */
  YYSYMBOL_497_80 = 497,                   /* $@80  */
  YYSYMBOL_498_81 = 498,                   /* $@81  */
  YYSYMBOL_virtual_statement = 499,        /* virtual_statement  */
  YYSYMBOL_rect_statement = 500,           /* rect_statement  */
  YYSYMBOL_path_item_list = 501,           /* path_item_list  */
  YYSYMBOL_path_item = 502,                /* path_item  */
  YYSYMBOL_503_82 = 503,                   /* $@82  */
  YYSYMBOL_504_83 = 504,                   /* $@83  */
  YYSYMBOL_path_pt = 505,                  /* path_pt  */
  YYSYMBOL_virtual_pt = 506,               /* virtual_pt  */
  YYSYMBOL_rect_pts = 507,                 /* rect_pts  */
  YYSYMBOL_opt_taper_style_s = 508,        /* opt_taper_style_s  */
  YYSYMBOL_opt_taper_style = 509,          /* opt_taper_style  */
  YYSYMBOL_opt_taper = 510,                /* opt_taper  */
  YYSYMBOL_511_84 = 511,                   /* $@84  */
  YYSYMBOL_opt_style = 512,                /* opt_style  */
  YYSYMBOL_opt_spaths = 513,               /* opt_spaths  */
  YYSYMBOL_opt_shape_style = 514,          /* opt_shape_style  */
  YYSYMBOL_end_nets = 515,                 /* end_nets  */
  YYSYMBOL_shape_type = 516,               /* shape_type  */
  YYSYMBOL_snets_section = 517,            /* snets_section  */
  YYSYMBOL_snet_rules = 518,               /* snet_rules  */
  YYSYMBOL_snet_rule = 519,                /* snet_rule  */
  YYSYMBOL_snet_options = 520,             /* snet_options  */
  YYSYMBOL_snet_option = 521,              /* snet_option  */
  YYSYMBOL_snet_other_option = 522,        /* snet_other_option  */
  YYSYMBOL_523_85 = 523,                   /* $@85  */
  YYSYMBOL_524_86 = 524,                   /* $@86  */
  YYSYMBOL_525_87 = 525,                   /* $@87  */
  YYSYMBOL_526_88 = 526,                   /* $@88  */
  YYSYMBOL_527_89 = 527,                   /* $@89  */
  YYSYMBOL_528_90 = 528,                   /* $@90  */
  YYSYMBOL_529_91 = 529,                   /* $@91  */
  YYSYMBOL_530_92 = 530,                   /* $@92  */
  YYSYMBOL_531_93 = 531,                   /* $@93  */
  YYSYMBOL_532_94 = 532,                   /* $@94  */
  YYSYMBOL_orient_pt = 533,                /* orient_pt  */
  YYSYMBOL_shield_layer = 534,             /* shield_layer  */
  YYSYMBOL_535_95 = 535,                   /* $@95  */
  YYSYMBOL_snet_width = 536,               /* snet_width  */
  YYSYMBOL_537_96 = 537,                   /* $@96  */
  YYSYMBOL_snet_voltage = 538,             /* snet_voltage  */
  YYSYMBOL_539_97 = 539,                   /* $@97  */
  YYSYMBOL_snet_spacing = 540,             /* snet_spacing  */
  YYSYMBOL_541_98 = 541,                   /* $@98  */
  YYSYMBOL_542_99 = 542,                   /* $@99  */
  YYSYMBOL_snet_prop_list = 543,           /* snet_prop_list  */
  YYSYMBOL_snet_prop = 544,                /* snet_prop  */
  YYSYMBOL_opt_snet_range = 545,           /* opt_snet_range  */
  YYSYMBOL_opt_range = 546,                /* opt_range  */
  YYSYMBOL_pattern_type = 547,             /* pattern_type  */
  YYSYMBOL_spaths = 548,                   /* spaths  */
  YYSYMBOL_snew_path = 549,                /* snew_path  */
  YYSYMBOL_550_100 = 550,                  /* $@100  */
  YYSYMBOL_spath = 551,                    /* spath  */
  YYSYMBOL_552_101 = 552,                  /* $@101  */
  YYSYMBOL_553_102 = 553,                  /* $@102  */
  YYSYMBOL_width = 554,                    /* width  */
  YYSYMBOL_start_snets = 555,              /* start_snets  */
  YYSYMBOL_end_snets = 556,                /* end_snets  */
  YYSYMBOL_groups_section = 557,           /* groups_section  */
  YYSYMBOL_groups_start = 558,             /* groups_start  */
  YYSYMBOL_group_rules = 559,              /* group_rules  */
  YYSYMBOL_group_rule = 560,               /* group_rule  */
  YYSYMBOL_start_group = 561,              /* start_group  */
  YYSYMBOL_562_103 = 562,                  /* $@103  */
  YYSYMBOL_group_members = 563,            /* group_members  */
  YYSYMBOL_group_member = 564,             /* group_member  */
  YYSYMBOL_group_options = 565,            /* group_options  */
  YYSYMBOL_group_option = 566,             /* group_option  */
  YYSYMBOL_567_104 = 567,                  /* $@104  */
  YYSYMBOL_568_105 = 568,                  /* $@105  */
  YYSYMBOL_group_region = 569,             /* group_region  */
  YYSYMBOL_group_prop_list = 570,          /* group_prop_list  */
  YYSYMBOL_group_prop = 571,               /* group_prop  */
  YYSYMBOL_group_soft_options = 572,       /* group_soft_options  */
  YYSYMBOL_group_soft_option = 573,        /* group_soft_option  */
  YYSYMBOL_groups_end = 574,               /* groups_end  */
  YYSYMBOL_assertions_section = 575,       /* assertions_section  */
  YYSYMBOL_constraint_section = 576,       /* constraint_section  */
  YYSYMBOL_assertions_start = 577,         /* assertions_start  */
  YYSYMBOL_constraints_start = 578,        /* constraints_start  */
  YYSYMBOL_constraint_rules = 579,         /* constraint_rules  */
  YYSYMBOL_constraint_rule = 580,          /* constraint_rule  */
  YYSYMBOL_operand_rule = 581,             /* operand_rule  */
  YYSYMBOL_operand = 582,                  /* operand  */
  YYSYMBOL_583_106 = 583,                  /* $@106  */
  YYSYMBOL_584_107 = 584,                  /* $@107  */
  YYSYMBOL_operand_list = 585,             /* operand_list  */
  YYSYMBOL_wiredlogic_rule = 586,          /* wiredlogic_rule  */
  YYSYMBOL_587_108 = 587,                  /* $@108  */
  YYSYMBOL_opt_plus = 588,                 /* opt_plus  */
  YYSYMBOL_delay_specs = 589,              /* delay_specs  */
  YYSYMBOL_delay_spec = 590,               /* delay_spec  */
  YYSYMBOL_constraints_end = 591,          /* constraints_end  */
  YYSYMBOL_assertions_end = 592,           /* assertions_end  */
  YYSYMBOL_scanchains_section = 593,       /* scanchains_section  */
  YYSYMBOL_scanchain_start = 594,          /* scanchain_start  */
  YYSYMBOL_scanchain_rules = 595,          /* scanchain_rules  */
  YYSYMBOL_scan_rule = 596,                /* scan_rule  */
  YYSYMBOL_start_scan = 597,               /* start_scan  */
  YYSYMBOL_598_109 = 598,                  /* $@109  */
  YYSYMBOL_scan_members = 599,             /* scan_members  */
  YYSYMBOL_opt_pin = 600,                  /* opt_pin  */
  YYSYMBOL_scan_member = 601,              /* scan_member  */
  YYSYMBOL_602_110 = 602,                  /* $@110  */
  YYSYMBOL_603_111 = 603,                  /* $@111  */
  YYSYMBOL_604_112 = 604,                  /* $@112  */
  YYSYMBOL_605_113 = 605,                  /* $@113  */
  YYSYMBOL_606_114 = 606,                  /* $@114  */
  YYSYMBOL_607_115 = 607,                  /* $@115  */
  YYSYMBOL_opt_common_pins = 608,          /* opt_common_pins  */
  YYSYMBOL_floating_inst_list = 609,       /* floating_inst_list  */
  YYSYMBOL_one_floating_inst = 610,        /* one_floating_inst  */
  YYSYMBOL_611_116 = 611,                  /* $@116  */
  YYSYMBOL_floating_pins = 612,            /* floating_pins  */
  YYSYMBOL_ordered_inst_list = 613,        /* ordered_inst_list  */
  YYSYMBOL_one_ordered_inst = 614,         /* one_ordered_inst  */
  YYSYMBOL_615_117 = 615,                  /* $@117  */
  YYSYMBOL_ordered_pins = 616,             /* ordered_pins  */
  YYSYMBOL_partition_maxbits = 617,        /* partition_maxbits  */
  YYSYMBOL_scanchain_end = 618,            /* scanchain_end  */
  YYSYMBOL_iotiming_section = 619,         /* iotiming_section  */
  YYSYMBOL_iotiming_start = 620,           /* iotiming_start  */
  YYSYMBOL_iotiming_rules = 621,           /* iotiming_rules  */
  YYSYMBOL_iotiming_rule = 622,            /* iotiming_rule  */
  YYSYMBOL_start_iotiming = 623,           /* start_iotiming  */
  YYSYMBOL_624_118 = 624,                  /* $@118  */
  YYSYMBOL_iotiming_members = 625,         /* iotiming_members  */
  YYSYMBOL_iotiming_member = 626,          /* iotiming_member  */
  YYSYMBOL_627_119 = 627,                  /* $@119  */
  YYSYMBOL_628_120 = 628,                  /* $@120  */
  YYSYMBOL_iotiming_drivecell_opt = 629,   /* iotiming_drivecell_opt  */
  YYSYMBOL_630_121 = 630,                  /* $@121  */
  YYSYMBOL_631_122 = 631,                  /* $@122  */
  YYSYMBOL_iotiming_frompin = 632,         /* iotiming_frompin  */
  YYSYMBOL_633_123 = 633,                  /* $@123  */
  YYSYMBOL_iotiming_parallel = 634,        /* iotiming_parallel  */
  YYSYMBOL_risefall = 635,                 /* risefall  */
  YYSYMBOL_iotiming_end = 636,             /* iotiming_end  */
  YYSYMBOL_floorplan_contraints_section = 637, /* floorplan_contraints_section  */
  YYSYMBOL_fp_start = 638,                 /* fp_start  */
  YYSYMBOL_fp_stmts = 639,                 /* fp_stmts  */
  YYSYMBOL_fp_stmt = 640,                  /* fp_stmt  */
  YYSYMBOL_641_124 = 641,                  /* $@124  */
  YYSYMBOL_642_125 = 642,                  /* $@125  */
  YYSYMBOL_h_or_v = 643,                   /* h_or_v  */
  YYSYMBOL_constraint_type = 644,          /* constraint_type  */
  YYSYMBOL_constrain_what_list = 645,      /* constrain_what_list  */
  YYSYMBOL_constrain_what = 646,           /* constrain_what  */
  YYSYMBOL_647_126 = 647,                  /* $@126  */
  YYSYMBOL_648_127 = 648,                  /* $@127  */
  YYSYMBOL_row_or_comp_list = 649,         /* row_or_comp_list  */
  YYSYMBOL_row_or_comp = 650,              /* row_or_comp  */
  YYSYMBOL_651_128 = 651,                  /* $@128  */
  YYSYMBOL_652_129 = 652,                  /* $@129  */
  YYSYMBOL_timingdisables_section = 653,   /* timingdisables_section  */
  YYSYMBOL_timingdisables_start = 654,     /* timingdisables_start  */
  YYSYMBOL_timingdisables_rules = 655,     /* timingdisables_rules  */
  YYSYMBOL_timingdisables_rule = 656,      /* timingdisables_rule  */
  YYSYMBOL_657_130 = 657,                  /* $@130  */
  YYSYMBOL_658_131 = 658,                  /* $@131  */
  YYSYMBOL_659_132 = 659,                  /* $@132  */
  YYSYMBOL_660_133 = 660,                  /* $@133  */
  YYSYMBOL_td_macro_option = 661,          /* td_macro_option  */
  YYSYMBOL_662_134 = 662,                  /* $@134  */
  YYSYMBOL_663_135 = 663,                  /* $@135  */
  YYSYMBOL_664_136 = 664,                  /* $@136  */
  YYSYMBOL_timingdisables_end = 665,       /* timingdisables_end  */
  YYSYMBOL_partitions_section = 666,       /* partitions_section  */
  YYSYMBOL_partitions_start = 667,         /* partitions_start  */
  YYSYMBOL_partition_rules = 668,          /* partition_rules  */
  YYSYMBOL_partition_rule = 669,           /* partition_rule  */
  YYSYMBOL_start_partition = 670,          /* start_partition  */
  YYSYMBOL_671_137 = 671,                  /* $@137  */
  YYSYMBOL_turnoff = 672,                  /* turnoff  */
  YYSYMBOL_turnoff_setup = 673,            /* turnoff_setup  */
  YYSYMBOL_turnoff_hold = 674,             /* turnoff_hold  */
  YYSYMBOL_partition_members = 675,        /* partition_members  */
  YYSYMBOL_partition_member = 676,         /* partition_member  */
  YYSYMBOL_677_138 = 677,                  /* $@138  */
  YYSYMBOL_678_139 = 678,                  /* $@139  */
  YYSYMBOL_679_140 = 679,                  /* $@140  */
  YYSYMBOL_680_141 = 680,                  /* $@141  */
  YYSYMBOL_681_142 = 681,                  /* $@142  */
  YYSYMBOL_682_143 = 682,                  /* $@143  */
  YYSYMBOL_minmaxpins = 683,               /* minmaxpins  */
  YYSYMBOL_684_144 = 684,                  /* $@144  */
  YYSYMBOL_min_or_max_list = 685,          /* min_or_max_list  */
  YYSYMBOL_min_or_max_member = 686,        /* min_or_max_member  */
  YYSYMBOL_pin_list = 687,                 /* pin_list  */
  YYSYMBOL_risefallminmax1_list = 688,     /* risefallminmax1_list  */
  YYSYMBOL_risefallminmax1 = 689,          /* risefallminmax1  */
  YYSYMBOL_risefallminmax2_list = 690,     /* risefallminmax2_list  */
  YYSYMBOL_risefallminmax2 = 691,          /* risefallminmax2  */
  YYSYMBOL_partitions_end = 692,           /* partitions_end  */
  YYSYMBOL_comp_names = 693,               /* comp_names  */
  YYSYMBOL_comp_name = 694,                /* comp_name  */
  YYSYMBOL_695_145 = 695,                  /* $@145  */
  YYSYMBOL_subnet_opt_syn = 696,           /* subnet_opt_syn  */
  YYSYMBOL_subnet_options = 697,           /* subnet_options  */
  YYSYMBOL_subnet_option = 698,            /* subnet_option  */
  YYSYMBOL_699_146 = 699,                  /* $@146  */
  YYSYMBOL_700_147 = 700,                  /* $@147  */
  YYSYMBOL_subnet_type = 701,              /* subnet_type  */
  YYSYMBOL_pin_props_section = 702,        /* pin_props_section  */
  YYSYMBOL_begin_pin_props = 703,          /* begin_pin_props  */
  YYSYMBOL_opt_semi = 704,                 /* opt_semi  */
  YYSYMBOL_end_pin_props = 705,            /* end_pin_props  */
  YYSYMBOL_pin_prop_list = 706,            /* pin_prop_list  */
  YYSYMBOL_pin_prop_terminal = 707,        /* pin_prop_terminal  */
  YYSYMBOL_708_148 = 708,                  /* $@148  */
  YYSYMBOL_709_149 = 709,                  /* $@149  */
  YYSYMBOL_pin_prop_options = 710,         /* pin_prop_options  */
  YYSYMBOL_pin_prop = 711,                 /* pin_prop  */
  YYSYMBOL_712_150 = 712,                  /* $@150  */
  YYSYMBOL_pin_prop_name_value_list = 713, /* pin_prop_name_value_list  */
  YYSYMBOL_pin_prop_name_value = 714,      /* pin_prop_name_value  */
  YYSYMBOL_blockage_section = 715,         /* blockage_section  */
  YYSYMBOL_blockage_start = 716,           /* blockage_start  */
  YYSYMBOL_blockage_end = 717,             /* blockage_end  */
  YYSYMBOL_blockage_defs = 718,            /* blockage_defs  */
  YYSYMBOL_blockage_def = 719,             /* blockage_def  */
  YYSYMBOL_blockage_rule = 720,            /* blockage_rule  */
  YYSYMBOL_721_151 = 721,                  /* $@151  */
  YYSYMBOL_722_152 = 722,                  /* $@152  */
  YYSYMBOL_723_153 = 723,                  /* $@153  */
  YYSYMBOL_layer_blockage_rules = 724,     /* layer_blockage_rules  */
  YYSYMBOL_layer_blockage_rule = 725,      /* layer_blockage_rule  */
  YYSYMBOL_mask_blockage_rule = 726,       /* mask_blockage_rule  */
  YYSYMBOL_comp_blockage_rule = 727,       /* comp_blockage_rule  */
  YYSYMBOL_728_154 = 728,                  /* $@154  */
  YYSYMBOL_placement_comp_rules = 729,     /* placement_comp_rules  */
  YYSYMBOL_placement_comp_rule = 730,      /* placement_comp_rule  */
  YYSYMBOL_731_155 = 731,                  /* $@155  */
  YYSYMBOL_rectPoly_blockage_rules = 732,  /* rectPoly_blockage_rules  */
  YYSYMBOL_rectPoly_blockage = 733,        /* rectPoly_blockage  */
  YYSYMBOL_734_156 = 734,                  /* $@156  */
  YYSYMBOL_slot_section = 735,             /* slot_section  */
  YYSYMBOL_slot_start = 736,               /* slot_start  */
  YYSYMBOL_slot_end = 737,                 /* slot_end  */
  YYSYMBOL_slot_defs = 738,                /* slot_defs  */
  YYSYMBOL_slot_def = 739,                 /* slot_def  */
  YYSYMBOL_slot_rule = 740,                /* slot_rule  */
  YYSYMBOL_741_157 = 741,                  /* $@157  */
  YYSYMBOL_742_158 = 742,                  /* $@158  */
  YYSYMBOL_geom_slot_rules = 743,          /* geom_slot_rules  */
  YYSYMBOL_geom_slot = 744,                /* geom_slot  */
  YYSYMBOL_745_159 = 745,                  /* $@159  */
  YYSYMBOL_fill_section = 746,             /* fill_section  */
  YYSYMBOL_fill_start = 747,               /* fill_start  */
  YYSYMBOL_fill_end = 748,                 /* fill_end  */
  YYSYMBOL_fill_defs = 749,                /* fill_defs  */
  YYSYMBOL_fill_def = 750,                 /* fill_def  */
  YYSYMBOL_751_160 = 751,                  /* $@160  */
  YYSYMBOL_752_161 = 752,                  /* $@161  */
  YYSYMBOL_fill_rule = 753,                /* fill_rule  */
  YYSYMBOL_754_162 = 754,                  /* $@162  */
  YYSYMBOL_755_163 = 755,                  /* $@163  */
  YYSYMBOL_geom_fill_rules = 756,          /* geom_fill_rules  */
  YYSYMBOL_geom_fill = 757,                /* geom_fill  */
  YYSYMBOL_758_164 = 758,                  /* $@164  */
  YYSYMBOL_fill_layer_mask_opc_opt = 759,  /* fill_layer_mask_opc_opt  */
  YYSYMBOL_opt_mask_opc_l = 760,           /* opt_mask_opc_l  */
  YYSYMBOL_fill_layer_opc = 761,           /* fill_layer_opc  */
  YYSYMBOL_fill_via_pt = 762,              /* fill_via_pt  */
  YYSYMBOL_fill_via_mask_opc_opt = 763,    /* fill_via_mask_opc_opt  */
  YYSYMBOL_opt_mask_opc = 764,             /* opt_mask_opc  */
  YYSYMBOL_fill_via_opc = 765,             /* fill_via_opc  */
  YYSYMBOL_fill_mask = 766,                /* fill_mask  */
  YYSYMBOL_fill_viaMask = 767,             /* fill_viaMask  */
  YYSYMBOL_nondefaultrule_section = 768,   /* nondefaultrule_section  */
  YYSYMBOL_nondefault_start = 769,         /* nondefault_start  */
  YYSYMBOL_nondefault_end = 770,           /* nondefault_end  */
  YYSYMBOL_nondefault_defs = 771,          /* nondefault_defs  */
  YYSYMBOL_nondefault_def = 772,           /* nondefault_def  */
  YYSYMBOL_773_165 = 773,                  /* $@165  */
  YYSYMBOL_774_166 = 774,                  /* $@166  */
  YYSYMBOL_nondefault_options = 775,       /* nondefault_options  */
  YYSYMBOL_nondefault_option = 776,        /* nondefault_option  */
  YYSYMBOL_777_167 = 777,                  /* $@167  */
  YYSYMBOL_778_168 = 778,                  /* $@168  */
  YYSYMBOL_779_169 = 779,                  /* $@169  */
  YYSYMBOL_780_170 = 780,                  /* $@170  */
  YYSYMBOL_781_171 = 781,                  /* $@171  */
  YYSYMBOL_nondefault_layer_options = 782, /* nondefault_layer_options  */
  YYSYMBOL_nondefault_layer_option = 783,  /* nondefault_layer_option  */
  YYSYMBOL_nondefault_prop_opt = 784,      /* nondefault_prop_opt  */
  YYSYMBOL_785_172 = 785,                  /* $@172  */
  YYSYMBOL_nondefault_prop_list = 786,     /* nondefault_prop_list  */
  YYSYMBOL_nondefault_prop = 787,          /* nondefault_prop  */
  YYSYMBOL_styles_section = 788,           /* styles_section  */
  YYSYMBOL_styles_start = 789,             /* styles_start  */
  YYSYMBOL_styles_end = 790,               /* styles_end  */
  YYSYMBOL_styles_rules = 791,             /* styles_rules  */
  YYSYMBOL_styles_rule = 792,              /* styles_rule  */
  YYSYMBOL_793_173 = 793                   /* $@173  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_int16 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if !defined yyoverflow

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* !defined yyoverflow */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  5
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1483

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  273
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  521
/* YYNRULES -- Number of rules.  */
#define YYNRULES  955
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  1665

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   520


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int16 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     269,   270,   271,   268,   272,   267,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,   266,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,   129,   130,   131,   132,   133,   134,
     135,   136,   137,   138,   139,   140,   141,   142,   143,   144,
     145,   146,   147,   148,   149,   150,   151,   152,   153,   154,
     155,   156,   157,   158,   159,   160,   161,   162,   163,   164,
     165,   166,   167,   168,   169,   170,   171,   172,   173,   174,
     175,   176,   177,   178,   179,   180,   181,   182,   183,   184,
     185,   186,   187,   188,   189,   190,   191,   192,   193,   194,
     195,   196,   197,   198,   199,   200,   201,   202,   203,   204,
     205,   206,   207,   208,   209,   210,   211,   212,   213,   214,
     215,   216,   217,   218,   219,   220,   221,   222,   223,   224,
     225,   226,   227,   228,   229,   230,   231,   232,   233,   234,
     235,   236,   237,   238,   239,   240,   241,   242,   243,   244,
     245,   246,   247,   248,   249,   250,   251,   252,   253,   254,
     255,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   315,   315,   318,   319,   319,   345,   346,   359,   377,
     378,   379,   382,   382,   382,   382,   383,   383,   383,   383,
     384,   384,   384,   385,   385,   385,   386,   386,   386,   387,
     387,   387,   387,   388,   391,   391,   391,   391,   392,   392,
     392,   393,   393,   393,   394,   394,   394,   395,   395,   395,
     395,   400,   400,   407,   432,   432,   438,   438,   444,   444,
     450,   457,   456,   472,   473,   476,   476,   485,   485,   494,
     494,   503,   503,   512,   512,   521,   521,   530,   530,   541,
     540,   551,   550,   571,   573,   573,   578,   578,   584,   589,
     594,   600,   601,   604,   612,   619,   626,   626,   638,   638,
     651,   652,   653,   654,   655,   656,   657,   658,   661,   660,
     673,   676,   688,   689,   692,   703,   706,   709,   715,   716,
     719,   720,   721,   719,   733,   734,   736,   742,   748,   754,
     775,   775,   795,   795,   815,   819,   840,   841,   840,   859,
     860,   859,   895,   895,   921,   932,   950,   968,   986,  1004,
    1004,  1022,  1022,  1041,  1059,  1059,  1077,  1094,  1095,  1109,
    1110,  1117,  1118,  1131,  1132,  1155,  1179,  1180,  1203,  1227,
    1232,  1237,  1242,  1248,  1250,  1252,  1254,  1256,  1258,  1260,
    1262,  1267,  1268,  1268,  1271,  1277,  1279,  1277,  1293,  1303,
    1341,  1344,  1352,  1353,  1356,  1356,  1360,  1361,  1364,  1376,
    1385,  1396,  1395,  1429,  1434,  1436,  1439,  1441,  1442,  1453,
    1454,  1457,  1458,  1458,  1461,  1462,  1465,  1471,  1500,  1506,
    1509,  1512,  1518,  1519,  1522,  1523,  1522,  1534,  1535,  1538,
    1538,  1545,  1546,  1545,  1572,  1572,  1582,  1584,  1582,  1607,
    1608,  1614,  1626,  1638,  1650,  1650,  1663,  1666,  1669,  1670,
    1673,  1680,  1686,  1692,  1699,  1700,  1703,  1709,  1715,  1721,
    1722,  1725,  1726,  1725,  1735,  1738,  1743,  1744,  1747,  1747,
    1750,  1765,  1766,  1769,  1784,  1793,  1803,  1805,  1808,  1825,
    1828,  1835,  1836,  1839,  1846,  1847,  1850,  1856,  1862,  1862,
    1870,  1871,  1876,  1882,  1883,  1886,  1886,  1886,  1886,  1886,
    1887,  1887,  1887,  1887,  1888,  1888,  1888,  1891,  1898,  1898,
    1904,  1904,  1912,  1913,  1916,  1922,  1924,  1926,  1928,  1933,
    1935,  1941,  1952,  1965,  1964,  1986,  1987,  2007,  2007,  2027,
    2027,  2031,  2032,  2035,  2046,  2055,  2065,  2068,  2068,  2083,
    2085,  2088,  2095,  2102,  2116,  2125,  2127,  2129,  2132,  2138,
    2144,  2147,  2154,  2155,  2158,  2169,  2173,  2173,  2176,  2175,
    2184,  2184,  2192,  2193,  2196,  2196,  2210,  2210,  2217,  2217,
    2226,  2227,  2234,  2239,  2240,  2244,  2243,  2262,  2265,  2282,
    2282,  2300,  2300,  2303,  2306,  2309,  2312,  2315,  2318,  2321,
    2321,  2332,  2334,  2334,  2337,  2338,  2337,  2384,  2390,  2400,
    2383,  2414,  2414,  2418,  2424,  2425,  2428,  2439,  2448,  2458,
    2460,  2462,  2464,  2466,  2470,  2469,  2480,  2480,  2483,  2484,
    2484,  2487,  2488,  2491,  2493,  2495,  2498,  2500,  2502,  2506,
    2510,  2513,  2513,  2519,  2538,  2518,  2552,  2569,  2586,  2587,
    2592,  2603,  2617,  2624,  2635,  2659,  2693,  2727,  2752,  2753,
    2754,  2754,  2765,  2764,  2774,  2785,  2793,  2800,  2807,  2813,
    2821,  2829,  2837,  2846,  2854,  2861,  2870,  2879,  2880,  2882,
    2883,  2886,  2890,  2890,  2895,  2915,  2916,  2920,  2924,  2944,
    2950,  2952,  2954,  2956,  2958,  2960,  2962,  2964,  2980,  2982,
    2984,  2986,  2989,  2992,  2993,  2996,  2999,  3000,  3003,  3003,
    3004,  3004,  3007,  3021,  3020,  3042,  3043,  3042,  3049,  3053,
    3059,  3060,  3059,  3100,  3100,  3133,  3134,  3133,  3166,  3169,
    3172,  3175,  3175,  3178,  3181,  3184,  3193,  3196,  3199,  3199,
    3203,  3207,  3208,  3209,  3210,  3211,  3212,  3213,  3214,  3215,
    3218,  3232,  3232,  3287,  3287,  3296,  3296,  3314,  3315,  3314,
    3322,  3323,  3326,  3338,  3347,  3357,  3358,  3363,  3364,  3367,
    3369,  3371,  3373,  3377,  3394,  3397,  3397,  3416,  3424,  3415,
    3432,  3437,  3444,  3451,  3454,  3460,  3461,  3464,  3470,  3470,
    3481,  3482,  3485,  3492,  3493,  3496,  3498,  3498,  3501,  3501,
    3503,  3509,  3519,  3524,  3525,  3528,  3539,  3548,  3558,  3559,
    3562,  3570,  3578,  3587,  3594,  3598,  3601,  3615,  3629,  3630,
    3633,  3634,  3644,  3657,  3657,  3662,  3662,  3667,  3672,  3678,
    3679,  3681,  3683,  3683,  3692,  3693,  3696,  3697,  3700,  3705,
    3710,  3715,  3721,  3732,  3743,  3746,  3752,  3753,  3756,  3762,
    3762,  3769,  3770,  3775,  3776,  3779,  3779,  3783,  3783,  3786,
    3785,  3794,  3794,  3798,  3798,  3800,  3800,  3818,  3825,  3826,
    3835,  3849,  3850,  3854,  3853,  3864,  3865,  3878,  3899,  3930,
    3931,  3935,  3934,  3943,  3944,  3957,  3978,  4010,  4011,  4014,
    4023,  4026,  4037,  4038,  4041,  4047,  4047,  4053,  4054,  4058,
    4063,  4068,  4073,  4074,  4073,  4082,  4089,  4090,  4088,  4096,
    4097,  4097,  4103,  4104,  4110,  4110,  4112,  4118,  4124,  4130,
    4131,  4134,  4135,  4134,  4139,  4141,  4144,  4146,  4148,  4150,
    4153,  4154,  4158,  4157,  4161,  4160,  4165,  4166,  4168,  4168,
    4170,  4170,  4173,  4177,  4184,  4185,  4188,  4189,  4188,  4197,
    4197,  4205,  4205,  4213,  4219,  4220,  4219,  4225,  4225,  4231,
    4238,  4241,  4248,  4249,  4252,  4258,  4258,  4264,  4265,  4272,
    4273,  4275,  4279,  4280,  4282,  4285,  4286,  4289,  4289,  4295,
    4295,  4301,  4301,  4307,  4307,  4313,  4313,  4319,  4319,  4324,
    4332,  4331,  4335,  4336,  4339,  4344,  4350,  4351,  4354,  4355,
    4357,  4359,  4361,  4363,  4367,  4368,  4371,  4374,  4377,  4380,
    4384,  4388,  4389,  4392,  4392,  4401,  4402,  4405,  4406,  4409,
    4408,  4421,  4421,  4424,  4426,  4428,  4430,  4433,  4435,  4441,
    4442,  4445,  4449,  4450,  4453,  4454,  4453,  4463,  4464,  4466,
    4466,  4470,  4471,  4474,  4485,  4494,  4504,  4506,  4510,  4514,
    4515,  4518,  4527,  4528,  4527,  4547,  4546,  4563,  4564,  4567,
    4593,  4615,  4616,  4619,  4628,  4628,  4647,  4668,  4689,  4707,
    4739,  4740,  4745,  4745,  4763,  4781,  4815,  4851,  4852,  4855,
    4861,  4860,  4886,  4888,  4892,  4896,  4897,  4900,  4908,  4909,
    4908,  4916,  4917,  4920,  4926,  4925,  4938,  4940,  4944,  4948,
    4949,  4952,  4959,  4960,  4959,  4969,  4970,  4969,  4978,  4979,
    4982,  4988,  4987,  5006,  5007,  5009,  5010,  5015,  5034,  5044,
    5045,  5047,  5048,  5053,  5073,  5083,  5093,  5096,  5114,  5118,
    5119,  5122,  5123,  5122,  5133,  5134,  5137,  5142,  5144,  5142,
    5151,  5151,  5157,  5157,  5163,  5163,  5169,  5172,  5173,  5176,
    5182,  5188,  5196,  5196,  5200,  5201,  5204,  5215,  5224,  5235,
    5237,  5254,  5258,  5259,  5263,  5262
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if YYDEBUG || 0
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "QSTRING", "T_STRING",
  "SITE_PATTERN", "NUMBER", "K_HISTORY", "K_NAMESCASESENSITIVE",
  "K_DESIGN", "K_VIAS", "K_TECH", "K_UNITS", "K_ARRAY", "K_FLOORPLAN",
  "K_SITE", "K_CANPLACE", "K_CANNOTOCCUPY", "K_DIEAREA", "K_PINS",
  "K_DEFAULTCAP", "K_MINPINS", "K_WIRECAP", "K_TRACKS", "K_GCELLGRID",
  "K_DO", "K_BY", "K_STEP", "K_LAYER", "K_ROW", "K_RECT", "K_COMPS",
  "K_COMP_GEN", "K_SOURCE", "K_WEIGHT", "K_EEQMASTER", "K_FIXED",
  "K_COVER", "K_UNPLACED", "K_PLACED", "K_FOREIGN", "K_REGION",
  "K_REGIONS", "K_NETS", "K_START_NET", "K_MUSTJOIN", "K_ORIGINAL",
  "K_USE", "K_STYLE", "K_PATTERN", "K_PATTERNNAME", "K_ESTCAP", "K_ROUTED",
  "K_NEW", "K_SNETS", "K_SHAPE", "K_WIDTH", "K_VOLTAGE", "K_SPACING",
  "K_NONDEFAULTRULE", "K_NONDEFAULTRULES", "K_N", "K_S", "K_E", "K_W",
  "K_FN", "K_FE", "K_FS", "K_FW", "K_GROUPS", "K_GROUP", "K_SOFT",
  "K_MAXX", "K_MAXY", "K_MAXHALFPERIMETER", "K_CONSTRAINTS", "K_NET",
  "K_PATH", "K_SUM", "K_DIFF", "K_SCANCHAINS", "K_START", "K_FLOATING",
  "K_ORDERED", "K_STOP", "K_IN", "K_OUT", "K_RISEMIN", "K_RISEMAX",
  "K_FALLMIN", "K_FALLMAX", "K_WIREDLOGIC", "K_MAXDIST", "K_ASSERTIONS",
  "K_DISTANCE", "K_MICRONS", "K_END", "K_IOTIMINGS", "K_RISE", "K_FALL",
  "K_VARIABLE", "K_SLEWRATE", "K_CAPACITANCE", "K_DRIVECELL", "K_FROMPIN",
  "K_TOPIN", "K_PARALLEL", "K_TIMINGDISABLES", "K_THRUPIN", "K_MACRO",
  "K_PARTITIONS", "K_TURNOFF", "K_FROMCLOCKPIN", "K_FROMCOMPPIN",
  "K_FROMIOPIN", "K_TOCLOCKPIN", "K_TOCOMPPIN", "K_TOIOPIN", "K_SETUPRISE",
  "K_SETUPFALL", "K_HOLDRISE", "K_HOLDFALL", "K_VPIN", "K_SUBNET",
  "K_XTALK", "K_PIN", "K_SYNTHESIZED", "K_DEFINE", "K_DEFINES",
  "K_DEFINEB", "K_IF", "K_THEN", "K_ELSE", "K_FALSE", "K_TRUE", "K_EQ",
  "K_NE", "K_LE", "K_LT", "K_GE", "K_GT", "K_OR", "K_AND", "K_NOT",
  "K_SPECIAL", "K_DIRECTION", "K_RANGE", "K_FPC", "K_HORIZONTAL",
  "K_VERTICAL", "K_ALIGN", "K_MIN", "K_MAX", "K_EQUAL", "K_BOTTOMLEFT",
  "K_TOPRIGHT", "K_ROWS", "K_TAPER", "K_TAPERRULE", "K_VERSION",
  "K_DIVIDERCHAR", "K_BUSBITCHARS", "K_PROPERTYDEFINITIONS", "K_STRING",
  "K_REAL", "K_INTEGER", "K_PROPERTY", "K_BEGINEXT", "K_ENDEXT",
  "K_NAMEMAPSTRING", "K_ON", "K_OFF", "K_X", "K_Y", "K_COMPONENT",
  "K_MASK", "K_MASKSHIFT", "K_COMPSMASKSHIFT", "K_SAMEMASK",
  "K_PINPROPERTIES", "K_TEST", "K_COMMONSCANPINS", "K_SNET",
  "K_COMPONENTPIN", "K_REENTRANTPATHS", "K_SHIELD", "K_SHIELDNET",
  "K_NOSHIELD", "K_VIRTUAL", "K_ANTENNAPINPARTIALMETALAREA",
  "K_ANTENNAPINPARTIALMETALSIDEAREA", "K_ANTENNAPINGATEAREA",
  "K_ANTENNAPINDIFFAREA", "K_ANTENNAPINMAXAREACAR",
  "K_ANTENNAPINMAXSIDEAREACAR", "K_ANTENNAPINPARTIALCUTAREA",
  "K_ANTENNAPINMAXCUTCAR", "K_SIGNAL", "K_POWER", "K_GROUND", "K_CLOCK",
  "K_TIEOFF", "K_ANALOG", "K_SCAN", "K_RESET", "K_RING", "K_STRIPE",
  "K_FOLLOWPIN", "K_IOWIRE", "K_COREWIRE", "K_BLOCKWIRE", "K_FILLWIRE",
  "K_BLOCKAGEWIRE", "K_PADRING", "K_BLOCKRING", "K_BLOCKAGES",
  "K_PLACEMENT", "K_SLOTS", "K_FILLS", "K_PUSHDOWN", "K_NETLIST", "K_DIST",
  "K_USER", "K_TIMING", "K_BALANCED", "K_STEINER", "K_TRUNK",
  "K_FIXEDBUMP", "K_FENCE", "K_FREQUENCY", "K_GUIDE", "K_MAXBITS",
  "K_PARTITION", "K_TYPE", "K_ANTENNAMODEL", "K_DRCFILL", "K_OXIDE1",
  "K_OXIDE2", "K_OXIDE3", "K_OXIDE4", "K_CUTSIZE", "K_CUTSPACING",
  "K_DESIGNRULEWIDTH", "K_DIAGWIDTH", "K_ENCLOSURE", "K_HALO",
  "K_GROUNDSENSITIVITY", "K_HARDSPACING", "K_LAYERS", "K_MINCUTS",
  "K_NETEXPR", "K_OFFSET", "K_ORIGIN", "K_ROWCOL", "K_STYLES", "K_POLYGON",
  "K_PORT", "K_SUPPLYSENSITIVITY", "K_VIA", "K_VIARULE", "K_WIREEXT",
  "K_EXCEPTPGNET", "K_FILLWIREOPC", "K_OPC", "K_PARTIAL", "K_ROUTEHALO",
  "';'", "'-'", "'+'", "'('", "')'", "'*'", "','", "$accept", "def_file",
  "version_stmt", "$@1", "case_sens_stmt", "rules", "rule",
  "design_section", "design_name", "$@2", "end_design", "tech_name", "$@3",
  "array_name", "$@4", "floorplan_name", "$@5", "history",
  "prop_def_section", "$@6", "property_defs", "property_def", "$@7", "$@8",
  "$@9", "$@10", "$@11", "$@12", "$@13", "$@14", "$@15",
  "property_type_and_val", "$@16", "$@17", "opt_num_val", "units",
  "divider_char", "bus_bit_chars", "canplace", "$@18", "cannotoccupy",
  "$@19", "orient", "die_area", "$@20", "pin_cap_rule", "start_def_cap",
  "pin_caps", "pin_cap", "end_def_cap", "pin_rule", "start_pins", "pins",
  "pin", "$@21", "$@22", "$@23", "pin_options", "pin_option", "$@24",
  "$@25", "$@26", "$@27", "$@28", "$@29", "$@30", "$@31", "$@32", "$@33",
  "pin_layer_mask_opt", "pin_via_mask_opt", "pin_poly_mask_opt",
  "pin_layer_spacing_opt", "pin_poly_spacing_opt", "pin_oxide", "use_type",
  "pin_layer_opt", "$@34", "end_pins", "row_rule", "$@35", "$@36",
  "row_do_option", "row_step_option", "row_options", "row_option", "$@37",
  "row_prop_list", "row_prop", "tracks_rule", "$@38", "track_start",
  "track_type", "track_opts", "track_mask_statement", "same_mask",
  "track_layer_statement", "$@39", "track_layers", "track_layer",
  "gcellgrid", "extension_section", "extension_stmt", "via_section", "via",
  "via_declarations", "via_declaration", "$@40", "$@41", "layer_stmts",
  "layer_stmt", "$@42", "$@43", "$@44", "$@45", "$@46", "$@47",
  "layer_viarule_opts", "$@48", "firstPt", "nextPt", "otherPts", "pt",
  "mask", "via_end", "regions_section", "regions_start", "regions_stmts",
  "regions_stmt", "$@49", "$@50", "rect_list", "region_options",
  "region_option", "$@51", "region_prop_list", "region_prop",
  "region_type", "comps_maskShift_section", "comps_section", "start_comps",
  "layer_statement", "maskLayer", "comps_rule", "comp", "comp_start",
  "comp_id_and_name", "$@52", "comp_net_list", "comp_options",
  "comp_option", "comp_extension_stmt", "comp_eeq", "$@53",
  "comp_generate", "$@54", "opt_pattern", "comp_source", "source_type",
  "comp_region", "comp_pnt_list", "comp_halo", "$@55", "halo_soft",
  "comp_routehalo", "$@56", "comp_property", "$@57", "comp_prop_list",
  "comp_prop", "comp_region_start", "comp_foreign", "$@58", "opt_paren",
  "comp_type", "maskShift", "placement_status", "weight", "end_comps",
  "nets_section", "start_nets", "net_rules", "one_net",
  "net_and_connections", "net_start", "$@59", "net_name", "$@60", "$@61",
  "net_connections", "net_connection", "$@62", "$@63", "$@64", "conn_opt",
  "net_options", "net_option", "$@65", "$@66", "$@67", "$@68", "$@69",
  "$@70", "$@71", "$@72", "$@73", "$@74", "$@75", "net_prop_list",
  "net_prop", "netsource_type", "vpin_stmt", "$@76", "vpin_begin", "$@77",
  "vpin_layer_opt", "$@78", "vpin_options", "vpin_status", "net_type",
  "paths", "new_path", "$@79", "path", "$@80", "$@81", "virtual_statement",
  "rect_statement", "path_item_list", "path_item", "$@82", "$@83",
  "path_pt", "virtual_pt", "rect_pts", "opt_taper_style_s",
  "opt_taper_style", "opt_taper", "$@84", "opt_style", "opt_spaths",
  "opt_shape_style", "end_nets", "shape_type", "snets_section",
  "snet_rules", "snet_rule", "snet_options", "snet_option",
  "snet_other_option", "$@85", "$@86", "$@87", "$@88", "$@89", "$@90",
  "$@91", "$@92", "$@93", "$@94", "orient_pt", "shield_layer", "$@95",
  "snet_width", "$@96", "snet_voltage", "$@97", "snet_spacing", "$@98",
  "$@99", "snet_prop_list", "snet_prop", "opt_snet_range", "opt_range",
  "pattern_type", "spaths", "snew_path", "$@100", "spath", "$@101",
  "$@102", "width", "start_snets", "end_snets", "groups_section",
  "groups_start", "group_rules", "group_rule", "start_group", "$@103",
  "group_members", "group_member", "group_options", "group_option",
  "$@104", "$@105", "group_region", "group_prop_list", "group_prop",
  "group_soft_options", "group_soft_option", "groups_end",
  "assertions_section", "constraint_section", "assertions_start",
  "constraints_start", "constraint_rules", "constraint_rule",
  "operand_rule", "operand", "$@106", "$@107", "operand_list",
  "wiredlogic_rule", "$@108", "opt_plus", "delay_specs", "delay_spec",
  "constraints_end", "assertions_end", "scanchains_section",
  "scanchain_start", "scanchain_rules", "scan_rule", "start_scan", "$@109",
  "scan_members", "opt_pin", "scan_member", "$@110", "$@111", "$@112",
  "$@113", "$@114", "$@115", "opt_common_pins", "floating_inst_list",
  "one_floating_inst", "$@116", "floating_pins", "ordered_inst_list",
  "one_ordered_inst", "$@117", "ordered_pins", "partition_maxbits",
  "scanchain_end", "iotiming_section", "iotiming_start", "iotiming_rules",
  "iotiming_rule", "start_iotiming", "$@118", "iotiming_members",
  "iotiming_member", "$@119", "$@120", "iotiming_drivecell_opt", "$@121",
  "$@122", "iotiming_frompin", "$@123", "iotiming_parallel", "risefall",
  "iotiming_end", "floorplan_contraints_section", "fp_start", "fp_stmts",
  "fp_stmt", "$@124", "$@125", "h_or_v", "constraint_type",
  "constrain_what_list", "constrain_what", "$@126", "$@127",
  "row_or_comp_list", "row_or_comp", "$@128", "$@129",
  "timingdisables_section", "timingdisables_start", "timingdisables_rules",
  "timingdisables_rule", "$@130", "$@131", "$@132", "$@133",
  "td_macro_option", "$@134", "$@135", "$@136", "timingdisables_end",
  "partitions_section", "partitions_start", "partition_rules",
  "partition_rule", "start_partition", "$@137", "turnoff", "turnoff_setup",
  "turnoff_hold", "partition_members", "partition_member", "$@138",
  "$@139", "$@140", "$@141", "$@142", "$@143", "minmaxpins", "$@144",
  "min_or_max_list", "min_or_max_member", "pin_list",
  "risefallminmax1_list", "risefallminmax1", "risefallminmax2_list",
  "risefallminmax2", "partitions_end", "comp_names", "comp_name", "$@145",
  "subnet_opt_syn", "subnet_options", "subnet_option", "$@146", "$@147",
  "subnet_type", "pin_props_section", "begin_pin_props", "opt_semi",
  "end_pin_props", "pin_prop_list", "pin_prop_terminal", "$@148", "$@149",
  "pin_prop_options", "pin_prop", "$@150", "pin_prop_name_value_list",
  "pin_prop_name_value", "blockage_section", "blockage_start",
  "blockage_end", "blockage_defs", "blockage_def", "blockage_rule",
  "$@151", "$@152", "$@153", "layer_blockage_rules", "layer_blockage_rule",
  "mask_blockage_rule", "comp_blockage_rule", "$@154",
  "placement_comp_rules", "placement_comp_rule", "$@155",
  "rectPoly_blockage_rules", "rectPoly_blockage", "$@156", "slot_section",
  "slot_start", "slot_end", "slot_defs", "slot_def", "slot_rule", "$@157",
  "$@158", "geom_slot_rules", "geom_slot", "$@159", "fill_section",
  "fill_start", "fill_end", "fill_defs", "fill_def", "$@160", "$@161",
  "fill_rule", "$@162", "$@163", "geom_fill_rules", "geom_fill", "$@164",
  "fill_layer_mask_opc_opt", "opt_mask_opc_l", "fill_layer_opc",
  "fill_via_pt", "fill_via_mask_opc_opt", "opt_mask_opc", "fill_via_opc",
  "fill_mask", "fill_viaMask", "nondefaultrule_section",
  "nondefault_start", "nondefault_end", "nondefault_defs",
  "nondefault_def", "$@165", "$@166", "nondefault_options",
  "nondefault_option", "$@167", "$@168", "$@169", "$@170", "$@171",
  "nondefault_layer_options", "nondefault_layer_option",
  "nondefault_prop_opt", "$@172", "nondefault_prop_list",
  "nondefault_prop", "styles_section", "styles_start", "styles_end",
  "styles_rules", "styles_rule", "$@173", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-1120)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-542)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     -67, -1120,    97,   127,   141, -1120,   -58,    46,  -112,  -100,
     -96, -1120,   338, -1120, -1120, -1120, -1120, -1120,   191, -1120,
     111, -1120, -1120, -1120, -1120, -1120,   231,   238,   -32,   -32,
   -1120,   242,   252,   271,   279,   282,   302,   316,   324,   332,
     245,   340,   347,   354,   370,   385,   394, -1120, -1120, -1120,
     393,   406,   421,   427,   431, -1120, -1120, -1120, -1120, -1120,
   -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120,
   -1120, -1120, -1120, -1120, -1120, -1120,   436, -1120, -1120, -1120,
   -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120,
   -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120,
   -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120,
   -1120, -1120, -1120, -1120, -1120,   180, -1120, -1120,   447,   214,
     487,   407,   502,   507,   519,   538,   285,   294, -1120, -1120,
   -1120, -1120,   560,   566,   306,   307,   308,   309,   310,   311,
     315,   317,   318, -1120,   319,   320,   323,   327,   329,   333,
   -1120,     9,   348,   352,   365,   367,   371,    65,   -52, -1120,
     -46,   -35,   -29,   -24,   -22,   -20,   -18,   -17,   -11,    -5,
       3,     6,    24,    37,    38,    50,    56, -1120, -1120,    66,
     372, -1120,   375,   572,   376,   379,   576,   584,    12,   285,
   -1120, -1120,   609,   587, -1120, -1120, -1120, -1120, -1120, -1120,
   -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120,   330,
   -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120,   630,
     620, -1120, -1120,   624, -1120, -1120, -1120,   635,   648, -1120,
   -1120, -1120,   625, -1120, -1120,   641, -1120, -1120, -1120, -1120,
   -1120,   623, -1120, -1120, -1120, -1120, -1120,   619, -1120, -1120,
   -1120,   612, -1120, -1120, -1120, -1120,   589,   325, -1120, -1120,
   -1120, -1120,   608, -1120,   604, -1120, -1120, -1120, -1120,   588,
     417, -1120, -1120, -1120,   541, -1120, -1120,   582,     1, -1120,
   -1120,   580, -1120, -1120, -1120, -1120,   512, -1120, -1120, -1120,
     477,    17, -1120, -1120,    10,   485,   675, -1120, -1120, -1120,
     486,    14, -1120, -1120, -1120,   728,    67,   480,   687, -1120,
   -1120, -1120, -1120,   470, -1120, -1120,   731,   734,    13,    15,
   -1120, -1120,   735,   736,   478, -1120, -1120, -1120, -1120, -1120,
   -1120,   581, -1120, -1120, -1120, -1120,   723, -1120, -1120,   742,
     741, -1120,   744, -1120,   749, -1120,   750,  -137,     8, -1120,
      83,   -74, -1120,   -42, -1120,   751,   752, -1120, -1120, -1120,
     488,   489, -1120, -1120, -1120, -1120,   756,    61, -1120, -1120,
     104, -1120,   757, -1120, -1120, -1120, -1120,   496, -1120,   759,
     116, -1120,   762, -1120, -1120, -1120,   285, -1120, -1120, -1120,
   -1120,   -13, -1120, -1120, -1120,     0, -1120,   707, -1120, -1120,
   -1120,   763, -1120,   633,   633,   500,   501,   503,   505,  -162,
     745,   768, -1120,   772,   773,   775,   776,   777,   778, -1120,
     779,   780,   781,   782,   518,   760, -1120, -1120,   785, -1120,
     140, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120,
   -1120,    18, -1120, -1120, -1120,   285, -1120, -1120, -1120, -1120,
     521, -1120, -1120,   628, -1120, -1120, -1120,   764, -1120,   564,
   -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120,   164,
     792,   793,   287,   287,   794,   170, -1120, -1120,   312, -1120,
   -1120,   795, -1120,   288, -1120, -1120,    71,   796,   797,   798,
   -1120,   692, -1120,   357, -1120, -1120,   800,   801, -1120,   285,
     285,     2,   802,   285, -1120, -1120, -1120,   803,   804,   285,
   -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120,
   -1120, -1120, -1120, -1120,   784,   786, -1120, -1120, -1120, -1120,
   -1120, -1120,   806,   633,    53,    53,    53,    53,    53,    53,
      53,    53,    53,   544,   737,   810, -1120,   285, -1120, -1120,
     202,   811, -1120, -1120, -1120,   285, -1120, -1120, -1120, -1120,
   -1120,   812, -1120,   814, -1120,   285,   285,   633, -1120,   817,
     188,   818, -1120, -1120, -1120,   509,   819,   -37,   820, -1120,
   -1120, -1120, -1120,   821, -1120, -1120, -1120, -1120, -1120, -1120,
   -1120,   285, -1120,   202,   823, -1120,   509,   824,   -37,   825,
     443, -1120, -1120, -1120, -1120,   826, -1120, -1120,   827, -1120,
   -1120,   830, -1120,    43, -1120, -1120, -1120,   831, -1120,     4,
      40,   555, -1120,   374, -1120, -1120, -1120, -1120, -1120, -1120,
   -1120,   832, -1120, -1120,   833, -1120,   181, -1120, -1120, -1120,
     834,   836,    87,   195, -1120, -1120, -1120, -1120, -1120, -1120,
   -1120, -1120, -1120,   573, -1120,   285, -1120, -1120, -1120,   285,
     285, -1120, -1120,   285,   285,   224,   285,   837,   838,   579,
   -1120,   839, -1120, -1120,   842,   583,   585,   586,   590,   592,
     593,   594,   595,   596, -1120, -1120,   672,   235,   285,   285,
     844, -1120, -1120, -1120, -1120, -1120, -1120,   846,   633,   849,
     850, -1120,   799, -1120, -1120,   285, -1120,   597, -1120, -1120,
   -1120, -1120, -1120, -1120, -1120, -1120,   859, -1120, -1120, -1120,
   -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120,
   -1120, -1120, -1120,   860,   863,   864, -1120,   865,   867, -1120,
     866,   869,   870,   285,   871, -1120, -1120,   872, -1120, -1120,
   -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120,
   -1120, -1120, -1120, -1120, -1120,   873,   874,   876,   877, -1120,
     878, -1120,   879,   880,   881, -1120, -1120, -1120,   883, -1120,
     287, -1120, -1120, -1120,   805,   882,   884,   885,   886,   889,
   -1120, -1120,   891,   627,   894,   629, -1120,   896,   895,   897,
     331,   807,   636, -1120, -1120,   638, -1120, -1120,   299,   901,
     902,   903,   905,   906,   907, -1120, -1120,   -23, -1120,   285,
      16, -1120,   285, -1120, -1120, -1120,   285, -1120,    62, -1120,
   -1120,   285,   887,   888, -1120,   890, -1120,   743,   743, -1120,
   -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120,   912,
     911,   652,   892, -1120,    73, -1120, -1120, -1120, -1120,   285,
     248,   915, -1120, -1120,    33,   208,   850, -1120, -1120,   916,
     917, -1120,     7, -1120,   919, -1120, -1120, -1120, -1120,   411,
     865, -1120, -1120,   869, -1120, -1120,   875, -1120, -1120, -1120,
     285, -1120,   918, -1120,   920,   440,   877, -1120, -1120, -1120,
     653, -1120,   893, -1120,    20,   157, -1120,   921, -1120,   923,
   -1120, -1120, -1120, -1120,   926,   927,   928,   926,   929, -1120,
     696, -1120, -1120,   930,   931, -1120,   932,   933,   934, -1120,
   -1120, -1120,   937,   938, -1120, -1120, -1120, -1120,   939,   940,
   -1120,   941,   943, -1120,   254,   666, -1120, -1120, -1120,   942,
   -1120, -1120,   285,   -16,   189,   285, -1120, -1120, -1120, -1120,
   -1120, -1120, -1120,   944,   945,   946, -1120,   948,   949,   949,
   -1120,   771, -1120, -1120, -1120, -1120, -1120, -1120,   950,   951,
     952, -1120, -1120, -1120, -1120,   -95, -1120, -1120, -1120,   953,
   -1120,   633, -1120, -1120, -1120, -1120,   954,   957, -1120, -1120,
   -1120,   665, -1120, -1120, -1120, -1120, -1120,   875, -1120, -1120,
   -1120,   305,   285, -1120, -1120, -1120, -1120, -1120, -1120,   958,
     285, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120,
     959, -1120, -1120, -1120,   285, -1120,   961,   962,   963, -1120,
     960, -1120,   697, -1120, -1120, -1120, -1120, -1120, -1120, -1120,
     966,   965, -1120,   868, -1120, -1120, -1120, -1120, -1120,   262,
     969,   898, -1120,   368,   388,   399,   368,   388,   399, -1120,
     808, -1120,   -15, -1120, -1120, -1120,   971, -1120,   285, -1120,
     -81, -1120, -1120, -1120, -1120,   -79, -1120,   710, -1120, -1120,
   -1120, -1120,   973, -1120,   974,   975,   976,  -158,   955,   956,
     964,   275,   978, -1120, -1120, -1120, -1120, -1120, -1120,   977,
     981,   982,   983,   985,   986,   987,   984,   990, -1120,    39,
   -1120, -1120,   989, -1120,   992,   993,   994, -1120,   720,   -10,
     869, -1120, -1120, -1120, -1120,   285, -1120,   853, -1120,   881,
     285,   285, -1120, -1120,   881, -1120, -1120, -1120, -1120,   450,
   -1120, -1120,   732,   733,   730, -1120, -1120, -1120,   899, -1120,
     377, -1120,  1001, -1120, -1120,  1000,  1002,  1004,  1005,   388,
   -1120,  1006,  1007,  1008,  1010, -1120, -1120,   388, -1120,  1011,
   -1120,  1012, -1120, -1120, -1120,  1013, -1120, -1120,   285,  1014,
   -1120,  1015, -1120,   285, -1120,   285,   967,  1003,  1016, -1120,
   -1120, -1120,  1018,  1019,  1020, -1120,   861, -1120, -1120,   293,
   -1120,   977,   761, -1120, -1120,  1022, -1120, -1120,   761,   765,
   -1120, -1120, -1120, -1120,  1024,   766,   766,   766, -1120, -1120,
   -1120,  1025, -1120, -1120,    19, -1120, -1120, -1120, -1120, -1120,
     633,  1026, -1120,   893,   285, -1120,   267, -1120, -1120, -1120,
   -1120,  1031, -1120,  1032, -1120,   769,  1033, -1120, -1120, -1120,
     774,  1035, -1120,    49,  1036,  1037,  1038,  1039, -1120, -1120,
   -1120, -1120, -1120, -1120, -1120, -1120,  1042, -1120, -1120, -1120,
   -1120,  1041,   504, -1120, -1120,  1043,  1044,  1021, -1120, -1120,
     413, -1120, -1120,   285, -1120,   977,   900,   285,  1045, -1120,
     813,  1048, -1120,   -31, -1120,   788,   789,   790,  1051,   131,
   -1120,  1052,    21,    22, -1120, -1120,  1055, -1120,   285,   102,
   -1120, -1120,  1058,  1059,  1060, -1120,  1061, -1120, -1120, -1120,
   -1120, -1120,  1062,  1063, -1120, -1120, -1120, -1120, -1120,  1066,
   -1120, -1120, -1120, -1120, -1120,   815,   816,  1065, -1120, -1120,
   -1120,   509, -1120,  1068,  1067,  1070,  1071,  1072,  1073,  1074,
    1077,  1078,   258, -1120,  1064, -1120, -1120, -1120, -1120,   633,
   -1120,  1079,   285, -1120,   285,  1080,   515, -1120, -1120, -1120,
   -1120, -1120,  1083, -1120, -1120, -1120, -1120, -1120, -1120, -1120,
   -1120,    27,    28,    29,    31,    11, -1120,   285,  1082,   443,
   -1120,   822,   828,  1085, -1120,   835,   835, -1120,  1084,  1087,
     523, -1120, -1120, -1120, -1120,  1088,  1091,  1093, -1120, -1120,
    1046,  1046,  1046,  1046,  1075,  1081,  1046,  1086, -1120, -1120,
   -1120, -1120, -1120,  1095, -1120,  1096,  1097,  1098, -1120, -1120,
   -1120,   285,  1099, -1120, -1120, -1120,   840,  1102,   869,   841,
   -1120,   843, -1120,   845, -1120,   847, -1120,   484,   851,  1101,
     852, -1120, -1120, -1120, -1120, -1120, -1120,    11,   854,   855,
     848,   935,    42, -1120,  1106, -1120, -1120, -1120, -1120, -1120,
      -9, -1120,   561, -1120, -1120, -1120, -1120, -1120, -1120, -1120,
   -1120, -1120, -1120, -1120, -1120, -1120, -1120,   947,   285,   857,
    1009,   856, -1120,   875, -1120, -1120, -1120, -1120,  1110,  1094,
    1121, -1120,    94,    23, -1120,  1108,  1124, -1120,  1123, -1120,
   -1120, -1120, -1120,  1125,  1126,  1127, -1120, -1120, -1120, -1120,
     968,  1130,  1132,  1133,  1134,   970,  1135,   904, -1120,   908,
   -1120, -1120,  1040,  1138,  1140,   562, -1120,   909,    25,  1141,
    1136,  1144, -1120,  1145,  1146, -1120, -1120, -1120,  1147,    -7,
   -1120, -1120, -1120, -1120,  1148,    -6, -1120,  1149,   285, -1120,
    1151,  1104,  1152,  1153,  1114,   910, -1120,   913,   914,   922,
     924,   925,   936,   972, -1120,  1154,  1155,   285, -1120,  1156,
    1157,   285,  1158,  1161,  1139,  1162,  1163,  1150,  1164,  1166,
   -1120, -1120, -1120,   980,   988, -1120, -1120, -1120, -1120,   285,
   -1120, -1120,   285,   991,  1170,  1169,  1159,   995,  1171,  1165,
    1174,  1177,  1178, -1120,   285, -1120,  1181,  1182,  1183, -1120,
    1160,  1184,  1187,  1192,  1193, -1120,   979, -1120,  1194,  1195,
    1172,  1196,   996,   997,   285,   998, -1120,  1197,  1198,   999,
   -1120, -1120,  1199, -1120,  1201, -1120,  1202, -1120,  1017,  1027,
    1203,  1204,  1205,  1206, -1120
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int16 yydefact[] =
{
       3,     4,     0,     6,     0,     1,     0,     0,     0,     0,
       0,    11,     0,     5,     7,     8,    60,    51,     0,    54,
       0,    56,    58,    96,    98,   108,     0,     0,     0,     0,
     185,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    61,   218,   281,
       0,     0,     0,     0,     0,    10,    12,    38,     2,    48,
      34,    41,    43,    46,    50,    40,    35,    36,    37,    39,
      44,   112,    45,   118,    47,    49,     0,    42,    17,    33,
     222,    27,   259,    19,    15,   284,    23,   352,    30,   493,
      21,   575,    13,    16,   608,   608,    28,   636,    22,   682,
      20,   709,    32,   734,    25,   752,    26,   822,    14,   839,
      29,   875,    18,   889,    24,     0,    31,   952,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   111,   204,
     205,   203,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    53,     0,     0,     0,     0,     0,     0,
      63,     0,   819,     0,     0,     0,     0,     0,     0,   201,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   921,   919,     0,
       0,   221,     0,     0,     0,     0,     0,     0,     0,     0,
     246,   117,     0,     0,   280,   258,   351,   571,   917,   574,
     607,   635,   606,   681,   733,   751,   708,    94,    95,     0,
     283,   278,   282,   820,   818,   837,   873,   887,   950,     0,
       0,   113,   110,     0,   120,   119,   116,     0,     0,   224,
     223,   220,     0,   261,   260,     0,   288,   285,   293,   290,
     279,     0,   356,   353,   373,   355,   350,     0,   496,   494,
     492,     0,   578,   576,   580,   573,     0,     0,   609,   610,
     611,   604,     0,   605,     0,   639,   637,   641,   634,     0,
       0,   683,   687,   680,     0,   711,   710,     0,     0,   735,
     732,     0,   755,   753,   765,   750,     0,   824,   817,   823,
       0,     0,   836,   840,     0,     0,     0,   872,   876,   881,
       0,     0,   886,   890,   898,     0,     0,     0,     0,   949,
     953,    52,    55,     0,    57,    59,     0,     0,     0,     0,
     248,   247,     0,     0,     0,    65,    77,    71,    81,    73,
      67,     0,    75,    69,    79,    64,     0,   115,   184,     0,
       0,   256,     0,   257,     0,   349,     0,     0,   287,   479,
       0,     0,   572,     0,   603,     0,   583,   633,   613,   615,
       0,     0,   622,   626,   632,   679,     0,     0,   706,   685,
       0,   707,     0,   749,   736,   739,   741,     0,   800,     0,
       0,   821,     0,   838,   842,   845,     0,   870,   867,   874,
     878,     0,   888,   895,   892,     0,   922,     0,   916,   920,
     951,     0,    93,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    83,     0,     0,     0,     0,     0,     0,    62,
       0,     0,     0,     0,     0,     0,   225,   262,     0,   286,
       0,   307,   294,   306,   302,   295,   296,   301,   303,   304,
     305,     0,   300,   297,   299,     0,   298,   292,   291,   358,
       0,   357,   354,     0,   403,   374,   391,   418,   495,     0,
     530,   497,   501,   498,   499,   500,   579,   582,   581,     0,
       0,     0,     0,     0,     0,     0,   640,   638,     0,   657,
     642,     0,   684,     0,   695,   688,     0,     0,     0,     0,
     743,   757,   754,     0,   779,   766,     0,     0,   860,     0,
       0,     0,     0,     0,   884,   877,   882,     0,     0,     0,
     901,   891,   899,   924,   918,   954,   100,   102,   103,   101,
     104,   107,   106,   105,     0,     0,   250,   252,   251,   253,
     109,   249,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   227,     0,   289,   310,
       0,     0,   308,   345,   346,   342,   347,   337,   336,   329,
     219,     0,   323,     0,   320,     0,   319,     0,   362,     0,
       0,     0,   426,   427,   381,     0,     0,     0,     0,   428,
     389,   416,   397,     0,   401,   392,   394,   378,   379,   375,
     419,     0,   513,     0,     0,   521,     0,     0,     0,     0,
       0,   543,   545,   547,   528,     0,   505,   519,     0,   510,
     515,   502,   577,     0,   590,   584,   614,     0,   619,     0,
       0,   624,   612,     0,   627,   645,   647,   649,   651,   653,
     655,     0,   704,   705,     0,   692,     0,   714,   715,   712,
       0,     0,     0,   759,   756,   767,   769,   771,   773,   775,
     777,   825,   843,   846,   869,     0,   841,   868,   879,     0,
       0,   896,   893,     0,     0,     0,     0,     0,     0,     0,
     186,    88,    86,    84,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   114,   121,   207,     0,     0,   266,
       0,   315,   316,   317,   318,   314,   348,     0,     0,     0,
       0,   344,   325,   327,   321,     0,   341,   359,   360,   413,
     409,   410,   411,   412,   377,   384,     0,   173,   174,   175,
     176,   177,   178,   179,   180,   387,   388,   562,   559,   560,
     561,   383,   386,     0,     0,     0,   385,     0,     0,   395,
       0,     0,     0,     0,     0,   518,   524,     0,   526,   527,
     523,   525,   480,   481,   482,   483,   484,   485,   486,   489,
     490,   491,   488,   487,   508,     0,     0,     0,     0,   509,
       0,   520,     0,     0,     0,   588,   598,   586,     0,   617,
       0,   620,   618,   625,     0,     0,     0,     0,     0,     0,
     661,   669,     0,   658,     0,     0,   691,     0,     0,     0,
       0,     0,     0,   744,   747,     0,   760,   761,   762,     0,
       0,     0,     0,     0,     0,   827,   847,     0,   861,     0,
       0,   883,     0,   903,   909,   900,     0,   923,     0,   925,
     936,     0,     0,     0,   217,   188,    89,   557,   557,    90,
      66,    78,    72,    82,    74,    68,    76,    70,    80,     0,
       0,     0,   211,   226,     0,   240,   228,   239,   264,     0,
       0,   312,   309,   343,     0,     0,   330,   331,   326,     0,
       0,   322,     0,   363,     0,   382,   390,   417,   398,     0,
     402,   404,   393,     0,   380,   433,   376,   429,   420,   414,
       0,   522,     0,   546,     0,     0,   529,   550,   506,   511,
     531,   567,   504,   563,     0,   585,   593,     0,   621,     0,
     628,   629,   630,   631,   643,   648,   650,   643,     0,   654,
     677,   686,   693,     0,     0,   716,     0,     0,     0,   720,
     737,   740,     0,     0,   742,   763,   764,   758,     0,     0,
     788,     0,     0,   788,     0,   844,   865,   862,   864,     0,
     248,   880,     0,     0,     0,     0,   927,   942,   926,   934,
     930,   932,   248,     0,     0,     0,   192,     0,    91,    91,
     122,   209,   202,   212,   206,   229,   244,   234,     0,     0,
       0,   231,   236,   265,   263,     0,   267,   313,   311,     0,
     339,     0,   334,   335,   333,   332,     0,     0,   364,   368,
     366,     0,   801,   407,   408,   406,   405,   396,   467,   431,
     430,   421,     0,   544,   548,   553,   554,   552,   551,   540,
       0,   532,   534,   535,   533,   536,   539,   538,   537,   516,
       0,   565,   564,   592,     0,   589,     0,     0,     0,   599,
     587,   616,     0,   644,   646,   663,   662,   671,   670,   652,
       0,     0,   656,   699,   689,   690,   718,   717,   719,     0,
       0,     0,   748,     0,     0,   772,     0,     0,   778,   826,
       0,   828,     0,   848,   851,   852,     0,   866,   871,   248,
       0,   897,   904,   905,   906,     0,   248,     0,   910,   911,
     912,   248,     0,   944,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    92,    87,    85,   124,   210,   208,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   268,     0,
     340,   338,     0,   328,     0,     0,     0,   361,   399,     0,
       0,   424,   425,   423,   415,     0,   514,   555,   507,     0,
       0,     0,   570,   475,     0,   591,   600,   601,   602,     0,
     594,   623,   665,   673,     0,   678,   700,   694,     0,   713,
       0,   721,     0,   745,   782,     0,     0,     0,     0,   770,
     794,     0,     0,     0,     0,   789,   782,   776,   829,     0,
     854,     0,   856,   857,   858,     0,   859,   863,   885,     0,
     907,     0,   913,   908,   894,   902,     0,   943,     0,   931,
     933,   955,     0,     0,     0,   187,     0,   193,   558,     0,
     216,   214,   254,   245,   235,     0,   242,   241,   254,     0,
     271,   276,   277,   270,     0,   370,   370,   370,   803,   807,
     802,     0,   471,   472,     0,   434,   468,   470,   469,   432,
       0,     0,   549,   542,     0,   248,     0,   566,   596,   597,
     595,     0,   664,     0,   672,   659,     0,   696,   722,   724,
       0,     0,   768,     0,     0,     0,     0,     0,   795,   790,
     792,   791,   793,   774,   831,   849,     0,   853,   850,   914,
     915,     0,     0,   945,   935,     0,     0,   190,   194,   123,
       0,   125,   127,     0,   213,   214,     0,     0,     0,   232,
       0,   269,   324,     0,   371,     0,     0,     0,     0,   400,
     474,     0,     0,     0,   438,   422,     0,   248,   517,     0,
     568,   476,     0,     0,     0,   701,     0,   726,   726,   738,
     746,   780,     0,     0,   783,   796,   798,   797,   799,   830,
     855,   928,   947,   948,   946,     0,     0,     0,   189,   196,
     136,     0,   126,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   132,     0,   139,   135,   130,   142,     0,
     215,     0,     0,   243,     0,     0,     0,   272,   372,   365,
     369,   367,     0,   813,   814,   815,   811,   816,   808,   809,
     473,     0,     0,     0,     0,   435,   556,   512,     0,     0,
     438,     0,     0,     0,   697,   723,   725,   786,     0,     0,
       0,   832,   937,    97,    99,     0,   195,     0,   134,   128,
     181,   181,   181,   181,     0,     0,   181,     0,   169,   170,
     171,   172,   156,     0,   129,     0,     0,     0,   144,   255,
     230,     0,     0,   274,   275,   273,   805,     0,     0,     0,
     455,     0,   457,     0,   456,     0,   458,   440,     0,     0,
       0,   448,   449,   439,   454,   478,   477,   569,   666,   674,
       0,   702,     0,   727,   781,   784,   785,   834,   835,   833,
     929,   191,     0,   197,   137,   182,   145,   146,   147,   148,
     149,   151,   153,   154,   133,   140,   131,   159,     0,     0,
       0,     0,   812,   810,   459,   461,   460,   462,     0,   442,
       0,   437,   452,     0,   436,     0,     0,   660,     0,   698,
     730,   728,   787,     0,     0,     0,   938,   199,   200,   198,
     157,     0,     0,     0,     0,   161,     0,     0,   248,     0,
     806,   804,     0,     0,     0,   441,   450,     0,     0,     0,
       0,     0,   703,     0,     0,   940,   939,   941,     0,   163,
     183,   150,   152,   155,     0,   166,   160,     0,   233,   237,
       0,     0,     0,     0,   443,     0,   453,     0,     0,     0,
       0,     0,     0,     0,   158,     0,     0,     0,   162,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     463,   465,   464,   667,   675,   731,   729,   164,   165,     0,
     167,   168,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   138,     0,   143,     0,     0,     0,   466,
       0,     0,     0,     0,     0,   248,     0,   445,     0,     0,
       0,     0,     0,     0,   141,     0,   446,     0,     0,     0,
     668,   676,     0,   444,     0,   451,     0,   447,     0,     0,
       0,     0,     0,     0,   238
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
   -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120,
   -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120,
   -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120,
   -1120,   187, -1120, -1120,   122, -1120, -1120, -1120, -1120, -1120,
   -1120, -1120,  -403, -1120, -1120, -1120, -1120, -1120, -1120, -1120,
   -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120,
   -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120,
   -1120, -1120, -1120, -1120, -1120,  -592, -1086, -1120, -1120, -1120,
   -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120,
   -1120, -1120,  1122, -1120, -1120, -1120, -1120, -1120,  -143,    89,
   -1120, -1120,  -344, -1120, -1120, -1120, -1120, -1120, -1120, -1120,
   -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120,  -495,
    -187,  -942,  -126,    -4, -1120, -1120, -1120, -1120, -1120, -1120,
   -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120,
   -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120,
   -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120,   622, -1120,
   -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120,   350,
   -1120, -1120, -1120, -1120, -1120, -1120,    26, -1120, -1120, -1120,
   -1120, -1120, -1120,  1049, -1120, -1120, -1120, -1120, -1120, -1120,
   -1120, -1120, -1120, -1120,  -658, -1120, -1120, -1120, -1120, -1120,
   -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120,   337,
   -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120,   767,
    -877, -1120, -1120,    88, -1120, -1120, -1120, -1120,  -181, -1120,
   -1120, -1120, -1119, -1120, -1120, -1120, -1120, -1120, -1120, -1120,
   -1120, -1120, -1120,  -179, -1120, -1120, -1120, -1120, -1120, -1120,
   -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120,
   -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120,
   -1120,   326, -1120,   383,   626,    84, -1120, -1120,    81, -1120,
   -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120,
   -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120,
   -1120, -1120, -1120, -1120, -1120, -1120,  1137, -1120, -1120,  -241,
   -1120, -1120,   754, -1120, -1120, -1120, -1120, -1120, -1120, -1120,
   -1120, -1120, -1120, -1120, -1120, -1120, -1120,   313, -1120, -1120,
   -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120,
   -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120,
   -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120,
   -1120, -1120,  -908, -1120, -1120, -1120, -1120, -1120, -1120, -1120,
   -1120, -1120, -1120, -1120, -1120, -1120,   -99, -1120, -1120, -1120,
   -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120,
   -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120,
   -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120,
      52, -1120, -1120, -1120, -1120,   290, -1120,   167, -1076, -1120,
   -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120,
   -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120,
   -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120,
   -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120,
     739, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120,
   -1120,   416, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120,
   -1120, -1120, -1120, -1120,   278, -1120, -1120, -1120, -1120, -1120,
   -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120,  1023,
   -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120,
   -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120, -1120,
   -1120
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,     2,     3,     4,     7,    12,    55,    56,    57,   118,
      58,    59,   120,    60,   122,    61,   123,    62,    63,   150,
     209,   335,   413,   418,   421,   415,   417,   420,   414,   422,
     416,   675,   838,   837,  1104,    64,    65,    66,    67,   124,
      68,   125,   524,    69,   126,    70,    71,   157,   221,   222,
      72,    73,   158,   225,   339,   849,  1106,  1209,  1291,  1436,
    1433,  1417,  1530,  1435,  1535,  1437,  1532,  1533,  1534,  1559,
    1537,  1565,  1587,  1591,  1432,   725,  1486,  1531,   226,    74,
     133,   835,   966,  1348,  1101,  1207,  1349,  1416,  1483,    75,
     227,    76,   131,   851,   852,  1108,   974,  1109,  1294,  1295,
      77,    78,  1304,    79,    80,   160,   230,   342,   546,   687,
     856,  1110,  1116,  1374,  1112,  1117,  1593,   857,  1111,   189,
     531,   409,   321,  1297,   231,    81,    82,   161,   234,   344,
     547,   689,   860,   986,  1220,  1301,  1377,  1223,    83,    84,
      85,   151,   212,   162,   237,   238,   239,   346,   348,   347,
     432,   433,   434,   697,   435,   690,   988,   436,   695,   437,
     566,   438,   702,   869,   439,   870,   440,   700,   866,   867,
     441,   442,   699,   991,   443,   444,   445,   446,   240,    86,
      87,   163,   243,   244,   245,   350,   451,   568,   874,   707,
     873,  1124,  1126,  1125,  1305,   351,   455,   741,   740,   716,
     733,   738,   739,   883,   735,  1002,  1229,   737,   880,   881,
     714,   456,  1011,   457,   734,   591,   742,  1134,  1135,   589,
     886,  1010,  1130,   887,  1008,  1314,  1461,  1462,  1395,  1463,
    1575,  1547,  1464,  1514,  1511,  1129,  1236,  1237,  1311,  1238,
    1246,  1321,   246,   764,    88,   164,   249,   353,   461,   462,
     774,   770,  1019,   772,  1020,   744,   773,  1141,   747,   768,
    1029,  1138,  1139,   463,   765,   464,   766,   465,   767,  1137,
     896,   897,  1242,   968,   731,   902,  1032,  1144,   903,  1030,
    1400,  1143,    89,   250,    90,    91,   165,   253,   254,   355,
     356,   468,   469,   615,   906,   904,  1035,  1040,  1150,   905,
    1039,   255,    92,    93,    94,    95,   166,   258,   259,   618,
     470,   471,   619,   260,   474,   784,   475,   624,   263,   261,
      96,    97,   168,   266,   267,   366,   367,  1044,   480,   789,
     790,   791,   792,   793,   794,   919,   915,  1046,  1152,  1252,
     916,  1048,  1153,  1254,  1052,   268,    98,    99,   169,   271,
     272,   481,   370,   485,   797,  1053,  1157,  1326,  1471,  1158,
    1256,  1519,   636,   273,   100,   101,   170,   276,   372,   800,
     639,   929,  1059,  1161,  1327,  1328,  1405,  1473,  1554,  1553,
     102,   103,   171,   279,   487,  1060,   488,   489,   805,   932,
    1261,   933,   280,   104,   105,   172,   283,   284,   379,   644,
     808,   937,   380,   495,   809,   810,   811,   812,   813,   814,
    1262,  1407,  1263,  1334,  1474,  1065,  1175,  1169,  1170,   285,
    1128,  1230,  1308,  1501,  1309,  1388,  1448,  1447,  1389,   106,
     107,   214,   288,   173,   289,   382,   815,   944,  1071,  1274,
    1339,  1411,   108,   109,   292,   174,   293,   294,   497,   816,
     498,   945,  1073,  1074,  1075,  1276,   653,   818,  1076,   501,
     388,   500,   110,   111,   297,   175,   298,   299,   502,   820,
     391,   506,   660,   112,   113,   302,   176,   303,   508,   824,
     304,   507,   823,   395,   512,   664,   953,  1082,  1083,  1087,
     954,  1088,  1089,  1084,  1090,   114,   115,   398,   306,   178,
     305,   513,   665,   829,  1092,  1412,  1095,  1096,  1094,  1480,
    1526,   830,  1093,  1197,  1283,   116,   117,   309,   179,   310,
     666
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
     190,   525,   320,   431,   748,   655,  1007,   454,  1078,   460,
    1235,   998,   447,   210,   509,  1457,   363,   503,   318,   405,
    1097,   407,   564,   479,  1033,  1312,   484,  1391,  1393,  1548,
     509,  1577,   386,  1449,  1451,  1453,   494,  1455,  1231,   989,
     386,  1458,   393,  1179,   223,   384,   503,    11,   946,  1523,
     228,  1585,  1589,    -9,   727,    -9,    -9,    -9,    -9,    -9,
      -9,   232,    -9,    -9,    -9,    -9,    -9,   235,  1331,    -9,
      -9,  1118,   241,  1520,   247,    -9,   251,    -9,   256,   262,
     358,   359,   360,   361,   775,   264,   219,   449,    -9,    -9,
     956,   269,     1,  1268,  1189,  1378,  1191,     5,  1545,   274,
      -9,  1268,   277,   975,   530,   374,    -9,   188,  1201,   375,
     376,   188,     9,    10,   776,    -9,   358,   359,   360,   361,
     281,    -9,   976,   977,  1546,   614,    -9,  1320,   450,   429,
     670,   430,   999,   286,   290,     6,   560,  1188,  1119,    -9,
     129,   130,    -9,    -9,  1193,     8,   295,  1232,  1233,  1195,
    1398,   947,   300,    -9,    13,  1164,    -9,  1399,  1176,  1180,
    1181,   220,   307,   397,   706,   822,    14,  1383,  1384,   826,
      15,   831,   549,   550,   551,   552,   553,   554,   555,   556,
     557,   558,  1190,  1385,  1192,   377,  1459,   728,   729,   730,
    1386,   803,   452,    -9,   453,   804,   948,   119,  1521,  1460,
    1332,  1333,  1182,  1183,  1184,   121,    -9,    -9,    -9,   777,
     560,   992,   993,    -9,   994,   224,   671,   672,   673,   637,
     638,   229,   674,    -9,   458,    -9,   459,  1185,   957,  1036,
    1037,  1038,   233,   385,  1524,  1586,  1590,   127,   236,   510,
     560,   949,   504,   242,   128,   242,  1186,   252,   134,   257,
     257,  1525,  1080,   505,   143,   510,   265,   387,   135,  1234,
     499,    -9,   270,    -9,    -9,   387,   511,  1221,   656,  1222,
     275,   504,   394,   278,   779,   211,   780,   136,  1000,   448,
    1234,   798,   799,   319,   406,   137,   408,   188,   138,   188,
    1313,   282,  1392,  1394,  1549,   863,  1578,  1450,  1452,  1454,
      -9,  1456,   188,  1318,   287,   291,   559,   560,   139,   958,
     782,   959,   780,   806,   807,   565,   561,   296,  1387,   567,
     960,   961,   140,   301,   978,   979,   980,   477,   981,   478,
     141,   324,   982,   308,   177,  1487,  1488,  1489,   142,   325,
    1492,  1131,  1132,   855,  1133,    16,   144,    17,    18,    19,
      20,    21,    22,   145,    23,    24,    25,    26,    27,   326,
     146,    28,    29,   358,   359,   360,   361,    30,   709,    31,
     482,   327,   483,   654,   190,  1397,   147,   659,   781,   781,
      32,    33,   492,   663,   493,   562,   632,   633,   148,   328,
     634,   635,    34,   625,   626,   627,   628,   149,    35,   152,
     329,   358,   359,   360,   361,   563,   330,    36,   710,   711,
     712,   713,   153,    37,  1003,  1004,   362,  1005,    38,   935,
     936,   688,   691,   692,   693,   694,   331,   154,  1576,   698,
     612,    39,   613,   155,    40,    41,   622,   156,   623,   704,
     705,  1350,   159,  1015,  1016,    42,  1017,   177,    43,   553,
     554,   180,   556,  1248,  1249,   560,  1250,  1085,   188,  1086,
    1351,   785,   786,   787,   788,   743,   632,   633,   819,   645,
     646,   647,   648,   649,   650,  1165,  1166,  1167,  1168,   560,
     181,   925,   926,   927,   928,    44,  1171,  1172,  1173,  1174,
     827,   182,   828,   629,  1428,  1429,  1430,  1431,    45,    46,
      47,   853,   183,   854,   332,    48,   184,  1342,  1343,  1508,
    1344,   185,   333,   334,   984,    49,   985,    50,  1443,  1444,
    1069,  1445,  1070,   186,   560,  1140,  1477,  1478,  1159,  1479,
    1160,  1258,  1259,   821,   190,  1319,  1234,   825,   190,   908,
     190,  1205,   187,  1206,   630,   516,   517,   518,   519,   520,
     521,   522,   523,    51,   188,    52,    53,  1352,  1353,  1289,
     191,  1290,   858,   859,  1527,  1528,   192,  1529,  1306,  1307,
     193,  1503,   194,   195,   196,   197,   198,   199,   313,   871,
     560,   200,   316,   201,   202,   203,   204,  1573,  1121,   205,
     317,   323,    54,   206,   592,   207,  1568,   593,   594,   208,
     572,   573,  1354,  1355,  1356,  1357,  1358,  1359,  1360,  1361,
     595,   596,   597,   598,   213,   599,   579,   889,   215,   600,
     601,   602,   603,   516,   517,   518,   519,   520,   521,   522,
     523,   216,   950,   217,   322,   952,   336,   218,   311,   955,
     337,   312,   314,   338,   962,   315,  1245,  1362,   752,   753,
     754,   755,   756,   757,   758,   759,   760,   761,   341,  1363,
     340,   570,   571,  1364,   572,   573,   349,   343,  1365,  1366,
    1367,  1368,   345,   352,   574,   575,   576,   577,   762,   578,
     579,   354,   357,   364,   365,   368,   369,   580,   371,   373,
     378,   381,   383,  1644,   516,   517,   518,   519,   520,   521,
     522,   523,   389,   390,   392,   763,   717,   718,   719,   720,
     721,   722,   723,   724,  1021,  1022,  1023,  1024,  1025,  1026,
    1027,  1028,   676,   677,   678,   679,   680,   681,   682,   683,
     604,   560,   396,   983,   400,   401,   402,   403,   990,   605,
     404,   410,   411,   419,   412,   423,   424,   425,   426,   606,
     581,   582,   583,   427,   428,   466,   467,   472,   473,  1418,
     476,   486,   490,   491,  1012,  1079,   496,   514,  1091,   515,
     526,   527,   532,   528,   533,   529,   534,   535,  1034,   536,
     537,   538,   539,   540,   541,   542,   544,   545,   543,   548,
     569,   607,   590,   608,   584,   560,   616,   617,   621,   631,
     640,   641,   642,   643,   651,   652,   658,   661,   662,   667,
     684,   668,   669,   685,   585,   586,   686,   696,   701,   609,
     703,   708,   610,   783,   715,   726,   732,   736,   190,   746,
     749,   751,   769,   771,  -503,   778,   795,  1315,   801,   796,
     802,   817,   836,   832,   833,   834,   839,   850,   861,   840,
     862,   841,   842,   864,   865,   587,   843,   588,   844,   845,
     846,   847,   848,   875,   876,  1292,   872,   877,   878,   879,
     868,   882,   884,   885,   888,   890,   891,   892,   893,  1441,
     894,   895,   898,   899,   900,   901,  1136,   907,   910,   967,
     911,   912,   913,   914,   190,   917,   918,   909,   920,   921,
     922,   923,   931,   924,   934,   938,   939,   940,  1145,   941,
     942,   943,   930,   963,   964,   965,   970,   971,   972,   987,
     973,   997,   996,  1001,  1013,  1041,  1014,  1051,  1009,  1042,
    1043,  1045,  1047,  1050,  1072,  1127,  1054,  1055,  1056,  1057,
    1058,  1061,  1062,  1063,  1064,  1066,  1031,  1067,  1077,  1107,
    1098,  1099,  1100,  1244,  1102,  1103,  1113,  1114,  1115,  1120,
    1122,  1123,  -541,  1151,  1149,  1142,  1438,  1146,  1147,  1148,
    1154,  1155,  1156,  1162,  1178,  1187,  1194,  1196,  1198,  1199,
    1200,  1210,  1202,  1203,  1208,  1212,  1213,  1214,  1218,  1228,
    1204,  1215,  1216,  1217,  1219,  1224,  1225,  1226,  1227,  1241,
    1255,  1251,  1253,  1163,  1257,  1260,  1264,  1282,  1265,  1240,
    1266,  1267,  1269,  1270,  1271,   190,  1272,  1275,  1277,  1278,
    1279,  1280,  1284,  1281,  1285,  1286,  1287,  1288,  1298,  1296,
    1302,  1310,  1316,  1300,  1303,  1322,  1323,  1325,  1324,  1330,
    1329,  1518,  1335,  1336,  1337,  1338,  1340,  1341,  1347,  1345,
    1346,  1373,  1376,  1375,  1509,  1382,  1390,  1317,  1379,  1380,
    1381,  1396,  1401,  1402,  1403,  1404,  1570,  1434,  1408,  1409,
    1410,  1415,  1419,  1420,  1485,  1371,  1421,  1422,  1423,  1424,
    1425,  1413,  1414,  1426,  1427,  1439,  1442,  1446,  1465,  1470,
    1475,  1105,  1468,  1476,  1481,  1482,  1612,  1484,  1469,  1494,
    1495,  1496,  1497,  1490,  1472,  1499,  1502,  1512,  1500,  1491,
    1522,  1504,  1550,  1505,  1493,  1506,  1542,  1507,  1517,  1543,
    1510,  1513,  1536,  1515,  1516,  1539,  1541,  1544,  1551,  1552,
    1595,  1555,  1556,  1557,  1560,  1540,  1561,  1562,  1563,  1598,
    1580,  1566,  1574,  1558,  1571,  1564,  1572,  1579,  1581,  1582,
    1583,   132,  1370,  1584,  1588,  1592,  1569,  1594,  1596,  1597,
    1607,  1608,  1610,  1611,  1613,  1614,  1615,  1369,  1616,  1617,
    1619,  1372,  1620,  1567,  1626,  1627,  1618,  1630,  1234,  1599,
    1632,  1633,  1634,  1600,  1601,  1636,  1628,  1639,  1637,  1638,
    1640,  1631,  1602,  1641,  1603,  1604,  1642,  1643,  1211,  1648,
    1646,  1647,  1649,  1653,  1654,  1656,  1605,  1657,  1658,  1661,
    1662,  1663,  1664,   248,  1299,   745,   995,  1006,  1239,  1467,
    1466,   969,  1018,  1243,   750,  1247,   611,   620,  1273,  1406,
    1049,  1081,   167,  1068,  1177,  1293,   951,     0,     0,  1652,
     657,     0,  1606,     0,     0,     0,  1440,  1645,   190,  1621,
       0,     0,     0,     0,  1498,     0,     0,  1622,     0,     0,
       0,  1625,     0,     0,     0,  1629,  1650,  1651,     0,  1655,
       0,  1660,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,  1659,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,  1538,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   399,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,  1624,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,  1635,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,  1609,     0,     0,     0,   190,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,  1623
};

static const yytype_int16 yycheck[] =
{
     126,   404,   189,   347,   596,   500,   883,   351,   950,   353,
    1129,     4,     4,     4,    30,     4,   257,    30,     6,     6,
     962,     6,     4,   367,     4,     6,   370,     6,     6,     6,
      30,     6,    30,     6,     6,     6,   380,     6,    48,     6,
      30,    30,    28,    58,    96,    28,    30,     1,    71,    58,
      96,    58,    58,     7,    91,     9,    10,    11,    12,    13,
      14,    96,    16,    17,    18,    19,    20,    96,    19,    23,
      24,   166,    96,    31,    96,    29,    96,    31,    96,    96,
      76,    77,    78,    79,    41,    96,    21,     4,    42,    43,
      28,    96,   159,  1169,   175,   126,   175,     0,     4,    96,
      54,  1177,    96,    30,   266,   104,    60,   269,   266,   108,
     109,   269,   170,   171,    71,    69,    76,    77,    78,    79,
      96,    75,    49,    50,    30,   469,    80,  1246,    45,   266,
     533,   268,   125,    96,    96,     8,   167,  1079,   233,    93,
     172,   173,    96,    97,  1086,     4,    96,   157,   158,  1091,
      48,   174,    96,   107,   266,  1063,   110,    55,  1066,   174,
     175,    96,    96,    96,   567,   660,   266,    36,    37,   664,
     266,   666,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    41,   263,    52,   263,   184,   175,   224,   225,   226,
      59,   104,   266,   147,   268,   108,   219,     6,   156,   188,
     151,   152,   217,   218,   219,    94,   160,   161,   162,   166,
     167,     3,     4,   167,     6,   267,   163,   164,   165,   148,
     149,   267,   169,   177,   266,   179,   268,   242,   166,    72,
      73,    74,   267,   216,   243,   242,   242,     6,   267,   255,
     167,   264,   255,   267,     6,   267,   261,   267,     6,   267,
     267,   260,   268,   266,     9,   255,   267,   255,     6,   269,
     386,   215,   267,   217,   218,   255,   266,   228,   266,   230,
     267,   255,   258,   267,   270,   266,   272,     6,   271,   271,
     269,   100,   101,   271,   271,     6,   271,   269,     6,   269,
     271,   267,   271,   271,   271,   698,   271,   270,   270,   270,
     254,   270,   269,  1245,   267,   267,   166,   167,     6,   247,
     270,   249,   272,   118,   119,   441,   176,   267,   187,   445,
     258,   259,     6,   267,   251,   252,   253,   266,   255,   268,
       6,     1,   259,   267,   267,  1421,  1422,  1423,     6,     9,
    1426,    36,    37,   687,    39,     7,     6,     9,    10,    11,
      12,    13,    14,     6,    16,    17,    18,    19,    20,    29,
       6,    23,    24,    76,    77,    78,    79,    29,   180,    31,
     266,    41,   268,   499,   500,  1317,     6,   503,   619,   620,
      42,    43,   266,   509,   268,   245,    98,    99,     3,    59,
     102,   103,    54,    81,    82,    83,    84,     3,    60,     6,
      70,    76,    77,    78,    79,   265,    76,    69,   220,   221,
     222,   223,     6,    75,     3,     4,    91,     6,    80,   120,
     121,   547,   220,   221,   222,   223,    96,     6,  1547,   555,
     266,    93,   268,     6,    96,    97,   266,     6,   268,   565,
     566,    28,     6,     3,     4,   107,     6,   267,   110,    36,
      37,     4,    39,     3,     4,   167,     6,   268,   269,   954,
      47,    87,    88,    89,    90,   591,    98,    99,   655,   112,
     113,   114,   115,   116,   117,    87,    88,    89,    90,   167,
     266,   150,   151,   152,   153,   147,    87,    88,    89,    90,
     266,     4,   268,   181,   236,   237,   238,   239,   160,   161,
     162,   266,    95,   268,   174,   167,     4,     3,     4,    25,
       6,     4,   182,   183,   266,   177,   268,   179,     3,     4,
     266,     6,   268,     4,   167,  1020,     3,     4,   266,     6,
     268,   154,   155,   659,   660,   268,   269,   663,   664,   780,
     666,   266,     4,   268,   232,    61,    62,    63,    64,    65,
      66,    67,    68,   215,   269,   217,   218,   144,   145,   266,
     266,   268,   688,   689,     3,     4,     6,     6,  1226,  1227,
       4,  1448,   266,   266,   266,   266,   266,   266,     6,   705,
     167,   266,     6,   266,   266,   266,   266,    25,   991,   266,
       6,     4,   254,   266,    30,   266,  1538,    33,    34,   266,
      36,    37,   189,   190,   191,   192,   193,   194,   195,   196,
      46,    47,    48,    49,   266,    51,    52,   743,   266,    55,
      56,    57,    58,    61,    62,    63,    64,    65,    66,    67,
      68,   266,   819,   266,    25,   822,     6,   266,   266,   826,
      20,   266,   266,    19,   831,   266,  1141,   234,   205,   206,
     207,   208,   209,   210,   211,   212,   213,   214,    10,   246,
      25,    33,    34,   250,    36,    37,    43,    42,   255,   256,
     257,   258,    31,    54,    46,    47,    48,    49,   235,    51,
      52,    69,    93,    75,    80,    97,   269,    59,   147,   107,
     110,   179,   215,  1635,    61,    62,    63,    64,    65,    66,
      67,    68,   217,    28,   218,   262,   197,   198,   199,   200,
     201,   202,   203,   204,    61,    62,    63,    64,    65,    66,
      67,    68,   535,   536,   537,   538,   539,   540,   541,   542,
     166,   167,     4,   859,   254,    48,   266,     6,   864,   175,
       6,     6,     6,   162,   266,    22,     4,     6,     4,   185,
     122,   123,   124,     4,     4,     4,     4,   269,   269,  1351,
       4,     4,   266,     4,   890,   952,     4,    60,   955,     6,
     270,   270,    27,   270,     6,   270,     4,     4,   904,     4,
       4,     4,     4,     4,     4,     4,   268,    27,     6,     4,
     269,   227,    28,   229,   166,   167,     4,     4,     4,     4,
       4,     4,     4,   111,     4,     4,     4,     4,     4,    25,
     266,    25,     6,    76,   186,   187,     6,     6,     6,   255,
       6,     4,   258,   268,     6,     6,     6,     6,   954,     6,
       6,     6,     6,     6,     4,     4,     4,  1240,     4,     6,
       4,   268,     3,     6,     6,   266,     4,   175,     4,   266,
       4,   266,   266,     4,     4,   227,   266,   229,   266,   266,
     266,   266,   266,     4,     4,  1209,   269,     4,     4,     4,
      71,     4,     6,     4,     4,     4,     4,     4,     4,  1374,
       4,     4,     4,     4,     4,     4,  1012,     4,     6,   146,
       6,     6,     6,     4,  1020,     4,   269,    92,     4,   270,
       4,     6,   266,     6,   266,     4,     4,     4,  1034,     4,
       4,     4,   105,    26,    26,    25,     4,     6,   266,     4,
      28,     4,     6,     4,     6,     4,     6,   231,    53,     6,
       4,     4,     4,     4,   268,   270,     6,     6,     6,     6,
       6,     4,     4,     4,     4,     4,    53,     4,     6,   178,
       6,     6,     6,  1140,     6,     6,     6,     6,     6,     6,
       6,     4,     4,   266,     4,     6,  1369,     6,     6,     6,
       4,     6,   104,     4,   166,     4,   266,     4,     4,     4,
       4,     4,    27,    27,     6,     4,     4,     4,     4,   269,
      26,     6,     6,     6,     4,     6,     4,     4,     4,   146,
     270,   269,   269,   105,   105,     4,     6,     4,     6,  1135,
       6,     6,     6,     6,     6,  1141,     6,     6,     6,     6,
       6,     6,     6,    56,     6,     6,     6,   166,     6,   268,
       6,     6,     6,   268,   268,     4,     4,     4,   269,     4,
     266,   106,     6,     6,     6,     6,     4,     6,    27,     6,
       6,     6,     4,   240,  1457,     4,     4,  1244,   270,   270,
     270,     6,     4,     4,     4,     4,    26,     3,     6,     6,
       4,     6,     4,     6,    28,   175,     6,     6,     6,     6,
       6,   266,   266,     6,     6,     6,     6,     4,     6,     4,
       6,   969,   270,     6,     6,     4,  1591,     4,   270,     4,
       4,     4,     4,    28,   269,     6,     4,     6,   268,    28,
       4,   270,     4,   270,    28,   270,     6,   270,   270,    25,
     269,   269,   175,   269,   269,   268,   270,     6,     4,     6,
      26,     6,     6,     6,     4,   126,     4,     4,     4,    25,
       4,     6,  1545,   175,     6,   175,     6,     6,     4,     4,
       4,    29,  1295,     6,     6,     6,   248,     6,     6,     6,
       6,     6,     6,     6,     6,     4,    27,  1293,     6,     6,
       6,  1297,     6,   269,     4,     6,    26,     6,   269,   269,
       6,     4,     4,   270,   270,     4,    27,    27,     6,     6,
       6,    26,   270,     6,   270,   270,     4,     4,  1109,    27,
       6,     6,     6,     6,     6,     6,   270,     6,     6,     6,
       6,     6,     6,   164,  1218,   593,   866,   880,  1130,  1400,
    1399,   838,   896,  1139,   598,  1144,   459,   473,  1176,  1328,
     917,   953,    95,   943,  1067,  1209,   820,    -1,    -1,   241,
     501,    -1,   270,    -1,    -1,    -1,  1372,   268,  1374,   269,
      -1,    -1,    -1,    -1,  1441,    -1,    -1,   269,    -1,    -1,
      -1,   270,    -1,    -1,    -1,   270,   270,   270,    -1,   270,
      -1,   244,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   268,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,  1498,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   306,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,  1612,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,  1624,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,  1587,    -1,    -1,    -1,  1591,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,  1609
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int16 yystos[] =
{
       0,   159,   274,   275,   276,     0,     8,   277,     4,   170,
     171,     1,   278,   266,   266,   266,     7,     9,    10,    11,
      12,    13,    14,    16,    17,    18,    19,    20,    23,    24,
      29,    31,    42,    43,    54,    60,    69,    75,    80,    93,
      96,    97,   107,   110,   147,   160,   161,   162,   167,   177,
     179,   215,   217,   218,   254,   279,   280,   281,   283,   284,
     286,   288,   290,   291,   308,   309,   310,   311,   313,   316,
     318,   319,   323,   324,   352,   362,   364,   373,   374,   376,
     377,   398,   399,   411,   412,   413,   452,   453,   517,   555,
     557,   558,   575,   576,   577,   578,   593,   594,   619,   620,
     637,   638,   653,   654,   666,   667,   702,   703,   715,   716,
     735,   736,   746,   747,   768,   769,   788,   789,   282,     6,
     285,    94,   287,   289,   312,   314,   317,     6,     6,   172,
     173,   365,   365,   353,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     9,     6,     6,     6,     6,     3,     3,
     292,   414,     6,     6,     6,     6,     6,   320,   325,     6,
     378,   400,   416,   454,   518,   559,   579,   579,   595,   621,
     639,   655,   668,   706,   718,   738,   749,   267,   772,   791,
       4,   266,     4,    95,     4,     4,     4,     4,   269,   392,
     395,   266,     6,     4,   266,   266,   266,   266,   266,   266,
     266,   266,   266,   266,   266,   266,   266,   266,   266,   293,
       4,   266,   415,   266,   704,   266,   266,   266,   266,    21,
      96,   321,   322,    96,   267,   326,   351,   363,    96,   267,
     379,   397,    96,   267,   401,    96,   267,   417,   418,   419,
     451,    96,   267,   455,   456,   457,   515,    96,   456,   519,
     556,    96,   267,   560,   561,   574,    96,   267,   580,   581,
     586,   592,    96,   591,    96,   267,   596,   597,   618,    96,
     267,   622,   623,   636,    96,   267,   640,    96,   267,   656,
     665,    96,   267,   669,   670,   692,    96,   267,   705,   707,
      96,   267,   717,   719,   720,    96,   267,   737,   739,   740,
      96,   267,   748,   750,   753,   773,   771,    96,   267,   790,
     792,   266,   266,     6,   266,   266,     6,     6,     6,   271,
     393,   395,    25,     4,     1,     9,    29,    41,    59,    70,
      76,    96,   174,   182,   183,   294,     6,    20,    19,   327,
      25,    10,   380,    42,   402,    31,   420,   422,   421,    43,
     458,   468,    54,   520,    69,   562,   563,    93,    76,    77,
      78,    79,    91,   582,    75,    80,   598,   599,    97,   269,
     625,   147,   641,   107,   104,   108,   109,   184,   110,   671,
     675,   179,   708,   215,    28,   216,    30,   255,   733,   217,
      28,   743,   218,    28,   258,   756,     4,    96,   770,   772,
     254,    48,   266,     6,     6,     6,   271,     6,   271,   394,
       6,     6,   266,   295,   301,   298,   303,   299,   296,   162,
     300,   297,   302,    22,     4,     6,     4,     4,     4,   266,
     268,   375,   423,   424,   425,   427,   430,   432,   434,   437,
     439,   443,   444,   447,   448,   449,   450,     4,   271,     4,
      45,   459,   266,   268,   375,   469,   484,   486,   266,   268,
     375,   521,   522,   536,   538,   540,     4,     4,   564,   565,
     583,   584,   269,   269,   587,   589,     4,   266,   268,   375,
     601,   624,   266,   268,   375,   626,     4,   657,   659,   660,
     266,     4,   266,   268,   375,   676,     4,   721,   723,   395,
     734,   732,   741,    30,   255,   266,   744,   754,   751,    30,
     255,   266,   757,   774,    60,     6,    61,    62,    63,    64,
      65,    66,    67,    68,   315,   315,   270,   270,   270,   270,
     266,   393,    27,     6,     4,     4,     4,     4,     4,     4,
       4,     4,     4,     6,   268,    27,   381,   403,     4,    32,
      33,    34,    35,    36,    37,    38,    39,    40,    41,   166,
     167,   176,   245,   265,     4,   395,   433,   395,   460,   269,
      33,    34,    36,    37,    46,    47,    48,    49,    51,    52,
      59,   122,   123,   124,   166,   186,   187,   227,   229,   492,
      28,   488,    30,    33,    34,    46,    47,    48,    49,    51,
      55,    56,    57,    58,   166,   175,   185,   227,   229,   255,
     258,   492,   266,   268,   375,   566,     4,     4,   582,   585,
     585,     4,   266,   268,   590,    81,    82,    83,    84,   181,
     232,     4,    98,    99,   102,   103,   635,   148,   149,   643,
       4,     4,     4,   111,   672,   112,   113,   114,   115,   116,
     117,     4,     4,   729,   395,   392,   266,   733,     4,   395,
     745,     4,     4,   395,   758,   775,   793,    25,    25,     6,
     315,   163,   164,   165,   169,   304,   304,   304,   304,   304,
     304,   304,   304,   304,   266,    76,     6,   382,   395,   404,
     428,   220,   221,   222,   223,   431,     6,   426,   395,   445,
     440,     6,   435,     6,   395,   395,   315,   462,     4,   180,
     220,   221,   222,   223,   483,     6,   472,   197,   198,   199,
     200,   201,   202,   203,   204,   348,     6,    91,   224,   225,
     226,   547,     6,   473,   487,   477,     6,   480,   474,   475,
     471,   470,   489,   395,   528,   431,     6,   531,   348,     6,
     547,     6,   205,   206,   207,   208,   209,   210,   211,   212,
     213,   214,   235,   262,   516,   537,   539,   541,   532,     6,
     524,     6,   526,   529,   523,    41,    71,   166,     4,   270,
     272,   582,   270,   268,   588,    87,    88,    89,    90,   602,
     603,   604,   605,   606,   607,     4,     6,   627,   100,   101,
     642,     4,     4,   104,   108,   661,   118,   119,   673,   677,
     678,   679,   680,   681,   682,   709,   722,   268,   730,   393,
     742,   395,   392,   755,   752,   395,   392,   266,   268,   776,
     784,   392,     6,     6,   266,   354,     3,   306,   305,     4,
     266,   266,   266,   266,   266,   266,   266,   266,   266,   328,
     175,   366,   367,   266,   268,   375,   383,   390,   395,   395,
     405,     4,     4,   315,     4,     4,   441,   442,    71,   436,
     438,   395,   269,   463,   461,     4,     4,     4,     4,     4,
     481,   482,     4,   476,     6,     4,   493,   496,     4,   395,
       4,     4,     4,     4,     4,     4,   543,   544,     4,     4,
       4,     4,   548,   551,   568,   572,   567,     4,   582,    92,
       6,     6,     6,     6,     4,   609,   613,     4,   269,   608,
       4,   270,     4,     6,     6,   150,   151,   152,   153,   644,
     105,   266,   662,   664,   266,   120,   121,   674,     4,     4,
       4,     4,     4,     4,   710,   724,    71,   174,   219,   264,
     393,   744,   393,   759,   763,   393,    28,   166,   247,   249,
     258,   259,   393,    26,    26,    25,   355,   146,   546,   546,
       4,     6,   266,    28,   369,    30,    49,    50,   251,   252,
     253,   255,   259,   395,   266,   268,   406,     4,   429,     6,
     395,   446,     3,     4,     6,   442,     6,     4,     4,   125,
     271,     4,   478,     3,     4,     6,   482,   493,   497,    53,
     494,   485,   395,     6,     6,     3,     4,     6,   544,   525,
     527,    61,    62,    63,    64,    65,    66,    67,    68,   533,
     552,    53,   549,     4,   395,   569,    72,    73,    74,   573,
     570,     4,     6,     4,   600,     4,   610,     4,   614,   600,
       4,   231,   617,   628,     6,     6,     6,     6,     6,   645,
     658,     4,     4,     4,     4,   688,     4,     4,   688,   266,
     268,   711,   268,   725,   726,   727,   731,     6,   394,   393,
     268,   757,   760,   761,   766,   268,   392,   762,   764,   765,
     767,   393,   777,   785,   781,   779,   780,   394,     6,     6,
       6,   357,     6,     6,   307,   307,   329,   178,   368,   370,
     384,   391,   387,     6,     6,     6,   385,   388,   166,   233,
       6,   315,     6,     4,   464,   466,   465,   270,   693,   508,
     495,    36,    37,    39,   490,   491,   395,   542,   534,   535,
     392,   530,     6,   554,   550,   395,     6,     6,     6,     4,
     571,   266,   611,   615,     4,     6,   104,   629,   632,   266,
     268,   646,     4,   105,   635,    87,    88,    89,    90,   690,
     691,    87,    88,    89,    90,   689,   635,   690,   166,    58,
     174,   175,   217,   218,   219,   242,   261,     4,   394,   175,
     263,   175,   263,   394,   266,   394,     4,   786,     4,     4,
       4,   266,    27,    27,    26,   266,   268,   358,     6,   330,
       4,   372,     4,     4,     4,     6,     6,     6,     4,     4,
     407,   228,   230,   410,     6,     4,     4,     4,   269,   479,
     694,    48,   157,   158,   269,   505,   509,   510,   512,   496,
     395,   146,   545,   548,   393,   392,   513,   551,     3,     4,
       6,   269,   612,   269,   616,   270,   633,   105,   154,   155,
       4,   663,   683,   685,     6,     6,     6,     6,   691,     6,
       6,     6,     6,   683,   712,     6,   728,     6,     6,     6,
       6,    56,     4,   787,     6,     6,     6,     6,   166,   266,
     268,   331,   375,   449,   371,   372,   268,   396,     6,   396,
     268,   408,     6,   268,   375,   467,   467,   467,   695,   697,
       6,   511,     6,   271,   498,   315,     6,   393,   394,   268,
     505,   514,     4,     4,   269,     4,   630,   647,   648,   266,
       4,    19,   151,   152,   686,     6,     6,     6,     6,   713,
       4,     6,     3,     4,     6,     6,     6,    27,   356,   359,
      28,    47,   144,   145,   189,   190,   191,   192,   193,   194,
     195,   196,   234,   246,   250,   255,   256,   257,   258,   395,
     371,   175,   395,     6,   386,   240,     4,   409,   126,   270,
     270,   270,     4,    36,    37,    52,    59,   187,   698,   701,
       4,     6,   271,     6,   271,   501,     6,   394,    48,    55,
     553,     4,     4,     4,     4,   649,   649,   684,     6,     6,
       4,   714,   778,   266,   266,     6,   360,   334,   348,     4,
       6,     6,     6,     6,     6,     6,     6,     6,   236,   237,
     238,   239,   347,   333,     3,   336,   332,   338,   315,     6,
     395,   392,     6,     3,     4,     6,     4,   700,   699,     6,
     270,     6,   270,     6,   270,     6,   270,     4,    30,   175,
     188,   499,   500,   502,   505,     6,   516,   501,   270,   270,
       4,   631,   269,   650,   687,     6,     6,     3,     4,     6,
     782,     6,     4,   361,     4,    28,   349,   349,   349,   349,
      28,    28,   349,    28,     4,     4,     4,     4,   393,     6,
     268,   696,     4,   493,   270,   270,   270,   270,    25,   315,
     269,   507,     6,   269,   506,   269,   269,   270,   106,   634,
      31,   156,     4,    58,   243,   260,   783,     3,     4,     6,
     335,   350,   339,   340,   341,   337,   175,   343,   393,   268,
     126,   270,     6,    25,     6,     4,    30,   504,     6,   271,
       4,     4,     6,   652,   651,     6,     6,     6,   175,   342,
       4,     4,     4,     4,   175,   344,     6,   269,   394,   248,
      26,     6,     6,    25,   315,   503,   505,     6,   271,     6,
       4,     4,     4,     4,     6,    58,   242,   345,     6,    58,
     242,   346,     6,   389,     6,    26,     6,     6,    25,   269,
     270,   270,   270,   270,   270,   270,   270,     6,     6,   395,
       6,     6,   392,     6,     4,    27,     6,     6,    26,     6,
       6,   269,   269,   395,   393,   270,     4,     6,    27,   270,
       6,    26,     6,     4,     4,   393,     4,     6,     6,    27,
       6,     6,     4,     4,   394,   268,     6,     6,    27,     6,
     270,   270,   241,     6,     6,   270,     6,     6,     6,   268,
     244,     6,     6,     6,     6
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int16 yyr1[] =
{
       0,   273,   274,   275,   276,   275,   277,   277,   277,   278,
     278,   278,   279,   279,   279,   279,   279,   279,   279,   279,
     279,   279,   279,   279,   279,   279,   279,   279,   279,   279,
     279,   279,   279,   279,   280,   280,   280,   280,   280,   280,
     280,   280,   280,   280,   280,   280,   280,   280,   280,   280,
     280,   282,   281,   283,   285,   284,   287,   286,   289,   288,
     290,   292,   291,   293,   293,   295,   294,   296,   294,   297,
     294,   298,   294,   299,   294,   300,   294,   301,   294,   302,
     294,   303,   294,   294,   305,   304,   306,   304,   304,   304,
     304,   307,   307,   308,   309,   310,   312,   311,   314,   313,
     315,   315,   315,   315,   315,   315,   315,   315,   317,   316,
     318,   319,   320,   320,   321,   322,   323,   324,   325,   325,
     327,   328,   329,   326,   330,   330,   331,   331,   331,   331,
     332,   331,   333,   331,   331,   331,   334,   335,   331,   336,
     337,   331,   338,   331,   331,   331,   331,   331,   331,   339,
     331,   340,   331,   331,   341,   331,   331,   342,   342,   343,
     343,   344,   344,   345,   345,   345,   346,   346,   346,   347,
     347,   347,   347,   348,   348,   348,   348,   348,   348,   348,
     348,   349,   350,   349,   351,   353,   354,   352,   355,   355,
     356,   356,   357,   357,   359,   358,   360,   360,   361,   361,
     361,   363,   362,   364,   365,   365,   366,   367,   367,   368,
     368,   369,   370,   369,   371,   371,   372,   373,   374,   375,
     376,   377,   378,   378,   380,   381,   379,   382,   382,   384,
     383,   385,   386,   383,   387,   383,   388,   389,   383,   383,
     383,   390,   390,   390,   391,   390,   392,   393,   394,   394,
     395,   395,   395,   395,   396,   396,   397,   398,   399,   400,
     400,   402,   403,   401,   404,   404,   405,   405,   407,   406,
     406,   408,   408,   409,   409,   409,   410,   410,   411,   412,
     413,   414,   414,   415,   416,   416,   417,   418,   420,   419,
     421,   421,   421,   422,   422,   423,   423,   423,   423,   423,
     423,   423,   423,   423,   423,   423,   423,   424,   426,   425,
     428,   427,   429,   429,   430,   431,   431,   431,   431,   432,
     432,   433,   433,   435,   434,   436,   436,   438,   437,   440,
     439,   441,   441,   442,   442,   442,   443,   445,   444,   446,
     446,   447,   447,   447,   448,   449,   449,   449,   450,   451,
     452,   453,   454,   454,   455,   456,   458,   457,   460,   459,
     461,   459,   462,   462,   464,   463,   465,   463,   466,   463,
     467,   467,   467,   468,   468,   470,   469,   469,   469,   471,
     469,   472,   469,   469,   469,   469,   469,   469,   469,   473,
     469,   469,   474,   469,   475,   476,   469,   477,   478,   479,
     469,   480,   469,   469,   481,   481,   482,   482,   482,   483,
     483,   483,   483,   483,   485,   484,   487,   486,   488,   489,
     488,   490,   490,   491,   491,   491,   492,   492,   492,   493,
     493,   495,   494,   497,   498,   496,   499,   500,   501,   501,
     502,   502,   502,   502,   502,   502,   502,   502,   502,   502,
     503,   502,   504,   502,   502,   505,   505,   505,   505,   505,
     505,   505,   505,   506,   506,   506,   507,   508,   508,   509,
     509,   510,   511,   510,   512,   513,   513,   514,   514,   515,
     516,   516,   516,   516,   516,   516,   516,   516,   516,   516,
     516,   516,   517,   518,   518,   519,   520,   520,   521,   521,
     521,   521,   522,   523,   522,   524,   525,   522,   522,   522,
     526,   527,   522,   528,   522,   529,   530,   522,   522,   522,
     522,   531,   522,   522,   522,   522,   522,   522,   532,   522,
     522,   533,   533,   533,   533,   533,   533,   533,   533,   533,
     534,   535,   534,   537,   536,   539,   538,   541,   542,   540,
     543,   543,   544,   544,   544,   545,   545,   546,   546,   547,
     547,   547,   547,   548,   548,   550,   549,   552,   553,   551,
     554,   555,   556,   557,   558,   559,   559,   560,   562,   561,
     563,   563,   564,   565,   565,   566,   567,   566,   568,   566,
     566,   569,   569,   570,   570,   571,   571,   571,   572,   572,
     573,   573,   573,   574,   575,   576,   577,   578,   579,   579,
     580,   580,   581,   583,   582,   584,   582,   582,   582,   585,
     585,   585,   587,   586,   588,   588,   589,   589,   590,   590,
     590,   590,   591,   592,   593,   594,   595,   595,   596,   598,
     597,   599,   599,   600,   600,   602,   601,   603,   601,   604,
     601,   605,   601,   606,   601,   607,   601,   601,   608,   608,
     608,   609,   609,   611,   610,   612,   612,   612,   612,   613,
     613,   615,   614,   616,   616,   616,   616,   617,   617,   618,
     619,   620,   621,   621,   622,   624,   623,   625,   625,   626,
     626,   626,   627,   628,   626,   626,   630,   631,   629,   632,
     633,   632,   634,   634,   635,   635,   636,   637,   638,   639,
     639,   641,   642,   640,   643,   643,   644,   644,   644,   644,
     645,   645,   647,   646,   648,   646,   649,   649,   651,   650,
     652,   650,   653,   654,   655,   655,   657,   658,   656,   659,
     656,   660,   656,   656,   662,   663,   661,   664,   661,   665,
     666,   667,   668,   668,   669,   671,   670,   672,   672,   673,
     673,   673,   674,   674,   674,   675,   675,   677,   676,   678,
     676,   679,   676,   680,   676,   681,   676,   682,   676,   676,
     684,   683,   685,   685,   686,   686,   687,   687,   688,   688,
     689,   689,   689,   689,   690,   690,   691,   691,   691,   691,
     692,   693,   693,   695,   694,   696,   696,   697,   697,   699,
     698,   700,   698,   701,   701,   701,   701,   702,   703,   704,
     704,   705,   706,   706,   708,   709,   707,   710,   710,   712,
     711,   713,   713,   714,   714,   714,   715,   716,   717,   718,
     718,   719,   721,   722,   720,   723,   720,   724,   724,   725,
     725,   725,   725,   726,   728,   727,   727,   727,   727,   727,
     729,   729,   731,   730,   730,   730,   730,   732,   732,   733,
     734,   733,   735,   736,   737,   738,   738,   739,   741,   742,
     740,   743,   743,   744,   745,   744,   746,   747,   748,   749,
     749,   750,   751,   752,   750,   754,   755,   753,   756,   756,
     757,   758,   757,   759,   759,   760,   760,   761,   762,   763,
     763,   764,   764,   765,   766,   767,   768,   769,   770,   771,
     771,   773,   774,   772,   775,   775,   776,   777,   778,   776,
     779,   776,   780,   776,   781,   776,   776,   782,   782,   783,
     783,   783,   785,   784,   786,   786,   787,   787,   787,   788,
     789,   790,   791,   791,   793,   792
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     4,     0,     0,     4,     0,     3,     3,     0,
       2,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     0,     4,     2,     0,     4,     0,     4,     0,     4,
       1,     0,     5,     0,     2,     0,     5,     0,     5,     0,
       5,     0,     5,     0,     5,     0,     5,     0,     5,     0,
       5,     0,     5,     2,     0,     4,     0,     4,     1,     2,
       2,     0,     1,     5,     3,     3,     0,    14,     0,    14,
       1,     1,     1,     1,     1,     1,     1,     1,     0,     6,
       3,     2,     0,     2,     5,     2,     3,     3,     0,     2,
       0,     0,     0,    10,     0,     2,     2,     1,     3,     3,
       0,     4,     0,     4,     3,     2,     0,     0,     9,     0,
       0,    11,     0,     9,     3,     4,     4,     4,     4,     0,
       6,     0,     6,     4,     0,     6,     3,     0,     2,     0,
       2,     0,     2,     0,     2,     2,     0,     2,     2,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     0,     0,     3,     2,     0,     0,    11,     0,     5,
       0,     3,     0,     2,     0,     4,     0,     2,     2,     2,
       2,     0,     9,     2,     1,     1,     2,     0,     3,     0,
       1,     0,     0,     4,     0,     2,     1,     8,     1,     2,
       3,     3,     0,     2,     0,     0,     6,     0,     2,     0,
       7,     0,     0,    10,     0,     4,     0,     0,    24,     1,
       1,     4,     4,     6,     0,     4,     1,     1,     0,     2,
       4,     4,     4,     4,     0,     3,     2,     4,     3,     0,
       2,     0,     0,     7,     2,     3,     0,     2,     0,     4,
       3,     0,     2,     2,     2,     2,     1,     1,     3,     3,
       3,     0,     2,     1,     0,     2,     3,     2,     0,     4,
       0,     2,     2,     0,     2,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     0,     4,
       0,     5,     0,     1,     3,     1,     1,     1,     1,     2,
       2,     2,     3,     0,     8,     0,     1,     0,     6,     0,
       4,     1,     2,     2,     2,     2,     2,     0,     6,     1,
       2,     3,     2,     4,     3,     2,     2,     2,     3,     2,
       3,     3,     0,     2,     3,     1,     0,     3,     0,     3,
       0,     6,     0,     2,     0,     6,     0,     6,     0,     6,
       0,     1,     2,     0,     2,     0,     4,     3,     2,     0,
       4,     0,     4,     3,     3,     3,     3,     3,     3,     0,
       4,     1,     0,     4,     0,     0,     5,     0,     0,     0,
       8,     0,     4,     1,     1,     2,     2,     2,     2,     1,
       1,     1,     1,     1,     0,     6,     0,     4,     0,     0,
       3,     0,     3,     1,     1,     1,     1,     1,     1,     1,
       2,     0,     3,     0,     0,     6,     2,     2,     0,     2,
       1,     3,     2,     4,    10,     8,     9,    11,     1,     1,
       0,    10,     0,     4,     1,     4,     4,     4,     4,     5,
       5,     5,     5,     4,     4,     4,     6,     0,     2,     1,
       1,     1,     0,     3,     2,     0,     2,     3,     3,     2,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     3,     0,     2,     3,     0,     2,     1,     1,
       1,     1,     2,     0,     4,     0,     0,     6,     3,     3,
       0,     0,     9,     0,     6,     0,     0,     8,     3,     2,
       3,     0,     4,     3,     3,     3,     3,     3,     0,     4,
       1,     0,     1,     1,     1,     1,     1,     1,     1,     1,
       0,     0,     2,     0,     5,     0,     4,     0,     0,     7,
       1,     2,     2,     2,     2,     0,     3,     0,     3,     1,
       1,     1,     1,     1,     2,     0,     3,     0,     0,     7,
       1,     3,     2,     3,     3,     0,     2,     4,     0,     3,
       0,     2,     1,     0,     2,     3,     0,     4,     0,     4,
       1,     2,     1,     0,     2,     2,     2,     2,     0,     2,
       2,     2,     2,     2,     3,     3,     3,     3,     0,     2,
       1,     1,     4,     0,     3,     0,     6,     4,     4,     1,
       2,     3,     0,     8,     0,     1,     0,     2,     3,     3,
       3,     3,     2,     2,     3,     3,     0,     2,     3,     0,
       3,     0,     2,     0,     1,     0,     5,     0,     4,     0,
       4,     0,     5,     0,     4,     0,     5,     1,     0,     4,
       8,     0,     2,     0,     3,     0,     4,     8,    12,     0,
       2,     0,     3,     0,     4,     8,    12,     0,     2,     2,
       3,     3,     0,     2,     3,     0,     6,     0,     2,     5,
       5,     3,     0,     0,     6,     1,     0,     0,     6,     0,
       0,     3,     0,     2,     1,     1,     2,     4,     3,     0,
       2,     0,     0,     8,     1,     1,     1,     2,     2,     2,
       0,     2,     0,     4,     0,     4,     0,     2,     0,     5,
       0,     5,     3,     3,     0,     2,     0,     0,    10,     0,
       6,     0,     6,     3,     0,     0,     6,     0,     3,     2,
       3,     3,     0,     2,     3,     0,     4,     0,     3,     0,
       1,     1,     0,     1,     1,     0,     2,     0,     7,     0,
       6,     0,     5,     0,     7,     0,     6,     0,     5,     1,
       0,     4,     0,     2,     3,     3,     0,     2,     0,     2,
       2,     2,     2,     2,     1,     2,     3,     3,     3,     3,
       2,     0,     2,     0,     6,     0,     2,     0,     2,     0,
       3,     0,     3,     1,     1,     1,     1,     3,     3,     0,
       1,     2,     0,     2,     0,     0,     7,     0,     2,     0,
       4,     0,     2,     2,     2,     2,     3,     3,     2,     0,
       2,     4,     0,     0,     6,     0,     4,     0,     2,     3,
       3,     1,     1,     3,     0,     4,     2,     2,     2,     2,
       0,     2,     0,     4,     2,     2,     3,     0,     2,     3,
       0,     6,     3,     3,     2,     0,     2,     3,     0,     0,
       6,     0,     2,     3,     0,     6,     3,     3,     2,     0,
       2,     3,     0,     0,     8,     0,     0,     7,     0,     2,
       3,     0,     6,     0,     2,     1,     1,     2,     2,     0,
       2,     1,     1,     2,     3,     3,     4,     3,     2,     0,
       2,     0,     0,     6,     0,     2,     2,     0,     0,     8,
       0,     4,     0,     4,     0,     5,     1,     0,     2,     2,
       2,     2,     0,     4,     0,     2,     2,     2,     2,     3,
       3,     2,     0,     2,     0,     8
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYNOMEM         goto yyexhaustedlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)




# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  if (!yyvaluep)
    return;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  yy_symbol_value_print (yyo, yykind, yyvaluep);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp,
                 int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)]);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif






/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep)
{
  YY_USE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/* Lookahead token kind.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;




/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */

  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    YYNOMEM;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        YYNOMEM;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          YYNOMEM;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */


  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      goto yyerrlab1;
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 4: /* $@1: %empty  */
#line 319 "def.y"
                { defData->dumb_mode = 1; defData->no_num = 1; }
#line 3385 "def.tab.c"
    break;

  case 5: /* version_stmt: K_VERSION $@1 T_STRING ';'  */
#line 320 "def.y"
      {
        defData->VersionNum = convert_defname2num((yyvsp[-1].string));
        if (defData->VersionNum > CURRENT_VERSION) {
          char temp[300];
          sprintf(temp,
          "The execution has been stopped because the DEF parser %.1f does not support DEF file with version %s.\nUpdate your DEF file to version 5.8 or earlier.",
                  CURRENT_VERSION, (yyvsp[-1].string));
          defError(6503, temp);
          return 1;
        }
        if (defCallbacks->VersionStrCbk) {
          CALLBACK(defCallbacks->VersionStrCbk, defrVersionStrCbkType, (yyvsp[-1].string));
        } else if (defCallbacks->VersionCbk) {
            CALLBACK(defCallbacks->VersionCbk, defrVersionCbkType, defData->VersionNum);
        }
        if (defData->VersionNum > 5.3 && defData->VersionNum < 5.4)
          defData->defIgnoreVersion = 1;
        if (defData->VersionNum < 5.6)     // default to false before 5.6
          defData->names_case_sensitive = defSettings->reader_case_sensitive;
        else
          defData->names_case_sensitive = 1;
        defData->hasVer = 1;
        defData->doneDesign = 0;
    }
#line 3414 "def.tab.c"
    break;

  case 7: /* case_sens_stmt: K_NAMESCASESENSITIVE K_ON ';'  */
#line 347 "def.y"
      {
        if (defData->VersionNum < 5.6) {
          defData->names_case_sensitive = 1;
          if (defCallbacks->CaseSensitiveCbk)
            CALLBACK(defCallbacks->CaseSensitiveCbk, defrCaseSensitiveCbkType,
                     defData->names_case_sensitive); 
          defData->hasNameCase = 1;
        } else
          if (defCallbacks->CaseSensitiveCbk) // write error only if cbk is set 
             if (defData->caseSensitiveWarnings++ < defSettings->CaseSensitiveWarnings)
               defWarning(7011, "The NAMESCASESENSITIVE statement is obsolete in version 5.6 and later.\nThe DEF parser will ignore this statement.");
      }
#line 3431 "def.tab.c"
    break;

  case 8: /* case_sens_stmt: K_NAMESCASESENSITIVE K_OFF ';'  */
#line 360 "def.y"
      {
        if (defData->VersionNum < 5.6) {
          defData->names_case_sensitive = 0;
          if (defCallbacks->CaseSensitiveCbk)
            CALLBACK(defCallbacks->CaseSensitiveCbk, defrCaseSensitiveCbkType,
                     defData->names_case_sensitive);
          defData->hasNameCase = 1;
        } else {
          if (defCallbacks->CaseSensitiveCbk) { // write error only if cbk is set 
            if (defData->caseSensitiveWarnings++ < defSettings->CaseSensitiveWarnings) {
              defError(6504, "Def parser version 5.7 and later does not support NAMESCASESENSITIVE OFF.\nEither remove this optional construct or set it to ON.");
              CHKERR();
            }
          }
        }
      }
#line 3452 "def.tab.c"
    break;

  case 51: /* $@2: %empty  */
#line 400 "def.y"
                      {defData->dumb_mode = 1; defData->no_num = 1; }
#line 3458 "def.tab.c"
    break;

  case 52: /* design_name: K_DESIGN $@2 T_STRING ';'  */
#line 401 "def.y"
      {
            if (defCallbacks->DesignCbk)
              CALLBACK(defCallbacks->DesignCbk, defrDesignStartCbkType, (yyvsp[-1].string));
            defData->hasDes = 1;
          }
#line 3468 "def.tab.c"
    break;

  case 53: /* end_design: K_END K_DESIGN  */
#line 408 "def.y"
          {
            defData->doneDesign = 1;
            if (defCallbacks->DesignEndCbk)
              CALLBACK(defCallbacks->DesignEndCbk, defrDesignEndCbkType, 0);
            // 11/16/2001 - pcr 408334
            // Return 1 if there is any defData->errors during parsing
            if (defData->errors)
                return 1;

            if (!defData->hasVer) {
              char temp[300];
              sprintf(temp, "No VERSION statement found, using the default value %2g.", defData->VersionNum);
              defWarning(7012, temp);            
            }
            if (!defData->hasNameCase && defData->VersionNum < 5.6)
              defWarning(7013, "The DEF file is invalid if NAMESCASESENSITIVE is undefined.\nNAMESCASESENSITIVE is a mandatory statement in the DEF file with version 5.6 and earlier.\nTo define the NAMESCASESENSITIVE statement, refer to the LEF/DEF 5.5 and earlier Language Reference manual.");
            if (!defData->hasBusBit && defData->VersionNum < 5.6)
              defWarning(7014, "The DEF file is invalid if BUSBITCHARS is undefined.\nBUSBITCHARS is a mandatory statement in the DEF file with version 5.6 and earlier.\nTo define the BUSBITCHARS statement, refer to the LEF/DEF 5.5 and earlier Language Reference manual.");
            if (!defData->hasDivChar && defData->VersionNum < 5.6)
              defWarning(7015, "The DEF file is invalid if DIVIDERCHAR is undefined.\nDIVIDERCHAR is a mandatory statement in the DEF file with version 5.6 and earlier.\nTo define the DIVIDERCHAR statement, refer to the LEF/DEF 5.5 and earlier Language Reference manual.");
            if (!defData->hasDes)
              defWarning(7016, "DESIGN is a mandatory statement in the DEF file. Ensure that it exists in the file.");
          }
#line 3496 "def.tab.c"
    break;

  case 54: /* $@3: %empty  */
#line 432 "def.y"
                  { defData->dumb_mode = 1; defData->no_num = 1; }
#line 3502 "def.tab.c"
    break;

  case 55: /* tech_name: K_TECH $@3 T_STRING ';'  */
#line 433 "def.y"
          { 
            if (defCallbacks->TechnologyCbk)
              CALLBACK(defCallbacks->TechnologyCbk, defrTechNameCbkType, (yyvsp[-1].string));
          }
#line 3511 "def.tab.c"
    break;

  case 56: /* $@4: %empty  */
#line 438 "def.y"
                    {defData->dumb_mode = 1; defData->no_num = 1;}
#line 3517 "def.tab.c"
    break;

  case 57: /* array_name: K_ARRAY $@4 T_STRING ';'  */
#line 439 "def.y"
          { 
            if (defCallbacks->ArrayNameCbk)
              CALLBACK(defCallbacks->ArrayNameCbk, defrArrayNameCbkType, (yyvsp[-1].string));
          }
#line 3526 "def.tab.c"
    break;

  case 58: /* $@5: %empty  */
#line 444 "def.y"
                            { defData->dumb_mode = 1; defData->no_num = 1; }
#line 3532 "def.tab.c"
    break;

  case 59: /* floorplan_name: K_FLOORPLAN $@5 T_STRING ';'  */
#line 445 "def.y"
          { 
            if (defCallbacks->FloorPlanNameCbk)
              CALLBACK(defCallbacks->FloorPlanNameCbk, defrFloorPlanNameCbkType, (yyvsp[-1].string));
          }
#line 3541 "def.tab.c"
    break;

  case 60: /* history: K_HISTORY  */
#line 451 "def.y"
          { 
            if (defCallbacks->HistoryCbk)
              CALLBACK(defCallbacks->HistoryCbk, defrHistoryCbkType, &defData->History_text[0]);
          }
#line 3550 "def.tab.c"
    break;

  case 61: /* $@6: %empty  */
#line 457 "def.y"
          {
            defData->parsing_property = 1;
            defData->defInPropDef = 1;     // set flag as inside propertydefinitions 
            if (defCallbacks->PropDefStartCbk)
              CALLBACK(defCallbacks->PropDefStartCbk, defrPropDefStartCbkType, 0);
          }
#line 3561 "def.tab.c"
    break;

  case 62: /* prop_def_section: K_PROPERTYDEFINITIONS $@6 property_defs K_END K_PROPERTYDEFINITIONS  */
#line 464 "def.y"
          { 
            if (defCallbacks->PropDefEndCbk)
              CALLBACK(defCallbacks->PropDefEndCbk, defrPropDefEndCbkType, 0);
            defData->real_num = 0;     // just want to make sure it is reset 
            defData->parsing_property = 0;
            defData->defInPropDef = 0;     // reset flag 
          }
#line 3573 "def.tab.c"
    break;

  case 64: /* property_defs: property_defs property_def  */
#line 474 "def.y"
            { }
#line 3579 "def.tab.c"
    break;

  case 65: /* $@7: %empty  */
#line 476 "def.y"
                       {defData->dumb_mode = 1; defData->no_num = 1; defData->Prop.clear(); }
#line 3585 "def.tab.c"
    break;

  case 66: /* property_def: K_DESIGN $@7 T_STRING property_type_and_val ';'  */
#line 478 "def.y"
            {
              if (defCallbacks->PropCbk) {
                defData->Prop.setPropType("design", (yyvsp[-2].string));
                CALLBACK(defCallbacks->PropCbk, defrPropCbkType, &defData->Prop);
              }
              defSettings->DesignProp.setPropType((yyvsp[-2].string), defData->defPropDefType);
            }
#line 3597 "def.tab.c"
    break;

  case 67: /* $@8: %empty  */
#line 485 "def.y"
                { defData->dumb_mode = 1 ; defData->no_num = 1; defData->Prop.clear(); }
#line 3603 "def.tab.c"
    break;

  case 68: /* property_def: K_NET $@8 T_STRING property_type_and_val ';'  */
#line 487 "def.y"
            {
              if (defCallbacks->PropCbk) {
                defData->Prop.setPropType("net", (yyvsp[-2].string));
                CALLBACK(defCallbacks->PropCbk, defrPropCbkType, &defData->Prop);
              }
              defSettings->NetProp.setPropType((yyvsp[-2].string), defData->defPropDefType);
            }
#line 3615 "def.tab.c"
    break;

  case 69: /* $@9: %empty  */
#line 494 "def.y"
                 { defData->dumb_mode = 1 ; defData->no_num = 1; defData->Prop.clear(); }
#line 3621 "def.tab.c"
    break;

  case 70: /* property_def: K_SNET $@9 T_STRING property_type_and_val ';'  */
#line 496 "def.y"
            {
              if (defCallbacks->PropCbk) {
                defData->Prop.setPropType("specialnet", (yyvsp[-2].string));
                CALLBACK(defCallbacks->PropCbk, defrPropCbkType, &defData->Prop);
              }
              defSettings->SNetProp.setPropType((yyvsp[-2].string), defData->defPropDefType);
            }
#line 3633 "def.tab.c"
    break;

  case 71: /* $@10: %empty  */
#line 503 "def.y"
                   { defData->dumb_mode = 1 ; defData->no_num = 1; defData->Prop.clear(); }
#line 3639 "def.tab.c"
    break;

  case 72: /* property_def: K_REGION $@10 T_STRING property_type_and_val ';'  */
#line 505 "def.y"
            {
              if (defCallbacks->PropCbk) {
                defData->Prop.setPropType("region", (yyvsp[-2].string));
                CALLBACK(defCallbacks->PropCbk, defrPropCbkType, &defData->Prop);
              }
              defSettings->RegionProp.setPropType((yyvsp[-2].string), defData->defPropDefType);
            }
#line 3651 "def.tab.c"
    break;

  case 73: /* $@11: %empty  */
#line 512 "def.y"
                  { defData->dumb_mode = 1 ; defData->no_num = 1; defData->Prop.clear(); }
#line 3657 "def.tab.c"
    break;

  case 74: /* property_def: K_GROUP $@11 T_STRING property_type_and_val ';'  */
#line 514 "def.y"
            {
              if (defCallbacks->PropCbk) {
                defData->Prop.setPropType("group", (yyvsp[-2].string));
                CALLBACK(defCallbacks->PropCbk, defrPropCbkType, &defData->Prop);
              }
              defSettings->GroupProp.setPropType((yyvsp[-2].string), defData->defPropDefType);
            }
#line 3669 "def.tab.c"
    break;

  case 75: /* $@12: %empty  */
#line 521 "def.y"
                      { defData->dumb_mode = 1 ; defData->no_num = 1; defData->Prop.clear(); }
#line 3675 "def.tab.c"
    break;

  case 76: /* property_def: K_COMPONENT $@12 T_STRING property_type_and_val ';'  */
#line 523 "def.y"
            {
              if (defCallbacks->PropCbk) {
                defData->Prop.setPropType("component", (yyvsp[-2].string));
                CALLBACK(defCallbacks->PropCbk, defrPropCbkType, &defData->Prop);
              }
              defSettings->CompProp.setPropType((yyvsp[-2].string), defData->defPropDefType);
            }
#line 3687 "def.tab.c"
    break;

  case 77: /* $@13: %empty  */
#line 530 "def.y"
                { defData->dumb_mode = 1 ; defData->no_num = 1; defData->Prop.clear(); }
#line 3693 "def.tab.c"
    break;

  case 78: /* property_def: K_ROW $@13 T_STRING property_type_and_val ';'  */
#line 532 "def.y"
            {
              if (defCallbacks->PropCbk) {
                defData->Prop.setPropType("row", (yyvsp[-2].string));
                CALLBACK(defCallbacks->PropCbk, defrPropCbkType, &defData->Prop);
              }
              defSettings->RowProp.setPropType((yyvsp[-2].string), defData->defPropDefType);
            }
#line 3705 "def.tab.c"
    break;

  case 79: /* $@14: %empty  */
#line 541 "def.y"
          { defData->dumb_mode = 1 ; defData->no_num = 1; defData->Prop.clear(); }
#line 3711 "def.tab.c"
    break;

  case 80: /* property_def: K_COMPONENTPIN $@14 T_STRING property_type_and_val ';'  */
#line 543 "def.y"
            {
              if (defCallbacks->PropCbk) {
                defData->Prop.setPropType("componentpin", (yyvsp[-2].string));
                CALLBACK(defCallbacks->PropCbk, defrPropCbkType, &defData->Prop);
              }
              defSettings->CompPinProp.setPropType((yyvsp[-2].string), defData->defPropDefType);
            }
#line 3723 "def.tab.c"
    break;

  case 81: /* $@15: %empty  */
#line 551 "def.y"
          { defData->dumb_mode = 1 ; defData->no_num = 1; defData->Prop.clear(); }
#line 3729 "def.tab.c"
    break;

  case 82: /* property_def: K_NONDEFAULTRULE $@15 T_STRING property_type_and_val ';'  */
#line 553 "def.y"
            {
              if (defData->VersionNum < 5.6) {
                if (defData->nonDefaultWarnings++ < defSettings->NonDefaultWarnings) {
                  defData->defMsg = (char*)defMalloc(1000); 
                  sprintf (defData->defMsg,
                     "The NONDEFAULTRULE statement is available in version 5.6 and later.\nHowever, your DEF file is defined with version %g.", defData->VersionNum);
                  defError(6505, defData->defMsg);
                  defFree(defData->defMsg);
                  CHKERR();
                }
              } else {
                if (defCallbacks->PropCbk) {
                  defData->Prop.setPropType("nondefaultrule", (yyvsp[-2].string));
                  CALLBACK(defCallbacks->PropCbk, defrPropCbkType, &defData->Prop);
                }
                defSettings->NDefProp.setPropType((yyvsp[-2].string), defData->defPropDefType);
              }
            }
#line 3752 "def.tab.c"
    break;

  case 83: /* property_def: error ';'  */
#line 571 "def.y"
                    { yyerrok; yyclearin;}
#line 3758 "def.tab.c"
    break;

  case 84: /* $@16: %empty  */
#line 573 "def.y"
                                 { defData->real_num = 0; }
#line 3764 "def.tab.c"
    break;

  case 85: /* property_type_and_val: K_INTEGER $@16 opt_range opt_num_val  */
#line 574 "def.y"
            {
              if (defCallbacks->PropCbk) defData->Prop.setPropInteger();
              defData->defPropDefType = 'I';
            }
#line 3773 "def.tab.c"
    break;

  case 86: /* $@17: %empty  */
#line 578 "def.y"
                 { defData->real_num = 1; }
#line 3779 "def.tab.c"
    break;

  case 87: /* property_type_and_val: K_REAL $@17 opt_range opt_num_val  */
#line 579 "def.y"
            {
              if (defCallbacks->PropCbk) defData->Prop.setPropReal();
              defData->defPropDefType = 'R';
              defData->real_num = 0;
            }
#line 3789 "def.tab.c"
    break;

  case 88: /* property_type_and_val: K_STRING  */
#line 585 "def.y"
            {
              if (defCallbacks->PropCbk) defData->Prop.setPropString();
              defData->defPropDefType = 'S';
            }
#line 3798 "def.tab.c"
    break;

  case 89: /* property_type_and_val: K_STRING QSTRING  */
#line 590 "def.y"
            {
              if (defCallbacks->PropCbk) defData->Prop.setPropQString((yyvsp[0].string));
              defData->defPropDefType = 'Q';
            }
#line 3807 "def.tab.c"
    break;

  case 90: /* property_type_and_val: K_NAMEMAPSTRING T_STRING  */
#line 595 "def.y"
            {
              if (defCallbacks->PropCbk) defData->Prop.setPropNameMapString((yyvsp[0].string));
              defData->defPropDefType = 'S';
            }
#line 3816 "def.tab.c"
    break;

  case 92: /* opt_num_val: NUMBER  */
#line 602 "def.y"
            { if (defCallbacks->PropCbk) defData->Prop.setNumber((yyvsp[0].dval)); }
#line 3822 "def.tab.c"
    break;

  case 93: /* units: K_UNITS K_DISTANCE K_MICRONS NUMBER ';'  */
#line 605 "def.y"
          {
            if (defCallbacks->UnitsCbk) {
              if (defValidNum((int)(yyvsp[-1].dval)))
                CALLBACK(defCallbacks->UnitsCbk,  defrUnitsCbkType, (yyvsp[-1].dval));
            }
          }
#line 3833 "def.tab.c"
    break;

  case 94: /* divider_char: K_DIVIDERCHAR QSTRING ';'  */
#line 613 "def.y"
          {
            if (defCallbacks->DividerCbk)
              CALLBACK(defCallbacks->DividerCbk, defrDividerCbkType, (yyvsp[-1].string));
            defData->hasDivChar = 1;
          }
#line 3843 "def.tab.c"
    break;

  case 95: /* bus_bit_chars: K_BUSBITCHARS QSTRING ';'  */
#line 620 "def.y"
          { 
            if (defCallbacks->BusBitCbk)
              CALLBACK(defCallbacks->BusBitCbk, defrBusBitCbkType, (yyvsp[-1].string));
            defData->hasBusBit = 1;
          }
#line 3853 "def.tab.c"
    break;

  case 96: /* $@18: %empty  */
#line 626 "def.y"
                     {defData->dumb_mode = 1;defData->no_num = 1; }
#line 3859 "def.tab.c"
    break;

  case 97: /* canplace: K_CANPLACE $@18 T_STRING NUMBER NUMBER orient K_DO NUMBER K_BY NUMBER K_STEP NUMBER NUMBER ';'  */
#line 628 "def.y"
            {
              if (defCallbacks->CanplaceCbk) {
                defData->Canplace.setName((yyvsp[-11].string));
                defData->Canplace.setLocation((yyvsp[-10].dval),(yyvsp[-9].dval));
                defData->Canplace.setOrient((yyvsp[-8].integer));
                defData->Canplace.setDo((yyvsp[-6].dval),(yyvsp[-4].dval),(yyvsp[-2].dval),(yyvsp[-1].dval));
                CALLBACK(defCallbacks->CanplaceCbk, defrCanplaceCbkType,
                &(defData->Canplace));
              }
            }
#line 3874 "def.tab.c"
    break;

  case 98: /* $@19: %empty  */
#line 638 "def.y"
                             {defData->dumb_mode = 1;defData->no_num = 1; }
#line 3880 "def.tab.c"
    break;

  case 99: /* cannotoccupy: K_CANNOTOCCUPY $@19 T_STRING NUMBER NUMBER orient K_DO NUMBER K_BY NUMBER K_STEP NUMBER NUMBER ';'  */
#line 640 "def.y"
            {
              if (defCallbacks->CannotOccupyCbk) {
                defData->CannotOccupy.setName((yyvsp[-11].string));
                defData->CannotOccupy.setLocation((yyvsp[-10].dval),(yyvsp[-9].dval));
                defData->CannotOccupy.setOrient((yyvsp[-8].integer));
                defData->CannotOccupy.setDo((yyvsp[-6].dval),(yyvsp[-4].dval),(yyvsp[-2].dval),(yyvsp[-1].dval));
                CALLBACK(defCallbacks->CannotOccupyCbk, defrCannotOccupyCbkType,
                        &(defData->CannotOccupy));
              }
            }
#line 3895 "def.tab.c"
    break;

  case 100: /* orient: K_N  */
#line 651 "def.y"
               {(yyval.integer) = 0;}
#line 3901 "def.tab.c"
    break;

  case 101: /* orient: K_W  */
#line 652 "def.y"
               {(yyval.integer) = 1;}
#line 3907 "def.tab.c"
    break;

  case 102: /* orient: K_S  */
#line 653 "def.y"
               {(yyval.integer) = 2;}
#line 3913 "def.tab.c"
    break;

  case 103: /* orient: K_E  */
#line 654 "def.y"
               {(yyval.integer) = 3;}
#line 3919 "def.tab.c"
    break;

  case 104: /* orient: K_FN  */
#line 655 "def.y"
               {(yyval.integer) = 4;}
#line 3925 "def.tab.c"
    break;

  case 105: /* orient: K_FW  */
#line 656 "def.y"
               {(yyval.integer) = 5;}
#line 3931 "def.tab.c"
    break;

  case 106: /* orient: K_FS  */
#line 657 "def.y"
               {(yyval.integer) = 6;}
#line 3937 "def.tab.c"
    break;

  case 107: /* orient: K_FE  */
#line 658 "def.y"
               {(yyval.integer) = 7;}
#line 3943 "def.tab.c"
    break;

  case 108: /* $@20: %empty  */
#line 661 "def.y"
          {
            defData->Geometries.Reset();
          }
#line 3951 "def.tab.c"
    break;

  case 109: /* die_area: K_DIEAREA $@20 firstPt nextPt otherPts ';'  */
#line 665 "def.y"
          {
            if (defCallbacks->DieAreaCbk) {
               defData->DieArea.addPoint(&defData->Geometries);
               CALLBACK(defCallbacks->DieAreaCbk, defrDieAreaCbkType, &(defData->DieArea));
            }
          }
#line 3962 "def.tab.c"
    break;

  case 110: /* pin_cap_rule: start_def_cap pin_caps end_def_cap  */
#line 674 "def.y"
            { }
#line 3968 "def.tab.c"
    break;

  case 111: /* start_def_cap: K_DEFAULTCAP NUMBER  */
#line 677 "def.y"
        {
          if (defData->VersionNum < 5.4) {
             if (defCallbacks->DefaultCapCbk)
                CALLBACK(defCallbacks->DefaultCapCbk, defrDefaultCapCbkType, ROUND((yyvsp[0].dval)));
          } else {
             if (defCallbacks->DefaultCapCbk) // write error only if cbk is set 
                if (defData->defaultCapWarnings++ < defSettings->DefaultCapWarnings)
                   defWarning(7017, "The DEFAULTCAP statement is obsolete in version 5.4 and later.\nThe DEF parser will ignore this statement.");
          }
        }
#line 3983 "def.tab.c"
    break;

  case 114: /* pin_cap: K_MINPINS NUMBER K_WIRECAP NUMBER ';'  */
#line 693 "def.y"
          {
            if (defData->VersionNum < 5.4) {
              if (defCallbacks->PinCapCbk) {
                defData->PinCap.setPin(ROUND((yyvsp[-3].dval)));
                defData->PinCap.setCap((yyvsp[-1].dval));
                CALLBACK(defCallbacks->PinCapCbk, defrPinCapCbkType, &(defData->PinCap));
              }
            }
          }
#line 3997 "def.tab.c"
    break;

  case 115: /* end_def_cap: K_END K_DEFAULTCAP  */
#line 704 "def.y"
            { }
#line 4003 "def.tab.c"
    break;

  case 116: /* pin_rule: start_pins pins end_pins  */
#line 707 "def.y"
            { }
#line 4009 "def.tab.c"
    break;

  case 117: /* start_pins: K_PINS NUMBER ';'  */
#line 710 "def.y"
          { 
            if (defCallbacks->StartPinsCbk)
              CALLBACK(defCallbacks->StartPinsCbk, defrStartPinsCbkType, ROUND((yyvsp[-1].dval)));
          }
#line 4018 "def.tab.c"
    break;

  case 120: /* $@21: %empty  */
#line 719 "def.y"
         {defData->dumb_mode = 1; defData->no_num = 1; }
#line 4024 "def.tab.c"
    break;

  case 121: /* $@22: %empty  */
#line 720 "def.y"
         {defData->dumb_mode = 1; defData->no_num = 1; }
#line 4030 "def.tab.c"
    break;

  case 122: /* $@23: %empty  */
#line 721 "def.y"
          {
            if (defCallbacks->PinCbk || defCallbacks->PinExtCbk) {
              defData->Pin.Setup((yyvsp[-4].string), (yyvsp[0].string));
            }
            defData->hasPort = 0;
          }
#line 4041 "def.tab.c"
    break;

  case 123: /* pin: '-' $@21 T_STRING '+' K_NET $@22 T_STRING $@23 pin_options ';'  */
#line 728 "def.y"
          { 
            if (defCallbacks->PinCbk)
              CALLBACK(defCallbacks->PinCbk, defrPinCbkType, &defData->Pin);
          }
#line 4050 "def.tab.c"
    break;

  case 126: /* pin_option: '+' K_SPECIAL  */
#line 737 "def.y"
          {
            if (defCallbacks->PinCbk)
              defData->Pin.setSpecial();
          }
#line 4059 "def.tab.c"
    break;

  case 127: /* pin_option: extension_stmt  */
#line 743 "def.y"
          { 
            if (defCallbacks->PinExtCbk)
              CALLBACK(defCallbacks->PinExtCbk, defrPinExtCbkType, &defData->History_text[0]);
          }
#line 4068 "def.tab.c"
    break;

  case 128: /* pin_option: '+' K_DIRECTION T_STRING  */
#line 749 "def.y"
          {
            if (defCallbacks->PinCbk || defCallbacks->PinExtCbk)
              defData->Pin.setDirection((yyvsp[0].string));
          }
#line 4077 "def.tab.c"
    break;

  case 129: /* pin_option: '+' K_NETEXPR QSTRING  */
#line 755 "def.y"
          {
            if (defData->VersionNum < 5.6) {
              if (defCallbacks->PinCbk || defCallbacks->PinExtCbk) {
                if ((defData->pinWarnings++ < defSettings->PinWarnings) &&
                    (defData->pinWarnings++ < defSettings->PinExtWarnings)) {
                  defData->defMsg = (char*)defMalloc(1000);
                  sprintf (defData->defMsg,
                     "The NETEXPR statement is available in version 5.6 and later.\nHowever, your DEF file is defined with version %g.", defData->VersionNum);
                  defError(6506, defData->defMsg);
                  defFree(defData->defMsg);
                  CHKERR();
                }
              }
            } else {
              if (defCallbacks->PinCbk || defCallbacks->PinExtCbk)
                defData->Pin.setNetExpr((yyvsp[0].string));

            }
          }
#line 4101 "def.tab.c"
    break;

  case 130: /* $@24: %empty  */
#line 775 "def.y"
                                  { defData->dumb_mode = 1; }
#line 4107 "def.tab.c"
    break;

  case 131: /* pin_option: '+' K_SUPPLYSENSITIVITY $@24 T_STRING  */
#line 776 "def.y"
          {
            if (defData->VersionNum < 5.6) {
              if (defCallbacks->PinCbk || defCallbacks->PinExtCbk) {
                if ((defData->pinWarnings++ < defSettings->PinWarnings) &&
                    (defData->pinWarnings++ < defSettings->PinExtWarnings)) {
                  defData->defMsg = (char*)defMalloc(1000);
                  sprintf (defData->defMsg,
                     "The SUPPLYSENSITIVITY statement is available in version 5.6 and later.\nHowever, your DEF file is defined with version %g.", defData->VersionNum);
                  defError(6507, defData->defMsg);
                  defFree(defData->defMsg);
                  CHKERR();
                }
              }
            } else {
              if (defCallbacks->PinCbk || defCallbacks->PinExtCbk)
                defData->Pin.setSupplySens((yyvsp[0].string));
            }
          }
#line 4130 "def.tab.c"
    break;

  case 132: /* $@25: %empty  */
#line 795 "def.y"
                                  { defData->dumb_mode = 1; }
#line 4136 "def.tab.c"
    break;

  case 133: /* pin_option: '+' K_GROUNDSENSITIVITY $@25 T_STRING  */
#line 796 "def.y"
          {
            if (defData->VersionNum < 5.6) {
              if (defCallbacks->PinCbk || defCallbacks->PinExtCbk) {
                if ((defData->pinWarnings++ < defSettings->PinWarnings) &&
                    (defData->pinWarnings++ < defSettings->PinExtWarnings)) {
                  defData->defMsg = (char*)defMalloc(1000);
                  sprintf (defData->defMsg,
                     "The GROUNDSENSITIVITY statement is available in version 5.6 and later.\nHowever, your DEF file is defined with version %g.", defData->VersionNum);
                  defError(6508, defData->defMsg);
                  defFree(defData->defMsg);
                  CHKERR();
                }
              }
            } else {
              if (defCallbacks->PinCbk || defCallbacks->PinExtCbk)
                defData->Pin.setGroundSens((yyvsp[0].string));
            }
          }
#line 4159 "def.tab.c"
    break;

  case 134: /* pin_option: '+' K_USE use_type  */
#line 816 "def.y"
          {
            if (defCallbacks->PinCbk || defCallbacks->PinExtCbk) defData->Pin.setUse((yyvsp[0].string));
          }
#line 4167 "def.tab.c"
    break;

  case 135: /* pin_option: '+' K_PORT  */
#line 820 "def.y"
          {
            if (defData->VersionNum < 5.7) {
               if (defCallbacks->PinCbk || defCallbacks->PinExtCbk) {
                 if ((defData->pinWarnings++ < defSettings->PinWarnings) &&
                     (defData->pinWarnings++ < defSettings->PinExtWarnings)) {
                   defData->defMsg = (char*)defMalloc(10000);
                   sprintf (defData->defMsg,
                     "The PORT in PINS is available in version 5.7 or later.\nHowever, your DEF file is defined with version %g.", defData->VersionNum);
                   defError(6555, defData->defMsg);
                   defFree(defData->defMsg);
                   CHKERR();
                 }
               }
            } else {
               if (defCallbacks->PinCbk || defCallbacks->PinExtCbk)
                 defData->Pin.addPort();
               defData->hasPort = 1;
            }
          }
#line 4191 "def.tab.c"
    break;

  case 136: /* $@26: %empty  */
#line 840 "def.y"
                      { defData->dumb_mode = 1; }
#line 4197 "def.tab.c"
    break;

  case 137: /* $@27: %empty  */
#line 841 "def.y"
          {
            if (defCallbacks->PinCbk || defCallbacks->PinExtCbk) {
              if (defData->hasPort)
                 defData->Pin.addPortLayer((yyvsp[0].string));
              else
                 defData->Pin.addLayer((yyvsp[0].string));
            }
          }
#line 4210 "def.tab.c"
    break;

  case 138: /* pin_option: '+' K_LAYER $@26 T_STRING $@27 pin_layer_mask_opt pin_layer_spacing_opt pt pt  */
#line 850 "def.y"
          {
            if (defCallbacks->PinCbk || defCallbacks->PinExtCbk) {
              if (defData->hasPort)
                 defData->Pin.addPortLayerPts((yyvsp[-1].pt).x, (yyvsp[-1].pt).y, (yyvsp[0].pt).x, (yyvsp[0].pt).y);
              else
                 defData->Pin.addLayerPts((yyvsp[-1].pt).x, (yyvsp[-1].pt).y, (yyvsp[0].pt).x, (yyvsp[0].pt).y);
            }
          }
#line 4223 "def.tab.c"
    break;

  case 139: /* $@28: %empty  */
#line 859 "def.y"
                        { defData->dumb_mode = 1; }
#line 4229 "def.tab.c"
    break;

  case 140: /* $@29: %empty  */
#line 860 "def.y"
          {
            if (defData->VersionNum < 5.6) {
              if (defCallbacks->PinCbk || defCallbacks->PinExtCbk) {
                if ((defData->pinWarnings++ < defSettings->PinWarnings) &&
                    (defData->pinWarnings++ < defSettings->PinExtWarnings)) {
                  defData->defMsg = (char*)defMalloc(1000);
                  sprintf (defData->defMsg,
                     "The POLYGON statement is available in version 5.6 and later.\nHowever, your DEF file is defined with version %g.", defData->VersionNum);
                  defError(6509, defData->defMsg);
                  defFree(defData->defMsg);
                  CHKERR();
                }
              }
            } else {
              if (defCallbacks->PinCbk || defCallbacks->PinExtCbk) {
                if (defData->hasPort)
                   defData->Pin.addPortPolygon((yyvsp[0].string));
                else
                   defData->Pin.addPolygon((yyvsp[0].string));
              }
            }
            
            defData->Geometries.Reset();            
          }
#line 4258 "def.tab.c"
    break;

  case 141: /* pin_option: '+' K_POLYGON $@28 T_STRING $@29 pin_poly_mask_opt pin_poly_spacing_opt firstPt nextPt nextPt otherPts  */
#line 885 "def.y"
          {
            if (defData->VersionNum >= 5.6) {  // only add if 5.6 or beyond
              if (defCallbacks->PinCbk || defCallbacks->PinExtCbk) {
                if (defData->hasPort)
                   defData->Pin.addPortPolygonPts(&defData->Geometries);
                else
                   defData->Pin.addPolygonPts(&defData->Geometries);
              }
            }
          }
#line 4273 "def.tab.c"
    break;

  case 142: /* $@30: %empty  */
#line 895 "def.y"
                    { defData->dumb_mode = 1; }
#line 4279 "def.tab.c"
    break;

  case 143: /* pin_option: '+' K_VIA $@30 T_STRING pin_via_mask_opt '(' NUMBER NUMBER ')'  */
#line 896 "def.y"
          {
            if (defData->VersionNum < 5.7) {
              if (defCallbacks->PinCbk || defCallbacks->PinExtCbk) {
                if ((defData->pinWarnings++ < defSettings->PinWarnings) &&
                    (defData->pinWarnings++ < defSettings->PinExtWarnings)) {
                  defData->defMsg = (char*)defMalloc(1000);
                  sprintf (defData->defMsg,
                     "The PIN VIA statement is available in version 5.7 and later.\nHowever, your DEF file is defined with version %g.", defData->VersionNum);
                  defError(6556, defData->defMsg);
                  defFree(defData->defMsg);
                  CHKERR();
                }
              }
            } else {
              if (defCallbacks->PinCbk || defCallbacks->PinExtCbk) {
                if (defData->hasPort)
                   defData->Pin.addPortVia((yyvsp[-5].string), (int)(yyvsp[-2].dval),
                                               (int)(yyvsp[-1].dval), (yyvsp[-4].integer));
                else
                   defData->Pin.addVia((yyvsp[-5].string), (int)(yyvsp[-2].dval),
                                               (int)(yyvsp[-1].dval), (yyvsp[-4].integer));
              }
            }
          }
#line 4308 "def.tab.c"
    break;

  case 144: /* pin_option: placement_status pt orient  */
#line 922 "def.y"
          {
            if (defCallbacks->PinCbk || defCallbacks->PinExtCbk) {
              if (defData->hasPort)
                 defData->Pin.setPortPlacement((yyvsp[-2].integer), (yyvsp[-1].pt).x, (yyvsp[-1].pt).y, (yyvsp[0].integer));
              else
                 defData->Pin.setPlacement((yyvsp[-2].integer), (yyvsp[-1].pt).x, (yyvsp[-1].pt).y, (yyvsp[0].integer));
            }
          }
#line 4321 "def.tab.c"
    break;

  case 145: /* pin_option: '+' K_ANTENNAPINPARTIALMETALAREA NUMBER pin_layer_opt  */
#line 933 "def.y"
          {
            if (defData->VersionNum <= 5.3) {
              if (defCallbacks->PinCbk || defCallbacks->PinExtCbk) {
                if ((defData->pinWarnings++ < defSettings->PinWarnings) &&
                    (defData->pinWarnings++ < defSettings->PinExtWarnings)) {
                  defData->defMsg = (char*)defMalloc(1000);
                  sprintf (defData->defMsg,
                     "The ANTENNAPINPARTIALMETALAREA statement is available in version 5.4 and later.\nHowever, your DEF file is defined with version %g.", defData->VersionNum);
                  defError(6510, defData->defMsg);
                  defFree(defData->defMsg);
                  CHKERR();
                }
              }
            }
            if (defCallbacks->PinCbk || defCallbacks->PinExtCbk)
              defData->Pin.addAPinPartialMetalArea((int)(yyvsp[-1].dval), (yyvsp[0].string)); 
          }
#line 4343 "def.tab.c"
    break;

  case 146: /* pin_option: '+' K_ANTENNAPINPARTIALMETALSIDEAREA NUMBER pin_layer_opt  */
#line 951 "def.y"
          {
            if (defData->VersionNum <= 5.3) {
              if (defCallbacks->PinCbk || defCallbacks->PinExtCbk) {
                if ((defData->pinWarnings++ < defSettings->PinWarnings) &&
                    (defData->pinWarnings++ < defSettings->PinExtWarnings)) {
                  defData->defMsg = (char*)defMalloc(1000);
                  sprintf (defData->defMsg,
                     "The ANTENNAPINPARTIALMETALSIDEAREA statement is available in version 5.4 and later.\nHowever, your DEF file is defined with version %g.", defData->VersionNum);
                  defError(6511, defData->defMsg);
                  defFree(defData->defMsg);
                  CHKERR();
                }
              }
            }
            if (defCallbacks->PinCbk || defCallbacks->PinExtCbk)
              defData->Pin.addAPinPartialMetalSideArea((int)(yyvsp[-1].dval), (yyvsp[0].string)); 
          }
#line 4365 "def.tab.c"
    break;

  case 147: /* pin_option: '+' K_ANTENNAPINGATEAREA NUMBER pin_layer_opt  */
#line 969 "def.y"
          {
            if (defData->VersionNum <= 5.3) {
              if (defCallbacks->PinCbk || defCallbacks->PinExtCbk) {
                if ((defData->pinWarnings++ < defSettings->PinWarnings) &&
                    (defData->pinWarnings++ < defSettings->PinExtWarnings)) {
                  defData->defMsg = (char*)defMalloc(1000);
                  sprintf (defData->defMsg,
                     "The ANTENNAPINGATEAREA statement is available in version 5.4 and later.\nHowever, your DEF file is defined with version %g.", defData->VersionNum);
                  defError(6512, defData->defMsg);
                  defFree(defData->defMsg);
                  CHKERR();
                }
              }
            }
              if (defCallbacks->PinCbk || defCallbacks->PinExtCbk)
                defData->Pin.addAPinGateArea((int)(yyvsp[-1].dval), (yyvsp[0].string)); 
            }
#line 4387 "def.tab.c"
    break;

  case 148: /* pin_option: '+' K_ANTENNAPINDIFFAREA NUMBER pin_layer_opt  */
#line 987 "def.y"
          {
            if (defData->VersionNum <= 5.3) {
              if (defCallbacks->PinCbk || defCallbacks->PinExtCbk) {
                if ((defData->pinWarnings++ < defSettings->PinWarnings) &&
                    (defData->pinWarnings++ < defSettings->PinExtWarnings)) {
                  defData->defMsg = (char*)defMalloc(1000);
                  sprintf (defData->defMsg,
                     "The ANTENNAPINDIFFAREA statement is available in version 5.4 and later.\nHowever, your DEF file is defined with version %g.", defData->VersionNum);
                  defError(6513, defData->defMsg);
                  defFree(defData->defMsg);
                  CHKERR();
                }
              }
            }
            if (defCallbacks->PinCbk || defCallbacks->PinExtCbk)
              defData->Pin.addAPinDiffArea((int)(yyvsp[-1].dval), (yyvsp[0].string)); 
          }
#line 4409 "def.tab.c"
    break;

  case 149: /* $@31: %empty  */
#line 1004 "def.y"
                                                    {defData->dumb_mode=1;}
#line 4415 "def.tab.c"
    break;

  case 150: /* pin_option: '+' K_ANTENNAPINMAXAREACAR NUMBER K_LAYER $@31 T_STRING  */
#line 1005 "def.y"
          {
            if (defData->VersionNum <= 5.3) {
              if (defCallbacks->PinCbk || defCallbacks->PinExtCbk) {
                if ((defData->pinWarnings++ < defSettings->PinWarnings) &&
                    (defData->pinWarnings++ < defSettings->PinExtWarnings)) {
                  defData->defMsg = (char*)defMalloc(1000);
                  sprintf (defData->defMsg,
                     "The ANTENNAPINMAXAREACAR statement is available in version 5.4 and later.\nHowever, your DEF file is defined with version %g.", defData->VersionNum);
                  defError(6514, defData->defMsg);
                  defFree(defData->defMsg);
                  CHKERR();
                }
              }
            }
            if (defCallbacks->PinCbk || defCallbacks->PinExtCbk)
              defData->Pin.addAPinMaxAreaCar((int)(yyvsp[-3].dval), (yyvsp[0].string)); 
          }
#line 4437 "def.tab.c"
    break;

  case 151: /* $@32: %empty  */
#line 1022 "def.y"
                                                        {defData->dumb_mode=1;}
#line 4443 "def.tab.c"
    break;

  case 152: /* pin_option: '+' K_ANTENNAPINMAXSIDEAREACAR NUMBER K_LAYER $@32 T_STRING  */
#line 1024 "def.y"
          {
            if (defData->VersionNum <= 5.3) {
              if (defCallbacks->PinCbk || defCallbacks->PinExtCbk) {
                if ((defData->pinWarnings++ < defSettings->PinWarnings) &&
                    (defData->pinWarnings++ < defSettings->PinExtWarnings)) {
                  defData->defMsg = (char*)defMalloc(1000);
                  sprintf (defData->defMsg,
                     "The ANTENNAPINMAXSIDEAREACAR statement is available in version 5.4 and later.\nHowever, your DEF file is defined with version %g.", defData->VersionNum);
                  defError(6515, defData->defMsg);
                  defFree(defData->defMsg);
                  CHKERR();
                }
              }
            }
            if (defCallbacks->PinCbk || defCallbacks->PinExtCbk)
              defData->Pin.addAPinMaxSideAreaCar((int)(yyvsp[-3].dval), (yyvsp[0].string)); 
          }
#line 4465 "def.tab.c"
    break;

  case 153: /* pin_option: '+' K_ANTENNAPINPARTIALCUTAREA NUMBER pin_layer_opt  */
#line 1042 "def.y"
          {
            if (defData->VersionNum <= 5.3) {
              if (defCallbacks->PinCbk || defCallbacks->PinExtCbk) {
                if ((defData->pinWarnings++ < defSettings->PinWarnings) &&
                    (defData->pinWarnings++ < defSettings->PinExtWarnings)) {
                  defData->defMsg = (char*)defMalloc(1000);
                  sprintf (defData->defMsg,
                     "The ANTENNAPINPARTIALCUTAREA statement is available in version 5.4 and later.\nHowever, your DEF file is defined with version %g.", defData->VersionNum);
                  defError(6516, defData->defMsg);
                  defFree(defData->defMsg);
                  CHKERR();
                }
              }
            }
            if (defCallbacks->PinCbk || defCallbacks->PinExtCbk)
              defData->Pin.addAPinPartialCutArea((int)(yyvsp[-1].dval), (yyvsp[0].string)); 
          }
#line 4487 "def.tab.c"
    break;

  case 154: /* $@33: %empty  */
#line 1059 "def.y"
                                                   {defData->dumb_mode=1;}
#line 4493 "def.tab.c"
    break;

  case 155: /* pin_option: '+' K_ANTENNAPINMAXCUTCAR NUMBER K_LAYER $@33 T_STRING  */
#line 1060 "def.y"
          {
            if (defData->VersionNum <= 5.3) {
              if (defCallbacks->PinCbk || defCallbacks->PinExtCbk) {
                if ((defData->pinWarnings++ < defSettings->PinWarnings) &&
                    (defData->pinWarnings++ < defSettings->PinExtWarnings)) {
                  defData->defMsg = (char*)defMalloc(1000);
                  sprintf (defData->defMsg,
                     "The ANTENNAPINMAXCUTCAR statement is available in version 5.4 and later.\nHowever, your DEF file is defined with version %g.", defData->VersionNum);
                  defError(6517, defData->defMsg);
                  defFree(defData->defMsg);
                  CHKERR();
                }
              }
            }
            if (defCallbacks->PinCbk || defCallbacks->PinExtCbk)
              defData->Pin.addAPinMaxCutCar((int)(yyvsp[-3].dval), (yyvsp[0].string)); 
          }
#line 4515 "def.tab.c"
    break;

  case 156: /* pin_option: '+' K_ANTENNAMODEL pin_oxide  */
#line 1078 "def.y"
          {  // 5.5 syntax 
            if (defData->VersionNum < 5.5) {
              if (defCallbacks->PinCbk || defCallbacks->PinExtCbk) {
                if ((defData->pinWarnings++ < defSettings->PinWarnings) &&
                    (defData->pinWarnings++ < defSettings->PinExtWarnings)) {
                  defData->defMsg = (char*)defMalloc(1000);
                  sprintf (defData->defMsg,
                     "The ANTENNAMODEL statement is available in version 5.5 and later.\nHowever, your DEF file is defined with version %g.", defData->VersionNum);
                  defError(6518, defData->defMsg);
                  defFree(defData->defMsg);
                  CHKERR();
                }
              }
            }
          }
#line 4535 "def.tab.c"
    break;

  case 158: /* pin_layer_mask_opt: K_MASK NUMBER  */
#line 1096 "def.y"
         { 
           if (validateMaskInput((int)(yyvsp[0].dval), defData->pinWarnings, defSettings->PinWarnings)) {
              if (defCallbacks->PinCbk || defCallbacks->PinExtCbk) {
                if (defData->hasPort)
                   defData->Pin.addPortLayerMask((int)(yyvsp[0].dval));
                else
                   defData->Pin.addLayerMask((int)(yyvsp[0].dval));
              }
           }
         }
#line 4550 "def.tab.c"
    break;

  case 159: /* pin_via_mask_opt: %empty  */
#line 1109 "def.y"
        { (yyval.integer) = 0; }
#line 4556 "def.tab.c"
    break;

  case 160: /* pin_via_mask_opt: K_MASK NUMBER  */
#line 1111 "def.y"
         { 
           if (validateMaskInput((int)(yyvsp[0].dval), defData->pinWarnings, defSettings->PinWarnings)) {
             (yyval.integer) = (yyvsp[0].dval);
           }
         }
#line 4566 "def.tab.c"
    break;

  case 162: /* pin_poly_mask_opt: K_MASK NUMBER  */
#line 1119 "def.y"
         { 
           if (validateMaskInput((int)(yyvsp[0].dval), defData->pinWarnings, defSettings->PinWarnings)) {
              if (defCallbacks->PinCbk || defCallbacks->PinExtCbk) {
                if (defData->hasPort)
                   defData->Pin.addPortPolyMask((int)(yyvsp[0].dval));
                else
                   defData->Pin.addPolyMask((int)(yyvsp[0].dval));
              }
           }
         }
#line 4581 "def.tab.c"
    break;

  case 164: /* pin_layer_spacing_opt: K_SPACING NUMBER  */
#line 1133 "def.y"
          {
            if (defData->VersionNum < 5.6) {
              if (defCallbacks->PinCbk || defCallbacks->PinExtCbk) {
                if ((defData->pinWarnings++ < defSettings->PinWarnings) &&
                    (defData->pinWarnings++ < defSettings->PinExtWarnings)) {
                  defData->defMsg = (char*)defMalloc(1000);
                  sprintf (defData->defMsg,
                     "The SPACING statement is available in version 5.6 and later.\nHowever, your DEF file is defined with version %g.", defData->VersionNum);
                  defError(6519, defData->defMsg);
                  defFree(defData->defMsg);
                  CHKERR();
                }
              }
            } else {
              if (defCallbacks->PinCbk || defCallbacks->PinExtCbk) {
                if (defData->hasPort)
                   defData->Pin.addPortLayerSpacing((int)(yyvsp[0].dval));
                else
                   defData->Pin.addLayerSpacing((int)(yyvsp[0].dval));
              }
            }
          }
#line 4608 "def.tab.c"
    break;

  case 165: /* pin_layer_spacing_opt: K_DESIGNRULEWIDTH NUMBER  */
#line 1156 "def.y"
          {
            if (defData->VersionNum < 5.6) {
              if (defCallbacks->PinCbk || defCallbacks->PinExtCbk) {
                if ((defData->pinWarnings++ < defSettings->PinWarnings) &&
                    (defData->pinWarnings++ < defSettings->PinExtWarnings)) {
                  defData->defMsg = (char*)defMalloc(1000);
                  sprintf (defData->defMsg,
                     "DESIGNRULEWIDTH statement is a version 5.6 and later syntax.\nYour def file is defined with version %g", defData->VersionNum);
                  defError(6520, defData->defMsg);
                  defFree(defData->defMsg);
                  CHKERR();
                }
              }
            } else {
              if (defCallbacks->PinCbk || defCallbacks->PinExtCbk) {
                if (defData->hasPort)
                   defData->Pin.addPortLayerDesignRuleWidth((int)(yyvsp[0].dval));
                else
                   defData->Pin.addLayerDesignRuleWidth((int)(yyvsp[0].dval));
              }
            }
          }
#line 4635 "def.tab.c"
    break;

  case 167: /* pin_poly_spacing_opt: K_SPACING NUMBER  */
#line 1181 "def.y"
          {
            if (defData->VersionNum < 5.6) {
              if (defCallbacks->PinCbk || defCallbacks->PinExtCbk) {
                if ((defData->pinWarnings++ < defSettings->PinWarnings) &&
                    (defData->pinWarnings++ < defSettings->PinExtWarnings)) {
                  defData->defMsg = (char*)defMalloc(1000);
                  sprintf (defData->defMsg,
                     "SPACING statement is a version 5.6 and later syntax.\nYour def file is defined with version %g", defData->VersionNum);
                  defError(6521, defData->defMsg);
                  defFree(defData->defMsg);
                  CHKERR();
                }
              }
            } else {
              if (defCallbacks->PinCbk || defCallbacks->PinExtCbk) {
                if (defData->hasPort)
                   defData->Pin.addPortPolySpacing((int)(yyvsp[0].dval));
                else
                   defData->Pin.addPolySpacing((int)(yyvsp[0].dval));
              }
            }
          }
#line 4662 "def.tab.c"
    break;

  case 168: /* pin_poly_spacing_opt: K_DESIGNRULEWIDTH NUMBER  */
#line 1204 "def.y"
          {
            if (defData->VersionNum < 5.6) {
              if (defCallbacks->PinCbk || defCallbacks->PinExtCbk) {
                if ((defData->pinWarnings++ < defSettings->PinWarnings) &&
                    (defData->pinWarnings++ < defSettings->PinExtWarnings)) {
                  defData->defMsg = (char*)defMalloc(1000);
                  sprintf (defData->defMsg,
                     "The DESIGNRULEWIDTH statement is available in version 5.6 and later.\nHowever, your DEF file is defined with version %g.", defData->VersionNum);
                  defError(6520, defData->defMsg);
                  defFree(defData->defMsg);
                  CHKERR();
                }
              }
            } else {
              if (defCallbacks->PinCbk || defCallbacks->PinExtCbk) {
                if (defData->hasPort)
                   defData->Pin.addPortPolyDesignRuleWidth((int)(yyvsp[0].dval));
                else
                   defData->Pin.addPolyDesignRuleWidth((int)(yyvsp[0].dval));
              }
            }
          }
#line 4689 "def.tab.c"
    break;

  case 169: /* pin_oxide: K_OXIDE1  */
#line 1228 "def.y"
          { defData->aOxide = 1;
            if (defCallbacks->PinCbk || defCallbacks->PinExtCbk)
              defData->Pin.addAntennaModel(defData->aOxide);
          }
#line 4698 "def.tab.c"
    break;

  case 170: /* pin_oxide: K_OXIDE2  */
#line 1233 "def.y"
          { defData->aOxide = 2;
            if (defCallbacks->PinCbk || defCallbacks->PinExtCbk)
              defData->Pin.addAntennaModel(defData->aOxide);
          }
#line 4707 "def.tab.c"
    break;

  case 171: /* pin_oxide: K_OXIDE3  */
#line 1238 "def.y"
          { defData->aOxide = 3;
            if (defCallbacks->PinCbk || defCallbacks->PinExtCbk)
              defData->Pin.addAntennaModel(defData->aOxide);
          }
#line 4716 "def.tab.c"
    break;

  case 172: /* pin_oxide: K_OXIDE4  */
#line 1243 "def.y"
          { defData->aOxide = 4;
            if (defCallbacks->PinCbk || defCallbacks->PinExtCbk)
              defData->Pin.addAntennaModel(defData->aOxide);
          }
#line 4725 "def.tab.c"
    break;

  case 173: /* use_type: K_SIGNAL  */
#line 1249 "def.y"
          { (yyval.string) = (char*)"SIGNAL"; }
#line 4731 "def.tab.c"
    break;

  case 174: /* use_type: K_POWER  */
#line 1251 "def.y"
          { (yyval.string) = (char*)"POWER"; }
#line 4737 "def.tab.c"
    break;

  case 175: /* use_type: K_GROUND  */
#line 1253 "def.y"
          { (yyval.string) = (char*)"GROUND"; }
#line 4743 "def.tab.c"
    break;

  case 176: /* use_type: K_CLOCK  */
#line 1255 "def.y"
          { (yyval.string) = (char*)"CLOCK"; }
#line 4749 "def.tab.c"
    break;

  case 177: /* use_type: K_TIEOFF  */
#line 1257 "def.y"
          { (yyval.string) = (char*)"TIEOFF"; }
#line 4755 "def.tab.c"
    break;

  case 178: /* use_type: K_ANALOG  */
#line 1259 "def.y"
          { (yyval.string) = (char*)"ANALOG"; }
#line 4761 "def.tab.c"
    break;

  case 179: /* use_type: K_SCAN  */
#line 1261 "def.y"
          { (yyval.string) = (char*)"SCAN"; }
#line 4767 "def.tab.c"
    break;

  case 180: /* use_type: K_RESET  */
#line 1263 "def.y"
          { (yyval.string) = (char*)"RESET"; }
#line 4773 "def.tab.c"
    break;

  case 181: /* pin_layer_opt: %empty  */
#line 1267 "def.y"
          { (yyval.string) = (char*)""; }
#line 4779 "def.tab.c"
    break;

  case 182: /* $@34: %empty  */
#line 1268 "def.y"
                  {defData->dumb_mode=1;}
#line 4785 "def.tab.c"
    break;

  case 183: /* pin_layer_opt: K_LAYER $@34 T_STRING  */
#line 1269 "def.y"
          { (yyval.string) = (yyvsp[0].string); }
#line 4791 "def.tab.c"
    break;

  case 184: /* end_pins: K_END K_PINS  */
#line 1272 "def.y"
        { 
          if (defCallbacks->PinEndCbk)
            CALLBACK(defCallbacks->PinEndCbk, defrPinEndCbkType, 0);
        }
#line 4800 "def.tab.c"
    break;

  case 185: /* $@35: %empty  */
#line 1277 "def.y"
                {defData->dumb_mode = 2; defData->no_num = 2; }
#line 4806 "def.tab.c"
    break;

  case 186: /* $@36: %empty  */
#line 1279 "def.y"
        {
          if (defCallbacks->RowCbk) {
            defData->rowName = (yyvsp[-4].string);
            defData->Row.setup((yyvsp[-4].string), (yyvsp[-3].string), (yyvsp[-2].dval), (yyvsp[-1].dval), (yyvsp[0].integer));
          }
        }
#line 4817 "def.tab.c"
    break;

  case 187: /* row_rule: K_ROW $@35 T_STRING T_STRING NUMBER NUMBER orient $@36 row_do_option row_options ';'  */
#line 1287 "def.y"
        {
          if (defCallbacks->RowCbk) 
            CALLBACK(defCallbacks->RowCbk, defrRowCbkType, &defData->Row);
        }
#line 4826 "def.tab.c"
    break;

  case 188: /* row_do_option: %empty  */
#line 1293 "def.y"
        {
          if (defData->VersionNum < 5.6) {
            if (defCallbacks->RowCbk) {
              if (defData->rowWarnings++ < defSettings->RowWarnings) {
                defError(6523, "Invalid ROW statement defined in the DEF file. The DO statement which is required in the ROW statement is not defined.\nUpdate your DEF file with a DO statement.");
                CHKERR();
              }
            }
          }
        }
#line 4841 "def.tab.c"
    break;

  case 189: /* row_do_option: K_DO NUMBER K_BY NUMBER row_step_option  */
#line 1304 "def.y"
        {
          // 06/05/2002 - pcr 448455 
          // Check for 1 and 0 in the correct position 
          // 07/26/2002 - Commented out due to pcr 459218 
          if (defData->hasDoStep) {
            // 04/29/2004 - pcr 695535 
            // changed the testing 
            if ((((yyvsp[-1].dval) == 1) && (defData->yStep == 0)) ||
                (((yyvsp[-3].dval) == 1) && (defData->xStep == 0))) {
              // do nothing 
            } else 
              if (defData->VersionNum < 5.6) {
                if (defCallbacks->RowCbk) {
                  if (defData->rowWarnings++ < defSettings->RowWarnings) {
                    defData->defMsg = (char*)defMalloc(1000);
                    sprintf(defData->defMsg,
                            "The DO statement in the ROW statement with the name %s has invalid syntax.\nThe valid syntax is \"DO numX BY 1 STEP spaceX 0 | DO 1 BY numY STEP 0 spaceY\".\nSpecify the valid syntax and try again.", defData->rowName);
                    defWarning(7018, defData->defMsg);
                    defFree(defData->defMsg);
                    }
                  }
              }
          }
          // pcr 459218 - Error if at least numX or numY does not equal 1 
          if (((yyvsp[-3].dval) != 1) && ((yyvsp[-1].dval) != 1)) {
            if (defCallbacks->RowCbk) {
              if (defData->rowWarnings++ < defSettings->RowWarnings) {
                defError(6524, "Invalid syntax specified. The valid syntax is either \"DO 1 BY num or DO num BY 1\". Specify the valid syntax and try again.");
                CHKERR();
              }
            }
          }
          if (defCallbacks->RowCbk)
            defData->Row.setDo(ROUND((yyvsp[-3].dval)), ROUND((yyvsp[-1].dval)), defData->xStep, defData->yStep);
        }
#line 4881 "def.tab.c"
    break;

  case 190: /* row_step_option: %empty  */
#line 1341 "def.y"
        {
          defData->hasDoStep = 0;
        }
#line 4889 "def.tab.c"
    break;

  case 191: /* row_step_option: K_STEP NUMBER NUMBER  */
#line 1345 "def.y"
        {
          defData->hasDoStep = 1;
          defData->Row.setHasDoStep();
          defData->xStep = (yyvsp[-1].dval);
          defData->yStep = (yyvsp[0].dval);
        }
#line 4900 "def.tab.c"
    break;

  case 194: /* $@37: %empty  */
#line 1356 "def.y"
                            {defData->dumb_mode = DEF_MAX_INT; defData->parsing_property = 1;}
#line 4906 "def.tab.c"
    break;

  case 195: /* row_option: '+' K_PROPERTY $@37 row_prop_list  */
#line 1358 "def.y"
         { defData->dumb_mode = 0; defData->parsing_property = 0; }
#line 4912 "def.tab.c"
    break;

  case 198: /* row_prop: T_STRING NUMBER  */
#line 1365 "def.y"
        {
          if (defCallbacks->RowCbk) {
             char propTp;
             char* str = ringCopy("                       ");
             propTp =  defSettings->RowProp.propType((yyvsp[-1].string));
             CHKPROPTYPE(propTp, (yyvsp[-1].string), "ROW");
             // For backword compatibility, also set the string value 
             sprintf(str, "%g", (yyvsp[0].dval));
             defData->Row.addNumProperty((yyvsp[-1].string), (yyvsp[0].dval), str, propTp);
          }
        }
#line 4928 "def.tab.c"
    break;

  case 199: /* row_prop: T_STRING QSTRING  */
#line 1377 "def.y"
        {
          if (defCallbacks->RowCbk) {
             char propTp;
             propTp =  defSettings->RowProp.propType((yyvsp[-1].string));
             CHKPROPTYPE(propTp, (yyvsp[-1].string), "ROW");
             defData->Row.addProperty((yyvsp[-1].string), (yyvsp[0].string), propTp);
          }
        }
#line 4941 "def.tab.c"
    break;

  case 200: /* row_prop: T_STRING T_STRING  */
#line 1386 "def.y"
        {
          if (defCallbacks->RowCbk) {
             char propTp;
             propTp =  defSettings->RowProp.propType((yyvsp[-1].string));
             CHKPROPTYPE(propTp, (yyvsp[-1].string), "ROW");
             defData->Row.addProperty((yyvsp[-1].string), (yyvsp[0].string), propTp);
          }
        }
#line 4954 "def.tab.c"
    break;

  case 201: /* $@38: %empty  */
#line 1396 "def.y"
        {
          if (defCallbacks->TrackCbk) {
            defData->Track.setup((yyvsp[-1].string));
          }
        }
#line 4964 "def.tab.c"
    break;

  case 202: /* tracks_rule: track_start NUMBER $@38 K_DO NUMBER K_STEP NUMBER track_opts ';'  */
#line 1402 "def.y"
        {
          if (((yyvsp[-4].dval) <= 0) && (defData->VersionNum >= 5.4)) {
            if (defCallbacks->TrackCbk)
              if (defData->trackWarnings++ < defSettings->TrackWarnings) {
                defData->defMsg = (char*)defMalloc(1000);
                sprintf (defData->defMsg,
                   "The DO number %g in TRACK is invalid.\nThe number value has to be greater than 0. Specify the valid syntax and try again.", (yyvsp[-4].dval));
                defError(6525, defData->defMsg);
                defFree(defData->defMsg);
              }
          }
          if ((yyvsp[-2].dval) < 0) {
            if (defCallbacks->TrackCbk)
              if (defData->trackWarnings++ < defSettings->TrackWarnings) {
                defData->defMsg = (char*)defMalloc(1000);
                sprintf (defData->defMsg,
                   "The STEP number %g in TRACK is invalid.\nThe number value has to be greater than 0. Specify the valid syntax and try again.", (yyvsp[-2].dval));
                defError(6526, defData->defMsg);
                defFree(defData->defMsg);
              }
          }
          if (defCallbacks->TrackCbk) {
            defData->Track.setDo(ROUND((yyvsp[-7].dval)), ROUND((yyvsp[-4].dval)), (yyvsp[-2].dval));
            CALLBACK(defCallbacks->TrackCbk, defrTrackCbkType, &defData->Track);
          }
        }
#line 4995 "def.tab.c"
    break;

  case 203: /* track_start: K_TRACKS track_type  */
#line 1430 "def.y"
        {
          (yyval.string) = (yyvsp[0].string);
        }
#line 5003 "def.tab.c"
    break;

  case 204: /* track_type: K_X  */
#line 1435 "def.y"
            { (yyval.string) = (char*)"X";}
#line 5009 "def.tab.c"
    break;

  case 205: /* track_type: K_Y  */
#line 1437 "def.y"
            { (yyval.string) = (char*)"Y";}
#line 5015 "def.tab.c"
    break;

  case 208: /* track_mask_statement: K_MASK NUMBER same_mask  */
#line 1443 "def.y"
           { 
              if (validateMaskInput((int)(yyvsp[-1].dval), defData->trackWarnings, defSettings->TrackWarnings)) {
                  if (defCallbacks->TrackCbk) {
                    defData->Track.addMask((yyvsp[-1].dval), (yyvsp[0].integer));
                  }
               }
            }
#line 5027 "def.tab.c"
    break;

  case 209: /* same_mask: %empty  */
#line 1453 "def.y"
        { (yyval.integer) = 0; }
#line 5033 "def.tab.c"
    break;

  case 210: /* same_mask: K_SAMEMASK  */
#line 1455 "def.y"
        { (yyval.integer) = 1; }
#line 5039 "def.tab.c"
    break;

  case 212: /* $@39: %empty  */
#line 1458 "def.y"
                  { defData->dumb_mode = 1000; }
#line 5045 "def.tab.c"
    break;

  case 213: /* track_layer_statement: K_LAYER $@39 track_layer track_layers  */
#line 1459 "def.y"
            { defData->dumb_mode = 0; }
#line 5051 "def.tab.c"
    break;

  case 216: /* track_layer: T_STRING  */
#line 1466 "def.y"
        {
          if (defCallbacks->TrackCbk)
            defData->Track.addLayer((yyvsp[0].string));
        }
#line 5060 "def.tab.c"
    break;

  case 217: /* gcellgrid: K_GCELLGRID track_type NUMBER K_DO NUMBER K_STEP NUMBER ';'  */
#line 1473 "def.y"
        {
          if ((yyvsp[-3].dval) <= 0) {
            if (defCallbacks->GcellGridCbk)
              if (defData->gcellGridWarnings++ < defSettings->GcellGridWarnings) {
                defData->defMsg = (char*)defMalloc(1000);
                sprintf (defData->defMsg,
                   "The DO number %g in GCELLGRID is invalid.\nThe number value has to be greater than 0. Specify the valid syntax and try again.", (yyvsp[-3].dval));
                defError(6527, defData->defMsg);
                defFree(defData->defMsg);
              }
          }
          if ((yyvsp[-1].dval) < 0) {
            if (defCallbacks->GcellGridCbk)
              if (defData->gcellGridWarnings++ < defSettings->GcellGridWarnings) {
                defData->defMsg = (char*)defMalloc(1000);
                sprintf (defData->defMsg,
                   "The STEP number %g in GCELLGRID is invalid.\nThe number value has to be greater than 0. Specify the valid syntax and try again.", (yyvsp[-1].dval));
                defError(6528, defData->defMsg);
                defFree(defData->defMsg);
              }
          }
          if (defCallbacks->GcellGridCbk) {
            defData->GcellGrid.setup((yyvsp[-6].string), ROUND((yyvsp[-5].dval)), ROUND((yyvsp[-3].dval)), (yyvsp[-1].dval));
            CALLBACK(defCallbacks->GcellGridCbk, defrGcellGridCbkType, &defData->GcellGrid);
          }
        }
#line 5091 "def.tab.c"
    break;

  case 218: /* extension_section: K_BEGINEXT  */
#line 1501 "def.y"
        {
          if (defCallbacks->ExtensionCbk)
             CALLBACK(defCallbacks->ExtensionCbk, defrExtensionCbkType, &defData->History_text[0]);
        }
#line 5100 "def.tab.c"
    break;

  case 219: /* extension_stmt: '+' K_BEGINEXT  */
#line 1507 "def.y"
        { }
#line 5106 "def.tab.c"
    break;

  case 221: /* via: K_VIAS NUMBER ';'  */
#line 1513 "def.y"
        {
          if (defCallbacks->ViaStartCbk)
            CALLBACK(defCallbacks->ViaStartCbk, defrViaStartCbkType, ROUND((yyvsp[-1].dval)));
        }
#line 5115 "def.tab.c"
    break;

  case 224: /* $@40: %empty  */
#line 1522 "def.y"
                     {defData->dumb_mode = 1;defData->no_num = 1; }
#line 5121 "def.tab.c"
    break;

  case 225: /* $@41: %empty  */
#line 1523 "def.y"
            {
              if (defCallbacks->ViaCbk) defData->Via.setup((yyvsp[0].string));
              defData->viaRule = 0;
            }
#line 5130 "def.tab.c"
    break;

  case 226: /* via_declaration: '-' $@40 T_STRING $@41 layer_stmts ';'  */
#line 1528 "def.y"
            {
              if (defCallbacks->ViaCbk)
                CALLBACK(defCallbacks->ViaCbk, defrViaCbkType, &defData->Via);
              defData->Via.clear();
            }
#line 5140 "def.tab.c"
    break;

  case 229: /* $@42: %empty  */
#line 1538 "def.y"
                       {defData->dumb_mode = 1;defData->no_num = 1; }
#line 5146 "def.tab.c"
    break;

  case 230: /* layer_stmt: '+' K_RECT $@42 T_STRING mask pt pt  */
#line 1539 "def.y"
            { 
              if (defCallbacks->ViaCbk)
                if (validateMaskInput((yyvsp[-2].integer), defData->viaWarnings, defSettings->ViaWarnings)) {
                    defData->Via.addLayer((yyvsp[-3].string), (yyvsp[-1].pt).x, (yyvsp[-1].pt).y, (yyvsp[0].pt).x, (yyvsp[0].pt).y, (yyvsp[-2].integer));
                }
            }
#line 5157 "def.tab.c"
    break;

  case 231: /* $@43: %empty  */
#line 1545 "def.y"
                        { defData->dumb_mode = 1; }
#line 5163 "def.tab.c"
    break;

  case 232: /* $@44: %empty  */
#line 1546 "def.y"
            {
              if (defData->VersionNum < 5.6) {
                if (defCallbacks->ViaCbk) {
                  if (defData->viaWarnings++ < defSettings->ViaWarnings) {
                    defData->defMsg = (char*)defMalloc(1000);
                    sprintf (defData->defMsg,
                       "The POLYGON statement is available in version 5.6 and later.\nHowever, your DEF file is defined with version %g.", defData->VersionNum);
                    defError(6509, defData->defMsg);
                    defFree(defData->defMsg);
                    CHKERR();
                  }
                }
              }
              
              defData->Geometries.Reset();
              
            }
#line 5185 "def.tab.c"
    break;

  case 233: /* layer_stmt: '+' K_POLYGON $@43 T_STRING mask $@44 firstPt nextPt nextPt otherPts  */
#line 1564 "def.y"
            {
              if (defData->VersionNum >= 5.6) {  // only add if 5.6 or beyond
                if (defCallbacks->ViaCbk)
                  if (validateMaskInput((yyvsp[-5].integer), defData->viaWarnings, defSettings->ViaWarnings)) {
                    defData->Via.addPolygon((yyvsp[-6].string), &defData->Geometries, (yyvsp[-5].integer));
                  }
              }
            }
#line 5198 "def.tab.c"
    break;

  case 234: /* $@45: %empty  */
#line 1572 "def.y"
                            {defData->dumb_mode = 1;defData->no_num = 1; }
#line 5204 "def.tab.c"
    break;

  case 235: /* layer_stmt: '+' K_PATTERNNAME $@45 T_STRING  */
#line 1573 "def.y"
            {
              if (defData->VersionNum < 5.6) {
                if (defCallbacks->ViaCbk)
                  defData->Via.addPattern((yyvsp[0].string));
              } else
                if (defCallbacks->ViaCbk)
                  if (defData->viaWarnings++ < defSettings->ViaWarnings)
                    defWarning(7019, "The PATTERNNAME statement is obsolete in version 5.6 and later.\nThe DEF parser will ignore this statement."); 
            }
#line 5218 "def.tab.c"
    break;

  case 236: /* $@46: %empty  */
#line 1582 "def.y"
                        {defData->dumb_mode = 1;defData->no_num = 1; }
#line 5224 "def.tab.c"
    break;

  case 237: /* $@47: %empty  */
#line 1584 "def.y"
                       {defData->dumb_mode = 3;defData->no_num = 1; }
#line 5230 "def.tab.c"
    break;

  case 238: /* layer_stmt: '+' K_VIARULE $@46 T_STRING '+' K_CUTSIZE NUMBER NUMBER '+' K_LAYERS $@47 T_STRING T_STRING T_STRING '+' K_CUTSPACING NUMBER NUMBER '+' K_ENCLOSURE NUMBER NUMBER NUMBER NUMBER  */
#line 1587 "def.y"
            {
               defData->viaRule = 1;
               if (defData->VersionNum < 5.6) {
                if (defCallbacks->ViaCbk) {
                  if (defData->viaWarnings++ < defSettings->ViaWarnings) {
                    defData->defMsg = (char*)defMalloc(1000);
                    sprintf (defData->defMsg,
                       "The VIARULE statement is available in version 5.6 and later.\nHowever, your DEF file is defined with version %g.", defData->VersionNum);
                    defError(6557, defData->defMsg);
                    defFree(defData->defMsg);
                    CHKERR();
                  }
                }
              } else {
                if (defCallbacks->ViaCbk)
                   defData->Via.addViaRule((yyvsp[-20].string), (int)(yyvsp[-17].dval), (int)(yyvsp[-16].dval), (yyvsp[-12].string), (yyvsp[-11].string),
                                             (yyvsp[-10].string), (int)(yyvsp[-7].dval), (int)(yyvsp[-6].dval), (int)(yyvsp[-3].dval),
                                             (int)(yyvsp[-2].dval), (int)(yyvsp[-1].dval), (int)(yyvsp[0].dval)); 
              }
            }
#line 5255 "def.tab.c"
    break;

  case 240: /* layer_stmt: extension_stmt  */
#line 1609 "def.y"
          { 
            if (defCallbacks->ViaExtCbk)
              CALLBACK(defCallbacks->ViaExtCbk, defrViaExtCbkType, &defData->History_text[0]);
          }
#line 5264 "def.tab.c"
    break;

  case 241: /* layer_viarule_opts: '+' K_ROWCOL NUMBER NUMBER  */
#line 1615 "def.y"
            {
              if (!defData->viaRule) {
                if (defCallbacks->ViaCbk) {
                  if (defData->viaWarnings++ < defSettings->ViaWarnings) {
                    defError (6559, "The ROWCOL statement is missing from the VIARULE statement. Ensure that it exists in the VIARULE statement.");
                    CHKERR();
                  }
                }
              } else if (defCallbacks->ViaCbk)
                 defData->Via.addRowCol((int)(yyvsp[-1].dval), (int)(yyvsp[0].dval));
            }
#line 5280 "def.tab.c"
    break;

  case 242: /* layer_viarule_opts: '+' K_ORIGIN NUMBER NUMBER  */
#line 1627 "def.y"
            {
              if (!defData->viaRule) {
                if (defCallbacks->ViaCbk) {
                  if (defData->viaWarnings++ < defSettings->ViaWarnings) {
                    defError (6560, "The ORIGIN statement is missing from the VIARULE statement. Ensure that it exists in the VIARULE statement.");
                    CHKERR();
                  }
                }
              } else if (defCallbacks->ViaCbk)
                 defData->Via.addOrigin((int)(yyvsp[-1].dval), (int)(yyvsp[0].dval));
            }
#line 5296 "def.tab.c"
    break;

  case 243: /* layer_viarule_opts: '+' K_OFFSET NUMBER NUMBER NUMBER NUMBER  */
#line 1639 "def.y"
            {
              if (!defData->viaRule) {
                if (defCallbacks->ViaCbk) {
                  if (defData->viaWarnings++ < defSettings->ViaWarnings) {
                    defError (6561, "The OFFSET statement is missing from the VIARULE statement. Ensure that it exists in the VIARULE statement.");
                    CHKERR();
                  }
                }
              } else if (defCallbacks->ViaCbk)
                 defData->Via.addOffset((int)(yyvsp[-3].dval), (int)(yyvsp[-2].dval), (int)(yyvsp[-1].dval), (int)(yyvsp[0].dval));
            }
#line 5312 "def.tab.c"
    break;

  case 244: /* $@48: %empty  */
#line 1650 "def.y"
                        {defData->dumb_mode = 1;defData->no_num = 1; }
#line 5318 "def.tab.c"
    break;

  case 245: /* layer_viarule_opts: '+' K_PATTERN $@48 T_STRING  */
#line 1651 "def.y"
            {
              if (!defData->viaRule) {
                if (defCallbacks->ViaCbk) {
                  if (defData->viaWarnings++ < defSettings->ViaWarnings) {
                    defError (6562, "The PATTERN statement is missing from the VIARULE statement. Ensure that it exists in the VIARULE statement.");
                    CHKERR();
                  }
                }
              } else if (defCallbacks->ViaCbk)
                 defData->Via.addCutPattern((yyvsp[0].string));
            }
#line 5334 "def.tab.c"
    break;

  case 246: /* firstPt: pt  */
#line 1664 "def.y"
          { defData->Geometries.startList((yyvsp[0].pt).x, (yyvsp[0].pt).y); }
#line 5340 "def.tab.c"
    break;

  case 247: /* nextPt: pt  */
#line 1667 "def.y"
          { defData->Geometries.addToList((yyvsp[0].pt).x, (yyvsp[0].pt).y); }
#line 5346 "def.tab.c"
    break;

  case 250: /* pt: '(' NUMBER NUMBER ')'  */
#line 1674 "def.y"
          {
            defData->save_x = (yyvsp[-2].dval);
            defData->save_y = (yyvsp[-1].dval);
            (yyval.pt).x = ROUND((yyvsp[-2].dval));
            (yyval.pt).y = ROUND((yyvsp[-1].dval));
          }
#line 5357 "def.tab.c"
    break;

  case 251: /* pt: '(' '*' NUMBER ')'  */
#line 1681 "def.y"
          {
            defData->save_y = (yyvsp[-1].dval);
            (yyval.pt).x = ROUND(defData->save_x);
            (yyval.pt).y = ROUND((yyvsp[-1].dval));
          }
#line 5367 "def.tab.c"
    break;

  case 252: /* pt: '(' NUMBER '*' ')'  */
#line 1687 "def.y"
          {
            defData->save_x = (yyvsp[-2].dval);
            (yyval.pt).x = ROUND((yyvsp[-2].dval));
            (yyval.pt).y = ROUND(defData->save_y);
          }
#line 5377 "def.tab.c"
    break;

  case 253: /* pt: '(' '*' '*' ')'  */
#line 1693 "def.y"
          {
            (yyval.pt).x = ROUND(defData->save_x);
            (yyval.pt).y = ROUND(defData->save_y);
          }
#line 5386 "def.tab.c"
    break;

  case 254: /* mask: %empty  */
#line 1699 "def.y"
      { (yyval.integer) = 0; }
#line 5392 "def.tab.c"
    break;

  case 255: /* mask: '+' K_MASK NUMBER  */
#line 1701 "def.y"
      { (yyval.integer) = (yyvsp[0].dval); }
#line 5398 "def.tab.c"
    break;

  case 256: /* via_end: K_END K_VIAS  */
#line 1704 "def.y"
        { 
          if (defCallbacks->ViaEndCbk)
            CALLBACK(defCallbacks->ViaEndCbk, defrViaEndCbkType, 0);
        }
#line 5407 "def.tab.c"
    break;

  case 257: /* regions_section: regions_start regions_stmts K_END K_REGIONS  */
#line 1710 "def.y"
        {
          if (defCallbacks->RegionEndCbk)
            CALLBACK(defCallbacks->RegionEndCbk, defrRegionEndCbkType, 0);
        }
#line 5416 "def.tab.c"
    break;

  case 258: /* regions_start: K_REGIONS NUMBER ';'  */
#line 1716 "def.y"
        {
          if (defCallbacks->RegionStartCbk)
            CALLBACK(defCallbacks->RegionStartCbk, defrRegionStartCbkType, ROUND((yyvsp[-1].dval)));
        }
#line 5425 "def.tab.c"
    break;

  case 260: /* regions_stmts: regions_stmts regions_stmt  */
#line 1723 "def.y"
            {}
#line 5431 "def.tab.c"
    break;

  case 261: /* $@49: %empty  */
#line 1725 "def.y"
                  { defData->dumb_mode = 1; defData->no_num = 1; }
#line 5437 "def.tab.c"
    break;

  case 262: /* $@50: %empty  */
#line 1726 "def.y"
        {
          if (defCallbacks->RegionCbk)
             defData->Region.setup((yyvsp[0].string));
          defData->regTypeDef = 0;
        }
#line 5447 "def.tab.c"
    break;

  case 263: /* regions_stmt: '-' $@49 T_STRING $@50 rect_list region_options ';'  */
#line 1732 "def.y"
        { CALLBACK(defCallbacks->RegionCbk, defrRegionCbkType, &defData->Region); }
#line 5453 "def.tab.c"
    break;

  case 264: /* rect_list: pt pt  */
#line 1736 "def.y"
        { if (defCallbacks->RegionCbk)
          defData->Region.addRect((yyvsp[-1].pt).x, (yyvsp[-1].pt).y, (yyvsp[0].pt).x, (yyvsp[0].pt).y); }
#line 5460 "def.tab.c"
    break;

  case 265: /* rect_list: rect_list pt pt  */
#line 1739 "def.y"
        { if (defCallbacks->RegionCbk)
          defData->Region.addRect((yyvsp[-1].pt).x, (yyvsp[-1].pt).y, (yyvsp[0].pt).x, (yyvsp[0].pt).y); }
#line 5467 "def.tab.c"
    break;

  case 268: /* $@51: %empty  */
#line 1747 "def.y"
                               {defData->dumb_mode = DEF_MAX_INT; defData->parsing_property = 1; }
#line 5473 "def.tab.c"
    break;

  case 269: /* region_option: '+' K_PROPERTY $@51 region_prop_list  */
#line 1749 "def.y"
         { defData->dumb_mode = 0; defData->parsing_property = 0; }
#line 5479 "def.tab.c"
    break;

  case 270: /* region_option: '+' K_TYPE region_type  */
#line 1751 "def.y"
         {
           if (defData->regTypeDef) {
              if (defCallbacks->RegionCbk) {
                if (defData->regionWarnings++ < defSettings->RegionWarnings) {
                  defError(6563, "The TYPE statement already exists. It has been defined in the REGION statement.");
                  CHKERR();
                }
              }
           }
           if (defCallbacks->RegionCbk) defData->Region.setType((yyvsp[0].string));
           defData->regTypeDef = 1;
         }
#line 5496 "def.tab.c"
    break;

  case 273: /* region_prop: T_STRING NUMBER  */
#line 1770 "def.y"
        {
          if (defCallbacks->RegionCbk) {
             char propTp;
             char* str = ringCopy("                       ");
             propTp = defSettings->RegionProp.propType((yyvsp[-1].string));
             CHKPROPTYPE(propTp, (yyvsp[-1].string), "REGION");
             // For backword compatibility, also set the string value 
             // We will use a temporary string to store the number.
             // The string space is borrowed from the ring buffer
             // in the lexer.
             sprintf(str, "%g", (yyvsp[0].dval));
             defData->Region.addNumProperty((yyvsp[-1].string), (yyvsp[0].dval), str, propTp);
          }
        }
#line 5515 "def.tab.c"
    break;

  case 274: /* region_prop: T_STRING QSTRING  */
#line 1785 "def.y"
        {
          if (defCallbacks->RegionCbk) {
             char propTp;
             propTp = defSettings->RegionProp.propType((yyvsp[-1].string));
             CHKPROPTYPE(propTp, (yyvsp[-1].string), "REGION");
             defData->Region.addProperty((yyvsp[-1].string), (yyvsp[0].string), propTp);
          }
        }
#line 5528 "def.tab.c"
    break;

  case 275: /* region_prop: T_STRING T_STRING  */
#line 1794 "def.y"
        {
          if (defCallbacks->RegionCbk) {
             char propTp;
             propTp = defSettings->RegionProp.propType((yyvsp[-1].string));
             CHKPROPTYPE(propTp, (yyvsp[-1].string), "REGION");
             defData->Region.addProperty((yyvsp[-1].string), (yyvsp[0].string), propTp);
          }
        }
#line 5541 "def.tab.c"
    break;

  case 276: /* region_type: K_FENCE  */
#line 1804 "def.y"
            { (yyval.string) = (char*)"FENCE"; }
#line 5547 "def.tab.c"
    break;

  case 277: /* region_type: K_GUIDE  */
#line 1806 "def.y"
            { (yyval.string) = (char*)"GUIDE"; }
#line 5553 "def.tab.c"
    break;

  case 278: /* comps_maskShift_section: K_COMPSMASKSHIFT layer_statement ';'  */
#line 1809 "def.y"
         {
           if (defData->VersionNum < 5.8) {
                if (defData->componentWarnings++ < defSettings->ComponentWarnings) {
                   defData->defMsg = (char*)defMalloc(10000);
                   sprintf (defData->defMsg,
                     "The MASKSHIFT statement is available in version 5.8 and later.\nHowever, your DEF file is defined with version %g", defData->VersionNum);
                   defError(7415, defData->defMsg);
                   defFree(defData->defMsg);
                   CHKERR();
                }
            }
            if (defCallbacks->ComponentMaskShiftLayerCbk) {
                CALLBACK(defCallbacks->ComponentMaskShiftLayerCbk, defrComponentMaskShiftLayerCbkType, &defData->ComponentMaskShiftLayer);
            }
         }
#line 5573 "def.tab.c"
    break;

  case 280: /* start_comps: K_COMPS NUMBER ';'  */
#line 1829 "def.y"
         { 
            if (defCallbacks->ComponentStartCbk)
              CALLBACK(defCallbacks->ComponentStartCbk, defrComponentStartCbkType,
                       ROUND((yyvsp[-1].dval)));
         }
#line 5583 "def.tab.c"
    break;

  case 283: /* maskLayer: T_STRING  */
#line 1840 "def.y"
        {
            if (defCallbacks->ComponentMaskShiftLayerCbk) {
              defData->ComponentMaskShiftLayer.addMaskShiftLayer((yyvsp[0].string));
            }
        }
#line 5593 "def.tab.c"
    break;

  case 286: /* comp: comp_start comp_options ';'  */
#line 1851 "def.y"
         {
            if (defCallbacks->ComponentCbk)
              CALLBACK(defCallbacks->ComponentCbk, defrComponentCbkType, &defData->Component);
         }
#line 5602 "def.tab.c"
    break;

  case 287: /* comp_start: comp_id_and_name comp_net_list  */
#line 1857 "def.y"
         {
            defData->dumb_mode = 0;
            defData->no_num = 0;
         }
#line 5611 "def.tab.c"
    break;

  case 288: /* $@52: %empty  */
#line 1862 "def.y"
                      {defData->dumb_mode = DEF_MAX_INT; defData->no_num = DEF_MAX_INT; }
#line 5617 "def.tab.c"
    break;

  case 289: /* comp_id_and_name: '-' $@52 T_STRING T_STRING  */
#line 1864 "def.y"
         {
            if (defCallbacks->ComponentCbk)
              defData->Component.IdAndName((yyvsp[-1].string), (yyvsp[0].string));
         }
#line 5626 "def.tab.c"
    break;

  case 290: /* comp_net_list: %empty  */
#line 1870 "def.y"
        { }
#line 5632 "def.tab.c"
    break;

  case 291: /* comp_net_list: comp_net_list '*'  */
#line 1872 "def.y"
            {
              if (defCallbacks->ComponentCbk)
                defData->Component.addNet("*");
            }
#line 5641 "def.tab.c"
    break;

  case 292: /* comp_net_list: comp_net_list T_STRING  */
#line 1877 "def.y"
            {
              if (defCallbacks->ComponentCbk)
                defData->Component.addNet((yyvsp[0].string));
            }
#line 5650 "def.tab.c"
    break;

  case 307: /* comp_extension_stmt: extension_stmt  */
#line 1892 "def.y"
        {
          if (defCallbacks->ComponentCbk)
            CALLBACK(defCallbacks->ComponentExtCbk, defrComponentExtCbkType,
                     &defData->History_text[0]);
        }
#line 5660 "def.tab.c"
    break;

  case 308: /* $@53: %empty  */
#line 1898 "def.y"
                          {defData->dumb_mode=1; defData->no_num = 1; }
#line 5666 "def.tab.c"
    break;

  case 309: /* comp_eeq: '+' K_EEQMASTER $@53 T_STRING  */
#line 1899 "def.y"
        {
          if (defCallbacks->ComponentCbk)
            defData->Component.setEEQ((yyvsp[0].string));
        }
#line 5675 "def.tab.c"
    break;

  case 310: /* $@54: %empty  */
#line 1904 "def.y"
                              { defData->dumb_mode = 2;  defData->no_num = 2; }
#line 5681 "def.tab.c"
    break;

  case 311: /* comp_generate: '+' K_COMP_GEN $@54 T_STRING opt_pattern  */
#line 1906 "def.y"
        {
          if (defCallbacks->ComponentCbk)
             defData->Component.setGenerate((yyvsp[-1].string), (yyvsp[0].string));
        }
#line 5690 "def.tab.c"
    break;

  case 312: /* opt_pattern: %empty  */
#line 1912 "def.y"
      { (yyval.string) = (char*)""; }
#line 5696 "def.tab.c"
    break;

  case 313: /* opt_pattern: T_STRING  */
#line 1914 "def.y"
      { (yyval.string) = (yyvsp[0].string); }
#line 5702 "def.tab.c"
    break;

  case 314: /* comp_source: '+' K_SOURCE source_type  */
#line 1917 "def.y"
        {
          if (defCallbacks->ComponentCbk)
            defData->Component.setSource((yyvsp[0].string));
        }
#line 5711 "def.tab.c"
    break;

  case 315: /* source_type: K_NETLIST  */
#line 1923 "def.y"
            { (yyval.string) = (char*)"NETLIST"; }
#line 5717 "def.tab.c"
    break;

  case 316: /* source_type: K_DIST  */
#line 1925 "def.y"
            { (yyval.string) = (char*)"DIST"; }
#line 5723 "def.tab.c"
    break;

  case 317: /* source_type: K_USER  */
#line 1927 "def.y"
            { (yyval.string) = (char*)"USER"; }
#line 5729 "def.tab.c"
    break;

  case 318: /* source_type: K_TIMING  */
#line 1929 "def.y"
            { (yyval.string) = (char*)"TIMING"; }
#line 5735 "def.tab.c"
    break;

  case 319: /* comp_region: comp_region_start comp_pnt_list  */
#line 1934 "def.y"
        { }
#line 5741 "def.tab.c"
    break;

  case 320: /* comp_region: comp_region_start T_STRING  */
#line 1936 "def.y"
        {
          if (defCallbacks->ComponentCbk)
            defData->Component.setRegionName((yyvsp[0].string));
        }
#line 5750 "def.tab.c"
    break;

  case 321: /* comp_pnt_list: pt pt  */
#line 1942 "def.y"
        { 
          // 11/12/2002 - this is obsolete in 5.5, & will be ignored 
          if (defData->VersionNum < 5.5) {
            if (defCallbacks->ComponentCbk)
               defData->Component.setRegionBounds((yyvsp[-1].pt).x, (yyvsp[-1].pt).y, 
                                                            (yyvsp[0].pt).x, (yyvsp[0].pt).y);
          }
          else
            defWarning(7020, "The REGION pt pt statement is obsolete in version 5.5 and later.\nThe DEF parser will ignore this statement.");
        }
#line 5765 "def.tab.c"
    break;

  case 322: /* comp_pnt_list: comp_pnt_list pt pt  */
#line 1953 "def.y"
        { 
          // 11/12/2002 - this is obsolete in 5.5, & will be ignored 
          if (defData->VersionNum < 5.5) {
            if (defCallbacks->ComponentCbk)
               defData->Component.setRegionBounds((yyvsp[-1].pt).x, (yyvsp[-1].pt).y,
                                                            (yyvsp[0].pt).x, (yyvsp[0].pt).y);
          }
          else
            defWarning(7020, "The REGION pt pt statement is obsolete in version 5.5 and later.\nThe DEF parser will ignore this statement.");
        }
#line 5780 "def.tab.c"
    break;

  case 323: /* $@55: %empty  */
#line 1965 "def.y"
        {
          if (defData->VersionNum < 5.6) {
             if (defCallbacks->ComponentCbk) {
               if (defData->componentWarnings++ < defSettings->ComponentWarnings) {
                 defData->defMsg = (char*)defMalloc(1000);
                 sprintf (defData->defMsg,
                    "The HALO statement is a version 5.6 and later syntax.\nHowever, your DEF file is defined with version %g.", defData->VersionNum);
                 defError(6529, defData->defMsg);
                 defFree(defData->defMsg);
                 CHKERR();
               }
             }
          }
        }
#line 5799 "def.tab.c"
    break;

  case 324: /* comp_halo: '+' K_HALO $@55 halo_soft NUMBER NUMBER NUMBER NUMBER  */
#line 1980 "def.y"
        {
          if (defCallbacks->ComponentCbk)
            defData->Component.setHalo((int)(yyvsp[-3].dval), (int)(yyvsp[-2].dval),
                                                 (int)(yyvsp[-1].dval), (int)(yyvsp[0].dval));
        }
#line 5809 "def.tab.c"
    break;

  case 326: /* halo_soft: K_SOFT  */
#line 1988 "def.y"
      {
        if (defData->VersionNum < 5.7) {
           if (defCallbacks->ComponentCbk) {
             if (defData->componentWarnings++ < defSettings->ComponentWarnings) {
                defData->defMsg = (char*)defMalloc(10000);
                sprintf (defData->defMsg,
                  "The HALO SOFT is available in version 5.7 or later.\nHowever, your DEF file is defined with version %g.", defData->VersionNum);
                defError(6550, defData->defMsg);
                defFree(defData->defMsg);
                CHKERR();
             }
           }
        } else {
           if (defCallbacks->ComponentCbk)
             defData->Component.setHaloSoft();
        }
      }
#line 5831 "def.tab.c"
    break;

  case 327: /* $@56: %empty  */
#line 2007 "def.y"
                                       { defData->dumb_mode = 2; defData->no_num = 2; }
#line 5837 "def.tab.c"
    break;

  case 328: /* comp_routehalo: '+' K_ROUTEHALO NUMBER $@56 T_STRING T_STRING  */
#line 2008 "def.y"
      {
        if (defData->VersionNum < 5.7) {
           if (defCallbacks->ComponentCbk) {
             if (defData->componentWarnings++ < defSettings->ComponentWarnings) {
                defData->defMsg = (char*)defMalloc(10000);
                sprintf (defData->defMsg,
                  "The ROUTEHALO is available in version 5.7 or later.\nHowever, your DEF file is defined with version %g.", defData->VersionNum);
                defError(6551, defData->defMsg);
                defFree(defData->defMsg);
                CHKERR();
             }
           }
        } else {
           if (defCallbacks->ComponentCbk)
             defData->Component.setRouteHalo(
                            (int)(yyvsp[-3].dval), (yyvsp[-1].string), (yyvsp[0].string));
        }
      }
#line 5860 "def.tab.c"
    break;

  case 329: /* $@57: %empty  */
#line 2027 "def.y"
                              { defData->dumb_mode = DEF_MAX_INT; defData->parsing_property = 1; }
#line 5866 "def.tab.c"
    break;

  case 330: /* comp_property: '+' K_PROPERTY $@57 comp_prop_list  */
#line 2029 "def.y"
      { defData->dumb_mode = 0; defData->parsing_property = 0; }
#line 5872 "def.tab.c"
    break;

  case 333: /* comp_prop: T_STRING NUMBER  */
#line 2036 "def.y"
        {
          if (defCallbacks->ComponentCbk) {
            char propTp;
            char* str = ringCopy("                       ");
            propTp = defSettings->CompProp.propType((yyvsp[-1].string));
            CHKPROPTYPE(propTp, (yyvsp[-1].string), "COMPONENT");
            sprintf(str, "%g", (yyvsp[0].dval));
            defData->Component.addNumProperty((yyvsp[-1].string), (yyvsp[0].dval), str, propTp);
          }
        }
#line 5887 "def.tab.c"
    break;

  case 334: /* comp_prop: T_STRING QSTRING  */
#line 2047 "def.y"
        {
          if (defCallbacks->ComponentCbk) {
            char propTp;
            propTp = defSettings->CompProp.propType((yyvsp[-1].string));
            CHKPROPTYPE(propTp, (yyvsp[-1].string), "COMPONENT");
            defData->Component.addProperty((yyvsp[-1].string), (yyvsp[0].string), propTp);
          }
        }
#line 5900 "def.tab.c"
    break;

  case 335: /* comp_prop: T_STRING T_STRING  */
#line 2056 "def.y"
        {
          if (defCallbacks->ComponentCbk) {
            char propTp;
            propTp = defSettings->CompProp.propType((yyvsp[-1].string));
            CHKPROPTYPE(propTp, (yyvsp[-1].string), "COMPONENT");
            defData->Component.addProperty((yyvsp[-1].string), (yyvsp[0].string), propTp);
          }
        }
#line 5913 "def.tab.c"
    break;

  case 336: /* comp_region_start: '+' K_REGION  */
#line 2066 "def.y"
        { defData->dumb_mode = 1; defData->no_num = 1; }
#line 5919 "def.tab.c"
    break;

  case 337: /* $@58: %empty  */
#line 2068 "def.y"
                            { defData->dumb_mode = 1; defData->no_num = 1; }
#line 5925 "def.tab.c"
    break;

  case 338: /* comp_foreign: '+' K_FOREIGN $@58 T_STRING opt_paren orient  */
#line 2070 "def.y"
        { 
          if (defData->VersionNum < 5.6) {
            if (defCallbacks->ComponentCbk) {
              defData->Component.setForeignName((yyvsp[-2].string));
              defData->Component.setForeignLocation((yyvsp[-1].pt).x, (yyvsp[-1].pt).y, (yyvsp[0].integer));
            }
         } else
            if (defCallbacks->ComponentCbk)
              if (defData->componentWarnings++ < defSettings->ComponentWarnings)
                defWarning(7021, "The FOREIGN statement is obsolete in version 5.6 and later.\nThe DEF parser will ignore this statement.");
         }
#line 5941 "def.tab.c"
    break;

  case 339: /* opt_paren: pt  */
#line 2084 "def.y"
         { (yyval.pt) = (yyvsp[0].pt); }
#line 5947 "def.tab.c"
    break;

  case 340: /* opt_paren: NUMBER NUMBER  */
#line 2086 "def.y"
         { (yyval.pt).x = ROUND((yyvsp[-1].dval)); (yyval.pt).y = ROUND((yyvsp[0].dval)); }
#line 5953 "def.tab.c"
    break;

  case 341: /* comp_type: placement_status pt orient  */
#line 2089 "def.y"
        {
          if (defCallbacks->ComponentCbk) {
            defData->Component.setPlacementStatus((yyvsp[-2].integer));
            defData->Component.setPlacementLocation((yyvsp[-1].pt).x, (yyvsp[-1].pt).y, (yyvsp[0].integer));
          }
        }
#line 5964 "def.tab.c"
    break;

  case 342: /* comp_type: '+' K_UNPLACED  */
#line 2096 "def.y"
        {
          if (defCallbacks->ComponentCbk)
            defData->Component.setPlacementStatus(
                                         DEFI_COMPONENT_UNPLACED);
            defData->Component.setPlacementLocation(-1, -1, -1);
        }
#line 5975 "def.tab.c"
    break;

  case 343: /* comp_type: '+' K_UNPLACED pt orient  */
#line 2103 "def.y"
        {
          if (defData->VersionNum < 5.4) {   // PCR 495463 
            if (defCallbacks->ComponentCbk) {
              defData->Component.setPlacementStatus(
                                          DEFI_COMPONENT_UNPLACED);
              defData->Component.setPlacementLocation((yyvsp[-1].pt).x, (yyvsp[-1].pt).y, (yyvsp[0].integer));
            }
          } else {
            if (defData->componentWarnings++ < defSettings->ComponentWarnings)
               defWarning(7022, "In the COMPONENT UNPLACED statement, point and orient are invalid in version 5.4 and later.\nThe DEF parser will ignore this statement.");
          }
        }
#line 5992 "def.tab.c"
    break;

  case 344: /* maskShift: '+' K_MASKSHIFT NUMBER  */
#line 2117 "def.y"
        {  
          if (defCallbacks->ComponentCbk) {
            if (validateMaskInput((int)(yyvsp[0].dval), defData->componentWarnings, defSettings->ComponentWarnings)) {
                defData->Component.setMaskShift(int((yyvsp[0].dval)));
            }
          }
        }
#line 6004 "def.tab.c"
    break;

  case 345: /* placement_status: '+' K_FIXED  */
#line 2126 "def.y"
        { (yyval.integer) = DEFI_COMPONENT_FIXED; }
#line 6010 "def.tab.c"
    break;

  case 346: /* placement_status: '+' K_COVER  */
#line 2128 "def.y"
        { (yyval.integer) = DEFI_COMPONENT_COVER; }
#line 6016 "def.tab.c"
    break;

  case 347: /* placement_status: '+' K_PLACED  */
#line 2130 "def.y"
        { (yyval.integer) = DEFI_COMPONENT_PLACED; }
#line 6022 "def.tab.c"
    break;

  case 348: /* weight: '+' K_WEIGHT NUMBER  */
#line 2133 "def.y"
        {
          if (defCallbacks->ComponentCbk)
            defData->Component.setWeight(ROUND((yyvsp[0].dval)));
        }
#line 6031 "def.tab.c"
    break;

  case 349: /* end_comps: K_END K_COMPS  */
#line 2139 "def.y"
        { 
          if (defCallbacks->ComponentCbk)
            CALLBACK(defCallbacks->ComponentEndCbk, defrComponentEndCbkType, 0);
        }
#line 6040 "def.tab.c"
    break;

  case 351: /* start_nets: K_NETS NUMBER ';'  */
#line 2148 "def.y"
        { 
          if (defCallbacks->NetStartCbk)
            CALLBACK(defCallbacks->NetStartCbk, defrNetStartCbkType, ROUND((yyvsp[-1].dval)));
          defData->netOsnet = 1;
        }
#line 6050 "def.tab.c"
    break;

  case 354: /* one_net: net_and_connections net_options ';'  */
#line 2159 "def.y"
        { 
          if (defCallbacks->NetCbk)
            CALLBACK(defCallbacks->NetCbk, defrNetCbkType, &defData->Net);
        }
#line 6059 "def.tab.c"
    break;

  case 355: /* net_and_connections: net_start  */
#line 2170 "def.y"
        {defData->dumb_mode = 0; defData->no_num = 0; }
#line 6065 "def.tab.c"
    break;

  case 356: /* $@59: %empty  */
#line 2173 "def.y"
               {defData->dumb_mode = DEF_MAX_INT; defData->no_num = DEF_MAX_INT; defData->nondef_is_keyword = TRUE; defData->mustjoin_is_keyword = TRUE;}
#line 6071 "def.tab.c"
    break;

  case 358: /* $@60: %empty  */
#line 2176 "def.y"
        {
          // 9/22/1999 
          // this is shared by both net and special net 
          if ((defCallbacks->NetCbk && (defData->netOsnet==1)) || (defCallbacks->SNetCbk && (defData->netOsnet==2)))
            defData->Net.setName((yyvsp[0].string));
          if (defCallbacks->NetNameCbk)
            CALLBACK(defCallbacks->NetNameCbk, defrNetNameCbkType, (yyvsp[0].string));
        }
#line 6084 "def.tab.c"
    break;

  case 360: /* $@61: %empty  */
#line 2184 "def.y"
                                  {defData->dumb_mode = 1; defData->no_num = 1;}
#line 6090 "def.tab.c"
    break;

  case 361: /* net_name: K_MUSTJOIN '(' T_STRING $@61 T_STRING ')'  */
#line 2185 "def.y"
        {
          if ((defCallbacks->NetCbk && (defData->netOsnet==1)) || (defCallbacks->SNetCbk && (defData->netOsnet==2)))
            defData->Net.addMustPin((yyvsp[-3].string), (yyvsp[-1].string), 0);
          defData->dumb_mode = 3;
          defData->no_num = 3;
        }
#line 6101 "def.tab.c"
    break;

  case 364: /* $@62: %empty  */
#line 2196 "def.y"
                             {defData->dumb_mode = DEF_MAX_INT; defData->no_num = DEF_MAX_INT;}
#line 6107 "def.tab.c"
    break;

  case 365: /* net_connection: '(' T_STRING $@62 T_STRING conn_opt ')'  */
#line 2198 "def.y"
        {
          // 9/22/1999 
          // since the code is shared by both net & special net, 
          // need to check on both flags 
          if ((defCallbacks->NetCbk && (defData->netOsnet==1)) || (defCallbacks->SNetCbk && (defData->netOsnet==2)))
            defData->Net.addPin((yyvsp[-4].string), (yyvsp[-2].string), (yyvsp[-1].integer));
          // 1/14/2000 - pcr 289156 
          // reset defData->dumb_mode & defData->no_num to 3 , just in case 
          // the next statement is another net_connection 
          defData->dumb_mode = 3;
          defData->no_num = 3;
        }
#line 6124 "def.tab.c"
    break;

  case 366: /* $@63: %empty  */
#line 2210 "def.y"
                  {defData->dumb_mode = 1; defData->no_num = 1;}
#line 6130 "def.tab.c"
    break;

  case 367: /* net_connection: '(' '*' $@63 T_STRING conn_opt ')'  */
#line 2211 "def.y"
        {
          if ((defCallbacks->NetCbk && (defData->netOsnet==1)) || (defCallbacks->SNetCbk && (defData->netOsnet==2)))
            defData->Net.addPin("*", (yyvsp[-2].string), (yyvsp[-1].integer));
          defData->dumb_mode = 3;
          defData->no_num = 3;
        }
#line 6141 "def.tab.c"
    break;

  case 368: /* $@64: %empty  */
#line 2217 "def.y"
                    {defData->dumb_mode = 1; defData->no_num = 1;}
#line 6147 "def.tab.c"
    break;

  case 369: /* net_connection: '(' K_PIN $@64 T_STRING conn_opt ')'  */
#line 2218 "def.y"
        {
          if ((defCallbacks->NetCbk && (defData->netOsnet==1)) || (defCallbacks->SNetCbk && (defData->netOsnet==2)))
            defData->Net.addPin("PIN", (yyvsp[-2].string), (yyvsp[-1].integer));
          defData->dumb_mode = 3;
          defData->no_num = 3;
        }
#line 6158 "def.tab.c"
    break;

  case 370: /* conn_opt: %empty  */
#line 2226 "def.y"
          { (yyval.integer) = 0; }
#line 6164 "def.tab.c"
    break;

  case 371: /* conn_opt: extension_stmt  */
#line 2228 "def.y"
        {
          if (defCallbacks->NetConnectionExtCbk)
            CALLBACK(defCallbacks->NetConnectionExtCbk, defrNetConnectionExtCbkType,
              &defData->History_text[0]);
          (yyval.integer) = 0;
        }
#line 6175 "def.tab.c"
    break;

  case 372: /* conn_opt: '+' K_SYNTHESIZED  */
#line 2235 "def.y"
        { (yyval.integer) = 1; }
#line 6181 "def.tab.c"
    break;

  case 375: /* $@65: %empty  */
#line 2244 "def.y"
        {  
          if (defCallbacks->NetCbk) defData->Net.addWire((yyvsp[0].string), NULL);
        }
#line 6189 "def.tab.c"
    break;

  case 376: /* net_option: '+' net_type $@65 paths  */
#line 2248 "def.y"
        {
          defData->by_is_keyword = FALSE;
          defData->do_is_keyword = FALSE;
          defData->new_is_keyword = FALSE;
          defData->nondef_is_keyword = FALSE;
          defData->mustjoin_is_keyword = FALSE;
          defData->step_is_keyword = FALSE;
          defData->orient_is_keyword = FALSE;
          defData->virtual_is_keyword = FALSE;
          defData->rect_is_keyword = FALSE;
          defData->mask_is_keyword = FALSE;
          defData->needNPCbk = 0;
        }
#line 6207 "def.tab.c"
    break;

  case 377: /* net_option: '+' K_SOURCE netsource_type  */
#line 2263 "def.y"
        { if (defCallbacks->NetCbk) defData->Net.setSource((yyvsp[0].string)); }
#line 6213 "def.tab.c"
    break;

  case 378: /* net_option: '+' K_FIXEDBUMP  */
#line 2266 "def.y"
        {
          if (defData->VersionNum < 5.5) {
            if (defCallbacks->NetCbk) {
              if (defData->netWarnings++ < defSettings->NetWarnings) {
                 defData->defMsg = (char*)defMalloc(1000);
                 sprintf (defData->defMsg,
                    "The FIXEDBUMP statement is available in version 5.5 and later.\nHowever, your DEF file is defined with version %g.", defData->VersionNum);
                 defError(6530, defData->defMsg);
                 defFree(defData->defMsg);
                 CHKERR();
              }
            }
          }
          if (defCallbacks->NetCbk) defData->Net.setFixedbump();
        }
#line 6233 "def.tab.c"
    break;

  case 379: /* $@66: %empty  */
#line 2282 "def.y"
                          { defData->real_num = 1; }
#line 6239 "def.tab.c"
    break;

  case 380: /* net_option: '+' K_FREQUENCY $@66 NUMBER  */
#line 2283 "def.y"
        {
          if (defData->VersionNum < 5.5) {
            if (defCallbacks->NetCbk) {
              if (defData->netWarnings++ < defSettings->NetWarnings) {
                 defData->defMsg = (char*)defMalloc(1000);
                 sprintf (defData->defMsg,
                    "The FREQUENCY statement is a version 5.5 and later syntax.\nHowever, your DEF file is defined with version %g", defData->VersionNum);
                 defError(6558, defData->defMsg);
                 defFree(defData->defMsg);
                 CHKERR();
              }
            }
          }
          if (defCallbacks->NetCbk) defData->Net.setFrequency((yyvsp[0].dval));
          defData->real_num = 0;
        }
#line 6260 "def.tab.c"
    break;

  case 381: /* $@67: %empty  */
#line 2300 "def.y"
                         {defData->dumb_mode = 1; defData->no_num = 1;}
#line 6266 "def.tab.c"
    break;

  case 382: /* net_option: '+' K_ORIGINAL $@67 T_STRING  */
#line 2301 "def.y"
        { if (defCallbacks->NetCbk) defData->Net.setOriginal((yyvsp[0].string)); }
#line 6272 "def.tab.c"
    break;

  case 383: /* net_option: '+' K_PATTERN pattern_type  */
#line 2304 "def.y"
        { if (defCallbacks->NetCbk) defData->Net.setPattern((yyvsp[0].string)); }
#line 6278 "def.tab.c"
    break;

  case 384: /* net_option: '+' K_WEIGHT NUMBER  */
#line 2307 "def.y"
        { if (defCallbacks->NetCbk) defData->Net.setWeight(ROUND((yyvsp[0].dval))); }
#line 6284 "def.tab.c"
    break;

  case 385: /* net_option: '+' K_XTALK NUMBER  */
#line 2310 "def.y"
        { if (defCallbacks->NetCbk) defData->Net.setXTalk(ROUND((yyvsp[0].dval))); }
#line 6290 "def.tab.c"
    break;

  case 386: /* net_option: '+' K_ESTCAP NUMBER  */
#line 2313 "def.y"
        { if (defCallbacks->NetCbk) defData->Net.setCap((yyvsp[0].dval)); }
#line 6296 "def.tab.c"
    break;

  case 387: /* net_option: '+' K_USE use_type  */
#line 2316 "def.y"
        { if (defCallbacks->NetCbk) defData->Net.setUse((yyvsp[0].string)); }
#line 6302 "def.tab.c"
    break;

  case 388: /* net_option: '+' K_STYLE NUMBER  */
#line 2319 "def.y"
        { if (defCallbacks->NetCbk) defData->Net.setStyle((int)(yyvsp[0].dval)); }
#line 6308 "def.tab.c"
    break;

  case 389: /* $@68: %empty  */
#line 2321 "def.y"
                               { defData->dumb_mode = 1; defData->no_num = 1; }
#line 6314 "def.tab.c"
    break;

  case 390: /* net_option: '+' K_NONDEFAULTRULE $@68 T_STRING  */
#line 2322 "def.y"
        { 
          if (defCallbacks->NetCbk && defCallbacks->NetNonDefaultRuleCbk) {
             // User wants a callback on nondefaultrule 
             CALLBACK(defCallbacks->NetNonDefaultRuleCbk,
                      defrNetNonDefaultRuleCbkType, (yyvsp[0].string));
          }
          // Still save data in the class 
          if (defCallbacks->NetCbk) defData->Net.setNonDefaultRule((yyvsp[0].string));
        }
#line 6328 "def.tab.c"
    break;

  case 392: /* $@69: %empty  */
#line 2334 "def.y"
                          { defData->dumb_mode = 1; defData->no_num = 1; }
#line 6334 "def.tab.c"
    break;

  case 393: /* net_option: '+' K_SHIELDNET $@69 T_STRING  */
#line 2335 "def.y"
        { if (defCallbacks->NetCbk) defData->Net.addShieldNet((yyvsp[0].string)); }
#line 6340 "def.tab.c"
    break;

  case 394: /* $@70: %empty  */
#line 2337 "def.y"
                         { defData->dumb_mode = 1; defData->no_num = 1; }
#line 6346 "def.tab.c"
    break;

  case 395: /* $@71: %empty  */
#line 2338 "def.y"
        { // since the parser still support 5.3 and earlier, can't 
          // move NOSHIELD in net_type 
          if (defData->VersionNum < 5.4) {   // PCR 445209 
            if (defCallbacks->NetCbk) defData->Net.addNoShield("");
            defData->by_is_keyword = FALSE;
            defData->do_is_keyword = FALSE;
            defData->new_is_keyword = FALSE;
            defData->step_is_keyword = FALSE;
            defData->orient_is_keyword = FALSE;
            defData->virtual_is_keyword = FALSE;
            defData->mask_is_keyword = FALSE;
            defData->rect_is_keyword = FALSE;
            defData->shield = TRUE;    // save the path info in the defData->shield paths 
          } else
            if (defCallbacks->NetCbk) defData->Net.addWire("NOSHIELD", NULL);
        }
#line 6367 "def.tab.c"
    break;

  case 396: /* net_option: '+' K_NOSHIELD $@70 $@71 paths  */
#line 2355 "def.y"
        {
          if (defData->VersionNum < 5.4) {   // PCR 445209 
            defData->shield = FALSE;
            defData->by_is_keyword = FALSE;
            defData->do_is_keyword = FALSE;
            defData->new_is_keyword = FALSE;
            defData->step_is_keyword = FALSE;
            defData->nondef_is_keyword = FALSE;
            defData->mustjoin_is_keyword = FALSE;
            defData->orient_is_keyword = FALSE;
            defData->virtual_is_keyword = FALSE;
            defData->rect_is_keyword = FALSE;
            defData->mask_is_keyword = FALSE;
          } else {
            defData->by_is_keyword = FALSE;
            defData->do_is_keyword = FALSE;
            defData->new_is_keyword = FALSE;
            defData->step_is_keyword = FALSE;
            defData->nondef_is_keyword = FALSE;
            defData->mustjoin_is_keyword = FALSE;
            defData->orient_is_keyword = FALSE;
            defData->virtual_is_keyword = FALSE;
            defData->rect_is_keyword = FALSE;
            defData->mask_is_keyword = FALSE;
          }
          defData->needNPCbk = 0;
        }
#line 6399 "def.tab.c"
    break;

  case 397: /* $@72: %empty  */
#line 2384 "def.y"
        { defData->dumb_mode = 1; defData->no_num = 1;
          if (defCallbacks->NetCbk) {
            defData->Subnet = (defiSubnet*)defMalloc(sizeof(defiSubnet));
            defData->Subnet->Init();
          }
        }
#line 6410 "def.tab.c"
    break;

  case 398: /* $@73: %empty  */
#line 2390 "def.y"
                 {
          if (defCallbacks->NetCbk && defCallbacks->NetSubnetNameCbk) {
            // User wants a callback on Net subnetName 
            CALLBACK(defCallbacks->NetSubnetNameCbk, defrNetSubnetNameCbkType, (yyvsp[0].string));
          }
          // Still save the subnet name in the class 
          if (defCallbacks->NetCbk) {
            defData->Subnet->setName((yyvsp[0].string));
          }
        }
#line 6425 "def.tab.c"
    break;

  case 399: /* $@74: %empty  */
#line 2400 "def.y"
                   {
          defData->routed_is_keyword = TRUE;
          defData->fixed_is_keyword = TRUE;
          defData->cover_is_keyword = TRUE;
        }
#line 6435 "def.tab.c"
    break;

  case 400: /* net_option: '+' K_SUBNET $@72 T_STRING $@73 comp_names $@74 subnet_options  */
#line 2404 "def.y"
                         {
          if (defCallbacks->NetCbk) {
            defData->Net.addSubnet(defData->Subnet);
            defData->Subnet = NULL;
            defData->routed_is_keyword = FALSE;
            defData->fixed_is_keyword = FALSE;
            defData->cover_is_keyword = FALSE;
          }
        }
#line 6449 "def.tab.c"
    break;

  case 401: /* $@75: %empty  */
#line 2414 "def.y"
                         {defData->dumb_mode = DEF_MAX_INT; defData->parsing_property = 1; }
#line 6455 "def.tab.c"
    break;

  case 402: /* net_option: '+' K_PROPERTY $@75 net_prop_list  */
#line 2416 "def.y"
        { defData->dumb_mode = 0; defData->parsing_property = 0; }
#line 6461 "def.tab.c"
    break;

  case 403: /* net_option: extension_stmt  */
#line 2419 "def.y"
        { 
          if (defCallbacks->NetExtCbk)
            CALLBACK(defCallbacks->NetExtCbk, defrNetExtCbkType, &defData->History_text[0]);
        }
#line 6470 "def.tab.c"
    break;

  case 406: /* net_prop: T_STRING NUMBER  */
#line 2429 "def.y"
        {
          if (defCallbacks->NetCbk) {
            char propTp;
            char* str = ringCopy("                       ");
            propTp = defSettings->NetProp.propType((yyvsp[-1].string));
            CHKPROPTYPE(propTp, (yyvsp[-1].string), "NET");
            sprintf(str, "%g", (yyvsp[0].dval));
            defData->Net.addNumProp((yyvsp[-1].string), (yyvsp[0].dval), str, propTp);
          }
        }
#line 6485 "def.tab.c"
    break;

  case 407: /* net_prop: T_STRING QSTRING  */
#line 2440 "def.y"
        {
          if (defCallbacks->NetCbk) {
            char propTp;
            propTp = defSettings->NetProp.propType((yyvsp[-1].string));
            CHKPROPTYPE(propTp, (yyvsp[-1].string), "NET");
            defData->Net.addProp((yyvsp[-1].string), (yyvsp[0].string), propTp);
          }
        }
#line 6498 "def.tab.c"
    break;

  case 408: /* net_prop: T_STRING T_STRING  */
#line 2449 "def.y"
        {
          if (defCallbacks->NetCbk) {
            char propTp;
            propTp = defSettings->NetProp.propType((yyvsp[-1].string));
            CHKPROPTYPE(propTp, (yyvsp[-1].string), "NET");
            defData->Net.addProp((yyvsp[-1].string), (yyvsp[0].string), propTp);
          }
        }
#line 6511 "def.tab.c"
    break;

  case 409: /* netsource_type: K_NETLIST  */
#line 2459 "def.y"
        { (yyval.string) = (char*)"NETLIST"; }
#line 6517 "def.tab.c"
    break;

  case 410: /* netsource_type: K_DIST  */
#line 2461 "def.y"
        { (yyval.string) = (char*)"DIST"; }
#line 6523 "def.tab.c"
    break;

  case 411: /* netsource_type: K_USER  */
#line 2463 "def.y"
        { (yyval.string) = (char*)"USER"; }
#line 6529 "def.tab.c"
    break;

  case 412: /* netsource_type: K_TIMING  */
#line 2465 "def.y"
        { (yyval.string) = (char*)"TIMING"; }
#line 6535 "def.tab.c"
    break;

  case 413: /* netsource_type: K_TEST  */
#line 2467 "def.y"
        { (yyval.string) = (char*)"TEST"; }
#line 6541 "def.tab.c"
    break;

  case 414: /* $@76: %empty  */
#line 2470 "def.y"
        {
          // vpin_options may have to deal with orient 
          defData->orient_is_keyword = TRUE;
        }
#line 6550 "def.tab.c"
    break;

  case 415: /* vpin_stmt: vpin_begin vpin_layer_opt pt pt $@76 vpin_options  */
#line 2475 "def.y"
        { if (defCallbacks->NetCbk)
            defData->Net.addVpinBounds((yyvsp[-3].pt).x, (yyvsp[-3].pt).y, (yyvsp[-2].pt).x, (yyvsp[-2].pt).y);
          defData->orient_is_keyword = FALSE;
        }
#line 6559 "def.tab.c"
    break;

  case 416: /* $@77: %empty  */
#line 2480 "def.y"
                       {defData->dumb_mode = 1; defData->no_num = 1;}
#line 6565 "def.tab.c"
    break;

  case 417: /* vpin_begin: '+' K_VPIN $@77 T_STRING  */
#line 2481 "def.y"
        { if (defCallbacks->NetCbk) defData->Net.addVpin((yyvsp[0].string)); }
#line 6571 "def.tab.c"
    break;

  case 419: /* $@78: %empty  */
#line 2484 "def.y"
                  {defData->dumb_mode=1;}
#line 6577 "def.tab.c"
    break;

  case 420: /* vpin_layer_opt: K_LAYER $@78 T_STRING  */
#line 2485 "def.y"
        { if (defCallbacks->NetCbk) defData->Net.addVpinLayer((yyvsp[0].string)); }
#line 6583 "def.tab.c"
    break;

  case 422: /* vpin_options: vpin_status pt orient  */
#line 2489 "def.y"
        { if (defCallbacks->NetCbk) defData->Net.addVpinLoc((yyvsp[-2].string), (yyvsp[-1].pt).x, (yyvsp[-1].pt).y, (yyvsp[0].integer)); }
#line 6589 "def.tab.c"
    break;

  case 423: /* vpin_status: K_PLACED  */
#line 2492 "def.y"
        { (yyval.string) = (char*)"PLACED"; }
#line 6595 "def.tab.c"
    break;

  case 424: /* vpin_status: K_FIXED  */
#line 2494 "def.y"
        { (yyval.string) = (char*)"FIXED"; }
#line 6601 "def.tab.c"
    break;

  case 425: /* vpin_status: K_COVER  */
#line 2496 "def.y"
        { (yyval.string) = (char*)"COVER"; }
#line 6607 "def.tab.c"
    break;

  case 426: /* net_type: K_FIXED  */
#line 2499 "def.y"
        { (yyval.string) = (char*)"FIXED"; defData->dumb_mode = 1; }
#line 6613 "def.tab.c"
    break;

  case 427: /* net_type: K_COVER  */
#line 2501 "def.y"
        { (yyval.string) = (char*)"COVER"; defData->dumb_mode = 1; }
#line 6619 "def.tab.c"
    break;

  case 428: /* net_type: K_ROUTED  */
#line 2503 "def.y"
        { (yyval.string) = (char*)"ROUTED"; defData->dumb_mode = 1; }
#line 6625 "def.tab.c"
    break;

  case 429: /* paths: path  */
#line 2507 "def.y"
      { if (defData->NeedPathData && defCallbacks->NetCbk)
          pathIsDone(defData->shield, 0, defData->netOsnet, &defData->needNPCbk);
      }
#line 6633 "def.tab.c"
    break;

  case 430: /* paths: paths new_path  */
#line 2511 "def.y"
      { }
#line 6639 "def.tab.c"
    break;

  case 431: /* $@79: %empty  */
#line 2513 "def.y"
                { defData->dumb_mode = 1; }
#line 6645 "def.tab.c"
    break;

  case 432: /* new_path: K_NEW $@79 path  */
#line 2514 "def.y"
      { if (defData->NeedPathData && defCallbacks->NetCbk)
          pathIsDone(defData->shield, 0, defData->netOsnet, &defData->needNPCbk);
      }
#line 6653 "def.tab.c"
    break;

  case 433: /* $@80: %empty  */
#line 2519 "def.y"
      {
        if ((strcmp((yyvsp[0].string), "TAPER") == 0) || (strcmp((yyvsp[0].string), "TAPERRULE") == 0)) {
          if (defData->NeedPathData && defCallbacks->NetCbk) {
            if (defData->netWarnings++ < defSettings->NetWarnings) {
              defError(6531, "The layerName which is required in path is missing. Include the layerName in the path and then try again.");
              CHKERR();
            }
          }
          // Since there is already error, the next token is insignificant 
          defData->dumb_mode = 1; defData->no_num = 1;
        } else {
          // CCR 766289 - Do not accummulate the layer information if there 
          // is not a callback set 
          if (defData->NeedPathData && defCallbacks->NetCbk)
              defData->PathObj.addLayer((yyvsp[0].string));
          defData->dumb_mode = 0; defData->no_num = 0;
        }
      }
#line 6676 "def.tab.c"
    break;

  case 434: /* $@81: %empty  */
#line 2538 "def.y"
      { defData->dumb_mode = DEF_MAX_INT; defData->by_is_keyword = TRUE; defData->do_is_keyword = TRUE;
/*
       dumb_mode = 1; by_is_keyword = TRUE; do_is_keyword = TRUE;
*/
        defData->new_is_keyword = TRUE; defData->step_is_keyword = TRUE; 
        defData->orient_is_keyword = TRUE; defData->virtual_is_keyword = TRUE;
        defData->mask_is_keyword = TRUE, defData->rect_is_keyword = TRUE;  }
#line 6688 "def.tab.c"
    break;

  case 435: /* path: T_STRING $@80 opt_taper_style_s path_pt $@81 path_item_list  */
#line 2548 "def.y"
      { defData->dumb_mode = 0;   defData->virtual_is_keyword = FALSE; defData->mask_is_keyword = FALSE,
       defData->rect_is_keyword = FALSE; }
#line 6695 "def.tab.c"
    break;

  case 436: /* virtual_statement: K_VIRTUAL virtual_pt  */
#line 2553 "def.y"
    {
      if (defData->VersionNum < 5.8) {
              if (defCallbacks->SNetCbk) {
                if (defData->sNetWarnings++ < defSettings->SNetWarnings) {
                  defData->defMsg = (char*)defMalloc(1000);
                  sprintf (defData->defMsg,
                     "The VIRTUAL statement is available in version 5.8 and later.\nHowever, your DEF file is defined with version %g", defData->VersionNum);
                  defError(6536, defData->defMsg);
                  defFree(defData->defMsg);
                  CHKERR();
                }
              }
          }
    }
#line 6714 "def.tab.c"
    break;

  case 437: /* rect_statement: K_RECT rect_pts  */
#line 2570 "def.y"
    {
      if (defData->VersionNum < 5.8) {
              if (defCallbacks->SNetCbk) {
                if (defData->sNetWarnings++ < defSettings->SNetWarnings) {
                  defData->defMsg = (char*)defMalloc(1000);
                  sprintf (defData->defMsg,
                     "The RECT statement is available in version 5.8 and later.\nHowever, your DEF file is defined with version %g", defData->VersionNum);
                  defError(6536, defData->defMsg);
                  defFree(defData->defMsg);
                  CHKERR();
                }
              }
      }
    }
#line 6733 "def.tab.c"
    break;

  case 440: /* path_item: T_STRING  */
#line 2593 "def.y"
      {
        if (defData->NeedPathData && ((defCallbacks->NetCbk && (defData->netOsnet==1)) ||
            (defCallbacks->SNetCbk && (defData->netOsnet==2)))) {
          if (strcmp((yyvsp[0].string), "TAPER") == 0)
            defData->PathObj.setTaper();
          else {
            defData->PathObj.addVia((yyvsp[0].string));
            }
        }
      }
#line 6748 "def.tab.c"
    break;

  case 441: /* path_item: K_MASK NUMBER T_STRING  */
#line 2604 "def.y"
      {
        if (validateMaskInput((int)(yyvsp[-1].dval), defData->sNetWarnings, defSettings->SNetWarnings)) {
            if (defData->NeedPathData && ((defCallbacks->NetCbk && (defData->netOsnet==1)) ||
                (defCallbacks->SNetCbk && (defData->netOsnet==2)))) {
              if (strcmp((yyvsp[0].string), "TAPER") == 0)
                defData->PathObj.setTaper();
              else {
                defData->PathObj.addViaMask((yyvsp[-1].dval));
                defData->PathObj.addVia((yyvsp[0].string));
                }
            }
        }
      }
#line 6766 "def.tab.c"
    break;

  case 442: /* path_item: T_STRING orient  */
#line 2618 "def.y"
      { if (defData->NeedPathData && ((defCallbacks->NetCbk && (defData->netOsnet==1)) ||
            (defCallbacks->SNetCbk && (defData->netOsnet==2)))) {
            defData->PathObj.addVia((yyvsp[-1].string));
            defData->PathObj.addViaRotation((yyvsp[0].integer));
        }
      }
#line 6777 "def.tab.c"
    break;

  case 443: /* path_item: K_MASK NUMBER T_STRING orient  */
#line 2625 "def.y"
      { 
        if (validateMaskInput((int)(yyvsp[-2].dval), defData->sNetWarnings, defSettings->SNetWarnings)) {
            if (defData->NeedPathData && ((defCallbacks->NetCbk && (defData->netOsnet==1)) ||
                (defCallbacks->SNetCbk && (defData->netOsnet==2)))) {
                defData->PathObj.addViaMask((yyvsp[-2].dval));
                defData->PathObj.addVia((yyvsp[-1].string));
                defData->PathObj.addViaRotation((yyvsp[0].integer));
            }
        }
      }
#line 6792 "def.tab.c"
    break;

  case 444: /* path_item: K_MASK NUMBER T_STRING K_DO NUMBER K_BY NUMBER K_STEP NUMBER NUMBER  */
#line 2636 "def.y"
      {
        if (validateMaskInput((int)(yyvsp[-8].dval), defData->sNetWarnings, defSettings->SNetWarnings)) {      
            if (((yyvsp[-5].dval) == 0) || ((yyvsp[-3].dval) == 0)) {
              if (defData->NeedPathData &&
                  defCallbacks->SNetCbk) {
                if (defData->netWarnings++ < defSettings->NetWarnings) {
                  defError(6533, "Either the numX or numY in the VIA DO statement has the value. The value specified is 0.\nUpdate your DEF file with the correct value and then try again.\n");
                  CHKERR();
                }
              }
            }
            if (defData->NeedPathData && (defCallbacks->SNetCbk && (defData->netOsnet==2))) {
                defData->PathObj.addViaMask((yyvsp[-8].dval));
                defData->PathObj.addVia((yyvsp[-7].string));
                defData->PathObj.addViaData((int)(yyvsp[-5].dval), (int)(yyvsp[-3].dval), (int)(yyvsp[-1].dval), (int)(yyvsp[0].dval));
            }  else if (defData->NeedPathData && (defCallbacks->NetCbk && (defData->netOsnet==1))) {
              if (defData->netWarnings++ < defSettings->NetWarnings) {
                defError(6567, "The VIA DO statement is defined in the NET statement and is invalid.\nRemove this statement from your DEF file and try again.");
                CHKERR();
              }
            }
        }
      }
#line 6820 "def.tab.c"
    break;

  case 445: /* path_item: T_STRING K_DO NUMBER K_BY NUMBER K_STEP NUMBER NUMBER  */
#line 2660 "def.y"
      {
        if (defData->VersionNum < 5.5) {
          if (defData->NeedPathData && 
              defCallbacks->SNetCbk) {
            if (defData->netWarnings++ < defSettings->NetWarnings) {
              defData->defMsg = (char*)defMalloc(1000);
              sprintf (defData->defMsg,
                 "The VIA DO statement is available in version 5.5 and later.\nHowever, your DEF file is defined with version %g", defData->VersionNum);
              defError(6532, defData->defMsg);
              defFree(defData->defMsg);
              CHKERR();
            }
          }
        }
        if (((yyvsp[-5].dval) == 0) || ((yyvsp[-3].dval) == 0)) {
          if (defData->NeedPathData &&
              defCallbacks->SNetCbk) {
            if (defData->netWarnings++ < defSettings->NetWarnings) {
              defError(6533, "Either the numX or numY in the VIA DO statement has the value. The value specified is 0.\nUpdate your DEF file with the correct value and then try again.\n");
              CHKERR();
            }
          }
        }
        if (defData->NeedPathData && (defCallbacks->SNetCbk && (defData->netOsnet==2))) {
            defData->PathObj.addVia((yyvsp[-7].string));
            defData->PathObj.addViaData((int)(yyvsp[-5].dval), (int)(yyvsp[-3].dval), (int)(yyvsp[-1].dval), (int)(yyvsp[0].dval));
        }  else if (defData->NeedPathData && (defCallbacks->NetCbk && (defData->netOsnet==1))) {
          if (defData->netWarnings++ < defSettings->NetWarnings) {
            defError(6567, "The VIA DO statement is defined in the NET statement and is invalid.\nRemove this statement from your DEF file and try again.");
            CHKERR();
          }
        }
      }
#line 6858 "def.tab.c"
    break;

  case 446: /* path_item: T_STRING orient K_DO NUMBER K_BY NUMBER K_STEP NUMBER NUMBER  */
#line 2694 "def.y"
      {
        if (defData->VersionNum < 5.5) {
          if (defData->NeedPathData &&
              defCallbacks->SNetCbk) {
            if (defData->netWarnings++ < defSettings->NetWarnings) {
              defData->defMsg = (char*)defMalloc(1000);
              sprintf (defData->defMsg,
                 "The VIA DO statement is available in version 5.5 and later.\nHowever, your DEF file is defined with version %g", defData->VersionNum);
              defError(6532, defData->defMsg);
              CHKERR();
            }
          }
        }
        if (((yyvsp[-5].dval) == 0) || ((yyvsp[-3].dval) == 0)) {
          if (defData->NeedPathData &&
              defCallbacks->SNetCbk) {
            if (defData->netWarnings++ < defSettings->NetWarnings) {
              defError(6533, "Either the numX or numY in the VIA DO statement has the value. The value specified is 0.\nUpdate your DEF file with the correct value and then try again.\n");
              CHKERR();
            }
          }
        }
        if (defData->NeedPathData && (defCallbacks->SNetCbk && (defData->netOsnet==2))) {
            defData->PathObj.addVia((yyvsp[-8].string));
            defData->PathObj.addViaRotation((yyvsp[-7].integer));
            defData->PathObj.addViaData((int)(yyvsp[-5].dval), (int)(yyvsp[-3].dval), (int)(yyvsp[-1].dval), (int)(yyvsp[0].dval));
        } else if (defData->NeedPathData && (defCallbacks->NetCbk && (defData->netOsnet==1))) {
          if (defData->netWarnings++ < defSettings->NetWarnings) {
            defError(6567, "The VIA DO statement is defined in the NET statement and is invalid.\nRemove this statement from your DEF file and try again.");
            CHKERR();
          }
        }
      }
#line 6896 "def.tab.c"
    break;

  case 447: /* path_item: K_MASK NUMBER T_STRING orient K_DO NUMBER K_BY NUMBER K_STEP NUMBER NUMBER  */
#line 2728 "def.y"
      {
        if (validateMaskInput((int)(yyvsp[-9].dval), defData->sNetWarnings, defSettings->SNetWarnings)) {
            if (((yyvsp[-5].dval) == 0) || ((yyvsp[-3].dval) == 0)) {
              if (defData->NeedPathData &&
                  defCallbacks->SNetCbk) {
                if (defData->netWarnings++ < defSettings->NetWarnings) {
                  defError(6533, "Either the numX or numY in the VIA DO statement has the value. The value specified is 0.\nUpdate your DEF file with the correct value and then try again.\n");
                  CHKERR();
                }
              }
            }
            if (defData->NeedPathData && (defCallbacks->SNetCbk && (defData->netOsnet==2))) {
                defData->PathObj.addViaMask((yyvsp[-9].dval)); 
                defData->PathObj.addVia((yyvsp[-8].string));
                defData->PathObj.addViaRotation((yyvsp[-7].integer));;
                defData->PathObj.addViaData((int)(yyvsp[-5].dval), (int)(yyvsp[-3].dval), (int)(yyvsp[-1].dval), (int)(yyvsp[0].dval));
            } else if (defData->NeedPathData && (defCallbacks->NetCbk && (defData->netOsnet==1))) {
              if (defData->netWarnings++ < defSettings->NetWarnings) {
                defError(6567, "The VIA DO statement is defined in the NET statement and is invalid.\nRemove this statement from your DEF file and try again.");
                CHKERR();
              }
            }
        }
      }
#line 6925 "def.tab.c"
    break;

  case 450: /* $@82: %empty  */
#line 2754 "def.y"
                          { defData->dumb_mode = 6; }
#line 6931 "def.tab.c"
    break;

  case 451: /* path_item: K_MASK NUMBER K_RECT $@82 '(' NUMBER NUMBER NUMBER NUMBER ')'  */
#line 2755 "def.y"
    {
      if (validateMaskInput((int)(yyvsp[-8].dval), defData->sNetWarnings, defSettings->SNetWarnings)) {
        if (defData->NeedPathData && ((defCallbacks->NetCbk && (defData->netOsnet==1)) ||
          (defCallbacks->SNetCbk && (defData->netOsnet==2)))) {
          defData->PathObj.addMask((yyvsp[-8].dval));
          defData->PathObj.addViaRect((yyvsp[-4].dval), (yyvsp[-3].dval), (yyvsp[-2].dval), (yyvsp[-1].dval));
        }
      }
    }
#line 6945 "def.tab.c"
    break;

  case 452: /* $@83: %empty  */
#line 2765 "def.y"
    {
       if (validateMaskInput((int)(yyvsp[0].dval), defData->sNetWarnings, defSettings->SNetWarnings)) {
        if (defData->NeedPathData && ((defCallbacks->NetCbk && (defData->netOsnet==1)) ||
          (defCallbacks->SNetCbk && (defData->netOsnet==2)))) {
          defData->PathObj.addMask((yyvsp[0].dval)); 
        }
       }  
    }
#line 6958 "def.tab.c"
    break;

  case 454: /* path_item: path_pt  */
#line 2775 "def.y"
    {
       // reset defData->dumb_mode to 1 just incase the next token is a via of the path
        // 2/5/2004 - pcr 686781
        defData->dumb_mode = DEF_MAX_INT; defData->by_is_keyword = TRUE; defData->do_is_keyword = TRUE;
        defData->new_is_keyword = TRUE; defData->step_is_keyword = TRUE;
        defData->orient_is_keyword = TRUE;
    }
#line 6970 "def.tab.c"
    break;

  case 455: /* path_pt: '(' NUMBER NUMBER ')'  */
#line 2786 "def.y"
      {
        if (defData->NeedPathData && ((defCallbacks->NetCbk && (defData->netOsnet==1)) ||
          (defCallbacks->SNetCbk && (defData->netOsnet==2))))
          defData->PathObj.addPoint(ROUND((yyvsp[-2].dval)), ROUND((yyvsp[-1].dval))); 
        defData->save_x = (yyvsp[-2].dval);
        defData->save_y = (yyvsp[-1].dval); 
      }
#line 6982 "def.tab.c"
    break;

  case 456: /* path_pt: '(' '*' NUMBER ')'  */
#line 2794 "def.y"
      {
        if (defData->NeedPathData && ((defCallbacks->NetCbk && (defData->netOsnet==1)) ||
          (defCallbacks->SNetCbk && (defData->netOsnet==2))))
          defData->PathObj.addPoint(ROUND(defData->save_x), ROUND((yyvsp[-1].dval))); 
        defData->save_y = (yyvsp[-1].dval);
      }
#line 6993 "def.tab.c"
    break;

  case 457: /* path_pt: '(' NUMBER '*' ')'  */
#line 2801 "def.y"
      {
        if (defData->NeedPathData && ((defCallbacks->NetCbk && (defData->netOsnet==1)) ||
          (defCallbacks->SNetCbk && (defData->netOsnet==2))))
          defData->PathObj.addPoint(ROUND((yyvsp[-2].dval)), ROUND(defData->save_y)); 
        defData->save_x = (yyvsp[-2].dval);
      }
#line 7004 "def.tab.c"
    break;

  case 458: /* path_pt: '(' '*' '*' ')'  */
#line 2808 "def.y"
      {
        if (defData->NeedPathData && ((defCallbacks->NetCbk && (defData->netOsnet==1)) ||
            (defCallbacks->SNetCbk && (defData->netOsnet==2))))
          defData->PathObj.addPoint(ROUND(defData->save_x), ROUND(defData->save_y)); 
      }
#line 7014 "def.tab.c"
    break;

  case 459: /* path_pt: '(' NUMBER NUMBER NUMBER ')'  */
#line 2814 "def.y"
      {
        if (defData->NeedPathData && ((defCallbacks->NetCbk && (defData->netOsnet==1)) ||
            (defCallbacks->SNetCbk && (defData->netOsnet==2))))
          defData->PathObj.addFlushPoint(ROUND((yyvsp[-3].dval)), ROUND((yyvsp[-2].dval)), ROUND((yyvsp[-1].dval))); 
        defData->save_x = (yyvsp[-3].dval);
        defData->save_y = (yyvsp[-2].dval);
      }
#line 7026 "def.tab.c"
    break;

  case 460: /* path_pt: '(' '*' NUMBER NUMBER ')'  */
#line 2822 "def.y"
      {
        if (defData->NeedPathData && ((defCallbacks->NetCbk && (defData->netOsnet==1)) ||
          (defCallbacks->SNetCbk && (defData->netOsnet==2))))
          defData->PathObj.addFlushPoint(ROUND(defData->save_x), ROUND((yyvsp[-2].dval)),
          ROUND((yyvsp[-1].dval))); 
        defData->save_y = (yyvsp[-2].dval);
      }
#line 7038 "def.tab.c"
    break;

  case 461: /* path_pt: '(' NUMBER '*' NUMBER ')'  */
#line 2830 "def.y"
      {
        if (defData->NeedPathData && ((defCallbacks->NetCbk && (defData->netOsnet==1)) ||
          (defCallbacks->SNetCbk && (defData->netOsnet==2))))
          defData->PathObj.addFlushPoint(ROUND((yyvsp[-3].dval)), ROUND(defData->save_y),
          ROUND((yyvsp[-1].dval))); 
        defData->save_x = (yyvsp[-3].dval);
      }
#line 7050 "def.tab.c"
    break;

  case 462: /* path_pt: '(' '*' '*' NUMBER ')'  */
#line 2838 "def.y"
      {
        if (defData->NeedPathData && ((defCallbacks->NetCbk && (defData->netOsnet==1)) ||
          (defCallbacks->SNetCbk && (defData->netOsnet==2))))
          defData->PathObj.addFlushPoint(ROUND(defData->save_x), ROUND(defData->save_y),
          ROUND((yyvsp[-1].dval))); 
      }
#line 7061 "def.tab.c"
    break;

  case 463: /* virtual_pt: '(' NUMBER NUMBER ')'  */
#line 2847 "def.y"
      {
        if (defData->NeedPathData && ((defCallbacks->NetCbk && (defData->netOsnet==1)) ||
          (defCallbacks->SNetCbk && (defData->netOsnet==2))))
          defData->PathObj.addVirtualPoint(ROUND((yyvsp[-2].dval)), ROUND((yyvsp[-1].dval))); 
        defData->save_x = (yyvsp[-2].dval);
        defData->save_y = (yyvsp[-1].dval);
      }
#line 7073 "def.tab.c"
    break;

  case 464: /* virtual_pt: '(' '*' NUMBER ')'  */
#line 2855 "def.y"
      {
        if (defData->NeedPathData && ((defCallbacks->NetCbk && (defData->netOsnet==1)) ||
          (defCallbacks->SNetCbk && (defData->netOsnet==2))))
          defData->PathObj.addVirtualPoint(ROUND(defData->save_x), ROUND((yyvsp[-1].dval))); 
        defData->save_y = (yyvsp[-1].dval);
      }
#line 7084 "def.tab.c"
    break;

  case 465: /* virtual_pt: '(' NUMBER '*' ')'  */
#line 2862 "def.y"
      {
        if (defData->NeedPathData && ((defCallbacks->NetCbk && (defData->netOsnet==1)) ||
          (defCallbacks->SNetCbk && (defData->netOsnet==2))))
          defData->PathObj.addVirtualPoint(ROUND((yyvsp[-2].dval)), ROUND(defData->save_y)); 
        defData->save_x = (yyvsp[-2].dval);
      }
#line 7095 "def.tab.c"
    break;

  case 466: /* rect_pts: '(' NUMBER NUMBER NUMBER NUMBER ')'  */
#line 2871 "def.y"
    {
        if (defData->NeedPathData && ((defCallbacks->NetCbk && (defData->netOsnet==1)) ||
          (defCallbacks->SNetCbk && (defData->netOsnet==2)))) {
          defData->PathObj.addViaRect((yyvsp[-4].dval), (yyvsp[-3].dval), (yyvsp[-2].dval), (yyvsp[-1].dval)); 
        }    
    }
#line 7106 "def.tab.c"
    break;

  case 471: /* opt_taper: K_TAPER  */
#line 2887 "def.y"
      { if (defData->NeedPathData && ((defCallbacks->NetCbk && (defData->netOsnet==1)) ||
          (defCallbacks->SNetCbk && (defData->netOsnet==2))))
          defData->PathObj.setTaper(); }
#line 7114 "def.tab.c"
    break;

  case 472: /* $@84: %empty  */
#line 2890 "def.y"
                  { defData->dumb_mode = 1; }
#line 7120 "def.tab.c"
    break;

  case 473: /* opt_taper: K_TAPERRULE $@84 T_STRING  */
#line 2891 "def.y"
      { if (defData->NeedPathData && ((defCallbacks->NetCbk && (defData->netOsnet==1)) ||
          (defCallbacks->SNetCbk && (defData->netOsnet==2))))
          defData->PathObj.addTaperRule((yyvsp[0].string)); }
#line 7128 "def.tab.c"
    break;

  case 474: /* opt_style: K_STYLE NUMBER  */
#line 2896 "def.y"
      { 
        if (defData->VersionNum < 5.6) {
           if (defData->NeedPathData && (defCallbacks->NetCbk ||
               defCallbacks->SNetCbk)) {
             if (defData->netWarnings++ < defSettings->NetWarnings) {
               defData->defMsg = (char*)defMalloc(1000);
               sprintf (defData->defMsg,
                  "The STYLE statement is available in version 5.6 and later.\nHowever, your DEF file is defined with version %g", defData->VersionNum);
               defError(6534, defData->defMsg);
               defFree(defData->defMsg);
               CHKERR();
             }
           }
        } else
           if (defData->NeedPathData && ((defCallbacks->NetCbk && (defData->netOsnet==1)) ||
             (defCallbacks->SNetCbk && (defData->netOsnet==2))))
             defData->PathObj.addStyle((int)(yyvsp[0].dval));
      }
#line 7151 "def.tab.c"
    break;

  case 477: /* opt_shape_style: '+' K_SHAPE shape_type  */
#line 2921 "def.y"
      { if (defData->NeedPathData && ((defCallbacks->NetCbk && (defData->netOsnet==1)) ||
          (defCallbacks->SNetCbk && (defData->netOsnet==2))))
          defData->PathObj.addShape((yyvsp[0].string)); }
#line 7159 "def.tab.c"
    break;

  case 478: /* opt_shape_style: '+' K_STYLE NUMBER  */
#line 2925 "def.y"
      { if (defData->VersionNum < 5.6) {
          if (defData->NeedPathData && ((defCallbacks->NetCbk && (defData->netOsnet==1)) ||
            (defCallbacks->SNetCbk && (defData->netOsnet==2)))) {
            if (defData->netWarnings++ < defSettings->NetWarnings) {
              defData->defMsg = (char*)defMalloc(1000);
              sprintf (defData->defMsg,
                 "The STYLE statement is available in version 5.6 and later.\nHowever, your DEF file is defined with version %g", defData->VersionNum);
              defError(6534, defData->defMsg);
              defFree(defData->defMsg);
              CHKERR();
            }
          }
        } else {
          if (defData->NeedPathData && ((defCallbacks->NetCbk && (defData->netOsnet==1)) ||
            (defCallbacks->SNetCbk && (defData->netOsnet==2))))
            defData->PathObj.addStyle((int)(yyvsp[0].dval));
        }
      }
#line 7182 "def.tab.c"
    break;

  case 479: /* end_nets: K_END K_NETS  */
#line 2945 "def.y"
          { 
            CALLBACK(defCallbacks->NetEndCbk, defrNetEndCbkType, 0);
            defData->netOsnet = 0;
          }
#line 7191 "def.tab.c"
    break;

  case 480: /* shape_type: K_RING  */
#line 2951 "def.y"
            { (yyval.string) = (char*)"RING"; }
#line 7197 "def.tab.c"
    break;

  case 481: /* shape_type: K_STRIPE  */
#line 2953 "def.y"
            { (yyval.string) = (char*)"STRIPE"; }
#line 7203 "def.tab.c"
    break;

  case 482: /* shape_type: K_FOLLOWPIN  */
#line 2955 "def.y"
            { (yyval.string) = (char*)"FOLLOWPIN"; }
#line 7209 "def.tab.c"
    break;

  case 483: /* shape_type: K_IOWIRE  */
#line 2957 "def.y"
            { (yyval.string) = (char*)"IOWIRE"; }
#line 7215 "def.tab.c"
    break;

  case 484: /* shape_type: K_COREWIRE  */
#line 2959 "def.y"
            { (yyval.string) = (char*)"COREWIRE"; }
#line 7221 "def.tab.c"
    break;

  case 485: /* shape_type: K_BLOCKWIRE  */
#line 2961 "def.y"
            { (yyval.string) = (char*)"BLOCKWIRE"; }
#line 7227 "def.tab.c"
    break;

  case 486: /* shape_type: K_FILLWIRE  */
#line 2963 "def.y"
            { (yyval.string) = (char*)"FILLWIRE"; }
#line 7233 "def.tab.c"
    break;

  case 487: /* shape_type: K_FILLWIREOPC  */
#line 2965 "def.y"
            {
              if (defData->VersionNum < 5.7) {
                 if (defData->NeedPathData) {
                   if (defData->fillWarnings++ < defSettings->FillWarnings) {
                     defData->defMsg = (char*)defMalloc(10000);
                     sprintf (defData->defMsg,
                       "The FILLWIREOPC is available in version 5.7 or later.\nHowever, your DEF file is defined with version %g.", defData->VersionNum);
                     defError(6552, defData->defMsg);
                     defFree(defData->defMsg);
                     CHKERR();
                  }
                }
              }
              (yyval.string) = (char*)"FILLWIREOPC";
            }
#line 7253 "def.tab.c"
    break;

  case 488: /* shape_type: K_DRCFILL  */
#line 2981 "def.y"
            { (yyval.string) = (char*)"DRCFILL"; }
#line 7259 "def.tab.c"
    break;

  case 489: /* shape_type: K_BLOCKAGEWIRE  */
#line 2983 "def.y"
            { (yyval.string) = (char*)"BLOCKAGEWIRE"; }
#line 7265 "def.tab.c"
    break;

  case 490: /* shape_type: K_PADRING  */
#line 2985 "def.y"
            { (yyval.string) = (char*)"PADRING"; }
#line 7271 "def.tab.c"
    break;

  case 491: /* shape_type: K_BLOCKRING  */
#line 2987 "def.y"
            { (yyval.string) = (char*)"BLOCKRING"; }
#line 7277 "def.tab.c"
    break;

  case 495: /* snet_rule: net_and_connections snet_options ';'  */
#line 2997 "def.y"
        { CALLBACK(defCallbacks->SNetCbk, defrSNetCbkType, &defData->Net); }
#line 7283 "def.tab.c"
    break;

  case 502: /* snet_other_option: '+' net_type  */
#line 3008 "def.y"
            {
             if (defData->VersionNum >= 5.8) {
                defData->specialWire_routeStatus = (yyvsp[0].string);
             } else {
                 if (defCallbacks->SNetCbk) {   // PCR 902306 
                   defData->defMsg = (char*)defMalloc(1024);
                   sprintf(defData->defMsg, "The SPECIAL NET statement, with type %s, does not have any net statement defined.\nThe DEF parser will ignore this statemnet.", (yyvsp[0].string));
                   defWarning(7023, defData->defMsg);
                   defFree(defData->defMsg);
                 }
             }
            }
#line 7300 "def.tab.c"
    break;

  case 503: /* $@85: %empty  */
#line 3021 "def.y"
            {
            if (defCallbacks->SNetCbk) defData->Net.addWire((yyvsp[0].string), NULL);
            }
#line 7308 "def.tab.c"
    break;

  case 504: /* snet_other_option: '+' net_type $@85 spaths  */
#line 3025 "def.y"
            {
            // 7/17/2003 - Fix for pcr 604848, add a callback for each wire
            if (defCallbacks->SNetWireCbk) {
               CALLBACK(defCallbacks->SNetWireCbk, defrSNetWireCbkType, &defData->Net);
               defData->Net.freeWire();
            }
            defData->by_is_keyword = FALSE;
            defData->do_is_keyword = FALSE;
            defData->new_is_keyword = FALSE;
            defData->step_is_keyword = FALSE;
            defData->orient_is_keyword = FALSE;
            defData->virtual_is_keyword = FALSE;
            defData->mask_is_keyword = FALSE;
            defData->rect_is_keyword = FALSE;
            defData->needSNPCbk = 0;
            }
#line 7329 "def.tab.c"
    break;

  case 505: /* $@86: %empty  */
#line 3042 "def.y"
                       { defData->dumb_mode = 1; defData->no_num = 1; }
#line 7335 "def.tab.c"
    break;

  case 506: /* $@87: %empty  */
#line 3043 "def.y"
            { defData->shieldName = (yyvsp[0].string); 
              defData->specialWire_routeStatus = (char*)"SHIELD";
              defData->specialWire_routeStatusName = (yyvsp[0].string); 
            }
#line 7344 "def.tab.c"
    break;

  case 508: /* snet_other_option: '+' K_SHAPE shape_type  */
#line 3050 "def.y"
          {  
            defData->specialWire_shapeType = (yyvsp[0].string);
          }
#line 7352 "def.tab.c"
    break;

  case 509: /* snet_other_option: '+' K_MASK NUMBER  */
#line 3054 "def.y"
          {
            if (validateMaskInput((int)(yyvsp[0].dval), defData->sNetWarnings, defSettings->SNetWarnings)) {
                defData->specialWire_mask = (yyvsp[0].dval);
            }     
          }
#line 7362 "def.tab.c"
    break;

  case 510: /* $@88: %empty  */
#line 3059 "def.y"
                        { defData->dumb_mode = 1; }
#line 7368 "def.tab.c"
    break;

  case 511: /* $@89: %empty  */
#line 3060 "def.y"
          {
            if (defData->VersionNum < 5.6) {
              if (defCallbacks->SNetCbk) {
                if (defData->sNetWarnings++ < defSettings->SNetWarnings) {
                  defData->defMsg = (char*)defMalloc(1000);
                  sprintf (defData->defMsg,
                     "The POLYGON statement is available in version 5.6 and later.\nHowever, your DEF file is defined with version %g", defData->VersionNum);
                  defError(6535, defData->defMsg);
                  defFree(defData->defMsg);
                  CHKERR();
                }
              }
            }
            
            defData->Geometries.Reset();
          }
#line 7389 "def.tab.c"
    break;

  case 512: /* snet_other_option: '+' K_POLYGON $@88 T_STRING $@89 firstPt nextPt nextPt otherPts  */
#line 3077 "def.y"
          {
            if (defData->VersionNum >= 5.6) {  // only add if 5.6 or beyond
              if (defCallbacks->SNetCbk) {
                // defData->needSNPCbk will indicate that it has reach the max
                // memory and if user has set partialPathCBk, def parser
                // will make the callback.
                // This will improve performance
                // This construct is only in specialnet
                defData->Net.addPolygon((yyvsp[-5].string), &defData->Geometries, &defData->needSNPCbk, defData->specialWire_mask, defData->specialWire_routeStatus, defData->specialWire_shapeType,
                                                                defData->specialWire_routeStatusName);
                defData->specialWire_mask = 0;
                defData->specialWire_routeStatus = (char*)"ROUTED";
                defData->specialWire_shapeType = (char*)"";
                if (defData->needSNPCbk && defCallbacks->SNetPartialPathCbk) {
                   CALLBACK(defCallbacks->SNetPartialPathCbk, defrSNetPartialPathCbkType,
                            &defData->Net);
                   defData->Net.clearRectPolyNPath();
                   defData->Net.clearVia();
                }
              }
            }
          }
#line 7416 "def.tab.c"
    break;

  case 513: /* $@90: %empty  */
#line 3100 "def.y"
                     { defData->dumb_mode = 1; }
#line 7422 "def.tab.c"
    break;

  case 514: /* snet_other_option: '+' K_RECT $@90 T_STRING pt pt  */
#line 3101 "def.y"
          {
            if (defData->VersionNum < 5.6) {
              if (defCallbacks->SNetCbk) {
                if (defData->sNetWarnings++ < defSettings->SNetWarnings) {
                  defData->defMsg = (char*)defMalloc(1000);
                  sprintf (defData->defMsg,
                     "The RECT statement is available in version 5.6 and later.\nHowever, your DEF file is defined with version %g", defData->VersionNum);
                  defError(6536, defData->defMsg);
                  defFree(defData->defMsg);
                  CHKERR();
                }
              }
            }
            if (defCallbacks->SNetCbk) {
              // defData->needSNPCbk will indicate that it has reach the max
              // memory and if user has set partialPathCBk, def parser
              // will make the callback.
              // This will improve performance
              // This construct is only in specialnet
              defData->Net.addRect((yyvsp[-2].string), (yyvsp[-1].pt).x, (yyvsp[-1].pt).y, (yyvsp[0].pt).x, (yyvsp[0].pt).y, &defData->needSNPCbk, defData->specialWire_mask, defData->specialWire_routeStatus, defData->specialWire_shapeType, defData->specialWire_routeStatusName);
              defData->specialWire_mask = 0;
              defData->specialWire_routeStatus = (char*)"ROUTED";
              defData->specialWire_shapeType = (char*)"";
              defData->specialWire_routeStatusName = (char*)"";
              if (defData->needSNPCbk && defCallbacks->SNetPartialPathCbk) {
                 CALLBACK(defCallbacks->SNetPartialPathCbk, defrSNetPartialPathCbkType,
                          &defData->Net);
                 defData->Net.clearRectPolyNPath();
                 defData->Net.clearVia();
              }
            }
          }
#line 7459 "def.tab.c"
    break;

  case 515: /* $@91: %empty  */
#line 3133 "def.y"
                    { defData->dumb_mode = 1; }
#line 7465 "def.tab.c"
    break;

  case 516: /* $@92: %empty  */
#line 3134 "def.y"
        {
          if (defData->VersionNum < 5.8) {
              if (defCallbacks->SNetCbk) {
                if (defData->sNetWarnings++ < defSettings->SNetWarnings) {
                  defData->defMsg = (char*)defMalloc(1000);
                  sprintf (defData->defMsg,
                     "The VIA statement is available in version 5.8 and later.\nHowever, your DEF file is defined with version %g", defData->VersionNum);
                  defError(6536, defData->defMsg);
                  defFree(defData->defMsg);
                  CHKERR();
                }
              }
          }
        }
#line 7484 "def.tab.c"
    break;

  case 517: /* snet_other_option: '+' K_VIA $@91 T_STRING orient_pt $@92 firstPt otherPts  */
#line 3149 "def.y"
        {
          if (defData->VersionNum >= 5.8 && defCallbacks->SNetCbk) {
              defData->Net.addPts((yyvsp[-4].string), (yyvsp[-3].integer), &defData->Geometries, &defData->needSNPCbk, defData->specialWire_mask, defData->specialWire_routeStatus, defData->specialWire_shapeType,
                                                          defData->specialWire_routeStatusName);
              defData->specialWire_mask = 0;
              defData->specialWire_routeStatus = (char*)"ROUTED";
              defData->specialWire_shapeType = (char*)"";
              defData->specialWire_routeStatusName = (char*)"";
              if (defData->needSNPCbk && defCallbacks->SNetPartialPathCbk) {
                 CALLBACK(defCallbacks->SNetPartialPathCbk, defrSNetPartialPathCbkType,
                          &defData->Net);
                 defData->Net.clearRectPolyNPath();
                 defData->Net.clearVia();
              }
            }
        }
#line 7505 "def.tab.c"
    break;

  case 518: /* snet_other_option: '+' K_SOURCE source_type  */
#line 3167 "def.y"
            { if (defCallbacks->SNetCbk) defData->Net.setSource((yyvsp[0].string)); }
#line 7511 "def.tab.c"
    break;

  case 519: /* snet_other_option: '+' K_FIXEDBUMP  */
#line 3170 "def.y"
            { if (defCallbacks->SNetCbk) defData->Net.setFixedbump(); }
#line 7517 "def.tab.c"
    break;

  case 520: /* snet_other_option: '+' K_FREQUENCY NUMBER  */
#line 3173 "def.y"
            { if (defCallbacks->SNetCbk) defData->Net.setFrequency((yyvsp[0].dval)); }
#line 7523 "def.tab.c"
    break;

  case 521: /* $@93: %empty  */
#line 3175 "def.y"
                         {defData->dumb_mode = 1; defData->no_num = 1;}
#line 7529 "def.tab.c"
    break;

  case 522: /* snet_other_option: '+' K_ORIGINAL $@93 T_STRING  */
#line 3176 "def.y"
            { if (defCallbacks->SNetCbk) defData->Net.setOriginal((yyvsp[0].string)); }
#line 7535 "def.tab.c"
    break;

  case 523: /* snet_other_option: '+' K_PATTERN pattern_type  */
#line 3179 "def.y"
            { if (defCallbacks->SNetCbk) defData->Net.setPattern((yyvsp[0].string)); }
#line 7541 "def.tab.c"
    break;

  case 524: /* snet_other_option: '+' K_WEIGHT NUMBER  */
#line 3182 "def.y"
            { if (defCallbacks->SNetCbk) defData->Net.setWeight(ROUND((yyvsp[0].dval))); }
#line 7547 "def.tab.c"
    break;

  case 525: /* snet_other_option: '+' K_ESTCAP NUMBER  */
#line 3185 "def.y"
            { 
              // 11/12/2002 - this is obsolete in 5.5, & will be ignored 
              if (defData->VersionNum < 5.5)
                 if (defCallbacks->SNetCbk) defData->Net.setCap((yyvsp[0].dval));
              else
                 defWarning(7024, "The ESTCAP statement is obsolete in version 5.5 and later.\nThe DEF parser will ignore this statement.");
            }
#line 7559 "def.tab.c"
    break;

  case 526: /* snet_other_option: '+' K_USE use_type  */
#line 3194 "def.y"
            { if (defCallbacks->SNetCbk) defData->Net.setUse((yyvsp[0].string)); }
#line 7565 "def.tab.c"
    break;

  case 527: /* snet_other_option: '+' K_STYLE NUMBER  */
#line 3197 "def.y"
            { if (defCallbacks->SNetCbk) defData->Net.setStyle((int)(yyvsp[0].dval)); }
#line 7571 "def.tab.c"
    break;

  case 528: /* $@94: %empty  */
#line 3199 "def.y"
                         {defData->dumb_mode = DEF_MAX_INT; defData->parsing_property = 1; }
#line 7577 "def.tab.c"
    break;

  case 529: /* snet_other_option: '+' K_PROPERTY $@94 snet_prop_list  */
#line 3201 "def.y"
            { defData->dumb_mode = 0; defData->parsing_property = 0; }
#line 7583 "def.tab.c"
    break;

  case 530: /* snet_other_option: extension_stmt  */
#line 3204 "def.y"
          { CALLBACK(defCallbacks->NetExtCbk, defrNetExtCbkType, &defData->History_text[0]); }
#line 7589 "def.tab.c"
    break;

  case 531: /* orient_pt: %empty  */
#line 3207 "def.y"
        { (yyval.integer) = 0; }
#line 7595 "def.tab.c"
    break;

  case 532: /* orient_pt: K_N  */
#line 3208 "def.y"
               {(yyval.integer) = 0;}
#line 7601 "def.tab.c"
    break;

  case 533: /* orient_pt: K_W  */
#line 3209 "def.y"
               {(yyval.integer) = 1;}
#line 7607 "def.tab.c"
    break;

  case 534: /* orient_pt: K_S  */
#line 3210 "def.y"
               {(yyval.integer) = 2;}
#line 7613 "def.tab.c"
    break;

  case 535: /* orient_pt: K_E  */
#line 3211 "def.y"
               {(yyval.integer) = 3;}
#line 7619 "def.tab.c"
    break;

  case 536: /* orient_pt: K_FN  */
#line 3212 "def.y"
               {(yyval.integer) = 4;}
#line 7625 "def.tab.c"
    break;

  case 537: /* orient_pt: K_FW  */
#line 3213 "def.y"
               {(yyval.integer) = 5;}
#line 7631 "def.tab.c"
    break;

  case 538: /* orient_pt: K_FS  */
#line 3214 "def.y"
               {(yyval.integer) = 6;}
#line 7637 "def.tab.c"
    break;

  case 539: /* orient_pt: K_FE  */
#line 3215 "def.y"
               {(yyval.integer) = 7;}
#line 7643 "def.tab.c"
    break;

  case 540: /* shield_layer: %empty  */
#line 3218 "def.y"
            {
                if (defCallbacks->SNetCbk) {
                    if (defData->VersionNum < 5.8) { 
                        defData->defMsg = (char*)defMalloc(1024);
                        sprintf(defData->defMsg, "The SPECIAL NET SHIELD statement doesn't have routing points definition.\nWill be ignored.");
                        defWarning(7025, defData->defMsg);
                        defFree(defData->defMsg);
                    } else {  // CCR 1244433
                      defData->specialWire_routeStatus = (char*)"SHIELD";
                      defData->specialWire_routeStatusName = defData->shieldName;
                    }
                }
            }
#line 7661 "def.tab.c"
    break;

  case 541: /* $@95: %empty  */
#line 3232 "def.y"
            { // since the parser still supports 5.3 and earlier, 
              // can't just move SHIELD in net_type 
              if (defData->VersionNum < 5.4) { // PCR 445209 
                if (defCallbacks->SNetCbk) defData->Net.addShield(defData->shieldName);
                defData->by_is_keyword = FALSE;
                defData->do_is_keyword = FALSE;
                defData->new_is_keyword = FALSE;
                defData->step_is_keyword = FALSE;
                defData->orient_is_keyword = FALSE;
                defData->virtual_is_keyword = FALSE;
                defData->mask_is_keyword = FALSE;
                defData->rect_is_keyword = FALSE;
                defData->specialWire_routeStatus = (char*)"ROUTED";
                defData->specialWire_routeStatusName = (char*)"";
                defData->shield = TRUE;   // save the path info in the defData->shield paths 
              } else
                if (defCallbacks->SNetCbk) defData->Net.addWire("SHIELD", defData->shieldName);
                defData->specialWire_routeStatus = (char*)"ROUTED";
                defData->specialWire_routeStatusName = (char*)"";
            }
#line 7686 "def.tab.c"
    break;

  case 542: /* shield_layer: $@95 spaths  */
#line 3253 "def.y"
            {
              // 7/17/2003 - Fix for pcr 604848, add a callback for each wire
              if (defCallbacks->SNetWireCbk) {
                 CALLBACK(defCallbacks->SNetWireCbk, defrSNetWireCbkType, &defData->Net);
                 if (defData->VersionNum < 5.4)
                   defData->Net.freeShield();
                 else
                   defData->Net.freeWire();
              }
              if (defData->VersionNum < 5.4) {  // PCR 445209 
                defData->shield = FALSE;
                defData->by_is_keyword = FALSE;
                defData->do_is_keyword = FALSE;
                defData->new_is_keyword = FALSE;
                defData->step_is_keyword = FALSE;
                defData->nondef_is_keyword = FALSE;
                defData->mustjoin_is_keyword = FALSE;
                defData->orient_is_keyword = FALSE;
                defData->virtual_is_keyword = FALSE;
                defData->mask_is_keyword = FALSE;
                defData->rect_is_keyword = FALSE;
              } else {
                defData->by_is_keyword = FALSE;
                defData->do_is_keyword = FALSE;
                defData->new_is_keyword = FALSE;
                defData->step_is_keyword = FALSE;
                defData->orient_is_keyword = FALSE;
                defData->virtual_is_keyword = FALSE;
                defData->mask_is_keyword = FALSE;
                defData->rect_is_keyword = FALSE;
              }
              defData->needSNPCbk = 0;
            }
#line 7724 "def.tab.c"
    break;

  case 543: /* $@96: %empty  */
#line 3287 "def.y"
                        { defData->dumb_mode = 1; }
#line 7730 "def.tab.c"
    break;

  case 544: /* snet_width: '+' K_WIDTH $@96 T_STRING NUMBER  */
#line 3288 "def.y"
            {
              // 11/12/2002 - this is obsolete in 5.5, & will be ignored 
              if (defData->VersionNum < 5.5)
                 if (defCallbacks->SNetCbk) defData->Net.setWidth((yyvsp[-1].string), (yyvsp[0].dval));
              else
                 defWarning(7026, "The WIDTH statement is obsolete in version 5.5 and later.\nThe DEF parser will ignore this statement.");
            }
#line 7742 "def.tab.c"
    break;

  case 545: /* $@97: %empty  */
#line 3296 "def.y"
                             { defData->dumb_mode = 1; defData->no_num = 1; }
#line 7748 "def.tab.c"
    break;

  case 546: /* snet_voltage: '+' K_VOLTAGE $@97 T_STRING  */
#line 3297 "def.y"
            {
              if (numIsInt((yyvsp[0].string))) {
                 if (defCallbacks->SNetCbk) defData->Net.setVoltage(atoi((yyvsp[0].string)));
              } else {
                 if (defCallbacks->SNetCbk) {
                   if (defData->sNetWarnings++ < defSettings->SNetWarnings) {
                     defData->defMsg = (char*)defMalloc(1000);
                     sprintf (defData->defMsg,
                        "The value %s for statement VOLTAGE is invalid. The value can only be integer.\nSpecify a valid value in units of millivolts", (yyvsp[0].string));
                     defError(6537, defData->defMsg);
                     defFree(defData->defMsg);
                     CHKERR();
                   }
                 }
              }
            }
#line 7769 "def.tab.c"
    break;

  case 547: /* $@98: %empty  */
#line 3314 "def.y"
                            { defData->dumb_mode = 1; }
#line 7775 "def.tab.c"
    break;

  case 548: /* $@99: %empty  */
#line 3315 "def.y"
            {
              if (defCallbacks->SNetCbk) defData->Net.setSpacing((yyvsp[-1].string),(yyvsp[0].dval));
            }
#line 7783 "def.tab.c"
    break;

  case 549: /* snet_spacing: '+' K_SPACING $@98 T_STRING NUMBER $@99 opt_snet_range  */
#line 3319 "def.y"
            {
            }
#line 7790 "def.tab.c"
    break;

  case 552: /* snet_prop: T_STRING NUMBER  */
#line 3327 "def.y"
            {
              if (defCallbacks->SNetCbk) {
                char propTp;
                char* str = ringCopy("                       ");
                propTp = defSettings->SNetProp.propType((yyvsp[-1].string));
                CHKPROPTYPE(propTp, (yyvsp[-1].string), "SPECIAL NET");
                // For backword compatibility, also set the string value 
                sprintf(str, "%g", (yyvsp[0].dval));
                defData->Net.addNumProp((yyvsp[-1].string), (yyvsp[0].dval), str, propTp);
              }
            }
#line 7806 "def.tab.c"
    break;

  case 553: /* snet_prop: T_STRING QSTRING  */
#line 3339 "def.y"
            {
              if (defCallbacks->SNetCbk) {
                char propTp;
                propTp = defSettings->SNetProp.propType((yyvsp[-1].string));
                CHKPROPTYPE(propTp, (yyvsp[-1].string), "SPECIAL NET");
                defData->Net.addProp((yyvsp[-1].string), (yyvsp[0].string), propTp);
              }
            }
#line 7819 "def.tab.c"
    break;

  case 554: /* snet_prop: T_STRING T_STRING  */
#line 3348 "def.y"
            {
              if (defCallbacks->SNetCbk) {
                char propTp;
                propTp = defSettings->SNetProp.propType((yyvsp[-1].string));
                CHKPROPTYPE(propTp, (yyvsp[-1].string), "SPECIAL NET");
                defData->Net.addProp((yyvsp[-1].string), (yyvsp[0].string), propTp);
              }
            }
#line 7832 "def.tab.c"
    break;

  case 556: /* opt_snet_range: K_RANGE NUMBER NUMBER  */
#line 3359 "def.y"
            {
              if (defCallbacks->SNetCbk) defData->Net.setRange((yyvsp[-1].dval),(yyvsp[0].dval));
            }
#line 7840 "def.tab.c"
    break;

  case 558: /* opt_range: K_RANGE NUMBER NUMBER  */
#line 3365 "def.y"
            { defData->Prop.setRange((yyvsp[-1].dval), (yyvsp[0].dval)); }
#line 7846 "def.tab.c"
    break;

  case 559: /* pattern_type: K_BALANCED  */
#line 3368 "def.y"
            { (yyval.string) = (char*)"BALANCED"; }
#line 7852 "def.tab.c"
    break;

  case 560: /* pattern_type: K_STEINER  */
#line 3370 "def.y"
            { (yyval.string) = (char*)"STEINER"; }
#line 7858 "def.tab.c"
    break;

  case 561: /* pattern_type: K_TRUNK  */
#line 3372 "def.y"
            { (yyval.string) = (char*)"TRUNK"; }
#line 7864 "def.tab.c"
    break;

  case 562: /* pattern_type: K_WIREDLOGIC  */
#line 3374 "def.y"
            { (yyval.string) = (char*)"WIREDLOGIC"; }
#line 7870 "def.tab.c"
    break;

  case 563: /* spaths: spath  */
#line 3378 "def.y"
      { 
        if (defData->NeedPathData && defCallbacks->SNetCbk) {
           if (defData->needSNPCbk && defCallbacks->SNetPartialPathCbk) { 
              // require a callback before proceed because defData->needSNPCbk must be
              // set to 1 from the previous pathIsDone and user has registered
              // a callback routine.
              CALLBACK(defCallbacks->SNetPartialPathCbk, defrSNetPartialPathCbkType,
                       &defData->Net);
              defData->needSNPCbk = 0;   // reset the flag
              pathIsDone(defData->shield, 1, defData->netOsnet, &defData->needSNPCbk);
              defData->Net.clearRectPolyNPath();
              defData->Net.clearVia();
           } else
              pathIsDone(defData->shield, 0, defData->netOsnet, &defData->needSNPCbk);
        }
      }
#line 7891 "def.tab.c"
    break;

  case 564: /* spaths: spaths snew_path  */
#line 3395 "def.y"
      { }
#line 7897 "def.tab.c"
    break;

  case 565: /* $@100: %empty  */
#line 3397 "def.y"
                 { defData->dumb_mode = 1; }
#line 7903 "def.tab.c"
    break;

  case 566: /* snew_path: K_NEW $@100 spath  */
#line 3398 "def.y"
      { if (defData->NeedPathData && defCallbacks->SNetCbk) {
           if (defData->needSNPCbk && defCallbacks->SNetPartialPathCbk) {
              // require a callback before proceed because defData->needSNPCbk must be
              // set to 1 from the previous pathIsDone and user has registered
              // a callback routine.
              CALLBACK(defCallbacks->SNetPartialPathCbk, defrSNetPartialPathCbkType,
                       &defData->Net);
              defData->needSNPCbk = 0;   // reset the flag
              pathIsDone(defData->shield, 1, defData->netOsnet, &defData->needSNPCbk);
              // reset any poly or rect in special wiring statement
              defData->Net.clearRectPolyNPath();
              defData->Net.clearVia();
           } else
              pathIsDone(defData->shield, 0, defData->netOsnet, &defData->needSNPCbk);
        }
      }
#line 7924 "def.tab.c"
    break;

  case 567: /* $@101: %empty  */
#line 3416 "def.y"
      { if (defData->NeedPathData && defCallbacks->SNetCbk)
           defData->PathObj.addLayer((yyvsp[0].string));
        defData->dumb_mode = 0; defData->no_num = 0;
      }
#line 7933 "def.tab.c"
    break;

  case 568: /* $@102: %empty  */
#line 3424 "def.y"
      { defData->dumb_mode = DEF_MAX_INT; defData->by_is_keyword = TRUE; defData->do_is_keyword = TRUE;
        defData->new_is_keyword = TRUE; defData->step_is_keyword = TRUE;
         defData->orient_is_keyword = TRUE; defData->rect_is_keyword = TRUE, defData->mask_is_keyword = TRUE; 
         defData->virtual_is_keyword = TRUE;  }
#line 7942 "def.tab.c"
    break;

  case 569: /* spath: T_STRING $@101 width opt_spaths path_pt $@102 path_item_list  */
#line 3430 "def.y"
      { defData->dumb_mode = 0; defData->rect_is_keyword = FALSE, defData->mask_is_keyword = FALSE, defData->virtual_is_keyword = FALSE; }
#line 7948 "def.tab.c"
    break;

  case 570: /* width: NUMBER  */
#line 3433 "def.y"
      { if (defData->NeedPathData && defCallbacks->SNetCbk)
          defData->PathObj.addWidth(ROUND((yyvsp[0].dval)));
      }
#line 7956 "def.tab.c"
    break;

  case 571: /* start_snets: K_SNETS NUMBER ';'  */
#line 3438 "def.y"
      { 
        if (defCallbacks->SNetStartCbk)
          CALLBACK(defCallbacks->SNetStartCbk, defrSNetStartCbkType, ROUND((yyvsp[-1].dval)));
        defData->netOsnet = 2;
      }
#line 7966 "def.tab.c"
    break;

  case 572: /* end_snets: K_END K_SNETS  */
#line 3445 "def.y"
      { 
        if (defCallbacks->SNetEndCbk)
          CALLBACK(defCallbacks->SNetEndCbk, defrSNetEndCbkType, 0);
        defData->netOsnet = 0;
      }
#line 7976 "def.tab.c"
    break;

  case 574: /* groups_start: K_GROUPS NUMBER ';'  */
#line 3455 "def.y"
      {
        if (defCallbacks->GroupsStartCbk)
           CALLBACK(defCallbacks->GroupsStartCbk, defrGroupsStartCbkType, ROUND((yyvsp[-1].dval)));
      }
#line 7985 "def.tab.c"
    break;

  case 577: /* group_rule: start_group group_members group_options ';'  */
#line 3465 "def.y"
      {
        if (defCallbacks->GroupCbk)
           CALLBACK(defCallbacks->GroupCbk, defrGroupCbkType, &defData->Group);
      }
#line 7994 "def.tab.c"
    break;

  case 578: /* $@103: %empty  */
#line 3470 "def.y"
                 { defData->dumb_mode = 1; defData->no_num = 1; }
#line 8000 "def.tab.c"
    break;

  case 579: /* start_group: '-' $@103 T_STRING  */
#line 3471 "def.y"
      {
        defData->dumb_mode = DEF_MAX_INT;
        defData->no_num = DEF_MAX_INT;
        /* dumb_mode is automatically turned off at the first
         * + in the options or at the ; at the end of the group */
        if (defCallbacks->GroupCbk) defData->Group.setup((yyvsp[0].string));
        if (defCallbacks->GroupNameCbk)
           CALLBACK(defCallbacks->GroupNameCbk, defrGroupNameCbkType, (yyvsp[0].string));
      }
#line 8014 "def.tab.c"
    break;

  case 581: /* group_members: group_members group_member  */
#line 3483 "def.y"
      {  }
#line 8020 "def.tab.c"
    break;

  case 582: /* group_member: T_STRING  */
#line 3486 "def.y"
      {
        // if (defCallbacks->GroupCbk) defData->Group.addMember($1); 
        if (defCallbacks->GroupMemberCbk)
          CALLBACK(defCallbacks->GroupMemberCbk, defrGroupMemberCbkType, (yyvsp[0].string));
      }
#line 8030 "def.tab.c"
    break;

  case 585: /* group_option: '+' K_SOFT group_soft_options  */
#line 3497 "def.y"
      { }
#line 8036 "def.tab.c"
    break;

  case 586: /* $@104: %empty  */
#line 3498 "def.y"
                           { defData->dumb_mode = DEF_MAX_INT; defData->parsing_property = 1; }
#line 8042 "def.tab.c"
    break;

  case 587: /* group_option: '+' K_PROPERTY $@104 group_prop_list  */
#line 3500 "def.y"
      { defData->dumb_mode = 0; defData->parsing_property = 0; }
#line 8048 "def.tab.c"
    break;

  case 588: /* $@105: %empty  */
#line 3501 "def.y"
                         { defData->dumb_mode = 1;  defData->no_num = 1; }
#line 8054 "def.tab.c"
    break;

  case 589: /* group_option: '+' K_REGION $@105 group_region  */
#line 3502 "def.y"
      { }
#line 8060 "def.tab.c"
    break;

  case 590: /* group_option: extension_stmt  */
#line 3504 "def.y"
      { 
        if (defCallbacks->GroupMemberCbk)
          CALLBACK(defCallbacks->GroupExtCbk, defrGroupExtCbkType, &defData->History_text[0]);
      }
#line 8069 "def.tab.c"
    break;

  case 591: /* group_region: pt pt  */
#line 3510 "def.y"
      {
        // 11/12/2002 - this is obsolete in 5.5, & will be ignored 
        if (defData->VersionNum < 5.5) {
          if (defCallbacks->GroupCbk)
            defData->Group.addRegionRect((yyvsp[-1].pt).x, (yyvsp[-1].pt).y, (yyvsp[0].pt).x, (yyvsp[0].pt).y);
        }
        else
          defWarning(7027, "The GROUP REGION pt pt statement is obsolete in version 5.5 and later.\nThe DEF parser will ignore this statement.");
      }
#line 8083 "def.tab.c"
    break;

  case 592: /* group_region: T_STRING  */
#line 3520 "def.y"
      { if (defCallbacks->GroupCbk)
          defData->Group.setRegionName((yyvsp[0].string));
      }
#line 8091 "def.tab.c"
    break;

  case 595: /* group_prop: T_STRING NUMBER  */
#line 3529 "def.y"
      {
        if (defCallbacks->GroupCbk) {
          char propTp;
          char* str = ringCopy("                       ");
          propTp = defSettings->GroupProp.propType((yyvsp[-1].string));
          CHKPROPTYPE(propTp, (yyvsp[-1].string), "GROUP");
          sprintf(str, "%g", (yyvsp[0].dval));
          defData->Group.addNumProperty((yyvsp[-1].string), (yyvsp[0].dval), str, propTp);
        }
      }
#line 8106 "def.tab.c"
    break;

  case 596: /* group_prop: T_STRING QSTRING  */
#line 3540 "def.y"
      {
        if (defCallbacks->GroupCbk) {
          char propTp;
          propTp = defSettings->GroupProp.propType((yyvsp[-1].string));
          CHKPROPTYPE(propTp, (yyvsp[-1].string), "GROUP");
          defData->Group.addProperty((yyvsp[-1].string), (yyvsp[0].string), propTp);
        }
      }
#line 8119 "def.tab.c"
    break;

  case 597: /* group_prop: T_STRING T_STRING  */
#line 3549 "def.y"
      {
        if (defCallbacks->GroupCbk) {
          char propTp;
          propTp = defSettings->GroupProp.propType((yyvsp[-1].string));
          CHKPROPTYPE(propTp, (yyvsp[-1].string), "GROUP");
          defData->Group.addProperty((yyvsp[-1].string), (yyvsp[0].string), propTp);
        }
      }
#line 8132 "def.tab.c"
    break;

  case 599: /* group_soft_options: group_soft_options group_soft_option  */
#line 3560 "def.y"
      { }
#line 8138 "def.tab.c"
    break;

  case 600: /* group_soft_option: K_MAXX NUMBER  */
#line 3563 "def.y"
      {
        // 11/12/2002 - this is obsolete in 5.5, & will be ignored 
        if (defData->VersionNum < 5.5)
          if (defCallbacks->GroupCbk) defData->Group.setMaxX(ROUND((yyvsp[0].dval)));
        else
          defWarning(7028, "The GROUP SOFT MAXX statement is obsolete in version 5.5 and later.\nThe DEF parser will ignore this statement.");
      }
#line 8150 "def.tab.c"
    break;

  case 601: /* group_soft_option: K_MAXY NUMBER  */
#line 3571 "def.y"
      { 
        // 11/12/2002 - this is obsolete in 5.5, & will be ignored 
        if (defData->VersionNum < 5.5)
          if (defCallbacks->GroupCbk) defData->Group.setMaxY(ROUND((yyvsp[0].dval)));
        else
          defWarning(7029, "The GROUP SOFT MAXY statement is obsolete in version 5.5 and later.\nThe DEF parser will ignore this statement.");
      }
#line 8162 "def.tab.c"
    break;

  case 602: /* group_soft_option: K_MAXHALFPERIMETER NUMBER  */
#line 3579 "def.y"
      { 
        // 11/12/2002 - this is obsolete in 5.5, & will be ignored 
        if (defData->VersionNum < 5.5)
          if (defCallbacks->GroupCbk) defData->Group.setPerim(ROUND((yyvsp[0].dval)));
        else
          defWarning(7030, "The GROUP SOFT MAXHALFPERIMETER statement is obsolete in version 5.5 and later.\nThe DEF parser will ignore this statement.");
      }
#line 8174 "def.tab.c"
    break;

  case 603: /* groups_end: K_END K_GROUPS  */
#line 3588 "def.y"
      { 
        if (defCallbacks->GroupsEndCbk)
          CALLBACK(defCallbacks->GroupsEndCbk, defrGroupsEndCbkType, 0);
      }
#line 8183 "def.tab.c"
    break;

  case 606: /* assertions_start: K_ASSERTIONS NUMBER ';'  */
#line 3602 "def.y"
      {
        if ((defData->VersionNum < 5.4) && (defCallbacks->AssertionsStartCbk)) {
          CALLBACK(defCallbacks->AssertionsStartCbk, defrAssertionsStartCbkType,
                   ROUND((yyvsp[-1].dval)));
        } else {
          if (defCallbacks->AssertionCbk)
            if (defData->assertionWarnings++ < defSettings->AssertionWarnings)
              defWarning(7031, "The ASSERTIONS statement is obsolete in version 5.4 and later.\nThe DEF parser will ignore this statement.");
        }
        if (defCallbacks->AssertionCbk)
          defData->Assertion.setAssertionMode();
      }
#line 8200 "def.tab.c"
    break;

  case 607: /* constraints_start: K_CONSTRAINTS NUMBER ';'  */
#line 3616 "def.y"
      {
        if ((defData->VersionNum < 5.4) && (defCallbacks->ConstraintsStartCbk)) {
          CALLBACK(defCallbacks->ConstraintsStartCbk, defrConstraintsStartCbkType,
                   ROUND((yyvsp[-1].dval)));
        } else {
          if (defCallbacks->ConstraintCbk)
            if (defData->constraintWarnings++ < defSettings->ConstraintWarnings)
              defWarning(7032, "The CONSTRAINTS statement is obsolete in version 5.4 and later.\nThe DEF parser will ignore this statement.");
        }
        if (defCallbacks->ConstraintCbk)
          defData->Assertion.setConstraintMode();
      }
#line 8217 "def.tab.c"
    break;

  case 611: /* constraint_rule: wiredlogic_rule  */
#line 3635 "def.y"
      {
        if ((defData->VersionNum < 5.4) && (defCallbacks->ConstraintCbk || defCallbacks->AssertionCbk)) {
          if (defData->Assertion.isConstraint()) 
            CALLBACK(defCallbacks->ConstraintCbk, defrConstraintCbkType, &defData->Assertion);
          if (defData->Assertion.isAssertion()) 
            CALLBACK(defCallbacks->AssertionCbk, defrAssertionCbkType, &defData->Assertion);
        }
      }
#line 8230 "def.tab.c"
    break;

  case 612: /* operand_rule: '-' operand delay_specs ';'  */
#line 3645 "def.y"
      { 
        if ((defData->VersionNum < 5.4) && (defCallbacks->ConstraintCbk || defCallbacks->AssertionCbk)) {
          if (defData->Assertion.isConstraint()) 
            CALLBACK(defCallbacks->ConstraintCbk, defrConstraintCbkType, &defData->Assertion);
          if (defData->Assertion.isAssertion()) 
            CALLBACK(defCallbacks->AssertionCbk, defrAssertionCbkType, &defData->Assertion);
        }
   
        // reset all the flags and everything
        defData->Assertion.clear();
      }
#line 8246 "def.tab.c"
    break;

  case 613: /* $@106: %empty  */
#line 3657 "def.y"
               { defData->dumb_mode = 1; defData->no_num = 1; }
#line 8252 "def.tab.c"
    break;

  case 614: /* operand: K_NET $@106 T_STRING  */
#line 3658 "def.y"
      {
         if (defCallbacks->ConstraintCbk || defCallbacks->AssertionCbk)
           defData->Assertion.addNet((yyvsp[0].string));
      }
#line 8261 "def.tab.c"
    break;

  case 615: /* $@107: %empty  */
#line 3662 "def.y"
               {defData->dumb_mode = 4; defData->no_num = 4;}
#line 8267 "def.tab.c"
    break;

  case 616: /* operand: K_PATH $@107 T_STRING T_STRING T_STRING T_STRING  */
#line 3663 "def.y"
      {
         if (defCallbacks->ConstraintCbk || defCallbacks->AssertionCbk)
           defData->Assertion.addPath((yyvsp[-3].string), (yyvsp[-2].string), (yyvsp[-1].string), (yyvsp[0].string));
      }
#line 8276 "def.tab.c"
    break;

  case 617: /* operand: K_SUM '(' operand_list ')'  */
#line 3668 "def.y"
      {
        if (defCallbacks->ConstraintCbk || defCallbacks->AssertionCbk)
           defData->Assertion.setSum();
      }
#line 8285 "def.tab.c"
    break;

  case 618: /* operand: K_DIFF '(' operand_list ')'  */
#line 3673 "def.y"
      {
        if (defCallbacks->ConstraintCbk || defCallbacks->AssertionCbk)
           defData->Assertion.setDiff();
      }
#line 8294 "def.tab.c"
    break;

  case 620: /* operand_list: operand_list operand  */
#line 3680 "def.y"
      { }
#line 8300 "def.tab.c"
    break;

  case 622: /* $@108: %empty  */
#line 3683 "def.y"
                                  { defData->dumb_mode = 1; defData->no_num = 1; }
#line 8306 "def.tab.c"
    break;

  case 623: /* wiredlogic_rule: '-' K_WIREDLOGIC $@108 T_STRING opt_plus K_MAXDIST NUMBER ';'  */
#line 3685 "def.y"
      {
        if (defCallbacks->ConstraintCbk || defCallbacks->AssertionCbk)
          defData->Assertion.setWiredlogic((yyvsp[-4].string), (yyvsp[-1].dval));
      }
#line 8315 "def.tab.c"
    break;

  case 624: /* opt_plus: %empty  */
#line 3692 "def.y"
      { (yyval.string) = (char*)""; }
#line 8321 "def.tab.c"
    break;

  case 625: /* opt_plus: '+'  */
#line 3694 "def.y"
      { (yyval.string) = (char*)"+"; }
#line 8327 "def.tab.c"
    break;

  case 628: /* delay_spec: '+' K_RISEMIN NUMBER  */
#line 3701 "def.y"
      {
        if (defCallbacks->ConstraintCbk || defCallbacks->AssertionCbk)
          defData->Assertion.setRiseMin((yyvsp[0].dval));
      }
#line 8336 "def.tab.c"
    break;

  case 629: /* delay_spec: '+' K_RISEMAX NUMBER  */
#line 3706 "def.y"
      {
        if (defCallbacks->ConstraintCbk || defCallbacks->AssertionCbk)
          defData->Assertion.setRiseMax((yyvsp[0].dval));
      }
#line 8345 "def.tab.c"
    break;

  case 630: /* delay_spec: '+' K_FALLMIN NUMBER  */
#line 3711 "def.y"
      {
        if (defCallbacks->ConstraintCbk || defCallbacks->AssertionCbk)
          defData->Assertion.setFallMin((yyvsp[0].dval));
      }
#line 8354 "def.tab.c"
    break;

  case 631: /* delay_spec: '+' K_FALLMAX NUMBER  */
#line 3716 "def.y"
      {
        if (defCallbacks->ConstraintCbk || defCallbacks->AssertionCbk)
          defData->Assertion.setFallMax((yyvsp[0].dval));
      }
#line 8363 "def.tab.c"
    break;

  case 632: /* constraints_end: K_END K_CONSTRAINTS  */
#line 3722 "def.y"
      { if ((defData->VersionNum < 5.4) && defCallbacks->ConstraintsEndCbk) {
          CALLBACK(defCallbacks->ConstraintsEndCbk, defrConstraintsEndCbkType, 0);
        } else {
          if (defCallbacks->ConstraintsEndCbk) {
            if (defData->constraintWarnings++ < defSettings->ConstraintWarnings)
              defWarning(7032, "The CONSTRAINTS statement is obsolete in version 5.4 and later.\nThe DEF parser will ignore this statement.");
          }
        }
      }
#line 8377 "def.tab.c"
    break;

  case 633: /* assertions_end: K_END K_ASSERTIONS  */
#line 3733 "def.y"
      { if ((defData->VersionNum < 5.4) && defCallbacks->AssertionsEndCbk) {
          CALLBACK(defCallbacks->AssertionsEndCbk, defrAssertionsEndCbkType, 0);
        } else {
          if (defCallbacks->AssertionsEndCbk) {
            if (defData->assertionWarnings++ < defSettings->AssertionWarnings)
              defWarning(7031, "The ASSERTIONS statement is obsolete in version 5.4 and later.\nThe DEF parser will ignore this statement.");
          }
        }
      }
#line 8391 "def.tab.c"
    break;

  case 635: /* scanchain_start: K_SCANCHAINS NUMBER ';'  */
#line 3747 "def.y"
      { if (defCallbacks->ScanchainsStartCbk)
          CALLBACK(defCallbacks->ScanchainsStartCbk, defrScanchainsStartCbkType,
                   ROUND((yyvsp[-1].dval)));
      }
#line 8400 "def.tab.c"
    break;

  case 637: /* scanchain_rules: scanchain_rules scan_rule  */
#line 3754 "def.y"
      {}
#line 8406 "def.tab.c"
    break;

  case 638: /* scan_rule: start_scan scan_members ';'  */
#line 3757 "def.y"
      { 
        if (defCallbacks->ScanchainCbk)
          CALLBACK(defCallbacks->ScanchainCbk, defrScanchainCbkType, &defData->Scanchain);
      }
#line 8415 "def.tab.c"
    break;

  case 639: /* $@109: %empty  */
#line 3762 "def.y"
                {defData->dumb_mode = 1; defData->no_num = 1;}
#line 8421 "def.tab.c"
    break;

  case 640: /* start_scan: '-' $@109 T_STRING  */
#line 3763 "def.y"
      {
        if (defCallbacks->ScanchainCbk)
          defData->Scanchain.setName((yyvsp[0].string));
        defData->bit_is_keyword = TRUE;
      }
#line 8431 "def.tab.c"
    break;

  case 643: /* opt_pin: %empty  */
#line 3775 "def.y"
      { (yyval.string) = (char*)""; }
#line 8437 "def.tab.c"
    break;

  case 644: /* opt_pin: T_STRING  */
#line 3777 "def.y"
      { (yyval.string) = (yyvsp[0].string); }
#line 8443 "def.tab.c"
    break;

  case 645: /* $@110: %empty  */
#line 3779 "def.y"
                         {defData->dumb_mode = 2; defData->no_num = 2;}
#line 8449 "def.tab.c"
    break;

  case 646: /* scan_member: '+' K_START $@110 T_STRING opt_pin  */
#line 3780 "def.y"
      { if (defCallbacks->ScanchainCbk)
          defData->Scanchain.setStart((yyvsp[-1].string), (yyvsp[0].string));
      }
#line 8457 "def.tab.c"
    break;

  case 647: /* $@111: %empty  */
#line 3783 "def.y"
                       { defData->dumb_mode = 1; defData->no_num = 1; }
#line 8463 "def.tab.c"
    break;

  case 648: /* scan_member: '+' K_FLOATING $@111 floating_inst_list  */
#line 3784 "def.y"
      { defData->dumb_mode = 0; defData->no_num = 0; }
#line 8469 "def.tab.c"
    break;

  case 649: /* $@112: %empty  */
#line 3786 "def.y"
      {
         defData->dumb_mode = 1;
         defData->no_num = 1;
         if (defCallbacks->ScanchainCbk)
           defData->Scanchain.addOrderedList();
      }
#line 8480 "def.tab.c"
    break;

  case 650: /* scan_member: '+' K_ORDERED $@112 ordered_inst_list  */
#line 3793 "def.y"
      { defData->dumb_mode = 0; defData->no_num = 0; }
#line 8486 "def.tab.c"
    break;

  case 651: /* $@113: %empty  */
#line 3794 "def.y"
                   {defData->dumb_mode = 2; defData->no_num = 2; }
#line 8492 "def.tab.c"
    break;

  case 652: /* scan_member: '+' K_STOP $@113 T_STRING opt_pin  */
#line 3795 "def.y"
      { if (defCallbacks->ScanchainCbk)
          defData->Scanchain.setStop((yyvsp[-1].string), (yyvsp[0].string));
      }
#line 8500 "def.tab.c"
    break;

  case 653: /* $@114: %empty  */
#line 3798 "def.y"
                             { defData->dumb_mode = 10; defData->no_num = 10; }
#line 8506 "def.tab.c"
    break;

  case 654: /* scan_member: '+' K_COMMONSCANPINS $@114 opt_common_pins  */
#line 3799 "def.y"
      { defData->dumb_mode = 0;  defData->no_num = 0; }
#line 8512 "def.tab.c"
    break;

  case 655: /* $@115: %empty  */
#line 3800 "def.y"
                        { defData->dumb_mode = 1; defData->no_num = 1; }
#line 8518 "def.tab.c"
    break;

  case 656: /* scan_member: '+' K_PARTITION $@115 T_STRING partition_maxbits  */
#line 3802 "def.y"
      {
        if (defData->VersionNum < 5.5) {
          if (defCallbacks->ScanchainCbk) {
            if (defData->scanchainWarnings++ < defSettings->ScanchainWarnings) {
              defData->defMsg = (char*)defMalloc(1000);
              sprintf (defData->defMsg,
                 "The PARTITION statement is available in version 5.5 and later.\nHowever, your DEF file is defined with version %g", defData->VersionNum);
              defError(6538, defData->defMsg);
              defFree(defData->defMsg);
              CHKERR();
            }
          }
        }
        if (defCallbacks->ScanchainCbk)
          defData->Scanchain.setPartition((yyvsp[-1].string), (yyvsp[0].integer));
      }
#line 8539 "def.tab.c"
    break;

  case 657: /* scan_member: extension_stmt  */
#line 3819 "def.y"
      {
        if (defCallbacks->ScanChainExtCbk)
          CALLBACK(defCallbacks->ScanChainExtCbk, defrScanChainExtCbkType, &defData->History_text[0]);
      }
#line 8548 "def.tab.c"
    break;

  case 658: /* opt_common_pins: %empty  */
#line 3825 "def.y"
      { }
#line 8554 "def.tab.c"
    break;

  case 659: /* opt_common_pins: '(' T_STRING T_STRING ')'  */
#line 3827 "def.y"
      {
        if (defCallbacks->ScanchainCbk) {
          if (strcmp((yyvsp[-2].string), "IN") == 0 || strcmp((yyvsp[-2].string), "in") == 0)
            defData->Scanchain.setCommonIn((yyvsp[-1].string));
          else if (strcmp((yyvsp[-2].string), "OUT") == 0 || strcmp((yyvsp[-2].string), "out") == 0)
            defData->Scanchain.setCommonOut((yyvsp[-1].string));
        }
      }
#line 8567 "def.tab.c"
    break;

  case 660: /* opt_common_pins: '(' T_STRING T_STRING ')' '(' T_STRING T_STRING ')'  */
#line 3836 "def.y"
      {
        if (defCallbacks->ScanchainCbk) {
          if (strcmp((yyvsp[-6].string), "IN") == 0 || strcmp((yyvsp[-6].string), "in") == 0)
            defData->Scanchain.setCommonIn((yyvsp[-5].string));
          else if (strcmp((yyvsp[-6].string), "OUT") == 0 || strcmp((yyvsp[-6].string), "out") == 0)
            defData->Scanchain.setCommonOut((yyvsp[-5].string));
          if (strcmp((yyvsp[-2].string), "IN") == 0 || strcmp((yyvsp[-2].string), "in") == 0)
            defData->Scanchain.setCommonIn((yyvsp[-1].string));
          else if (strcmp((yyvsp[-2].string), "OUT") == 0 || strcmp((yyvsp[-2].string), "out") == 0)
            defData->Scanchain.setCommonOut((yyvsp[-1].string));
        }
      }
#line 8584 "def.tab.c"
    break;

  case 663: /* $@116: %empty  */
#line 3854 "def.y"
      {
        defData->dumb_mode = 1000;
        defData->no_num = 1000;
        if (defCallbacks->ScanchainCbk)
          defData->Scanchain.addFloatingInst((yyvsp[0].string));
      }
#line 8595 "def.tab.c"
    break;

  case 664: /* one_floating_inst: T_STRING $@116 floating_pins  */
#line 3861 "def.y"
      { defData->dumb_mode = 1; defData->no_num = 1; }
#line 8601 "def.tab.c"
    break;

  case 665: /* floating_pins: %empty  */
#line 3864 "def.y"
      { }
#line 8607 "def.tab.c"
    break;

  case 666: /* floating_pins: '(' T_STRING T_STRING ')'  */
#line 3866 "def.y"
      {
        if (defCallbacks->ScanchainCbk) {
          if (strcmp((yyvsp[-2].string), "IN") == 0 || strcmp((yyvsp[-2].string), "in") == 0)
            defData->Scanchain.addFloatingIn((yyvsp[-1].string));
          else if (strcmp((yyvsp[-2].string), "OUT") == 0 || strcmp((yyvsp[-2].string), "out") == 0)
            defData->Scanchain.addFloatingOut((yyvsp[-1].string));
          else if (strcmp((yyvsp[-2].string), "BITS") == 0 || strcmp((yyvsp[-2].string), "bits") == 0) {
            defData->bitsNum = atoi((yyvsp[-1].string));
            defData->Scanchain.setFloatingBits(defData->bitsNum);
          }
        }
      }
#line 8624 "def.tab.c"
    break;

  case 667: /* floating_pins: '(' T_STRING T_STRING ')' '(' T_STRING T_STRING ')'  */
#line 3879 "def.y"
      {
        if (defCallbacks->ScanchainCbk) {
          if (strcmp((yyvsp[-6].string), "IN") == 0 || strcmp((yyvsp[-6].string), "in") == 0)
            defData->Scanchain.addFloatingIn((yyvsp[-5].string));
          else if (strcmp((yyvsp[-6].string), "OUT") == 0 || strcmp((yyvsp[-6].string), "out") == 0)
            defData->Scanchain.addFloatingOut((yyvsp[-5].string));
          else if (strcmp((yyvsp[-6].string), "BITS") == 0 || strcmp((yyvsp[-6].string), "bits") == 0) {
            defData->bitsNum = atoi((yyvsp[-5].string));
            defData->Scanchain.setFloatingBits(defData->bitsNum);
          }
          if (strcmp((yyvsp[-2].string), "IN") == 0 || strcmp((yyvsp[-2].string), "in") == 0)
            defData->Scanchain.addFloatingIn((yyvsp[-1].string));
          else if (strcmp((yyvsp[-2].string), "OUT") == 0 || strcmp((yyvsp[-2].string), "out") == 0)
            defData->Scanchain.addFloatingOut((yyvsp[-1].string));
          else if (strcmp((yyvsp[-2].string), "BITS") == 0 || strcmp((yyvsp[-2].string), "bits") == 0) {
            defData->bitsNum = atoi((yyvsp[-1].string));
            defData->Scanchain.setFloatingBits(defData->bitsNum);
          }
        }
      }
#line 8649 "def.tab.c"
    break;

  case 668: /* floating_pins: '(' T_STRING T_STRING ')' '(' T_STRING T_STRING ')' '(' T_STRING T_STRING ')'  */
#line 3901 "def.y"
      {
        if (defCallbacks->ScanchainCbk) {
          if (strcmp((yyvsp[-10].string), "IN") == 0 || strcmp((yyvsp[-10].string), "in") == 0)
            defData->Scanchain.addFloatingIn((yyvsp[-9].string));
          else if (strcmp((yyvsp[-10].string), "OUT") == 0 || strcmp((yyvsp[-10].string), "out") == 0)
            defData->Scanchain.addFloatingOut((yyvsp[-9].string));
          else if (strcmp((yyvsp[-10].string), "BITS") == 0 || strcmp((yyvsp[-10].string), "bits") == 0) {
            defData->bitsNum = atoi((yyvsp[-9].string));
            defData->Scanchain.setFloatingBits(defData->bitsNum);
          }
          if (strcmp((yyvsp[-6].string), "IN") == 0 || strcmp((yyvsp[-6].string), "in") == 0)
            defData->Scanchain.addFloatingIn((yyvsp[-5].string));
          else if (strcmp((yyvsp[-6].string), "OUT") == 0 || strcmp((yyvsp[-6].string), "out") == 0)
            defData->Scanchain.addFloatingOut((yyvsp[-5].string));
          else if (strcmp((yyvsp[-6].string), "BITS") == 0 || strcmp((yyvsp[-6].string), "bits") == 0) {
            defData->bitsNum = atoi((yyvsp[-5].string));
            defData->Scanchain.setFloatingBits(defData->bitsNum);
          }
          if (strcmp((yyvsp[-2].string), "IN") == 0 || strcmp((yyvsp[-2].string), "in") == 0)
            defData->Scanchain.addFloatingIn((yyvsp[-1].string));
          else if (strcmp((yyvsp[-2].string), "OUT") == 0 || strcmp((yyvsp[-2].string), "out") == 0)
            defData->Scanchain.addFloatingOut((yyvsp[-1].string));
          else if (strcmp((yyvsp[-2].string), "BITS") == 0 || strcmp((yyvsp[-2].string), "bits") == 0) {
            defData->bitsNum = atoi((yyvsp[-1].string));
            defData->Scanchain.setFloatingBits(defData->bitsNum);
          }
        }
      }
#line 8682 "def.tab.c"
    break;

  case 671: /* $@117: %empty  */
#line 3935 "def.y"
      { defData->dumb_mode = 1000; defData->no_num = 1000; 
        if (defCallbacks->ScanchainCbk)
          defData->Scanchain.addOrderedInst((yyvsp[0].string));
      }
#line 8691 "def.tab.c"
    break;

  case 672: /* one_ordered_inst: T_STRING $@117 ordered_pins  */
#line 3940 "def.y"
      { defData->dumb_mode = 1; defData->no_num = 1; }
#line 8697 "def.tab.c"
    break;

  case 673: /* ordered_pins: %empty  */
#line 3943 "def.y"
      { }
#line 8703 "def.tab.c"
    break;

  case 674: /* ordered_pins: '(' T_STRING T_STRING ')'  */
#line 3945 "def.y"
      {
        if (defCallbacks->ScanchainCbk) {
          if (strcmp((yyvsp[-2].string), "IN") == 0 || strcmp((yyvsp[-2].string), "in") == 0)
            defData->Scanchain.addOrderedIn((yyvsp[-1].string));
          else if (strcmp((yyvsp[-2].string), "OUT") == 0 || strcmp((yyvsp[-2].string), "out") == 0)
            defData->Scanchain.addOrderedOut((yyvsp[-1].string));
          else if (strcmp((yyvsp[-2].string), "BITS") == 0 || strcmp((yyvsp[-2].string), "bits") == 0) {
            defData->bitsNum = atoi((yyvsp[-1].string));
            defData->Scanchain.setOrderedBits(defData->bitsNum);
         }
        }
      }
#line 8720 "def.tab.c"
    break;

  case 675: /* ordered_pins: '(' T_STRING T_STRING ')' '(' T_STRING T_STRING ')'  */
#line 3958 "def.y"
      {
        if (defCallbacks->ScanchainCbk) {
          if (strcmp((yyvsp[-6].string), "IN") == 0 || strcmp((yyvsp[-6].string), "in") == 0)
            defData->Scanchain.addOrderedIn((yyvsp[-5].string));
          else if (strcmp((yyvsp[-6].string), "OUT") == 0 || strcmp((yyvsp[-6].string), "out") == 0)
            defData->Scanchain.addOrderedOut((yyvsp[-5].string));
          else if (strcmp((yyvsp[-6].string), "BITS") == 0 || strcmp((yyvsp[-6].string), "bits") == 0) {
            defData->bitsNum = atoi((yyvsp[-5].string));
            defData->Scanchain.setOrderedBits(defData->bitsNum);
          }
          if (strcmp((yyvsp[-2].string), "IN") == 0 || strcmp((yyvsp[-2].string), "in") == 0)
            defData->Scanchain.addOrderedIn((yyvsp[-1].string));
          else if (strcmp((yyvsp[-2].string), "OUT") == 0 || strcmp((yyvsp[-2].string), "out") == 0)
            defData->Scanchain.addOrderedOut((yyvsp[-1].string));
          else if (strcmp((yyvsp[-2].string), "BITS") == 0 || strcmp((yyvsp[-2].string), "bits") == 0) {
            defData->bitsNum = atoi((yyvsp[-1].string));
            defData->Scanchain.setOrderedBits(defData->bitsNum);
          }
        }
      }
#line 8745 "def.tab.c"
    break;

  case 676: /* ordered_pins: '(' T_STRING T_STRING ')' '(' T_STRING T_STRING ')' '(' T_STRING T_STRING ')'  */
#line 3980 "def.y"
      {
        if (defCallbacks->ScanchainCbk) {
          if (strcmp((yyvsp[-10].string), "IN") == 0 || strcmp((yyvsp[-10].string), "in") == 0)
            defData->Scanchain.addOrderedIn((yyvsp[-9].string));
          else if (strcmp((yyvsp[-10].string), "OUT") == 0 || strcmp((yyvsp[-10].string), "out") == 0)
            defData->Scanchain.addOrderedOut((yyvsp[-9].string));
          else if (strcmp((yyvsp[-10].string), "BITS") == 0 || strcmp((yyvsp[-10].string), "bits") == 0) {
            defData->bitsNum = atoi((yyvsp[-9].string));
            defData->Scanchain.setOrderedBits(defData->bitsNum);
          }
          if (strcmp((yyvsp[-6].string), "IN") == 0 || strcmp((yyvsp[-6].string), "in") == 0)
            defData->Scanchain.addOrderedIn((yyvsp[-5].string));
          else if (strcmp((yyvsp[-6].string), "OUT") == 0 || strcmp((yyvsp[-6].string), "out") == 0)
            defData->Scanchain.addOrderedOut((yyvsp[-5].string));
          else if (strcmp((yyvsp[-6].string), "BITS") == 0 || strcmp((yyvsp[-6].string), "bits") == 0) {
            defData->bitsNum = atoi((yyvsp[-5].string));
            defData->Scanchain.setOrderedBits(defData->bitsNum);
          }
          if (strcmp((yyvsp[-2].string), "IN") == 0 || strcmp((yyvsp[-2].string), "in") == 0)
            defData->Scanchain.addOrderedIn((yyvsp[-1].string));
          else if (strcmp((yyvsp[-2].string), "OUT") == 0 || strcmp((yyvsp[-2].string), "out") == 0)
            defData->Scanchain.addOrderedOut((yyvsp[-1].string));
          else if (strcmp((yyvsp[-2].string), "BITS") == 0 || strcmp((yyvsp[-2].string), "bits") == 0) {
            defData->bitsNum = atoi((yyvsp[-1].string));
            defData->Scanchain.setOrderedBits(defData->bitsNum);
          }
        }
      }
#line 8778 "def.tab.c"
    break;

  case 677: /* partition_maxbits: %empty  */
#line 4010 "def.y"
      { (yyval.integer) = -1; }
#line 8784 "def.tab.c"
    break;

  case 678: /* partition_maxbits: K_MAXBITS NUMBER  */
#line 4012 "def.y"
      { (yyval.integer) = ROUND((yyvsp[0].dval)); }
#line 8790 "def.tab.c"
    break;

  case 679: /* scanchain_end: K_END K_SCANCHAINS  */
#line 4015 "def.y"
      { 
        if (defCallbacks->ScanchainsEndCbk)
          CALLBACK(defCallbacks->ScanchainsEndCbk, defrScanchainsEndCbkType, 0);
        defData->bit_is_keyword = FALSE;
        defData->dumb_mode = 0; defData->no_num = 0;
      }
#line 8801 "def.tab.c"
    break;

  case 681: /* iotiming_start: K_IOTIMINGS NUMBER ';'  */
#line 4027 "def.y"
      {
        if (defData->VersionNum < 5.4 && defCallbacks->IOTimingsStartCbk) {
          CALLBACK(defCallbacks->IOTimingsStartCbk, defrIOTimingsStartCbkType, ROUND((yyvsp[-1].dval)));
        } else {
          if (defCallbacks->IOTimingsStartCbk)
            if (defData->iOTimingWarnings++ < defSettings->IOTimingWarnings)
              defWarning(7035, "The IOTIMINGS statement is obsolete in version 5.4 and later.\nThe DEF parser will ignore this statement.");
        }
      }
#line 8815 "def.tab.c"
    break;

  case 683: /* iotiming_rules: iotiming_rules iotiming_rule  */
#line 4039 "def.y"
      { }
#line 8821 "def.tab.c"
    break;

  case 684: /* iotiming_rule: start_iotiming iotiming_members ';'  */
#line 4042 "def.y"
      { 
        if (defData->VersionNum < 5.4 && defCallbacks->IOTimingCbk)
          CALLBACK(defCallbacks->IOTimingCbk, defrIOTimingCbkType, &defData->IOTiming);
      }
#line 8830 "def.tab.c"
    break;

  case 685: /* $@118: %empty  */
#line 4047 "def.y"
                        {defData->dumb_mode = 2; defData->no_num = 2; }
#line 8836 "def.tab.c"
    break;

  case 686: /* start_iotiming: '-' '(' $@118 T_STRING T_STRING ')'  */
#line 4048 "def.y"
      {
        if (defCallbacks->IOTimingCbk)
          defData->IOTiming.setName((yyvsp[-2].string), (yyvsp[-1].string));
      }
#line 8845 "def.tab.c"
    break;

  case 689: /* iotiming_member: '+' risefall K_VARIABLE NUMBER NUMBER  */
#line 4059 "def.y"
      {
        if (defCallbacks->IOTimingCbk) 
          defData->IOTiming.setVariable((yyvsp[-3].string), (yyvsp[-1].dval), (yyvsp[0].dval));
      }
#line 8854 "def.tab.c"
    break;

  case 690: /* iotiming_member: '+' risefall K_SLEWRATE NUMBER NUMBER  */
#line 4064 "def.y"
      {
        if (defCallbacks->IOTimingCbk) 
          defData->IOTiming.setSlewRate((yyvsp[-3].string), (yyvsp[-1].dval), (yyvsp[0].dval));
      }
#line 8863 "def.tab.c"
    break;

  case 691: /* iotiming_member: '+' K_CAPACITANCE NUMBER  */
#line 4069 "def.y"
      {
        if (defCallbacks->IOTimingCbk) 
          defData->IOTiming.setCapacitance((yyvsp[0].dval));
      }
#line 8872 "def.tab.c"
    break;

  case 692: /* $@119: %empty  */
#line 4073 "def.y"
                        {defData->dumb_mode = 1; defData->no_num = 1; }
#line 8878 "def.tab.c"
    break;

  case 693: /* $@120: %empty  */
#line 4074 "def.y"
      {
        if (defCallbacks->IOTimingCbk) 
          defData->IOTiming.setDriveCell((yyvsp[0].string));
      }
#line 8887 "def.tab.c"
    break;

  case 695: /* iotiming_member: extension_stmt  */
#line 4083 "def.y"
      {
        if (defData->VersionNum < 5.4 && defCallbacks->IoTimingsExtCbk)
          CALLBACK(defCallbacks->IoTimingsExtCbk, defrIoTimingsExtCbkType, &defData->History_text[0]);
      }
#line 8896 "def.tab.c"
    break;

  case 696: /* $@121: %empty  */
#line 4089 "def.y"
              {defData->dumb_mode = 1; defData->no_num = 1; }
#line 8902 "def.tab.c"
    break;

  case 697: /* $@122: %empty  */
#line 4090 "def.y"
      {
        if (defCallbacks->IOTimingCbk) 
          defData->IOTiming.setTo((yyvsp[0].string));
      }
#line 8911 "def.tab.c"
    break;

  case 700: /* $@123: %empty  */
#line 4097 "def.y"
                  {defData->dumb_mode = 1; defData->no_num = 1; }
#line 8917 "def.tab.c"
    break;

  case 701: /* iotiming_frompin: K_FROMPIN $@123 T_STRING  */
#line 4098 "def.y"
      {
        if (defCallbacks->IOTimingCbk)
          defData->IOTiming.setFrom((yyvsp[0].string));
      }
#line 8926 "def.tab.c"
    break;

  case 703: /* iotiming_parallel: K_PARALLEL NUMBER  */
#line 4105 "def.y"
      {
        if (defCallbacks->IOTimingCbk)
          defData->IOTiming.setParallel((yyvsp[0].dval));
      }
#line 8935 "def.tab.c"
    break;

  case 704: /* risefall: K_RISE  */
#line 4110 "def.y"
                 { (yyval.string) = (char*)"RISE"; }
#line 8941 "def.tab.c"
    break;

  case 705: /* risefall: K_FALL  */
#line 4110 "def.y"
                                                  { (yyval.string) = (char*)"FALL"; }
#line 8947 "def.tab.c"
    break;

  case 706: /* iotiming_end: K_END K_IOTIMINGS  */
#line 4113 "def.y"
      {
        if (defData->VersionNum < 5.4 && defCallbacks->IOTimingsEndCbk)
          CALLBACK(defCallbacks->IOTimingsEndCbk, defrIOTimingsEndCbkType, 0);
      }
#line 8956 "def.tab.c"
    break;

  case 707: /* floorplan_contraints_section: fp_start fp_stmts K_END K_FPC  */
#line 4119 "def.y"
      { 
        if (defCallbacks->FPCEndCbk)
          CALLBACK(defCallbacks->FPCEndCbk, defrFPCEndCbkType, 0);
      }
#line 8965 "def.tab.c"
    break;

  case 708: /* fp_start: K_FPC NUMBER ';'  */
#line 4125 "def.y"
      {
        if (defCallbacks->FPCStartCbk)
          CALLBACK(defCallbacks->FPCStartCbk, defrFPCStartCbkType, ROUND((yyvsp[-1].dval)));
      }
#line 8974 "def.tab.c"
    break;

  case 710: /* fp_stmts: fp_stmts fp_stmt  */
#line 4132 "def.y"
      {}
#line 8980 "def.tab.c"
    break;

  case 711: /* $@124: %empty  */
#line 4134 "def.y"
             { defData->dumb_mode = 1; defData->no_num = 1;  }
#line 8986 "def.tab.c"
    break;

  case 712: /* $@125: %empty  */
#line 4135 "def.y"
      { if (defCallbacks->FPCCbk) defData->FPC.setName((yyvsp[-1].string), (yyvsp[0].string)); }
#line 8992 "def.tab.c"
    break;

  case 713: /* fp_stmt: '-' $@124 T_STRING h_or_v $@125 constraint_type constrain_what_list ';'  */
#line 4137 "def.y"
      { if (defCallbacks->FPCCbk) CALLBACK(defCallbacks->FPCCbk, defrFPCCbkType, &defData->FPC); }
#line 8998 "def.tab.c"
    break;

  case 714: /* h_or_v: K_HORIZONTAL  */
#line 4140 "def.y"
      { (yyval.string) = (char*)"HORIZONTAL"; }
#line 9004 "def.tab.c"
    break;

  case 715: /* h_or_v: K_VERTICAL  */
#line 4142 "def.y"
      { (yyval.string) = (char*)"VERTICAL"; }
#line 9010 "def.tab.c"
    break;

  case 716: /* constraint_type: K_ALIGN  */
#line 4145 "def.y"
      { if (defCallbacks->FPCCbk) defData->FPC.setAlign(); }
#line 9016 "def.tab.c"
    break;

  case 717: /* constraint_type: K_MAX NUMBER  */
#line 4147 "def.y"
      { if (defCallbacks->FPCCbk) defData->FPC.setMax((yyvsp[0].dval)); }
#line 9022 "def.tab.c"
    break;

  case 718: /* constraint_type: K_MIN NUMBER  */
#line 4149 "def.y"
      { if (defCallbacks->FPCCbk) defData->FPC.setMin((yyvsp[0].dval)); }
#line 9028 "def.tab.c"
    break;

  case 719: /* constraint_type: K_EQUAL NUMBER  */
#line 4151 "def.y"
      { if (defCallbacks->FPCCbk) defData->FPC.setEqual((yyvsp[0].dval)); }
#line 9034 "def.tab.c"
    break;

  case 722: /* $@126: %empty  */
#line 4158 "def.y"
      { if (defCallbacks->FPCCbk) defData->FPC.setDoingBottomLeft(); }
#line 9040 "def.tab.c"
    break;

  case 724: /* $@127: %empty  */
#line 4161 "def.y"
      { if (defCallbacks->FPCCbk) defData->FPC.setDoingTopRight(); }
#line 9046 "def.tab.c"
    break;

  case 728: /* $@128: %empty  */
#line 4168 "def.y"
                         {defData->dumb_mode = 1; defData->no_num = 1; }
#line 9052 "def.tab.c"
    break;

  case 729: /* row_or_comp: '(' K_ROWS $@128 T_STRING ')'  */
#line 4169 "def.y"
      { if (defCallbacks->FPCCbk) defData->FPC.addRow((yyvsp[-1].string)); }
#line 9058 "def.tab.c"
    break;

  case 730: /* $@129: %empty  */
#line 4170 "def.y"
                       {defData->dumb_mode = 1; defData->no_num = 1; }
#line 9064 "def.tab.c"
    break;

  case 731: /* row_or_comp: '(' K_COMPS $@129 T_STRING ')'  */
#line 4171 "def.y"
      { if (defCallbacks->FPCCbk) defData->FPC.addComps((yyvsp[-1].string)); }
#line 9070 "def.tab.c"
    break;

  case 733: /* timingdisables_start: K_TIMINGDISABLES NUMBER ';'  */
#line 4178 "def.y"
      { 
        if (defCallbacks->TimingDisablesStartCbk)
          CALLBACK(defCallbacks->TimingDisablesStartCbk, defrTimingDisablesStartCbkType,
                   ROUND((yyvsp[-1].dval)));
      }
#line 9080 "def.tab.c"
    break;

  case 735: /* timingdisables_rules: timingdisables_rules timingdisables_rule  */
#line 4186 "def.y"
      {}
#line 9086 "def.tab.c"
    break;

  case 736: /* $@130: %empty  */
#line 4188 "def.y"
                                   { defData->dumb_mode = 2; defData->no_num = 2;  }
#line 9092 "def.tab.c"
    break;

  case 737: /* $@131: %empty  */
#line 4189 "def.y"
                       { defData->dumb_mode = 2; defData->no_num = 2;  }
#line 9098 "def.tab.c"
    break;

  case 738: /* timingdisables_rule: '-' K_FROMPIN $@130 T_STRING T_STRING K_TOPIN $@131 T_STRING T_STRING ';'  */
#line 4190 "def.y"
      {
        if (defCallbacks->TimingDisableCbk) {
          defData->TimingDisable.setFromTo((yyvsp[-6].string), (yyvsp[-5].string), (yyvsp[-2].string), (yyvsp[-1].string));
          CALLBACK(defCallbacks->TimingDisableCbk, defrTimingDisableCbkType,
                &defData->TimingDisable);
        }
      }
#line 9110 "def.tab.c"
    break;

  case 739: /* $@132: %empty  */
#line 4197 "def.y"
                      {defData->dumb_mode = 2; defData->no_num = 2; }
#line 9116 "def.tab.c"
    break;

  case 740: /* timingdisables_rule: '-' K_THRUPIN $@132 T_STRING T_STRING ';'  */
#line 4198 "def.y"
      {
        if (defCallbacks->TimingDisableCbk) {
          defData->TimingDisable.setThru((yyvsp[-2].string), (yyvsp[-1].string));
          CALLBACK(defCallbacks->TimingDisableCbk, defrTimingDisableCbkType,
                   &defData->TimingDisable);
        }
      }
#line 9128 "def.tab.c"
    break;

  case 741: /* $@133: %empty  */
#line 4205 "def.y"
                    {defData->dumb_mode = 1; defData->no_num = 1;}
#line 9134 "def.tab.c"
    break;

  case 742: /* timingdisables_rule: '-' K_MACRO $@133 T_STRING td_macro_option ';'  */
#line 4206 "def.y"
      {
        if (defCallbacks->TimingDisableCbk) {
          defData->TimingDisable.setMacro((yyvsp[-2].string));
          CALLBACK(defCallbacks->TimingDisableCbk, defrTimingDisableCbkType,
                &defData->TimingDisable);
        }
      }
#line 9146 "def.tab.c"
    break;

  case 743: /* timingdisables_rule: '-' K_REENTRANTPATHS ';'  */
#line 4214 "def.y"
      { if (defCallbacks->TimingDisableCbk)
          defData->TimingDisable.setReentrantPathsFlag();
      }
#line 9154 "def.tab.c"
    break;

  case 744: /* $@134: %empty  */
#line 4219 "def.y"
                           {defData->dumb_mode = 1; defData->no_num = 1;}
#line 9160 "def.tab.c"
    break;

  case 745: /* $@135: %empty  */
#line 4220 "def.y"
      {defData->dumb_mode=1; defData->no_num = 1;}
#line 9166 "def.tab.c"
    break;

  case 746: /* td_macro_option: K_FROMPIN $@134 T_STRING K_TOPIN $@135 T_STRING  */
#line 4221 "def.y"
      {
        if (defCallbacks->TimingDisableCbk)
          defData->TimingDisable.setMacroFromTo((yyvsp[-3].string),(yyvsp[0].string));
      }
#line 9175 "def.tab.c"
    break;

  case 747: /* $@136: %empty  */
#line 4225 "def.y"
                         {defData->dumb_mode=1; defData->no_num = 1;}
#line 9181 "def.tab.c"
    break;

  case 748: /* td_macro_option: K_THRUPIN $@136 T_STRING  */
#line 4226 "def.y"
      {
        if (defCallbacks->TimingDisableCbk)
          defData->TimingDisable.setMacroThru((yyvsp[0].string));
      }
#line 9190 "def.tab.c"
    break;

  case 749: /* timingdisables_end: K_END K_TIMINGDISABLES  */
#line 4232 "def.y"
      { 
        if (defCallbacks->TimingDisablesEndCbk)
          CALLBACK(defCallbacks->TimingDisablesEndCbk, defrTimingDisablesEndCbkType, 0);
      }
#line 9199 "def.tab.c"
    break;

  case 751: /* partitions_start: K_PARTITIONS NUMBER ';'  */
#line 4242 "def.y"
      {
        if (defCallbacks->PartitionsStartCbk)
          CALLBACK(defCallbacks->PartitionsStartCbk, defrPartitionsStartCbkType,
                   ROUND((yyvsp[-1].dval)));
      }
#line 9209 "def.tab.c"
    break;

  case 753: /* partition_rules: partition_rules partition_rule  */
#line 4250 "def.y"
      { }
#line 9215 "def.tab.c"
    break;

  case 754: /* partition_rule: start_partition partition_members ';'  */
#line 4253 "def.y"
      { 
        if (defCallbacks->PartitionCbk)
          CALLBACK(defCallbacks->PartitionCbk, defrPartitionCbkType, &defData->Partition);
      }
#line 9224 "def.tab.c"
    break;

  case 755: /* $@137: %empty  */
#line 4258 "def.y"
                     { defData->dumb_mode = 1; defData->no_num = 1; }
#line 9230 "def.tab.c"
    break;

  case 756: /* start_partition: '-' $@137 T_STRING turnoff  */
#line 4259 "def.y"
      {
        if (defCallbacks->PartitionCbk)
          defData->Partition.setName((yyvsp[-1].string));
      }
#line 9239 "def.tab.c"
    break;

  case 758: /* turnoff: K_TURNOFF turnoff_setup turnoff_hold  */
#line 4266 "def.y"
      {
        if (defCallbacks->PartitionCbk)
          defData->Partition.addTurnOff((yyvsp[-1].string), (yyvsp[0].string));
      }
#line 9248 "def.tab.c"
    break;

  case 759: /* turnoff_setup: %empty  */
#line 4272 "def.y"
      { (yyval.string) = (char*)" "; }
#line 9254 "def.tab.c"
    break;

  case 760: /* turnoff_setup: K_SETUPRISE  */
#line 4274 "def.y"
      { (yyval.string) = (char*)"R"; }
#line 9260 "def.tab.c"
    break;

  case 761: /* turnoff_setup: K_SETUPFALL  */
#line 4276 "def.y"
      { (yyval.string) = (char*)"F"; }
#line 9266 "def.tab.c"
    break;

  case 762: /* turnoff_hold: %empty  */
#line 4279 "def.y"
      { (yyval.string) = (char*)" "; }
#line 9272 "def.tab.c"
    break;

  case 763: /* turnoff_hold: K_HOLDRISE  */
#line 4281 "def.y"
      { (yyval.string) = (char*)"R"; }
#line 9278 "def.tab.c"
    break;

  case 764: /* turnoff_hold: K_HOLDFALL  */
#line 4283 "def.y"
      { (yyval.string) = (char*)"F"; }
#line 9284 "def.tab.c"
    break;

  case 767: /* $@138: %empty  */
#line 4289 "def.y"
                                     {defData->dumb_mode=2; defData->no_num = 2;}
#line 9290 "def.tab.c"
    break;

  case 768: /* partition_member: '+' K_FROMCLOCKPIN $@138 T_STRING T_STRING risefall minmaxpins  */
#line 4291 "def.y"
      {
        if (defCallbacks->PartitionCbk)
          defData->Partition.setFromClockPin((yyvsp[-3].string), (yyvsp[-2].string));
      }
#line 9299 "def.tab.c"
    break;

  case 769: /* $@139: %empty  */
#line 4295 "def.y"
                          {defData->dumb_mode=2; defData->no_num = 2; }
#line 9305 "def.tab.c"
    break;

  case 770: /* partition_member: '+' K_FROMCOMPPIN $@139 T_STRING T_STRING risefallminmax2_list  */
#line 4297 "def.y"
      {
        if (defCallbacks->PartitionCbk)
          defData->Partition.setFromCompPin((yyvsp[-2].string), (yyvsp[-1].string));
      }
#line 9314 "def.tab.c"
    break;

  case 771: /* $@140: %empty  */
#line 4301 "def.y"
                        {defData->dumb_mode=1; defData->no_num = 1; }
#line 9320 "def.tab.c"
    break;

  case 772: /* partition_member: '+' K_FROMIOPIN $@140 T_STRING risefallminmax1_list  */
#line 4303 "def.y"
      {
        if (defCallbacks->PartitionCbk)
          defData->Partition.setFromIOPin((yyvsp[-1].string));
      }
#line 9329 "def.tab.c"
    break;

  case 773: /* $@141: %empty  */
#line 4307 "def.y"
                         {defData->dumb_mode=2; defData->no_num = 2; }
#line 9335 "def.tab.c"
    break;

  case 774: /* partition_member: '+' K_TOCLOCKPIN $@141 T_STRING T_STRING risefall minmaxpins  */
#line 4309 "def.y"
      {
        if (defCallbacks->PartitionCbk)
          defData->Partition.setToClockPin((yyvsp[-3].string), (yyvsp[-2].string));
      }
#line 9344 "def.tab.c"
    break;

  case 775: /* $@142: %empty  */
#line 4313 "def.y"
                        {defData->dumb_mode=2; defData->no_num = 2; }
#line 9350 "def.tab.c"
    break;

  case 776: /* partition_member: '+' K_TOCOMPPIN $@142 T_STRING T_STRING risefallminmax2_list  */
#line 4315 "def.y"
      {
        if (defCallbacks->PartitionCbk)
          defData->Partition.setToCompPin((yyvsp[-2].string), (yyvsp[-1].string));
      }
#line 9359 "def.tab.c"
    break;

  case 777: /* $@143: %empty  */
#line 4319 "def.y"
                      {defData->dumb_mode=1; defData->no_num = 2; }
#line 9365 "def.tab.c"
    break;

  case 778: /* partition_member: '+' K_TOIOPIN $@143 T_STRING risefallminmax1_list  */
#line 4320 "def.y"
      {
        if (defCallbacks->PartitionCbk)
          defData->Partition.setToIOPin((yyvsp[-1].string));
      }
#line 9374 "def.tab.c"
    break;

  case 779: /* partition_member: extension_stmt  */
#line 4325 "def.y"
      { 
        if (defCallbacks->PartitionsExtCbk)
          CALLBACK(defCallbacks->PartitionsExtCbk, defrPartitionsExtCbkType,
                   &defData->History_text[0]);
      }
#line 9384 "def.tab.c"
    break;

  case 780: /* $@144: %empty  */
#line 4332 "def.y"
      { defData->dumb_mode = DEF_MAX_INT; defData->no_num = DEF_MAX_INT; }
#line 9390 "def.tab.c"
    break;

  case 781: /* minmaxpins: min_or_max_list K_PINS $@144 pin_list  */
#line 4333 "def.y"
      { defData->dumb_mode = 0; defData->no_num = 0; }
#line 9396 "def.tab.c"
    break;

  case 783: /* min_or_max_list: min_or_max_list min_or_max_member  */
#line 4337 "def.y"
      { }
#line 9402 "def.tab.c"
    break;

  case 784: /* min_or_max_member: K_MIN NUMBER NUMBER  */
#line 4340 "def.y"
      {
        if (defCallbacks->PartitionCbk)
          defData->Partition.setMin((yyvsp[-1].dval), (yyvsp[0].dval));
      }
#line 9411 "def.tab.c"
    break;

  case 785: /* min_or_max_member: K_MAX NUMBER NUMBER  */
#line 4345 "def.y"
      {
        if (defCallbacks->PartitionCbk)
          defData->Partition.setMax((yyvsp[-1].dval), (yyvsp[0].dval));
      }
#line 9420 "def.tab.c"
    break;

  case 787: /* pin_list: pin_list T_STRING  */
#line 4352 "def.y"
      { if (defCallbacks->PartitionCbk) defData->Partition.addPin((yyvsp[0].string)); }
#line 9426 "def.tab.c"
    break;

  case 790: /* risefallminmax1: K_RISEMIN NUMBER  */
#line 4358 "def.y"
      { if (defCallbacks->PartitionCbk) defData->Partition.addRiseMin((yyvsp[0].dval)); }
#line 9432 "def.tab.c"
    break;

  case 791: /* risefallminmax1: K_FALLMIN NUMBER  */
#line 4360 "def.y"
      { if (defCallbacks->PartitionCbk) defData->Partition.addFallMin((yyvsp[0].dval)); }
#line 9438 "def.tab.c"
    break;

  case 792: /* risefallminmax1: K_RISEMAX NUMBER  */
#line 4362 "def.y"
      { if (defCallbacks->PartitionCbk) defData->Partition.addRiseMax((yyvsp[0].dval)); }
#line 9444 "def.tab.c"
    break;

  case 793: /* risefallminmax1: K_FALLMAX NUMBER  */
#line 4364 "def.y"
      { if (defCallbacks->PartitionCbk) defData->Partition.addFallMax((yyvsp[0].dval)); }
#line 9450 "def.tab.c"
    break;

  case 796: /* risefallminmax2: K_RISEMIN NUMBER NUMBER  */
#line 4372 "def.y"
      { if (defCallbacks->PartitionCbk)
          defData->Partition.addRiseMinRange((yyvsp[-1].dval), (yyvsp[0].dval)); }
#line 9457 "def.tab.c"
    break;

  case 797: /* risefallminmax2: K_FALLMIN NUMBER NUMBER  */
#line 4375 "def.y"
      { if (defCallbacks->PartitionCbk)
          defData->Partition.addFallMinRange((yyvsp[-1].dval), (yyvsp[0].dval)); }
#line 9464 "def.tab.c"
    break;

  case 798: /* risefallminmax2: K_RISEMAX NUMBER NUMBER  */
#line 4378 "def.y"
      { if (defCallbacks->PartitionCbk)
          defData->Partition.addRiseMaxRange((yyvsp[-1].dval), (yyvsp[0].dval)); }
#line 9471 "def.tab.c"
    break;

  case 799: /* risefallminmax2: K_FALLMAX NUMBER NUMBER  */
#line 4381 "def.y"
      { if (defCallbacks->PartitionCbk)
          defData->Partition.addFallMaxRange((yyvsp[-1].dval), (yyvsp[0].dval)); }
#line 9478 "def.tab.c"
    break;

  case 800: /* partitions_end: K_END K_PARTITIONS  */
#line 4385 "def.y"
      { if (defCallbacks->PartitionsEndCbk)
          CALLBACK(defCallbacks->PartitionsEndCbk, defrPartitionsEndCbkType, 0); }
#line 9485 "def.tab.c"
    break;

  case 802: /* comp_names: comp_names comp_name  */
#line 4390 "def.y"
      { }
#line 9491 "def.tab.c"
    break;

  case 803: /* $@145: %empty  */
#line 4392 "def.y"
               {defData->dumb_mode=2; defData->no_num = 2; }
#line 9497 "def.tab.c"
    break;

  case 804: /* comp_name: '(' $@145 T_STRING T_STRING subnet_opt_syn ')'  */
#line 4394 "def.y"
      {
        // note that the defData->first T_STRING could be the keyword VPIN 
        if (defCallbacks->NetCbk)
          defData->Subnet->addPin((yyvsp[-3].string), (yyvsp[-2].string), (yyvsp[-1].integer));
      }
#line 9507 "def.tab.c"
    break;

  case 805: /* subnet_opt_syn: %empty  */
#line 4401 "def.y"
      { (yyval.integer) = 0; }
#line 9513 "def.tab.c"
    break;

  case 806: /* subnet_opt_syn: '+' K_SYNTHESIZED  */
#line 4403 "def.y"
      { (yyval.integer) = 1; }
#line 9519 "def.tab.c"
    break;

  case 809: /* $@146: %empty  */
#line 4409 "def.y"
      {  
        if (defCallbacks->NetCbk) defData->Subnet->addWire((yyvsp[0].string));
      }
#line 9527 "def.tab.c"
    break;

  case 810: /* subnet_option: subnet_type $@146 paths  */
#line 4413 "def.y"
      {  
        defData->by_is_keyword = FALSE;
        defData->do_is_keyword = FALSE;
        defData->new_is_keyword = FALSE;
        defData->step_is_keyword = FALSE;
        defData->orient_is_keyword = FALSE;
        defData->needNPCbk = 0;
      }
#line 9540 "def.tab.c"
    break;

  case 811: /* $@147: %empty  */
#line 4421 "def.y"
                         { defData->dumb_mode = 1; defData->no_num = 1; }
#line 9546 "def.tab.c"
    break;

  case 812: /* subnet_option: K_NONDEFAULTRULE $@147 T_STRING  */
#line 4422 "def.y"
      { if (defCallbacks->NetCbk) defData->Subnet->setNonDefault((yyvsp[0].string)); }
#line 9552 "def.tab.c"
    break;

  case 813: /* subnet_type: K_FIXED  */
#line 4425 "def.y"
      { (yyval.string) = (char*)"FIXED"; defData->dumb_mode = 1; }
#line 9558 "def.tab.c"
    break;

  case 814: /* subnet_type: K_COVER  */
#line 4427 "def.y"
      { (yyval.string) = (char*)"COVER"; defData->dumb_mode = 1; }
#line 9564 "def.tab.c"
    break;

  case 815: /* subnet_type: K_ROUTED  */
#line 4429 "def.y"
      { (yyval.string) = (char*)"ROUTED"; defData->dumb_mode = 1; }
#line 9570 "def.tab.c"
    break;

  case 816: /* subnet_type: K_NOSHIELD  */
#line 4431 "def.y"
      { (yyval.string) = (char*)"NOSHIELD"; defData->dumb_mode = 1; }
#line 9576 "def.tab.c"
    break;

  case 818: /* begin_pin_props: K_PINPROPERTIES NUMBER opt_semi  */
#line 4436 "def.y"
      { if (defCallbacks->PinPropStartCbk)
          CALLBACK(defCallbacks->PinPropStartCbk, defrPinPropStartCbkType, ROUND((yyvsp[-1].dval))); }
#line 9583 "def.tab.c"
    break;

  case 819: /* opt_semi: %empty  */
#line 4441 "def.y"
      { }
#line 9589 "def.tab.c"
    break;

  case 820: /* opt_semi: ';'  */
#line 4443 "def.y"
      { }
#line 9595 "def.tab.c"
    break;

  case 821: /* end_pin_props: K_END K_PINPROPERTIES  */
#line 4446 "def.y"
      { if (defCallbacks->PinPropEndCbk)
          CALLBACK(defCallbacks->PinPropEndCbk, defrPinPropEndCbkType, 0); }
#line 9602 "def.tab.c"
    break;

  case 824: /* $@148: %empty  */
#line 4453 "def.y"
                       { defData->dumb_mode = 2; defData->no_num = 2; }
#line 9608 "def.tab.c"
    break;

  case 825: /* $@149: %empty  */
#line 4454 "def.y"
      { if (defCallbacks->PinPropCbk) defData->PinProp.setName((yyvsp[-1].string), (yyvsp[0].string)); }
#line 9614 "def.tab.c"
    break;

  case 826: /* pin_prop_terminal: '-' $@148 T_STRING T_STRING $@149 pin_prop_options ';'  */
#line 4456 "def.y"
      { if (defCallbacks->PinPropCbk) {
          CALLBACK(defCallbacks->PinPropCbk, defrPinPropCbkType, &defData->PinProp);
         // reset the property number
         defData->PinProp.clear();
        }
      }
#line 9625 "def.tab.c"
    break;

  case 829: /* $@150: %empty  */
#line 4466 "def.y"
                         { defData->dumb_mode = DEF_MAX_INT; defData->parsing_property = 1; }
#line 9631 "def.tab.c"
    break;

  case 830: /* pin_prop: '+' K_PROPERTY $@150 pin_prop_name_value_list  */
#line 4468 "def.y"
      { defData->dumb_mode = 0; defData->parsing_property = 0; }
#line 9637 "def.tab.c"
    break;

  case 833: /* pin_prop_name_value: T_STRING NUMBER  */
#line 4475 "def.y"
      {
        if (defCallbacks->PinPropCbk) {
          char propTp;
          char* str = ringCopy("                       ");
          propTp = defSettings->CompPinProp.propType((yyvsp[-1].string));
          CHKPROPTYPE(propTp, (yyvsp[-1].string), "PINPROPERTIES");
          sprintf(str, "%g", (yyvsp[0].dval));
          defData->PinProp.addNumProperty((yyvsp[-1].string), (yyvsp[0].dval), str, propTp);
        }
      }
#line 9652 "def.tab.c"
    break;

  case 834: /* pin_prop_name_value: T_STRING QSTRING  */
#line 4486 "def.y"
      {
        if (defCallbacks->PinPropCbk) {
          char propTp;
          propTp = defSettings->CompPinProp.propType((yyvsp[-1].string));
          CHKPROPTYPE(propTp, (yyvsp[-1].string), "PINPROPERTIES");
          defData->PinProp.addProperty((yyvsp[-1].string), (yyvsp[0].string), propTp);
        }
      }
#line 9665 "def.tab.c"
    break;

  case 835: /* pin_prop_name_value: T_STRING T_STRING  */
#line 4495 "def.y"
      {
        if (defCallbacks->PinPropCbk) {
          char propTp;
          propTp = defSettings->CompPinProp.propType((yyvsp[-1].string));
          CHKPROPTYPE(propTp, (yyvsp[-1].string), "PINPROPERTIES");
          defData->PinProp.addProperty((yyvsp[-1].string), (yyvsp[0].string), propTp);
        }
      }
#line 9678 "def.tab.c"
    break;

  case 837: /* blockage_start: K_BLOCKAGES NUMBER ';'  */
#line 4507 "def.y"
      { if (defCallbacks->BlockageStartCbk)
          CALLBACK(defCallbacks->BlockageStartCbk, defrBlockageStartCbkType, ROUND((yyvsp[-1].dval))); }
#line 9685 "def.tab.c"
    break;

  case 838: /* blockage_end: K_END K_BLOCKAGES  */
#line 4511 "def.y"
      { if (defCallbacks->BlockageEndCbk)
          CALLBACK(defCallbacks->BlockageEndCbk, defrBlockageEndCbkType, 0); }
#line 9692 "def.tab.c"
    break;

  case 841: /* blockage_def: blockage_rule rectPoly_blockage rectPoly_blockage_rules ';'  */
#line 4520 "def.y"
      {
        if (defCallbacks->BlockageCbk) {
          CALLBACK(defCallbacks->BlockageCbk, defrBlockageCbkType, &defData->Blockage);
          defData->Blockage.clear();
        }
      }
#line 9703 "def.tab.c"
    break;

  case 842: /* $@151: %empty  */
#line 4527 "def.y"
                           { defData->dumb_mode = 1; defData->no_num = 1; }
#line 9709 "def.tab.c"
    break;

  case 843: /* $@152: %empty  */
#line 4528 "def.y"
      {
        if (defCallbacks->BlockageCbk) {
          if (defData->Blockage.hasPlacement() != 0) {
            if (defData->blockageWarnings++ < defSettings->BlockageWarnings) {
              defError(6539, "Invalid BLOCKAGE statement defined in the DEF file. The BLOCKAGE statment has both the LAYER and the PLACEMENT statements defined.\nUpdate your DEF file to have either BLOCKAGE or PLACEMENT statement only.");
              CHKERR();
            }
          }
          defData->Blockage.setLayer((yyvsp[0].string));
          defData->Blockage.clearPoly(); // free poly, if any
        }
        defData->hasBlkLayerComp = 0;
        defData->hasBlkLayerSpac = 0;
        defData->hasBlkLayerTypeComp = 0;
      }
#line 9729 "def.tab.c"
    break;

  case 845: /* $@153: %empty  */
#line 4547 "def.y"
      {
        if (defCallbacks->BlockageCbk) {
          if (defData->Blockage.hasLayer() != 0) {
            if (defData->blockageWarnings++ < defSettings->BlockageWarnings) {
              defError(6539, "Invalid BLOCKAGE statement defined in the DEF file. The BLOCKAGE statment has both the LAYER and the PLACEMENT statements defined.\nUpdate your DEF file to have either BLOCKAGE or PLACEMENT statement only.");
              CHKERR();
            }
          }
          defData->Blockage.setPlacement();
          defData->Blockage.clearPoly(); // free poly, if any
        }
        defData->hasBlkPlaceComp = 0;
        defData->hasBlkPlaceTypeComp = 0;
      }
#line 9748 "def.tab.c"
    break;

  case 849: /* layer_blockage_rule: '+' K_SPACING NUMBER  */
#line 4568 "def.y"
      {
        if (defData->VersionNum < 5.6) {
          if (defCallbacks->BlockageCbk) {
            if (defData->blockageWarnings++ < defSettings->BlockageWarnings) {
              defData->defMsg = (char*)defMalloc(1000);
              sprintf (defData->defMsg,
                 "The SPACING statement is available in version 5.6 and later.\nHowever, your DEF file is defined with version %g", defData->VersionNum);
              defError(6540, defData->defMsg);
              defFree(defData->defMsg);
              CHKERR();
            }
          }
        } else if (defData->hasBlkLayerSpac) {
          if (defCallbacks->BlockageCbk) {
            if (defData->blockageWarnings++ < defSettings->BlockageWarnings) {
              defError(6541, "The SPACING statement is defined in the LAYER statement,\nbut there is already either a SPACING statement or DESIGNRULEWIDTH  statement has defined in the LAYER statement.\nUpdate your DEF file to have either SPACING statement or a DESIGNRULEWIDTH statement.");
              CHKERR();
            }
          }
        } else {
          if (defCallbacks->BlockageCbk)
            defData->Blockage.setSpacing((int)(yyvsp[0].dval));
          defData->hasBlkLayerSpac = 1;
        }
      }
#line 9778 "def.tab.c"
    break;

  case 850: /* layer_blockage_rule: '+' K_DESIGNRULEWIDTH NUMBER  */
#line 4594 "def.y"
      {
        if (defData->VersionNum < 5.6) {
          if (defCallbacks->BlockageCbk) {
            if (defData->blockageWarnings++ < defSettings->BlockageWarnings) {
              defError(6541, "The SPACING statement is defined in the LAYER statement,\nbut there is already either a SPACING statement or DESIGNRULEWIDTH  statement has defined in the LAYER statement.\nUpdate your DEF file to have either SPACING statement or a DESIGNRULEWIDTH statement.");
              CHKERR();
            }
          }
        } else if (defData->hasBlkLayerSpac) {
          if (defCallbacks->BlockageCbk) {
            if (defData->blockageWarnings++ < defSettings->BlockageWarnings) {
              defError(6541, "The SPACING statement is defined in the LAYER statement,\nbut there is already either a SPACING statement or DESIGNRULEWIDTH  statement has defined in the LAYER statement.\nUpdate your DEF file to have either SPACING statement or a DESIGNRULEWIDTH statement.");
              CHKERR();
            }
          }
        } else {
          if (defCallbacks->BlockageCbk)
            defData->Blockage.setDesignRuleWidth((int)(yyvsp[0].dval));
          defData->hasBlkLayerSpac = 1;
        }
      }
#line 9804 "def.tab.c"
    break;

  case 853: /* mask_blockage_rule: '+' K_MASK NUMBER  */
#line 4620 "def.y"
      {      
        if (validateMaskInput((int)(yyvsp[0].dval), defData->blockageWarnings, defSettings->BlockageWarnings)) {
          defData->Blockage.setMask((int)(yyvsp[0].dval));
        }
      }
#line 9814 "def.tab.c"
    break;

  case 854: /* $@154: %empty  */
#line 4628 "def.y"
                      { defData->dumb_mode = 1; defData->no_num = 1; }
#line 9820 "def.tab.c"
    break;

  case 855: /* comp_blockage_rule: '+' K_COMPONENT $@154 T_STRING  */
#line 4629 "def.y"
      {
        if (defData->hasBlkLayerComp) {
          if (defCallbacks->BlockageCbk) {
            if (defData->blockageWarnings++ < defSettings->BlockageWarnings) {
              defError(6542, "The defined BLOCKAGES COMPONENT statement has either COMPONENT, SLOTS, FILLS, PUSHDOWN or EXCEPTPGNET defined.\nOnly one of these statements is allowed per LAYER. Updated the DEF file to define a valid BLOCKAGES COMPONENT statement per layer.");
              CHKERR();
            }
          }
        } else {
          if (defCallbacks->BlockageCbk) {
            defData->Blockage.setComponent((yyvsp[0].string));
          }
          if (defData->VersionNum < 5.8) {
            defData->hasBlkLayerComp = 1;
          }
        }
      }
#line 9842 "def.tab.c"
    break;

  case 856: /* comp_blockage_rule: '+' K_SLOTS  */
#line 4648 "def.y"
      {
        if (defData->hasBlkLayerComp || defData->hasBlkLayerTypeComp) {
          if (defCallbacks->BlockageCbk) {
            if (defData->blockageWarnings++ < defSettings->BlockageWarnings) {
              defError(6542, "The defined BLOCKAGES COMPONENT statement has either COMPONENT, SLOTS, FILLS, PUSHDOWN or EXCEPTPGNET defined.\nOnly one of these statements is allowed per LAYER. Updated the DEF file to define a valid BLOCKAGES COMPONENT statement per layer.");
              CHKERR();
            }
          }
        } else {
          if (defCallbacks->BlockageCbk) {
            defData->Blockage.setSlots();
          }
          if (defData->VersionNum < 5.8) {
            defData->hasBlkLayerComp = 1;
          }
          if (defData->VersionNum == 5.8) {
            defData->hasBlkLayerTypeComp = 1;
          }
        }
      }
#line 9867 "def.tab.c"
    break;

  case 857: /* comp_blockage_rule: '+' K_FILLS  */
#line 4669 "def.y"
      {
        if (defData->hasBlkLayerComp || defData->hasBlkLayerTypeComp) {
          if (defCallbacks->BlockageCbk) {
            if (defData->blockageWarnings++ < defSettings->BlockageWarnings) {
              defError(6542, "The defined BLOCKAGES COMPONENT statement has either COMPONENT, SLOTS, FILLS, PUSHDOWN or EXCEPTPGNET defined.\nOnly one of these statements is allowed per LAYER. Updated the DEF file to define a valid BLOCKAGES COMPONENT statement per layer.");
              CHKERR();
            }
          }
        } else {
          if (defCallbacks->BlockageCbk){
            defData->Blockage.setFills();
          }
          if (defData->VersionNum < 5.8) {
            defData->hasBlkLayerComp = 1;
          }
          if (defData->VersionNum == 5.8) {
            defData->hasBlkLayerTypeComp = 1;
          }
        }
      }
#line 9892 "def.tab.c"
    break;

  case 858: /* comp_blockage_rule: '+' K_PUSHDOWN  */
#line 4690 "def.y"
      {
        if (defData->hasBlkLayerComp) {
          if (defCallbacks->BlockageCbk) {
            if (defData->blockageWarnings++ < defSettings->BlockageWarnings) {
              defError(6542, "The defined BLOCKAGES COMPONENT statement has either COMPONENT, SLOTS, FILLS, PUSHDOWN or EXCEPTPGNET defined.\nOnly one of these statements is allowed per LAYER. Updated the DEF file to define a valid BLOCKAGES COMPONENT statement per layer.");
              CHKERR();
            }
          }
        } else {
          if (defCallbacks->BlockageCbk){
            defData->Blockage.setPushdown();
          }
          if (defData->VersionNum < 5.8) {
            defData->hasBlkLayerComp = 1;
          }
        }
      }
#line 9914 "def.tab.c"
    break;

  case 859: /* comp_blockage_rule: '+' K_EXCEPTPGNET  */
#line 4708 "def.y"
      {
        if (defData->VersionNum < 5.7) {
           if (defCallbacks->BlockageCbk) {
             if (defData->blockageWarnings++ < defSettings->BlockageWarnings) {
               defData->defMsg = (char*)defMalloc(10000);
               sprintf (defData->defMsg,
                 "The EXCEPTPGNET is available in version 5.7 or later.\nHowever, your DEF file is defined with version %g.", defData->VersionNum);
               defError(6549, defData->defMsg);
               defFree(defData->defMsg);
               CHKERR();
              }
           }
        } else {
           if (defData->hasBlkLayerComp) {
             if (defCallbacks->BlockageCbk) {
               if (defData->blockageWarnings++ < defSettings->BlockageWarnings) {
                 defError(6542, "The defined BLOCKAGES COMPONENT statement has either COMPONENT, SLOTS, FILLS, PUSHDOWN or EXCEPTPGNET defined.\nOnly one of these statements is allowed per LAYER. Updated the DEF file to define a valid BLOCKAGES COMPONENT statement per layer.");
                 CHKERR();
               }
             }
           } else {
             if (defCallbacks->BlockageCbk){
               defData->Blockage.setExceptpgnet();
             }
             if (defData->VersionNum < 5.8){
               defData->hasBlkLayerComp = 1;
             }
           }
        }
      }
#line 9949 "def.tab.c"
    break;

  case 862: /* $@155: %empty  */
#line 4745 "def.y"
                      { defData->dumb_mode = 1; defData->no_num = 1; }
#line 9955 "def.tab.c"
    break;

  case 863: /* placement_comp_rule: '+' K_COMPONENT $@155 T_STRING  */
#line 4746 "def.y"
      {
        if (defData->hasBlkPlaceComp) {
          if (defCallbacks->BlockageCbk) {
            if (defData->blockageWarnings++ < defSettings->BlockageWarnings) {
              defError(6543, "The defined BLOCKAGES PLACEMENT statement has either COMPONENT, PUSHDOWN, SOFT or PARTIAL defined.\nOnly one of these statements is allowed per LAYER. Updated the DEF file to define a valid BLOCKAGES PLACEMENT statement.");
              CHKERR();
            }
          }
        } else {
          if (defCallbacks->BlockageCbk){
            defData->Blockage.setComponent((yyvsp[0].string));
          }
          if (defData->VersionNum < 5.8) {
            defData->hasBlkPlaceComp = 1;
          }
        }
      }
#line 9977 "def.tab.c"
    break;

  case 864: /* placement_comp_rule: '+' K_PUSHDOWN  */
#line 4764 "def.y"
      {
        if (defData->hasBlkPlaceComp) {
          if (defCallbacks->BlockageCbk) {
            if (defData->blockageWarnings++ < defSettings->BlockageWarnings) {
              defError(6543, "The defined BLOCKAGES PLACEMENT statement has either COMPONENT, PUSHDOWN, SOFT or PARTIAL defined.\nOnly one of these statements is allowed per LAYER. Updated the DEF file to define a valid BLOCKAGES PLACEMENT statement.");
              CHKERR();
            }
          }
        } else {
          if (defCallbacks->BlockageCbk){
            defData->Blockage.setPushdown();
          }
          if (defData->VersionNum < 5.8) {
            defData->hasBlkPlaceComp = 1;
          }
        }
      }
#line 9999 "def.tab.c"
    break;

  case 865: /* placement_comp_rule: '+' K_SOFT  */
#line 4782 "def.y"
      {
        if (defData->VersionNum < 5.7) {
           if (defCallbacks->BlockageCbk) {
             if (defData->blockageWarnings++ < defSettings->BlockageWarnings) {
               defData->defMsg = (char*)defMalloc(10000);
               sprintf (defData->defMsg,
                 "The PLACEMENT SOFT is available in version 5.7 or later.\nHowever, your DEF file is defined with version %g.", defData->VersionNum);
               defError(6547, defData->defMsg);
               defFree(defData->defMsg);
               CHKERR();
             }
           }
        } else {
           if (defData->hasBlkPlaceComp || defData->hasBlkPlaceTypeComp) {
             if (defCallbacks->BlockageCbk) {
               if (defData->blockageWarnings++ < defSettings->BlockageWarnings) {
                 defError(6543, "The defined BLOCKAGES PLACEMENT statement has either COMPONENT, PUSHDOWN, SOFT or PARTIAL defined.\nOnly one of these statements is allowed per LAYER. Updated the DEF file to define a valid BLOCKAGES PLACEMENT statement.");
                 CHKERR();
               }
             }
           } else {
             if (defCallbacks->BlockageCbk){
               defData->Blockage.setSoft();
             }
             if (defData->VersionNum < 5.8) {
               defData->hasBlkPlaceComp = 1;
             }
             if (defData->VersionNum == 5.8) {
               defData->hasBlkPlaceTypeComp = 1;
             }
           }
        }
      }
#line 10037 "def.tab.c"
    break;

  case 866: /* placement_comp_rule: '+' K_PARTIAL NUMBER  */
#line 4816 "def.y"
      {
        if (defData->VersionNum < 5.7) {
           if (defCallbacks->BlockageCbk) {
             if (defData->blockageWarnings++ < defSettings->BlockageWarnings) {
                defData->defMsg = (char*)defMalloc(10000);
                sprintf (defData->defMsg,
                  "The PARTIAL is available in version 5.7 or later.\nHowever, your DEF file is defined with version %g.", defData->VersionNum);
                defError(6548, defData->defMsg);
                defFree(defData->defMsg);
                CHKERR();
             }
           }
        } else {
           if (defData->hasBlkPlaceComp || defData->hasBlkPlaceTypeComp) {
             if (defCallbacks->BlockageCbk) {
               if (defData->blockageWarnings++ < defSettings->BlockageWarnings) {
                 defError(6543, "The defined BLOCKAGES PLACEMENT statement has either COMPONENT, PUSHDOWN, SOFT or PARTIAL defined.\nOnly one of these statements is allowed per LAYER. Updated the DEF file to define a valid BLOCKAGES PLACEMENT statement.");
                 CHKERR();
               }
             }
           } else {
             if (defCallbacks->BlockageCbk){
               defData->Blockage.setPartial((yyvsp[0].dval));
             } 
             if (defData->VersionNum < 5.8) {
               defData->hasBlkPlaceComp = 1;
             }
             if (defData->VersionNum == 5.8) {
               defData->hasBlkPlaceTypeComp = 1;
             }
           }
         }
      }
#line 10075 "def.tab.c"
    break;

  case 869: /* rectPoly_blockage: K_RECT pt pt  */
#line 4856 "def.y"
      {
        if (defCallbacks->BlockageCbk)
          defData->Blockage.addRect((yyvsp[-1].pt).x, (yyvsp[-1].pt).y, (yyvsp[0].pt).x, (yyvsp[0].pt).y);
      }
#line 10084 "def.tab.c"
    break;

  case 870: /* $@156: %empty  */
#line 4861 "def.y"
      {
        if (defCallbacks->BlockageCbk) {
            defData->Geometries.Reset();
        }
      }
#line 10094 "def.tab.c"
    break;

  case 871: /* rectPoly_blockage: K_POLYGON $@156 firstPt nextPt nextPt otherPts  */
#line 4867 "def.y"
      {
        if (defCallbacks->BlockageCbk) {
          if (defData->VersionNum >= 5.6) {  // only 5.6 and beyond
            if (defData->Blockage.hasLayer()) {  // only in layer
              if (defCallbacks->BlockageCbk)
                defData->Blockage.addPolygon(&defData->Geometries);
            } else {
              if (defCallbacks->BlockageCbk) {
                if (defData->blockageWarnings++ < defSettings->BlockageWarnings) {
                  defError(6544, "A POLYGON statement is defined in the BLOCKAGE statement,\nbut it is not defined in the BLOCKAGE LAYER statement.\nUpdate your DEF file to either remove the POLYGON statement from the BLOCKAGE statement or\ndefine the POLYGON statement in a BLOCKAGE LAYER statement.");
                  CHKERR();
                }
              }
            }
          }
        }
      }
#line 10116 "def.tab.c"
    break;

  case 873: /* slot_start: K_SLOTS NUMBER ';'  */
#line 4889 "def.y"
      { if (defCallbacks->SlotStartCbk)
          CALLBACK(defCallbacks->SlotStartCbk, defrSlotStartCbkType, ROUND((yyvsp[-1].dval))); }
#line 10123 "def.tab.c"
    break;

  case 874: /* slot_end: K_END K_SLOTS  */
#line 4893 "def.y"
      { if (defCallbacks->SlotEndCbk)
          CALLBACK(defCallbacks->SlotEndCbk, defrSlotEndCbkType, 0); }
#line 10130 "def.tab.c"
    break;

  case 877: /* slot_def: slot_rule geom_slot_rules ';'  */
#line 4901 "def.y"
      {
        if (defCallbacks->SlotCbk) {
          CALLBACK(defCallbacks->SlotCbk, defrSlotCbkType, &defData->Slot);
          defData->Slot.clear();
        }
      }
#line 10141 "def.tab.c"
    break;

  case 878: /* $@157: %empty  */
#line 4908 "def.y"
                       { defData->dumb_mode = 1; defData->no_num = 1; }
#line 10147 "def.tab.c"
    break;

  case 879: /* $@158: %empty  */
#line 4909 "def.y"
      {
        if (defCallbacks->SlotCbk) {
          defData->Slot.setLayer((yyvsp[0].string));
          defData->Slot.clearPoly();     // free poly, if any
        }
      }
#line 10158 "def.tab.c"
    break;

  case 883: /* geom_slot: K_RECT pt pt  */
#line 4921 "def.y"
      {
        if (defCallbacks->SlotCbk)
          defData->Slot.addRect((yyvsp[-1].pt).x, (yyvsp[-1].pt).y, (yyvsp[0].pt).x, (yyvsp[0].pt).y);
      }
#line 10167 "def.tab.c"
    break;

  case 884: /* $@159: %empty  */
#line 4926 "def.y"
      {
          defData->Geometries.Reset();
      }
#line 10175 "def.tab.c"
    break;

  case 885: /* geom_slot: K_POLYGON $@159 firstPt nextPt nextPt otherPts  */
#line 4930 "def.y"
      {
        if (defData->VersionNum >= 5.6) {  // only 5.6 and beyond
          if (defCallbacks->SlotCbk)
            defData->Slot.addPolygon(&defData->Geometries);
        }
      }
#line 10186 "def.tab.c"
    break;

  case 887: /* fill_start: K_FILLS NUMBER ';'  */
#line 4941 "def.y"
      { if (defCallbacks->FillStartCbk)
          CALLBACK(defCallbacks->FillStartCbk, defrFillStartCbkType, ROUND((yyvsp[-1].dval))); }
#line 10193 "def.tab.c"
    break;

  case 888: /* fill_end: K_END K_FILLS  */
#line 4945 "def.y"
      { if (defCallbacks->FillEndCbk)
          CALLBACK(defCallbacks->FillEndCbk, defrFillEndCbkType, 0); }
#line 10200 "def.tab.c"
    break;

  case 891: /* fill_def: fill_rule geom_fill_rules ';'  */
#line 4953 "def.y"
      {
        if (defCallbacks->FillCbk) {
          CALLBACK(defCallbacks->FillCbk, defrFillCbkType, &defData->Fill);
          defData->Fill.clear();
        }
      }
#line 10211 "def.tab.c"
    break;

  case 892: /* $@160: %empty  */
#line 4959 "def.y"
                  { defData->dumb_mode = 1; defData->no_num = 1; }
#line 10217 "def.tab.c"
    break;

  case 893: /* $@161: %empty  */
#line 4960 "def.y"
      {
        if (defCallbacks->FillCbk) {
          defData->Fill.setVia((yyvsp[0].string));
          defData->Fill.clearPts();
          defData->Geometries.Reset();
        }
      }
#line 10229 "def.tab.c"
    break;

  case 895: /* $@162: %empty  */
#line 4969 "def.y"
                       { defData->dumb_mode = 1; defData->no_num = 1; }
#line 10235 "def.tab.c"
    break;

  case 896: /* $@163: %empty  */
#line 4970 "def.y"
      {
        if (defCallbacks->FillCbk) {
          defData->Fill.setLayer((yyvsp[0].string));
          defData->Fill.clearPoly();    // free poly, if any
        }
      }
#line 10246 "def.tab.c"
    break;

  case 900: /* geom_fill: K_RECT pt pt  */
#line 4983 "def.y"
      {
        if (defCallbacks->FillCbk)
          defData->Fill.addRect((yyvsp[-1].pt).x, (yyvsp[-1].pt).y, (yyvsp[0].pt).x, (yyvsp[0].pt).y);
      }
#line 10255 "def.tab.c"
    break;

  case 901: /* $@164: %empty  */
#line 4988 "def.y"
      {
        defData->Geometries.Reset();
      }
#line 10263 "def.tab.c"
    break;

  case 902: /* geom_fill: K_POLYGON $@164 firstPt nextPt nextPt otherPts  */
#line 4992 "def.y"
      {
        if (defData->VersionNum >= 5.6) {  // only 5.6 and beyond
          if (defCallbacks->FillCbk)
            defData->Fill.addPolygon(&defData->Geometries);
        } else {
            defData->defMsg = (char*)defMalloc(10000);
            sprintf (defData->defMsg,
              "POLYGON statement in FILLS LAYER is a version 5.6 and later syntax.\nYour def file is defined with version %g.", defData->VersionNum);
            defError(6564, defData->defMsg);
            defFree(defData->defMsg);
            CHKERR();
        }
      }
#line 10281 "def.tab.c"
    break;

  case 907: /* fill_layer_opc: '+' K_OPC  */
#line 5016 "def.y"
      {
        if (defData->VersionNum < 5.7) {
           if (defCallbacks->FillCbk) {
             if (defData->fillWarnings++ < defSettings->FillWarnings) {
               defData->defMsg = (char*)defMalloc(10000);
               sprintf (defData->defMsg,
                 "The LAYER OPC is available in version 5.7 or later.\nHowever, your DEF file is defined with version %g.", defData->VersionNum);
               defError(6553, defData->defMsg);
               defFree(defData->defMsg);
               CHKERR();
             }
           }
        } else {
           if (defCallbacks->FillCbk)
             defData->Fill.setLayerOpc();
        }
      }
#line 10303 "def.tab.c"
    break;

  case 908: /* fill_via_pt: firstPt otherPts  */
#line 5035 "def.y"
    {
        if (defCallbacks->FillCbk) {
          defData->Fill.addPts(&defData->Geometries);
          CALLBACK(defCallbacks->FillCbk, defrFillCbkType, &defData->Fill);
          defData->Fill.clear();
        }
    }
#line 10315 "def.tab.c"
    break;

  case 913: /* fill_via_opc: '+' K_OPC  */
#line 5054 "def.y"
      {
        if (defData->VersionNum < 5.7) {
           if (defCallbacks->FillCbk) {
             if (defData->fillWarnings++ < defSettings->FillWarnings) {
               defData->defMsg = (char*)defMalloc(10000);
               sprintf (defData->defMsg,
                 "The VIA OPC is available in version 5.7 or later.\nHowever, your DEF file is defined with version %g.", defData->VersionNum);
               defError(6554, defData->defMsg);
               defFree(defData->defMsg);
               CHKERR();
             }
           }
        } else {
           if (defCallbacks->FillCbk)
             defData->Fill.setViaOpc();
        }
      }
#line 10337 "def.tab.c"
    break;

  case 914: /* fill_mask: '+' K_MASK NUMBER  */
#line 5074 "def.y"
      { 
        if (validateMaskInput((int)(yyvsp[0].dval), defData->fillWarnings, defSettings->FillWarnings)) {
             if (defCallbacks->FillCbk) {
                defData->Fill.setMask((int)(yyvsp[0].dval));
             }
        }
      }
#line 10349 "def.tab.c"
    break;

  case 915: /* fill_viaMask: '+' K_MASK NUMBER  */
#line 5084 "def.y"
      { 
        if (validateMaskInput((int)(yyvsp[0].dval), defData->fillWarnings, defSettings->FillWarnings)) {
             if (defCallbacks->FillCbk) {
                defData->Fill.setMask((int)(yyvsp[0].dval));
             }
        }
      }
#line 10361 "def.tab.c"
    break;

  case 917: /* nondefault_start: K_NONDEFAULTRULES NUMBER ';'  */
#line 5097 "def.y"
      { 
        if (defData->VersionNum < 5.6) {
          if (defCallbacks->NonDefaultStartCbk) {
            if (defData->nonDefaultWarnings++ < defSettings->NonDefaultWarnings) {
              defData->defMsg = (char*)defMalloc(1000);
              sprintf (defData->defMsg,
                 "The NONDEFAULTRULE statement is available in version 5.6 and later.\nHowever, your DEF file is defined with version %g.", defData->VersionNum);
              defError(6545, defData->defMsg);
              defFree(defData->defMsg);
              CHKERR();
            }
          }
        } else if (defCallbacks->NonDefaultStartCbk)
          CALLBACK(defCallbacks->NonDefaultStartCbk, defrNonDefaultStartCbkType,
                   ROUND((yyvsp[-1].dval)));
      }
#line 10382 "def.tab.c"
    break;

  case 918: /* nondefault_end: K_END K_NONDEFAULTRULES  */
#line 5115 "def.y"
      { if (defCallbacks->NonDefaultEndCbk)
          CALLBACK(defCallbacks->NonDefaultEndCbk, defrNonDefaultEndCbkType, 0); }
#line 10389 "def.tab.c"
    break;

  case 921: /* $@165: %empty  */
#line 5122 "def.y"
                    { defData->dumb_mode = 1; defData->no_num = 1; }
#line 10395 "def.tab.c"
    break;

  case 922: /* $@166: %empty  */
#line 5123 "def.y"
      {
        if (defCallbacks->NonDefaultCbk) {
          defData->NonDefault.clear(); 
          defData->NonDefault.setName((yyvsp[0].string));
        }
      }
#line 10406 "def.tab.c"
    break;

  case 923: /* nondefault_def: '-' $@165 T_STRING $@166 nondefault_options ';'  */
#line 5130 "def.y"
      { if (defCallbacks->NonDefaultCbk)
          CALLBACK(defCallbacks->NonDefaultCbk, defrNonDefaultCbkType, &defData->NonDefault); }
#line 10413 "def.tab.c"
    break;

  case 926: /* nondefault_option: '+' K_HARDSPACING  */
#line 5138 "def.y"
      {
        if (defCallbacks->NonDefaultCbk)
          defData->NonDefault.setHardspacing();
      }
#line 10422 "def.tab.c"
    break;

  case 927: /* $@167: %empty  */
#line 5142 "def.y"
                    { defData->dumb_mode = 1; defData->no_num = 1; }
#line 10428 "def.tab.c"
    break;

  case 928: /* $@168: %empty  */
#line 5144 "def.y"
      {
        if (defCallbacks->NonDefaultCbk) {
          defData->NonDefault.addLayer((yyvsp[-2].string));
          defData->NonDefault.addWidth((yyvsp[0].dval));
        }
      }
#line 10439 "def.tab.c"
    break;

  case 930: /* $@169: %empty  */
#line 5151 "def.y"
                  { defData->dumb_mode = 1; defData->no_num = 1; }
#line 10445 "def.tab.c"
    break;

  case 931: /* nondefault_option: '+' K_VIA $@169 T_STRING  */
#line 5152 "def.y"
      {
        if (defCallbacks->NonDefaultCbk) {
          defData->NonDefault.addVia((yyvsp[0].string));
        }
      }
#line 10455 "def.tab.c"
    break;

  case 932: /* $@170: %empty  */
#line 5157 "def.y"
                      { defData->dumb_mode = 1; defData->no_num = 1; }
#line 10461 "def.tab.c"
    break;

  case 933: /* nondefault_option: '+' K_VIARULE $@170 T_STRING  */
#line 5158 "def.y"
      {
        if (defCallbacks->NonDefaultCbk) {
          defData->NonDefault.addViaRule((yyvsp[0].string));
        }
      }
#line 10471 "def.tab.c"
    break;

  case 934: /* $@171: %empty  */
#line 5163 "def.y"
                      { defData->dumb_mode = 1; defData->no_num = 1; }
#line 10477 "def.tab.c"
    break;

  case 935: /* nondefault_option: '+' K_MINCUTS $@171 T_STRING NUMBER  */
#line 5164 "def.y"
      {
        if (defCallbacks->NonDefaultCbk) {
          defData->NonDefault.addMinCuts((yyvsp[-1].string), (int)(yyvsp[0].dval));
        }
      }
#line 10487 "def.tab.c"
    break;

  case 939: /* nondefault_layer_option: K_DIAGWIDTH NUMBER  */
#line 5177 "def.y"
      {
        if (defCallbacks->NonDefaultCbk) {
          defData->NonDefault.addDiagWidth((yyvsp[0].dval));
        }
      }
#line 10497 "def.tab.c"
    break;

  case 940: /* nondefault_layer_option: K_SPACING NUMBER  */
#line 5183 "def.y"
      {
        if (defCallbacks->NonDefaultCbk) {
          defData->NonDefault.addSpacing((yyvsp[0].dval));
        }
      }
#line 10507 "def.tab.c"
    break;

  case 941: /* nondefault_layer_option: K_WIREEXT NUMBER  */
#line 5189 "def.y"
      {
        if (defCallbacks->NonDefaultCbk) {
          defData->NonDefault.addWireExt((yyvsp[0].dval));
        }
      }
#line 10517 "def.tab.c"
    break;

  case 942: /* $@172: %empty  */
#line 5196 "def.y"
                                    { defData->dumb_mode = DEF_MAX_INT; defData->parsing_property = 1; }
#line 10523 "def.tab.c"
    break;

  case 943: /* nondefault_prop_opt: '+' K_PROPERTY $@172 nondefault_prop_list  */
#line 5198 "def.y"
      { defData->dumb_mode = 0; defData->parsing_property = 0; }
#line 10529 "def.tab.c"
    break;

  case 946: /* nondefault_prop: T_STRING NUMBER  */
#line 5205 "def.y"
      {
        if (defCallbacks->NonDefaultCbk) {
          char propTp;
          char* str = ringCopy("                       ");
          propTp = defSettings->NDefProp.propType((yyvsp[-1].string));
          CHKPROPTYPE(propTp, (yyvsp[-1].string), "NONDEFAULTRULE");
          sprintf(str, "%g", (yyvsp[0].dval));
          defData->NonDefault.addNumProperty((yyvsp[-1].string), (yyvsp[0].dval), str, propTp);
        }
      }
#line 10544 "def.tab.c"
    break;

  case 947: /* nondefault_prop: T_STRING QSTRING  */
#line 5216 "def.y"
      {
        if (defCallbacks->NonDefaultCbk) {
          char propTp;
          propTp = defSettings->NDefProp.propType((yyvsp[-1].string));
          CHKPROPTYPE(propTp, (yyvsp[-1].string), "NONDEFAULTRULE");
          defData->NonDefault.addProperty((yyvsp[-1].string), (yyvsp[0].string), propTp);
        }
      }
#line 10557 "def.tab.c"
    break;

  case 948: /* nondefault_prop: T_STRING T_STRING  */
#line 5225 "def.y"
      {
        if (defCallbacks->NonDefaultCbk) {
          char propTp;
          propTp = defSettings->NDefProp.propType((yyvsp[-1].string));
          CHKPROPTYPE(propTp, (yyvsp[-1].string), "NONDEFAULTRULE");
          defData->NonDefault.addProperty((yyvsp[-1].string), (yyvsp[0].string), propTp);
        }
      }
#line 10570 "def.tab.c"
    break;

  case 950: /* styles_start: K_STYLES NUMBER ';'  */
#line 5238 "def.y"
      {
        if (defData->VersionNum < 5.6) {
          if (defCallbacks->StylesStartCbk) {
            if (defData->stylesWarnings++ < defSettings->StylesWarnings) {
              defData->defMsg = (char*)defMalloc(1000);
              sprintf (defData->defMsg,
                 "The STYLES statement is available in version 5.6 and later.\nHowever, your DEF file is defined with version %g", defData->VersionNum);
              defError(6546, defData->defMsg);
              defFree(defData->defMsg);
              CHKERR();
            }
          }
        } else if (defCallbacks->StylesStartCbk)
          CALLBACK(defCallbacks->StylesStartCbk, defrStylesStartCbkType, ROUND((yyvsp[-1].dval)));
      }
#line 10590 "def.tab.c"
    break;

  case 951: /* styles_end: K_END K_STYLES  */
#line 5255 "def.y"
      { if (defCallbacks->StylesEndCbk)
          CALLBACK(defCallbacks->StylesEndCbk, defrStylesEndCbkType, 0); }
#line 10597 "def.tab.c"
    break;

  case 954: /* $@173: %empty  */
#line 5263 "def.y"
      {
        if (defCallbacks->StylesCbk) defData->Styles.setStyle((int)(yyvsp[0].dval));
        defData->Geometries.Reset();
      }
#line 10606 "def.tab.c"
    break;

  case 955: /* styles_rule: '-' K_STYLE NUMBER $@173 firstPt nextPt otherPts ';'  */
#line 5268 "def.y"
      {
        if (defData->VersionNum >= 5.6) {  // only 5.6 and beyond will call the callback
          if (defCallbacks->StylesCbk) {
            defData->Styles.setPolygon(&defData->Geometries);
            CALLBACK(defCallbacks->StylesCbk, defrStylesCbkType, &defData->Styles);
          }
        }
      }
#line 10619 "def.tab.c"
    break;


#line 10623 "def.tab.c"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      yyerror (YY_("syntax error"));
    }

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;
  ++yynerrs;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  YY_ACCESSING_SYMBOL (yystate), yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturnlab;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturnlab;


/*-----------------------------------------------------------.
| yyexhaustedlab -- YYNOMEM (memory exhaustion) comes here.  |
`-----------------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;


/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif

  return yyresult;
}

#line 5278 "def.y"


END_LEFDEF_PARSER_NAMESPACE
