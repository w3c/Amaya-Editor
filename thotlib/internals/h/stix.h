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
int GetMathFontWidth (char shape,
		      int size,
		      int height);

void GiveStixSize (ThotFont font, PtrAbstractBox pAb, 
		   int *width, int *height, int size);


void DrawCenteredStixChar (ThotFont font, unsigned char symb, 
		   int x, int y, 
		   int l, int h, 
		   int fg, int frame);
void DrawStixIntegral (int frame, int x, int y, int l, int h,
		       int type, int size, int fg);
void DrawStixBracket (int frame, int x, int y, int l, int h,
		      int direction, int size, int fg);
void DrawStixPointyBracket (int frame, int x, int y, int l, int h,
			    int direction, int size, int fg);
void DrawStixParenthesis (int frame, int x, int y, int l, int h,
			  int direction, int size, int fg);
void DrawStixBrace (int frame, int x, int y, int l, int h,
		    int direction, int size, int fg);
void DrawStixSigma (int frame, int x, int y, 
		    int l, int h, 
		    ThotFont font, int fg);


int SizetoLogical (int size);

#endif /* __STIX_*/
