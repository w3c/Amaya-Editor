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

unsigned short ISOLatin2Map [] = {
    0x0000, 0x0001, 0x0002, 0x0003, 0x0004, 0x0005, 0x0006, 0x0007,
    0x0008, 0x0009, 0x000A, 0x000B, 0x000C, 0x000D, 0x000E, 0x000F,
    0x0010, 0x0011, 0x0012, 0x0013, 0x0014, 0x0015, 0x0016, 0x0017,
    0x0018, 0x0019, 0x001A, 0x001B, 0x001C, 0x001D, 0x001E, 0x001F,
    0x0020, 0x0021, 0x0022, 0x0023, 0x0024, 0x0025, 0x0026, 0x0027,
    0x0028, 0x0029, 0x002A, 0x002B, 0x002C, 0x002D, 0x002E, 0x002F,
    0x0030, 0x0031, 0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0037,
    0x0038, 0x0039, 0x003A, 0x003B, 0x003C, 0x003D, 0x003E, 0x003F,
    0x0040, 0x0041, 0x0042, 0x0043, 0x0044, 0x0045, 0x0046, 0x0047,
    0x0048, 0x0049, 0x004A, 0x004B, 0x004C, 0x004D, 0x004E, 0x004F,
    0x0050, 0x0051, 0x0052, 0x0053, 0x0054, 0x0055, 0x0056, 0x0057,
    0x0058, 0x0059, 0x005A, 0x005B, 0x005C, 0x005D, 0x005E, 0x005F,
    0x0060, 0x0061, 0x0062, 0x0063, 0x0064, 0x0065, 0x0066, 0x0067,
    0x0068, 0x0069, 0x006A, 0x006B, 0x006C, 0x006D, 0x006E, 0x006F,
    0x0070, 0x0071, 0x0072, 0x0073, 0x0074, 0x0075, 0x0076, 0x0077,
    0x0078, 0x0079, 0x007A, 0x007B, 0x007C, 0x007D, 0x007E, 0x007F,
    0x0080, 0x0081, 0x0082, 0x0083, 0x0084, 0x0085, 0x0086, 0x0087,
    0x0088, 0x0089, 0x008A, 0x008B, 0x008C, 0x008D, 0x008E, 0x008F,
    0x0090, 0x0091, 0x0092, 0x0093, 0x0094, 0x0095, 0x0096, 0x0097,
    0x0098, 0x0099, 0x009A, 0x009B, 0x009C, 0x009D, 0x009E, 0x009F,
    0x00A0, 0x0104, 0x02D8, 0x0141, 0x00A4, 0x013D, 0x015A, 0x00A7,
    0x00A8, 0x0160, 0x015E, 0x0164, 0x0179, 0x00AD, 0x017D, 0x017B,
    0x00B0, 0x0105, 0x02DB, 0x0142, 0x00B4, 0x013E, 0x015B, 0x02C7,
    0x00B8, 0x0161, 0x015F, 0x0165, 0x017A, 0x02DD, 0x017E, 0x017C,
    0x0154, 0x00C1, 0x00C2, 0x0102, 0x00C4, 0x0139, 0x0106, 0x00C7,
    0x010C, 0x00C9, 0x0118, 0x00CB, 0x011A, 0x00CD, 0x00CE, 0x010E,
    0x0110, 0x0143, 0x0147, 0x00D3, 0x00D4, 0x0150, 0x00D6, 0x00D7,
    0x0158, 0x016E, 0x00DA, 0x0170, 0x00DC, 0x00DD, 0x0162, 0x00DF,
    0x0155, 0x00E1, 0x00E2, 0x0103, 0x00E4, 0x013A, 0x0107, 0x00E7,
    0x010D, 0x00E9, 0x0119, 0x00EB, 0x011B, 0x00ED, 0x00EE, 0x010F,
    0x0111, 0x0144, 0x0148, 0x00F3, 0x00F4, 0x0151, 0x00F6, 0x00F7,
    0x0159, 0x016F, 0x00FA, 0x0171, 0x00FC, 0x00FD, 0x0163, 0x02D9
};    

#if 0 /* @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ */
typedef struct {
        unsigned char Fbyte;
        unsigned char Sbyte;
        unsigned char Tbyte;
        unsigned char Frbyte;
} MBC;

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
#endif /* @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ */

#ifdef __STDC__
extern STRING TtaAllocString (unsigned int);
extern void*  TtaGetMemory (unsigned int);
#else  /* !__STDC__ */
extern STRING TtaAllocString ();
extern void*  TtaGetMemory ();
#endif /* !__STDC__ */

/*----------------------------------------------------------------------------*\
 * uctio                                                                      *
\*----------------------------------------------------------------------------*/
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

/*----------------------------------------------------------------------------*\
 * uctol                                                                      *
\*----------------------------------------------------------------------------*/
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

