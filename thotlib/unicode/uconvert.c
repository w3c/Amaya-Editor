/*
 *
 *  (c) COPYRIGHT INRIA 1999.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/* Unicode Routines
 * Author: R. Guetari (W3C/INRIA)
 */
#include "thot_sys.h"
#include "uconvert.h"

extern unsigned long offset[6];

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

/* Arabic ISO Latin encoding */
unsigned short ISOLatin6Map [] = {
    0x0000, 0x0001, 0x0002, 0x0003, 0x0004, 0x0005, 0x0006, 0x0007,
    0x0008, 0x0009, 0x000A, 0x000B, 0x000C, 0x000D, 0x000E, 0x000F,
    0x0010, 0x0011, 0x0012, 0x0013, 0x0014, 0x0015, 0x0016, 0x0017,
    0x0018, 0x0019, 0x001A, 0x001B, 0x001C, 0x001D, 0x001E, 0x001F,
    0x0020, 0x0021, 0x0022, 0x0023, 0x0024, 0x0025, 0x0026, 0x0027,
    0x0028, 0x0029, 0x002A, 0x002B, 0x002C, 0x002D, 0x002E, 0x002F,
    0x0660, 0x0661, 0x0662, 0x0663, 0x0664, 0x0665, 0x0666, 0x0667,
    0x0668, 0x0669, 0x003A, 0x003B, 0x003C, 0x003D, 0x003E, 0x003F,
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
    0x00A0, 0x00A1, 0x00A2, 0x00A3, 0x00A4, 0x00A5, 0x00A6, 0x00A7,
    0x00A8, 0x00A9, 0x00AA, 0x00AB, 0x060C, 0x00AD, 0x00AE, 0x00AF,
    0x00B0, 0x00B1, 0x00B2, 0x00B3, 0x00B4, 0x00B5, 0x00B6, 0x00B7,
    0x00B8, 0x00B9, 0x00BA, 0x061B, 0x00BC, 0x00BD, 0x00BE, 0x061F,
    0x00C0, 0x0621, 0x0622, 0x0623, 0x0624, 0x0625, 0x0626, 0x0627,
    0x0628, 0x0629, 0x062A, 0x062B, 0x062C, 0x062D, 0x062E, 0x062F,
    0x0630, 0x0631, 0x0632, 0x0633, 0x0634, 0x0635, 0x0636, 0x0637,
    0x0638, 0x0639, 0x063A, 0x00DB, 0x00DC, 0x00DD, 0x00DE, 0x00DF,
    0x0640, 0x0641, 0x0642, 0x0643, 0x0644, 0x0645, 0x0646, 0x0647,
    0x0648, 0x0649, 0x064A, 0x064B, 0x064C, 0x064D, 0x064E, 0x064F,
    0x0650, 0x0651, 0x0652, 0x00F3, 0x00F4, 0x00F5, 0x00F6, 0x00F7,
    0x00F8, 0x00F9, 0x00FA, 0x00FB, 0x00FC, 0x00FD, 0x00FE, 0x00FF
};

/* Windows Arabic Code Page */
unsigned short WIN1256CP [] = {
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
    0xFFFE, 0x067E, 0x201A, 0x0192, 0x201E, 0x2026, 0x2020, 0x2021,
    0x02C6, 0x2030, 0xFFFE, 0x2039, 0x0152, 0x0686, 0x0698, 0xFFFE,
    0x06AF, 0x2018, 0x2019, 0x201C, 0x201D, 0x2022, 0x2013, 0x2014,
    0xFFFE, 0x2122, 0xFFFE, 0x203A, 0x0153, 0x200C, 0x200D, 0xFFFE,
    0x00A0, 0x060C, 0x00A2, 0x00A3, 0x00A4, 0x00A5, 0x00A6, 0x00A7,
    0x00A8, 0x00A9, 0xFFFE, 0x00AB, 0x00AC, 0x00AD, 0x00AE, 0x00AF,
    0x00B0, 0x00B1, 0x00B2, 0x00B3, 0x00B4, 0x00B5, 0x00B6, 0x00B7,
    0x00B8, 0x00B9, 0x061B, 0x00BB, 0x00BC, 0x00BD, 0x00BE, 0x061F,
    0xFFFE, 0x0621, 0x0622, 0x0623, 0x0624, 0x0625, 0x0626, 0x0627,
    0x0628, 0x0629, 0x062A, 0x062B, 0x062C, 0x062D, 0x062E, 0x062F,
    0x0630, 0x0631, 0x0632, 0x0633, 0x0634, 0x0635, 0x0636, 0x00D7,
    0x0637, 0x0638, 0x0639, 0x063A, 0x0640, 0x0641, 0x0642, 0x0643,
    0x00E0, 0x0644, 0x00E2, 0x0645, 0x0646, 0x0647, 0x0648, 0x00E7,
    0x00E8, 0x00E9, 0x00EA, 0x00EB, 0x0649, 0x064A, 0x00EE, 0x00EF,
    0x064B, 0x064C, 0x064D, 0x064E, 0x00F4, 0x064F, 0x0650, 0x00F7,
    0x0651, 0x00F9, 0x0652, 0x00FB, 0x00FC, 0x200E, 0x200F, 0xFFFE
};


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
      For the moment Unix platforms do not provide a routine 
      allowing to convert from a wide character string to an 
      interger.  We have to  convert string  into  multibyte 
      character string and use atoi.
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
 * TtaGetISOLatin2CodeFromUnicode: return the ISO latin 2 code corresponding to
 * the Unicode value wc.
