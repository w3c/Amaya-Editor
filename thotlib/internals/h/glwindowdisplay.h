

#ifdef _WINDOWS

void GL_Win32ContextClose (int frame);
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
void GL_Swap (int frame);
void GL_MakeCurrent (int frame);
void SetGlPipelineState ();
void GLResize (int width, int height, int x, int y);
void GL_SetForeground (int fg);
void GL_SetBackground (int bg, int frame);
void GL_ClearArea (int x, int y, int width, int height);
void GL_window_copy_area (int frame, int xf, int yf, int xd, int yd,
			  int width, int height);
void GL_BackBufferRegionSwapping (int x, int y, int width, int height, int Totalheight);

void GL_Point (int fg, float width, float x, float y);
void GL_DrawLine (int x1, int y1, int x2, int y2);
void GL_DrawLines (ThotPoint *point, int npoints);

void GL_DrawEmptyRectangle (int fg, int x, int y, int width, int height);
void GL_DrawRectangle (int fg, int x, int y, int width, int height);
void GL_DrawSegments (XSegment *point, int npoints);
void GL_DrawArc (int x, int y, int w, int h, int angle1, int angle2, ThotBool filled);
void GL_DrawPolygon (ThotPoint *points, int npoints);
void GL_VideoInvert (int width, int height, int x, int y);
void CountourCountReset ();
void CountourCountAdd (int npoints);


void InitDrawing (int style, int thick, int fg);

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



void GL_ActivateDrawing();
void GL_DrawAll (ThotWidget widget, int frame);


ThotBool GL_prepare (ThotWidget *widget);
void GL_realize ();

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
