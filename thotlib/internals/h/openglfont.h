int   gl_font_char_width  (void *gl_void_font, wchar_t c);
int   gl_font_char_height (void *gl_void_font, wchar_t *c);
int   gl_font_char_ascent (void *gl_void_font, wchar_t *c);

int   UnicodeFontRender   (void *gl_font, wchar_t *string, float x, float y, 
			   int size, int TotalHeight);

int   gl_font_ascent      (void *gl_void_font);
int   gl_font_height      (void *gl_void_font);
void *gl_font_init        (const char *font_filename, char alphabet, int size);
void  gl_font_delete      (void *gl_font);

int   GetFontFilename     (char script, int family, 
			   int highlight, int size, 
			   int UseLucidaFamily, int UseAdobeFamily,
			   char *filename);
