/*
 *
 *  (c) COPYRIGHT INRIA 1999.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/* Unicode Routines
 * Author: R. Guetari (W3C/INRIA)
 */

#include "uconvert.h"

typedef struct {
        unsigned char Fbyte;
        unsigned char Sbyte;
        unsigned char Tbyte;
        unsigned char Frbyte;
} MBC;

#ifdef __STDC__
extern STRING TtaAllocString (unsigned int);
extern void*  TtaGetMemory (unsigned int);
#else  /* !__STDC__ */
extern STRING TtaAllocString ();
extern void*  TtaGetMemory ();
#endif /* !__STDC__ */


/**********************************************************************
 * ISO2WideChar: Converts an ISO String into a Wide Characters coded  *
 * String. An ISO String is a string which declaration is in the form *
 * of char* blahblah. A Wide Character string which declaration is    *
 * STRING blahblah may coded in 8, 16 or 32 bits.                     *
 **********************************************************************/
#ifdef __STDC__
STRING ISO2WideChar (const char* str)
#else  /* !__STDC__ */
STRING ISO2WideChar (str)
const char* str;
#endif /* !__STDC__ */
{
#   if defined(_I18N_) || defined(__JIS__)
    if (str) {
       int len = strlen (str);
       STRING puStr, uStr = TtaAllocString (len + 1);
	   puStr = uStr;
       while (*puStr++ = ((CHAR_T) *str++));
       *puStr = (CHAR_T)0;
	   return uStr;
	}
    return (STRING) 0;   
#   else /* defined(_I18N_) || defined(__JIS__) */
    return (STRING)str;
#   endif /* defined(_I18N_) || defined(__JIS__) */
}


/**********************************************************************
 * WideChar2ISO: Converts a Wide Characters coded String into an ISO  *
 * String.                                                            *
 **********************************************************************/
#ifdef __STDC__
char* WideChar2ISO (STRING str)
#else  /* !__STDC__ */
char* WideChar2ISO (str)
const STRING str;
#endif /* !__STDC__ */
{     
#   if defined(_I18N_) || defined(__JIS__)
    if (str) {
       int ndx, len = ustrlen (str);
       char* isoStr = (char*) TtaGetMemory ((len + 1) * sizeof (char));
       for (ndx = 0; ndx < len; ndx++) {
           char* pStr = (char*) &str[ndx];
           isoStr [ndx] = *pStr;
	   }
       isoStr [ndx] = (char)0;
	   return isoStr;
	}
    return (char*) 0;   
#   else /* defined(_I18N_) || defined(__JIS__) */
    return (char*)str;
#   endif /* defined(_I18N_) || defined(__JIS__) */
}

#ifdef _I18N_
#ifdef __STDC__
int uctoi (const STRING string )
#else  /* !__STDC__ */
int uctoi (string )
const STRING string;
#endif /* !__STDC__ */
{
#   ifdef _WINDOWS
    return _wtoi (string);
#   else  /* !_WINDOWS */
#   endif /* !_WINDOWS */
}

#ifdef __STDC__
long uctol (const STRING string)
#else  /* !__STDC__ */
long uctol (string)
const STRING string;
#endif /* __STDC_+_ */
{
#   ifdef _WINDOWS
    return _wtol (string);
#   else  /* !_WINDOWS */
#   endif /* !_WINDOWS */
}

#ifdef __STDC__
CHAR_T utolower (CHAR_T c)
#else  /* !__STDC__ */
CHAR_T utolower (c)
CHAR_T c;
#endif /* __STDC__ */
{
#    ifdef _WINDOWS
     return (CHAR_T) towlower((wint_t ) c);
#    else  /* !_WINDOWS */
#    endif /* _WINDOWS */
}

#ifdef __STDC__
int ISO2Unicode (char* c, PCHAR_T dest) 
#else   /* !__STDC__   */
int ISO2Unicode (c , dest )
char*     c;
STRING    dest;
#endif   /* __STDC__ */
{
   if (!c) {
      dest = (PCHAR_T) 0;
      return -1;
   }
   if ((*c) < 0x80)                          /* code ASCII  */
      *dest = (CHAR_T) (*c);  
   else {                   /*  Invalid ASCII code  */
        dest = (PCHAR_T) 0;
        return -1;
   } 
   return 1;
} 
  
