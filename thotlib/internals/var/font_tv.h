/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 
/* 
 * Global variables for font management
 */


#ifndef THOT_FONT_VAR
#define THOT_FONT_VAR

THOT_EXPORT PtrFont   FontDialogue;	/* Standard font dialogue */
THOT_EXPORT PtrFont   IFontDialogue;	/* Italics font dialogue  */
THOT_EXPORT PtrFont   LargeFontDialogue;	/* Large font dialogue    */
THOT_EXPORT PtrFont   SmallFontDialogue;	/* Small font dialogue    */
THOT_EXPORT PtrFont   GraphicsIcons;	/* Graphics icons         */
THOT_EXPORT int	      MenuSize;	        /* Dialogue font size     */
THOT_EXPORT int       FontZoom;         /* Zoom applied to fonts  */
#ifdef _WINDOWS
THOT_EXPORT HFONT     ActiveFont;
#endif /* _WINDOWS */

#endif
