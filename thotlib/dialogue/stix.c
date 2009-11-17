/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2009
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
/*
 * Handles Stix fonts for MathML 
 * 
 * Authors: P. Cheyrou-lagreze (INRIA) I. Vatton (INRIA)
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "thot_gui.h"
#include "ustring.h"
#include "math.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "frame.h"
#include "appdialogue.h"
#include "registry.h"
#ifdef _GL
#include "openglfont.h"
#include "glwindowdisplay.h"
#else /*_GL*/
#include "windowdisplay_f.h"
#endif /*_GL*/

#include "font_f.h"
#include "stix_f.h"
#include "units_f.h"

#define THOT_EXPORT extern
#include "boxes_tv.h"
#include "font_tv.h"

#define LOW_HEIGHT 2
#define MID_HEIGHT 3
#define HIGH_HEIGHT 5

#ifdef _GL
#define LOW_CHAR 25
#define MID_CHAR 45
#define HIGH_CHAR 45
#else /*_GL*/
#define LOW_CHAR 25
#define MID_CHAR 45
#define HIGH_CHAR 45
#endif /*_GL*/

typedef struct _MapEntry {
  unsigned char MapFont;
  unsigned char MapIndex;
} MapEntry;

/* Mapping of Unicode symbols (0x3D0-0x3FF) to esstix fonts */
int Stix_Greek_Start = 0x3D0;
MapEntry     Stix_Greek [] = {
  /* 3Dx */ {11, 0x30}, {11, 0x77}, {11, 0x59}, {0, 0x00},
  {0, 0x00}, {9, 0x66}, {11, 0x36}, {0, 0x00},
  {0, 0x00}, {0, 0x00}, {0, 0x00}, {0, 0x00},
  {0, 0x00}, {0, 0x00}, {0, 0x00}, {0, 0x00},
  /* 3Ex */ {0, 0x00}, {0, 0x00}, {0, 0x00}, {0, 0x00},
  {0, 0x00}, {0, 0x00}, {0, 0x00}, {0, 0x00},
  {0, 0x00}, {0, 0x00}, {0, 0x00}, {0, 0x00},
  {0, 0x00}, {0, 0x00}, {0, 0x00}, {0, 0x00},
  /* 3Fx */ {11, 0x38}, {11, 0x39}, {0, 0x00}, {0, 0x00},
  {0, 0x00}, {9, 0x65}, {0, 0x00}, {0, 0x00},
  {0, 0x00}, {0, 0x00}, {0, 0x00}, {0, 0x00},
  {0, 0x00}, {0, 0x00}, {0, 0x00}, {0, 0x00}
};
#define Stix_Greek_length sizeof(Stix_Symbs) / sizeof(MapEntry)

/* Mapping of Unicode symbols (0x2100-0x214b) to esstix fonts */
int Stix_Symbs_Start = 0x2100;
MapEntry     Stix_Symbs [] = {
  /* 210x */ {0, 0x00}, {0, 0x00}, {14, 0x43}, {0, 0x00},
  {0, 0x00}, {0, 0x00}, {0, 0x00}, {13, 0x45},
  {0, 0x00}, {0, 0x00}, {13, 0x67}, {13, 0x48},
  {15, 0x48}, {14, 0x48}, {13, 0x68}, {5, 0x5a},
  /* 211x */ {13, 0x49}, {15, 0x49}, {13, 0x4c}, {13, 0x6c},
  {0, 0x00}, {14, 0x4e}, {0, 0x00}, {0, 0x00},
  {13, 0x50}, {14, 0x50}, {14, 0x51}, {13, 0x52},
  {15, 0x52}, {14, 0x52}, {0, 0x00}, {0, 0x00},
  /* 212x */ {0, 0x00}, {0, 0x00}, {0, 0x00}, {0, 0x00},
  {14, 0x5a}, {16, 0x42}, {10, 0x55}, {10, 0x5b},
  {15, 0x5a}, {10, 0x31}, {0, 0x00}, {0, 0x00},
  {13, 0x42}, {15, 0x43}, {13, 0x65}, {13, 0x65},
  /* 213x */ {13, 0x45}, {13, 0x46}, {0, 0x00}, {13, 0x4D},
  {13, 0x6f}, {14, 0x61}, {14, 0x62}, {14, 0x64},
  {14, 0x63}, {0, 0x00}, {0, 0x00}, {0, 0x00},
  {0, 0x00}, {0, 0x00}, {0, 0x00}, {0, 0x00},
  /* 214x */ {0, 0x00}, {0, 0x00}, {0, 0x00}, {0, 0x00},
  {0, 0x00}, {0, 0x00}, {0, 0x00}, {0, 0x00},
  {0, 0x00}, {0, 0x00}, {0, 0x00}, {0, 0x00}
};
#define Stix_Symbs_length sizeof(Stix_Symbs) / sizeof(MapEntry)

/* Mapping of Unicode arrows (0x2190-0x21ff) to esstix fonts */
int Stix_Arrows_Start = 0x2190;
MapEntry     Stix_Arrows [] = {
  /* 219x */ {1, 0x29}, {1, 0x5b}, {1, 0x2f}, {1, 0x59},
  {1, 0x34}, {1, 0x68}, {1, 0x5f}, {1, 0x62},
  {1, 0x61}, {1, 0x63}, {1, 0x46}, {1, 0x4b},
  {0, 0x00}, {1, 0x3d}, {1, 0x38}, {0, 0x00},
  /* 21Ax */ {1, 0x39}, {0, 0x00}, {1, 0x3b}, {1, 0x3c},
  {0, 0x00}, {0, 0x00}, {1, 0x31}, {0, 0x00},
  {0, 0x00}, {1, 0x26}, {1, 0x2d}, {1, 0x44},
  {1, 0x49}, {1, 0x2b}, {1, 0x4f}, {0, 0x00},
  /* 21Bx */ {1, 0x51}, {1, 0x6c}, {0, 0x00}, {1, 0x6d},
  {0, 0x00}, {0, 0x00}, {1, 0x66}, {1, 0x67},
  {0, 0x00}, {0, 0x00}, {1, 0x4d}, {1, 0x4e},
  {1, 0x28}, {1, 0x45}, {1, 0x5e}, {1, 0x5d},
  /* 21Cx */ {1, 0x2e}, {1, 0x48}, {1, 0x58}, {1, 0x57},
  {1, 0x25}, {1, 0x64}, {1, 0x24}, {1, 0x37},
  {1, 0x6a}, {1, 0x36}, {1, 0x6b}, {1, 0x21},
  {1, 0x23}, {1, 0x47}, {1, 0x50}, {1, 0x4c},
  /* 21Dx */ {1, 0x2a}, {1, 0x5c}, {1, 0x30}, {1, 0x5a},
  {1, 0x35}, {1, 0x69}, {0, 0x00}, {0, 0x00},
  {0, 0x00}, {0, 0x00}, {1, 0x33}, {1, 0x32},
  {0, 0x00}, {1, 0x2c}, {0, 0x00}, {0, 0x00},
  /* 21Ex */ {0, 0x00}, {0, 0x00}, {0, 0x00}, {0, 0x00},
  {0, 0x00}, {0, 0x00}, {0, 0x00}, {0, 0x00},
  {0, 0x00}, {0, 0x00}, {0, 0x00}, {0, 0x00},
  {0, 0x00}, {0, 0x00}, {0, 0x00}, {0, 0x00},
  /* 21Fx */ {0, 0x00}, {0, 0x00}, {0, 0x00}, {0, 0x00}
};
#define Stix_Arrows_length sizeof(Stix_Arrows) / sizeof(MapEntry)