\*----------------------------------------------------------------------------*/
#ifdef __STDC__
unsigned char TtaGetISOLatin2CodeFromUnicode (const CHAR_T wc)
#else  /* !__STDC__ */
unsigned char TtaGetISOLatin2CodeFromUnicode (wc)
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

/*----------------------------------------------------------------------------*\
 * TtaGetUnicodeValueFromISOLatin2Code: return the Unicode val corresponding to
 * the ISO Latin 2 code c.
\*----------------------------------------------------------------------------*/
#ifdef __STDC__
CHAR_T              TtaGetUnicodeValueFromISOLatin2Code (const unsigned char c)
#else  /* !__STDC__ */
CHAR_T              TtaGetUnicodeValueFromISOLatin2Code (c)
const unsigned char wc;
#endif /* !__STDC__ */
{
#  ifdef _I18N_
   return ISOLatin2Map [c];
#  else  /* !_I18N_ */
   return c;
#  endif /* !_I18N_ */
}

/*----------------------------------------------------------------------------*\
 * TtaGetISOLatin6CodeFromUnicode: return the ISO latin 6 code corresponding to
 * the Unicode value wc.
\*----------------------------------------------------------------------------*/
#ifdef __STDC__
unsigned char TtaGetISOLatin6CodeFromUnicode (const CHAR_T wc)
#else  /* !__STDC__ */
unsigned char TtaGetISOLatin6CodeFromUnicode (wc)
const CHAR_T  wc;
#endif /* !__STDC__ */
{
#   ifdef _I18N_
    switch (wc) {
           case 0x0660: return 0x30; /* ARABIC-INDIC DIGIT ZERO             */
           case 0x0661: return 0x31; /* ARABIC-INDIC DIGIT ONE              */
           case 0x0662: return 0x32; /* ARABIC-INDIC DIGIT TWO              */
           case 0x0663: return 0x33; /* ARABIC-INDIC DIGIT THREE            */
           case 0x0664: return 0x34; /* ARABIC-INDIC DIGIT FOUR             */
           case 0x0665: return 0x35; /* ARABIC-INDIC DIGIT FIVE             */
           case 0x0666: return 0x36; /* ARABIC-INDIC DIGIT SIX              */
           case 0x0667: return 0x37; /* ARABIC-INDIC DIGIT SEVEN            */
           case 0x0668: return 0x38; /* ARABIC-INDIC DIGIT EIGHT            */
           case 0x0669: return 0x39; /* ARABIC-INDIC DIGIT NINE             */
           case 0x060C: return 0xAC; /* ARABIC COMMA                        */
           case 0x061B: return 0xBB; /* ARABIC SEMICOLON                    */
           case 0x061F: return 0xBF; /* ARABIC QUESTION MARK                */
           case 0x0621: return 0xC1; /* ARABIC LETTER HAMZA                 */
           case 0x0622: return 0xC2; /* ARABIC LETTER ALEF WITH MADDA ABOVE */
           case 0x0623: return 0xC3; /* ARABIC LETTER ALEF WITH HAMZA ABOVE */
           case 0x0624: return 0xC4; /* ARABIC LETTER WAW WITH HAMZA ABOVE  */
           case 0x0625: return 0xC5; /* ARABIC LETTER ALEF WITH HAMZA BELOW */
           case 0x0626: return 0xC6; /* ARABIC LETTER YEH WITH HAMZA ABOVE  */
           case 0x0627: return 0xC7; /* ARABIC LETTER ALEF                  */
           case 0x0628: return 0xC8; /* ARABIC LETTER BEH                   */
           case 0x0629: return 0xC9; /* ARABIC LETTER TEH MARBUTA           */
           case 0x062A: return 0xCA; /* ARABIC LETTER TEH                   */
           case 0x062B: return 0xCB; /* ARABIC LETTER THEH                  */
           case 0x062C: return 0xCC; /* ARABIC LETTER JEEM                  */
           case 0x062D: return 0xCD; /* ARABIC LETTER HAH                   */
           case 0x062E: return 0xCE; /* ARABIC LETTER KHAH                  */
           case 0x062F: return 0xCF; /* ARABIC LETTER DAL                   */
           case 0x0630: return 0xD0; /* ARABIC LETTER THAL                  */
           case 0x0631: return 0xD1; /* ARABIC LETTER REH                   */
           case 0x0632: return 0xD2; /* ARABIC LETTER ZAIN                  */
           case 0x0633: return 0xD3; /* ARABIC LETTER SEEN                  */
           case 0x0634: return 0xD4; /* ARABIC LETTER SHEEN                 */
           case 0x0635: return 0xD5; /* ARABIC LETTER SAD                   */
           case 0x0636: return 0xD6; /* ARABIC LETTER DAD                   */
           case 0x0637: return 0xD7; /* ARABIC LETTER TAH                   */
           case 0x0638: return 0xD8; /* ARABIC LETTER ZAH                   */
           case 0x0639: return 0xD9; /* ARABIC LETTER AIN                   */
           case 0x063A: return 0xDA; /* ARABIC LETTER GHAIN                 */
           case 0x0640: return 0xE0; /* ARABIC TATWEEL                      */
           case 0x0641: return 0xE1; /* ARABIC LETTER FEH                   */
           case 0x0642: return 0xE2; /* ARABIC LETTER QAF                   */
           case 0x0643: return 0xE3; /* ARABIC LETTER KAF                   */
           case 0x0644: return 0xE4; /* ARABIC LETTER LAM                   */
           case 0x0645: return 0xE5; /* ARABIC LETTER MEEM                  */
           case 0x0646: return 0xE6; /* ARABIC LETTER NOON                  */
           case 0x0647: return 0xE7; /* ARABIC LETTER HEH                   */
           case 0x0648: return 0xE8; /* ARABIC LETTER WAW                   */
           case 0x0649: return 0xE9; /* ARABIC LETTER ALEF MAKSURA          */
           case 0x064A: return 0xEA; /* ARABIC LETTER YEH                   */
           case 0x064B: return 0xEB; /* ARABIC FATHATAN                     */
           case 0x064C: return 0xEC; /* ARABIC DAMMATAN                     */
           case 0x064D: return 0xED; /* ARABIC KASRATAN                     */
           case 0x064E: return 0xEE; /* ARABIC FATHA                        */
           case 0x064F: return 0xEF; /* ARABIC DAMMA                        */
           case 0x0650: return 0xF0; /* ARABIC KASRA                        */
           case 0x0651: return 0xF1; /* ARABIC SHADDA                       */
           case 0x0652: return 0xF2; /* ARABIC SUKUN                        */
           default:     return (unsigned char) wc;
	}
#   else  /* !_I18N_ */
    return wc;
#   endif /* !_I18N_ */
}

