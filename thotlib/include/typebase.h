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
 
#ifndef _TYPEBASE_H_
#define _TYPEBASE_H_

#ifndef HAVE_BOOLEAN
typedef unsigned char boolean;

#define HAVE_BOOLEAN
#endif
/*
   #ifndef False
   #define False   0
   #endif
   #ifndef True
   #define True    (!False)
   #endif
 */

/* Description des unites */
typedef enum
  {
     UnRelative,		/* em */
     UnXHeight,
     UnPoint,
     UnPixel,
     UnPercent
  }
TypeUnit;

#ifdef MAXINT
#undef MAXINT
#endif
#define THOT_MAXINT ((1<<((sizeof(int) * 8) - 2)) - 1)

#if 0
#define MAXINT  2147483647
#endif

#define ord(c) (int)((unsigned char)(c))

#endif
