#ifndef TEMPLATE_DECLARATIONS
#define TEMPLATE_DECLARATIONS

#define PREDEFINED_LIB "-Predefined-"

#define THOT_EXPORT extern
#include "amaya.h"
#include "document.h"
#include "mydictionary.h"

/*----------------------------------------------------------------------
  Structure of a template
  ----------------------------------------------------------------------*/
struct _XTigerTemplate;
typedef struct _XTigerTemplate *XTigerTemplate;

struct _XTigerTemplate {	
	bool isLibrary;					//Is this a library? (otherway it's a template)
	DicDictionary libraries;		//Imported libraries
	DicDictionary declaredTypes;	//All types declared in the document
	int documentUsingMe;			//How many documents are using this template or library?
};

/*----------------------------------------------------------------------
  Structure of a Declaration
  ----------------------------------------------------------------------*/
typedef enum TypeNatureEnum { SIMPLE_TYPE, COMPONENT, UNION } TypeNature;
typedef enum SimpleTypeEnum { XTIGER_INTEGER, XTIGER_STRING, XTIGER_BOOLEAN, XTIGER_CUSTOM } SimpleTypeType;

typedef struct _SimpleType {
	SimpleTypeType type;
} SimpleType;

typedef struct _Component {
	Element content;
} Component;

typedef struct _Union {
	DicDictionary include; //Dictionary<Declaration>
	DicDictionary exclude; //Dictionary<Declaration>
} Union;

typedef struct _Declaration {
	char *name;
	TypeNature nature;
	XTigerTemplate declaredIn;
	union {
		SimpleType simpleType;
		Component componentType;
		Union unionType;
	};
} *Declaration;

/*----------------------------------------------------------------------
  Initializing the template environment
  ----------------------------------------------------------------------*/

DicDictionary InitializeTemplateEnvironment();

/*----------------------------------------------------------------------
  Creation and freeing of declarations
  ----------------------------------------------------------------------*/
Declaration NewDeclaration(const XTigerTemplate t, const char *name, TypeNature type);

Declaration NewSimpleType(const XTigerTemplate t, const char *name, SimpleTypeType type);

Declaration NewComponent(const XTigerTemplate t, const char *name, const Element el);

Declaration NewUnion(const XTigerTemplate t, const char *name, DicDictionary include, DicDictionary exclude);

void FreeDeclaration(Declaration dec);

/*----------------------------------------------------------------------
  Creates a new template with its dictionaries
  ----------------------------------------------------------------------*/
XTigerTemplate NewXTigerTemplate(bool addPredefined = true);

void FreeXTigerTemplate(XTigerTemplate t);

#endif //TEMPLATE_DECLARATIONS