int gl_font_char_width (void *gl_void_font, const char *c);
int gl_font_char_height (void *gl_void_font, const char *c);
int gl_font_char_ascent (void *gl_void_font, const char *c);
int gl_font_ascent (void *gl_void_font);
int gl_font_height (void *gl_void_font);
void gl_draw_text (void *gl_void_font, const char *str);
void *gl_font_init (const char *font_filename, char alphabet, int size);
void gl_font_delete (void *gl_font);
int GetFontFilename(const char *xlfd, char *filename);
