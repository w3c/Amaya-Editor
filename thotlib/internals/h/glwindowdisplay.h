
#ifndef _GLWINDOWDISPLAY_H_
#define _GLWINDOWDISPLAY_H_

#ifdef _WINGUI

#pragma warning (disable: 4786)
#pragma warning (disable: 4788)
#pragma warning (disable: 4244)

void GL_Win32ContextClose (int frame, HWND hwndClient);
void GL_Win32ContextInit (HWND hwndClient, int frame);

void SetupPixelFormatPrintGL (HDC hDC, int frame);
void initwgl (HDC hDC, int frame);
void closewgl (HDC hDC, int frame);
void GL_SetupPixelFormat (HDC hDC);

#endif /*_WINGUI*/

/* Drawing */
void InitDrawing (int style, int thick, int fg);

void Clear (int frame, int width, int height, int x, int y);
void Scroll (int frame, int width, int height, int xd, int yd, int xf, int yf);

void DrawChar (CHAR_T car, int frame, int x, int y, ThotFont font, int fg);

void DrawResizeTriangle (int frame, int size, int x_point, int y_point,
                         int bg, int fg, int direction);

void GL_Point (int fg, float width, float x, float y);

void GL_DrawLine (int x1, int y1, int x2, int y2, ThotBool round);
void GL_DrawLines (ThotPoint *points, int npoints);

void GL_DrawEmptyRectangle (int fg, 
			    float x, float y, 
			    float width, float height, 
			    float thick);

void GL_DrawRectangle (int fg, float x, float y, float width, float height);

void GL_DrawSegments (ThotSegment *point, int npoints);
void GL_DrawArc (float x, float y, float w, float h, 
                 int angle1, int angle2, int mode, ThotBool filled);

void GL_DrawPolygon (ThotPoint *points, int npoints, int mode);

void GL_VideoInvert (int width, int height, int x, int y);


/*Transformations...*/
void DisplayTransformation (int frame, PtrTransform Trans, int Width, int Height);
void DisplayTransformationExit ();

void ComputeBoundingBox (PtrBox box, int frame, int xmin, int xmax, int ymin, int ymax);
void ComputeFilledBox (PtrBox box, int frame, int xmin, int xmax, int ymin,
                       int ymax, ThotBool show_bgimage);

/*Text rendering*/
/*void TranslateChars (CHAR_T *text);*/
int UnicodeCharacterWidth (CHAR_T c, ThotFont font);
int GL_DrawString (int fg, 
		   CHAR_T *str, 
		   float x, float y, 
		   int hyphen,
		   void *GL_font, int end);
void GL_DrawUnicodeChar (CHAR_T const c, 
			 float x, float y, 
			 void *GL_font, int fg);

/* GL specific */
ThotBool glhard ();
ThotBool GL_NotInFeedbackMode ();
void GL_Swap (int frame);
ThotBool GL_SwapGet (int frame);
void SetGlPipelineState ();
void GLResize (int width, int height, int x, int y);
void gl_window_resize (int frame, int width, int height);
void GL_SetForeground (int fg, ThotBool fillstyle);
void GL_SetBackground (int bg, int frame);
void GL_ClearArea (int x, int y, int width, int height);
void GL_window_copy_area (int frame, int xf, int yf, int xd, int yd,
			  int width, int height);
void GL_BackBufferRegionSwapping (int x, int y, int width, int height, int Totalheight);
void GL_SetOpacity (int opacity);
void GL_SetStrokeOpacity (int opacity);
void GL_SetFillOpacity (int opacity);


void GL_ActivateDrawing();
ThotBool GL_DrawAll ();
void GL_DestroyFrame (int frame);
void ClearAll(int frame);
ThotBool GL_prepare (int frame);
void GL_realize (int frame);
ThotBool SavePng (const char *filename, 
		 unsigned char *m_pData,
		 unsigned int m_width,
		  unsigned int m_height);
void saveBuffer (char *filename, int width, int height);
void GL_UnsetClipping ();
void GL_SetClipping (int x, int y, int width, int height);
void GL_GetCurrentClipping (int *x, int *y, int *width, int *height);

void DisplayBoxTransformation (void *v_trans, int x, int y);

ThotBool IsTransformed (void *v_trans);
ThotBool IsBoxTransformed (PtrBox box);

ThotBool GetBoxTransformed (void *v_trans, int *x, int *y);

ThotBool IsDeformed (void *v_trans);
ThotBool IsBoxDeformed (PtrBox box);

void gl_synchronize ();

void GL_SwapStop (int frame);
void GL_SwapEnable (int frame);

/* Picture */
void *Group_shot (int x, int y, 
		  int width, int height, int frame, ThotBool is_rgb);
void GL_SetPicForeground ();
void ResetMainWindowBackgroundColor (int frame);
void FreeGlTexture (void *Image);
void FreeGlTextureNoCache (void *Image);
unsigned char *test_gradien_linear (); 
ThotBool GL_TransText ();
void GL_SetTransText (ThotBool value);

/*--------- PRINT------------- */
void PrintBoxes (int frame, int xmin, int xmax, int ymin, int ymax,
		 PtrAbstractBox pInitAb);

void PrintBox (PtrBox box, int frame, int xmin, int xmax, int ymin, int ymax);
ThotBool GL_Printing ();

void TtaChangePlay (int frame, ThotBool status);
double ComputeThotCurrentTime (int frame);
ThotBool GL_Err ();
void update_bg_colorGTK (int frame, int color);
void getboundingbox (int size, float *buffer, int frame,
		     int *xorig, int *yorig, 
		     int *worig, int *horig);

void GL_SetPrintForeground (int fg);
void SetSoftware_Mode (ThotBool value);

void SetBadCard (ThotBool badbuffer);
ThotBool GetBadCard ();

#ifdef _WINGUI
void WinGL_Swap (HDC hDC);
#endif /*_WINGUI*/

#ifndef _NOSHARELIST
int GetSharedContext ();
void SetSharedContext (int frame);
#endif /* _NOSHARELIST */

void GetGLContext ();

#endif /*_GLWINDOWDISPLAY_H_*/