/* Mapping of Unicode Mathematical Operators (0x2200-0x227f) to esstix fonts */
int Stix_MathOp1_Start = 0x2200;
MapEntry     Stix_MathOp1 [] = {
  /* 220x */ {2, 0x63}, {2, 0x66}, {9, 0x76}, {2, 0x64},
  {2, 0x65}, {5, 0x3a}, {10, 0x44}, {10, 0x56},
  {4, 0x32}, {4, 0x3b}, {0, 0x00}, {4, 0x48},
  {4, 0x4f}, {0, 0x00}, {5, 0x42}, {6, 0x2e},
  /* 221x */ {6, 0x2f}, {6, 0x2d}, {5, 0x4b}, {5, 0x48},
  {5, 0x46}, {0, 0x00}, {0, 0x00}, {4, 0x29}, /* use '/' and '\' */
  {4, 0x2b}, {4, 0x2c}, {6, 0x30}, {0, 0x00},
  {0, 0x00}, {3, 0x66}, {3, 0x4e}, {0, 0x00},
  /* 222x */ {3, 0x3a}, {3, 0x3b}, {2, 0xa3}, {1, 0x72},
  {1, 0xa7}, {1, 0x73}, {1, 0xa4}, {2, 0x6f},
  {2, 0x6e}, {2, 0x68}, {2, 0x67}, {6, 0x45},
  {0, 0x00}, {0, 0x00}, {6, 0x46}, {6, 0x48},
  /* 223x */ {6, 0x49}, {6, 0x4d}, {6, 0x4c}, {6, 0x4b},
  {4, 0x72}, {4, 0x71}, {0, 0x00}, {5, 0x54},
  {5, 0x4e}, {0, 0x00}, {5, 0x50}, {0, 0x00},
  {4, 0x77}, {4, 0xa2}, {4, 0x47}, {0, 0x00},
  /* 224x */ {5, 0x2c}, {4, 0xa7}, {3, 0x32}, {4, 0x78},
  {4, 0xa4}, {4, 0x79}, {4, 0x76}, {0, 0x00},
  {4, 0x7a}, {4, 0xab}, {4, 0x7c}, {4, 0xa1},
  {0, 0x00}, {4, 0x70}, {4, 0x6f}, {4, 0x6d},
  /* 225x */ {4, 0x5e}, {4, 0x5f}, {4, 0x6a}, {4, 0x69},
  {4, 0x64}, {4, 0x65}, {4, 0x63}, {4, 0x5d},
  {0, 0x00}, {4, 0x61}, {4, 0x6e}, {0, 0x00},
  {4, 0x62}, {0, 0x00}, {0, 0x00}, {4, 0x5c},
  /* 226x */ {4, 0x73}, {4, 0x68}, {4, 0x75}, {5, 0x63},
  {3, 0x25}, {3, 0x52}, {3, 0x26}, {3, 0x53},
  {3, 0x41}, {3, 0x6d}, {3, 0x2f}, {3, 0x5b},
  {3, 0x48}, {4, 0x74}, {3, 0x3e}, {3, 0x6a},
  /* 227x */ {3, 0x46}, {3, 0x72}, {3, 0x28}, {3, 0x54},
  {3, 0x44}, {3, 0x70}, {3, 0x2b}, {3, 0x57},
  {3, 0x45}, {3, 0x71}, {3, 0x33}, {3, 0x5f},
  {3, 0x37}, {3, 0x64}, {3, 0x34}, {3, 0x61}
};
#define Stix_MathOp1_length sizeof(Stix_MathOp1) / sizeof(MapEntry)

/* Mapping of Unicode Mathematical Operators (0x2280-0x22ff) to esstix fonts */
int Stix_MathOp2_Start = 0x2280;
MapEntry     Stix_MathOp2 [] = {
  /* 228x */ {3, 0x49}, {3, 0x74}, {4, 0x33}, {4, 0x49},
  {4, 0x3c}, {4, 0x50}, {4, 0x34}, {4, 0x4a},
  {4, 0x3f}, {4, 0x53}, {4, 0x3d}, {4, 0x51},
  {0, 0x00}, {0, 0x00}, {6, 0x5d}, {4, 0x37},
  /* 229x */ {4, 0x4d}, {4, 0x38}, {4, 0x4e}, {2, 0x6c},
  {2, 0x6b}, {5, 0x34}, {5, 0x32}, {5, 0x35},
  {5, 0x2f}, {5, 0x31}, {5, 0x30}, {5, 0x3b},
  {0, 0x00}, {5, 0x2e}, {5, 0x3f}, {5, 0x3e},
  /* 22ax */ {5, 0x40}, {0, 0x00}, {1, 0x77}, {1, 0x78},
  {1, 0x75}, {1, 0x74}, {0, 0x00}, {0, 0x00},
  {1, 0x7e}, {1, 0x79}, {1, 0x7c}, {1, 0x7a},
  {0, 0x00}, {0, 0x00}, {0, 0x00}, {0, 0x00},
  /* 22bx */ {1, 0xa1}, {0, 0x00}, {2, 0x7a}, {0, 0x00},
  {5, 0x55}, {5, 0x56}, {4, 0x46}, {4, 0x3a},
  {4, 0x39}, {5, 0x49}, {0, 0x00}, {2, 0x76},
  {2, 0x77}, {0, 0x00}, {3, 0x3c}, {3, 0x3d},
  /* 22cx */ {2, 0x6f}, {2, 0x6e}, {2, 0x68}, {2, 0x67},
  {2, 0x3e}, {4, 0x2c}, {4, 0x2d}, {5, 0x4a},
  {5, 0x29}, {5, 0x26}, {5, 0x28}, {5, 0x2b},
  {5, 0x2a}, {4, 0xa3}, {2, 0x75}, {2, 0x74},
  /* 22dx */ {4, 0x36}, {4, 0x4c}, {2, 0x6a}, {2, 0x69},
  {3, 0x69}, {0, 0x00}, {3, 0x31}, {3, 0x5d},
  {3, 0x30}, {3, 0x5c}, {3, 0x2c}, {3, 0x58},
  {0, 0x00}, {0, 0x00}, {3, 0x38}, {3, 0x65},
  /* 22ex */ {0, 0x00}, {0, 0x00}, {4, 0x44}, {4, 0x58},
  {4, 0x45}, {4, 0x59}, {3, 0x42}, {3, 0x6e},
  {3, 0x4a}, {3, 0x75}, {2, 0xa1}, {2, 0x7c},
  {5, 0x57}, {5, 0x58}, {1, 0xab}, {4, 0x2f},
  /* 22fx */ {4, 0x30}, {4, 0x31}, {0, 0x00}, {0, 0x00},
  {0, 0x00}, {0, 0x00}, {0, 0x00}, {0, 0x00},
  {0, 0x00}, {0, 0x00}, {0, 0x00}, {0, 0x00},
  {0, 0x00}, {0, 0x00}, {0, 0x00}, {0, 0x00},
  /* 230x */ {5, 0x3A}, {5, 0x2C}, {5, 0x6B}, {2, 0x6f},
  {2, 0x6E}, {2, 0x77}, {2, 0x79}, {0, 0x00},
  {7, 0x51}, {7, 0x53}, {7, 0x50}, {7, 0x52},
  {1, 0xA3}, {1, 0xB7}, {1, 0xA2}, {1, 0xB6},
  /* 231x */ {0, 0x00}, {0, 0x00}, {0, 0x00}, {0, 0x00},
  {0, 0x00}, {0, 0x00}, {0, 0x00}, {0, 0x00},
  {0, 0x00}, {0, 0x00}, {0, 0x00}, {0, 0x00},
  {0, 0x00}, {0, 0x00}, {0, 0x00}, {0, 0x00},
  /* 232x */ {0, 0x00}, {0, 0x00}, {17, 0x54}, {0, 0x00},
  {0, 0x00}, {0, 0x00}, {0, 0x00}, {0, 0x00},
  {0, 0x00}, {7, 0x43}, {7, 0x44}, {0, 0x00},
  {0, 0x00}, {0, 0x00}, {0, 0x00}, {0, 0x00}
};
#define Stix_MathOp2_length sizeof(Stix_MathOp2) / sizeof(MapEntry)

