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
 
/*
 * Picture Handling
 * Authors: I. Vatton (INRIA)
 *          N. Layaida (INRIA) - New picture formats
 *          R. Guetari (W3C/INRIA) - Unicode, Windows version and Plug-ins
 *
 * Last modification: Jan 09 1997
 */


#include "thot_gui.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "lost.xpm"
#include "picture.h"
#include "frame.h"
#include "epsflogo.h"
#include "interface.h"
#include "fileaccess.h"
#include "thotdir.h"
#include "png.h"
#include "fileaccess.h"

#ifdef _WINDOWS
#include "winsys.h"
#endif /* _WINDOWS */

#define THOT_EXPORT extern
#include "boxes_tv.h"
#include "edit_tv.h"
#include "frame_tv.h"
#include "font_tv.h"
#include "platform_tv.h"
#undef THOT_EXPORT
#define THOT_EXPORT
#include "picture_tv.h"

#ifdef _WINDOWS 
#include "units_tv.h"

#include "wininclude.h"
#endif /* _WINDOWS */

#include "appli_f.h"
#include "epshandler_f.h"
#include "fileaccess_f.h"
#include "font_f.h"
#include "frame_f.h"
#include "gifhandler_f.h"
#include "inites_f.h"
#include "jpeghandler_f.h"
#include "memory_f.h"
#include "picture_f.h"
#include "platform_f.h"
#include "pnghandler_f.h"
#include "presrules_f.h"
#include "tree_f.h"
#include "views_f.h"
#include "xbmhandler_f.h"
#include "xpmhandler_f.h"

static char*    PictureMenu;
static Pixmap   PictureLogo;
static ThotGC   tiledGC;

char* FileExtension[] = {
      ".xbm", ".eps", ".xpm", ".gif", ".jpg", ".png"
};

static unsigned char MirrorBytes[0x100] = {
   0x00, 0x80, 0x40, 0xc0, 0x20, 0xa0, 0x60, 0xe0,
   0x10, 0x90, 0x50, 0xd0, 0x30, 0xb0, 0x70, 0xf0,
   0x08, 0x88, 0x48, 0xc8, 0x28, 0xa8, 0x68, 0xe8,
   0x18, 0x98, 0x58, 0xd8, 0x38, 0xb8, 0x78, 0xf8,
   0x04, 0x84, 0x44, 0xc4, 0x24, 0xa4, 0x64, 0xe4,
   0x14, 0x94, 0x54, 0xd4, 0x34, 0xb4, 0x74, 0xf4,
   0x0c, 0x8c, 0x4c, 0xcc, 0x2c, 0xac, 0x6c, 0xec,
   0x1c, 0x9c, 0x5c, 0xdc, 0x3c, 0xbc, 0x7c, 0xfc,
   0x02, 0x82, 0x42, 0xc2, 0x22, 0xa2, 0x62, 0xe2,
   0x12, 0x92, 0x52, 0xd2, 0x32, 0xb2, 0x72, 0xf2,
   0x0a, 0x8a, 0x4a, 0xca, 0x2a, 0xaa, 0x6a, 0xea,
   0x1a, 0x9a, 0x5a, 0xda, 0x3a, 0xba, 0x7a, 0xfa,
   0x06, 0x86, 0x46, 0xc6, 0x26, 0xa6, 0x66, 0xe6,
   0x16, 0x96, 0x56, 0xd6, 0x36, 0xb6, 0x76, 0xf6,
   0x0e, 0x8e, 0x4e, 0xce, 0x2e, 0xae, 0x6e, 0xee,
   0x1e, 0x9e, 0x5e, 0xde, 0x3e, 0xbe, 0x7e, 0xfe,
   0x01, 0x81, 0x41, 0xc1, 0x21, 0xa1, 0x61, 0xe1,
   0x11, 0x91, 0x51, 0xd1, 0x31, 0xb1, 0x71, 0xf1,
   0x09, 0x89, 0x49, 0xc9, 0x29, 0xa9, 0x69, 0xe9,
   0x19, 0x99, 0x59, 0xd9, 0x39, 0xb9, 0x79, 0xf9,
   0x05, 0x85, 0x45, 0xc5, 0x25, 0xa5, 0x65, 0xe5,
   0x15, 0x95, 0x55, 0xd5, 0x35, 0xb5, 0x75, 0xf5,
   0x0d, 0x8d, 0x4d, 0xcd, 0x2d, 0xad, 0x6d, 0xed,
   0x1d, 0x9d, 0x5d, 0xdd, 0x3d, 0xbd, 0x7d, 0xfd,
   0x03, 0x83, 0x43, 0xc3, 0x23, 0xa3, 0x63, 0xe3,
   0x13, 0x93, 0x53, 0xd3, 0x33, 0xb3, 0x73, 0xf3,
   0x0b, 0x8b, 0x4b, 0xcb, 0x2b, 0xab, 0x6b, 0xeb,
   0x1b, 0x9b, 0x5b, 0xdb, 0x3b, 0xbb, 0x7b, 0xfb,
   0x07, 0x87, 0x47, 0xc7, 0x27, 0xa7, 0x67, 0xe7,
   0x17, 0x97, 0x57, 0xd7, 0x37, 0xb7, 0x77, 0xf7,
   0x0f, 0x8f, 0x4f, 0xcf, 0x2f, 0xaf, 0x6f, 0xef,
   0x1f, 0x9f, 0x5f, 0xdf, 0x3f, 0xbf, 0x7f, 0xff
};

#ifdef _WINDOWS 

/* Macro to determine to round off the given value to the closest byte */
#define WIDTHBYTES(i)   ((i+31)/32*4)

extern ThotBool peInitialized;


#ifdef _WIN_PRINT

extern HWND currentWindow;
/*----------------------------------------------------------------------*
 *                                                                      *
 * FUNCTION: DibNumColors(VOID FAR * pv)                                *
 *                                                                      *
 * PURPOSE : Determines the number of colors in the DIB by looking at   *
 *           the BitCount filed in the info block.                      *
 *                                                                      *
 * RETURNS : The number of colors in the DIB.                           *
 *                                                                      *
  ----------------------------------------------------------------------*/
#ifdef __STDC__
WORD DibNumColors (VOID FAR* pv)
#else  /* !__STDC__ */
WORD DibNumColors (pv)
VOID FAR* pv;
#endif /* __STDC__ */
{
    INT                 bits;
    LPBITMAPINFOHEADER  lpbi;
    LPBITMAPCOREHEADER  lpbc;

    lpbi = ((LPBITMAPINFOHEADER)pv);
    lpbc = ((LPBITMAPCOREHEADER)pv);

    /*  With the BITMAPINFO format headers, the size of the palette
     *  is in biClrUsed, whereas in the BITMAPCORE - style headers, it
     *  is dependent on the bits per pixel ( = 2 raised to the power of
     *  bits/pixel).
     */
    if (lpbi->biSize != sizeof (BITMAPCOREHEADER)) {
       if (lpbi->biClrUsed != 0)
          return (WORD)lpbi->biClrUsed;
       bits = lpbi->biBitCount;
	} else
          bits = lpbc->bcBitCount;

    switch (bits) {
          case 1:  return   2;
          case 4:  return  16;
          case 8:  return 256;
          default: return   0; /* A 24 bitcount DIB has no color table */
	}
}

/*----------------------------------------------------------------------*
 *                                                                      *
 * FUNCTION:  PaletteSize(VOID FAR * pv)                                *
 *                                                                      *
 * PURPOSE :  Calculates the palette size in bytes. If the info. block  *
 *            is of the BITMAPCOREHEADER type, the number of colors is  *
 *            multiplied by 3 to give the palette size, otherwise the   *
 *            number of colors is multiplied by 4.                      *                                                       *
 *                                                                      *
 * RETURNS :  Palette size in number of bytes.                          *
 *                                                                      *
 *----------------------------------------------------------------------*/
WORD PaletteSize (VOID FAR * pv)
{
    LPBITMAPINFOHEADER lpbi;
    WORD               NumColors;

    lpbi      = (LPBITMAPINFOHEADER)pv;
    NumColors = DibNumColors(lpbi);

    if (lpbi->biSize == sizeof(BITMAPCOREHEADER))
        return (WORD)(NumColors * sizeof(RGBTRIPLE));
    else
        return (WORD)(NumColors * sizeof(RGBQUAD));
}
/*----------------------------------------------------------------------
 *                                                                      *
 *  FUNCTION   : DibInfo (LPBITMAPINFOHEADER lpbi)                      *
 *                                                                      *
 *  PURPOSE    : Retrieves the DIB info associated with a CF_DIB        *
 *               format memory block.                                   *
 *                                                                      *
 *  RETURNS    : TRUE  - if successful.                                 *
 *               FALSE - otherwise                                      *
  ----------------------------------------------------------------------*/
#ifdef __STDC__
BOOL DibInfo (LPBITMAPINFOHEADER lpbi)
#else /* !__STDC__ */
BOOL DibInfo (lpbi)
LPBITMAPINFOHEADER lpbi;
#endif /* __STDC__ */
{
    if (lpbi){
       /* fill in the default fields */
       if (lpbi->biSize != sizeof (BITMAPCOREHEADER)) {
          if (lpbi->biSizeImage == 0L)
             lpbi->biSizeImage = WIDTHBYTES (lpbi->biWidth * lpbi->biBitCount) * lpbi->biHeight;

          if (lpbi->biClrUsed == 0L)
             lpbi->biClrUsed = DibNumColors (lpbi);
		}
        return TRUE;
    }
    return FALSE;
}

/* ---------------------------------------------------------------------- *
 *                                                                        *
 * FUNCTION:  PrintDIB(HWND hWnd, HDC hDC, int x, int y, int dx, int dy)  *
 *                                                                        *
 * PURPOSE :  Set the DIB bits to the printer DC.                         *
 *                                                                        *
 * ---------------------------------------------------------------------- */
