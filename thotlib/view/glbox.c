/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2009
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * glbox.c : Handling of Double buffering and Bounding box calculation, 
 *           with Opengl library both for MS-Windows and Unix
 *
 * Author: P. Cheyrou-lagreze (INRIA)
 *
 */
#ifdef _GL
#ifdef _WINGUI
  #include <windows.h>
#endif /* _WINGUI */

#ifdef _WX
  #include "wx/wx.h"
  #include "wx/glcanvas.h"
  #include "AmayaFrame.h"
#else /* _WX */
  #include <GL/gl.h>
#endif /* _WX */
#endif /* _GL */
#include "ustring.h"
#include "math.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "frame.h"
#include "logdebug.h"

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "boxes_tv.h"
#include "font_tv.h"
#include "frame_tv.h"
#include "units_tv.h"
#include "edit_tv.h"
#include "thotcolor_tv.h"

#define ALLOC_POINTS    300

#include "boxlocate_f.h"
#include "buildlines_f.h"
#include "context_f.h"
#include "font_f.h"
#include "inites_f.h"
#include "memory_f.h"
#include "stix_f.h"
#include "units_f.h"
#include "xwindowdisplay_f.h"
#include "displaybox_f.h"
#include "frame_f.h"
#include "tesse_f.h"
#include "glwindowdisplay.h"

#ifdef _GL
/*
 * Math Macros conversion from
 * degrees to radians and so on...
 * All for EllipticSplit and/or GL_DrawArc
 */
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#define M_PI_DOUBLE (6.2831853718027492)

/* ((A)*(M_PI/180.0)) */
#define DEG_TO_RAD(A)   ((float)A)/57.29577957795135
#define RAD_TO_DEG(A)   ((float)A)*57.29577957795135

#define IS_ZERO(arg)                    (fabs(arg)<1.e-20)

/*If we should use a static table instead for
  performance bottleneck...*/
#define DCOS(A) ((float)cos (A))
#define DSIN(A) ((float)sin (A))
#define DACOS(A) ((float)acos (A))
#define A_DEGREE 0.017453293

/* Precision of a degree/1 
   If we need more precision 
   dision is our friend 
   1/2 degree = 0.0087266465
   1/4 degree = 0.0043633233
   or the inverse 
   24 degree = 0.41887903
   5 degree = 0.087266465
   2 degree = 0.034906586
   but best is a degree...
*/
#define TRIGO_PRECISION 1;
#define A_DEGREE_PART A_DEGREE/TRIGO_RECISION

/* Must find better one... 
   Bits methods...*/
/*for float => ( pow (N, 2))*/
/*for int  => (((int)N)<<1)*/
#define P2(N) (N*N)


#define	MIDDLE_OF(v1, v2) (((v1)+(v2))/2.0)
#define ALLOC_POINTS    300
#define MESA

#define FEEDBUFFERSIZE 32768

/*if just computing bounding box*/
static ThotBool NotFeedBackMode = TRUE;
/* background color*/
static int      GL_Background[MAX_FRAME];

/*----------------------------------------------------------------------
  SetMainWindowBackgroundColor :                          
  ----------------------------------------------------------------------*/
void SetMainWindowBackgroundColor (int frame, int color)
{
  unsigned short red, green, blue;

#ifdef _GL
  /* to be sure that the frame is the current one when drawing its background */
  GL_prepare(frame);
  GL_Background[frame] = color;
#endif /* _GL */
  if (color != -1)
    TtaGiveThotRGB (color, &red, &green, &blue);
  else
    TtaGiveThotRGB (WindowBColor, &red, &green, &blue);
  /* the 0.0 for alpha is needed for group opacity */
  glClearColor ((float)red/255., (float)green/255., (float)blue/255., 0.0);
#ifdef _GL_COLOR_DEBUG
  {
    float tmp[4];
    glGetFloatv( GL_COLOR_CLEAR_VALUE, tmp );
    TTALOGDEBUG_5( TTA_LOG_DRAW, _T("glClearColor CLEAR_VALUE(%f,%f,%f,%f) - frame=%d"),tmp[0],tmp[1],tmp[2],tmp[3],frame );
  }
#endif /* _GL_COLOR_DEBUG */
}