/*----------------------------------------------------------------------------*\
 * TtaGetUnicodeValueFromISOLatin6Code: return the Unicode val corresponding to
 * the ISO Latin 6 code c.
\*----------------------------------------------------------------------------*/
#ifdef __STDC__
CHAR_T              TtaGetUnicodeValueFromISOLatin6Code (const unsigned char c)
#else  /* !__STDC__ */
CHAR_T              TtaGetUnicodeValueFromISOLatin6Code (c)
const unsigned char wc;
#endif /* !__STDC__ */
{
#  ifdef _I18N_
   return ISOLatin6Map [c];
#  else  /* !_I18N_ */
   return c;
#  endif /* !_I18N_ */
}


/*----------------------------------------------------------------------------*\
 * TtaGetWindows1256CPCodeFromUnicode: return the WIN32 1256 CP code corresponding 
 * to the Unicode value wc.
\*----------------------------------------------------------------------------*/
#ifdef __STDC__
unsigned char TtaGetWindows1256CPCodeFromUnicode (const CHAR_T wc)
#else  /* !__STDC__ */
unsigned char TtaGetWindows1256CPCodeFromUnicode (wc)
const CHAR_T  wc;
#endif /* !__STDC__ */
{
#   ifdef _I18N_
    switch (wc) {
           case 0x067E: return 0x81;
           case 0x201A: return 0x82;
           case 0x0192: return 0x83;
           case 0x201E: return 0x84;
           case 0x2026: return 0x85;
           case 0x2020: return 0x86;
           case 0x2021: return 0x87;
           case 0x02C6: return 0x88;
           case 0x2030: return 0x89;
           case 0x2039: return 0x8B;
           case 0x0152: return 0x8C;
           case 0x0686: return 0x8D;
           case 0x0698: return 0x8E;
           case 0x06AF: return 0x90;
           case 0x2018: return 0x91;
           case 0x2019: return 0x92;
           case 0x201C: return 0x93;
           case 0x201D: return 0x94;
           case 0x2022: return 0x95;
           case 0x2013: return 0x96;
           case 0x2014: return 0x97;
           case 0x2122: return 0x99;
           case 0x203A: return 0x9B;
           case 0x0153: return 0x9C;
           case 0x200C: return 0x9D;
           case 0x200D: return 0x9E;
           case 0x060C: return 0xA1;
           case 0x061B: return 0xBA;
           case 0x061F: return 0xBF;
           case 0x0621: return 0xC1;
           case 0x0622: return 0xC2;
           case 0x0623: return 0xC3;
           case 0x0624: return 0xC4;
           case 0x0625: return 0xC5;
           case 0x0626: return 0xC6;
           case 0x0627: return 0xC7;
           case 0x0628: return 0xC8;
           case 0x0629: return 0xC9;
           case 0x062A: return 0xCA;
           case 0x062B: return 0xCB;
           case 0x062C: return 0xCC;
           case 0x062D: return 0xCD;
           case 0x062E: return 0xCE;
           case 0x062F: return 0xCF;
           case 0x0630: return 0xD0;
           case 0x0631: return 0xD1;
           case 0x0632: return 0xD2;
           case 0x0633: return 0xD3;
           case 0x0634: return 0xD4;
           case 0x0635: return 0xD5;
           case 0x0636: return 0xD6;
           case 0x0637: return 0xD8;
           case 0x0638: return 0xD9;
           case 0x0639: return 0xDA;
           case 0x063A: return 0xDB;
           case 0x0640: return 0xDC;
           case 0x0641: return 0xDD;
           case 0x0642: return 0xDE;
           case 0x0643: return 0xDF;
           case 0x0644: return 0xE1;
           case 0x0645: return 0xE3;
           case 0x0646: return 0xE4;
           case 0x0647: return 0xE5;
           case 0x0648: return 0xE6;
           case 0x0649: return 0xEC;
           case 0x064A: return 0xED;
           case 0x064B: return 0xF0;
           case 0x064C: return 0xF1;
           case 0x064D: return 0xF2;
           case 0x064E: return 0xF3;
           case 0x064F: return 0xF5;
           case 0x0650: return 0xF6;
           case 0x0651: return 0xF8;
           case 0x0652: return 0xFA;
           case 0x200E: return 0xFD;
           case 0x200F: return 0xFE;
           default:     return (unsigned char) wc;
	}
#   else  /* !_I18N_ */
    return wc;
#   endif /* !_I18N_ */
}

