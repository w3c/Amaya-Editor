/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2003
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 
/* 
 * Global variables for font management
 */


#ifndef THOT_FONT_VAR
#define THOT_FONT_VAR

THOT_EXPORT PtrFont   DialogFont;	/* Standard font dialogue */
THOT_EXPORT PtrFont   IDialogFont;	/* Italics font dialogue  */
THOT_EXPORT PtrFont   LargeDialogFont;	/* Large font dialogue    */
THOT_EXPORT PtrFont   SmallDialogFont;	/* Small font dialogue    */
THOT_EXPORT PtrFont   GraphicsIcons;	/* Graphics icons         */
THOT_EXPORT int	      MenuSize;	        /* Dialogue font size     */
THOT_EXPORT int       FontZoom;         /* Zoom applied to fonts  */
#ifdef _WINDOWS
THOT_EXPORT HFONT     ActiveFont;
#endif /* _WINDOWS */
#ifdef _GL
#include "openglfonts.h"
THOT_EXPORT GL_font  *DefaultGLFont;	/* Standard font dialogue */
#endif /* _GL */

#endif