/*----------------------------------------------------------------------
  ResetMainWindowBackgroundColor :                          
  ----------------------------------------------------------------------*/
void ResetMainWindowBackgroundColor (int frame)
{
  unsigned short red, green, blue;
  int color = GL_Background[frame];

#ifdef _GL
  /* to be sure that the frame is the current one when drawing its background */
  GL_prepare(frame);
#endif /* _GL */
  if (color != -1)
      TtaGiveThotRGB (color, &red, &green, &blue);
  else
    TtaGiveThotRGB (WindowBColor, &red, &green, &blue);
  /* the 0.0 for alpha is needed for group opacity */
  glClearColor ((float)red/255., (float)green/255., (float)blue/255., 0.0);

#ifdef _GL_COLOR_DEBUG
  float tmp[4];
  glGetFloatv( GL_COLOR_CLEAR_VALUE, tmp );
  TTALOGDEBUG_5( TTA_LOG_DRAW, _T("glClearColor CLEAR_VALUE(%f,%f,%f,%f) - frame=%d"),tmp[0],tmp[1],tmp[2],tmp[3],frame);
#endif /* _GL_COLOR_DEBUG */
}

/*----------------------------------------------------------------------
  Clear clear the area of frame located at (x, y) and of size width x height.
  ----------------------------------------------------------------------*/
void Clear (int frame, int width, int height, int x, int y)
{
  int        bottom;

  if (GL_prepare (frame))
    {
      bottom = FrameTable[frame].FrHeight + FrameTable[frame].FrTopMargin;
      if (bottom > y+height)
	{
	  GL_SetClipping (x, bottom - (y + height), width, height);
#ifdef _GL_COLOR_DEBUG
	  {
	    float tmp[4];
	    glGetFloatv( GL_COLOR_CLEAR_VALUE, tmp );
	    TTALOGDEBUG_5( TTA_LOG_DRAW, _T("glClear CLEAR_VALUE(%f,%f,%f,%f) - frame"),tmp[0],tmp[1],tmp[2],tmp[3],frame );
	  }
#endif /* _GL_COLOR_DEBUG */
	  glClear( GL_COLOR_BUFFER_BIT );
	  /*GL_UnsetClippingRestore (TRUE);*/
	}
    }
}

#ifdef _WINGUI 
/*----------------------------------------------------------------------
  ChoosePixelFormatEx : Get Pixel format descriptor in order to request it
to windows
  ----------------------------------------------------------------------*/