/* Mapping of Unicode Geometric Shapes (0x25a0-0x25ff) to esstix fonts */
int Stix_GeomShapes_Start = 0x25a0;
MapEntry     Stix_GeomShapes [] = {
  /* 25ax */ {2, 0x2d}, {2, 0x2c}, {0, 0x00}, {0, 0x00},
  {2, 0x44}, {2, 0x45}, {0, 0x00}, {2, 0x46},
  {2, 0x47}, {2, 0x51}, {0, 0x00}, {0, 0x00},
  {0, 0x00}, {2, 0x4c}, {0, 0x00}, {2, 0x4d},
  /* 25bx */ {0, 0x00}, {2, 0x4e}, {2, 0x3a}, {2, 0x36},
  {0, 0x00}, {0, 0x00}, {2, 0x3c}, {2, 0x38},
  {0, 0x00}, {0, 0x00}, {0, 0x00}, {0, 0x00},
  {2, 0x3b}, {2, 0x37}, {0, 0x00}, {0, 0x00},
  /* 25cx */ {2, 0x3d}, {2, 0x39}, {0, 0x00}, {0, 0x00},
  {0, 0x00}, {0, 0x00}, {2, 0x41}, {2, 0x3e},
  {0, 0x00}, {0, 0x00}, {0, 0x00}, {2, 0x42},
  {0, 0x00}, {0, 0x00}, {0, 0x00}, {2, 0x43},
  /* 25dx */ {2, 0x48}, {2, 0x49}, {2, 0x4b}, {2, 0x4a},
  {0, 0x00}, {0, 0x00}, {0, 0x00}, {0, 0x00},
  {0, 0x00}, {0, 0x00}, {0, 0x00}, {0, 0x00},
  {0, 0x00}, {0, 0x00}, {0, 0x00}, {0, 0x00},
  /* 25ex */ {0, 0x00}, {0, 0x00}, {0, 0x00}, {0, 0x00},
  {0, 0x00}, {0, 0x00}, {0, 0x00}, {2, 0x32},
  {2, 0x33}, {2, 0x2f}, {2, 0x31}, {0, 0x00},
  {0, 0x00}, {0, 0x00}, {0, 0x00}, {0, 0x00},
  /* 25fx */ {0, 0x00}, {0, 0x00}, {0, 0x00}, {0, 0x00},
  {0, 0x00}, {0, 0x00}, {0, 0x00}, {0, 0x00}
};
#define Stix_GeomShapes_length sizeof(Stix_GeomShapes) / sizeof(MapEntry)

/* Mapping of Unicode symbols (0xFE30-0xFE3F) to esstix fonts */
int Stix_OverBrace_Start = 0xFE30;
MapEntry     Stix_OverBrace [] = {
  /* FE3x */ {0, 0x00}, {0, 0x00}, {0, 0x00}, {0, 0x00},
  {0, 0x00}, {17, 0x54}, {17, 0x54}, {7, 0x55},
  {7, 0x55}, {0, 0x00}, {0, 0x00}, {0, 0x00},
  {0, 0x00}, {0, 0x00}, {0, 0x00}, {0, 0x00}
};
#define Stix_OverBrace_length sizeof(Stix_OverBrace) / sizeof(MapEntry)

/* Mapping of Unicode Script symbols (0x1D49C-0x1D4CF) */
int Stix_ScriptSymbols_Start = 0x1D49C;
MapEntry   Stix_ScriptSymbols [] = {
  /* 1D49C */ {13, 0x41}, {0, 0x00}, {13, 0x43}, {13, 0x44},
  /* 1D4Ax */ {0, 0x00}, {0, 0x00}, {13, 0x47}, {0, 0x00},
  {0, 0x00}, {13, 0x4a}, {13, 0x4b}, {0, 0x00},
  {0, 0x00}, {13, 0x4e}, {13, 0x4f}, {13, 0x50},
  {13, 0x51}, {0, 0x00}, {13, 0x53}, {13, 0x54},
  /* 1D4Bx */ {13, 0x55}, {13, 0x56}, {13, 0x57}, {13, 0x58},
  {13, 0x59}, {13, 0x5a}, {13, 0x61}, {13, 0x62},
  {13, 0x63}, {13, 0x64}, {0, 0x00}, {13, 0x66},
  {0, 0x00}, {13, 0x68}, {13, 0x69}, {13, 0x6a},
  /* 1D4Cx */ {13, 0x6b}, {13, 0x6c}, {13, 0x6d}, {13, 0x6e},
  {0, 0x00}, {13, 0x70}, {13, 0x71}, {13, 0x72},
  {13, 0x73}, {13, 0x74}, {13, 0x75}, {13, 0x76},
  {13, 0x77}, {13, 0x78}, {13, 0x79}, {13, 0x7a}
};
#define Stix_ScriptSymbols_length sizeof(Stix_ScriptSymbols) / sizeof(MapEntry)

/* Mapping of Unicode Fraktur and Double-struck symbols (0x1D504-0x1D56B) */
int Stix_DoubleStruck_Start = 0x1D504;
MapEntry   Stix_DoubleStruck [] = {
  /* 1D504 */ {15, 0x41}, {15, 0x42}, {0, 0x00}, {15, 0x44},
  {15, 0x45}, {15, 0x46}, {15, 0x47}, {0, 0x00},
  {0, 0x00}, {15, 0x4a}, {15, 0x4b}, {15, 0x4c},
  /* 1D51x */ {15, 0x4d}, {15, 0x4e}, {15, 0x4f}, {15, 0x50},
  {15, 0x51}, {0, 0x00}, {15, 0x53}, {15, 0x54},
  {15, 0x55}, {15, 0x56}, {15, 0x57}, {15, 0x58},
  {15, 0x59}, {0, 0x00}, {15, 0x61}, {15, 0x62},
  /* 1D52x */ {15, 0x63}, {15, 0x64}, {15, 0x65}, {15, 0x66},
  {15, 0x67}, {15, 0x68}, {15, 0x69}, {15, 0x6a},
  {15, 0x6b}, {15, 0x6c}, {15, 0x6d}, {15, 0x6e},
  {15, 0x6f}, {15, 0x70}, {15, 0x71}, {15, 0x72},
  /* 1D53x */ {15, 0x73}, {15, 0x74}, {15, 0x75}, {15, 0x76},
  {15, 0x77}, {15, 0x78}, {15, 0x79}, {15, 0x7a},
  {14, 0x41}, {14, 0x42}, {0, 0x00}, {14, 0x44},
  {14, 0x45}, {14, 0x46}, {14, 0x47}, {0, 0x00},
  /* 1D54x */ {14, 0x49}, {14, 0x4a}, {14, 0x4b}, {14, 0x4c},
  {14, 0x4d}, {0, 0x00}, {14, 0x4f}, {0, 0x00},
  {0, 0x00}, {0, 0x00}, {14, 0x53}, {14, 0x54},
  {14, 0x55}, {14, 0x56}, {14, 0x57}, {14, 0x58},
  /* 1D55x */ {14, 0x59}, {0, 0x00}
};
#define Stix_DoubleStruck_length sizeof(Stix_DoubleStruck) / sizeof(MapEntry)

/* Mapping of Unicode Double-struck digits (0x1D7D8-0x1D7E1) */
int Stix_DoubleDigits_Start = 0x1D7D8;
MapEntry   Stix_DoubleDigits [] = {
  /* 1D7D8 */ {14, 0x30}, {14, 0x31}, {14, 0x32}, {14, 0x33},
  {14, 0x34}, {14, 0x35}, {14, 0x36}, {14, 0x37},
  /* 1D7Ex */ {14, 0x38}, {14, 0x39}
};
#define Stix_DoubleDigits_length sizeof(Stix_DoubleDigits) / sizeof(MapEntry)

#ifndef _GL
/*----------------------------------------------------------------------
  DrawCompoundBraceStix
  Draw a big brace with several characters from the Esstix-eight font.
  ----------------------------------------------------------------------*/
