#ifndef __STIX_
#define __STIX_
int GetStixFontAndIndex (int c, SpecFont fontset, ThotFont **font);

void *LoadStixFont (int family, int size);

void GetMathFontFromChar (char typesymb,
			  SpecFont fontset,
			  void **font,
			  int height);

int GetFontandIndexFromGreekChar (char  *c,
				  void  **font,
				  char  *code,
				  int  *encoding,
				  int size);
int GetMathFontWidth (SpecFont fontset,
		      char shape,
		      int size,
		      int height);

void GiveStixSize (ThotFont font, PtrAbstractBox pAb, 
		   int *width, int *height, int size);


void DrawStixChar (ThotFont font, unsigned char symb, 
		   int x, int y, 
		   int l, int h, 
		   int fg, int frame);
void DrawStixIntegral (int frame, int thick, int x, int y, int l, int h,
		       int type, ThotFont font, int fg);
void DrawStixBracket (int frame, int thick, int x, int y, int l, int h,
		      int direction, ThotFont font, int fg);
void DrawStixPointyBracket (int frame, int thick, int x, int y, int l, int h,
			    int direction, ThotFont font, int fg);
void DrawStixParenthesis (int frame, int thick, int x, int y, int l, int h,
			  int direction, ThotFont font, int fg);
void DrawStixBrace (int frame, int thick, int x, int y, int l, int h,
		    int direction, ThotFont font, int fg);
void DrawStixSigma (int frame, int x, int y, 
		    int l, int h, 
		    ThotFont font, int fg);


int SizetoLogical (int size);

#endif /* __STIX_*/
