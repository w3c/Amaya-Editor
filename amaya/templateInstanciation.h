#ifndef TEMPLATE_INSTANCIATION
#define TEMPLATE_INSTANCIATION

#define THOT_EXPORT extern
#include "amaya.h"
#include "document.h"
#include "templateDeclarations.h"

extern void InstanciateTemplate(char *templatename, Document doc);

extern void InstanciateTemplate (Document doc, char *templatename,
								 char *docname, DocumentType docType,
								 ThotBool loaded);

extern void PreInstanciateComponents(XTigerTemplate t);

#endif /* TEMPLATE_INSTANCIATION */

