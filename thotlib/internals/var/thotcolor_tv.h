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

THOT_EXPORT int NColors;
THOT_EXPORT ThotColor Pix_Color[MAX_COLOR];
THOT_EXPORT RGBstruct *RGB_Table;
THOT_EXPORT char **Color_Table;

THOT_EXPORT int NbPatterns;
THOT_EXPORT char **Patterns;

#endif /* THOT_COLOR_VAR */