#ifdef __STDC__
void PrintDIB (LPBITMAPINFO lpBmpInfo, LPBYTE lpBits, HWND hWnd, HDC hDC, int x, int y, int dx, int dy)
#else  /* !__STDC__ */
void PrintDIB (lpBmpInfo, lpBits, hWnd, hDC, x, y, dx, dy)
LPBITMAPINFOHEADER lpBmpInfo;
LPBYTE             lpBits;
HWND               hWnd; 
HDC                hDC; 
int                x; 
int                y; 
int                dx; 
int                dy;
#endif /* __STDC__*/
{
    StretchDIBits (TtPrinterDC, x, y, dx, dy, 0, 0, lpBmpInfo->bmiHeader.biWidth, lpBmpInfo->bmiHeader.biHeight, lpBits, lpBmpInfo, DIB_RGB_COLORS, SRCCOPY);
    /* SetDIBitsToDevice (TtPrinterDC, x, y, lpBmpInfo->biWidth, lpBmpInfo->biHeight, 0, 0, 0, lpBmpInfo->biHeight, lpBits, lpBmpInfo, DIB_RGB_COLORS); */
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
LPBITMAPINFO CreateBitmapInfoStruct(HWND hwnd, HBITMAP hBmp) 
#else /* !__STDC__ */
LPBITMAPINFO CreateBitmapInfoStruct(hwnd, hBmp) 
HWND    hwnd; 
HBITMAP hBmp; 
#endif /* __STDC__ */
{ 
    BITMAP      bmp; 
    LPBITMAPINFO pbmi; 
    WORD        cClrBits; 
 
    /* Retrieve the bitmap's color format, width, and height. */ 
 
    if (!GetObject (hBmp, sizeof(BITMAP), (LPSTR)&bmp))
       WinErrorBox (hwnd, TEXT("CreateBitmapInfoStruct (1)")); 
 

    /* Convert the color format to a count of bits. */ 
 
    cClrBits = (WORD) (bmp.bmPlanes * bmp.bmBitsPixel); 
 
    if (cClrBits != 1) { 
       if (cClrBits <= 4) 
          cClrBits = 4; 
       else if (cClrBits <= 8) 
            cClrBits = 8; 
       else if (cClrBits <= 16) 
            cClrBits = 16; 
       else if (cClrBits <= 24) 
            cClrBits = 24; 
       else 
           cClrBits = 32; 
	}
 
    /* 
     * Allocate memory for the BITMAPINFO structure. (This structure 
     * contains a BITMAPINFOHEADER structure and an array of RGBQUAD data 
     * structures.) 
     */ 
 
    if (cClrBits != 24) 
       pbmi = (LPBITMAPINFO) LocalAlloc (LPTR, sizeof (BITMAPINFOHEADER) + sizeof (RGBQUAD) * (2^cClrBits)); 
 
    /* 
     * There is no RGBQUAD array for the 24-bit-per-pixel format. 
     */ 
 
    else 
         pbmi = (LPBITMAPINFO) LocalAlloc(LPTR, sizeof(BITMAPINFOHEADER)); 
 
    /* Initialize the fields in the BITMAPINFO structure. */ 
 
    pbmi->bmiHeader.biSize     = sizeof (BITMAPINFOHEADER); 
    pbmi->bmiHeader.biWidth    = bmp.bmWidth; 
    pbmi->bmiHeader.biHeight   = bmp.bmHeight; 
    pbmi->bmiHeader.biPlanes   = bmp.bmPlanes; 
    pbmi->bmiHeader.biBitCount = bmp.bmBitsPixel; 
    if (cClrBits < 24) 
       pbmi->bmiHeader.biClrUsed = 2^cClrBits; 

    /* If the bitmap is not compressed, set the BI_RGB flag. */  
    pbmi->bmiHeader.biCompression = BI_RGB; 
 
    /* 
     * Compute the number of bytes in the array of color 
     * indices and store the result in biSizeImage. 
     */ 
 
    pbmi->bmiHeader.biSizeImage = (pbmi->bmiHeader.biWidth + 7) / 8 * pbmi->bmiHeader.biHeight * cClrBits; 
 
    /* 
     * Set biClrImportant to 0, indicating that all of the 
     * device colors are important. 
     */ 
 
    pbmi->bmiHeader.biClrImportant = 0; 
 
    return pbmi; 
} 

/*----------------------------------------------------------------------*
 *                                                                      *
 * FUNCTION: GetTransparentDIBits (HBITMAP pixmap)                      *
 *                                                                      *
 *----------------------------------------------------------------------*/
#ifdef __STDC__
LPBYTE GetTransparentDIBits (int frame, HBITMAP pixmap, int x, int y, int width, int height, int red, int green, int blue, LPBITMAPINFO* lpBmpInfo)
#else  /* !__STDC__ */
LPBYTE GetTransparentDIBits (frame, pixmap, x, y, width, height, red, green, blue, lpBmpInfo)
int     frame;
HBITMAP pixmap;
int          x;
int          y;
int          width; 
int          height,;
int          red;
int          green;
int          blue;
LPBITMAPINFO* lpBmpInfo;
#endif /* __STDC__ */
{
   HDC      hDC = GetDC (currentWindow);
   HDC      hImageDC;
   HDC      hOrDC;
   HDC      hAndDC;
   HDC      hInvAndDC;
   HDC      hDestDC;
   HBITMAP  bitmap;
   HBITMAP  bitmapOr;
   HBITMAP  pOldBitmapOr;
   HBITMAP  bitmapAnd;
   HBITMAP  pOldBitmapAnd;
   HBITMAP  bitmapInvAnd;
   HBITMAP  pOldBitmapInvAnd;
   HBITMAP  bitmapDest;
   HBITMAP  pOldBitmapDest;
   COLORREF crColor = RGB (red, green, blue);
   COLORREF crOldBkColor ;
   LPBYTE   lpBits;
   HBRUSH   hBrush, hOldBrush;

   hBrush = CreateSolidBrush (RGB (255, 255, 255));

   hImageDC = CreateCompatibleDC (hDC);
   bitmap   = SelectObject (hImageDC, pixmap);
   SetMapMode (hImageDC, GetMapMode (hDC));
   
   /* newBmp = SelectObject (hNewDC, pixmap);
   SetMapMode (hNewDC, GetMapMode (hDC)); */


   /* SetBkMode (hNewDC, OPAQUE);
   SetBkColor (hNewDC, PALETTERGB (255, 255, 255)); */
   
   hOrDC = CreateCompatibleDC (hDC);
   SetMapMode (hOrDC, GetMapMode (hDC));
   bitmapOr = CreateCompatibleBitmap (hImageDC, width, height);
   pOldBitmapOr = SelectObject (hOrDC, bitmapOr);
   BitBlt (hOrDC, 0, 0, width, height, hImageDC, 0, 0, SRCCOPY);

   hAndDC = CreateCompatibleDC (hDC);
   SetMapMode (hAndDC, GetMapMode (hDC));
   bitmapAnd = CreateBitmap (width, height, 1, 1, NULL);
   pOldBitmapAnd = SelectObject (hAndDC, bitmapAnd);

   crOldBkColor = SetBkColor (hImageDC, crColor);
   BitBlt (hAndDC, 0, 0, width, height, hImageDC, 0, 0, SRCCOPY);

   SetBkColor (hImageDC, crOldBkColor);

   hInvAndDC = CreateCompatibleDC (hDC);
   SetMapMode (hInvAndDC, GetMapMode (hDC));
   bitmapInvAnd = CreateBitmap (width, height, 1, 1, NULL);
   pOldBitmapInvAnd = SelectObject (hInvAndDC, bitmapInvAnd);
   BitBlt (hInvAndDC, 0, 0, width, height, hAndDC, 0, 0, NOTSRCCOPY);

   BitBlt (hOrDC, 0, 0, width, height, hInvAndDC, 0, 0, SRCAND);

   hDestDC = CreateCompatibleDC (hDC);
   SetMapMode (hDestDC, GetMapMode (hDC));
   /*******/
   hOldBrush = SelectObject (hDestDC, hBrush);
   PatBlt (hDestDC, 0, 0, width, height, PATCOPY);
   /*******/
   bitmapDest = CreateCompatibleBitmap (hImageDC, width, height);
   pOldBitmapDest = SelectObject (hDestDC, bitmapDest);
   /* BitBlt (hDestDC, 0, 0, width, height, hDC, x * ScreenDPI / PrinterDPI, y * ScreenDPI / PrinterDPI, SRCCOPY); */
   BitBlt (hDestDC, 0, 0, width, height, hImageDC, 0, 0, SRCCOPY);

   BitBlt (hDestDC, 0, 0, width, height, hAndDC, 0, 0, SRCAND);

   BitBlt (hDestDC, 0, 0, width, height, hOrDC, 0, 0, SRCINVERT);

   /* Getting DIBits */

   *lpBmpInfo = CreateBitmapInfoStruct (FrRef[frame], bitmapDest);

   lpBits = (LPBYTE) GlobalAlloc (GMEM_FIXED, (*lpBmpInfo)->bmiHeader.biSizeImage);
   if (!lpBits)
      WinErrorBox (NULL, TEXT("GetTransparentDIBits (1)"));

   if (!GetDIBits (hDC, (HBITMAP) bitmapDest, 0, (WORD)(*lpBmpInfo)->bmiHeader.biHeight, lpBits, *lpBmpInfo, DIB_RGB_COLORS))
      WinErrorBox (NULL, TEXT("GetTransparentDIBits (2)")); 

   SelectObject (hDestDC, pOldBitmapDest);
   SelectObject (hInvAndDC, pOldBitmapInvAnd);
   SelectObject (hAndDC, pOldBitmapAnd);
   SelectObject (hOrDC, pOldBitmapOr);
   SelectObject (hImageDC, bitmap);

   if (hDestDC && !DeleteDC (hDestDC))
      WinErrorBox (WIN_Main_Wd, TEXT("GetTransparentDIBits (3)"));
   hDestDC = (HDC) 0;
   if (hInvAndDC && !DeleteDC (hInvAndDC))
      WinErrorBox (WIN_Main_Wd, TEXT("GetTransparentDIBits (4)"));
   hInvAndDC = (HDC) 0;
   if (hAndDC && !DeleteDC (hAndDC))
      WinErrorBox (WIN_Main_Wd, TEXT("GetTransparentDIBits (5)"));
   hAndDC = (HDC) 0;
   if (hOrDC && !DeleteDC (hOrDC))
      WinErrorBox (WIN_Main_Wd, TEXT("GetTransparentDIBits (6)"));
   hOrDC = (HDC) 0;
   if (hImageDC && !DeleteDC (hImageDC))
      WinErrorBox (WIN_Main_Wd, TEXT("GetTransparentDIBits (7)"));
   hImageDC = (HDC) 0;
   if (hDC && !DeleteDC (hDC))
      WinErrorBox (WIN_Main_Wd, TEXT("GetTransparentDIBits (8)"));
   hDC = (HDC) 0;
   if (bitmap && !DeleteObject (bitmap))
      WinErrorBox (WIN_Main_Wd, TEXT("GetTransparentDIBits (9)"));
   bitmap = (HBITMAP) 0;
   if (bitmapOr && !DeleteObject (bitmapOr))
      WinErrorBox (WIN_Main_Wd, TEXT("GetTransparentDIBits (10)"));
   bitmapOr = (HBITMAP) 0;
   if (pOldBitmapOr && !DeleteObject (pOldBitmapOr))
      WinErrorBox (WIN_Main_Wd, TEXT("GetTransparentDIBits (11)"));
   pOldBitmapOr = (HBITMAP) 0;
   if (bitmapAnd && !DeleteObject (bitmapAnd))
      WinErrorBox (WIN_Main_Wd, TEXT("GetTransparentDIBits (12)"));
   bitmapAnd = (HBITMAP) 0;
   if (pOldBitmapAnd && !DeleteObject (pOldBitmapAnd))
      WinErrorBox (WIN_Main_Wd, TEXT("GetTransparentDIBits (13)"));
   pOldBitmapAnd = (HBITMAP) 0;
   if (bitmapInvAnd && !DeleteObject (bitmapInvAnd))
      WinErrorBox (WIN_Main_Wd, TEXT("GetTransparentDIBits (14)"));
   bitmapInvAnd = (HBITMAP) 0;
   if (pOldBitmapInvAnd && !DeleteObject (pOldBitmapInvAnd))
      WinErrorBox (WIN_Main_Wd, TEXT("GetTransparentDIBits (15)"));
   pOldBitmapInvAnd = (HBITMAP) 0;
   if (bitmapDest && !DeleteObject (bitmapDest))
      WinErrorBox (WIN_Main_Wd, TEXT("GetTransparentDIBits (16)"));
   bitmapDest = (HBITMAP) 0;
   if (pOldBitmapDest && !DeleteObject (pOldBitmapDest))
      WinErrorBox (WIN_Main_Wd, TEXT("GetTransparentDIBits (17)"));
   pOldBitmapDest = (HBITMAP) 0;

   return lpBits;
}
#endif /* _WIN_PRINT */

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void WIN_LayoutTransparentPicture (HDC hDC, HBITMAP pixmap, int x, int y, int width, int height, int red, int green, int blue)
#else  /* !__STDC__ */
void WIN_LayoutTransparentPicture (hDC, pixmap, x, y, width, height, red, green, blue)
HDC      hDC;
HBITMAP  pixmap; 
int      x;
int      y;
int      width; 
int      height; 
int      red;
int      green;
int      blue;
#endif /* __STDC__ */
{
   HDC      hImageDC;
   HDC      hOrDC;
   HDC      hAndDC;
   HDC      hInvAndDC;
   HDC      hDestDC;
   HBITMAP  bitmap;
   HBITMAP  bitmapOr;
   HBITMAP  pOldBitmapOr;
   HBITMAP  bitmapAnd;
   HBITMAP  pOldBitmapAnd;
   HBITMAP  bitmapInvAnd;
   HBITMAP  pOldBitmapInvAnd;
   HBITMAP  bitmapDest;
   HBITMAP  pOldBitmapDest;
   COLORREF crColor = RGB (red, green, blue);
   COLORREF crOldBkColor ;

   hImageDC = CreateCompatibleDC (hDC);
   bitmap = SelectObject (hImageDC, pixmap);
   SetMapMode (hImageDC, GetMapMode (hDC));
   
   hOrDC = CreateCompatibleDC (hDC);
   SetMapMode (hOrDC, GetMapMode (hDC));
   bitmapOr = CreateCompatibleBitmap (hImageDC, width, height);
   pOldBitmapOr = SelectObject (hOrDC, bitmapOr);
   BitBlt (hOrDC, 0, 0, width, height, hImageDC, 0, 0, SRCCOPY);

   hAndDC = CreateCompatibleDC (hDC);
   SetMapMode (hAndDC, GetMapMode (hDC));
   bitmapAnd = CreateBitmap (width, height, 1, 1, NULL);
   pOldBitmapAnd = SelectObject (hAndDC, bitmapAnd);

   crOldBkColor = SetBkColor (hImageDC, crColor);
   BitBlt (hAndDC, 0, 0, width, height, hImageDC, 0, 0, SRCCOPY);

   SetBkColor (hImageDC, crOldBkColor);

   hInvAndDC = CreateCompatibleDC (hDC);
   SetMapMode (hInvAndDC, GetMapMode (hDC));
   bitmapInvAnd = CreateBitmap (width, height, 1, 1, NULL);
   pOldBitmapInvAnd = SelectObject (hInvAndDC, bitmapInvAnd);
   BitBlt (hInvAndDC, 0, 0, width, height, hAndDC, 0, 0, NOTSRCCOPY);

   BitBlt (hOrDC, 0, 0, width, height, hInvAndDC, 0, 0, SRCAND);

   hDestDC = CreateCompatibleDC (hDC);
   SetMapMode (hDestDC, GetMapMode (hDC));
   bitmapDest = CreateCompatibleBitmap (hImageDC, width, height);
   pOldBitmapDest = SelectObject (hDestDC, bitmapDest);
   BitBlt (hDestDC, 0, 0, width, height, hDC, x, y, SRCCOPY);

   BitBlt (hDestDC, 0, 0, width, height, hAndDC, 0, 0, SRCAND);

   BitBlt (hDestDC, 0, 0, width, height, hOrDC, 0, 0, SRCINVERT);

   BitBlt (hDC, x, y, width, height, hDestDC, 0, 0, SRCCOPY);

   SelectObject (hDestDC, pOldBitmapDest);
   SelectObject (hInvAndDC, pOldBitmapInvAnd);
   SelectObject (hAndDC, pOldBitmapAnd);
   SelectObject (hOrDC, pOldBitmapOr);
   SelectObject (hImageDC, bitmap);

   if (hDestDC && !DeleteDC (hDestDC))
      WinErrorBox (WIN_Main_Wd, TEXT("WIN_LayoutTransparentPicture (1)"));
   hDestDC = (HDC) 0;
   if (hInvAndDC && !DeleteDC (hInvAndDC))
      WinErrorBox (WIN_Main_Wd, TEXT("WIN_LayoutTransparentPicture (2)"));
   hInvAndDC = (HDC) 0;
   if (hAndDC && !DeleteDC (hAndDC))
      WinErrorBox (WIN_Main_Wd, TEXT("WIN_LayoutTransparentPicture (3)"));
   hAndDC = (HDC) 0;
   if (hOrDC && !DeleteDC (hOrDC))
      WinErrorBox (WIN_Main_Wd, TEXT("WIN_LayoutTransparentPicture (4)"));
   hOrDC = (HDC) 0;
   if (hImageDC && !DeleteDC (hImageDC))
      WinErrorBox (WIN_Main_Wd, TEXT("WIN_LayoutTransparentPicture (5)"));
   hImageDC = (HDC) 0;

   if (bitmap && !DeleteObject (bitmap))
      WinErrorBox (WIN_Main_Wd, TEXT("WIN_LayoutTransparentPicture (6)"));
   bitmap = (HBITMAP) 0;
   if (bitmapOr && !DeleteObject (bitmapOr))
      WinErrorBox (WIN_Main_Wd, TEXT("WIN_LayoutTransparentPicture (7)"));
   bitmapOr = (HBITMAP) 0;
   if (pOldBitmapOr && !DeleteObject (pOldBitmapOr))
      WinErrorBox (WIN_Main_Wd, TEXT("WIN_LayoutTransparentPicture (8)"));
   pOldBitmapOr = (HBITMAP) 0;
   if (bitmapAnd && !DeleteObject (bitmapAnd))
      WinErrorBox (WIN_Main_Wd, TEXT("WIN_LayoutTransparentPicture (9)"));
   bitmapAnd = (HBITMAP) 0;
   if (pOldBitmapAnd && !DeleteObject (pOldBitmapAnd))
      WinErrorBox (WIN_Main_Wd, TEXT("WIN_LayoutTransparentPicture (10)"));
   pOldBitmapAnd = (HBITMAP) 0;
   if (bitmapInvAnd && !DeleteObject (bitmapInvAnd))
      WinErrorBox (WIN_Main_Wd, TEXT("WIN_LayoutTransparentPicture (11)"));
   bitmapInvAnd = (HBITMAP) 0;
   if (pOldBitmapInvAnd && !DeleteObject (pOldBitmapInvAnd))
      WinErrorBox (WIN_Main_Wd, TEXT("WIN_LayoutTransparentPicture (12)"));
   pOldBitmapInvAnd = (HBITMAP) 0;
   if (bitmapDest && !DeleteObject (bitmapDest))
      WinErrorBox (WIN_Main_Wd, TEXT("WIN_LayoutTransparentPicture (13)"));
   bitmapDest = (HBITMAP) 0;
   if (pOldBitmapDest && !DeleteObject (pOldBitmapDest))
      WinErrorBox (WIN_Main_Wd, TEXT("WIN_LayoutTransparentPicture (14)"));
   pOldBitmapDest = (HBITMAP) 0;
}
#endif /* _WINDOWS */

/*----------------------------------------------------------------------
   Match_Format returns TRUE if the considered header file matches   
   the image file description, FALSE in the the other cases        
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static ThotBool     Match_Format (int typeImage, STRING fileName)
#else  /* __STDC__ */
static ThotBool     Match_Format (typeImage, fileName)
int                 typeImage;
STRING              fileName;
#endif /* __STDC__ */
{
   if (PictureHandlerTable[typeImage].Match_Format != NULL)
      return (*(PictureHandlerTable[typeImage].Match_Format)) (fileName);
   return FALSE;
}


/*----------------------------------------------------------------------
   FreePixmap frees the pixmap allocated in the X server if it is not
   empty and if it is not one of the internal images        
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                FreePixmap (Pixmap pixmap)
#else  /* __STDC__ */
void                FreePixmap (pixmap)
Pixmap              pixmap;

#endif /* __STDC__ */
{
  if (pixmap != None && pixmap != PictureLogo && pixmap != EpsfPictureLogo)
#ifndef _WINDOWS
    XFreePixmap (TtDisplay, pixmap);
#else  /* _WINDOWS */
  if (!DeleteObject ((HBITMAP)pixmap))
    WinErrorBox (WIN_Main_Wd, TEXT("FreePixmap"));
#endif /* _WINDOWS */
}


/*----------------------------------------------------------------------
   Picture_Center updates the parameters xTranslate, yTranslate,     
   picXOrg, picYOrg depending on the values of PicWArea,     
   PicHArea, wFrame, hFrame and pres.                            
   - If we use ReScale, the tranlation is performed        
   in one direction.                                       
   - If we use FillFrame, there's no translation           
   - if we use RealSize we translate to achieve            
   the centering               .                           
   if the picture size is greater than the frame then      
   picXOrg or picYOrg are positive.                            
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void Picture_Center (int picWArea, int picHArea, int wFrame, int hFrame, PictureScaling pres, int *xTranslate, int *yTranslate, int *picXOrg, int *picYOrg)
#else  /* __STDC__ */
static void Picture_Center (picWArea, picHArea, wFrame, hFrame, pres, xTranslate, yTranslate, picXOrg, picYOrg)
int                 picWArea;
int                 picHArea;
int                 wFrame;
int                 hFrame;
PictureScaling      pres;
int                *xTranslate;
int                *yTranslate;
int                *picXOrg;
int                *picYOrg;

#endif /* __STDC__ */
{
   float               Rapw, Raph;

   /* the box has the wFrame, hFrame dimensions */
   /* the picture has  picWArea, picHArea size */

   *picXOrg = 0;
   *picYOrg = 0;
   *xTranslate = 0;
   *yTranslate = 0;

   switch (pres)
	 {
	    case ReScale:
	       Rapw = (float) wFrame / (float) picWArea;
	       Raph = (float) hFrame / (float) picHArea;
	       if (Rapw <= Raph)
		  *yTranslate = (int) ((hFrame - (picHArea * Rapw)) / 2);
	       else
		  *xTranslate = (int) ((wFrame - (picWArea * Raph)) / 2);
	       break;
	    case RealSize:
	    case FillFrame:
	    case XRepeat:
	    case YRepeat:
	       /* we center the picture in the box frame */
	       *xTranslate = (wFrame - picWArea) / 2;
	       *yTranslate = (hFrame - picHArea) / 2;
	       break;
	 }
   if (picWArea > wFrame)
     {
	*picXOrg = -*xTranslate;
	*xTranslate = 0;
     }
   if (picHArea > hFrame)
     {
	*picYOrg = -*yTranslate;
	*yTranslate = 0;
     }
}

/*----------------------------------------------------------------------
   SetPictureClipping clips the picture into boundaries.              
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         SetPictureClipping (int *picWArea, int *picHArea, int wFrame, int hFrame, PictInfo *imageDesc)
#else  /* __STDC__ */
static void         SetPictureClipping (picWArea, picHArea, wFrame, hFrame, imageDesc)
int                *picWArea;
int                *picHArea;
int                 wFrame;
int                 hFrame;
PictInfo           *imageDesc;

#endif /* __STDC__ */
{
   if ((imageDesc->PicWArea == 0 && imageDesc->PicHArea == 0) ||
       (imageDesc->PicWArea > MAX_PICT_SIZE ||
	imageDesc->PicHArea > MAX_PICT_SIZE))
     {
	*picWArea = wFrame;
	*picHArea = hFrame;
     }
   else
     {
	*picWArea = imageDesc->PicWArea;
	*picHArea = imageDesc->PicHArea;
     }
}

/*----------------------------------------------------------------------
   LayoutPicture performs the layout of pixmap on the screen described 
   by the drawable.                                                          
   if picXOrg or picYOrg are postive, the copy operation is shifted      
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         LayoutPicture (Pixmap pixmap, Drawable drawable, int picXOrg, int picYOrg, int w, int h, int xFrame, int yFrame, int frame, PictInfo *imageDesc)
#else  /* __STDC__ */
static void         
 (pixmap, drawable, picXOrg, picYOrg, w, h, xFrame, yFrame, frame, imageDesc)
Pixmap              pixmap;
Drawable            drawable;
int                 picXOrg;
int                 picYOrg;
int                 w;
int                 h;
int                 xFrame;
int                 yFrame;
int                 frame;
PictInfo           *imageDesc;
#endif /* __STDC__ */
{

  ViewFrame*        pFrame;
  int               delta;
# ifndef _WINDOWS
  XRectangle        rect;
  XGCValues         values;
  unsigned int      valuemask;
# endif /* _WINDOWS */

# ifdef _WINDOWS
  HDC     hMemDC;
  HBITMAP hBkgBmp;
  HBITMAP hOldBitmap1;
  HBITMAP hOldBitmap2;
  HDC     hOrigDC;
  BITMAP  bm;
  POINT   ptOrg, ptSize;
  int     x, y, clipWidth, clipHeight;
  int     nbPalColors ;
  HRGN    hrgn;
# endif /* _WINDOWS */

  if (picXOrg < 0)
    {
      xFrame = xFrame - picXOrg;
      picXOrg = 0;
    }
  if (picYOrg < 0)
    {
      yFrame = yFrame - picYOrg;
      picYOrg = 0;
    }

# ifdef _WINDOWS 		 
  if (!TtIsTrueColor) {
     WIN_InitSystemColors (TtDisplay);
     SelectPalette (TtDisplay, TtCmap, FALSE);
     nbPalColors = RealizePalette (TtDisplay);
  }
# endif /* _WINDOWS */

  pFrame = &ViewFrameTable[frame - 1];
  if (pixmap != None)
    {
      switch (imageDesc->PicPresent)
	{
	case ReScale:
#         ifndef _WINDOWS
#ifndef _GTK
	  if (imageDesc->PicMask)
	    {
	      XSetClipOrigin (TtDisplay, TtGraphicGC, xFrame - picXOrg, yFrame - picYOrg);
	      XSetClipMask (TtDisplay, TtGraphicGC, imageDesc->PicMask);
	    }
	  XCopyArea (TtDisplay, pixmap, drawable, TtGraphicGC, picXOrg, picYOrg, w, h, xFrame, yFrame);
	   if (imageDesc->PicMask)
	     {
	       XSetClipMask (TtDisplay, TtGraphicGC, None);
	       XSetClipOrigin (TtDisplay, TtGraphicGC, 0, 0);
	     }
#endif /* _GTK */
#         else /* _WINDOWS */
	case RealSize:
	  if ((imageDesc->bgRed == -1 && imageDesc->bgGreen == -1 && imageDesc->bgBlue == -1) || imageDesc->PicType == -1) {
	    hMemDC = CreateCompatibleDC (TtDisplay);
	    hOldBitmap1 = SelectObject (hMemDC, pixmap);
	    SetMapMode (hMemDC, GetMapMode (TtDisplay));
	    GetObject (pixmap, sizeof (BITMAP), (LPVOID) &bm) ;
	    ptSize.x = bm.bmWidth;
	    ptSize.y = bm.bmHeight;
	    DPtoLP (TtDisplay, &ptSize, 1);
	    ptOrg.x = 0;
	    ptOrg.y = 0;
	    DPtoLP (hMemDC, &ptOrg, 1);
	    
	    if (!BitBlt (TtDisplay, xFrame, yFrame, ptSize.x, ptSize.y, hMemDC, ptOrg.x, ptOrg.y, SRCCOPY))
           WinErrorBox (NULL, TEXT("LayoutPicture (1)"));
		SelectObject (hMemDC, hOldBitmap1);
	    if (hMemDC && !DeleteDC (hMemDC))
           WinErrorBox (NULL, TEXT("LayoutPicture (2)"));
        hMemDC = (HDC) 0;
	  } else {
           WIN_LayoutTransparentPicture (TtDisplay, pixmap, xFrame, yFrame, w, h, imageDesc->bgRed, imageDesc->bgGreen, imageDesc->bgBlue);
	  }
#         endif /* _WINDOWS */
	  break;
	  
	case FillFrame:
	case XRepeat:
	case YRepeat:
#         ifndef _WINDOWS
	case RealSize:

          valuemask = GCTile | GCFillStyle | GCTileStipXOrigin | GCTileStipYOrigin;
	  values.fill_style = FillTiled;
          values.tile = pixmap;
          values.ts_x_origin = xFrame;
          values.ts_y_origin = yFrame;
#ifndef _GTK 
          XChangeGC (TtDisplay, tiledGC, valuemask, &values);
#endif /* _GTK */
	  
          rect.x = pFrame->FrClipXBegin;
          rect.y = pFrame->FrClipYBegin;
          rect.width = pFrame->FrClipXEnd - rect.x;
          rect.height = pFrame->FrClipYEnd - rect.y;
          rect.x -= pFrame->FrXOrg;
          rect.y -= pFrame->FrYOrg;
          if (imageDesc->PicPresent == FillFrame ||
	      imageDesc->PicPresent == YRepeat)
	    {
	      /* clipping height is done by the box height */
	      if (rect.y < yFrame)
		{
		  /* reduce the height in delta value */
		  rect.height = rect.height + rect.y - yFrame;
		  rect.y = yFrame;
		}
	      if (rect.height > h)
		rect.height = h;
	    }
	  else
	    {
               /* clipping height is done by the image height */
               delta = yFrame + imageDesc->PicHArea - rect.y;
               if (delta <= 0)
                  rect.height = 0;
               else
                  rect.height = delta;
	    }
	  
          if (imageDesc->PicPresent == FillFrame ||
	      imageDesc->PicPresent == XRepeat)
	    {
             /* clipping width is done by the box width */
             if (rect.x < xFrame)
	       {
		 /* reduce the width in delta value */
		 rect.width = rect.width +rect.x - xFrame;
		 rect.x = xFrame;
	       }
	     if (rect.width > w)
               rect.width = w;
	    }
	  else
	    {
               /* clipping width is done by the image width */
               delta = xFrame + imageDesc->PicWArea - rect.x;
               if (delta <= 0)
                  rect.width = 0;
               else
                  rect.width = delta;
	    }
#ifndef _GTK 
	  if (imageDesc->PicMask)
	    {
	      XSetClipOrigin (TtDisplay, tiledGC, xFrame - picXOrg, yFrame - picYOrg);
	      XSetClipMask (TtDisplay, tiledGC, imageDesc->PicMask);
	    }
	  else
	    XSetClipRectangles (TtDisplay, tiledGC, 0, 0, &rect, 1, Unsorted);
          XFillRectangle (TtDisplay, drawable, tiledGC, xFrame, yFrame, w, h);
          /* remove clipping */
          rect.x = 0;
          rect.y = 0;
          rect.width = MAX_SIZE;
          rect.height = MAX_SIZE;
          XSetClipRectangles (TtDisplay, tiledGC, 0, 0, &rect, 1, Unsorted);
	   if (imageDesc->PicMask)
	     {
	       XSetClipMask (TtDisplay, tiledGC, None);
	       XSetClipOrigin (TtDisplay, tiledGC, 0, 0);
	     }
#endif /* _GTK */
#         else  /* _WINDOWS */
          x          = pFrame->FrClipXBegin;
          y          = pFrame->FrClipYBegin;
          clipWidth  = pFrame->FrClipXEnd - x;
          clipHeight = pFrame->FrClipYEnd - y;
          x          -= pFrame->FrXOrg;
          y          -= pFrame->FrYOrg;
          if (imageDesc->PicPresent == FillFrame || imageDesc->PicPresent == YRepeat) {
             /* clipping height is done by the box height */
             if (y < yFrame) {
                /* reduce the height in delta value */
                clipHeight = clipHeight + y - yFrame;
                y = yFrame;
             }
             if (clipHeight > h)
                clipHeight = h;
          } else {
               /* clipping height is done by the image height */
               delta = yFrame + imageDesc->PicHArea - y;
               if (delta <= 0)
                  clipHeight = 0;
               else
                  clipHeight = delta;
          }
	  
          if (imageDesc->PicPresent == FillFrame || imageDesc->PicPresent == XRepeat) {
             /* clipping width is done by the box width */
             if (x < xFrame) {
                /* reduce the width in delta value */
                clipWidth = clipWidth + x - xFrame;
                x = xFrame;
             }
             if (clipWidth > w)
                clipWidth = w;
          } else {
               /* clipping width is done by the image width */
               delta = xFrame + imageDesc->PicWArea - x;
               if (delta <= 0)
                  clipWidth = 0;
               else
                  clipWidth = delta;
		  }
	  
          hMemDC  = CreateCompatibleDC (TtDisplay);
          hBkgBmp = CreateCompatibleBitmap (TtDisplay, w, h);
          hOrigDC = CreateCompatibleDC (TtDisplay);
          hrgn = CreateRectRgn (x, y, x + clipWidth, y + clipHeight);
          SelectClipRgn(TtDisplay, hrgn); 
          hOldBitmap1 = SelectObject (hOrigDC, pixmap);
          hOldBitmap2 = SelectObject (hMemDC, hBkgBmp);
          
          y = 0;

	  do {
             x = 0;
             do {
                if (!BitBlt (hMemDC, x, y, imageDesc->PicWArea, imageDesc->PicHArea, hOrigDC, 0, 0, SRCCOPY))
                   WinErrorBox (WIN_Main_Wd, TEXT("LayoutPicture (3)"));
                x += imageDesc->PicWArea;
             } while (x < (w - xFrame));
			 y += imageDesc->PicHArea;
		  } while (y < (h - yFrame));

          BitBlt (TtDisplay, xFrame, yFrame, w, h, hMemDC, 0, 0, SRCCOPY);

		  SelectObject (hOrigDC, hOldBitmap1);
		  SelectObject (hMemDC, hOldBitmap2);

          SelectClipRgn(TtDisplay, NULL); 

          if (hMemDC && !DeleteDC (hMemDC))
             WinErrorBox (WIN_Main_Wd, TEXT("LayoutPicture (4)"));
          hMemDC = (HDC) 0;
          if (hOrigDC && !DeleteDC (hOrigDC))
             WinErrorBox (WIN_Main_Wd, TEXT("LayoutPicture (5)"));
          hOrigDC = (HDC) 0;
          if (hBkgBmp && !DeleteObject (hBkgBmp))
             WinErrorBox (WIN_Main_Wd, TEXT("LayoutPicture (6)"));
          hBkgBmp = (HBITMAP) 0;
		  if (hrgn && !DeleteObject (hrgn))
             WinErrorBox (NULL, TEXT("LayoutPicture (7)"));
          hrgn = (HRGN) 0;
#         endif /* _WINDOWS */
	  break;
	}
    }
}



/*----------------------------------------------------------------------
   IsValid retourne FALSE si le pixmap contenu dans imageDesc       
   est vide. On retourne TRUE s'il est egal aux images     
   predefinies BadPixmap.                                  
   - if we use RealSize, we  return TRUE.                  
   - if we use  ReScale, we return TRUE                    
   the box have one of the two  dimensions a least equals  
   to the one of the pixmap.                               
   - if we use  ReScale FillFrame, we return TRUE if the   
   frame box  has the same size than the pixmap in         
   both directions.                                        
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static ThotBool     IsValid (int wFrame, int hFrame, PictInfo* imageDesc)
#else  /* __STDC__ */
static ThotBool     IsValid (wFrame, hFrame, imageDesc)
int                 wFrame;
int                 hFrame;
PictInfo           *imageDesc;
#endif /* __STDC__ */
{
  if (imageDesc->PicPixmap == None)
    return (FALSE);
  else if (imageDesc->PicPresent == ReScale &&
	   (imageDesc->PicWArea != wFrame || imageDesc->PicHArea != hFrame))
    return (FALSE);
  else
    return (TRUE);
}


/*----------------------------------------------------------------------
   GetPictureFormat returns the format of a file picture           
   the file  fileName or UNKNOWN_FORMAT if not recognized          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static int          GetPictureFormat (STRING fileName)
#else  /* __STDC__ */
static int          GetPictureFormat (fileName)
STRING              fileName;

#endif /* __STDC__ */
{
   int                 i;
   int                 l = 0;

   i = 0 ;
   l = ustrlen (fileName);

   while (i < HandlersCounter) {
         if (i >= InlineHandlers)
            currentExtraHandler = i - InlineHandlers;
         if (Match_Format (i, fileName))
            return i ;
         ++i ;
   }
   return UNKNOWN_FORMAT;
}


/*----------------------------------------------------------------------
   PictureFileOk returns Unsupported_Format if the file does not exist
   - if typeImage is defined it returns Supported_Format if the 
   file is of type typeImage, else Corrupted_File.            
   - if typeImage is not defined, it is updated and we return   
   Supported_Format is of an known type                       
   and Corrupted_File in the other cases                      
  ----------------------------------------------------------------------*/
#ifdef __STDC__
Picture_Report PictureFileOk (STRING fileName, int *typeImage)
#else  /* __STDC__ */
Picture_Report PictureFileOk (fileName, typeImage)
STRING              fileName;
int                *typeImage;

#endif /* __STDC__ */
{
   Picture_Report      status;

   /* we consider only the supported image formats */

   if (*typeImage >= MAX_PICT_FORMATS || *typeImage < 0)
      *typeImage = UNKNOWN_FORMAT;

   if (TtaFileExist (fileName))
     {
	if (*typeImage == UNKNOWN_FORMAT)
	  {
	     *typeImage = GetPictureFormat (fileName);
	     if (*typeImage == UNKNOWN_FORMAT)
		status = Corrupted_File;
	     else
		status = Supported_Format;
	  }
	else
	  {
	     if (Match_Format (*typeImage, fileName))
		status = Supported_Format;
	     else
		status = Corrupted_File;
	  }
     }
   else
     {
	status = Unsupported_Format;
     }

   return status;
}

/*----------------------------------------------------------------------
   Private Initializations of picture handlers and the visual type 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                InitPictureHandlers (ThotBool printing)
#else  /* __STDC__ */
void                InitPictureHandlers (printing)
ThotBool            printing;

#endif /* __STDC__ */
{
#ifndef _WINDOWS
#ifdef _GTK
   /* initialize Graphic context to display pictures */
   TtGraphicGC = gdk_gc_new (DefaultDrawable);
   gdk_rgb_gc_set_foreground (TtGraphicGC, Black_Color);
   gdk_rgb_gc_set_background (TtGraphicGC, White_Color);
   gdk_gc_set_exposures (TtGraphicGC,0);

   /* initialize Graphic context to create pixmap */
   /* GCimage = gdk_gc_new (DefaultDrawable);
   gdk_rgb_gc_set_foreground (GCimage, Black_Color);
   gdk_rgb_gc_set_background (GCimage, White_Color);
   gdk_gc_set_exposures (GCimage,0);*/

   /* initialize Graphic context to display tiled pictures */
   /* tiledGC = gdk_gc_new (DefaultDrawable);
   gdk_rgb_gc_set_foreground (tiledGC, Black_Color);
   gdk_rgb_gc_set_background (tiledGC, White_Color);
   gdk_gc_set_exposures (tiledGC,0);*/

   /* special Graphic context to display bitmaps */
   /* GCpicture = gdk_gc_new (DefaultDrawable);
   gdk_rgb_gc_set_foreground (GCpicture, Black_Color);
   gdk_rgb_gc_set_background (GCpicture, White_Color);
   gdk_gc_set_exposures (GCpicture,0);*/
   /* create a special logo for lost pictures */
   /* TODO */
#else /* _GTK */
   /* initialize Graphic context to display pictures */
   TtGraphicGC = XCreateGC (TtDisplay, TtRootWindow, 0, NULL);
   XSetForeground (TtDisplay, TtGraphicGC, Black_Color);
   XSetBackground (TtDisplay, TtGraphicGC, White_Color);
   XSetGraphicsExposures (TtDisplay, TtGraphicGC, FALSE);
   /* initialize Graphic context to create pixmap */
   GCimage = XCreateGC (TtDisplay, TtRootWindow, 0, NULL);
   XSetForeground (TtDisplay, GCimage, Black_Color);
   XSetBackground (TtDisplay, GCimage, White_Color);
   XSetGraphicsExposures (TtDisplay, GCimage, FALSE);

   /* initialize Graphic context to display tiled pictures */
   tiledGC = XCreateGC (TtDisplay, TtRootWindow, 0, NULL);
   XSetForeground (TtDisplay, tiledGC, Black_Color);
   XSetBackground (TtDisplay, tiledGC, White_Color);
   XSetGraphicsExposures (TtDisplay, tiledGC, FALSE);

   /* special Graphic context to display bitmaps */
   GCpicture = XCreateGC (TtDisplay, TtRootWindow, 0, NULL);
   XSetForeground (TtDisplay, GCpicture, Black_Color);
   XSetBackground (TtDisplay, GCpicture, White_Color);
   XSetGraphicsExposures (TtDisplay, GCpicture, FALSE);
   /* create a special logo for lost pictures */
   PictureLogo = TtaCreatePixmapLogo (lost_xpm);
   EpsfPictureLogo = XCreatePixmapFromBitmapData (TtDisplay, TtRootWindow,
						  epsflogo_bits,
						  epsflogo_width,
						  epsflogo_height,
						  Black_Color,
						  White_Color,
						  TtWDepth);
   theVisual = DefaultVisual (TtDisplay, TtScreen);
#endif /* _GTK */
#endif /* !_WINDOWS */

   Printing = printing;
   /* by default no plugins loaded */
   HandlersCounter = 0;
   currentExtraHandler = 0;
   strncpy (PictureHandlerTable[HandlersCounter].GUI_Name, XbmName, MAX_FORMAT_NAMELENGHT);
   PictureHandlerTable[HandlersCounter].Produce_Picture = XbmCreate;
   PictureHandlerTable[HandlersCounter].Produce_Postscript = XbmPrint;
   PictureHandlerTable[HandlersCounter].Match_Format = IsXbmFormat;

   PictureIdType[HandlersCounter] = XBM_FORMAT;
   PictureMenuType[HandlersCounter] = XBM_FORMAT;
   HandlersCounter++;

   strncpy (PictureHandlerTable[HandlersCounter].GUI_Name, EpsName, MAX_FORMAT_NAMELENGHT);
   PictureHandlerTable[HandlersCounter].Produce_Picture = EpsCreate;
   PictureHandlerTable[HandlersCounter].Produce_Postscript = EpsPrint;
   PictureHandlerTable[HandlersCounter].Match_Format = IsEpsFormat;

   PictureIdType[HandlersCounter] = EPS_FORMAT;
   PictureMenuType[HandlersCounter] = EPS_FORMAT;
   HandlersCounter++;

   strncpy (PictureHandlerTable[HandlersCounter].GUI_Name, XpmName, MAX_FORMAT_NAMELENGHT);
   PictureHandlerTable[HandlersCounter].Produce_Picture = XpmCreate;
   PictureHandlerTable[HandlersCounter].Produce_Postscript = XpmPrint;
   PictureHandlerTable[HandlersCounter].Match_Format = IsXpmFormat;

   PictureIdType[HandlersCounter] = XPM_FORMAT;
   PictureMenuType[HandlersCounter] = XPM_FORMAT;
   HandlersCounter++;

   strncpy (PictureHandlerTable[HandlersCounter].GUI_Name, GifName, MAX_FORMAT_NAMELENGHT);
   PictureHandlerTable[HandlersCounter].Produce_Picture = GifCreate;
   PictureHandlerTable[HandlersCounter].Produce_Postscript = GifPrint;
   PictureHandlerTable[HandlersCounter].Match_Format = IsGifFormat;

   PictureIdType[HandlersCounter] = GIF_FORMAT;
   PictureMenuType[HandlersCounter] = GIF_FORMAT;
   HandlersCounter++;

   strncpy (PictureHandlerTable[HandlersCounter].GUI_Name, PngName, MAX_FORMAT_NAMELENGHT);
   PictureHandlerTable[HandlersCounter].Produce_Picture = PngCreate;
   PictureHandlerTable[HandlersCounter].Produce_Postscript = PngPrint;
   PictureHandlerTable[HandlersCounter].Match_Format = IsPngFormat;

   PictureIdType[HandlersCounter] = PNG_FORMAT;
   PictureMenuType[HandlersCounter] = PNG_FORMAT;
   HandlersCounter++;

   strncpy (PictureHandlerTable[HandlersCounter].GUI_Name, JpegName, MAX_FORMAT_NAMELENGHT);
   PictureHandlerTable[HandlersCounter].Produce_Picture = JpegCreate;
   PictureHandlerTable[HandlersCounter].Produce_Postscript = JpegPrint;
   PictureHandlerTable[HandlersCounter].Match_Format = IsJpegFormat;

   PictureIdType[HandlersCounter] = JPEG_FORMAT;
   PictureMenuType[HandlersCounter] = JPEG_FORMAT;
   HandlersCounter++;
   InlineHandlers = HandlersCounter;
}


/*----------------------------------------------------------------------
   GetPictHandlersList creates in buffer the list of defined handlers 
   This function is used to create the GUI Menu            
   We return in count the number of handlers               
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                GetPictHandlersList (int *count, char* buffer)
#else  /* __STDC__ */
void                GetPictHandlersList (count, buffer)
int                *count;
char*              buffer;

#endif /* __STDC__ */
{
   int                 i = 0;
   int                 index = 0;
   char*               item;

   *count = HandlersCounter;
   while (i < HandlersCounter)
     {
	item = PictureHandlerTable[i].GUI_Name;
	strcpy (buffer + index, item);
	index += strlen (item) + 1;
	i++;
     }
   buffer = PictureMenu;

}

/*----------------------------------------------------------------------
   SimpleName

   Si filename est un nom de fichier absolu, retourne dans simplename le nom
   simple du fichier.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         SimpleName (STRING filename, STRING simplename)
#else  /* __STDC__ */
static void         SimpleName (filename, simplename)
STRING              filename;
STRING              simplename;

#endif /* __STDC__ */
{
   register STRING     from, to;
   CHAR_T                URL_DIR_SEP;

   if (filename && ustrchr (filename, TEXT('/')))
	  URL_DIR_SEP = TEXT('/');
   else 
	   URL_DIR_SEP = DIR_SEP;
 
   to = simplename;
   *to = EOS;
   for (from = filename; *from++;) ;
   for (--from; --from > filename;)
     {
        if (*from == URL_DIR_SEP)
          {
             ++from;
             break;
          }
     }
   if (*from == URL_DIR_SEP)
      ++from;
 
   for (; *from;)
      *to++ = *from++;
   *to = EOS;
}


/*----------------------------------------------------------------------
   DrawEpsBox draws the eps logo into the picture box.            
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         DrawEpsBox (PtrBox box, PictInfo *imageDesc, int frame, int wlogo, int hlogo)
#else  /* __STDC__ */
static void         DrawEpsBox (box, imageDesc, frame, wlogo, hlogo)
PtrBox              box;
PictInfo           *imageDesc;
int                 frame;
int                 wlogo;
int                 hlogo;
#endif /* __STDC__ */
{
#ifndef _GTK
   Pixmap              pixmap;
   float               scaleX, scaleY;
   int                 x, y, w, h, xFrame, yFrame, wFrame, hFrame;
   int                 XOrg, YOrg, picXOrg, picYOrg;
#ifdef _WINDOWS
   ThotWindow          drawable;
   HDC                 hDc, hMemDc;
   POINT               lPt[2];
   HBITMAP             hOldBitmap;
#else  /* _WINDOWS */
   CHAR_T              filename[255];
   Drawable            drawable;
   int                 fileNameWidth;
   int                 fnposx, fnposy;
#endif /* !_WINDOWS */

   /* Create the temporary picture */
   scaleX = 0.0;
   scaleY = 0.0;
   x = 0;
   y = 0;
   w = 0;
   h = 0;

   drawable = TtaGetThotWindow (frame);
   switch (imageDesc->PicPresent)
     {
     case RealSize:
     case FillFrame:
     case XRepeat:
     case YRepeat:
       w = imageDesc->PicWArea;
       h = imageDesc->PicHArea;
       break;
     case ReScale:
       /* what is the smallest scale */
       scaleX = (float) box->BxW / (float) imageDesc->PicWArea;
       scaleY = (float) box->BxH / (float) imageDesc->PicHArea;
       if (scaleX <= scaleY)
	 {
	   w = box->BxW;
	   h = (int) ((float) imageDesc->PicHArea * scaleX);
	 }
       else
	 {
	   h = box->BxH;
	   w = (int) ((float) imageDesc->PicWArea * scaleY);
	 }
       break;
     }
   
#ifndef _WINDOWS
   pixmap = XCreatePixmap (TtDisplay, TtRootWindow, w, h, TtWDepth);
   XFillRectangle (TtDisplay, pixmap, TtBlackGC, x, y, w, h);
   
   /* putting the cross edges */
   XDrawRectangle (TtDisplay, pixmap, TtDialogueGC, x, y, w - 1, h - 1);
   XDrawLine (TtDisplay, pixmap, TtDialogueGC, x, y, x + w - 1, y + h - 2);
   XDrawLine (TtDisplay, pixmap, TtDialogueGC, x + w - 1, y, x, y + h - 2);
   XDrawLine (TtDisplay, pixmap, TtWhiteGC, x, y + 1, x + w - 1, y + h - 1);
   XDrawLine (TtDisplay, pixmap, TtWhiteGC, x + w - 1, y + 1, x, y + h - 1);
#else  /* _WINDOWS */
   pixmap = CreateBitmap (w, h, TtWDepth, 1, NULL);
   hDc    = GetDC (drawable);
   hMemDc = CreateCompatibleDC (hDc);
   hOldBitmap = SelectObject (hMemDc, pixmap);
   Rectangle (hMemDc, 0, 0, w - 1, h - 1);
   lPt[0].x = 0;
   lPt[0].y = 0;
   lPt[1].x = w - 1;
   lPt[1].y = h - 2;
   Polyline  (hMemDc, lPt, 2);

   lPt[0].x = w - 1;
   lPt[0].y = 0;
   lPt[1].x = 0;
   lPt[1].y = h - 2;
   Polyline  (hMemDc, lPt, 2);

   lPt[0].x = 0;
   lPt[0].y = 1;
   lPt[1].x = w - 1;
   lPt[1].y = h - 2;
   Polyline  (hMemDc, lPt, 2);

   lPt[0].x = 0;
   lPt[0].y = w - 1;
   lPt[1].x = 1;
   lPt[1].y = h - 1;
   Polyline  (hMemDc, lPt, 2);
   SelectObject (hMemDc, hOldBitmap);
   DeleteDC (hDc);
   DeleteDC (hMemDc);
#endif /* _WINDOWS */

   /* copying the logo */
   /* 2 pixels used by the enclosing rectangle */
   if (wlogo > w - 2)
     {
       wFrame = w - 2;
       xFrame = x + 1;
       picXOrg = wlogo - w + 2;
     }
   else
     {
       wFrame = wlogo;
       xFrame = x + w - 1 - wlogo;
       picXOrg = 0;
     }
   /* 2 pixels used by the enclosing rectangle */
   if (hlogo > h - 2)
     {
       hFrame = h - 2;
       yFrame = y + 1;
       picYOrg = hlogo - h + 2;
     }
   else
     {
       hFrame = hlogo;
       yFrame = y + 1;
       picYOrg = 0;
     }
   /* Drawing In the Picture Box */
#ifndef _WINDOWS
   XCopyArea (TtDisplay, imageDesc->PicPixmap, pixmap, TtDialogueGC, picXOrg, picYOrg, wFrame, hFrame, xFrame, yFrame);
#endif /* _WINDOWS */
   GetXYOrg (frame, &XOrg, &YOrg);
   xFrame = box->BxXOrg + box->BxLMargin + box->BxLBorder + box->BxLPadding - XOrg;
   yFrame = box->BxYOrg + box->BxTMargin + box->BxTBorder + box->BxTPadding + FrameTable[frame].FrTopMargin - YOrg;
   wFrame = box->BxW;
   hFrame = box->BxH;
   Picture_Center (w, h, wFrame, hFrame, RealSize, &x, &y, &picXOrg, &picYOrg);
   if (w > wFrame)
      w = wFrame;
   if (h > hFrame)
      h = hFrame;
   x += xFrame;
   y += yFrame;

   LayoutPicture (pixmap, drawable, picXOrg, picYOrg, w, h, x, y, frame, imageDesc);

#ifdef _WINDOWS
   if (pixmap && !DeleteObject (pixmap))
      WinErrorBox (WIN_Main_Wd, TEXT("DrawEpsBox (1)"));
   pixmap = (HBITMAP) 0;
#else /* _WINDOWS */ 
   XFreePixmap (TtDisplay, pixmap);
   pixmap = None;
   XSetLineAttributes (TtDisplay, TtLineGC, 1, LineSolid, CapButt, JoinMiter);
   XDrawRectangle (TtDisplay, drawable, TtLineGC, xFrame, yFrame, wFrame - 1, hFrame - 1);

   /* Display the filename in the bottom of the Picture Box */
   SimpleName (imageDesc->PicFileName, filename);
   fileNameWidth = XTextWidth ((XFontStruct *) FontDialogue, filename, ustrlen (filename));
   if ((fileNameWidth + wlogo <= wFrame) && (FontHeight (FontDialogue) + hlogo <= hFrame))
     {
       fnposx = (wFrame - fileNameWidth) / 2 + xFrame;
       fnposy = hFrame - 5 + yFrame;
       XSetFont (TtDisplay, TtLineGC, ((XFontStruct *) FontDialogue)->fid);
       XDrawString (TtDisplay, drawable, TtLineGC, fnposx, fnposy, filename, ustrlen (filename));
     }
#endif /* _WINDOWS */
#endif /* _GTK */
}


/*----------------------------------------------------------------------
  DrawPicture draws the picture in the frame window.
  Parameters x, y, w, h give the displayed area of the box.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                DrawPicture (PtrBox box, PictInfo *imageDesc, int frame, int x, int y, int w, int h)
#else  /* __STDC__ */
void                DrawPicture (box, imageDesc, frame)
PtrBox              box;
PictInfo           *imageDesc;
int                 frame;
#endif /* __STDC__ */
{
  PathBuffer          fileName;
  PictureScaling      pres;
  int                 typeImage;
  int                 picXArea, picYArea, picWArea, picHArea;
  int                 xTranslate, yTranslate, picXOrg, picYOrg;
  int                 xFrame, yFrame;
  ThotColor           BackGroundPixel;
#ifdef _WINDOWS
  ThotWindow          drawable;
#ifdef _WIN_PRINT
  LPBITMAPINFO        lpBmpInfo;
#endif /* _WIN_PRINT */
#else  /* !_WINDOWS */
  Drawable            drawable;
#endif /* !_WINDOWS */
  
  if (w == 0 && h == 0)
    /* the picture is not visible */
    return;

  xTranslate = 0;
  yTranslate = 0;
  picXOrg = 0;
  picYOrg = 0;

  if (imageDesc->PicFileName == NULL || imageDesc->PicFileName[0] == EOS || 
      (box->BxAbstractBox->AbLeafType == LtCompound && imageDesc->PicPixmap == PictureLogo))
    return;

#ifdef _WINDOWS
  WIN_GetDeviceContext (frame);
#endif /* _WINDOWS */

  drawable = TtaGetThotWindow (frame);
  GetXYOrg (frame, &xFrame, &yFrame);
  typeImage = imageDesc->PicType;
  GetPictureFileName (imageDesc->PicFileName, fileName);

  pres = imageDesc->PicPresent;
  /* resize plugins if necessary */
  if (typeImage >= InlineHandlers)
    {
      imageDesc->PicWArea = w;
      imageDesc->PicHArea = h;
    }
  picXArea = imageDesc->PicXArea;
  picYArea = imageDesc->PicYArea;
  picWArea = imageDesc->PicWArea;
  picHArea = imageDesc->PicHArea;
  BackGroundPixel = box->BxAbstractBox->AbBackground;
  
  SetPictureClipping (&picWArea, &picHArea, w, h, imageDesc);
  if (!Printing)
    {
      if (imageDesc->PicType == EPS_FORMAT) 
	DrawEpsBox (box, imageDesc, frame, epsflogo_width, epsflogo_height);
      else
	{
	  if (!IsValid (w, h, imageDesc))
	    {
	      LoadPicture (frame, box, imageDesc);
	      picWArea = imageDesc->PicWArea;
	      picHArea = imageDesc->PicHArea;
	      SetPictureClipping (&picWArea, &picHArea, w, h, imageDesc);
	    }
	  if (imageDesc->PicPresent == RealSize && box->BxAbstractBox->AbLeafType == LtPicture)
	    /* Center real sized images wihin their picture boxes */
	    Picture_Center (picWArea, picHArea, w, h, pres, &xTranslate, &yTranslate, &picXOrg, &picYOrg);
	  
	  if (typeImage >= InlineHandlers)
	    {
	      if (PictureHandlerTable[typeImage].DrawPicture != NULL)
		(*(PictureHandlerTable[typeImage].DrawPicture)) (box, imageDesc, x + xTranslate, y + yTranslate);
	    }
	  else
	    LayoutPicture (imageDesc->PicPixmap, drawable, picXOrg, picYOrg,
			   w, h, x + xTranslate, y + yTranslate, frame, imageDesc);
	}
    }
  else if (typeImage < InlineHandlers && typeImage > -1)
    /* for the moment we didn't consider plugin printing */
#ifdef _WINDOWS
#ifdef _WIN_PRINT
    if (TtPrinterDC)
      {
        LPBYTE lpBits;

        LoadPicture (frame, box, imageDesc);
	if (imageDesc->PicPixmap == None) 
	  WinErrorBox (NULL, TEXT("DrawPicture (1)"));
	else
	  {
	    lpBmpInfo = CreateBitmapInfoStruct(FrRef[frame], imageDesc->PicPixmap);

	    lpBits = (LPBYTE) GlobalAlloc (GMEM_FIXED, lpBmpInfo->bmiHeader.biSizeImage);
	    if (!lpBits) 
	      WinErrorBox (NULL, TEXT("DrawPicture (2)"));

	    if (!GetDIBits (TtDisplay, (HBITMAP) (imageDesc->PicPixmap), 0, (UINT)lpBmpInfo->bmiHeader.biHeight, lpBits, lpBmpInfo, DIB_RGB_COLORS)) 
	      WinErrorBox (NULL, TEXT("DrawPicture (3)"));
			    
	    PrintDIB (lpBmpInfo, lpBits, FrRef[frame], TtPrinterDC, x, y, w, h);
               if (GlobalFree (lpBits) != NULL)
                  WinErrorBox (NULL, TEXT("DrawPicture (4)"));
	  }
      }
    else
      {
	(*(PictureHandlerTable[typeImage].Produce_Postscript)) (fileName, pres, x, y, w, h, picXArea,
                   picYArea, picWArea, picHArea,
                   (FILE *) drawable, BackGroundPixel);
      }
#endif /* _WIN_PRINT */
   WIN_ReleaseDeviceContext ();
#else  /* _WINDOWS */
  (*(PictureHandlerTable[typeImage].Produce_Postscript)) (fileName,
                   pres, x, y, w, h, picXArea,
		   picYArea, picWArea, picHArea,
		   (FILE *) drawable, BackGroundPixel);
#endif /* _WINDOWS */
}

/*----------------------------------------------------------------------
  UnmapImage unmaps plug-in widgets   
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void           UnmapImage (PictInfo* imageDesc)
#else /* __STDC__ */
void           UnmapImage (imageDesc)
PictInfo      *imageDesc;
#endif /* __STDC__ */
{
    int typeImage;

    if (imageDesc == NULL)
      return;

    typeImage = imageDesc->PicType;

#ifndef _WINDOWS
    if ((typeImage >= InlineHandlers) && (imageDesc->mapped) && (imageDesc->created))
      {	
	XtUnmapWidget ((Widget) (imageDesc->wid));
 	imageDesc->mapped = FALSE;
      }
#endif /* !_WINDOWS */
}


/*----------------------------------------------------------------------
   Routine handling the zoom-in zoom-out of an image   
  ----------------------------------------------------------------------*/
#ifdef __STDC__
unsigned char* ZoomPicture (unsigned char *cpic, int cWIDE, int cHIGH , int eWIDE, int eHIGH, int bperpix)
#else  /* __STDC__ */
unsigned char* ZoomPicture (cpic, cWIDE, cHIGH , eWIDE, eHIGH, bperpix)
unsigned char* cpic;
int            cWIDE;
int            cHIGH;
int            eWIDE;
int            eHIGH;
int            bperpix;
#endif /* __STDC__ */
{
  int           cy, ex, ey,*cxarr, *cxarrp;
  unsigned char *clptr,*elptr,*epptr, *epic;

  clptr = NULL;
  cxarrp = NULL;
  cy = 0; 
  /* check for size */
  if ((cWIDE < 0) || (cHIGH < 0) || (eWIDE < 0) || (eHIGH < 0) ||
      (cWIDE > 2000) || (cHIGH > 2000) || (eWIDE > 2000) || (eHIGH > 2000))
    return (NULL);

  /* generate a 'raw' epic, as we'll need it for ColorDither if EM_DITH */
  if (eWIDE == cWIDE && eHIGH == cHIGH)
    /* 1:1 expansion.  points destinqtion pic at source pic */
    epic = cpic;
  else
    {
      /* run the rescaling algorithm */
      /* create a new pic of the appropriate size */
      epic = (char *) TtaGetMemory((size_t) (eWIDE * eHIGH * bperpix));
      if (!epic)
	printf(" unable to TtaGetMemory memory for zoomed image \n");
      cxarr = (int *) TtaGetMemory(eWIDE * sizeof(int));
      if (!cxarr)
	printf("unable to allocate cxarr for zoomed image \n");
      for (ex=0; ex<eWIDE; ex++) 
	cxarr[ex] = bperpix * ((cWIDE * ex) / eWIDE);
      
      elptr = epptr = epic;
      for (ey=0;  ey<eHIGH;  ey++, elptr+=(eWIDE*bperpix))
	{
	  cy = (cHIGH * ey) / eHIGH;
	  epptr = elptr;
	  clptr = cpic + (cy * cWIDE * bperpix);
	  
	  if (bperpix == 1)
	    {
	      for (ex=0, cxarrp = cxarr;  ex<eWIDE;  ex++, epptr++) 
	      *epptr = clptr[*cxarrp++];
	    }
	  else
	    {
	      int j;  char *cp;
	      for (ex=0, cxarrp = cxarr; ex<eWIDE; ex++,cxarrp++)
		{
		  cp = clptr + *cxarrp;
		  for (j=0; j<bperpix; j++) 
		    *epptr++ = *cp++;
		}
	    }
	}
      TtaFreeMemory (cxarr);
    }
  
  return (epic);
}


/*----------------------------------------------------------------------
   Requests the picture handlers to get the corresponding pixmaps    
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                LoadPicture (int frame, PtrBox box, PictInfo *imageDesc)
#else  /* __STDC__ */
void                LoadPicture (frame, box, imageDesc)
int                 frame;
PtrBox              box;
PictInfo           *imageDesc;
#endif /* __STDC__ */
{
#ifndef _GTK
  PathBuffer          fileName;
  PictureScaling      pres;
  Drawable            picMask = None;
  Drawable            myDrawable = None;
  Picture_Report      status;
  unsigned long       Bgcolor;
  int                 typeImage;
  int                 xFrame = 0;
  int                 yFrame = 0;
  int                 wFrame, hFrame, w, h;
  int                 width, height;
#ifdef _WINDOWS
#ifndef _WIN_PRINT
  ThotBool            DeviceToRelease = FALSE;
#endif /* _WIN_PRINT */
#endif /* _WINDOWS */

  if (box->BxAbstractBox->AbVisibility < ViewFrameTable[frame - 1].FrVisibility)
    /* the picture is not visible */
    return;
  if (imageDesc->PicFileName == NULL || imageDesc->PicFileName[0] == EOS)
    return;

  GetPictureFileName (imageDesc->PicFileName, fileName);
  typeImage = imageDesc->PicType;
  status = PictureFileOk (fileName, &typeImage);
  w = 0;
  h = 0;
  Bgcolor = ColorPixel (box->BxAbstractBox->AbBackground);
  
#ifdef _WINDOWS
#ifndef _WIN_PRINT
  if (TtDisplay == 0)
    {
      WIN_GetDeviceContext (frame);
      DeviceToRelease = TRUE;
    }
#endif /* _WIN_PRINT */
#endif /* _WINDOWS */

  if (status != Supported_Format)
    {
#ifdef _WINDOWS
      if (TtPrinterDC == NULL)
	{
	  imageDesc->PicType = 3;
	  pres = RealSize;
	  imageDesc->PicPresent = pres;
	  myDrawable = (*(PictureHandlerTable[GIF_FORMAT].Produce_Picture)) 
	    (LostPicturePath, imageDesc, &xFrame, &yFrame, &wFrame, &hFrame, Bgcolor, &picMask, &width, &height);
	}
#else  /* !_WINDOWS */
      myDrawable = PictureLogo;
#endif /* _WINDOWS */

      imageDesc->PicType = -1;
      wFrame = w = 40;
      hFrame = h = 40;
      picMask = None;
    }
  else
    {
      /* Supported format */
      imageDesc->PicType = typeImage;
      pres = imageDesc->PicPresent;
      if ((typeImage == XBM_FORMAT || typeImage == XPM_FORMAT) &&
	  pres == ReScale)
	pres = imageDesc->PicPresent = RealSize;
      /* picture dimension */
      if (box == NULL)
	{
	  w = 20;
	  h = 20;
	}
      else if (box->BxAbstractBox->AbLeafType == LtCompound)
	{
	  /* a background image, draw over the whole box */
	  w = box->BxWidth;
	  h = box->BxHeight;
	}
      else
	{
	  /* draw within the inside box */
	  w = box->BxW;
	  h = box->BxH;
	}
      
      if (!Printing)
	{
#ifndef _WINDOWS
	  if (box != NULL)
	    /* set the colors of the  graphic context GC */
	    if (TtWDepth == 1)
	      {
		/* Black and white screen */
		XSetForeground (TtDisplay, TtGraphicGC, Black_Color);
		XSetBackground (TtDisplay, TtGraphicGC, ColorPixel (BackgroundColor[frame]));
	      }
	    else if (box->BxAbstractBox->AbSensitive && !box->BxAbstractBox->AbPresentationBox)
	      {
		/* Set active Box Color */
		XSetForeground (TtDisplay, TtGraphicGC, Box_Color);
		XSetForeground (TtDisplay, GCpicture, Box_Color);
		XSetBackground (TtDisplay, TtGraphicGC, ColorPixel (box->BxAbstractBox->AbBackground));
	      }
	    else
	      {
		/* Set Box Color */
		XSetForeground (TtDisplay, TtGraphicGC, ColorPixel (box->BxAbstractBox->AbForeground));
		XSetForeground (TtDisplay, GCpicture, ColorPixel (box->BxAbstractBox->AbForeground));
		XSetBackground (TtDisplay, TtGraphicGC, ColorPixel (box->BxAbstractBox->AbBackground));
	      }
#endif /* _WINDOWS */
	}
      
      if (PictureHandlerTable[typeImage].Produce_Picture != NULL)
	{
	  if (typeImage >= InlineHandlers)
	    {
	      /* Plugins are displayed in RealSize */
	      imageDesc->PicPresent = RealSize;
	      imageDesc->PicWArea = wFrame = w;
	      imageDesc->PicHArea = hFrame = h;
	      myDrawable = (*(PictureHandlerTable[typeImage].Produce_Picture)) (frame, imageDesc, fileName);
	      xFrame = imageDesc->PicXArea;
	      yFrame = imageDesc->PicYArea;
	    }
	  else
	    {
	      /* xFrame and yFrame get the box size if picture is */
	      /* rescaled and receive the position of the picture */
	      if (pres != ReScale || Printing)
		{
		  xFrame = 0;
		  yFrame = 0;
		}
	      else
		{
		  if (box->BxW != 0)
		    xFrame = w;
		  if(box->BxH != 0)
		    yFrame = h;
		}

	      myDrawable = (*(PictureHandlerTable[typeImage].Produce_Picture))
		(fileName, imageDesc, &xFrame, &yFrame, &wFrame, &hFrame, Bgcolor, &picMask, &width, &height, ViewFrameTable[frame - 1].FrMagnification);
	      /* intrinsic width and height */
	      imageDesc->PicWidth  = width;
	      imageDesc->PicHeight = height;
#ifdef _WINDOWS 
	      imageDesc->bgRed   = bgRed;
	      imageDesc->bgGreen = bgGreen;
	      imageDesc->bgBlue  = bgBlue;
#endif /* _WINDOWS */
	    }
	}
       
      if (myDrawable == None)
	{
	  myDrawable = PictureLogo;
	  imageDesc->PicType = -1;
	  wFrame = w = 40;
	  hFrame = h = 40;
	  picMask = None;
	}
      else if (box != NULL && (w == 0 || h == 0))
	{
	  /* one of box size is unknown, keep the image size */
	  if (w == 0)
	    w = wFrame;
	  if (h == 0)
	    h = hFrame;
	  /* Do you have to extend the clipping */
	  if (box->BxAbstractBox->AbLeafType == LtCompound)
	    DefClip (frame, box->BxXOrg, box->BxYOrg, box->BxXOrg + w, box->BxYOrg + h);
	  else
	    DefClip (frame, box->BxXOrg, box->BxYOrg, box->BxXOrg + box->BxRMargin + box->BxRBorder + box->BxRPadding + w, box->BxYOrg + box->BxBMargin + box->BxBBorder + box->BxBPadding + h);
	  if (box->BxAbstractBox && !box->BxAbstractBox->AbPresentationBox)
	    NewDimPicture (box->BxAbstractBox);
	}
    }

  if (imageDesc->PicPresent != ReScale || Printing)
    {
      imageDesc->PicXArea = xFrame;
      imageDesc->PicYArea = yFrame;
      imageDesc->PicWArea = wFrame;
      imageDesc->PicHArea = hFrame;
    }
  else
    {
      imageDesc->PicXArea = xFrame;
      imageDesc->PicYArea = yFrame;
      imageDesc->PicWArea = w;
      imageDesc->PicHArea = h;
    }
  if (box->BxType != BoPicture)
    {
      /* we don't use mask for background picture till we don't change
	 clipping management */
      FreePixmap (picMask);
      picMask = None;
    }
  FreePixmap (imageDesc->PicPixmap);
  imageDesc->PicPixmap = myDrawable;
#ifdef _WINDOWS
#ifndef _WIN_PRINT
  if (DeviceToRelease)
    WIN_ReleaseDeviceContext ();
#endif /* _WIN_PRINT */
#else /* _WINDOWS */
  FreePixmap (imageDesc->PicMask);
  imageDesc->PicMask = picMask;
#endif /* _WINDOWS */
#endif /* _GTK */
}


/*----------------------------------------------------------------------
   GetPictureType returns the type of the image based on the index 
   in the GUI form.                                        
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int                 GetPictureType (int GUIIndex)
#else  /* __STDC__ */
int                 GetPictureType (GUIIndex)
int                 menuIndex;

#endif /* __STDC__ */
{
   if (GUIIndex == 0)
      return UNKNOWN_FORMAT;
   else
      /* based on the function GetPictureHandlersList */
      return PictureMenuType[GUIIndex];

}

/*----------------------------------------------------------------------
   GetPictTypeIndex returns the menu type index of the picture.    
   		If the type is unkown we return 0.                      
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int                 GetPictTypeIndex (int picType)
#else  /* __STDC__ */
int                 GetPictTypeIndex (picType)
int                 picType;
#endif /* __STDC__ */
{
   int                 i = 0;

   /* based on the function GetPictureHandlersList */
   if (picType == UNKNOWN_FORMAT)
      return 0;

   while (i <= HandlersCounter)
     {
	if (PictureMenuType[i] == picType)
	   return i;
	i++;
     }
   return 0;
}
/*----------------------------------------------------------------------
   GetPictPresIndex returns the index of of the presentation.      
   	If the presentation is unknown we return RealSize.      
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int                 GetPictPresIndex (PictureScaling picPresent)
#else  /* __STDC__ */
int                 GetPictPresIndex (picPresent)
PictureScaling      picPresent;

#endif /* __STDC__ */
{
  int               i;

  switch (picPresent)
    {
    case RealSize:
    case ReScale:
    case FillFrame:
    case XRepeat:
    case YRepeat:
      i = (int) picPresent;
      break;
    default:
      i = (int) RealSize;
      break;
    }
  /* based on the function GetPictureHandlersList */
  return i;
}


/*----------------------------------------------------------------------
   GetPictureHandlersList creates the list of installed handlers.  
   This function is used to create the menu picture.       
   It returns the number of handlers in count.             
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                GetPictureHandlersList (int *count, char* buffer)
#else  /* __STDC__ */
void                GetPictureHandlersList (count, buffer)
int                *count;
char*               buffer;

#endif /* __STDC__ */
{
   int                 i = 0;
   int                 index = 0;
   char*               item;

   *count = HandlersCounter;
   while (i < HandlersCounter)
     {
	item = PictureHandlerTable[i].GUI_Name;
	strcpy (buffer + index, item);
	index += strlen (item) + 1;
	i++;
     }
   buffer = PictureMenu;

}


/*----------------------------------------------------------------------
   LittleXBigEndian allows conversion between big and little endian  
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                LittleXBigEndian (register unsigned char *b, register long n)
#else  /* __STDC__ */
void                LittleXBigEndian (b, n)
register unsigned char *b;
register long       n;

#endif /* __STDC__ */
{
   do
     {
	*b = MirrorBytes[*b];
	b++;
     }
   while (--n > 0);
}
