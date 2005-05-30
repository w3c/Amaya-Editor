/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2005
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 
/* 
 * Global variables for font management
 */


#ifndef THOT_FONT_VAR
#define THOT_FONT_VAR

THOT_EXPORT ThotFont  DialogFont;	/* Standard font dialogue */
THOT_EXPORT ThotFont  IDialogFont;	/* Italics font dialogue  */
THOT_EXPORT ThotFont  LargeDialogFont;	/* Large font dialogue    */
THOT_EXPORT ThotFont  SmallDialogFont;	/* Small font dialogue    */
THOT_EXPORT ThotFont  GraphicsIcons;	/* Graphics icons         */
THOT_EXPORT int	      MenuSize;	        /* Dialogue font size     */
THOT_EXPORT int       FontZoom;         /* Zoom applied to fonts  */
#ifdef _WINGUI
THOT_EXPORT HFONT     ActiveFont;
#endif /* _WINGUI */
#ifdef _GL
THOT_EXPORT ThotFont  DefaultGLFont;	/* Standard font dialogue */
#endif /* _GL */
THOT_EXPORT ThotBool  StixExist;        /* Stix fonts are available */
#endif
