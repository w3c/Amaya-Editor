/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2003
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * Picture Handling
 * Authors: I. Vatton (INRIA)
 *          N. Layaida (INRIA) - New picture formats
 *          R. Guetari (W3C/INRIA) - Windows version
 *          P. Cheyrou-lagreze (INRIA) - Opengl Version
 */

#define PNG_SETJMP_SUPPORTED
#include "thot_gui.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#ifndef _GTK
#include "lost.xpm"
#endif /*_GTK*/
#include "picture.h"
#include "frame.h"
#include "epsflogo.h"
#include "interface.h"
#include "fileaccess.h"
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
#include "boxmoves_f.h"
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
#ifdef _GL
static unsigned char *PictureLogo;
#else /*_GL*/
#if !defined (_GTK)
static Pixmap   PictureLogo;
#else /*_GTK*/
static GdkPixmap *PictureLogo;
#endif /*_GTK*/
#endif /*_GL*/
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
#ifdef _GL

#include "glwindowdisplay.h"

#ifdef _GTK
#include <gtkgl/gtkglarea.h>
#endif /*_GTK*/
#include <GL/gl.h>


#ifdef GL_MESA_window_pos
#define MESA
#endif
/* For now not software optimised but quality optimized...
 if too sloooooow we'll revert*/
#undef MESA

typedef struct _PicCache {
  int width;
  int height;
  float TexCoordW;
  float TexCoordH;
  char *filename;
  int texbind;
  int frame;  
  struct _PicCache *next;  
} Pic_Cache;

/*
  static linked list containing all
  pictures in video card memory
*/
static Pic_Cache *PicCache = NULL;

/*--------------------------------------------------
 Free_Pic_Chache : really free a unique structure Cache  
 ---------------------------------------------------*/
static void Free_Pic_Chache (Pic_Cache *Cache)
{
  if (glIsTexture (Cache->texbind))
    glDeleteTextures (1, 
		      &(Cache->texbind));
  
#ifdef _PCLDEBUG
  g_print ("\n Free Image %s from cache", 
	   Cache->filename);      
#endif /*_PCLDEBUG*/ 
  TtaFreeMemory (Cache->filename);
  TtaFreeMemory (Cache);
}

/*--------------------------------------------------
 Lookup for Free upon an unique index a unique Cache 
 ---------------------------------------------------*/
static void FreeAPicCache (int texbind, int frame)
{
  Pic_Cache *Before;
  Pic_Cache *Cache;

  Before = NULL;  
  Cache = PicCache;
  while (Cache)
    {
      if (Cache->texbind == texbind &&
	  Cache->frame == frame)
	break;
      Before = Cache;
      Cache = Cache->next;      
    }
  if (Cache)
    {
      if (!Before)
	PicCache = PicCache->next;
      else
	Before->next = Cache->next;
      Free_Pic_Chache (Cache);
    }
}

/*--------------------------------------------------
 Free index Cache freeing recursive function 
 ---------------------------------------------------*/
static void FreePicCache (Pic_Cache *Cache)
{
  if (Cache->next)
    FreePicCache (Cache->next);
  Free_Pic_Chache (Cache);
}
/*--------------------------------------------------
 AddInPicCache : Add a new Pic
 ---------------------------------------------------*/
static void AddInPicCache (PictInfo *Image, int frame)
{
  Pic_Cache *Cache = PicCache;

 if (PicCache)
   {
     while (Cache->next)
       Cache = Cache->next;
     Cache->next = TtaGetMemory (sizeof (Pic_Cache));
     Cache = Cache->next;     
   }
 else
   {
     PicCache = TtaGetMemory (sizeof (Pic_Cache));
     Cache = PicCache;      
   }   
  Cache->next = NULL; 
  Cache->frame = frame;
  Cache->texbind = Image->TextureBind;
  Cache->filename = TtaGetMemory (strlen(Image->PicFileName) + 1);
  Cache->height = Image->PicHeight;
  Cache->width = Image->PicWidth;
  Cache->TexCoordW = Image->TexCoordW;  
  Cache->TexCoordH = Image->TexCoordH;
  strcpy (Cache->filename, Image->PicFileName);
}

/*----------------------------------------------------------------------
 LookupInPicCache : Look in cache, if find an image with the same filename
 in the same frame (Tex Id are frame dependant)
  ----------------------------------------------------------------------*/
static int LookupInPicCache (PictInfo *Image, int frame)
{
  Pic_Cache *Cache = PicCache;
  char *filename = Image->PicFileName;
  
  while (Cache)
    {
      if (strcasecmp (Cache->filename, filename) == 0 &&
	  frame == Cache->frame)
	{
	  Image->PicWidth = (int) Cache->width;
	  Image->PicHeight = (int) Cache->height;
	  Image->TexCoordW = Cache->TexCoordW;
	  Image->TexCoordH = Cache->TexCoordH;
	  Image->TextureBind = Cache->texbind;  
#ifdef _PCLDEBUG
      g_print ("\n Lookup succeed Image %s ", Image->PicFileName);      
#endif /*_PCLDEBUG*/  
	  return 1;
	}      
      Cache = Cache->next; 
    }
  return 0;  
}
/*----------------------------------------------------------------------
 CacheLookupHeightAndWidth :
Get Height and Width based on cache info on this 
 Image (frame and Texture id)
  ----------------------------------------------------------------------*/
static void CacheLookupHeightAndWidth (PictInfo *Image,
				       int *width,
				       int *height,
				       int frame)
{
  Pic_Cache *Cache = PicCache;
  int TextureBind = Image->TextureBind;
  
  while (Cache)
    {
      if (TextureBind == Cache->texbind &&
	  frame == Cache->frame)
	{
	  *width = (int) Cache->width;
	  *height = (int) Cache->height;
	  Image->TexCoordW = Cache->TexCoordW;
	  Image->TexCoordH = Cache->TexCoordH;
	  return;	  
	}     
      Cache = Cache->next; 
    }
  *width = 0;
  *height = 0;
  return;  
}

/*----------------------------------------------------------------------
 Free video card memory from this texture.
  ----------------------------------------------------------------------*/
void FreeGlTexture (void *ImageDesc)
{
  PictInfo *Image;
  
  Image = (PictInfo *)ImageDesc;
  
  if (Image->TextureBind)
    {      
      FreeAPicCache (Image->TextureBind,
		     ActiveFrame);      
      if (glIsTexture (Image->TextureBind))
	glDeleteTextures (1, 
			  &(Image->TextureBind));
#ifdef _PCLDEBUG
      g_print ("\n Image %s Freed", Image->PicFileName);      
#endif /*_PCLDEBUG*/
      Image->TextureBind = 0;
      Image->RGBA = False;
    }
}

/*----------------------------------------------------------------------
  p2 :  Lowest power of two bigger than the argument.
  ----------------------------------------------------------------------*/
/*All these work on UNSIGNED BITS !!! 
 if little-big endianess is involved,
 all those atre wrong !!!*/

#define lowest_bit(x) (x & -x)
#define is_pow2(x) (x != 0 && x == lowest_bit(x))

static int ceil_pow2_minus_1(unsigned int x)
{
  unsigned int i;
  
  for (i=1; i; i <<= 1)
    x |= x >> i;
  return x;
}
#define p2(p) (is_pow2(p)?p:ceil_pow2_minus_1((unsigned int) p) + 1)
  
/* Don't know exactly wich is faster...
 this on is pretty good too...*/

/* 
int p2(p){
p -= 1; 
p |= p >> 16; 
p |= p >> 8; 
p |= p >> 4; 
p |= p >> 2; 
p |= p >> 1; 
return p + 1;

otherwise, identical, formulical,
but VERY VERY VERY slower 
(int to float, log, ceil, and finally float to int...)
return 1 << (int) ceilf(logf((float) p) / M_LN2);
} 
*/

/*----------------------------------------------------------------------
  GL_MakeTextureSize : Texture sizes must be power of two
  ----------------------------------------------------------------------*/
static void GL_MakeTextureSize(PictInfo *Image, 
			       int GL_w, int GL_h)
{
  unsigned char      *data, *ptr1, *ptr2;
  int                 xdiff, x, y, nbpixel;


  if (Image->PicPixmap != None)
    {
      nbpixel = GL_w * GL_h * ((Image->RGBA)?4:3);
      /* In this algo, just remember that a 
	 RGB pixel value is a list of 3 value in source data
	 and 4 for destination RGBA texture */
      data = TtaGetMemory (sizeof (unsigned char) * nbpixel);
      /* Black transparent filling */
      memset (data, 0, sizeof (unsigned char) * nbpixel);
      ptr1 = Image->PicPixmap;
      ptr2 = data;
      nbpixel = ((Image->RGBA)?4:3);
      xdiff = (GL_w - Image->PicWidth) * nbpixel;
      x = nbpixel * Image->PicWidth;
      for (y = 0; y < Image->PicHeight; y++)
	{
	  /* copy R,G,B,A */
	  memcpy (ptr2, ptr1, x); 
	  /* jump over the black transparent zone*/
	  ptr1 += x;
	  ptr2 += x + xdiff;
	}	
      if (Image->PicPixmap != PictureLogo)
	TtaFreeMemory (Image->PicPixmap);      
      Image->PicPixmap = data;
    }
}


/*----------------------------------------------------------------------
 GL_TextureBind : Put Texture in video card's Memory at
 a power of 2 size for height and width 
  ----------------------------------------------------------------------*/
static void GL_TextureBind (PictInfo *Image)
{  

  int       p2_w, p2_h;
  GLfloat   GL_w, GL_h;   
  GLint		Mode;
  
  /* Put texture in 3d card memory */
  if (!glIsTexture (Image->TextureBind) &&
      Image->PicWidth &&
      Image->PicHeight &&
      Image->PicPixmap)
    {      
      /* Another way is to split texture in 256x256 
	 pieces and render them on different quads
	 Declared to be the faster  */
      
      
      p2_w = p2 (Image->PicWidth);
      p2_h = p2 (Image->PicHeight);
      
      /* We have resized the picture to match a power of 2
	 We don't want to see all the picture, just the w and h 
	 portion*/
      
      GL_w = (GLfloat) Image->PicWidth/p2_w;
      GL_h = (GLfloat) Image->PicHeight/p2_h; 
      
      /* We give te texture to opengl Pipeline system */	    
      Mode = (Image->RGBA)?GL_RGBA:GL_RGB;
      GL_MakeTextureSize (Image, p2_w, p2_h);
      glGenTextures (1, 
		     &(Image->TextureBind));
      glBindTexture (GL_TEXTURE_2D, 
		     Image->TextureBind);       	
      glTexImage2D (GL_TEXTURE_2D, 
		    0, 
		    Mode, 
		    p2_w, 
		    p2_h, 
		    0,
		    Mode, 
		    GL_UNSIGNED_BYTE, 
		    (GLvoid *) Image->PicPixmap);
      TtaFreeMemory (Image->PicPixmap);

      Image->PicPixmap = None;
      Image->TexCoordW = GL_w;
      Image->TexCoordH = GL_h;
    }  
}
/*----------------------------------------------------------------------
 GL_TextureMap : map texture on a Quad (sort of a rectangle)
 Drawpixel Method for software implementation, as it's much faster for those
 Texture Method for hardware implementation as it's faster and better.
  ----------------------------------------------------------------------*/
