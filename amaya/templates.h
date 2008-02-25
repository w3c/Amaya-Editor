#ifndef TEMPLATE_TEMPLATES
#define TEMPLATE_TEMPLATES

#define THOT_EXPORT extern
#include "amaya.h"
#include "document.h"
#include "containers.h"
#include "MENUconf.h"


/* Structure of a template */
struct _XTigerTemplate;
typedef struct _XTigerTemplate *XTigerTemplate;

//Private structure of a template
struct _XTigerTemplate
{	
  char*           name;           //Template name
  char*           version;        //Version of XTiger language
  char*           templateVersion;//Version of template
  ThotBool        isLibrary;			//Is this a library? (otherway it's a template)
  ThotBool        isPredefined;   //Is this the predefined library
  ThotBool        isLoaded;       //Is the template is loaded ?
  HashMap         libraries;			//Imported libraries (StringHashMap<XTigerTemplate>)
  HashMap         simpleTypes;		//All simple types declared in the document (KeywordHashMap<Declaration>)
  HashMap         elements;				//All element types declared in the document (KeywordHashMap<Declaration>)
  HashMap         components;			//All component types declared in the document (KeywordHashMap<Declaration>)
  HashMap         unions;				  //All union types declared in the document (KeywordHashMap<Declaration>)
  HashMap         unknowns;       //All unknown declarations, used in template parsing,
                                  // must be empty after parsing. (KeywordHashMap<Declaration>)
  Document        doc;            //Use to store component structures
  int             users;          //Number of documents using this template
  
  DLList          errorList;      //Error string list (DLList<char*>)
                                  //  Used until new error system is written.
};

// Notes : 
// All KeywordHashMap<Declaration> must embed their own copy of their keys
// in order to prevent corruption.

// XTiger declaration nature
// Note : the order is very important for user presentation.
typedef enum _TypeNature
{
  SimpleTypeNat,
  UnionNat,
  ComponentNat,
  XmlElementNat,
  UnknownNat
} TypeNature;

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
	HashMap  include;  //KeywordHashMap<Declaration>
	HashMap  exclude;  //KeywordHashMap<Declaration>
  HashMap  expanded; //KeywordHashMap<Declaration>
} Union;

/* Structure of a declaration */
struct _Declaration;
typedef struct _Declaration *Declaration;

struct _Declaration
{
	char          *name;
	TypeNature     nature;
  ThotBool       blockLevel; // TRUE if the element is a block (not inline)
	XTigerTemplate declaredIn; // The template which declare the decl
  XTigerTemplate usedIn;     // The template which embed the decl
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
