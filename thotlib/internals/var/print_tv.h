/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 
#ifndef __PRINT_V_H__
#define __PRINT_V_H__

EXPORT int	    numOfJobs;
EXPORT PathBuffer   PSdir;
EXPORT boolean      PaperPrint;
EXPORT boolean      ManualFeed;
EXPORT boolean      NewPaperPrint;
EXPORT char         pPrinter[MAX_NAME_LENGTH];
EXPORT PtrDocument  pDocPrint;
EXPORT char         PageSize[MAX_NAME_LENGTH];
EXPORT int	    FirstPage;
EXPORT int	    LastPage;
EXPORT int	    NbCopies;
EXPORT int	    Reduction;
EXPORT int	    PagesPerSheet;

#endif /* __PRINT_V_H__ */
