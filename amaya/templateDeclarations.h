#ifndef TEMPLATE_DECLARATIONS
#define TEMPLATE_DECLARATIONS

#define PREDEFINED_LIB "-Predefined-"
#define COMPONENT   0
#define SIMPLE_TYPE 1
#define XMLELEMENT  2
#define UNION       3

#define XTIGER_INTEGER 0
#define XTIGER_DECIMAL 1
#define XTIGER_STRING  2
#define XTIGER_BOOLEAN 3
#define XTIGER_CUSTOM  4

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
//  DicDictionary   libraries;			//Imported libraries
  DicDictionary   simpleTypes;			//All simple types declared in the document
  DicDictionary   elements;				//All element types declared in the document
  DicDictionary   components;			//All component types declared in the document
  DicDictionary   unions;				//All union types declared in the document
};

/* Structure of a Declaration */
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

#endif //TEMPLATE_DECLARATIONS
