/*
 * definition of variables associated to the management of colors in Thot
 */

#ifndef THOT_COLOR_VAR
#define THOT_COLOR_VAR
#include "thotcolor.h"

EXPORT int NColors;
EXPORT ThotColor Pix_Color[MAX_COLOR];
EXPORT RGBstruct *RGB_Table;
EXPORT char **Color_Table;

EXPORT int NbPatterns;
EXPORT char **Patterns;

#endif /* THOT_COLOR_VAR */