int ChoosePixelFormatEx (HDC hdc)
{ 
  int wbpp = 32; 
  int wdepth = 16; 
  int wdbl = 1; 
  int wacc = 1; 
  PIXELFORMATDESCRIPTOR pfd; 
  int num;
  unsigned int maxqual=0; 
  int maxindex=0;
  int max_bpp, max_depth, max_dbl, max_acc;
  int i;
  int bpp;
  int depth;
  int pal, mcd, soft, icd,opengl, window, bitmap, dbuff;
  unsigned int q=0;

  ZeroMemory(&pfd,sizeof(pfd)); 
  pfd.nSize=sizeof(pfd); 
  pfd.nVersion=1;
  num=DescribePixelFormat(hdc,1,sizeof(pfd),&pfd);
  if (num==0) 
    return 0;
  for (i=1; i<=num; i++)
    { 
      ZeroMemory(&pfd,sizeof(pfd)); 
      pfd.nSize=sizeof(pfd); 
      pfd.nVersion=1;
      DescribePixelFormat(hdc,i,sizeof(pfd),&pfd);
      bpp=pfd.cColorBits;
      depth=pfd.cDepthBits;
      pal=(pfd.iPixelType==PFD_TYPE_COLORINDEX);
      mcd=((pfd.dwFlags & PFD_GENERIC_FORMAT) && (pfd.dwFlags & PFD_GENERIC_ACCELERATED));
      soft=((pfd.dwFlags & PFD_GENERIC_FORMAT) && !(pfd.dwFlags & PFD_GENERIC_ACCELERATED));
      icd=(!(pfd.dwFlags & PFD_GENERIC_FORMAT) && !(pfd.dwFlags & PFD_GENERIC_ACCELERATED));
      opengl=(pfd.dwFlags & PFD_SUPPORT_OPENGL);
      window=(pfd.dwFlags & PFD_DRAW_TO_WINDOW);
      bitmap=(pfd.dwFlags & PFD_DRAW_TO_BITMAP);
      dbuff=(pfd.dwFlags & PFD_DOUBLEBUFFER);
      if (opengl && window) 
	q=q+0x8000;
      if (wdepth==-1 || (wdepth>0 && depth>0)) 
	q=q+0x4000;
      if (wdbl==-1 || (wdbl==0 && !dbuff) || (wdbl==1 && dbuff)) 
	q=q+0x2000;
      if (wacc==-1 || (wacc==0 && soft) || (wacc==1 && (mcd || icd))) 
	q=q+0x1000;
      if (mcd || icd) 
	q=q+0x0040; 
      if (icd) 
	q=q+0x0002;
      if (wbpp==-1 || (wbpp==bpp)) 
	q=q+0x0800;
      if (bpp>=16) 
	q=q+0x0020; 
      if (bpp==16) 
	q=q+0x0008;
      if (wdepth==-1 || (wdepth==depth)) 
	q=q+0x0400;
      if (depth>=16) 
	q=q+0x0010; 
      if (depth==16) 
	q=q+0x0004;
      if (!pal) 
	q=q+0x0080;
      if (bitmap) 
	q=q+0x0001;
      if (q>maxqual) 
	{
	  maxqual=q; 
	  maxindex=i;
	  max_bpp=bpp; 
	  max_depth=depth; 
	  max_dbl=dbuff?1:0; 
	  max_acc=soft?0:1;
	}
    }
  if (maxindex==0) 
    return maxindex;
  return maxindex;
}
/*----------------------------------------------------------------------
  init_pfd : init the struct describing the screen we want
 ----------------------------------------------------------------------*/
void init_pfd ()
{
  static PIXELFORMATDESCRIPTOR myPFD;

  ZeroMemory(&myPFD, sizeof(myPFD));
  myPFD.nSize    = sizeof(myPFD);
  myPFD.nVersion = 1;
  myPFD.dwFlags  = PFD_DRAW_TO_WINDOW |
    PFD_SUPPORT_OPENGL |
    PFD_DOUBLEBUFFER;
  myPFD.cColorBits = 16;
  myPFD.cDepthBits = 16;
  myPFD.iPixelType = PFD_TYPE_RGBA;
  myPFD.iLayerType = PFD_MAIN_PLANE;

}

