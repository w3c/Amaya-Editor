/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

char               *SysErrorTmpl = "<HTML><HEAD>ERROR</HEAD><BODY>"
"<TITLE>ERROR: The requested URL could not be retrieved</TITLE>\n"
"<H2>The requested URL could not be retrieved</H2>\n"
"<HR>\n"
"<P>\n"
"While trying to retrieve the URL:\n"
"<A HREF=\"%s\">%s</A>\n"
"<P>\n"
"The following error was encountered:\n"
"<UL>\n"
"<LI><STRONG>Connection Failed</STRONG>\n"
"</UL>\n"
"<P>The system returned:\n"
"<PRE><I>(%d) %s</I></PRE>\n"
"<P>This means that:\n"
"<PRE>\n"
"The remote site or server may be down or non-existant.\n"
"Please try again soon.\n"
"</PRE>\n"
"<P> <HR>\n</BODY></HTML>";