/*----------------------------------------------------------------------------*\
 * utolower                                                                   *
\*----------------------------------------------------------------------------*/
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

#else  /* !_I18N_ */

#include <stdlib.h>
#include <ctype.h>

/*----------------------------------------------------------------------------*\
 * uctoi                                                                      *
\*----------------------------------------------------------------------------*/
#ifdef __STDC__
int uctoi (const STRING string )
#else  /* !__STDC__ */
int uctoi (string )
const STRING string;
#endif /* !__STDC__ */
{
    return atoi (string);
}

/*----------------------------------------------------------------------------*\
 * uctol                                                                      *
\*----------------------------------------------------------------------------*/
#ifdef __STDC__
long uctol (const STRING string )
#else  /* !__STDC__ */
long uctol (string )
const STRING string;
#endif /* !__STDC__ */
{
    return atol (string);
}

/*----------------------------------------------------------------------------*\
 * utolower                                                                   *
\*----------------------------------------------------------------------------*/
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

/*----------------------------------------------------------------------------*\
 * ToLower                                                                    *
\*----------------------------------------------------------------------------*/
#ifdef __STDC__
CharUnit ToLower (CharUnit c) 
#else  /* !__STDC__ */
CharUnit ToLower (c)
CharUnit c;
#endif /* __STDC__ */
{
#   if defined(_WINDOWS) && defined(_I18N_)
    return (CharUnit) towlower((wint_t ) c);
#   else  /* !(defined(_WINDOWS) && defined(_I18N_)) */
    return ((char) tolower ((int)c));
#   endif /* !(defined(_WINDOWS) && defined(_I18N_)) */
}

/*----------------------------------------------------------------------------*\
 * wctoi                                                                      *
\*----------------------------------------------------------------------------*/
#ifdef __STDC__
int wctoi (const CHAR_T* string)
#else  /* !__STDC__ */
int wctoi (string)
const CHAR_T* string;
#endif /* !__STDC__ */
{
#ifdef _I18N_
#  ifdef _WINDOWS
      /* 
      Windows provides a routine that allows to convert
      from a wide character string to an integer value 
      */
      return _wtoi (string);
#  else  /* !_WINDOWS */
      /* 
      For the moment Unix platforms do not provide allowing
      to convert from a wide character string to an interger.
      We have to convert string into multibyte character
      string and use atoi.
      */
      char  str[MAX_TXT_LEN];

      wcstombs (str, string, MAX_TXT_LEN);
      return atoi (str);
#  endif /* !_WINDOWS */
#else  /* !_I18N_ */
    return atoi (string);
#endif /* !_I18N_ */
}