static void DrawCompoundBraceStix (int frame, int x, int y, int l, int h,
                                   int size, int fg,
                                   int topChar, int middleChar, int bottomChar)
{
  int             xm, yf, yend, ym, second, hd, delta, fillChar;
  ThotFont        font;

  fillChar = 0x50;
  size = size + (size * ViewFrameTable[frame-1].FrMagnification / 10);
  size = PixelToPoint (size);
  font = (ThotFont)LoadStixFont (8, size);
  xm = x + ((l - CharacterWidth (topChar, font)) / 2);
  yf = y + CharacterAscent (topChar, font);
  hd = CharacterHeight (fillChar, font) - 2; /* 2 pixels overlap */
  if (h < (26 * hd) / 10)
    /* the brace is less than 2.8 times the heigth of a single character */
    /* use two half-braces and center them */
    {
      if (topChar == 0x4f)
        {  topChar = 0x48;  bottomChar = 0x47; }
      else
        {  topChar = 0x47;  bottomChar = 0x48; }
      yf += (h - (2 * hd)) / 2;
      DrawChar ((CHAR_T)topChar, frame, xm, yf, font, fg);
      DrawChar ((CHAR_T)bottomChar, frame, xm, yf+hd, font, fg);      
    }
  else if (h < 4 * hd)
    /* we can use three characters, but the two gaps are too narrow to
       fill each of them with a single fill character. Draw the three
       characters next to each other, centered in the box */
    {
      delta = (h - (3 * hd)) / 2;
      if (delta > 0)
        yf += delta;
      else
        delta -= 1;
      DrawChar ((CHAR_T)topChar, frame, xm, yf, font, fg);
      yf += hd;
      if (delta < 0)
        yf += delta;
      DrawChar ((CHAR_T)middleChar, frame, xm, yf, font, fg);
      yf += hd;
      if (delta < 0)
        yf += delta;
      DrawChar ((CHAR_T)bottomChar, frame, xm, yf, font, fg);
    }
  else
    /* use the total height */
    {
      DrawChar ((CHAR_T)topChar, frame, xm, yf, font, fg);
      yend = y + h - CharacterHeight (bottomChar, font) +
        CharacterAscent (bottomChar, font) - 1;
      DrawChar ((CHAR_T)bottomChar, frame, xm, yend, font, fg);
      ym = (yf + yend) / 2;
      DrawChar ((CHAR_T)middleChar, frame, xm, ym, font, fg);
      delta = (h - 3*hd) / 2;
      if (delta > 0 && hd > 0)
        /* there are two gaps between the top, middle and bottom characters */
        {
          if (delta <= hd)
            /* the gap is less than one character. Draw a single fill
               character centered in each gap */
            {
              DrawChar ((CHAR_T)fillChar, frame, xm, (ym + yf) / 2, font, fg);
              DrawChar ((CHAR_T)fillChar, frame, xm, (yend + ym) / 2, font, fg);
            }
          else
            /* draw several fill characters in each gap */
            {
              second = ym - yf;
              while (delta > 0)
                {
                  yf += hd;
                  DrawChar ((CHAR_T)fillChar, frame, xm, yf, font, fg);
                  DrawChar ((CHAR_T)fillChar, frame, xm, yf+second, font, fg);
                  delta -= hd;
                  ym -= hd;
                  if (delta > 0)
                    {
                      DrawChar ((CHAR_T)fillChar, frame, xm, ym, font, fg);
                      DrawChar ((CHAR_T)fillChar, frame, xm, ym+second+1, font, fg);
                      delta -= hd;
                    }
                }
            }
        }
    }
}
#endif

/*----------------------------------------------------------------------
  DrawCompoundExtendedStix
  Draw a big parenthesis, square bracket or integral sign with several
  characters from the Esstix-eight font.
  l < 0 means that the character must be aligned on the left side of its box.
  ----------------------------------------------------------------------*/
static void DrawCompoundExtendedStix (int frame, int x, int y, int l, int h,
                                      int size, int fg,
                                      int topChar, int fillChar, int bottomChar)
{
  int             xm, yf, yend, hd, delta;
  ThotFont        font;

  size = size + (size * ViewFrameTable[frame-1].FrMagnification / 10);
  size = PixelToPoint (size);
  font = (ThotFont)LoadStixFont (8, size);
  if (l < 0)
    xm = x;
  else
    xm = x + ((l - CharacterWidth (topChar, font)) / 2);
  yf = y + CharacterAscent (topChar, font);
  hd = CharacterHeight (fillChar, font) - 2; /* 2 pixels overlap */
  if (h > 2 * hd && h < (28 * hd) / 10)
    /* some space between the top and bottom characters, but not enough
       to display a single fill character. Draw only the top and bottom
       characters, next to each other, centered in the box */
    {
      yf += (h - (2 * hd)) / 2;
      DrawChar ((CHAR_T)topChar, frame, xm, yf, font, fg);
      DrawChar ((CHAR_T)bottomChar, frame, xm, yf+hd, font, fg);
    }
  else
    {
      DrawChar ((CHAR_T)topChar, frame, xm, yf, font, fg);
      yend = y + h - CharacterHeight (bottomChar, font) +
        CharacterAscent (bottomChar, font) - 1;
      DrawChar ((CHAR_T)bottomChar, frame, xm, yend, font, fg);
      delta = yend - yf - hd;
      if (delta > 0 && hd > 0)
        /* there is a gap between the top and bottom characters.
           fill it with some fill characters */
        {
          if (delta <= hd)
            /* the gap is less than one character. Draw a single fill
               character centered in the gap */
            DrawChar ((CHAR_T)fillChar, frame, xm, (yend + yf) / 2, font, fg);
          else
            /* draw several fill characters */
            {
              while (delta > 0)
                {
                  yf += hd;
                  DrawChar ((CHAR_T)fillChar, frame, xm, yf, font, fg);
                  delta -= hd;
                  yend -= hd;
                  if (delta > 0)
                    {
                      DrawChar ((CHAR_T)fillChar, frame, xm, yend, font, fg);
                      delta -= hd;
                    }
                }
            }
        }
    }
}

/*----------------------------------------------------------------------
  DrawCenteredStixChar draw a one glyph symbol centered in its box.
  parameter fg indicates the drawing color
  ----------------------------------------------------------------------*/
void DrawCenteredStixChar (ThotFont font, unsigned char symb, int x, int y, 
                           int l, int h, int fg, int frame)
{
  x = x + ((l - CharacterWidth ((char) symb, font)) / 2);
  y = y + ((h - CharacterHeight ((char)symb, font)) / 2)
	  + CharacterAscent ((char) symb, font);
  DrawChar ((CHAR_T) symb, frame, x, y, font, fg);
}

/*----------------------------------------------------------------------
  DrawStixChar draw a one glyph symbol vertically centered in its box.
  parameter fg indicates the drawing color
  ----------------------------------------------------------------------*/
static void DrawStixChar (ThotFont font, unsigned char symb, int x,
                          int y, int h, int fg, int frame)
{
  y = y + ((h - CharacterHeight ((char)symb, font)) / 2)
	  + CharacterAscent ((char) symb, font);
  DrawChar ((CHAR_T) symb, frame, x, y, font, fg);
}

/*----------------------------------------------------------------------
  DrawStixSigma
  ----------------------------------------------------------------------*/
void DrawStixSigma (int frame, int x, int y,  int l, int h, ThotFont font, int fg)
{
  if (fg < 0)
    return;

  /* Integrals using esstix6 charmap
     62 - => 3x text 3 line eq
     45 - => 2x text 2 line eq
     83 - => 1x text for one-line eq */

  if (h < LOW_CHAR)
    /* display a single glyph */
    DrawCenteredStixChar (font, 83, x, y, l, h, fg, frame);
  else if (h < MID_CHAR)
    /* display a single glyph */
    DrawCenteredStixChar (font, 45, x, y, l, h, fg, frame);
  else
    /* display a single glyph */
    DrawCenteredStixChar (font, 62, x, y, l, h, fg, frame);
}

/*----------------------------------------------------------------------
  DrawStixIntegral draws an integral. depending on type :
  - simple if type = 0
  - contour if type = 1
  - double if type = 2.
  parameter fg indicates the drawing color
  ----------------------------------------------------------------------*/
