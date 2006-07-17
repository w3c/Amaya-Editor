#ifndef TEMPLATE_TEMPLATES
#define TEMPLATE_TEMPLATES

#define THOT_EXPORT extern
#include "amaya.h"
#include "document.h"
#include "mydictionary.h"

extern void CreateInstanceOfTemplate (Document doc, char *templatename, char *docname);

/*----------------------------------------------------------------------
  ClosingDocument
  Callback called before closing a document. Checks for unused templates.
  ----------------------------------------------------------------------*/
extern void ClosingDocument(NotifyDialog* dialog)

#endif /* TEMPLATE_TEMPLATES */

