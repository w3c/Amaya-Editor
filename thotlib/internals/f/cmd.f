
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__
extern void TtcPreviousChar(Document document, View view);
extern void TtcNextChar(Document document, View view);
extern void TtcPreviousLine(Document document, View view);
extern void TtcNextLine(Document document, View view);
extern void TtcStartOfLine(Document document, View view);
extern void TtcEndOfLine(Document document, View view);
extern void TtcCopyToClipboard (Document document, View view);

extern int ConvBufInd(PtrTextBuffer bu, int ind);
extern void FinInsert ( void );
extern void EmacsCommand ( int code );
extern int CopierXBuffer ( unsigned char **buffer );

#else /* __STDC__ */

extern void TtcPreviousChar(/*Document document, View view*/);
extern void TtcNextChar(/*Document document, View view*/);
extern void TtcPreviousLine(/*Document document, View view*/);
extern void TtcNextLine(/*Document document, View view*/);
extern void TtcStartOfLine(/*Document document, View view*/);
extern void TtcEndOfLine(/*Document document, View view*/);
extern void TtcCopyToClipboard (/*Document document, View view*/);

extern int ConvBufInd(/*PtrTextBuffer bu, int ind*/);
extern void FinInsert (/* void */);
extern void EmacsCommand (/* int code */);
extern int CopierXBuffer (/* unsigned char **buffer */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
