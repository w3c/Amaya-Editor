/* CPP main program, using CPP Library.
   Copyright (C) 1995 Free Software Foundation, Inc.
   Written by Per Bothner, 1994-95.

This program is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 2, or (at your option) any
later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

 In other words, you are welcome to use, share and improve this program.
 You are forbidden to forbid anyone else to use, share and improve
 what you give them.   Help stamp out software-hoarding!  */

#include "cpplib.h"
#include <stdio.h>

#ifdef _WINDOWS
#include <windows.h>
#include "compilers_f.h"
#endif /* _WINDOWS */
#include "config.h"

extern char *getenv ();

char *progname;

cpp_reader parse_in;
cpp_options options;

/* More 'friendly' abort that prints the line and file.
   config.h can #define abort fancy_abort if you like that sort of thing.  */

#ifdef _WINDOWS
HWND hWND;
int  _CY_;
void CPPError (HWND hwnd, char* errorMsg)
{
   TEXTMETRIC  textMetric;
   COLORREF    oldColor;
   HFONT       hFont, hOldFont;
   HDC         hDC;
   int         cxChar, cyChar;

   if (hWND) {
      hDC = GetDC (hWND);
      /* hFont = CreateFont (16, 0, 0, 0, FW_NORMAL, TRUE, FALSE, FALSE, ANSI_CHARSET, 
                          OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, 
                          DEFAULT_PITCH | FF_DONTCARE, "Times New Roman"); */
      hFont = CreateFont (16, 0, 0, 0, FW_BOLD, TRUE, FALSE, FALSE, ANSI_CHARSET, 
                          OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, 
                          DEFAULT_PITCH | FF_DONTCARE, "Arial");

      hOldFont = SelectObject (hDC, hFont);

      GetTextMetrics (hDC, &textMetric);
      cxChar = textMetric.tmAveCharWidth;
      cyChar = textMetric.tmHeight + textMetric.tmExternalLeading + 1;

      if (_CY_ >= 550) {
         ScrollWindow (hwnd, 0, -cyChar, NULL, NULL);
         UpdateWindow (hwnd);
      } else
           _CY_ += cyChar;

      oldColor = SetTextColor (hDC, RGB (180, 0, 0));

      if (!TextOut (hDC, 5, _CY_, errorMsg, strlen (errorMsg)))
         MessageBox (NULL, "Error Writing text", "Thot Compilers", MB_OK);

      SetTextColor (hDC, oldColor);

      SelectObject (hDC, hOldFont);
	  DeleteObject (hFont);
      ReleaseDC (hWND, hDC);
   } 
}
#endif /* _WINDOWS */

void fancy_abort ()
{
     fatal ("Internal gcc abort.");
} 

int CPPmain (HWND hwnd, int argc, char **argv, int *Y)
{
   char *p;
   int i;
   int argi = 1;  /* Next argument to handle. */
   struct cpp_options *opts = &options; 
   FILE* outfile;

   hWND = hwnd;
   _CY_ = *Y;

   p = argv[0] + strlen (argv[0]);
   while (p != argv[0] && p[-1] != '/') --p;
   progname = p;

   init_parse_file (&parse_in);
   parse_in.data = opts;

   init_parse_options (opts);
  
   argi += cpp_handle_options (&parse_in, argc - argi , argv + argi);
   if (argi < argc)
      fatal ("Invalid option `%s'", argv[argi]);
   parse_in.show_column = 1;

   i = push_parse_file (&parse_in, opts->in_fname);
   if (i != SUCCESS_EXIT_CODE)
      return i;

   /* Now that we know the input file is valid, open the output.  */

   if (!opts->out_fname || !strcmp (opts->out_fname, ""))
      opts->out_fname = "stdout";
   else if (!(outfile = fopen (opts->out_fname, "w")))
        cpp_pfatal_with_name (&parse_in, opts->out_fname);

   for (;;) {
       enum cpp_token kind;
       if (! opts->no_output) {
          fwrite (parse_in.token_buffer, 1, CPP_WRITTEN (&parse_in), outfile);
	   }
       parse_in.limit = parse_in.token_buffer;
       kind = cpp_get_token (&parse_in);
       if (kind == CPP_EOF)
          break;
   } 

   fclose (outfile);
   cpp_finish (&parse_in);

   *Y = _CY_;

   if (parse_in.errors)
      return (FATAL_EXIT_CODE);
   return (SUCCESS_EXIT_CODE);
} 
