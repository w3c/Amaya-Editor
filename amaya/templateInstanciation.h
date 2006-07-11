#ifndef TEMPLATE_INSTANCIATION
#define TEMPLATE_INSTANCIATION

#define THOT_EXPORT extern
#include "amaya.h"
#include "document.h"
#include "templateDeclarations.h"

extern void InstanciateTemplate(char *templatename);

void  CreateInstance(char *templatePath, char *instancePath);

extern void PreInstanciateComponents(XTigerTemplate t);

#endif /* TEMPLATE_INSTANCIATION */