void DrawStixIntegral (int frame, int x, int y, int l, int h,
                       int type, int size, int fg)
{
  unsigned char   symb = 0x34;
  int             delta = 0;
  ThotFont        font;

  size = PixelToPoint (h);
  font = (ThotFont)LoadStixFont (6, size);
  if (CharacterHeight (0x21, font) > (3 * h) / 4 || type == 1 || type == 4 || type == 5)
    {
      /* Integrals using esstix6 charmap
         52 - => 3x text 3 line eq
         33 - => 2x text 2 line eq
         69 - => 1x text for oneline eq */
      /* display a single glyph */

      if (type == 0 || type == 2 || type == 3)
        {
          if (h < LOW_CHAR)
            symb = 0x45;
          else if (h < MID_CHAR)
            symb = 0x21;
          else
            symb = 0x34;
          DrawStixChar (font, symb, x, y, h, fg, frame);
        }
      if (type == 2 || type == 3)		
        /* double integral, display the second integral sign */
        {
          delta = CharacterWidth (symb, font) / 3;
          if (delta < 3)
            delta = 3;
          DrawStixChar (font, symb, x+delta, y, h, fg, frame);
        }
      if (type == 3)		
        /* triple integral, display the third integral sign */
        {
          DrawStixChar (font, symb, x+delta+delta, y, h, fg, frame);
        }

      if (type == 1)	
        /* contour integral */
        {
          if (h < LOW_CHAR)
            symb = 0x46;
          else if (h < MID_CHAR)
            symb = 0x23;
          else
            symb = 0x35;
          DrawStixChar (font, symb, x, y, h, fg, frame);
        }

      if (type == 4)	
        /* double contour integral */
        {
          if (h < LOW_CHAR)
            symb = 0x48;
          else if (h < MID_CHAR)
            symb = 0x25;
          else
            symb = 0x37;
          DrawStixChar (font, symb, x, y, h, fg, frame);
        }

      if (type == 5)	
        /* triple contour integral */
        {
          if (h < LOW_CHAR)
            symb = 0x49;
          else if (h < MID_CHAR)
            symb = 0x26;
          else
            symb = 0x38;
          DrawStixChar (font, symb, x, y, h, fg, frame);
        }
      if (type == 6)	
        /* Clockwise Integral */
        {
          if (h < LOW_CHAR)
            symb = 0x2B;
          else if (h < MID_CHAR)
            symb = 0x3C;
          else
            symb = 0x4D;
          DrawStixChar (font, symb, x, y, h, fg, frame);
        }
      if (type == 7)	
        /* Clockwise Contour Integral */
        {
          if (h < LOW_CHAR)
            symb = 0x2A;
          else if (h < MID_CHAR)
            symb = 0x3B;
          else
            symb = 0x4C;
          DrawStixChar (font, symb, x, y, h, fg, frame);
        }
      if (type == 8)	
        /* Counter Clockwise Contour integral */
        {
          if (h < LOW_CHAR)
            symb = 0x29;
          else if (h < MID_CHAR)
            symb = 0x3A;
          else
            symb = 0x4B;
          DrawStixChar (font, symb, x, y, h, fg, frame);
        }
    }
  else
    /* very high character. Display it with several components from the
       Esstix-8 font */
    {
      x -= l / 3;   /* in font Esstix these characters have too a wide margin*/
      DrawCompoundExtendedStix (frame, x, y, -1, h, size, fg,
                                0x61, 0x62, 0x63);
      if (type == 2 || type == 3)		
        /* double integral, display the second integral sign */
        DrawCompoundExtendedStix (frame, x+l/4, y, -1, h, size, fg,
                                  0x61, 0x62, 0x63);
      if (type == 3)		
        /* double integral, display the second integral sign */
        DrawCompoundExtendedStix (frame, x+l/2, y, -1, h, size, fg,
                                  0x61, 0x62, 0x63);
    }
}

/* ----------------------------------------------------------------------
   StixIntegralWidth
   ----------------------------------------------------------------------*/
static int StixIntegralWidth (int height, int type)
{
  int             i = 0, size;
  ThotFont        font;
  
  size = PixelToPoint (height);
  font = (ThotFont)LoadStixFont (6, size);  
  if (height < LOW_CHAR)
    {
      if (type == 0 || type == 2 || type == 3)
        {
          i = CharacterWidth (0x45, font);
          if (type == 2)
            i += i/4; /* double integral, drawn as 2 single integrals */
          if (type == 3)
            i += i/2; /* triple integral, drawn as 3 single integrals */
        }
      else
        {
          i = CharacterWidth (0x46, font);
          if (type == 4)
            i *= 2;
          if (type == 5)
            i *= 3;
        }
    }
  else if (height < MID_CHAR)
    {
      if (type == 0 || type == 2 || type == 3)
        {
          i = CharacterWidth (0x21, font);
          if (type == 4)
            i *= 2;
          if (type == 5)
            i *= 3;
        }
      else
        {
          i = CharacterWidth (0x23, font);
          if (type == 4)
            i += i/2;
          if (type == 5)
            i += i;
        }
    }
  else 
    {
      if (type == 0 || type == 2 || type == 3)
        {
          i = CharacterWidth (0x34, font);
          if (type == 2)
            i += i/4;
          if (type == 3)
            i += i/2;
        }
      else
        {
          i = CharacterWidth (0x35, font);
          if (type == 4)
            i += i/4;
          if (type == 5)
            i += i/2;
        }
    }
  return i;
}

/*----------------------------------------------------------------------
  DrawStixBracket draw an opening or closing bracket (depending on direction)
  parameter fg indicates the drawing color
  type : 0 = Bracket ; 1 = DoubleBracket ; 2 = Ceiling ; 3 = Floor
  ----------------------------------------------------------------------*/
void DrawStixBracket (int frame, int x, int y, int l, int h,
                      int direction, int type, int size, int fg)
{
  unsigned char   symb;
  ThotFont        font;

  size = PixelToPoint (l);
  font = (ThotFont)LoadStixFont (7, size);
  if (CharacterHeight (33, font) > (3 * h) / 4)
    {
      /*  write a single Esstix 7 character : normal, 2 line or 3 lines */
      if (h < LOW_CHAR )
        {
          if (direction == 0)
            {
            switch(type)
              {
              case 0 :symb = 63;break;
              case 1 :symb = 69;break;
              case 2 :symb = 81;break;
              default:symb = 80;break;
              }
            }
          else
            {
            switch(type)
              {
              case 0 :symb = 64;break;
              case 1 :symb = 70;break;
              case 2 :symb = 83;break;
              default:symb = 82;break;
              }
            }
        }
      else if (h < MID_CHAR)
        {
          if (direction == 0)
            {
            switch(type)
              {
              case 0 :symb = 36;break;
              case 1 :symb = 43;break;
              case 2 :symb = 81;break;
              default:symb = 80;break;
              }
            }
          else
            {
            switch(type)
              {
              case 0 :symb = 37;break;
              case 1 :symb = 44;break;
              case 2 :symb = 83;break;
              default:symb = 82;break;
              }
            }
        }
      else
        {
          if (direction == 0)
            {
            switch(type)
              {
              case 0 :symb = 50;break;
              case 1 :symb = 56;break;
              case 2 :symb = 81;break;
              default:symb = 80;break;
              }
            }
          else
            {
            switch(type)
              {
              case 0 :symb = 51;break;
              case 1 :symb = 57;break;
              case 2 :symb = 83;break;
              default:symb = 82;break;
              }
            }
        }
      DrawCenteredStixChar (font, symb, x, y, l, h, fg, frame);
    }
  else
    /* very high character. Display it with several components */
    if (direction == 0)
      { /* draw an opening bracket */
      switch(type)
        {
        case 0 :DrawCompoundExtendedStix (frame, x, y, l, h, size, fg, 0x49, 0x4a, 0x4b);break;
        case 1 :DrawCompoundExtendedStix (frame, x, y, l, h, size, fg, 0x49, 0x4a, 0x4b);break;
        case 2 :DrawCompoundExtendedStix (frame, x, y, l, h, size, fg, 0x49, 0x4a, 0x4b);break;
        default :DrawCompoundExtendedStix (frame, x, y, l, h, size, fg, 0x49, 0x4a, 0x4b);break;
        }
      }
    else
      { /* draw a closing bracket */
      switch(type)
        {
        case 0 :DrawCompoundExtendedStix (frame, x, y, l, h, size, fg, 0x4c, 0x4d, 0x4e);break;
        case 1 :DrawCompoundExtendedStix (frame, x, y, l, h, size, fg, 0x49, 0x4a, 0x4b);break;
        case 2 :DrawCompoundExtendedStix (frame, x, y, l, h, size, fg, 0x49, 0x4a, 0x4a);break;
        default :DrawCompoundExtendedStix (frame, x, y, l, h, size, fg, 0x4a, 0x4a, 0x4b);break;
        }
      }
}

/* ----------------------------------------------------------------------
   StixBracketWidth
   ----------------------------------------------------------------------*/
static int StixBracketWidth (int h, SpecFont font)
{
  int             i, size;
  ThotFont        pfont;

  GetFontAndIndexFromSpec (32, font, 1, &pfont);
  if (pfont && h <= (int) (1.3 * FontHeight (pfont)))
    /* use an ordinary parenthesis */
    i = CharacterWidth ('(', pfont);
  else
    {
      size = PixelToPoint (h);
      pfont = (ThotFont)LoadStixFont (7, size);
      if (h < LOW_HEIGHT)
        i = CharacterWidth (63, pfont);
      else if (h < MID_HEIGHT)
        i = CharacterWidth (36, pfont);
      else 
        i = CharacterWidth (50, pfont);
    }
  return i;
}

