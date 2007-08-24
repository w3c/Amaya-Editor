/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 
/*
 * thotcolor.h : define the Thot palettes data structures
 */

#ifndef THOT_COLOR_H
#define THOT_COLOR_H

typedef struct _RGBstruct{
  unsigned short red;
  unsigned short green;
  unsigned short blue;
  }RGBstruct;

#define MAX_COLOR 152

extern RGBstruct RGB_colors[];
extern const char *Name_colors[];

#endif /* THOT_COLOR_H */

