/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * Warning:
 * This module is part of the Thot library, which was originally
 * developed in French. That's why some comments are still in
 * French, but their translation is in progress and the full module
 * will be available in English in the next release.
 * 
 */
 
#ifndef _PRINT_H_
#define _PRINT_H_
typedef int PrintParameter;
#define PP_FirstPage 0
#define PP_LastPage 1
#define PP_Scale 2
#define PP_NumberCopies 3
#define PP_ManualFeed 4
#define PP_PagesPerSheet 5
#define PP_PaperSize 6
#define PP_Destination 7
#define PP_Paginate 8
#define PP_ON 1
#define PP_OFF 0
#define PP_A4 0
#define PP_US 1
#define PP_PRINTER 0
#define PP_PS 1

#include "appstruct.h"
#include "document.h"

#ifndef __CEXTRACT__
#ifdef __STDC__
extern void TtaGetPrintNames (STRING *printDocName, STRING *printDirName);
extern void TtaPrint (Document document, STRING viewNames, STRING cssNames);
extern void TtaSetPrintExportFunc (Func exportFunc);
extern void TtaSetPrintParameter (PrintParameter parameter, int value);
extern int TtaGetPrintParameter (PrintParameter parameter);
extern void TtaSetPrintCommand (STRING command);
extern void TtaGetPrintCommand (STRING command);
extern void TtaSetPsFile (STRING path);
extern void TtaGetPsFile (STRING path);
extern void TtaSetPrintSchema (CHAR_T* name);

#else /* __STDC__ */
extern void TtaGetPrintNames (/*STRING *printDocName, STRING *printDirName*/);
extern void TtaPrint(/*Document document, STRINGviewNames, STRING cssNames)*/);
extern void TtaSetPrintExportFunc (/*Func exportFunc*/);
extern void TtaSetPrintParameter (/*PrintParameter parameter, int value*/);
extern void TtaGetPrintParameter (/*PrintParameter parameter*/);
extern void TtaSetPrintCommand (/*STRING command*/);
extern void TtaGetPrintCommand (/*STRING command*/);
extern void TtaSetPsFile (/*STRING path*/);
extern void TtaGetPsFile (/*STRING path*/);
extern void TtaSetPrintSchema (/*CHAR_T* name*/);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */

#endif