/*----------------------------------------------------------------------
  DrawStixPointyBracket draw an opening or closing pointy bracket (depending
  on direction)
  parameter fg indicates the drawing color
  ----------------------------------------------------------------------*/
void DrawStixPointyBracket (int frame, int x, int y, int l, int h,
                            int direction, int size, int fg)
{
  unsigned char   symb;
  ThotFont        font;

  if (fg < 0)
    return;

  size = PixelToPoint (l);
  font = (ThotFont)LoadStixFont (7, size);
  /*  write a single Esstix 7 character:
      61 normal
      33 2 line
      48 3 line  */
  if (h <  LOW_CHAR)
    {
      if (direction == 0)
        symb = 67;
      else
        symb = 68;
    }
  else if (h < MID_CHAR)
    {
      if (direction == 0)
        symb = 41;
      else
        symb = 42;
    }
  else
    {
      if (direction == 0)
        symb = 54;
      else
        symb = 55;
    }
  DrawCenteredStixChar (font, symb, x, y, l, h, fg, frame); 
}

/* ----------------------------------------------------------------------
   StixPointyBracketWidth
   ----------------------------------------------------------------------*/
static int StixPointyBracketWidth (int h)
{
  int             i, size;
  ThotFont        font;

  size = PixelToPoint (h);
  font = (ThotFont)LoadStixFont (7, size);  
  if (h < LOW_HEIGHT)
    i = CharacterWidth (67, font);
  else if (h < MID_HEIGHT)
    i = CharacterWidth (41, font);
  else 
    i = CharacterWidth (54, font);
  return i;
}

/*----------------------------------------------------------------------
  DrawStixParenthesis draw a closing or opening parenthesis (direction).
  parameter fg indicates the drawing color
  ----------------------------------------------------------------------*/
void DrawStixParenthesis (int frame, int x, int y, int l, int h,
                          int direction, int size, int fg)
{
  unsigned char   symb;
  ThotFont        font;

  size = PixelToPoint (l);
  font = (ThotFont)LoadStixFont (7, size);
  if (CharacterHeight (33, font) > (3 * h) / 4)
    {
      /*  write a single Esstix 7 character: 
          61 normal
          33 2 line
          48 3 line */
      if (h < LOW_CHAR)
        {
          if (direction == 0)
            symb = 61;
          else
            symb = 62;
        }
      else if (h < MID_CHAR)
        {
          if (direction == 0)
            symb = 33;
          else
            symb = 35;
        }
      else
        {
          if (direction == 0)
            symb = 48;
          else
            symb = 49;
        }
      DrawCenteredStixChar (font, symb, x, y, l, h, fg, frame);
    }
  else
    /* very high character. Display it with several components from the
       Esstix-8 font */
    if (direction == 0)
      /* draw an opening parenthesis */
      DrawCompoundExtendedStix (frame, x, y, l, h, size, fg, 0x5a, 0x5b, 0x5c);
    else
      /* draw a closing parenthesis */
      DrawCompoundExtendedStix (frame, x, y, l, h, size, fg, 0x5d, 0x5e, 0x5f);
}

/* ----------------------------------------------------------------------
   StixParenthesisWidth
   ----------------------------------------------------------------------*/
static int StixParenthesisWidth (int height, SpecFont font)
{
  int          i, size;
  ThotFont     pfont;

  GetFontAndIndexFromSpec (32, font, 1, &pfont);
  if (pfont && height <= (int) (1.3 * FontHeight (pfont)))
    /* use an ordinary parenthesis */
    i = CharacterWidth ('(', pfont);
  else
    {
      size = PixelToPoint (height);
      pfont = (ThotFont)LoadStixFont (7, size);  
      if (height < LOW_HEIGHT)
        i = CharacterWidth (61, pfont);
      else if (height < MID_HEIGHT)
        i = CharacterWidth (33, pfont);
      else 
        i = CharacterWidth (48, pfont);
    }
  return i;
}

/*----------------------------------------------------------------------
  DrawStixBrace draw an opening of closing brace (depending on direction).
  parameter fg indicates the drawing color
  ----------------------------------------------------------------------*/
void DrawStixBrace (int frame, int x, int y, int l, int h,
                    int direction, int size, int fg)
{
  unsigned char   symb;
  ThotFont        font;

#ifdef _GL
  size = PixelToPoint (l);
  font = (ThotFont)LoadStixFont (7, size);
  if (direction == 0)
    symb = 38;
  else
    symb = 40;
  DrawCenteredStixChar (font, symb, x, y, l, h, fg, frame);
#else /* _GL */
  font = (ThotFont)LoadStixFont (7, size);
  if (CharacterHeight (38, font) > (3 * h) / 4)
    {
      /*  write a single Esstix 7 character: 
          61 normal
          33 2 line
          48 3 line */
      if (h < LOW_CHAR)
        {
          if (direction == 0)
            symb = 65;
          else
            symb = 66;
        }
      else if (h < MID_CHAR)
        {
          if (direction == 0)
            symb = 38;
          else
            symb = 40;
        }
      else
        {
          if (direction == 0)
            symb = 52;
          else
            symb = 53;
        }
      DrawCenteredStixChar (font, symb, x, y, l, h, fg, frame);
    }
  else
    /* very high character. Display it with several components */
    if (direction == 0)
      /* draw an opening brace */
      DrawCompoundBraceStix (frame, x, y, l, h, size, fg, 0x4f, 0x51, 0x52);
    else
      /* draw a closing brace */
      DrawCompoundBraceStix (frame, x, y, l, h, size, fg, 0x53, 0x54, 0x55);
#endif /* _GL */
}

/* ----------------------------------------------------------------------
   StixBraceWidth
   ----------------------------------------------------------------------*/
static int StixBraceWidth (int height, SpecFont font)
{
  int             i, size;
  ThotFont        pfont;

  GetFontAndIndexFromSpec (32, font, 1, &pfont);
  if (pfont && height <= (int) (1.3 * FontHeight (pfont)))
    /* use an ordinary parenthesis */
    i = CharacterWidth ('(', pfont);
  else
    {
      size = PixelToPoint (height);
      pfont = (ThotFont)LoadStixFont (7, size);
      if (height < LOW_HEIGHT)
        i = CharacterWidth (65, pfont);
      else if (height < MID_HEIGHT)
        i = CharacterWidth (38, pfont);
      else 
        i = CharacterWidth (52, pfont);
    }
  return i;
}

/*----------------------------------------------------------------------
  DrawCompoundHorizBraceStix
  Draw a wide horizontal brace with several characters from the Esstix-eight
  font.
  ----------------------------------------------------------------------*/
static void DrawCompoundHorizBraceStix (int frame, int x, int y, int l, int h,
                                        int size, int fg,
                                        int leftChar, int middleChar, int rightChar)
{
  int             fillChar, baseline, lWidth, mWidth, rWidth, fWidth;
  int             xMiddleChar, gap, xf1, xf2;
  ThotFont        font;

  fillChar = 0x43;
  font = (ThotFont)LoadStixFont (8, size);
  /* in this Esstix font, the base line is the bottom of the straight part
     of the brace */
  if (leftChar == 0x3f)
    /* underbrace */
    baseline = y + CharacterAscent (leftChar, font);
  else
    /* overbrace */
    baseline = y + CharacterAscent (middleChar, font);

  lWidth = CharacterWidth (leftChar, font);
  mWidth = CharacterWidth (middleChar, font);
  rWidth = CharacterWidth (rightChar, font);
  fWidth = CharacterWidth (fillChar, font);
  DrawChar ((CHAR_T)leftChar, frame, x, baseline, font, fg);
  xMiddleChar = x + (l - mWidth) / 2;
  DrawChar ((CHAR_T)middleChar, frame, xMiddleChar, baseline, font, fg);
  DrawChar ((CHAR_T)rightChar, frame, x + l - rWidth, baseline, font, fg);

  gap = l - lWidth - mWidth - rWidth;
  if (gap > 0 && fWidth > 0)
    /* there are gaps between components. Draw fill characters between
       components */
    {
      gap = gap / 2;
      if (gap <= fWidth)
        /* the gap is less than onefill character. Draw a single fill
           character centered in each gap */
        {
          DrawChar ((CHAR_T)fillChar, frame, x+lWidth-((fWidth-gap)*3/4), baseline,
                    font, fg);
          DrawChar ((CHAR_T)fillChar, frame, xMiddleChar+mWidth-(fWidth-gap)/4,
                    baseline, font, fg);
        }
      else
        /* draw several fill characters in each gap, starting from the
           middle character */
        {
          xf1 = xMiddleChar - fWidth;
          xf2 = xMiddleChar + mWidth;
          do
            {
              DrawChar ((CHAR_T)fillChar, frame, xf1, baseline, font, fg);
              DrawChar ((CHAR_T)fillChar, frame, xf2, baseline, font, fg);
              gap -= fWidth;
              if (gap > 0)
                {
                  xf1 -= fWidth;
                  xf2 += fWidth;
                }
            }
          while (gap > 0);
        }
    }
}

