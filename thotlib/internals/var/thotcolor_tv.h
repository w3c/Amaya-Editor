/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 
/*
 * definition of variables associated to the management of colors in Thot
 */

#ifndef THOT_COLOR_VAR
#define THOT_COLOR_VAR
#include "thotcolor.h"

/* standard set of colors */
THOT_EXPORT int              NColors;
THOT_EXPORT ThotColor        Pix_Color[MAX_COLOR];
THOT_EXPORT RGBstruct       *RGB_Table;
THOT_EXPORT STRING*          Color_Table;

/* extended set of colors */
THOT_EXPORT int              NbExtColors;
THOT_EXPORT ThotColor       *ExtColor;
THOT_EXPORT RGBstruct       *ExtRGB_Table;
THOT_EXPORT int             *ExtCount_Table;

THOT_EXPORT int              NbPatterns;
THOT_EXPORT STRING*          Patterns;

#endif /* THOT_COLOR_VAR */