/*----------------------------------------------------------------------------*\
 * TtaGetUnicodeValueFromWindows1256CP: return the Unicode val corresponding to
 * the Windows Arabic Code Page (1256) value c.
\*----------------------------------------------------------------------------*/
#ifdef __STDC__
CHAR_T              TtaGetUnicodeValueFromWindows1256CP (const unsigned char c)
#else  /* !__STDC__ */
CHAR_T              TtaGetUnicodeValueFromWindows1256CP (c)
const unsigned char wc;
#endif /* !__STDC__ */
{
#  ifdef _I18N_
   return WIN1256CP [c];
#  else  /* !_I18N_ */
   return c;
#  endif /* !_I18N_ */
}


/*------------------------------------------------------------------------------*\
 * TtaWC2MB: converts a wide character into a multibyte character.              *
 * Return value: -1 if fails                                                    *
 *               Number of bytes in the multibyte character                     *
\*------------------------------------------------------------------------------*/
#ifdef __STDC__
int     TtaWC2MB (CHAR_T wchar, char* mbchar, CHARSET encoding)
#else  /* !__STDC__ */
int     TtaWC2MB (wchar, mbchar, encoding)
CHAR_T  wchar;
char*   mbchar; 
CHARSET encoding;
#endif /* !__STDC__ */
{
    int            nbBytes = 1;
    unsigned char  LeadByteMark;
    unsigned char* mbcptr = mbchar;
    CHAR_T         WideChar = wchar;

    switch (encoding) {
           case ISOLatin1:
                *mbchar++ = (unsigned char) wchar;
                *mbchar   = 0;
                break;

           case ISOLatin2:
                *mbchar++ = TtaGetISOLatin2CodeFromUnicode (wchar);
                *mbchar   = 0;
                break;

           case ISOLatin6:
                *mbchar++ = TtaGetISOLatin6CodeFromUnicode (wchar);
                *mbchar   = 0;
                break;

           case WIN1256:
                *mbchar++ = TtaGetWindows1256CPCodeFromUnicode (wchar);
                *mbchar   = 0;
                break;

           case UTF8:
                if (WideChar < 0x80) {
                   nbBytes  = 1;
                   LeadByteMark = 0x00;
                } else if (WideChar < 0x800) {
                       nbBytes      = 2;
                       LeadByteMark = 0xC0;
                } else if (WideChar < 0x10000) {
                       nbBytes      = 3;
                       LeadByteMark = 0xE0;
                } else if (WideChar < 0x200000) {
                       nbBytes      = 4;
                       LeadByteMark = 0xF0;
                } else if (WideChar < 0x4000000) {
                       nbBytes      = 5;
                       LeadByteMark = 0xF8;
                } else if (WideChar < 0x7FFFFFFF) {
                       nbBytes      = 6;
                       LeadByteMark = 0xFC;
            	} else {
                       *mbchar = '?';
                       return -1;
            	}

                mbcptr += nbBytes;

                switch (nbBytes) {
                       case 6: *--mbcptr = (WideChar | 0x80) & 0xBF;
                               WideChar >>= 6;

                       case 5: *--mbcptr = (WideChar | 0x80) & 0xBF;
                               WideChar >>= 6;

                       case 4: *--mbcptr = (WideChar | 0x80) & 0xBF;
                               WideChar >>= 6;

                       case 3: *--mbcptr = (WideChar | 0x80) & 0xBF;
                               WideChar >>= 6;

                       case 2: *--mbcptr = (WideChar | 0x80) & 0xBF;
                               WideChar >>= 6;

                       case 1: *--mbcptr = WideChar | LeadByteMark;
                }
                break;
	}

    return nbBytes;
}


