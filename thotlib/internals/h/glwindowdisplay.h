
void SetGlPipelineState();
void GLResize (int width, int height);
void GL_SetForeground (int fg);
void GL_SetBackground (int bg, int frame);
void GL_ClearArea (int x, int y, int width, int height);
void GL_window_copy_area (int xf, int yf, int xd, int yd,
			  int width, int height);
void GL_BackBufferRegionSwapping (int x, int y, int width, int height, int Totalheight);

void GL_Point (int fg, float width, float x, float y);
void GL_DrawLine (int x1, int y1, int x2, int y2);
void GL_DrawEmptyRectangle (int fg, int x, int y, int width, int height);
void GL_DrawRectangle (int fg, int x, int y, int width, int height);
void GL_DrawSegments (XSegment *point, int npoints);
void GL_DrawArc (int x, int y, int w, int h, int angle1, int angle2, ThotBool filled);
void GL_DrawPolygon (ThotPoint *points, int npoints);
void CountourCountReset ();
void CountourCountAdd (int npoints);


void InitDrawing (int style, int thick, int fg);

int GL_UnicodeDrawString (int fg, 
			  CHAR_T *str, 
			  float x, float y, 
			  int hyphen,
			  void *GL_font, int end);
void GL_DrawUnicodeChar (CHAR_T const c, 
			 float x, float y, 
			 void *GL_font, int fg);

int GL_DrawString (char const *str, float x, float y, 
		   void *GL_font, int fg);
void GL_DrawChar (char const c, float x, float y, void *GL_font, int fg);


void GL_ActivateDrawing();
void GL_DrawAll (ThotWidget widget, int frame);

ThotBool GL_prepare (GtkWidget *widget);
void GL_realize (GtkWidget *widget);
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
