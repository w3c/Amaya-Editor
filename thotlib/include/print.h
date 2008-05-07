/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2005
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

#ifndef _PRINT_H_
#define _PRINT_H_
typedef int PrintParameter;
#define PP_FirstPage     0
#define PP_LastPage      1
#define PP_Scale         2
#define PP_NumberCopies  3
#define PP_ManualFeed    4
#define PP_PagesPerSheet 5
#define PP_PaperSize     6
#define PP_Destination   7
#define PP_Paginate      8
#define PP_Orientation   9

#define PP_ON 1
#define PP_OFF 0
#define PP_A4 0
#define PP_US 1
#define PP_PRINTER 0
#define PP_PS 1
#define PP_Portrait 0
#define PP_Landscape 1

#include "appstruct.h"
#include "document.h"

#ifndef __CEXTRACT__
extern void TtaGetPrintNames (char **printDocName, char **printDirName);
extern void TtaPrint (Document document, char *viewNames, char *cssNames);
extern void TtaSetPrintExportFunc (Func exportFunc);
extern void TtaSetPrintParameter (PrintParameter parameter, int value);
extern int TtaGetPrintParameter (PrintParameter parameter);
extern void TtaSetPrintCommand (const char *command);
extern void TtaSetPsFile (const char *path);
extern void TtaSetPrintSchema (const char *name);

#endif /* __CEXTRACT__ */

#endif
