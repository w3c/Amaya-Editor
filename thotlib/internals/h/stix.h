#ifndef __STIX_
#define __STIX_

void GetMathFontFromChar (char typesymb, void **font, int height);
void DrawStixChar (PtrFont font, CHAR_T symb, 
		   int x, int y, 
		   int l, int h, 
		   int fg, int frame);
void DrawStixIntegral (int frame, int thick, int x, int y, int l, int h,
		       int type, PtrFont font, int fg);
void DrawStixBracket (int frame, int thick, int x, int y, int l, int h,
		      int direction, PtrFont font, int fg);
void DrawStixPointyBracket (int frame, int thick, int x, int y, int l, int h,
			    int direction, PtrFont font, int fg);
void DrawStixParenthesis (int frame, int thick, int x, int y, int l, int h,
			  int direction, PtrFont font, int fg);
void DrawStixBrace (int frame, int thick, int x, int y, int l, int h,
		    int direction, PtrFont font, int fg);
#endif /* __STIX_*/
