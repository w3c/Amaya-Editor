/*
 * amaya_api.h : defines part of the programmable Amaya API.
 */

#define THOT_EXPORT extern
#include "amaya.h"
#include "amaya_api.h"


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