/*----------------------------------------------------------------------------*\
 * TtaGetISOLatinCodeFromUnicode: return the ISO latin 2 code corresponding to
 * the Unicode value wc.
\*----------------------------------------------------------------------------*/
#ifdef __STDC__
unsigned char TtaGetISOLatinCodeFromUnicode (const CHAR_T wc)
#else  /* !__STDC__ */
unsigned char TtaGetISOLatinCodeFromUnicode (wc)
const CHAR_T  wc;
#endif /* !__STDC__ */
{
#   ifdef _I18N_
    switch (wc) {
           case 0x0104: return 0xA1; /* LATIN CAPITAL LETTER A WITH OGONEK       */
           case 0x02D8: return 0xA2; /* BREVE                                    */
           case 0x0141: return 0xA3; /* LATIN CAPITAL LETTER L WITH STROKE       */
           case 0x013D: return 0xA5; /* LATIN CAPITAL LETTER L WITH CARON        */
           case 0x015A: return 0xA6; /* LATIN CAPITAL LETTER S WITH ACUTE        */
           case 0x0160: return 0xA9; /* LATIN CAPITAL LETTER S WITH CARON        */
           case 0x015E: return 0xAA; /* LATIN CAPITAL LETTER S WITH CEDILLA      */
           case 0x0164: return 0xAB; /* LATIN CAPITAL LETTER T WITH CARON        */
           case 0x0179: return 0xAC; /* LATIN CAPITAL LETTER Z WITH ACUTE        */
           case 0x017D: return 0xAE; /* LATIN CAPITAL LETTER Z WITH CARON        */
           case 0x017B: return 0xAF; /* LATIN CAPITAL LETTER Z WITH DOT ABOVE    */
           case 0x0105: return 0xB1; /* LATIN SMALL LETTER A WITH OGONEK         */
           case 0x02DB: return 0xB2; /* OGONEK                                   */
           case 0x0142: return 0xB3; /* LATIN SMALL LETTER L WITH STROKE         */
           case 0x013E: return 0xB5; /* LATIN SMALL LETTER L WITH CARON          */
           case 0x015B: return 0xB6; /* LATIN SMALL LETTER S WITH ACUTE          */
           case 0x02C7: return 0xB7; /* CARON                                    */
           case 0x0161: return 0xB9; /* LATIN SMALL LETTER S WITH CARON          */
           case 0x015F: return 0xBA; /* LATIN SMALL LETTER S WITH CEDILLA        */
           case 0x0165: return 0xBB; /* LATIN SMALL LETTER T WITH CARON          */
           case 0x017A: return 0xBC; /* LATIN SMALL LETTER Z WITH ACUTE          */
           case 0x02DD: return 0xBD; /* DOUBLE ACUTE ACCENT                      */
           case 0x017E: return 0xBE; /* LATIN SMALL LETTER Z WITH CARON          */
           case 0x017C: return 0xBF; /* LATIN SMALL LETTER Z WITH DOT ABOVE      */
           case 0x0154: return 0xC0; /* LATIN CAPITAL LETTER R WITH ACUTE        */
           case 0x0102: return 0xC3; /* LATIN CAPITAL LETTER A WITH BREVE        */
           case 0x00C4: return 0xC4; /* LATIN CAPITAL LETTER A WITH DIAERESIS    */
           case 0x0139: return 0xC5; /* LATIN CAPITAL LETTER L WITH ACUTE        */
           case 0x0106: return 0xC6; /* LATIN CAPITAL LETTER C WITH ACUTE        */
           case 0x010C: return 0xC8; /* LATIN CAPITAL LETTER C WITH CARON        */
           case 0x0118: return 0xCA; /* LATIN CAPITAL LETTER E WITH OGONEK       */
           case 0x011A: return 0xCC; /* LATIN CAPITAL LETTER E WITH CARON        */
           case 0x010E: return 0xCF; /* LATIN CAPITAL LETTER D WITH CARON        */
           case 0x0110: return 0xD0; /* LATIN CAPITAL LETTER D WITH STROKE       */
           case 0x0143: return 0xD1; /* LATIN CAPITAL LETTER N WITH ACUTE        */
           case 0x0147: return 0xD2; /* LATIN CAPITAL LETTER N WITH CARON        */
           case 0x0150: return 0xD5; /* LATIN CAPITAL LETTER O WITH DOUBLE ACUTE */
           case 0x0158: return 0xD8; /* LATIN CAPITAL LETTER R WITH CARON        */
           case 0x016E: return 0xD9; /* LATIN CAPITAL LETTER U WITH RING ABOVE   */
           case 0x0170: return 0xDB; /* LATIN CAPITAL LETTER U WITH DOUBLE ACUTE */
           case 0x0162: return 0xDE; /* LATIN CAPITAL LETTER T WITH CEDILLA      */
           case 0x0155: return 0xE0; /* LATIN SMALL LETTER R WITH ACUTE          */
           case 0x0103: return 0xE3; /* LATIN SMALL LETTER A WITH BREVE          */
           case 0x013A: return 0xE5; /* LATIN SMALL LETTER L WITH ACUTE          */
           case 0x0107: return 0xE6; /* LATIN SMALL LETTER C WITH ACUTE          */
           case 0x010D: return 0xE8; /* LATIN SMALL LETTER C WITH CARON          */
           case 0x0119: return 0xEA; /* LATIN SMALL LETTER E WITH OGONEK         */
           case 0x011B: return 0xEC; /* LATIN SMALL LETTER E WITH CARON          */
           case 0x010F: return 0xEF; /* LATIN SMALL LETTER D WITH CARON          */
           case 0x0111: return 0xF0; /* LATIN SMALL LETTER D WITH STROKE         */
           case 0x0144: return 0xF1; /* LATIN SMALL LETTER N WITH ACUTE          */
           case 0x0148: return 0xF2; /* LATIN SMALL LETTER N WITH CARON          */
           case 0x0151: return 0xF5; /* LATIN SMALL LETTER O WITH DOUBLE ACUTE   */
           case 0x0159: return 0xF8; /* LATIN SMALL LETTER R WITH CARON          */
           case 0x016F: return 0xF9; /* LATIN SMALL LETTER U WITH RING ABOVE     */
           case 0x0171: return 0xFB; /* LATIN SMALL LETTER U WITH DOUBLE ACUTE   */
           case 0x0163: return 0xFE; /* LATIN SMALL LETTER T WITH CEDILLA        */ 
           case 0x02D9: return 0xFF; /* DOT ABOVE                                */
           default:     return (unsigned char) wc;
	}
#   else  /* !_I18N_ */
    return wc;
#   endif /* !_I18N_ */
}

#ifdef __STDC__
CHAR_T              TtaGetUnicodeValueFromISOLatinCode (const unsigned char c)
#else  /* !__STDC__ */
CHAR_T              TtaGetUnicodeValueFromISOLatinCode (c)
const unsigned char wc;
#endif /* !__STDC__ */
{
#  ifdef _I18N_
   return ISOLatin2Map [c];
#  else  /* !_I18N_ */
   return c;
#  endif /* !_I18N_ */
}
