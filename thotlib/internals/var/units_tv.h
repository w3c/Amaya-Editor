/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * Warning:
 * This module is part of the Thot library, which was originally
 * developed in French. That's why some comments are still in
 * French, but their translation is in progress and the full module
 * will be available in English in the next release.
 *
 */

/*
 * Global variables for units management
 */

#ifndef THOT_UNITS_VAR
#define THOT_UNITS_VAR

THOT_EXPORT int      DOT_PER_INCH;  /* screen resolution */

#ifdef _WINDOWS 
THOT_EXPORT int      ScreenDPI;
THOT_EXPORT int      PrinterDPI;
#endif /* _WINDOWS */
#endif