/*----------------------------------------------------------------------
  DrawStixHorizontalBrace draws an overbrace or an underbrace (depending on
  direction).
  parameter fg indicates the drawing color
  ----------------------------------------------------------------------*/
void DrawStixHorizontalBrace (int frame, int x, int y, int l, int h,
                              int direction, int size, int fg)
{
  unsigned char   symb;
  ThotFont        font;

  size = size + (size * ViewFrameTable[frame-1].FrMagnification / 10);
  if (size <= 0)
    size = 1;
  font = (ThotFont)LoadStixFont (7, size);
  if (l < 2 * CharacterWidth (0x54, font))
    /*  write a single Esstix 7 character */
    {
      if (direction == 0)
        symb = 0x55;
      else
        symb = 0x54;
      DrawCenteredStixChar (font, symb, x, y, l, h, fg, frame);
    }
  else
    /* wide horizontal brace. Display it from several components */
    if (direction == 0)
      /* draw an overbrace */
      DrawCompoundHorizBraceStix (frame, x, y, l, h, size, fg, 0x44, 0x45, 0x46);
    else
      /* draw an underbrace */
      DrawCompoundHorizBraceStix (frame, x, y, l, h, size, fg, 0x3f, 0x40, 0x41);

}

/*----------------------------------------------------------------------
  DrawCompoundHorizArrow
  Draw a wide horizontal arrow with several characters from the Esstix-eight
  font.
  ----------------------------------------------------------------------*/
static void DrawCompoundHorizArrow (int frame, int x, int y, int l, int h,
                                        int size, int fg,
                                        int leftChar, int middleChar,int rightChar)
{
  int             baseline, lWidth, mWidth, rWidth;
  int             gap, xf1, xf2;
  ThotFont        font;

  font = (ThotFont)LoadStixFont (8, size);
  /* in this Esstix font, the base line is the bottom of the straight part */
  xf1 = CharacterAscent (leftChar, font);
  xf2 = CharacterAscent (rightChar, font);
  if (xf1 > xf2)
    baseline = y + xf1;
  else
     baseline = y + xf2;
  lWidth = CharacterWidth (leftChar, font);
  mWidth = CharacterWidth (middleChar, font);
  rWidth = CharacterWidth (rightChar, font);
  DrawChar ((CHAR_T)leftChar, frame, x, baseline, font, fg);
  DrawChar ((CHAR_T)rightChar, frame, x + l - rWidth, baseline, font, fg);

  gap = l - lWidth - rWidth;
  xf1 = x + lWidth;
  if (gap > 0 && mWidth > 0)
    /* there are gaps between components. Draw fill characters between components */
    {
      while (gap > 0)
        {
          if (gap < mWidth/2)
            /* the gap is less than onefill character. Draw a single fill
               character centered in each gap */
            DrawChar ((CHAR_T)middleChar, frame, xf1-(gap/2), baseline,
                      font, fg);
          else
            DrawChar ((CHAR_T)middleChar, frame, xf1, baseline, font, fg);
          gap -= mWidth;
          xf1 += mWidth;
        }
    }
}

/*----------------------------------------------------------------------
  DrawStixHorizArrow draws a horizontal arrow according to the type.
  type : 0 = Arrow, 1 = Arrow with opposite directions, 2 = DoubleArrow,
         3 = DoubleArrow with opposite directions
         4 = two arrows with opposite directions, 5 = TeeArrow, 6 = ArrowBar
         7 = Vector,  8 = Vector with opposite directions, 9 = TeeVector,
         10 = VectorBar, 
         11 = two vectors with opposite directions = Equilibrium
         12 = ReverseVector, 13 = ReverseVector with opposite directions,
         14 = TeeReverseVector, 15 = ReverseVectorBar
  parameter fg indicates the drawing color
  ----------------------------------------------------------------------*/
void DrawStixHorizArrow (int frame, int x, int y, int l, int h,
                         int type, int size, int fg)
{
  unsigned char   symb;
  ThotFont        font;

  size = size + (size * ViewFrameTable[frame-1].FrMagnification / 10);
  if (size <= 0)
    size = 1;
  font = (ThotFont)LoadStixFont (8, size);
  if (l <= CharacterWidth (0x25, font))
    /*  write a single Esstix 7 character */
    {
      if (type == 0)
        symb = 0x25;
      else
        symb = 0x21;
      DrawCenteredStixChar (font, symb, x, y, l, h, fg, frame);
    }
  else
    /* wide horizontal arrow. Display it from several components */
    if (type == 0)
      /* arrow to the right */
      DrawCompoundHorizArrow (frame, x, y, l, h, size, fg, 0x23, 0x23, 0x25);
    else
      /* arrow to the left */
      DrawCompoundHorizArrow (frame, x, y, l, h, size, fg, 0x21, 0x23, 0x23);

}

/*----------------------------------------------------------------------
  GetMathFontWidth : Calculates the width of the stix char
  ----------------------------------------------------------------------*/
int GetMathFontWidth (char shape, SpecFont font, int height)
{
  int           i;

  i = 0;
  if (StixExist && height > 0) 
    {
      switch (shape)
        {
        case '1':	/* Clockwise Integral */
          i = StixIntegralWidth (height, 6);
          break;
        case '2':	/* Clockwise Contour Integral */
          i = StixIntegralWidth (height, 7);
          break;
        case '3':	/* Counter Clockwise Contour Integral */
          i = StixIntegralWidth (height, 8);
          break;
        case 'd':	/* double integral */
          i = StixIntegralWidth (height, 2);
          break;
        case 'e':	/* double contour integral */
          i = StixIntegralWidth (height, 4);
          break;
        case 'f':	/* triple contour integral */
          i = StixIntegralWidth (height, 5);
          break;
        case 'i':	/* integral */
          i = StixIntegralWidth (height, 0);
          break;
        case 'c':	/* circle integral */
          i = StixIntegralWidth (height, 1);
          break;
        case 't':	/* triple integral */
          i = StixIntegralWidth (height, 3);
          break;
        case '(':
        case ')':
          i = StixParenthesisWidth (height, font);
          break;
        case '{':
        case '}':
          i = StixBraceWidth (height, font);
          break;
        case '[':
        case ']':
        case 'o':       /* overbrace */
        case 'u':       /* underbrace */
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
          i = StixBracketWidth (height, font);	
          break;
        case '<':
        case '>':
          i = StixPointyBracketWidth (height);	
          break;
        default:
          break;
        }
    }
  return i;
}

/*----------------------------------------------------------------------
  GiveStixSize gives the internal size of a symbol box.
  ----------------------------------------------------------------------*/