int ChoosePixelFormatWithout (HDC hdc, const PIXELFORMATDESCRIPTOR* ppfd,
DWORD dwRemove)
{
 PIXELFORMATDESCRIPTOR pfd;
 BOOL bSuccess;
 int nPixelIndex = 0, nBestPixelIndex = -1, nBestNotMatchedFlag =
0x7FFFFFFF, nNotMatchedFlag;
 const int
  nPixelTypeFailed = 0x40000000,   // 1 bit reserved
  nAccelTypeBad  = 0x10000000,   // 2 bits reserved
  nColorBitsFailed = 0x00800000,   // 5 bits reserved
  nStencilBitsFailed = 0x00040000,   // 5 bits reserved
  nDepthBitsFailed = 0x00002000,   // 5 bits reserved
  nAlphaBitsFailed = 0x00000400,   // 3 bits reserved
  nAccumBitsFailed = 0x00000010,   // 6 bits reserved
  nAuxBuffersFailed = 0x00000002,   // 3 bits reserved
  nLayerTypeFailed = 0x00000001;   // 1 bit reserved
 // Check for correct initialization
 if (ppfd->nSize != sizeof(PIXELFORMATDESCRIPTOR) || ppfd->nVersion != 1)
 {
  SetLastError(ERROR_SUCCESS);
  return 0;
 }
 // Iterate over all availible pixel formats
 while ((bSuccess = DescribePixelFormat(hdc, ++nPixelIndex,
    sizeof(PIXELFORMATDESCRIPTOR), &pfd)) != FALSE)
 {
  // take only pixel formats into account which satisfy the flags requested/not requested
  
  if (!(ppfd->dwFlags & PFD_DOUBLEBUFFER_DONTCARE))
  {
   if ((ppfd->dwFlags & PFD_DOUBLEBUFFER) && !(pfd.dwFlags &
PFD_DOUBLEBUFFER))
    continue;
   else if ((dwRemove & PFD_DOUBLEBUFFER) && (pfd.dwFlags &
PFD_DOUBLEBUFFER))
    continue;
  }
  
  if (!(ppfd->dwFlags & PFD_STEREO_DONTCARE))
  {
   if ((ppfd->dwFlags & PFD_STEREO) && !(pfd.dwFlags & PFD_STEREO))
    continue;
   else if ((dwRemove & PFD_STEREO) && (pfd.dwFlags & PFD_STEREO))
    continue;
  }
  
  if ((ppfd->dwFlags & PFD_DRAW_TO_WINDOW) && !(pfd.dwFlags &
PFD_DRAW_TO_WINDOW))
   continue;
  else if ((dwRemove & PFD_DRAW_TO_WINDOW) && (pfd.dwFlags &
PFD_DRAW_TO_WINDOW))
   continue;
  
  if ((ppfd->dwFlags & PFD_DRAW_TO_BITMAP) && !(pfd.dwFlags &
PFD_DRAW_TO_BITMAP))
   continue;
  else if ((dwRemove & PFD_DRAW_TO_BITMAP) && (pfd.dwFlags &
PFD_DRAW_TO_BITMAP))
   continue;
  
  if ((ppfd->dwFlags & PFD_SUPPORT_GDI) && !(pfd.dwFlags & PFD_SUPPORT_GDI))
   continue;
  else if ((dwRemove & PFD_SUPPORT_GDI) && (pfd.dwFlags & PFD_SUPPORT_GDI))
   continue;
  
  if ((ppfd->dwFlags & PFD_SUPPORT_OPENGL) && !(pfd.dwFlags &
PFD_SUPPORT_OPENGL))
   continue;
  else if ((dwRemove & PFD_SUPPORT_OPENGL) && (pfd.dwFlags &
PFD_SUPPORT_OPENGL))
   continue;
  
  if ((ppfd->dwFlags & PFD_GENERIC_FORMAT) && !(pfd.dwFlags &
PFD_GENERIC_FORMAT))
   continue;
  else if ((dwRemove & PFD_GENERIC_FORMAT) && (pfd.dwFlags &
PFD_GENERIC_FORMAT))
   continue;
  
  if ((ppfd->dwFlags & PFD_NEED_PALETTE) && !(pfd.dwFlags &
PFD_NEED_PALETTE))
   continue;
  else if ((dwRemove & PFD_NEED_PALETTE) && (pfd.dwFlags &
PFD_NEED_PALETTE))
   continue;
  
  if ((ppfd->dwFlags & PFD_NEED_SYSTEM_PALETTE) && !(pfd.dwFlags &
PFD_NEED_SYSTEM_PALETTE))
   continue;
  else if ((dwRemove & PFD_NEED_SYSTEM_PALETTE) && (pfd.dwFlags &
PFD_NEED_SYSTEM_PALETTE))
   continue;
  
  if ((ppfd->dwFlags & PFD_SWAP_EXCHANGE) && !(pfd.dwFlags &
PFD_SWAP_EXCHANGE))
   continue;
  else if ((dwRemove & PFD_SWAP_EXCHANGE) && (pfd.dwFlags &
PFD_SWAP_EXCHANGE))
   continue;
  
  if ((ppfd->dwFlags & PFD_SWAP_COPY) && !(pfd.dwFlags & PFD_SWAP_COPY))
   continue;
  else if ((dwRemove & PFD_SWAP_COPY) && (pfd.dwFlags & PFD_SWAP_COPY))
   continue;
 
  if ((ppfd->dwFlags & PFD_SWAP_LAYER_BUFFERS) && !(pfd.dwFlags &
PFD_SWAP_LAYER_BUFFERS))
   continue;
  else if ((dwRemove & PFD_SWAP_LAYER_BUFFERS) && (pfd.dwFlags &
PFD_SWAP_LAYER_BUFFERS))
   continue;
  
  if ((ppfd->dwFlags & PFD_GENERIC_ACCELERATED) && !(pfd.dwFlags &
PFD_GENERIC_ACCELERATED))
   continue;
  else if ((dwRemove & PFD_GENERIC_ACCELERATED) && (pfd.dwFlags &
PFD_GENERIC_ACCELERATED))
   continue;
  
  if ((ppfd->dwFlags & PFD_SUPPORT_DIRECTDRAW) && !(pfd.dwFlags &
PFD_SUPPORT_DIRECTDRAW))
   continue;
  else if ((dwRemove & PFD_SUPPORT_DIRECTDRAW) && (pfd.dwFlags &
PFD_SUPPORT_DIRECTDRAW))
   continue;
  // evaluate the other properties
  nNotMatchedFlag = 0;
  if (ppfd->iPixelType != pfd.iPixelType)
   nNotMatchedFlag |= nPixelTypeFailed;
  // ICD allowed, ...
  if (!(ppfd->dwFlags & PFD_GENERIC_FORMAT))
  {
   // ... but ICD not obtained
   if (pfd.dwFlags & PFD_GENERIC_FORMAT)
   {
    // Software obtained
    if ((pfd.dwFlags & PFD_GENERIC_FORMAT) && !(pfd.dwFlags &
PFD_GENERIC_ACCELERATED))
    {
     nNotMatchedFlag |= nAccelTypeBad * 2;
    }
    // MCD obtained
    else if ((pfd.dwFlags & PFD_GENERIC_FORMAT) && (pfd.dwFlags &
PFD_GENERIC_ACCELERATED))
    {
     nNotMatchedFlag |= nAccelTypeBad;
    }
   }
  }
  // ICD not allowed, but MCD allowed, ...
  else if (!(dwRemove & PFD_GENERIC_ACCELERATED))
  {
   // ... but MCD not obtained
   if (!(pfd.dwFlags & PFD_GENERIC_FORMAT) ||
    !(pfd.dwFlags & PFD_GENERIC_ACCELERATED))
   {
    // ICD obtained
    if (!(pfd.dwFlags & PFD_GENERIC_FORMAT))
    {
     nNotMatchedFlag |= nAccelTypeBad * 2;
    }
    // Software obtained
    else if ((pfd.dwFlags & PFD_GENERIC_FORMAT) && !(pfd.dwFlags &
PFD_GENERIC_ACCELERATED))
    {
     nNotMatchedFlag |= nAccelTypeBad;
    }
   }
  }
  // ICD not allowed and MCD not allowed, ...
  else
  {
   // ... but Software not obtained
   if ((!(pfd.dwFlags & PFD_GENERIC_FORMAT) ||
    (pfd.dwFlags & PFD_GENERIC_ACCELERATED)))
   {
    // ICD obtained
    if (!(pfd.dwFlags & PFD_GENERIC_FORMAT))
    {
     nNotMatchedFlag |= nAccelTypeBad * 2;
    }
    // MCD obtained
    else if ((pfd.dwFlags & PFD_GENERIC_FORMAT) && (pfd.dwFlags &
PFD_GENERIC_ACCELERATED))
    {
     nNotMatchedFlag |= nAccelTypeBad;
    }
   }
  }
  if (ppfd->cColorBits < pfd.cColorBits)
   nNotMatchedFlag |= nColorBitsFailed * (32-pfd.cColorBits);
  else if (ppfd->cColorBits > pfd.cColorBits)
   nNotMatchedFlag |= nColorBitsFailed * (32-pfd.cColorBits-1);
  if (ppfd->cAlphaBits < pfd.cAlphaBits)
   nNotMatchedFlag |= nAlphaBitsFailed * (8-pfd.cAlphaBits);
  else if (ppfd->cAlphaBits > pfd.cAlphaBits)
   nNotMatchedFlag |= nAlphaBitsFailed * (8-pfd.cAlphaBits-1);
  if (ppfd->cAccumBits < pfd.cAccumBits)
   nNotMatchedFlag |= nAccumBitsFailed * (64-pfd.cAccumBits);
  else if (ppfd->cAccumBits > pfd.cAccumBits)
   nNotMatchedFlag |= nAccumBitsFailed * (64-pfd.cAccumBits-1);
  if (ppfd->cDepthBits < pfd.cDepthBits)
   nNotMatchedFlag |= nDepthBitsFailed * (32-pfd.cDepthBits);
  else if (ppfd->cDepthBits > pfd.cDepthBits)
   nNotMatchedFlag |= nDepthBitsFailed * (32-pfd.cDepthBits-1);
  if (ppfd->cStencilBits < pfd.cStencilBits)
   nNotMatchedFlag |= nStencilBitsFailed * (32-pfd.cStencilBits);
  else if (ppfd->cStencilBits > pfd.cStencilBits)
   nNotMatchedFlag |= nStencilBitsFailed * (32-pfd.cStencilBits-1);
  if (ppfd->cAuxBuffers < pfd.cAuxBuffers)
   nNotMatchedFlag |= nAuxBuffersFailed * (8-pfd.cAuxBuffers);
  else if (ppfd->cAuxBuffers > pfd.cAuxBuffers)
   nNotMatchedFlag |= nAuxBuffersFailed * (8-pfd.cAuxBuffers-1);
  if (ppfd->iLayerType != pfd.iLayerType)
   nNotMatchedFlag |= nLayerTypeFailed;
  // choose the one with the best evaluation result
  if (nNotMatchedFlag < nBestNotMatchedFlag)
  {
   nBestPixelIndex = nPixelIndex;
   nBestNotMatchedFlag = nNotMatchedFlag;
  }
 }
 if (nBestPixelIndex == -1)
 {
  // no appropriate pixel format found, so let the API retry it
  return ChoosePixelFormat(hdc, ppfd);
 }
 else
 {
  // check for correct color depth
  DescribePixelFormat(hdc, nBestPixelIndex, sizeof(PIXELFORMATDESCRIPTOR), &pfd);
  if (pfd.cColorBits != GetDeviceCaps(hdc, PLANES) * GetDeviceCaps(hdc, BITSPIXEL))
   printf ("Warning: The requested pixel format does not satisfy the color depth of the device context.\n");
  // one based
  return nBestPixelIndex;
 }
}
/*----------------------------------------------------------------------
  GL_SetupPixelFormat : Sets up opengl buffers pixel format.
  Double Buffer, RGBA (32 bits), 
  no depth (z-buffer), no stencil (boolean buffer), no alpha (transparency), 
  no accum (special effect like multisampling, antialiasing),
  no aux (all purpose buffers),
  no pbuffers (?) buffers...
  ----------------------------------------------------------------------*/
