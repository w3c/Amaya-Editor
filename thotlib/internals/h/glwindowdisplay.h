
#ifndef _GLWINDOWDISPLAY_H_
#define _GLWINDOWDISPLAY_H_

#ifdef _WINDOWS

void GL_Win32ContextClose (int frame, HWND hwndClient);
void GL_Win32ContextInit (HWND hwndClient, int frame);

typedef struct _XSegment {
	int x1;
	int y1;
	int x2;
	int y2;		
} XSegment;

typedef struct _XArc {
	int x;
	int y;
	int width;
	int height;		
	int angle1;
	int angle2;
} XArc;

#endif /*_WINDOWS*/

#ifdef _GTK

gboolean GL_DrawCallback (ThotWidget widget, 
			  GdkEventExpose *event, 
			  gpointer data);
gboolean  GL_Init (ThotWidget widget, 
		   GdkEventExpose *event, 
		   gpointer data);
gboolean GL_FocusIn (ThotWidget widget, 
		     GdkEventExpose *event, 
		     gpointer data);
gboolean GL_FocusOut (ThotWidget widget, 
		      GdkEventExpose *event, 
		      gpointer data);
gboolean  GL_Destroy (ThotWidget widget, 
		      GdkEventExpose *event, 
		      gpointer data);
#endif /*_GTK*/


int glMatroxBUG (int frame, int x, int y, int width, int height);

/* Drawing */
void InitDrawing (int style, int thick, int fg);


void GL_Point (int fg, float width, float x, float y);

void GL_DrawLine (int x1, int y1, int x2, int y2, ThotBool Round);
void GL_DrawLines (ThotPoint *point, int npoints);

void FDrawRectangle (int frame, int thick, int style, float x, float y, float width,
		     float height, int fg, int bg, int pattern);

void GL_DrawEmptyRectangle (int fg, int x, int y, int width, int height);
void GL_DrawRectangle (int fg, int x, int y, int width, int height);

void GL_DrawEmptyRectanglef (int fg, float x, float y, float width, float height);
void GL_DrawRectanglef (int fg, float x, float y, float width, float height);

void GL_DrawSegments (XSegment *point, int npoints);
void GL_DrawArc (int x, int y, int w, int h, int angle1, int angle2, ThotBool filled);

void GL_DrawPolygon (ThotPoint *points, int npoints);

void GL_VideoInvert (int width, int height, int x, int y);


/*Transformations...*/
void DisplayTransformation (PtrTransform Trans, int Width, int Height);
void DisplayTransformationExit ();

void ComputeBoundingBox (PtrBox box, int frame, int xmin, int xmax, int ymin, int ymax);
void ComputeFilledBox (PtrBox box, int frame, int xmin, int xmax, int ymin, int ymax);

/*Text rendering*/
void TranslateChars (CHAR_T *text);
int UnicodeCharacterWidth (CHAR_T c, PtrFont font);
int GL_UnicodeDrawString (int fg, 
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
void SetGlPipelineState ();
void GLResize (int width, int height, int x, int y);
void gl_window_resize (int frame, int width, int height);
void GL_SetForeground (int fg);
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
void  GL_DestroyFrame (int frame);

void GL_UnsetClipping (int x, int y, int width, int height);
void GL_UnsetClippingRestore (ThotBool Restore);
void GL_SetClipping (int x, int y, int width, int height);
void GL_GetCurrentClipping (int *x, int *y, int *width, int *height);

void DisplayBoxTransformation (void *v_trans, int x, int y);
ThotBool IsTransformed (void *v_trans);
ThotBool GetBoxTransformed (void *v_trans, int *x, int *y);

void gl_synchronize ();

void GL_SwapStop (int frame);
void GL_SwapEnable (int frame);

/* Picture */
void GL_TextureMap (void *Image, 
		    int xFrame, int yFrame, 
		    int w, int h);
void *Group_shot (int x, int y, 
		  int width, int height, int frame, ThotBool is_rgb);
void GL_SetPicForeground ();
void ResetMainWindowBackgroundColor (int frame);
void FreeGlTexture (void *Image);
void FreeGlTextureNoCache (void *Image);
unsigned char *test_gradien_linear (); 

#ifndef _NOSHARELIST
int GetSharedContext ();
void SetSharedContext (int frame);
#endif /* _SHARELIST */
#endif /*_GLWINDOWDISPLAY_H_*/
