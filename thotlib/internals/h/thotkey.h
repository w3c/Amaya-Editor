/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2002
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 
#ifndef _THOTKEY_H_
#define _THOTKEY_H_

#define INVISIBLE_CHAR 1 /* not displayed char */
#define NEW_LINE 10
#define SHOWN_THIN_SPACE 96 /* THIN-SPACE display value */
#define BREAK_LINE 138
#define SHOWN_BREAK_LINE 182 /* BREAK_LINE display value */
#define UNBREAKABLE_SPACE 0xA0

#ifdef _I18N_
#define THIN_SPACE 0x2009
#define HALF_EM 0x2005
#define SHOWN_HALF_EM 166 /* HALF-EM display value */
#define SHOWN_UNBREAKABLE_SPACE 185 /* UNBREAKABLE-SPACE display value */
#define SHOWN_SPACE 183 /* SPACE display value */
#else /* _I18N_ */
#define THIN_SPACE 129
#define HALF_EM 130
#define SHOWN_HALF_EM 166 /* HALF-EM display value */
#define SHOWN_UNBREAKABLE_SPACE 185 /* UNBREAKABLE-SPACE display value */
#define SHOWN_SPACE 183 /* SPACE display value */
#endif /* _I18N_ */
#endif