void GL_SetupPixelFormat (HDC hDC)
{
  static PIXELFORMATDESCRIPTOR pfd = 
    {
      sizeof(PIXELFORMATDESCRIPTOR),  /* size */
      1,                              /* version */
      PFD_DRAW_TO_WINDOW |	      /* Format Must Support Window*/
      PFD_SUPPORT_OPENGL |	      /* Format Must Support OpenGL*/
      PFD_DOUBLEBUFFER  /* |    */    /* support double-buffering */
      /*PFD_DEPTH_DONTCARE |  */      /* If Depth is obligated by hardware*/
      /*PFD_GENERIC_ACCELERATED*/ ,   /* We try to get hardware here */       
      PFD_TYPE_RGBA,                  /* color type */
      32,                             /* prefered color depth */
      0, 0, 0, 0, 0, 0,               /* color bits (ignored) */
      1,                              /* alpha buffer */
      0,                              /* alpha bits (ignored) */
      0,                              /* no accumulation buffer */
      0, 0, 0, 0,                     /* accum bits (ignored) */
      0,                              /* depth buffer */
      1,                              /* stencil buffer */
      0,                              /* no auxiliary buffers */
      PFD_MAIN_PLANE,                 /* main layer */
      0,                              /* reserved */
      0, 0, 0,                        /* no layer, visible, damage masks */
    };
  int pixelFormat;	

  pixelFormat = ChoosePixelFormat (hDC, &pfd);
  if (pixelFormat == 0) 
    {
      MessageBox(WindowFromDC(hDC), "ChoosePixelFormat failed.", "Error",
		 MB_ICONERROR | MB_OK);
      exit(1);
    }

  if (SetPixelFormat(hDC, pixelFormat, &pfd) != TRUE) 
    {
      MessageBox(WindowFromDC(hDC), "SetPixelFormat failed.", "Error",
		 MB_ICONERROR | MB_OK);
      exit(1);
    }

  if ((pfd.dwFlags & PFD_GENERIC_ACCELERATED) != 0)
    SetSoftware_Mode (FALSE);/*MCD mini client driver*/
  else if ((pfd.dwFlags & PFD_GENERIC_FORMAT) != 0)
    SetSoftware_Mode (TRUE);/*software opengl*/
  else
    SetSoftware_Mode (FALSE);/*ICD installable client driver*/
}
#endif /*_WINGUI*/