#ifdef __STDC__
int Unicode2ISO (STRING source, char* dest)
#else /* !__STDC__ */
int Unicode2ISO (source , dest )
STRING   source;
char*    dest;
#endif   /* __STDC__ */
{

    if (!source) {
       dest = (char*) 0;
       return -1;
    }
    if ((*source) < 0x0080)
       *dest = (char) (*source);
    else {
         *dest = 0;
         return -1;
    }
    return 1;
}


#ifdef  __STDC__
int MB2Unicode (MBC* mbc, int length, STRING dest)
#else /*  !__STDC__  */
int NB2Unicode ( mbc, length, dest )
MBC*   mbc;
int    length;
STRING dest;
#endif  /* __STDC__ */
{

  switch (length) { 
         case  1: *dest = mbc->Fbyte;
                  break;
 
         case  2: *dest = mbc->Fbyte - 0xc0;
                  *dest <<= 6;
                  *dest |= (mbc->Sbyte - 0x80);
                  break;
 
         case  3: *dest = mbc->Fbyte - 0xe0;
                  *dest <<= 6;
                  *dest |= (mbc->Sbyte - 0x80);
                  *dest <<= 6;
                  *dest |= (mbc->Tbyte - 0x80);
                  break;

         default: return -1;
  }
  return 1;
}

#ifdef __STDC__
int Unicode2MB ( STRING source, int* length, MBC** mbc)
#else  /* !__STDC__ */
int Unicode2MB ( source, length, mbc )
STRING   source;
int*     length;
MBC**    mbc;
#endif /* __STDC__ */
{
  
  (*mbc)->Fbyte = 0;
  (*mbc)->Sbyte = 0;
  (*mbc)->Tbyte = 0;
  (*mbc)->Frbyte = 0;
  
  if ( (*source) < 0x0080 ){
     (*mbc)->Fbyte = (unsigned char )(*source);
     *length = 1;
  } else if ( (*source) < 0x0800 ){
          (*mbc)->Fbyte = (unsigned char )( 0x00c0 | (*source) >> 6 );
          (*mbc)->Sbyte = (unsigned char )( 0x0080 | (*source) & 0x003f);
          *length = 2;
  } else if ( (*source) < 0x10000 ){
          (*mbc)->Fbyte = (unsigned char)( 0x00e0 | (*source) >> 12);
          (*mbc)->Sbyte = (unsigned char)( 0x0080 | ((*source) >> 6 & 0x003f));
          (*mbc)->Tbyte = (unsigned char)( 0x0080 | ((*source) & 0x003f));
          *length = 3;
  } else 
       return -1;

  return 1;
}










#else  /* !_I18N_ */

#include <stdlib.h>
#include <ctype.h>

#ifdef __STDC__
int uctoi (const STRING string )
#else  /* !__STDC__ */
int uctoi (string )
const STRING string;
#endif /* !__STDC__ */
{
    return atoi (string);
}

#ifdef __STDC__
long uctol (const STRING string )
#else  /* !__STDC__ */
long uctol (string )
const STRING string;
#endif /* !__STDC__ */
{
    return atol (string);
}

#ifdef __STDC__
CHAR_T utolower (CHAR_T c)
#else  /* !__STDC__ */
CHAR_T utolower (c)
CHAR_T c;
#endif /* __STDC__ */
{
     return ((CHAR_T) tolower ((int)c));
}

#endif /* _I18N_ */

#ifdef __STDC__
CharUnit ToLower (CharUnit c) 
#else  /* !__STDC__ */
CharUnit ToLower (c)
CharUnit c;
#endif /* __STDC__ */
{
#   if defined(_WINDOWS) && defined(_I18N_)
    return (CHAR_T) towlower((wint_t ) c);
#   else  /* !(defined(_WINDOWS) && defined(_I18N_)) */
    return ((char) tolower ((int)c));
#   endif /* !(defined(_WINDOWS) && defined(_I18N_)) */
}

#ifdef __STDC__
int custoi (const CharUnit* string)
#else  /* !__STDC__ */
int custoi (string)
const CharUnit* string;
#endif /* !__STDC__ */
{
#   if defined(_WINDOWS) && defined(_I18N_)
    return _wtoi (string);
#   else  /* !_WINDOWS */
    return atoi (string);
#   endif /* !_WINDOWS */
}