/*------------------------------------------------------------------------------*\
 * TtaMB2WC: converts a multibyte character to a wide character.                *
 * Return value: -1 if fails                                                    *
 *               Number of bytes in the multibyte character                     *
\*------------------------------------------------------------------------------*/
#ifdef __STDC__
int     TtaMB2WC (char* mbchar, CHAR_T* wchar, CHARSET encoding)
#else  /* !__STDC__ */
int     TtaMB2WC (wchar, mbchar, encoding)
char*   mbchar; 
CHAR_T  wchar;
CHARSET encoding;
#endif /* !__STDC__ */
{
    int    nbBytesToConvert = 1;
    CHAR_T res;

    switch (encoding) {
           case ISOLatin1:
                *wchar = (CHAR_T)(*mbchar);
                break;

           case ISOLatin2:
                *wchar = TtaGetUnicodeValueFromISOLatin2Code (*mbchar);
                break;

           case ISOLatin6:
                *wchar = TtaGetUnicodeValueFromISOLatin6Code (*mbchar);
                break;

           case WIN1256:
                *wchar = TtaGetUnicodeValueFromWindows1256CP (*mbchar);
                break;

           case UTF8:
                if (*mbchar < 0xC0)
                   nbBytesToConvert = 1;
                else if (*mbchar < 0xE0)
                     nbBytesToConvert = 2;
                else if (*mbchar < 0xF0)
                     nbBytesToConvert = 3;
                else if (*mbchar < 0xF8)
                     nbBytesToConvert = 4;
                else if (*mbchar < 0xFC)
                     nbBytesToConvert = 5;
                else if (*mbchar <= 0xFF)
                     nbBytesToConvert = 6;
                 
                res = 0;
                switch (nbBytesToConvert) {        /** WARNING: There is not break statement between cases */
                       case 6: res += *mbchar++;
                               res <<= 6;
 
                       case 5: res += *mbchar++;
                               res <<= 6;
              
                       case 4: res += *mbchar++;
                               res <<= 6;
 
                       case 3: res += *mbchar++;
                               res <<= 6;
 
                       case 2: res += *mbchar++;
                               res <<= 6;
                
                       case 1: res += *mbchar++;
				}
                res -= offset[nbBytesToConvert - 1];

                if (res <= 0xFFFF)
                   *wchar = res;
                else 
                    *wchar = TEXT('?');
                break;
	}
    return nbBytesToConvert;
}


