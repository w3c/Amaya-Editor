/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 
#ifndef __PRINT_V_H__
#define __PRINT_V_H__

THOT_EXPORT int	    numOfJobs;
THOT_EXPORT PathBuffer   PSdir;
THOT_EXPORT ThotBool     PaperPrint;
THOT_EXPORT ThotBool     ManualFeed;
THOT_EXPORT ThotBool     NewPaperPrint;
THOT_EXPORT char         pPrinter[MAX_PATH];
THOT_EXPORT Document     PrintingDoc;
THOT_EXPORT char         PageSize[MAX_NAME_LENGTH];
THOT_EXPORT int	         FirstPage;
THOT_EXPORT int          LastPage;
THOT_EXPORT int	         NbCopies;
THOT_EXPORT int          Reduction;
THOT_EXPORT int          PagesPerSheet;
THOT_EXPORT int	         Paginate;

#endif /* __PRINT_V_H__ */
