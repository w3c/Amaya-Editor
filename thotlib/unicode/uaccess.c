/*
 *
 *  (c) COPYRIGHT INRIA, 1999.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/* Unicode Routines
 * Author: R. Guetari (W3C/INRIA)
 */

#include "uaccess.h"

/*----------------------------------------------------------------*\
 * TtaIsDigit                                                     *
\*----------------------------------------------------------------*/
#ifdef __STDC__
int TtaIsDigit (CHAR_T val)
#else  /* !__STDC__ */
int TtaIsDigit (val)
CHAR_T   val;
#endif /* !__STDC__ */
{
#ifdef _I18N_ 
#  ifdef _WINDOWS
   return iswdigit (val);
   /* The compatibility of iswdigit routine is supposed to be 
    * ANSI, WIN NT and WIN 95.
    * Use it when a I18N version of libc for Unix/Linux is available 
    */
#  else  /* !_WINDOWS */
   return isdigit ((int) val);
#  endif /* !_WINDOWS */
#else  /* !_I18N_ */
   return isdigit ((int)val);
#endif /* !_I18N_ */
}

/*----------------------------------------------------------------*\
 * TtaIsAlpha                                                     *
\*----------------------------------------------------------------*/
#ifdef __STDC__
int TtaIsAlpha (CHAR_T val)
#else  /* !__STDC__ */
int TtaIsAlpha (val)
CHAR_T   val;
#endif /* !__STDC__ */
{
#ifdef _I18N_ 
#  ifdef _WINDOWS
   return iswalpha (val);
   /* The compatibility of iswalpha routine is supposed to be 
    * ANSI, WIN NT and WIN 95.
    * Use it when a I18N version of libc for Unix/Linux is available 
    */
#  else  /* !_WINDOWS */
   return isalpha ((int) val);
#  endif /* !_WINDOWS */
#else  /* !_I18N_ */
   return isalpha ((int)val);
#endif /* !_I18N_ */
}

/*----------------------------------------------------------------*\
 * TtaIsAlnum                                                     *
\*----------------------------------------------------------------*/
#ifdef __STDC__
int TtaIsAlnum (CHAR_T val)
#else  /* !__STDC__ */
int TtaIsAlnum (val)
CHAR_T   val;
#endif /* !__STDC__ */
{
#ifdef _I18N_ 
#  ifdef _WINDOWS
   return iswalnum (val);
   /* The compatibility of iswalnum routine is supposed to be 
    * ANSI, WIN NT and WIN 95.
    * Use it when a I18N version of libc for Unix/Linux is available 
    */
#  else  /* !_WINDOWS */
   return isalnum ((int) val);
#  endif /* !_WINDOWS */
#else  /* !_I18N_ */
   return isalnum ((int)val);
#endif /* !_I18N_ */
}