#ifdef _I18N_
/*--------------------------------------------------------------------------------*\
 * TtaMBS2WCS: converts a multibyte string into a wide character string according *
 * to the charset.                                                                *
 * Return value: -1 if fails                                                      *
 *               Number of bytes in the multibyte character                       *
\*--------------------------------------------------------------------------------*/
#ifdef __STDC__
int TtaMBS2WCS (unsigned char** src, CHAR_T** target, CHARSET encoding)
#else  /* !__STDC__ */
int TtaMBS2WCS (src, target, encoding)
unsigned char** src;
CHAR_T**        target;
CHARSET         encoding;
#endif /* !__STDC__ */
{
	register unsigned char* ptrSrc           = *src;
    register CHAR_T*        ptrTarget        = *target;
    int                     nbBytesConverted = 0;

    switch (encoding) {
            case ISOLatin1: 
                 while (*ptrSrc) {
                       *ptrTarget = (CHAR_T)*ptrSrc;
                       nbBytesConverted++;
                       ptrSrc++;
                       ptrTarget++;
				 }
                 *ptrTarget = 0;
                 break;

            case ISOLatin2: 
                 while (*ptrSrc) {
                       *ptrTarget = TtaGetUnicodeValueFromISOLatin2Code (*ptrSrc);
                       nbBytesConverted++;
                       ptrSrc++;
                       ptrTarget++;
				 }
                 *ptrTarget = 0;
                 break;   

            case ISOLatin6: 
                 while (*ptrSrc) {
                       *ptrTarget = TtaGetUnicodeValueFromISOLatin6Code (*ptrSrc);
                       nbBytesConverted++;
                       ptrSrc++;
                       ptrTarget++;
				 }
                 *ptrTarget = 0;
                 break;

            case WIN1256: 
                 while (*ptrSrc) {
                       *ptrTarget = TtaGetUnicodeValueFromWindows1256CP (*ptrSrc);
                       nbBytesConverted++;
                       ptrSrc++;
                       ptrTarget++;
				 }
                 *ptrTarget = 0;
                 break;

            case UTF8:
                 while (*ptrSrc) {
                       int    nbBytesToConvert ;
                       CHAR_T res;
                       if (*ptrSrc < 0xC0)
                          nbBytesToConvert = 1;
                       else if (*ptrSrc < 0xE0)
                            nbBytesToConvert = 2;
                       else if (*ptrSrc < 0xF0)
                            nbBytesToConvert = 3;
                       else if (*ptrSrc < 0xF8)
                            nbBytesToConvert = 4;
                       else if (*ptrSrc < 0xFC)
                            nbBytesToConvert = 5;
                       else if (*ptrSrc <= 0xFF)
                            nbBytesToConvert = 6;
                 
                       nbBytesConverted += nbBytesToConvert;

                       res = 0;
                       switch (nbBytesToConvert) {        /** WARNING: There is not break statement between cases */
                              case 6: res += *ptrSrc++;
                                      res <<= 6;
   
                              case 5: res += *ptrSrc++;
                                      res <<= 6;
              
                              case 4: res += *ptrSrc++;
                                      res <<= 6;
 
                              case 3: res += *ptrSrc++;
                                      res <<= 6;
 
                              case 2: res += *ptrSrc++;
                                      res <<= 6;
                
                              case 1: res += *ptrSrc++;
					   }
                       res -= offset[nbBytesToConvert - 1];

                       if (res <= 0xFFFF)
                          *ptrTarget++ = res;
                       else 
                           *ptrTarget++ = TEXT('?');
				 }
                 *ptrTarget = 0L;
                 break;
	}
    if (nbBytesConverted > 0)
       return nbBytesConverted;
    return -1;
}


