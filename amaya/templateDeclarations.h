#ifndef TEMPLATE_DECLARATIONS
#define TEMPLATE_DECLARATIONS

//The predefined library id
#define PREDEFINED_LIB "-Predefined-"

//Type natures
#define COMPONENT   0
#define SIMPLE_TYPE 1
#define XMLELEMENT  2
#define UNION       3

//Simple type ids
#define XTIGER_NUMBER  0
#define XTIGER_STRING  1
#define XTIGER_BOOLEAN 2
//And for future issues
#define XTIGER_CUSTOM  3

#define THOT_EXPORT extern
#include "amaya.h"
#include "document.h"
#include "mydictionary.h"

/* Structure of a template */
struct _XTigerTemplate;
typedef struct _XTigerTemplate *XTigerTemplate;

/* Structure of a declaration */
struct _Declaration;
typedef struct _Declaration *Declaration;

typedef int SimpleTypeType;

#ifdef TEMPLATES
	//List of loaded templates and libraries
	THOT_EXPORT DicDictionary templates;
#endif

/*----------------------------------------------------------------------
  Initializing the template environment
  ----------------------------------------------------------------------*/
extern void InitializeTemplateEnvironment ( void );

/*----------------------------------------------------------------------
   Creates a new template with its dictionaries and stores it.
 ----------------------------------------------------------------------*/
extern XTigerTemplate NewXTigerTemplate (const char *templatePath, 
										 const ThotBool addPredefined );

/*----------------------------------------------------------------------
  Creates a new library with its dictionaries and stores it.
  ----------------------------------------------------------------------*/
extern XTigerTemplate NewXTigerLibrary (const char *templatePath, 
										const ThotBool addPredefined );

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
extern void NewSimpleType ( const XTigerTemplate t,
                                   const char *name,
                                   SimpleTypeType xtype );

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
extern void NewComponent ( const XTigerTemplate t,
                                  const char *name,
                                  const Element el );

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
extern void NewUnion ( const XTigerTemplate t,
                              const char *name,
                              DicDictionary include = NULL,
                              DicDictionary exclude = NULL );

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
extern void NewElement ( const XTigerTemplate t,
                              const char *name );

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
extern Declaration GetDeclaration(const XTigerTemplate t, const char *name);


/*----------------------------------------------------------------------
  Free all the space used by a template (also its dictionaries)
  ----------------------------------------------------------------------*/
extern void FreeXTigerTemplate ( XTigerTemplate t );

/*----------------------------------------------------------------------
  Imports all declarations in a library lib to a template t
  ----------------------------------------------------------------------*/
extern void AddLibraryDeclarations (XTigerTemplate t, XTigerTemplate lib);

extern void PreInstanciateComponents(XTigerTemplate t);
extern void RedefineSpecialUnions(XTigerTemplate t);
extern void DumpDeclarations(XTigerTemplate t);

#endif //TEMPLATE_DECLARATIONS