/*----------------------------------------------------------------------
  GL_NotInFeedbackMode: if all openGL operations are permitted or not.
  ----------------------------------------------------------------------*/
ThotBool GL_NotInFeedbackMode ()
{
  return NotFeedBackMode;
}

/*----------------------------------------------------------------------
  GL_prepare: If a modif has been done
  ----------------------------------------------------------------------*/
ThotBool GL_prepare (int frame)
{  
  if (frame >= 0 && frame < MAX_FRAME && NotFeedBackMode)
    {
      //#ifdef _TESTSWAP
      //FrameTable[frame].DblBuffNeedSwap = TRUE;
      //#endif /*_TESTSWAP*/

    if (FrameTable[frame].WdFrame)
      return FrameTable[frame].WdFrame->SetCurrent();
    }
  return FALSE;
}

/*----------------------------------------------------------------------
  GL_Swap : swap frontbuffer with backbuffer (display changes)
  ----------------------------------------------------------------------*/
void GL_Swap (int frame)
{
  if (frame >= 0 && frame < MAX_FRAME &&
      ViewFrameTable[frame - 1].FrReady &&
      ViewFrameTable[frame - 1].FrAbstractBox &&
      FrameTable[frame].SwapOK &&
      FrameTable[frame].WdFrame && FrameTable[frame].FrDoc &&
      documentDisplayMode[FrameTable[frame].FrDoc - 1] == DisplayImmediately)
    {
      glDisable (GL_SCISSOR_TEST);
#ifdef _GL_DEBUG
      TTALOGDEBUG_1( TTA_LOG_DRAW, _T("GL_Swap: frame=%d"), frame );
#endif /* _GL_DEBUG */
#ifdef DEBUG_MAC
printf ("=====>Swap frame=%d\n",frame);
#endif /* DEBUG_MAC */
      FrameTable[frame].WdFrame->SwapBuffers();
      glEnable (GL_SCISSOR_TEST); 
      FrameTable[frame].DblBuffNeedSwap = FALSE;
    }
}

