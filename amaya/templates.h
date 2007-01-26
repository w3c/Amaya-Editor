#ifndef TEMPLATE_TEMPLATES
#define TEMPLATE_TEMPLATES

#define THOT_EXPORT extern
#include "amaya.h"
#include "document.h"

#include "MENUconf.h"

#define TEMPLATE_SSHEMA_NAME  "Template"

typedef void  *DicElement;

//A record contains an element and its key.
struct sRecord;
typedef struct sRecord *Record;
struct sRecord
{
	char        *key;
  DicElement   element;
	Record       next;
};

//A dictionary contains a sequence of Records
struct sDictionary;
typedef struct sDictionary* DicDictionary;
struct sDictionary
{
	Record first;
	Record iter;
};

/* Structure of a template */
struct _XTigerTemplate;
typedef struct _XTigerTemplate *XTigerTemplate;

//Private structure of a template
struct _XTigerTemplate
{	
  char*           version;        //Version of XTiger language
  char*           templateVersion;//Version of template
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

typedef enum _TypeNature {SimpleTypeNat, XmlElementNat, ComponentNat,
                          UnionNat} TypeNature;

// XTiger simple type
typedef enum _SimpleTypeType {XTNumber, XTString, XTBoolean} SimpleTypeType;

/* Structure of a Declaration */

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
} Component_;

typedef struct _Union
{
	DicDictionary  include;  //Dictionary<Declaration>
	DicDictionary  exclude;  //Dictionary<Declaration>
  DicDictionary  expanded; //Dictionary<Declaration>
} Union;

/* Structure of a declaration */
struct _Declaration;
typedef struct _Declaration *Declaration;

struct _Declaration
{
	char          *name;
	TypeNature     nature;
	XTigerTemplate declaredIn;
	union
	{
		SimpleType   simpleType;
		Component_   componentType;
		Union        unionType;
		XmlElement   elementType;
	};
};

struct menuType
{
	char        *label;
	TypeNature   type;
};

#endif /* TEMPLATE_TEMPLATES */
