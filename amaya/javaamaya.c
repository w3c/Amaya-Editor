/*
 * javaamaya.h : defines part of the programmable Amaya API.
 */

#define THOT_EXPORT extern
#include "amaya.h"
#include "javaamaya.h"


/*----------------------------------------------------------------------
   GetDocumentURL returns the URL of the document.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
char *GetDocumentURL (Document doc)
#else
char *GetDocumentURL (doc)
Document doc;

#endif
{
    if ((doc < 0) || (doc >= DocumentTableLength)) return(NULL);
    return(DocumentURLs[doc]);
}

/*----------------------------------------------------------------------
   GetMessage returns an amaya message
  ----------------------------------------------------------------------*/
#ifdef __STDC__
char *GetMessage (int msg)
#else
char *GetMessage (msg)
int msg;

#endif
{
    return(TtaGetMessage(AMAYA, msg));
}

