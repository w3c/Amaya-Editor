/* xdumpdrvr.h -- Definition  X11 XDump image driver */


#ifndef XDumpDrvr_h
#define XDumpDrvr_h

#define XDumpName "XDump" /* ok ? or something else ? We need this
                               to name the format at the UI level... */

extern int XDumpOpenImageDrvr();
extern void XDumpCloseImageDrvr();
extern void XDumpInitImage();
extern Drawable XDumpReadImage();
extern void XDumpDrawImage();
extern Drawable XDumpCreateImage();
extern void XDumpEditImage();
extern void XDumpPrintImage();
extern Bool XDumpIsFormat();
extern ImageInfo* XDumpGetImageInfo();
extern void XDumpEscape();

#endif
