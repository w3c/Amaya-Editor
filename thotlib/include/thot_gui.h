/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2005
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
 
#ifndef THOT_GUI_H
#define THOT_GUI_H
#define MAX_LENGTH     800
#define MAX_EXT         10

/* strange constants .. to remove if possible */
#if defined(_UNIX)/* || defined(_WX)*/
/* button states */
#define TBSTYLE_BUTTON  0
#define TBSTYLE_CHECK   1
#define TBSTYLE_SEP     2
#define ThotColorNone ((Pixel)-1)
#define ThotBitmapNone ((ThotBitmap)-1)
#endif /* defined(_UNIX) */

/*
 * Specific types fore each GUI toolkit
 *
 * */
#include "thot_gui_windows.h"
#include "thot_gui_gtk.h"
#include "thot_gui_wx.h"

#endif /* THOT_GUI_H */