/*--------------------------------------------------------------------------------*\
 * TtaWCS2MBS: converts a wide character string into a multibyte string according *
 * to the charset.                                                                *
 * Return value: -1 if fails                                                      *
 *               Number of bytes in the multibyte character                       *
\*--------------------------------------------------------------------------------*/
#ifdef __STDC__
int TtaWCS2MBS (CHAR_T** src, unsigned char** target, CHARSET encoding)
#else  /* !__STDC__ */
int TtaWCS2MBS (src, target, encoding)
CHAR_T**        src;
unsigned char** target;
CHARSET         encoding;
#endif /* !__STDC__ */
{
	register CHAR_T*        ptrSrc           = *src;
    register unsigned char* ptrTarget        = *target;
    int                     nbBytesConverted = 0;
    unsigned char           LeadByteMark;

    switch (encoding) {
           case ISOLatin1: 
                while (*ptrSrc) {
                      *ptrTarget = (char)*ptrSrc;
                      nbBytesConverted++;
                      ptrSrc++;
                      ptrTarget++;
				}
                *ptrTarget = 0;
                break;

           case ISOLatin2: 
                while (*ptrSrc) {
                      *ptrTarget = TtaGetISOLatin2CodeFromUnicode (*ptrSrc);
                      nbBytesConverted++;
                      ptrSrc++;
                      ptrTarget++;
				}
                *ptrTarget = 0;
                break;   

           case ISOLatin6: 
                while (*ptrSrc) {
                      *ptrTarget = TtaGetISOLatin6CodeFromUnicode (*ptrSrc);
                      nbBytesConverted++;
                      ptrSrc++;
                      ptrTarget++;
				}
                *ptrTarget = 0;
                break;

           case WIN1256: 
                while (*ptrSrc) {
                      *ptrTarget = TtaGetWindows1256CPCodeFromUnicode (*ptrSrc);
                      nbBytesConverted++;
                      ptrSrc++;
                      ptrTarget++;
				}
                *ptrTarget = 0;
                break;

           case UTF8:
                while (*ptrSrc) {
                      int    nbBytesToConvert ;
                      CHAR_T WideChar = *ptrSrc++;

                      if (WideChar < 0x80) {
                         nbBytesToConvert = 1;
                         LeadByteMark     = 0x00;
					  } else if (WideChar < 0x800) {
                             nbBytesToConvert = 2;
                             LeadByteMark     = 0xC0;
					  } else if (WideChar < 0x10000) {
                             nbBytesToConvert = 3;
                             LeadByteMark     = 0xE0;
					  } else if (WideChar < 0x200000) {
                             nbBytesToConvert = 4;
                             LeadByteMark     = 0xF0;
					  } else if (WideChar < 0x4000000) {
                             nbBytesToConvert = 5;
                             LeadByteMark     = 0xF8;
					  } else if (WideChar < 0x7FFFFFFF) {
                             nbBytesToConvert = 6;
                             LeadByteMark     = 0xFC;
					  } else {
                             *ptrTarget++ = '?';
                             nbBytesToConvert = 1;
					  }

                      ptrTarget += nbBytesToConvert;
                      nbBytesConverted += nbBytesToConvert;

                      switch (nbBytesToConvert) {
                             case 6: *--ptrTarget = (WideChar | 0x80) & 0xBF;
                                     WideChar >>= 6;

                             case 5: *--ptrTarget = (WideChar | 0x80) & 0xBF;
                                     WideChar >>= 6;
 
                             case 4: *--ptrTarget = (WideChar | 0x80) & 0xBF;
                                     WideChar >>= 6;

                             case 3: *--ptrTarget = (WideChar | 0x80) & 0xBF;
                                     WideChar >>= 6;

                             case 2: *--ptrTarget = (WideChar | 0x80) & 0xBF;
                                     WideChar >>= 6;
 
                             case 1: *--ptrTarget = WideChar | LeadByteMark;
					  }
                      ptrTarget += nbBytesToConvert;
				}
                *ptrTarget = 0;
                break;
	} 
    if (nbBytesConverted > 0)
       return nbBytesConverted;
    return -1;
}
#endif /* _I18N_ */