void GL_TextureMap (void *ImagePt, 
		    int xFrame, int yFrame, 
		    int w, int h)
{  
  PictInfo *Image;
  
  Image = ImagePt;
  
  GL_SetPicForeground ();
  
   
  glBindTexture (GL_TEXTURE_2D, 
		 Image->TextureBind);
 	
  glEnable (GL_TEXTURE_2D);

  glTexParameteri (GL_TEXTURE_2D,
		       GL_TEXTURE_MIN_FILTER,
		   GL_NEAREST);
      glTexParameteri (GL_TEXTURE_2D,
		       GL_TEXTURE_MAG_FILTER,
		       GL_NEAREST);	    
      glTexParameteri (GL_TEXTURE_2D,
		       GL_TEXTURE_WRAP_S,
		       GL_CLAMP);
      glTexParameteri (GL_TEXTURE_2D,
		       GL_TEXTURE_WRAP_T,
		       GL_CLAMP); 
  /* does current Color modify texture no = GL_REPLACE, 
     else => GL_MODULATE, GL_DECAL, ou GL_BLEND */
  glTexEnvi( GL_TEXTURE_ENV, 
	     GL_TEXTURE_ENV_MODE, 
	     GL_MODULATE);

  /* Not sure of the vertex order 
     (not the faster one, I think) */
  glBegin (GL_QUADS);
  /* Texture coordinates are unrelative 
     to the size of the square */      
  /* lower left */
  glTexCoord2i (0,    0); 
  glVertex2i (xFrame,     yFrame + h);
  /* upper right*/
  glTexCoord2f (Image->TexCoordW, 0.0); 
  glVertex2i (xFrame + w, yFrame + h);
  /* lower right */
  glTexCoord2f (Image->TexCoordW, Image->TexCoordH); 
  glVertex2i (xFrame + w, yFrame); 
  /* upper left */
  glTexCoord2f (0.0,  Image->TexCoordH); 
  glVertex2i (xFrame,     yFrame);      
  glEnd ();	
  /* State disabling */
  glDisable (GL_TEXTURE_2D); 

}

void GL_StencilMap (PictInfo *Image, 
		    int xFrame, int yFrame, 
		    int w, int h)
{
  glEnable (GL_SCISSOR_TEST);
  glScissor (xFrame,
	     FrameTable[1].FrHeight
	     + FrameTable[1].FrTopMargin 
	     - (yFrame+h),
	     w,
	     h);

  glEnable (GL_STENCIL_TEST);
  glClear(GL_STENCIL_BUFFER_BIT);
  
  glStencilFunc (GL_ALWAYS, 1, 1);
  glStencilOp (GL_REPLACE, GL_REPLACE, GL_REPLACE);
  /* 
     The Geometric svg figure that determine the clip
     should be in a display list
  */
  glBegin (GL_TRIANGLE_STRIP);
  glVertex2i (xFrame,     yFrame + h);
  glVertex2i (xFrame + w/2+5,     yFrame + h/2 + 5);

  glVertex2i (xFrame + w, yFrame + h);
  glVertex2i (xFrame + w,     yFrame + h/2 + 5);

  glVertex2i (xFrame + w, yFrame); 
  glVertex2i (xFrame + w/2 +5, yFrame); 

  glVertex2i (xFrame,     yFrame); 
  
  glEnd ();
	
  glClear (GL_DEPTH_BUFFER_BIT);
  glStencilFunc (GL_EQUAL, 1, 1);
  glStencilOp (GL_KEEP, GL_KEEP, GL_KEEP);

  GL_TextureMap (Image, 
		 xFrame, yFrame, 
		 w, h);
 
  glStencilFunc (GL_NOTEQUAL, 1, 1);
  glStencilOp (GL_KEEP, GL_KEEP, GL_KEEP);

  glBegin (GL_TRIANGLE_STRIP);
  glVertex2i (xFrame,     yFrame + h);
  glVertex2i (xFrame + w/2+5,     yFrame + h/2 + 5);

  glVertex2i (xFrame + w, yFrame + h);
  glVertex2i (xFrame + w,     yFrame + h/2 + 5);

  glVertex2i (xFrame + w, yFrame); 
  glVertex2i (xFrame + w/2 +5, yFrame); 

  glVertex2i (xFrame,     yFrame); 
  
  glEnd ();
  glDisable (GL_STENCIL_TEST);
  glDisable (GL_SCISSOR_TEST);
}

#endif /* _GL */


#ifdef _WINDOWS
/*----------------------------------------------------------------------
  TransparentPicture
  displays the image without background (pixels with the color bg).
  The color bg is an index in the thot color table.
  ----------------------------------------------------------------------*/
static void TransparentPicture (HBITMAP pixmap, int xFrame, int yFrame,
				int w, int h, int bg)
{
   HDC            hMemDC, hOrDC;
   HDC             hAndDC, hInvAndDC;
   HDC            hDestDC;
   HBITMAP        bitmap, bitmapOr;
   HBITMAP        bitmapAnd, bitmapInvAnd;
   HBITMAP        bitmapDest, pBitmapOr;
   HBITMAP        pBitmapAnd, pBitmapInvAnd;
   HBITMAP        pBitmapDest;
   COLORREF       crColor;
   COLORREF       crOldBkColor;

   /* give the background color pixel */
   crColor = ColorPixel (bg);
   hMemDC = CreateCompatibleDC (TtDisplay);
   bitmap = SelectObject (hMemDC, pixmap);
   SetMapMode (hMemDC, GetMapMode (TtDisplay));
   
   hOrDC = CreateCompatibleDC (TtDisplay);
   SetMapMode (hOrDC, GetMapMode (TtDisplay));
   bitmapOr = CreateCompatibleBitmap (hMemDC, w, h);
   pBitmapOr = SelectObject (hOrDC, bitmapOr);
   /* hOrDC = */
   BitBlt (hOrDC, 0, 0, w, h, hMemDC, 0, 0, SRCCOPY);

   hAndDC = CreateCompatibleDC (TtDisplay);
   SetMapMode (hAndDC, GetMapMode (TtDisplay));
   bitmapAnd = CreateBitmap (w, h, 1, 1, NULL);
   pBitmapAnd = SelectObject (hAndDC, bitmapAnd);
   crOldBkColor = SetBkColor (hMemDC, crColor);
   /* hAndDC = */
   BitBlt (hAndDC, 0, 0, w, h, hMemDC, 0, 0, SRCCOPY);

   SetBkColor (hMemDC, crOldBkColor);
   hInvAndDC = CreateCompatibleDC (TtDisplay);
   SetMapMode (hInvAndDC, GetMapMode (TtDisplay));
   bitmapInvAnd = CreateBitmap (w, h, 1, 1, NULL);
   pBitmapInvAnd = SelectObject (hInvAndDC, bitmapInvAnd);
   /* hInvAndDC = */
   BitBlt (hInvAndDC, 0, 0, w, h, hAndDC, 0, 0, NOTSRCCOPY);
   /* hOrDC = */
   BitBlt (hOrDC, 0, 0, w, h, hInvAndDC, 0, 0, SRCAND);
   hDestDC = CreateCompatibleDC (TtDisplay);
   SetMapMode (hDestDC, GetMapMode (TtDisplay));
   bitmapDest = CreateCompatibleBitmap (hMemDC, w, h);
   pBitmapDest = SelectObject (hDestDC, bitmapDest);
   /* hDEstDC = */
   BitBlt (hDestDC, 0, 0, w, h, TtDisplay, xFrame, yFrame, SRCCOPY);
   BitBlt (hDestDC, 0, 0, w, h, hAndDC, 0, 0, SRCAND);
   BitBlt (hDestDC, 0, 0, w, h, hOrDC, 0, 0, SRCINVERT);
   BitBlt (TtDisplay, xFrame, yFrame, w, h, hDestDC, 0, 0, SRCCOPY);

   SelectObject (hDestDC, pBitmapDest);
   SelectObject (hInvAndDC, pBitmapInvAnd);
   SelectObject (hAndDC, pBitmapAnd);
   SelectObject (hOrDC, pBitmapOr);
   SelectObject (hMemDC, bitmap);
   if (hDestDC)
	   DeleteDC (hDestDC);
   if (hInvAndDC)
	   DeleteDC (hInvAndDC);
   if (hAndDC)
	   DeleteDC (hAndDC);
   if (hOrDC)
	   DeleteDC (hOrDC);
   if (hMemDC)
	   DeleteDC (hMemDC);

   if (bitmap)
	   DeleteObject (bitmap);
   if (bitmapOr)
	   DeleteObject (bitmapOr);
   if (pBitmapOr)
	   DeleteObject (pBitmapOr);
   if (bitmapAnd)
	   DeleteObject (bitmapAnd);
   if (pBitmapAnd)
	   DeleteObject (pBitmapAnd);
   if (bitmapInvAnd)
	   DeleteObject (bitmapInvAnd);
   if (pBitmapInvAnd)
	   DeleteObject (pBitmapInvAnd);
   if (bitmapDest)
	   DeleteObject (bitmapDest);
   if (pBitmapDest)
	   DeleteObject (pBitmapDest);
}
#endif /* _WINDOWS */

/*--------------------------------------------------
 Free All pics in video card memory and empty cache list
 in GL
 ---------------------------------------------------*/
void FreeAllPicCache ()
{
#ifdef _GL
  if (PicCache)
    FreePicCache (PicCache);  
#endif /* _GL */
}
/*--------------------------------------------------
  FreePicsCacheFromFrame : index Cache freeing  
 upon a frame destroy event
 ---------------------------------------------------*/
void FreeAllPicCacheFromFrame (int frame)
{
#ifdef _GL
  Pic_Cache *Cache = PicCache;
  Pic_Cache *Before;
  
 Before = NULL;  
 while (Cache)
   {
     if (Cache->frame == frame)
       {
	 if (GL_prepare (frame))
	   {
	     if (Before)
	       {
		 Before->next = Cache->next; 
		 Free_Pic_Chache (Cache);
		 Cache = Before->next;
	       }
	     else
	       {
		 /*we delete the first element 
		   of the linked list*/		 
		 PicCache = PicCache->next;
		 Free_Pic_Chache (Cache);
		 Cache = PicCache;
	       }
	   }
       }
     else
       {
	 Before = Cache; 
	 Cache = Cache->next;
       }     
   }
#endif /* _GL */
}
/*----------------------------------------------------------------------
  Match_Format returns TRUE if the considered header file matches   
  the image file description, FALSE in the the other cases        
  ----------------------------------------------------------------------*/