/*----------------------------------------------------------------------
  GL_SwapStop : Prevent savage swapping (causes flickering)
  ----------------------------------------------------------------------*/
void GL_SwapStop (int frame)
{
#ifdef DEBUG_MAC
printf ("************************ STOP frame=%d\n",frame);
#endif /* DEBUG_MAC */
  FrameTable[frame].SwapOK = FALSE;
}
/*----------------------------------------------------------------------
  GL_SwapGet : 
  ----------------------------------------------------------------------*/
ThotBool GL_SwapGet (int frame)
{
  return FrameTable[frame].SwapOK;
}

/*----------------------------------------------------------------------
  GL_SwapEnable : 
  ----------------------------------------------------------------------*/
void GL_SwapEnable (int frame)
{
#ifdef DEBUG_MAC
printf ("************************ ENABLE frame=%d\n",frame);
#endif /* DEBUG_MAC */
  FrameTable[frame].SwapOK = TRUE;
}
#endif /* _GL */

/*----------------------------------------------------------------------
  ComputeBoundingBox :
  Modify Bounding Box according to opengl feedback mechanism
  (after transformation, coordinates may have changed)			    
  ----------------------------------------------------------------------*/
void ComputeBoundingBox (PtrBox box, int frame, int xmin, int xmax, 
			 int ymin, int ymax)
{
#ifdef _GL
  GLfloat    feedBuffer[FEEDBUFFERSIZE];
  int        size;
  ViewFrame  *pFrame;
 
  if (NotFeedBackMode)
    {
      /* display into a temporary buffer */
      glFeedbackBuffer (FEEDBUFFERSIZE, GL_2D, feedBuffer);
      NotFeedBackMode = FALSE;  
      glRenderMode (GL_FEEDBACK);
      /* display the box with transformation and clipping */
      DisplayBox (box, frame, xmin, xmax, ymin, ymax, NULL, FALSE);
      size = glRenderMode (GL_RENDER);
      NotFeedBackMode = TRUE;
      if (size > 0)
        {
          /* the box is displayed */
          if (size > FEEDBUFFERSIZE)
            size = FEEDBUFFERSIZE;
          
          box->BxClipX = -1;
          box->BxClipY = -1;
          getboundingbox (size, feedBuffer, frame,
                          &box->BxClipX,
                          &box->BxClipY,
                          &box->BxClipW,
                          &box->BxClipH);    
          box->BxBoundinBoxComputed = TRUE; 
        }
      else
        {
          /* the box is not displayed */
          pFrame = &ViewFrameTable[frame - 1];
          /* */
          box->BxClipX = box->BxXOrg - (pFrame->FrXOrg?pFrame->FrXOrg:pFrame->OldFrXOrg);
          box->BxClipY = box->BxYOrg - (pFrame->FrYOrg?pFrame->FrYOrg:pFrame->OldFrYOrg);
          box->BxClipW = box->BxW;
          box->BxClipH = box->BxH;
          box->BxBoundinBoxComputed = FALSE; 
        }   
    }
#endif /* _GL */
}

