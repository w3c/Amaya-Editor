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

struct _XTigerTemplate
{	
  ThotBool        isLibrary;			//Is this a library? (otherway it's a template)
#ifdef TODO_XTIGER
  DicDictionary   libraries;			//Imported libraries
#endif
  DicDictionary   simpleTypes;			//All simple types declared in the document
  DicDictionary   elements;				//All element types declared in the document
  DicDictionary   components;			//All component types declared in the document
  DicDictionary   unions;				//All union types declared in the document
};

/* Structure of a Declaration */

//Just for clarity
typedef int TypeNature;
typedef int SimpleTypeType;

typedef struct _XmlElement
{
	char	*name;
} XmlElement;

typedef struct _SimpleType
{
	SimpleTypeType type;
} SimpleType;

typedef struct _Component
{
	Element        content;
} Component;

typedef struct _Union
{
	DicDictionary  include; //Dictionary<Declaration>
	DicDictionary  exclude; //Dictionary<Declaration>
} Union;

typedef struct _Declaration
{
	char          *name;
	TypeNature     nature;
	XTigerTemplate declaredIn;
	union
	{
		SimpleType   simpleType;
		Component    componentType;
		Union        unionType;
		XmlElement   elementType;
	};
} *Declaration;

//List of loaded templates and libraries
THOT_EXPORT DicDictionary templates;

/*----------------------------------------------------------------------
  Returns a library with the predefined types
  ----------------------------------------------------------------------*/
extern XTigerTemplate CreatePredefinedTypesLibrary ( void );

/*----------------------------------------------------------------------
  Initializing the template environment
  ----------------------------------------------------------------------*/
extern DicDictionary InitializeTemplateEnvironment ( void );

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
extern Declaration NewSimpleType ( const XTigerTemplate t,
                                   const char *name,
                                   TypeNature xtype );

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
extern Declaration NewComponent ( const XTigerTemplate t,
                                  const char *name,
                                  const Element el );

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
extern Declaration NewUnion ( const XTigerTemplate t,
                              const char *name,
                              DicDictionary include,
                              DicDictionary exclude );

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
extern Declaration NewElement ( const XTigerTemplate t,
                              const char *name );

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
extern void FreeDeclaration ( Declaration dec );

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
extern Declaration GetDeclaration(const XTigerTemplate t, const char *name);

/*----------------------------------------------------------------------
   Creates a new template with its dictionaries
 ----------------------------------------------------------------------*/
extern XTigerTemplate NewXTigerTemplate ( ThotBool addPredefined );

/*----------------------------------------------------------------------
  Free all the space used by a template (also its dictionaries)
  ----------------------------------------------------------------------*/
extern void FreeXTigerTemplate ( XTigerTemplate t );

/*----------------------------------------------------------------------
  Imports all declarations in a library lib to a template t
  ----------------------------------------------------------------------*/
extern void AddLibraryDeclarations (XTigerTemplate t, XTigerTemplate lib);

/*----------------------------------------------------------------------
Imports all declarations in a library lib to a template t
----------------------------------------------------------------------*/
extern void AddLibraryDeclarations (XTigerTemplate t, XTigerTemplate lib);

/*----------------------------------------------------------------------
Removes the declaration identified by name
----------------------------------------------------------------------*/
extern void RemoveOldDeclarations (XTigerTemplate t, char *name);
#endif //TEMPLATE_DECLARATIONS
