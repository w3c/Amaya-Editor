#define THOT_EXPORT extern
#include "amaya.h"
#include "document.h"

extern void InstanciateTemplate(char *templatename, Document doc);

extern void InstanciateTemplate (Document doc, char *templatename,
								 char *docname, DocumentType docType,
								 ThotBool loaded);