#ifdef _GL
/*----------------------------------------------------------------------
  ComputeFilledBox :
  Modify Bounding Box according to opengl feedback mechanism
  (after transformation, coordinates may have changed)
  
  ----------------------------------------------------------------------*/
void ComputeFilledBox (PtrBox box, int frame, int xmin, int xmax,
                       int ymin, int ymax, ThotBool show_bgimage)
{
  GLfloat feedBuffer[4096];
  int     size;
  
  if (NotFeedBackMode)
    {
      box->BxBoundinBoxComputed = TRUE; 
      glFeedbackBuffer (2048, GL_2D, feedBuffer);
      NotFeedBackMode = FALSE;
      glRenderMode (GL_FEEDBACK);
      DrawFilledBox (box, box->BxAbstractBox, frame, NULL,
		     xmin, xmax, ymin, ymax, FALSE, TRUE, TRUE, show_bgimage);
      size = glRenderMode (GL_RENDER);
      NotFeedBackMode = TRUE;
      if (size > 0)
        {
          box->BxClipX = -1;
          box->BxClipY = -1;
          getboundingbox (size, feedBuffer, frame,
                          &box->BxClipX,
                          &box->BxClipY,
                          &box->BxClipW,
                          &box->BxClipH);     
          box->BxBoundinBoxComputed = TRUE; 
          /* printBuffer (size, feedBuffer); */
        }
    }
}

/*----------------------------------------------------------------------
  InitPrintBox :  	     
  ----------------------------------------------------------------------*/
void InitPrintBox ()
{
}

/*----------------------------------------------------------------------
  ClosePrintBox :  	     
  ----------------------------------------------------------------------*/
void FinishPrintBox ()
{
 
}   


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
int GLDrawPixelsPoscript (int width, int height,
			    int xorig, int yorig,
			    GLenum format, GLenum type, 
			    unsigned char *pixels, 
			    GLfloat x, GLfloat y)
{
return 0;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
int GLString (unsigned char *buff, int lg, int frame, int x, int y,
		ThotFont font, int boxWidth, int bl, int hyphen,
	      int startABlock, int fg)
{
return 0;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
int GLText (const char *str,
	    const int fg,
	    const void *font,
	    const unsigned int fontsize, 
	    const int x, 
	    const int y,
	    const int length)
{
return 0;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void GLPrintPostScriptColor (void *rgb)
{
}
#endif /* _GL */
