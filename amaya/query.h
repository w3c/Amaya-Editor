/*                                                                 Public QUERY
OBJECT PUBLIC
                          PUBLIC DECLARATION OF QUERY MODULE

 */
/*
**  Copyright (c) 1994-1995 Inria/CNRS  All rights reserved. 
**      Please first read the full copyright statement in the file COPYRIGH.
*/
/*
	Module comment
 */
#ifndef QUERY_H
#define QUERY_H



/* AHTLibWWW includes */

#include "AHTCommon.h"

/**** Global variables ****/

extern HTList    *conv;      /* List of global converters */

extern AmayaContext *Amaya;             /* Amaya's global context */


/**** Function prototypes ****/

#ifdef __STDC__
extern void QueryInit();
#else
extern void QueryInit();
#endif /*__STDC__*/


#ifdef __STDC__
extern void QueryClose();
#else
extern void QueryClose();
#endif /*__STDC__*/

#ifdef __STDC__
extern int GetObjectWWW(int docid, char *urlName, char *outputfile, int mode, TIcbf * incremental_cbf, void *context_icbf, TTcbf * terminate_cbf, void *context_tcbf, BOOL error_html);
#else
extern int GetObjectWWW( /*int docid, char *urlName, char *outputfile, int mode, TIcbf *incremental_cbf, void *context_icbf, TTcbf *terminate_cbf, void *context_tcbf, BOOL error_html*/);
#endif

#ifdef __STDC__
extern int PutObjectWWW(int docid, char *fileName, char *urlName, int mode,
                 TTcbf * terminate_cbf, void *context_tcbf);
#else
extern int PutObjectWWW(/*int docid, char *fileName, char *urlName, int mode,
                        TTcbf * terminate_cbf, void *context_tcbf*/);
#endif /* _STDC_ */

#ifdef __STDC__
extern int UploadMemWWW(int docid, HTMethod method, char *mem_ptr, 
			unsigned long block_size, 
			char *urlName, int mode,
			TTcbf * terminate_cbf, void *context_tcbf,
			char *output_file);
#else                           /* __STDC__ */
extern int PutMemWWW(/*int docid, method char *mem_ptr, unsigned long block_size,
		char *urlName, int mode,
		TTcbf * terminate_cbf, void *context_tcbf, 
		char *output_file*/);
#endif                           /* __STDC__ */

#ifdef __STDC__
extern AHTDocId_Status * GetDocIdStatus (int docid, HTList *documents);
#else /* __STDC__ */
extern AHTDocID_Status * GetDocIdStatus (/* int docid,  HTList *documents*/);
#endif

#ifdef __STDC__
extern BOOL AHTReqContext_delete (AHTReqContext * me);
#else /* __STDC__ */
extern BOOL AHTReqContext_delete (/*AHTReqContext * me*/);
AHTReqContext * me;
#endif /*__STDC__*/

#ifdef __STDC__
extern boolean IsHTMLName(char *path);
#else				/* __STDC__ */
extern boolean IsHTMLName(/*char *path*/);
#endif				/* __STDC__ */

#ifdef __STDC__
extern boolean IsImageName(char *path);
#else				/* __STDC__ */
extern boolean IsImageName(/*char *path*/);
#endif				/* __STDC__ */

#ifdef __STDC__
extern boolean IsTextName(char *path);
#else				/* __STDC__ */
extern boolean IsTextName(/*char *path*/);
#endif				/* __STDC__ */

#ifdef __STDC__
extern boolean IsHTTPPath(char *path);
#else				/* __STDC__ */
extern boolean IsHTTPPath(/*char *path*/);
#endif				/* __STDC__ */

#ifdef __STDC__
extern boolean IsW3Path(char *path);
#else				/* __STDC__ */
extern boolean IsW3Path(/*char *path*/);
#endif				/* __STDC__ */

#ifdef __STDC__
extern void NormalizeURL(char *orgName, Document doc, char *newName, char *docName);
#else				/* __STDC__ */
extern void NormalizeURL(/*char *orgName, Document doc, char *newName, char *docName*/);
#endif				/* __STDC__ */

#ifdef __STDC__
extern void StopRequest(int docid);
#else 
extern void StopRequest(/*int docid*/);
#endif /* __STDC__ */


#endif /* QUERY_H */

/*

   End of declaration */










