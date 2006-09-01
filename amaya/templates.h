#ifndef TEMPLATE_TEMPLATES
#define TEMPLATE_TEMPLATES

#define THOT_EXPORT extern
#include "amaya.h"
#include "document.h"
#include "mydictionary.h"

#define UNION_ANY            "any"
#define UNION_ANYCOMPONENT   "anyComponent"
#define UNION_ANYSIMPLE      "anySimple"
#define UNION_ANYELEMENT     "anyElement"
#define UNION_ANY_DEFINITION "anyComponent anySimple anyElement"

#define TYPE_NUMBER          "number"
#define TYPE_STRING          "string"
#define TYPE_BOOLEAN         "boolean"

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

//Private structure of a template
struct _XTigerTemplate
{	
  ThotBool        isLibrary;			//Is this a library? (otherway it's a template)
  ThotBool        isPredefined;   //Is this the predefined library
  DicDictionary   libraries;			//Imported libraries
  DicDictionary   simpleTypes;		//All simple types declared in the document
  DicDictionary   elements;				//All element types declared in the document
  DicDictionary   components;			//All component types declared in the document
  DicDictionary   unions;				  //All union types declared in the document
  Document        doc;            //Use to store component structures
  int             users;          //Number of documents using this template
};

/* Structure of a Declaration */

//Just for clarity
typedef int TypeNature;

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

struct _Declaration
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
};

struct menuType
{
	char *label;
	int   type;
};

#endif /* TEMPLATE_TEMPLATES */

