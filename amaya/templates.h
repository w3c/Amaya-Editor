#ifndef TEMPLATE_TEMPLATES
#define TEMPLATE_TEMPLATES

#define THOT_EXPORT extern
#include "amaya.h"
#include "document.h"
#include "containers.h"
#include "MENUconf.h"


#define Template_Current_Version "0.9"


typedef enum _TemplateFlag
{
  templNone = 0,
  templTemplate    = 1 << 0,  // Template
  templLibraryFlag = 1 << 1,   // Library (implies template).
  templLibrary     = templLibraryFlag+templTemplate,
  templInstance    = 1 << 2,  // Instance of template
  
  templloaded      = 1 << 4,  // Template is really loaded.
  
  templInternal    = 1 << 8,  // Internal only, no edited document.
  templPredefined  = 1 << 9   // predefined library (base or html).
}TemplateFlag;


struct _XTigerTemplate;
typedef struct _XTigerTemplate *XTigerTemplate;
struct _XTigerTemplate
{
  int           state;          // Union of TemplateFlag
  char*         uri;            // Template URI (formerly name).
  char*         base_uri;       // URI of template if this is an instance.
  char*         version;        // Version of XTiger language
  char*         templateVersion;// Version of template

  HashMap       libraries;      // Imported libraries (StringHashMap<XTigerTemplate>)
  
  SearchSet     simpleTypes;    // All simple types declared in the document (SearchSet<Declaration, char*>)
  SearchSet     elements;       // All element types declared in the document (SearchSet<Declaration, char*>)
  SearchSet     components;     // All component types declared in the document (SearchSet<Declaration, char*>)
  SearchSet     unions;         // All union types declared in the document (SearchSet<Declaration, char*>)
  SearchSet     unknowns;       // All unknown declarations, used in template parsing (SearchSet<Declaration, char*>)

  Document      doc;            // Use to store component structures
  int           ref;            // Reference counting
  SList         errorList;      // Error string list (DLList<char*>)
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
  SearchSet  include;  // SearchSet<Declaration, char*>
  SearchSet  exclude;  // SearchSet<Declaration, char*>
  SearchSet  expanded; // SearchSet<Declaration, char*>
  char*      includeStr;
  char*      excludeStr;
} Union;

/* Structure of a declaration */
struct _Declaration;
typedef struct _Declaration *Declaration;

struct _Declaration
{
	char          *name;
	TypeNature     nature;
	// TODO Change blockLevel with a triplet (block,inline, both) to use with union
  int            blockLevel; // 0=unknown, 1=inline, 2=block, 3 = both
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
