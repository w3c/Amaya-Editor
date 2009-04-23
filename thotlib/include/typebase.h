/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2008
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

#ifndef _TYPEBASE_H_
#define _TYPEBASE_H_
/* do not declare before */
#ifndef False
#define False   0
#endif
#ifndef True
#define True    (!False)
#endif

/* Description of access rights */
typedef enum
  {
     ReadWrite, ReadOnly, Hidden, Inherited
  }
AccessRight;

/* Description of units */
typedef enum
  {
     UnRelative,		/* em */
     UnXHeight,
     UnPoint,
     UnPixel,
     UnPercent,
     UnAuto,
     UnGradient,
     UnUndefined
  }
TypeUnit;
#define MAX_DOCUMENTS 20  /* max. number of simultaneous open documents */
#ifdef MAXINT
#undef MAXINT
#endif
#define THOT_MAXINT ((1<<((sizeof(int) * 8) - 2)) - 1)

#define ord(c) (int)((unsigned char)(c))

#endif