static ThotBool Match_Format (int typeImage, char *fileName)
{
   if (PictureHandlerTable[typeImage].Match_Format != NULL)
      return (*(PictureHandlerTable[typeImage].Match_Format)) (fileName);
   return FALSE;
}


/*----------------------------------------------------------------------
  FreePixmap frees the pixmap allocated in the X server if it is not
  empty and if it is not one of the internal images        
  ----------------------------------------------------------------------*/
void FreePixmap (Pixmap pixmap)
{
  if (pixmap != None 
      && pixmap != (Pixmap) PictureLogo 
      && pixmap != EpsfPictureLogo)
#ifdef _GL
    TtaFreeMemory ((void *)pixmap);
#else /*_GL*/
#ifndef _WINDOWS
#ifndef _GTK
    XFreePixmap (TtDisplay, pixmap);
#else /* _GTK */
    if (((GdkPixmap *) pixmap) != PictureLogo)
      gdk_imlib_free_pixmap ((GdkPixmap *) pixmap);
#endif /* _GTK */
#else  /* _WINDOWS */
  if (!DeleteObject ((HBITMAP)pixmap))
    WinErrorBox (WIN_Main_Wd, "FreePixmap");
#endif /* _WINDOWS */
#endif /*_GL*/
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
static void Picture_Center (int picWArea, int picHArea, int wFrame,
			    int hFrame, PictureScaling pres,
			    int *xTranslate, int *yTranslate,
			    int *picXOrg, int *picYOrg)
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
    case DefaultPres:
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
static void SetPictureClipping (int *picWArea, int *picHArea, int wFrame,
				int hFrame, PictInfo *imageDesc)
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

#ifdef _GL
/*----------------------------------------------------------------------
  LayoutPicture performs the layout of pixmap on the screen described
  by the drawable.
  if picXOrg or picYOrg are postive, the copy operation is shifted
  ----------------------------------------------------------------------*/
static void LayoutPicture (Pixmap pixmap, 
			   Drawable drawable, 
			   int picXOrg, int picYOrg, 
			   int w, int h, 
			   int xFrame, int yFrame, 
			   int frame, 
			   PictInfo *imageDesc, PtrBox box)
{
  ViewFrame*        pFrame;
  PictureScaling    picPresent;
  int               x, y, clipWidth, clipHeight;
  int               delta, i, j;
  
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
  pFrame = &ViewFrameTable[frame - 1];
  if (glIsTexture (imageDesc->TextureBind))
    {
      /* the default presentation depends on the box type */
      picPresent = imageDesc->PicPresent;
      if (picPresent == DefaultPres)
	{
	  if (box->BxType == BoPicture)
	    /* an image is rescaled */
	    picPresent = ReScale;
	  else
	    /* a background image is repeated */
	    picPresent = FillFrame;
	}
      switch (picPresent)
	{
	case ReScale:
	  GL_TextureMap (imageDesc, xFrame, yFrame, w ,h);
	  break;
	case RealSize:
	  GL_TextureMap (imageDesc, xFrame, yFrame,
			 imageDesc->PicWidth, 
			 imageDesc->PicHeight);
	  break;
	case FillFrame:
	case XRepeat:
	case YRepeat:
	  x          = pFrame->FrClipXBegin;
          y          = pFrame->FrClipYBegin;
          clipWidth  = pFrame->FrClipXEnd - x;
          clipHeight = pFrame->FrClipYEnd - y;
          x          -= pFrame->FrXOrg;
          y          -= pFrame->FrYOrg;

          if (picPresent == FillFrame || picPresent == YRepeat)
	    {
	      /* clipping height is done by the box height */
	      if (y < yFrame)
		{
		  /* reduce the height in delta value */
		  clipHeight = clipHeight + y - yFrame;
		  y = yFrame;
		}
	      if (clipHeight > h)
		clipHeight = h;
	    }
	  else
	    {
	      clipHeight = imageDesc->PicHeight;
	      
	      /* clipping height is done by the image height */
	     /*  delta = yFrame + imageDesc->PicHArea - y; */
/* 	      if (delta <= 0) */
/* 		clipHeight = 0; */
/* 	      else */
/* 		clipHeight = delta; */
	    }	  

          if (picPresent == FillFrame || picPresent == XRepeat)
	    {
	      /* clipping width is done by the box width */
	      if (x < xFrame)
		{
		  /* reduce the width in delta value */
		  clipWidth = clipWidth + x - xFrame;
		  x = xFrame;
		}
	      if (clipWidth > w)
		clipWidth = w;
	    }
	  else
	    {
	      clipWidth = imageDesc->PicWidth;

	      /* clipping width is done by the image width */
	      /* delta = xFrame + imageDesc->PicWArea - x; */
/* 	      if (delta <= 0) */
/* 		clipWidth = 0; */
/* 	      else */
/* 		clipWidth = delta; */
	    }

	  j = 0;
	  do
	    {
	      i = 0;
	      do
		{
		 
		  GL_TextureMap (imageDesc, 
				 xFrame + i, yFrame + j,
				 imageDesc->PicWidth, 
				 imageDesc->PicHeight);
		  i += imageDesc->PicWidth;
		} 
	      while (i < clipWidth);
	      j += imageDesc->PicHeight;
	    } 
	  while (j < clipHeight);

	  break;  
	default: 
	  break;
	}
    }
}

#else /*_GL*/

/*----------------------------------------------------------------------
  LayoutPicture performs the layout of pixmap on the screen described
  by the drawable.
  if picXOrg or picYOrg are postive, the copy operation is shifted
  ----------------------------------------------------------------------*/
static void LayoutPicture (Pixmap pixmap, Drawable drawable, int picXOrg,
			   int picYOrg, int w, int h, int xFrame,
			   int yFrame, int frame, PictInfo *imageDesc,
			   PtrBox box)
{
  ViewFrame*        pFrame;
  PictureScaling    picPresent;
#ifdef _WINDOWS
  int               delta;
  HDC               hMemDC;
  BITMAP            bm;
  HBITMAP           bitmap;
  HBITMAP           bitmapTiled;
  HBITMAP           pBitmapTiled;
  HDC               hOrigDC;
  POINT             ptOrg, ptSize;
  int               x, y, clipWidth, clipHeight;
  int               nbPalColors;
  int               i, j, iw, jh;
  HRGN              hrgn;
#else /* _WINDOWS */
  XRectangle        rect;
#ifndef _GTK
  XGCValues         values;
  unsigned int      valuemask;
#endif /* _GTK */
#endif /* _WINDOWS */

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

#ifdef _WINDOWS
  if (!TtIsTrueColor)
    {
      SelectPalette (TtDisplay, TtCmap, FALSE);
      nbPalColors = RealizePalette (TtDisplay);
    }
#endif /* _WINDOWS */
  pFrame = &ViewFrameTable[frame - 1];
  if (pixmap != None)
    {
      /* the default presentation depends on the box type */
      picPresent = imageDesc->PicPresent;
      if (picPresent == DefaultPres)
	{
	  if (box->BxType == BoPicture)
	    /* an image is rescaled */
	    picPresent = ReScale;
	  else
	    /* a background image is repeated */
	    picPresent = FillFrame;
	}
      switch (picPresent)
	{
	case ReScale:
#ifndef _WINDOWS
#ifndef _GTK
	  if (imageDesc->PicMask)
	    {
	      XSetClipOrigin (TtDisplay, TtGraphicGC, xFrame - picXOrg, yFrame - picYOrg);
	      XSetClipMask (TtDisplay, TtGraphicGC, imageDesc->PicMask);
	    }
	  XCopyArea (TtDisplay, pixmap, drawable, TtGraphicGC, 
		     picXOrg, picYOrg, w, h, xFrame, yFrame);
	  if (imageDesc->PicMask)
	    {
	      XSetClipMask (TtDisplay, TtGraphicGC, None);
	      XSetClipOrigin (TtDisplay, TtGraphicGC, 0, 0);
	    }
#else /* _GTK */ 
	   if (w != imageDesc->PicWArea ||
	       h != imageDesc->PicHArea || 
	       imageDesc->PicPixmap == NULL)
	     {
	       /*W and H is not the same 
		 as when we load the image*/
	       if (imageDesc->im && 
		   w > 0 && h > 0)
		 {
		   gdk_imlib_render(imageDesc->im, w, h);
		   if (imageDesc->PicMask)
		     {
		       gdk_gc_set_clip_origin (TtGraphicGC, 
					       xFrame - picXOrg, 
					       yFrame - picYOrg);
		       gdk_gc_set_clip_mask (TtGraphicGC, 
					     (GdkPixmap *) 
					     gdk_imlib_move_mask (imageDesc->im));
		     }		   
		   gdk_draw_pixmap ((GdkDrawable *)drawable, TtGraphicGC,
				    (GdkPixmap *) imageDesc->im->pixmap, 
				    picXOrg, picYOrg, 
				    xFrame, yFrame, w ,h);
		 }
	     }
	   else
	     {
	       if (imageDesc->PicMask)
		 {
		   gdk_gc_set_clip_origin (TtGraphicGC, 
					   xFrame - picXOrg, 
					   yFrame - picYOrg);
		   gdk_gc_set_clip_mask (TtGraphicGC, 
					 (GdkPixmap *) imageDesc->PicMask);
		 }
	       gdk_draw_pixmap ((GdkDrawable *)drawable, TtGraphicGC,
				(GdkPixmap *) imageDesc->PicPixmap, 
				picXOrg, picYOrg, 
				xFrame, yFrame, w ,h);
	     }
	   /*Restablish to normal clip*/
	   if (imageDesc->PicMask)
	     {
	       gdk_gc_set_clip_mask (TtGraphicGC, (GdkPixmap *)None);
	       gdk_gc_set_clip_origin (TtGraphicGC, 0, 0);
	     }
#endif /* !_GTK */
#else /* _WINDOWS */
	case RealSize:
	  if (imageDesc->PicMask == -1 || imageDesc->PicType == -1)
	    {
	      /* No transparence */
	      hMemDC = CreateCompatibleDC (TtDisplay);
	      bitmap = SelectObject (hMemDC, pixmap);
	      SetMapMode (hMemDC, GetMapMode (TtDisplay));

	      GetObject (pixmap, sizeof (BITMAP), (LPVOID) &bm);
	      /*DPtoLP (TtDisplay, &ptSize, 1);*/
		  /* shift in the source image */
	      ptOrg.x = pFrame->FrClipXBegin - box->BxXOrg;
	      ptOrg.y = pFrame->FrClipYBegin - box->BxYOrg;
		  /* size of the copied zone */
	      ptSize.x = pFrame->FrClipXEnd - pFrame->FrClipXBegin;
	      ptSize.y = pFrame->FrClipYEnd - pFrame->FrClipYBegin;
		  if (ptOrg.x < 0)
			ptOrg.x = 0;
		  if (ptOrg.y < 0)
			ptOrg.y = 0;
		  if (ptSize.x > bm.bmWidth - ptOrg.x)
	        ptSize.x = bm.bmWidth - ptOrg.x;
		  if (ptSize.y > bm.bmHeight - ptOrg.y)
	        ptSize.y = bm.bmHeight - ptOrg.y;
	      /*DPtoLP (hMemDC, &ptOrg, 1);*/
	    
	      BitBlt (TtDisplay, xFrame + ptOrg.x, yFrame + ptOrg.y,
		      ptSize.x, ptSize.y, hMemDC, ptOrg.x, ptOrg.y, SRCCOPY);
	      SelectObject (hMemDC, bitmap);
	      if (hMemDC )
		DeleteDC (hMemDC);
	    }
	  else
	    {
	      TransparentPicture (pixmap, xFrame, yFrame,
				  imageDesc->PicWArea, imageDesc->PicHArea,
				  imageDesc->PicMask);
	    }
#endif /* _WINDOWS */
	  break;
	  
	case FillFrame:
	case XRepeat:
	case YRepeat:
#ifndef _WINDOWS
	case RealSize:
          rect.x = pFrame->FrClipXBegin;
          rect.y = pFrame->FrClipYBegin;
          rect.width = pFrame->FrClipXEnd - rect.x;
          rect.height = pFrame->FrClipYEnd - rect.y;
          rect.x -= pFrame->FrXOrg;
          rect.y -= pFrame->FrYOrg;
	  /* clipping height is done by the box height */
	  if (rect.y < yFrame)
	    {
	      /* reduce the height in delta value */
	      rect.height = rect.height + rect.y - yFrame;
	      rect.y = yFrame;
	    }
	  if (rect.height > h)
	    rect.height = h;
	  /* clipping width is done by the box width */
	  if (rect.x < xFrame)
	    {
	      /* reduce the width in delta value */
	      rect.width = rect.width +rect.x - xFrame;
	      rect.x = xFrame;
	    }
	  if (rect.width > w)
	    rect.width = w;

#ifndef _GTK
	  valuemask = GCTile | GCFillStyle | GCTileStipXOrigin | GCTileStipYOrigin;
	  values.tile = pixmap;
	  values.ts_x_origin = xFrame;
	  values.ts_y_origin = yFrame;
	  values.fill_style = FillTiled;
	  XChangeGC (TtDisplay, tiledGC, valuemask, &values);
	  if (picPresent == RealSize)
	    {
	      if (imageDesc->PicMask)
		{
		  XSetClipOrigin (TtDisplay, tiledGC, xFrame - picXOrg,
				  yFrame - picYOrg);
		  XSetClipMask (TtDisplay, tiledGC, imageDesc->PicMask);
		}
	      else
		XSetClipRectangles (TtDisplay, tiledGC, 0, 0, &rect, 1, Unsorted);
	      if (w > imageDesc->PicWArea)
		w = imageDesc->PicWArea;
	      if (h > imageDesc->PicHArea)
		h = imageDesc->PicHArea;
	    }
	  else
	    {
	      XSetClipRectangles (TtDisplay, tiledGC, 0, 0, &rect, 1, Unsorted);
	      if (picPresent == YRepeat && w > imageDesc->PicWArea)
		w = imageDesc->PicWArea;
	      if (picPresent == XRepeat && h > imageDesc->PicHArea)
		h = imageDesc->PicHArea;
	      XFillRectangle (TtDisplay, drawable, tiledGC, xFrame, yFrame, w, h);
	    }
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
#else /* _GTK */
	  gdk_gc_set_fill (tiledGC, GDK_TILED);
	  gdk_gc_set_ts_origin (tiledGC, xFrame, yFrame);
	  gdk_gc_set_tile (tiledGC, (GdkPixmap *)pixmap);
	  if (picPresent == RealSize)
	    {
	      if (imageDesc->PicMask)
		{
		  gdk_gc_set_clip_origin (tiledGC, xFrame - picXOrg, yFrame - picYOrg);
		  gdk_gc_set_clip_mask (tiledGC, (GdkPixmap *)imageDesc->PicMask);
		}
	      else
		gdk_gc_set_clip_rectangle (tiledGC, (GdkRectangle *)&rect);
	      if (w > imageDesc->PicWArea)
		w = imageDesc->PicWArea;
	      if (h > imageDesc->PicHArea)
		h = imageDesc->PicHArea;
	    }
	  else
	    {
	      gdk_gc_set_clip_rectangle (tiledGC, (GdkRectangle *)&rect);
	      if (picPresent == YRepeat && w > imageDesc->PicWArea)
		w = imageDesc->PicWArea;
	      if (picPresent == XRepeat && h > imageDesc->PicHArea)
		h = imageDesc->PicHArea;
	    }
	  gdk_draw_rectangle ((GdkDrawable *)drawable, /* the window */ 
			      tiledGC,  /* the GC */
			      TRUE,     /* filled=true */
			      xFrame,   /* x position drawing */
			      yFrame,   /* y position drawing */
			      w,        /* width drawing */
			      h);       /* height drawing */

	  /* remove clipping */
          rect.x = 0;
          rect.y = 0;
          rect.width = MAX_SIZE;
          rect.height = MAX_SIZE;
	  gdk_gc_set_clip_rectangle (tiledGC, (GdkRectangle *)&rect);
	  if (imageDesc->PicMask)
	    {
	      gdk_gc_set_clip_mask (tiledGC, None);
	      gdk_gc_set_clip_origin (tiledGC, 0, 0);
	    }
#endif /* !_GTK */
#else  /* _WINDOWS */
          x          = pFrame->FrClipXBegin;
          y          = pFrame->FrClipYBegin;
          clipWidth  = pFrame->FrClipXEnd - x;
          clipHeight = pFrame->FrClipYEnd - y;
          x          -= pFrame->FrXOrg;
          y          -= pFrame->FrYOrg;
          if (picPresent == FillFrame || picPresent == YRepeat)
	    {
	      /* clipping height is done by the box height */
	      if (y < yFrame)
		{
		  /* reduce the height in delta value */
		  clipHeight = clipHeight + y - yFrame;
		  y = yFrame;
		}
	      if (clipHeight > h)
                clipHeight = h;
	    }
	  else
	    {
	      /* clipping height is done by the image height */
	      delta = yFrame + imageDesc->PicHArea - y;
	      if (delta <= 0)
		clipHeight = 0;
	      else
		clipHeight = delta;
	    }
	  
          if (picPresent == FillFrame || picPresent == XRepeat)
	    {
	      /* clipping width is done by the box width */
	      if (x < xFrame)
		{
		  /* reduce the width in delta value */
		  clipWidth = clipWidth + x - xFrame;
		  x = xFrame;
		}
	      if (clipWidth > w)
		clipWidth = w;
	    }
	  else
	    {
	      /* clipping width is done by the image width */
	      delta = xFrame + imageDesc->PicWArea - x;
	      if (delta <= 0)
		clipWidth = 0;
	      else
		clipWidth = delta;
	    }
          hMemDC  = CreateCompatibleDC (TtDisplay);
          bitmapTiled = CreateCompatibleBitmap (TtDisplay, w, h);
          hOrigDC = CreateCompatibleDC (TtDisplay);
          hrgn = CreateRectRgn (x, y, x + clipWidth, y + clipHeight);
          SelectClipRgn(TtDisplay, hrgn);
          bitmap = SelectObject (hOrigDC, pixmap);
          pBitmapTiled = SelectObject (hMemDC, bitmapTiled);
          
          j = 0;
	  do
	    {
	      i = 0;
	      do
		{
		  /* check if the limits of the copied zone */
		  if (i + imageDesc->PicWArea <= w)
		    iw = imageDesc->PicWArea;
		  else
		    iw = w - i;
		  if (j + imageDesc->PicHArea <= h)
		    jh = imageDesc->PicHArea;
		  else
		    jh = h - j;
		  BitBlt (hMemDC, i, j, iw, jh, hOrigDC, 0, 0, SRCCOPY);
		  i += imageDesc->PicWArea;
		} while (i < w);
	      j += imageDesc->PicHArea;
	    } while (j < h);
#ifndef IV
          BitBlt (TtDisplay, xFrame, yFrame, w, h, hMemDC, 0, 0, SRCCOPY);
#else /* IV */
	  if (imageDesc->PicMask == -1 || imageDesc->PicType == -1)
	    BitBlt (TtDisplay, xFrame, yFrame, w, h, hMemDC, 0, 0, SRCCOPY);
	  else
	    TransparentPicture (bitmapTiled, xFrame, yFrame, w, h,
				imageDesc->PicMask);
#endif /* IV */
	  SelectObject (hOrigDC, bitmap);
	  SelectObject (hMemDC, pBitmapTiled);
          SelectClipRgn(TtDisplay, NULL);

          if (hMemDC)
	    DeleteDC (hMemDC);
          if (hOrigDC)
	    DeleteDC (hOrigDC);
          if (bitmapTiled)
	    DeleteObject (bitmapTiled);
	  if (hrgn)
	    DeleteObject (hrgn);
#endif /* _WINDOWS */
	  break;

	default: break;
	}
    }
}
#endif /*_GL*/

/*----------------------------------------------------------------------
   GetPictureFormat returns the format of a file picture           
   the file  fileName or UNKNOWN_FORMAT if not recognized          
  ----------------------------------------------------------------------*/
static int GetPictureFormat (char *fileName)
{
  int             i;
  int             l;

  i = 0 ;
  l = strlen (fileName);
  while (i < HandlersCounter)
    {
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
Picture_Report PictureFileOk (char *fileName, int *typeImage)
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
    status = Unsupported_Format;
  return status;
}

#if defined (_GTK) || defined (_WINDOWS)
/*----------------------------------------------------------------------
  Create The logo for lost pictures
  ----------------------------------------------------------------------*/
void CreateGifLogo ()
{
#if !defined(_GTK) || defined (_GL) 
  PictInfo            *imageDesc;
  unsigned long       Bgcolor = 0;
  int                 xBox = 0;
  int                 yBox = 0;
  int                 wBox = 0;
  int                 hBox = 0;
  int                 width = 0, height = 0;
  Drawable            drw;
  
  imageDesc = TtaGetMemory (sizeof (PictInfo));
  drw = (*(PictureHandlerTable[GIF_FORMAT].Produce_Picture)) 
    (LostPicturePath, imageDesc, &xBox, &yBox, &wBox,
     &hBox, Bgcolor, &width, &height, 0);
  TtaFreeMemory (imageDesc);
  PictureLogo = (unsigned char *) drw;
#else /*_WIN && _GL*/
  GdkImlibImage      *im;

  im = gdk_imlib_load_image (LostPicturePath);
  gdk_imlib_render (im, 40, 40);
  PictureLogo = gdk_imlib_copy_image (im);
  TtaFreeMemory (im);
#endif /*_WIN && _GL*/
}
#endif /*_GTK && _WIN*/

/*----------------------------------------------------------------------
   Private Initializations of picture handlers and the visual type 
  ----------------------------------------------------------------------*/
void InitPictureHandlers (ThotBool printing)
{
#ifndef _WINDOWS
#ifdef _GTK
  if (!printing)
    {
      /* initialize Graphic context to display pictures */
      TtGraphicGC = gdk_gc_new (DefaultDrawable);
      gdk_rgb_gc_set_foreground (TtGraphicGC, Black_Color);
      gdk_rgb_gc_set_background (TtGraphicGC, White_Color);
      gdk_gc_set_exposures (TtGraphicGC,0);

      /* initialize Graphic context to create pixmap */
      GCimage = gdk_gc_new (DefaultDrawable);
      gdk_rgb_gc_set_foreground (GCimage, Black_Color);
      gdk_rgb_gc_set_background (GCimage, White_Color);
      gdk_gc_set_exposures (GCimage,0);

      /* initialize Graphic context to display tiled pictures */
      tiledGC = gdk_gc_new (DefaultDrawable);
      gdk_gc_set_fill (tiledGC, GDK_TILED);
      gdk_rgb_gc_set_foreground (tiledGC, Black_Color);
      gdk_rgb_gc_set_background (tiledGC, White_Color);
      gdk_gc_set_exposures (tiledGC,0);





































      /* special Graphic context to display bitmaps */
      GCpicture = gdk_gc_new (DefaultDrawable);
      gdk_rgb_gc_set_foreground (GCpicture, Black_Color);
      gdk_rgb_gc_set_background (GCpicture, White_Color);
      gdk_gc_set_exposures (GCpicture,0);
    }
  theVisual = (Visual *) gdk_visual_get_system ();
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
#endif /* _WINDOWS */

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

   InitPngColors ();
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
void GetPictHandlersList (int *count, char* buffer)
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

#ifndef _GTK
/*----------------------------------------------------------------------
   SimpleName

   Si filename est un nom de fichier absolu, retourne dans simplename le nom
   simple du fichier.
  ----------------------------------------------------------------------*/
static void SimpleName (char *filename, char *simplename)
{
   char      *from, *to;
   char       URL_DIR_SEP;

   if (filename && strchr (filename, '/'))
     URL_DIR_SEP = '/';
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
#endif /*_GTK*/

/*----------------------------------------------------------------------
   DrawEpsBox draws the eps logo into the picture box.            
  ----------------------------------------------------------------------*/
static void  DrawEpsBox (PtrBox box, PictInfo *imageDesc, int frame,
			 int wlogo, int hlogo)
{
#ifndef _GTK
   Pixmap              pixmap;
   Drawable            drawable;
   float               scaleX, scaleY;
   int                 x, y, w, h, xFrame, yFrame, wFrame, hFrame;
   int                 XOrg, YOrg, picXOrg, picYOrg;
#ifdef _WINDOWS
   HDC                 hDc, hMemDc;
   POINT               lPt[2];
   HBITMAP             hOldBitmap;
#else  /* _WINDOWS */
   char                filename[255];
   int                 fileNameWidth;
   int                 fnposx, fnposy;
#endif /* _WINDOWS */

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
     default:
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

   LayoutPicture (pixmap, drawable, picXOrg, picYOrg, w, h, x, y, frame,
	              imageDesc, box);
#ifdef _WINDOWS
   if (pixmap)
	   DeleteObject (pixmap);
#else /* _WINDOWS */ 
   XFreePixmap (TtDisplay, pixmap);
   pixmap = None;
   XSetLineAttributes (TtDisplay, TtLineGC, 1, LineSolid, CapButt, JoinMiter);
   XDrawRectangle (TtDisplay, drawable, TtLineGC, xFrame, yFrame,
	               wFrame - 1, hFrame - 1);

   /* Display the filename in the bottom of the Picture Box */
   SimpleName (imageDesc->PicFileName, filename);
   fileNameWidth = XTextWidth ((XFontStruct *) FontDialogue, filename,
	                           strlen (filename));
   if ((fileNameWidth + wlogo <= wFrame) &&
	   (FontHeight (FontDialogue) + hlogo <= hFrame))
     {
       fnposx = (wFrame - fileNameWidth) / 2 + xFrame;
       fnposy = hFrame - 5 + yFrame;
       XSetFont (TtDisplay, TtLineGC, ((XFontStruct *) FontDialogue)->fid);
       XDrawString (TtDisplay, drawable, TtLineGC, fnposx, fnposy,
		            filename, strlen (filename));
     }
#endif /* _WINDOWS */
#endif /* _GTK */
}


/*----------------------------------------------------------------------
  DrawPicture draws the picture in the frame window.
  Parameters x, y, w, h give the displayed area of the box.
  ----------------------------------------------------------------------*/
void DrawPicture (PtrBox box, PictInfo *imageDesc, 
		  int frame, int x,
		  int y, int w, int h)
{
  PathBuffer          fileName;
  PictureScaling      pres;
  Drawable            drawable;
  int                 typeImage;
  int                 picXArea, picYArea, picWArea, picHArea;
  int                 xTranslate, yTranslate, picXOrg, picYOrg;
  int                 xFrame, yFrame;
  int                 bgColor;
#ifdef _WIN_PRINT
  LPBITMAPINFO        pbmi;
  LPBYTE              lpBits;
  BITMAP              bmp;
  WORD                cClrBits;
#endif /* _WIN_PRINT */
  
  if (w == 0 && h == 0)
    /* the picture is not visible */
    return;

  xTranslate = 0;
  yTranslate = 0;
  picXOrg = 0;
  picYOrg = 0;

#ifndef _GL
  if (imageDesc->PicFileName == NULL || imageDesc->PicFileName[0] == EOS || 
      (box->BxAbstractBox->AbLeafType == LtCompound &&
	  imageDesc->PicPixmap == PictureLogo))
    return;
#else /*_GL*/
  if (imageDesc->PicFileName == NULL || imageDesc->PicFileName[0] == EOS || 
      (box->BxAbstractBox->AbLeafType == LtCompound &&
       (strcmp (imageDesc->PicFileName, LostPicturePath) == 0)))
    return;
#endif /* _GL */
  drawable = (Drawable)TtaGetThotWindow (frame);
  GetXYOrg (frame, &xFrame, &yFrame);
  typeImage = imageDesc->PicType;
  GetPictureFileName (imageDesc->PicFileName, fileName);

  /* the default presenation depends on the box type */
  pres = imageDesc->PicPresent;
  if (pres == DefaultPres)
    {
      if (box->BxType == BoPicture)
	/* an image is rescaled */
	pres = ReScale;
      else
	/* a background image is repeated */
	pres = FillFrame;
    }
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
  bgColor = box->BxAbstractBox->AbBackground;
  
  SetPictureClipping (&picWArea, &picHArea, w, h, imageDesc);
  if (!Printing)
    {
      if (imageDesc->PicType == EPS_FORMAT) 
	DrawEpsBox (box, imageDesc, frame, epsflogo_width, epsflogo_height);
      else
	{
#ifdef _GL
	  if ((!glIsTexture (imageDesc->TextureBind)) ||
	      (pres == ReScale &&
	      (imageDesc->PicWArea != w || 
	       imageDesc->PicHArea != h)))	    
#else /*_GL*/
	  if (imageDesc->PicPixmap == None ||
	      (pres == ReScale &&
	      (imageDesc->PicWArea != w || 
	       imageDesc->PicHArea != h)))
#endif /*_GL*/
	    {
	      /* need to load or to rescale the picture */
	      LoadPicture (frame, box, imageDesc);
	      picWArea = imageDesc->PicWArea;
	      picHArea = imageDesc->PicHArea;
	      SetPictureClipping (&picWArea, &picHArea, 
				  w, h, imageDesc);
	    }
	  
	  
	  if (pres == RealSize && 
	      box->BxAbstractBox->AbLeafType == LtPicture)
	    /* Center real sized images wihin their picture boxes */
	    Picture_Center (picWArea, picHArea, w, h, 
			    pres, &xTranslate, &yTranslate,
			    &picXOrg, &picYOrg);
	  
	  if (typeImage >= InlineHandlers)
	    {
	      if (PictureHandlerTable[typeImage].DrawPicture != NULL)
		(*(PictureHandlerTable[typeImage].DrawPicture)) (box, imageDesc,
		                            x + xTranslate, y + yTranslate);
	    }
	  else
	    LayoutPicture ((Pixmap) imageDesc->PicPixmap, drawable,
			   picXOrg, picYOrg, w, h, 
			   x + xTranslate, y + yTranslate, frame,
			   imageDesc, box);
	}
    }
  else if (typeImage < InlineHandlers && typeImage > -1)
    /* for the moment we didn't consider plugin printing */
#ifdef _WINDOWS
    if (TtPrinterDC)
      {
#ifdef _WIN_PRINT
	/* load the device context into TtDisplay */
	WIN_GetDeviceContext (frame);
	LoadPicture (frame, box, imageDesc);
	if (imageDesc->PicPixmap == None) 
	  WinErrorBox (NULL, "DrawPicture (1)");
	else
	  {
	    /* Retrieve the bitmap's color format, width, and height. */ 
	    if (!GetObject ((HBITMAP)(imageDesc->PicPixmap),
			    sizeof(BITMAP), (LPSTR)&bmp))
	      WinErrorBox (NULL, "DrawPicture (1)");
	    else
	      {
		/* Convert the color format to a count of bits. */ 
		cClrBits = (WORD) (bmp.bmPlanes * bmp.bmBitsPixel);
		if (cClrBits != 1)
		  { 
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
		  pbmi = (LPBITMAPINFO) HeapAlloc (GetProcessHeap (), HEAP_ZERO_MEMORY,
						   sizeof (BITMAPINFOHEADER) + (2^cClrBits) * sizeof (RGBQUAD));
		else
		  /* There is no RGBQUAD array for the 24-bit-per-pixel format */ 
		  pbmi = (LPBITMAPINFO) LocalAlloc(LPTR, sizeof(BITMAPINFOHEADER));
		if (pbmi)
		  {
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
		    lpBits = GlobalAlloc (GMEM_FIXED, pbmi->bmiHeader.biSizeImage);
		    if (!lpBits) 
		      WinErrorBox (NULL, "DrawPicture (2)");
		    else
		      {
			if (!GetDIBits (TtDisplay, (HBITMAP) (imageDesc->PicPixmap), 0,
					(UINT)pbmi->bmiHeader.biHeight,
					lpBits, pbmi, DIB_RGB_COLORS)) 
			  WinErrorBox (NULL, "DrawPicture (3)");
			else
			  {
			    StretchDIBits (TtPrinterDC, x, y, w, h, 0, 0,
					   pbmi->bmiHeader.biWidth,
					   pbmi->bmiHeader.biHeight,
					   lpBits, pbmi, DIB_RGB_COLORS, SRCCOPY);
			  }
			GlobalFree (lpBits);
		      }
		    LocalFree (pbmi);
		  }
	      }
	  }
	WIN_ReleaseDeviceContext ();
#endif /* _WIN_PRINT */
      }
#else /* _WINDOWS */
  (*(PictureHandlerTable[typeImage].Produce_Postscript)) (fileName,pres,
							  x, y, w, h,
							  (FILE *) drawable,
							  bgColor);
#endif /* _WINDOWS*/
}

/*----------------------------------------------------------------------
  UnmapImage unmaps plug-in widgets   
  ----------------------------------------------------------------------*/
void UnmapImage (PictInfo *imageDesc)
{
  int   typeImage;

  if (imageDesc == NULL)
    return;
  typeImage = imageDesc->PicType;
#ifndef _WINDOWS
  if (typeImage >= InlineHandlers && imageDesc->mapped &&
      imageDesc->created)
    {	
      XtUnmapWidget ((Widget) (imageDesc->wid));
      imageDesc->mapped = FALSE;
    }
#endif /* _WINDOWS */
}


/*----------------------------------------------------------------------
   Routine handling the zoom-in zoom-out of an image   
  ----------------------------------------------------------------------*/
unsigned char *ZoomPicture (unsigned char *cpic, int cWIDE, int cHIGH ,
			    int eWIDE, int eHIGH, int bperpix)
{
  int            cy, ex, ey;
  int           *cxarr, *cxarrp;
  unsigned char *clptr, *elptr, *epptr, *epic;

  clptr = NULL;
  cxarrp = NULL;
  cy = 0;
  /* check for size */
  if ((cWIDE < 0) || (cHIGH < 0) || (eWIDE < 0) || (eHIGH < 0) ||
      (cWIDE > 2000) || (cHIGH > 2000) || (eWIDE > 2000) || (eHIGH > 2000))
    return (NULL);

  /* generate a 'raw' epic, as we'll need it for ColorDither if EM_DITH */
  if (eWIDE == cWIDE && eHIGH == cHIGH)
    /* 1:1 expansion.  points destination pic at source pic */
    epic = cpic;
  else
    {
      /* run the rescaling algorithm */
      /* create a new pic of the appropriate size */
      epic = (char *) TtaGetMemory((size_t) (eWIDE * eHIGH * bperpix));
      if (!epic)
	printf(" unable to TtaGetMemory memory for zoomed image \n");
      cxarr = (int *) TtaGetMemory (eWIDE * sizeof (int));
      if (!cxarr)
	printf("unable to allocate cxarr for zoomed image \n");
      for (ex = 0; ex < eWIDE; ex++) 
	cxarr[ex] = bperpix * ((cWIDE * ex) / eWIDE);
      
      elptr = epptr = epic;
      for (ey = 0;  ey < eHIGH;  ey++, elptr += (eWIDE * bperpix))
	{
	  cy = (cHIGH * ey) / eHIGH;
	  epptr = elptr;
	  clptr = cpic + (cy * cWIDE * bperpix);
	  
	  if (bperpix == 1)
	    {
	      for (ex = 0, cxarrp = cxarr;  ex < eWIDE;  ex++, epptr++) 
	      *epptr = clptr[*cxarrp++];
	    }
	  else
	    {
	      int j;  char *cp;
	      for (ex = 0, cxarrp = cxarr; ex < eWIDE; ex++,cxarrp++)
		{
		  cp = clptr + *cxarrp;
		  for (j = 0; j < bperpix; j++) 
		    *epptr++ = *cp++;
		}
	    }
	}
      TtaFreeMemory (cxarr);
    }
  
  return (epic);
}

#ifdef _GL
/*----------------------------------------------------------------------
   Requests the picture handlers to get the corresponding RGB or RGBA buffer
   and make a Texture or it (aka load into video card memory)   
  ----------------------------------------------------------------------*/
void LoadPicture (int frame, PtrBox box, PictInfo *imageDesc)
{
  PathBuffer          fileName;
  PictureScaling      pres;
  PtrAbstractBox      pAb;
  Picture_Report      status;
  unsigned long       Bgcolor;
  int                 typeImage;
  int                 xBox = 0;
  int                 yBox = 0;
  int                 wBox =0;
  int                 hBox = 0;
  int                 w, h;
  int                 width, height;
  int                 left, right, top, bottom;

  left = box->BxLMargin + box->BxLBorder + box->BxLPadding;
  right = box->BxRMargin + box->BxRBorder + box->BxRPadding;
  top = box->BxTMargin + box->BxTBorder + box->BxTPadding;
  bottom = box->BxBMargin + box->BxBBorder + box->BxBPadding;
  pAb = box->BxAbstractBox;

  if (pAb->AbVisibility < ViewFrameTable[frame - 1].FrVisibility)
    /* the picture is not visible */
    return;

  if (imageDesc->PicFileName == NULL || 
      imageDesc->PicFileName[0] == EOS)
    return;
 GetPictureFileName (imageDesc->PicFileName, fileName);
 /* For the Sync Image*/
 if (frame != ActiveFrame)
     GL_prepare (frame); 
 typeImage = LookupInPicCache (imageDesc, frame); 
 if (typeImage)
    {  
      typeImage = imageDesc->PicType;
      pres = imageDesc->PicPresent;
      if (pres == DefaultPres)
	{
	  if (box->BxType == BoPicture)
	    /* an image is rescaled */
	    pres = ReScale;
	  else
	    /* a background image is repeated */
	    pres = FillFrame;
	}
      if ((typeImage == XBM_FORMAT || typeImage == XPM_FORMAT) && 
	  pres == ReScale)
	pres = imageDesc->PicPresent = RealSize;
      /* picture dimension */
      if (pAb->AbLeafType == LtCompound)
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
      /* xBox and yBox get the box size if picture is */
      /* rescaled and receives the position of the picture */
      if (pres != ReScale || Printing)
	{
	  xBox = 0;
	  yBox = 0;
	}
      else
	{
	  if (box->BxW != 0)
	    xBox = w;
	  if(box->BxH != 0)
	    yBox = h;
	}

      if (pres != ReScale || Printing)
	{
	  imageDesc->PicXArea = xBox;
	  imageDesc->PicYArea = yBox;
	  imageDesc->PicWArea = wBox;
	  imageDesc->PicHArea = hBox;
	}
      else
	{
	  imageDesc->PicXArea = xBox;
	  imageDesc->PicYArea = yBox;
	  imageDesc->PicWArea = w;
	  imageDesc->PicHArea = h;
	}
      /* Gif and Png handles transparency 
	 so picture format is RGBA, 
	 all others are RGB*/
      if (typeImage != GIF_FORMAT 
	  && typeImage != PNG_FORMAT)
	imageDesc->RGBA = FALSE;
      else
	imageDesc->RGBA = TRUE;
      /*
	We succesfully get 
	image from the cache so..
      */
      return;      
    }



  typeImage = imageDesc->PicType;
  status = PictureFileOk (fileName, &typeImage);
  w = 0;
  h = 0;
  Bgcolor = ColorPixel (pAb->AbBackground);

  /* clean up the current image descriptor */
  /*CleanPictInfo (imageDesc);*/

  if (status == Supported_Format)
    {
      /* Supported format */
      imageDesc->PicType = typeImage;
      pres = imageDesc->PicPresent;
      if (pres == DefaultPres)
	{
	  if (box->BxType == BoPicture)
	    /* an image is rescaled */
	    pres = ReScale;
	  else
	    /* a background image is repeated */
	    pres = FillFrame;
	}
      if ((typeImage == XBM_FORMAT || typeImage == XPM_FORMAT) && 
	  pres == ReScale)
	pres = imageDesc->PicPresent = RealSize;
      /* picture dimension */
      if (pAb->AbLeafType == LtCompound)
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
      
      if (PictureHandlerTable[typeImage].Produce_Picture != NULL)
	{
	  if (typeImage >= InlineHandlers)
	    {
	      /* Plugins are displayed in RealSize */
	      imageDesc->PicPresent = RealSize;
	      imageDesc->PicWArea = wBox = w;
	      imageDesc->PicHArea = hBox = h;

	      imageDesc->PicPixmap = (unsigned char *) 
		(*(PictureHandlerTable[typeImage].Produce_Picture)) 
		(frame, imageDesc, fileName);
      
	      xBox = imageDesc->PicXArea;
	      yBox = imageDesc->PicYArea;
	    }
	  else
	    {
	      /* xBox and yBox get the box size if picture is */
	      /* rescaled and receives the position of the picture */
	      if (pres != ReScale || Printing)
		{
		  xBox = 0;
		  yBox = 0;
		}
	      else
		{
		  if (box->BxW != 0)
		    xBox = w;
		  if(box->BxH != 0)
		    yBox = h;
		}
	      if (imageDesc->TextureBind == 0)
		imageDesc->PicPixmap = (unsigned char *) 
		  (*(PictureHandlerTable[typeImage].Produce_Picture))
		  (fileName, imageDesc, &xBox, &yBox, &wBox, &hBox,
		   Bgcolor, &width, &height,
		   ViewFrameTable[frame - 1].FrMagnification);
	      else
		CacheLookupHeightAndWidth (imageDesc,
					   &width,
					   &height,
					   frame); 
	    }
	  /* intrinsic width and height */
	  imageDesc->PicWidth  = width;
	  imageDesc->PicHeight = height;
	}
    }

  /* Picture didn't load (corrupted, don't exists...)
     or format isn't supported*/
  if (imageDesc->PicPixmap == None 
      && !glIsTexture (imageDesc->TextureBind))
    {
      if (PictureLogo == None)
	/* create a special logo for lost pictures */
	CreateGifLogo ();
      imageDesc->PicFileName = TtaGetMemory (strlen(LostPicturePath)+1);
      strcpy (imageDesc->PicFileName,TtaStrdup (LostPicturePath));
      imageDesc->PicType = 3;
      imageDesc->PicPresent = RealSize;
      imageDesc->PicPixmap = (unsigned char*) PictureLogo;
	  typeImage = GIF_FORMAT;
		  /*TtaGetMemory (sizeof (unsigned char) * 6400);
	  memcpy (imageDesc->PicPixmap, PictureLogo, sizeof (unsigned char) * 6400);*/

      /* convert logo into a texture map 
      GL_w = (GLfloat) Image->PicWidth/p2_w;
      GL_h = (GLfloat) Image->PicHeight/p2_h; 
     We give te texture to opengl Pipeline system 
      GL_MakeTextureSize (Image, p2_w, p2_h);
	 */
      wBox = w = 40;
      hBox = h = 40;
	  imageDesc->PicWidth = w;
      imageDesc->PicHeight = h;
    }
  else if (w == 0 || h == 0)
    {
      /* one of box size is unknown, keep the image size */
      if (w == 0)
	w = wBox;
      if (h == 0)
	h = hBox;
      /* Do you have to extend the clipping */
      if (pAb->AbLeafType == LtCompound)
	DefClip (frame, box->BxXOrg, box->BxYOrg,
		 box->BxXOrg + w, box->BxYOrg + h);
      else
	DefClip (frame, box->BxXOrg - left, box->BxYOrg - top,
		 box->BxXOrg + right + w, box->BxYOrg + bottom + h);
      if (pAb->AbLeafType == LtPicture)
	{
	  /* transmit picture dimensions */
	  if (!(pAb->AbWidth.DimIsPosition))
	    {
	      if (pAb->AbWidth.DimMinimum)
		/* the rule min is applied to this box */
		ChangeDefaultWidth (box, box, w, 0, frame);
	      else if (pAb->AbEnclosing &&
		       pAb->AbWidth.DimAbRef == pAb->AbEnclosing &&
		       pAb->AbNext == NULL && pAb->AbPrevious == NULL)
		/* the parent box should inherit the picture width */
		ChangeWidth (pAb->AbEnclosing->AbBox,
			     pAb->AbEnclosing->AbBox, NULL,
			     w + left + right, 0, frame);
	    }
	  if (!(pAb->AbHeight.DimIsPosition))
	    {
	      if (pAb->AbHeight.DimMinimum)
		/* the rule min is applied to this box */
		ChangeDefaultHeight (box, box, h, frame);
	      else if (pAb->AbEnclosing &&
		       pAb->AbWidth.DimAbRef == pAb->AbEnclosing &&
		       pAb->AbNext == NULL && pAb->AbPrevious == NULL)
		/* the parent box should inherit the picture height */
		ChangeHeight (pAb->AbEnclosing->AbBox,
			      pAb->AbEnclosing->AbBox, NULL,
			      h + top + bottom + top + bottom, frame);
	    }
	}
    }  
  if (pres != ReScale || Printing)
    {
      imageDesc->PicXArea = xBox;
      imageDesc->PicYArea = yBox;
      imageDesc->PicWArea = wBox;
      imageDesc->PicHArea = hBox;
    }
  else
    {
      imageDesc->PicXArea = xBox;
      imageDesc->PicYArea = yBox;
      imageDesc->PicWArea = w;
      imageDesc->PicHArea = h;
    }
  /* Gif and Png handles transparency 
     so picture format is RGBA, 
     all others are RGB*/
  if (typeImage != GIF_FORMAT 
      && typeImage != PNG_FORMAT)
    imageDesc->RGBA = FALSE;
  else
    imageDesc->RGBA = TRUE;

  GL_TextureBind (imageDesc);
  /*frame or ActiveFrame*/
  AddInPicCache (imageDesc, frame); 

  /* For the Sync Image*/
  if (frame != ActiveFrame)
    GL_prepare (ActiveFrame); 
}

void *Group_shot (int x, int y, int width, int height, int frame)
{
  PictInfo *imageDesc = NULL;

  if (GL_prepare (frame))
    {
      imageDesc = malloc (sizeof (PictInfo));  
      imageDesc->PicFileName = "testing";
      imageDesc->RGBA = TRUE;
      imageDesc->PicWidth = width;
      imageDesc->PicHeight = height;
      imageDesc->PicXArea = 0;
      imageDesc->PicYArea = 0;
      imageDesc->PicWArea = width;
      imageDesc->PicHArea = height; 
      imageDesc->PicPixmap = TtaGetMemory (sizeof (unsigned char) * width * height * 4);
      imageDesc->TextureBind = 0; 

      glFlush ();
      glFinish ();
      glReadBuffer (GL_BACK);  

      glReadPixels (x, y, width, height, 
		    GL_RGBA, 
		    GL_UNSIGNED_BYTE, 
		    imageDesc->PicPixmap);

  /* SavePng ("/home/cheyroul/test.png", */
/*         imageDesc->PicPixmap, */
/*         (unsigned int) width, */
/*         (unsigned int) height); */

      GL_TextureBind (imageDesc);


      return imageDesc;
    }
  else
    return NULL;
  
}

void testing_gradient ()
{
  static PictInfo *imageDesc = NULL;

  if (imageDesc == NULL)
    {
      imageDesc = malloc (sizeof (PictInfo));  
      imageDesc->PicFileName = "testing";
      imageDesc->RGBA = TRUE;
      imageDesc->PicWidth = 100;
      imageDesc->PicHeight = 100;
      imageDesc->PicXArea = 0;
      imageDesc->PicYArea = 0;
      imageDesc->PicWArea = 100;
      imageDesc->PicHArea = 100;
      imageDesc->PicPixmap = test_gradien_linear ();   
 
      GL_TextureBind (imageDesc);
    }
  
  GL_StencilMap (imageDesc, 
		 0, 
		 0,
		 imageDesc->PicWidth, 
		 imageDesc->PicHeight);
}


#else /* _GL */    

/*----------------------------------------------------------------------
   Requests the picture handlers to get the corresponding pixmaps    
  ----------------------------------------------------------------------*/
void LoadPicture (int frame, PtrBox box, PictInfo *imageDesc)
{
  PathBuffer          fileName;
  PictureScaling      pres;
#ifdef _GTK
#ifndef _GTK2
  GdkImlibImage      *im = None;
#ifndef _GL
  GdkPixmap          *drw = None;
#else /* _GL*/
  unsigned char*  drw = NULL;
#endif /* _GL */
#else /* _GTK2 */
  GdkPixbuf          *im = None;
  GError             *error=NULL;
#endif /* _GTK2 */
#else /* _GTK */
  Drawable            drw = None;
#endif /* _GTK */
  PtrAbstractBox      pAb;
  Picture_Report      status;
  unsigned long       Bgcolor;
  int                 typeImage;
  int                 xBox = 0;
  int                 yBox = 0;
  int                 wBox = 0;
  int                 hBox = 0;
  int                 w, h;
  int                 width, height;
  int                 left, right, top, bottom;

#ifdef _WINDOWS
  ThotBool            releaseDC = FALSE;
#endif
  left = box->BxLMargin + box->BxLBorder + box->BxLPadding;
  right = box->BxRMargin + box->BxRBorder + box->BxRPadding;
  top = box->BxTMargin + box->BxTBorder + box->BxTPadding;
  bottom = box->BxBMargin + box->BxBBorder + box->BxBPadding;
  pAb = box->BxAbstractBox;
  if (pAb->AbVisibility < ViewFrameTable[frame - 1].FrVisibility)
    /* the picture is not visible */
    return;
  if (imageDesc->PicFileName == NULL || imageDesc->PicFileName[0] == EOS)
    return;

  GetPictureFileName (imageDesc->PicFileName, fileName);

  typeImage = imageDesc->PicType;
  status = PictureFileOk (fileName, &typeImage);
  w = 0;
  h = 0;
  Bgcolor = ColorPixel (pAb->AbBackground);
  /* clean up the current image descriptor */
  /*CleanPictInfo (imageDesc);*/
  if (status != Supported_Format)
    {
      pres = RealSize;
#if defined (_GTK) || defined (_WINDOWS)
	  if (PictureLogo == None)
	    /* create a special logo for lost pictures */
	    CreateGifLogo ();
#endif 
#ifdef _WINDOWS
#ifdef _WIN_PRINT
      if (TtDisplay == NULL)
	{
	  /* load the device context into TtDisplay */
	  WIN_GetDeviceContext (frame);
	  releaseDC = TRUE;
	}
#else /* _WIN_PRINT */
      imageDesc->PicType = 3;
      imageDesc->PicHeight = 40;
      imageDesc->PicWidth = 40;
      imageDesc->PicPresent = pres;
      drw = PictureLogo;
#endif /* _WIN_PRINT */
#else  /* _WINDOWS */
#ifdef _GTK 
      imageDesc->PicType = 3;
      imageDesc->PicPresent = pres;
      imageDesc->PicHeight = 40;
      imageDesc->PicWidth = 40;
      drw = (GdkPixmap *) PictureLogo;
#else /*_GTK*/
      drw = PictureLogo;
      imageDesc->PicType = -1;
#endif /* ! GTK */
#endif /* _WINDOWS */
      wBox = w = 40;
      hBox = h = 40;
    }
  else
    {
      /* Supported format */
      imageDesc->PicType = typeImage;
      pres = imageDesc->PicPresent;
      if (pres == DefaultPres)
	{
	  if (box->BxType == BoPicture)
	    /* an image is rescaled */
	    pres = ReScale;
	  else
	    /* a background image is repeated */
	    pres = FillFrame;
	}
      if ((typeImage == XBM_FORMAT || typeImage == XPM_FORMAT) && pres == ReScale)
	pres = imageDesc->PicPresent = RealSize;
      /* picture dimension */
      if (pAb->AbLeafType == LtCompound)
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
#ifndef _GTK
	  /* set the colors of the  graphic context GC */
	  if (TtWDepth == 1)
	    {
	      /* Black and white screen */
	      XSetForeground (TtDisplay, TtGraphicGC, Black_Color);
	      XSetBackground (TtDisplay, TtGraphicGC,
			      ColorPixel (BackgroundColor[frame]));
	    }
	  else if (pAb->AbSensitive && !pAb->AbPresentationBox)
	    {
	      /* Set active Box Color */
	      XSetForeground (TtDisplay, TtGraphicGC, Box_Color);
	      XSetForeground (TtDisplay, GCpicture, Box_Color);
	      XSetBackground (TtDisplay, TtGraphicGC,
			      ColorPixel (pAb->AbBackground));
	    }
	  else
	    {
	      /* Set Box Color */
	      XSetForeground (TtDisplay, TtGraphicGC,
			      ColorPixel (pAb->AbForeground));
	      XSetForeground (TtDisplay, GCpicture,
			      ColorPixel (pAb->AbForeground));
	      XSetBackground (TtDisplay, TtGraphicGC,
			      ColorPixel (pAb->AbBackground));
	    }
#endif /* _GTK */
#endif /* _WINDOWS */
	}
#ifndef _GTK
      if (PictureHandlerTable[typeImage].Produce_Picture != NULL)
	{
#endif /* _GTK */
	  if (typeImage >= InlineHandlers)
	    {
	      /* Plugins are displayed in RealSize */
	      imageDesc->PicPresent = RealSize;
#ifndef _GTK
	      imageDesc->PicWArea = wBox = w;
	      imageDesc->PicHArea = hBox = h;
	      drw = (*(PictureHandlerTable[typeImage].Produce_Picture)) (frame, imageDesc, fileName);
#else /* _GTK */
	      imageDesc->PicWArea = wBox = w;
	      imageDesc->PicHArea = hBox = h;
#ifndef _GTK2
	      im = gdk_imlib_load_image (fileName);
#ifndef _GL
	      gdk_imlib_render(im, w, h);
	      drw = gdk_imlib_move_image (im);
	      imageDesc->PicMask = (Pixmap) gdk_imlib_move_mask (im);
#else /* _GL */
	      /* opengl draw in the other way...*/
	      gdk_imlib_flip_image_vertical (im);
	      /* opengl texture have size that is a power of 2*/
#ifndef MESA
	      /* opengl texture have size that is a power of 2*/
	      drw = GL_MakeTexture (im->rgb_data, 
				    im->shape_color.r, 
				    im->shape_color.g,
				    im->shape_color.b,
				    w, h);
#else /* MESA*/
	      drw = GL_MakeTransparentRGB (im->rgb_data, 
					   im->shape_color.r, 
					   im->shape_color.g,
					   im->shape_color.b,
					   w, h);
#endif/*  MESA */
#endif /* _GL */
#else /* _GTK2 */
	      im = gdk_pixbuf_new_from_file(fileName, &error);
#endif /* _GTK2 */
#endif /* _GTK */
	      xBox = imageDesc->PicXArea;
	      yBox = imageDesc->PicYArea;
	    }
	  else
	    {
	      /* xBox and yBox get the box size if picture is */
	      /* rescaled and receives the position of the picture */
	      if (pres != ReScale || Printing)
		{
		  xBox = 0;
		  yBox = 0;
		}
	      else
		{
		  if (box->BxW != 0)
		    xBox = w;
		  if(box->BxH != 0)
		    yBox = h;
		}
#ifndef _GTK
	      drw = (*(PictureHandlerTable[typeImage].Produce_Picture))
		(fileName, imageDesc, &xBox, &yBox, &wBox, &hBox,
		 Bgcolor, &width, &height,
		 ViewFrameTable[frame - 1].FrMagnification);

#else /* _GTK */
	      if (typeImage == EPS_FORMAT)
		drw = (GdkPixmap *) (*(PictureHandlerTable[typeImage].Produce_Picture))
		  (fileName, imageDesc, &xBox, &yBox, &wBox, &hBox,
		   Bgcolor, &width, &height,
		   ViewFrameTable[frame - 1].FrMagnification);
	      else
		{
		  /* load the picture using ImLib */
		  im = gdk_imlib_load_image (fileName);	      
		  if (pres == RealSize)
		    {
		      /* if it's a background, dont rescale the picture */
		      wBox = im->rgb_width;
		      hBox = im->rgb_height;
		      if (xBox == 0)
			xBox = im->rgb_width;
		      if (yBox == 0)
			yBox = im->rgb_height;
		    }
		  else
		    {
		      if (wBox == 0)
			wBox = im->rgb_width;
		      if (hBox == 0)
			hBox = im->rgb_height;
		      if (xBox == 0)
			xBox = im->rgb_width;
		      if (yBox == 0)
			yBox = im->rgb_height;
		    }
#ifndef _GL
		  gdk_imlib_render(im,
				   (gint)xBox,
				   (gint)yBox);
		  drw = (GdkPixmap *) gdk_imlib_move_image (im);
		  imageDesc->PicMask = (Pixmap) gdk_imlib_move_mask (im);
#else /* _GL */
		  /* opengl draw in the other way...*/
		  gdk_imlib_flip_image_vertical (im);
#ifndef MESA
		  /* opengl texture have size that is a power of 2*/
		  drw = GL_MakeTexture (im->rgb_data, 
					im->shape_color.r, 
					im->shape_color.g,
					im->shape_color.b,
					(gint)wBox ,
					(gint)hBox);
#else /* MESA*/
		  drw = GL_MakeTransparentRGB(im->rgb_data, 
					      im->shape_color.r, 
					      im->shape_color.g,
					      im->shape_color.b,
					      (gint)wBox, 
					      (gint)hBox);
#endif/*  MESA */
#endif /* _GL */
		}
	      width = (gint) wBox;
	      height = (gint) hBox;
#endif /* _GTK */
	      /* intrinsic width and height */
	      imageDesc->PicWidth  = width;
	      imageDesc->PicHeight = height;
	    }
#ifndef _GTK
	}
#endif /* _GTK */
       
      if (drw == None)
	{
#if defined (_GTK) || defined (_WINDOWS)
	  if (PictureLogo == None)
	    /* create a special logo for lost pictures */
	    CreateGifLogo ();
#endif 
#ifdef _WINDOWS
#ifdef _WIN_PRINT
      if (TtDisplay == NULL)
	{
	  /* load the device context into TtDisplay */
	  WIN_GetDeviceContext (frame);
	  releaseDC = TRUE;
	}
#else /* _WIN_PRINT */
      imageDesc->PicType = 3;
      imageDesc->PicPresent = pres;
      drw = PictureLogo;
#endif /* _WIN_PRINT */
#else  /* _WINDOWS */
#ifdef _GTK 
      imageDesc->PicType = 3;
      imageDesc->PicPresent = pres;
      imageDesc->PicFileName = TtaStrdup (LostPicturePath);
      drw = (GdkPixmap *) PictureLogo;
#else /*_GTK*/
      drw = PictureLogo;
      imageDesc->PicType = -1;
#endif /* ! GTK */
#endif /* _WINDOWS */
      wBox = w = 40;
      hBox = h = 40;
	}
      else if (w == 0 || h == 0)
	{
	  /* one of box size is unknown, keep the image size */
	  if (w == 0)
	    w = wBox;
	  if (h == 0)
	    h = hBox;
	  /* Do you have to extend the clipping */
	  if (pAb->AbLeafType == LtCompound)
	    DefClip (frame, box->BxXOrg, box->BxYOrg,
		     box->BxXOrg + w, box->BxYOrg + h);
	  else
	    DefClip (frame, box->BxXOrg - left, box->BxYOrg - top,
		     box->BxXOrg + right + w, box->BxYOrg + bottom + h);
	  if (pAb->AbLeafType == LtPicture)
	    {
	      /* transmit picture dimensions */
	      if (!(pAb->AbWidth.DimIsPosition))
		{
		  if (pAb->AbWidth.DimMinimum)
		    /* the rule min is applied to this box */
		    ChangeDefaultWidth (box, box, w, 0, frame);
		  else if (pAb->AbEnclosing &&
			   pAb->AbWidth.DimAbRef == pAb->AbEnclosing &&
			   pAb->AbNext == NULL && pAb->AbPrevious == NULL)
		    /* the parent box should inherit the picture width */
		    ChangeWidth (pAb->AbEnclosing->AbBox,
				 pAb->AbEnclosing->AbBox, NULL,
				 w + left + right, 0, frame);
		}
	      if (!(pAb->AbHeight.DimIsPosition))
		{
		  if (pAb->AbHeight.DimMinimum)
		    /* the rule min is applied to this box */
		    ChangeDefaultHeight (box, box, h, frame);
		  else if (pAb->AbEnclosing &&
			   pAb->AbWidth.DimAbRef == pAb->AbEnclosing &&
			   pAb->AbNext == NULL && pAb->AbPrevious == NULL)
		    /* the parent box should inherit the picture height */
		    ChangeHeight (pAb->AbEnclosing->AbBox,
				  pAb->AbEnclosing->AbBox, NULL,
				  h + top + bottom + top + bottom, frame);
		}
	    }
	}
    }

  if (pres != ReScale || Printing)
    {
      imageDesc->PicXArea = xBox;
      imageDesc->PicYArea = yBox;
      imageDesc->PicWArea = wBox;
      imageDesc->PicHArea = hBox;
    }
  else
    {
      imageDesc->PicXArea = xBox;
      imageDesc->PicYArea = yBox;
      imageDesc->PicWArea = w;
      imageDesc->PicHArea = h;
    }
  imageDesc->PicPixmap = drw;
#ifdef _GTK
  if (im)
    imageDesc->im = im;
#endif /*_GTK*/
#ifdef _WIN_PRINT
  if (releaseDC)
    /* release the device context into TtDisplay */
    WIN_ReleaseDeviceContext ();
#endif /* _WIN_PRINT */
}
#endif /*_GL*/


/*----------------------------------------------------------------------
   GetPictureType returns the type of the image based on the index 
   in the GUI form.                                        
  ----------------------------------------------------------------------*/
int GetPictureType (int GUIIndex)
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
int GetPictTypeIndex (int picType)
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
int GetPictPresIndex (PictureScaling picPresent)
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
      i = (int) ReScale;
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
void GetPictureHandlersList (int *count, char* buffer)
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
void LittleXBigEndian (register unsigned char *b, register long n)
{
   do
     {
	*b = MirrorBytes[*b];
	b++;
     }
   while (--n > 0);
}


/*----------------------------------------------------------------------
  GetScreenshot makes a screenshot of Amaya drawing area
  this function allocates and returns the screenshot
  ----------------------------------------------------------------------*/
unsigned char *GetScreenshot (int frame, char *pngurl)
{
#ifdef _GL
  saveBuffer (pngurl, FrameTable[frame].FrWidth, FrameTable[frame].FrHeight);
  return NULL;
#else /*_GL*/
  int              widthb, heightb;
#ifdef _GTK
  GdkImlibImage *image;  
#else /* !_GTK */
#ifdef _WINDOWS
  int              x,y;
  DWORD            RGBcolor;
  unsigned char   *pixel = NULL;
  unsigned char   *screenshot = NULL;
  int              i = 0;
  HDC              SurfDC = NULL;
  HDC              OffscrDC = NULL; 
  HBITMAP          OffscrBmp = NULL;
  HBITMAP          OldBmp = NULL;
  LPBITMAPINFO     lpbi = NULL;
  LPVOID           lpvBits = NULL; 
  RECT             rect;
#endif /* _WINDOWS */
#endif /* _GTK */
  TtaHandlePendingEvents ();
#ifdef _GTK
  widthb = (FrameTable[frame].WdFrame)->allocation.width;
  heightb = (FrameTable[frame].WdFrame)->allocation.height;

  image =  gdk_imlib_create_image_from_drawable (FrRef[frame], NULL, 0, 0, widthb, heightb);
  gdk_imlib_save_image (image, pngurl, NULL);
  gdk_imlib_destroy_image (image);
  
  return NULL;
#else /* !_GTK */
#ifdef _WINDOWS
    GetClientRect (FrRef[frame], &rect);
    widthb = rect.right;
    heightb = rect.bottom;
	SurfDC = GetDC (FrRef[frame]);
	pixel = TtaGetMemory (heightb*widthb*4);
    memset (pixel, 255, sizeof (heightb*widthb*4));
	for (y = heightb; y > 0; y--)
		for (x = 0; x < widthb; x++)
		{
			RGBcolor = GetPixel(SurfDC, x - rect.left, y -rect.top);
			pixel [i++] = GetRValue (RGBcolor);
			pixel [i++] = GetGValue (RGBcolor);
			pixel [i++] = GetBValue (RGBcolor);
			i++; /*opacity*/
		}
	screenshot = pixel;
  SavePng (pngurl,
	   screenshot,
	   (unsigned int) widthb,
	   (unsigned int) heightb);
  return screenshot;
#endif /* _WINDOWS */
#endif /* _GTK */
#endif /*_GL*/
}