void GiveStixSize (ThotFont pfont, PtrAbstractBox pAb, int *width,
                   int *height, int size)
{
  int                 hfont;

  hfont = FontHeight (pfont);
  //*height = hfont * 2;
  *height = hfont;
  switch (pAb->AbShape)
    {
    case '1':	/* Clockwise Integral */
      *width = StixIntegralWidth (*height, 6);
      break;
    case '2':	/* Clockwise Contour Integral */
      *width = StixIntegralWidth (*height, 7);
      break;
    case '3':	/* Counter Clockwise Contour Integral */
      *width = StixIntegralWidth (*height, 8);
      break;
    case 'd':	/* double integral */
      *width = StixIntegralWidth (*height, 2);
      break;
    case 'e':	/* double contour integral */
      *width = StixIntegralWidth (*height, 4);
      break;
    case 'f':	/* triple contour integral */
      *width = StixIntegralWidth (*height, 5);
      break;
    case 'i':	/* integral */
      *width = StixIntegralWidth (*height, 0);
      break;
    case 'c':	/* circle integral */
      *width = StixIntegralWidth (*height, 1);
      break;
    case 't':	/* circle integral */
      *width = StixIntegralWidth (*height, 3);
      break;
    case '(':
    case ')':
      *width = StixParenthesisWidth (*height, pAb->AbBox->BxFont); 
      break;
    case '{':
    case '}':
      *width = StixBraceWidth (*height, pAb->AbBox->BxFont);
      break;
    case '[':
    case ']':
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
      *width = StixBracketWidth (*height, pAb->AbBox->BxFont);	
      break;
    case 'L':
    case 'R':
      *width = CharacterWidth (0x25, pfont);
      *height = CharacterHeight (0x25, pfont);
      break;
    case '<':
    case '>':
      *width = StixPointyBracketWidth (*height);	
      *width = *width + *width/2;
      break;
    case 'o':       /* overbrace */
    case 'u':       /* underbrace */
      *width = CharacterWidth  (0x43, pfont);
      *height = CharacterHeight (0x40, pfont) - CharacterAscent (0x40, pfont) +
        CharacterAscent (0x41, pfont);
      break;
#ifdef o
    case 'I':	/* intersection */
    case 'U':	/* union */
      *width = BoxCharacterWidth (0xe5, pfont);
      break;
#endif
    default:
      // like an integral
      *width = StixIntegralWidth (*height, 0);
      break;
    }
}


/*----------------------------------------------------------------------
  GetMathFontFromChar
  The size values must be expressed in points
  ----------------------------------------------------------------------*/
void GetMathFontFromChar (char typesymb, SpecFont fontset, void **font,
                          int size)
{
  switch (typesymb)
    {
      /*integral, union...*/
    case '1':
    case '2':
    case '3':
    case 'i':
    case 'c':
    case 'd':	  
    case 'e':	  
    case 'f':	  
    case 't':	  
    case 'I':
    case 'U':
      *font =  LoadStixFont (6, size);
      break;
    case '(':
    case ')':
    case '{':
    case '}':
    case '[':
    case ']':
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
      if (size > 2)
        *font = LoadStixFont (7, size);
      break;
    case '<':
    case '>':
      *font = LoadStixFont (7, size);
      break;
    case 'L':
    case 'R':
    case 'o':
    case 'u':
      *font = LoadStixFont (8, size);
      break;
    default:
#ifdef _GL
      GetStixFontAndIndex ((int)typesymb, fontset, (ThotFont **)font);
#else /* _GL */
      *font = NULL;	  
#endif /* _GL */
      return;
      break;
    }
}

/*----------------------------------------------------------------------
  GetStixFontAndIndex returns the glyph index and the font
  used to display the wide character c
  ----------------------------------------------------------------------*/
int GetStixFontAndIndex (int c, SpecFont fontset, ThotFont **font)
{
  MapEntry           entry;
  int                index = 0, face = 0;
#ifdef _GL
  ThotFont           lfont;
  int                frame;
  unsigned int       mask;
#endif /* _GL */
  if (c == 0x2970) /* roundimplies */
    {
     index = (int) 0x49;
     face = (int) 4;
    }
  else if (c == 0x2720) /* maltese cross */
    {
     index = (int) 0x21;
     face = (int) 2;
    }
  else if (c == 0x260E) /* phone */
    {
     index = (int) 0x54;
     face = (int) 2;
    }
  else if (c == 0x20DB || c == 0x20DC) /* dots */
    {
     index = (int) (c - 0x20DB + 0x61);
     face = (int) 17;
    }
  else if (c >= Stix_Greek_Start && c < (int) (Stix_Greek_Start + Stix_Greek_length))
    {
      entry = Stix_Greek[c - Stix_Greek_Start];
      index = (int) (entry.MapIndex);
      face = (int) (entry.MapFont);
    }
  else if (c >= Stix_Symbs_Start && c < (int) (Stix_Symbs_Start + Stix_Symbs_length))
    {
      entry = Stix_Symbs[c - Stix_Symbs_Start];
      index = (int) (entry.MapIndex);
      face = (int) (entry.MapFont);
    }
  else if (c >= Stix_Arrows_Start && c < (int) (Stix_Arrows_Start + Stix_Arrows_length))
    {
      entry = Stix_Arrows[c - Stix_Arrows_Start];
      index = (int) (entry.MapIndex);
      face = (int) (entry.MapFont);
    }
  else if (c >= Stix_MathOp1_Start && c < (int) (Stix_MathOp1_Start + Stix_MathOp1_length))
    {
      entry = Stix_MathOp1[c - Stix_MathOp1_Start];
      index = (int) (entry.MapIndex);
      face = (int) (entry.MapFont);
    }
  else if (c >= Stix_MathOp2_Start && c < (int) (Stix_MathOp2_Start + Stix_MathOp2_length))
    {
      entry = Stix_MathOp2[c - Stix_MathOp2_Start];
      index = (int) (entry.MapIndex);
      face = (int) (entry.MapFont);
    }
  else if (c >= Stix_GeomShapes_Start && c < (int) (Stix_GeomShapes_Start + Stix_GeomShapes_length))
    {
      entry = Stix_GeomShapes[c - Stix_GeomShapes_Start];
      index = (int) (entry.MapIndex);
      face = (int) (entry.MapFont);
    }
  else if (c >= Stix_OverBrace_Start && c < (int) (Stix_OverBrace_Start + Stix_OverBrace_length))
    {
      entry = Stix_OverBrace[c - Stix_OverBrace_Start];
      index = (int) (entry.MapIndex);
      face = (int) (entry.MapFont);
    }
  else if (c == 0x27fa) /* Long left right double arrow */
    /* use the short arrow instead */
    {
      entry = Stix_Arrows[0x21d4 - Stix_Arrows_Start];
      index = (int) (entry.MapIndex);
      face = (int) (entry.MapFont);
    }
  else if (c >= Stix_ScriptSymbols_Start && c < (int) (Stix_ScriptSymbols_Start + Stix_ScriptSymbols_length))
    {
      entry = Stix_ScriptSymbols[c - Stix_ScriptSymbols_Start];
      index = (int) (entry.MapIndex);
      face = (int) (entry.MapFont);
    }
  else if (c >= Stix_DoubleStruck_Start && c < (int) (Stix_DoubleStruck_Start + Stix_DoubleStruck_length))
    {
      entry = Stix_DoubleStruck[c - Stix_DoubleStruck_Start];
      index = (int) (entry.MapIndex);
      face = (int) (entry.MapFont);
    }
  else if (c >= Stix_DoubleDigits_Start && c < (int) (Stix_DoubleDigits_Start + Stix_DoubleDigits_length))
    {
      entry = Stix_DoubleDigits[c - Stix_DoubleDigits_Start];
      index = (int) (entry.MapIndex);
      face = (int) (entry.MapFont);
    }

  if (face == 1)
    *font = &(fontset->SFont_1);
  else if (face == 2)
    *font = &(fontset->SFont_2);
  else if (face == 3)
    *font = &(fontset->SFont_3);
  else if (face == 4)
    *font = &(fontset->SFont_4);
  else if (face == 5)
    *font = &(fontset->SFont_5);
  else if (face == 6)
    *font = &(fontset->SFont_6);
  else if (face == 7)
    *font = &(fontset->SFont_7);
  else if (face == 8)
    *font = &(fontset->SFont_8);
  else if (face == 9)
    *font = &(fontset->SFont_9);
  else if (face == 10)
    *font = &(fontset->SFont_10);
  else if (face == 11)
    *font = &(fontset->SFont_11);
  else if (face == 12)
    *font = &(fontset->SFont_12);
  else if (face == 13)
    *font = &(fontset->SFont_13);
  else if (face == 14)
    *font = &(fontset->SFont_14);
  else if (face == 15)
    *font = &(fontset->SFont_15);
  else if (face == 16)
    *font = &(fontset->SFont_16);
  else if (face == 17)
    *font = &(fontset->SFont_17);
  else
    *font = NULL;

#ifdef _GL
  /* load the stix font if needed */
  if (face > 0)
    {
      lfont = **font;
      for (frame = 1; frame <= MAX_FRAME; frame++)
        {
          mask = 1 << (frame - 1);
          if (fontset->FontMask & mask)
            {
              lfont = LoadNearestFont ('E', face, 0, fontset->FontSize,
                                       fontset->FontSize, frame,
                                       FALSE, FALSE);
            }
        }
      if (**font == NULL)
        **font = lfont;
    }
#endif /* _GL */
  return index;
}
