/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

#ifndef _ACTION_H_
#define _ACTION_H_

#include "document.h"
#include "pschema.h"
#include "tree.h"
#include "view.h"
#include "attribute.h"
#include "presentation.h"

/* ********* ATTENTION ********** */
/* The list RegisteredAppEvents within the module app.c have to be conform to
   this type enum APPevent */
/* NUMBER_OF_APP_EVENTS gives the number of entries in the list */
#define NUMBER_OF_APP_EVENTS 49
typedef enum
  {
     TteAttrMenu,
     TteAttrCreate,
     TteAttrModify,
     TteAttrRead,
     TteAttrSave,
     TteAttrExport,
     TteAttrDelete,
     TteElemMenu,
     TteElemNew,
     TteElemRead,
     TteElemSave,
     TteElemExport,
     TteElemDelete,
     TteElemSelect,
     TteElemExtendSelect,
     TteElemClick,
     TteElemLClick,
     TteElemMClick,
     TteElemRClick,
     TteElemActivate,
     TteElemSetReference,
     TteElemInclude,
     TteElemFetchInclude,
     TteElemCopy,
     TteElemPaste,
     TteElemChange,
     TteElemMove,
     TteElemTextModify,
     TteElemGraphModify,
     TteElemReturn,
     TteElemTab,
     TteElemMouseOver,
     TteElemMouseOut,
     TtePRuleCreate,
     TtePRuleModify,
     TtePRuleDelete,
     TteDocOpen,
     TteDocTmpOpen,
     TteDocCreate,
     TteDocClose,
     TteDocSave,
     TteDocExport,
     TteDocNatPresent,
     TteViewOpen,
     TteViewClose,
     TteViewResize,
     TteViewScroll,
     TteInit,
     TteExit,
     TteNull
  }
APPevent;

typedef struct
  {
     APPevent            event;
  }
NotifyEvent;

typedef struct
  {
     APPevent            event;
     Document            document;
     View                view;
  }
NotifyDialog;

typedef struct
  {
     APPevent            event;
     Document            document;
     View                view;
     int                 verticalValue;
     int                 horizontalValue;
  }
NotifyWindow;

typedef struct
  {
     APPevent            event;
     Document            document;
     Element             element;
     Attribute           attribute;
     AttributeType       attributeType;
     int		             info; /* 1 if event sent by undo/redo */

     TypeRestriction     restr;
  }
NotifyAttribute;

typedef struct
  {
     APPevent            event;
     Document            document;
     Element             element;
     ElementType         elementType;
     int                 position;
     int		 info; /* 1 if event sent by undo/redo */
  }
NotifyElement;

/*
 * Special value of "position" field for the
 * TteElemDelete.Pre event.
 * January 9th, 1997. Dominique
 */
#define TTE_STANDARD_DELETE_FIRST_ITEMS		0
#define TTE_STANDARD_DELETE_LAST_ITEM		1
#define TTE_MERGE_DELETE_ITEM			2
#define TTE_TOOLKIT_DELETE			3

typedef struct
  {
     APPevent            event;
     Document            document;
     Element             element;
     Element             target;
     Document            targetdocument;
  }
NotifyOnTarget;

typedef struct
  {
     APPevent            event;
     Document            document;
     Element             element;
     Element             target;
     int                 value;
  }
NotifyOnValue;

typedef struct
  {
     APPevent            event;
     Document            document;
     Element             element;
     PRule               pRule;
     int                 pRuleType;
     int                 value;
  }
NotifyPresentation;

typedef struct
  {
     APPevent            event;
     Document            document;
     SSchema             nature;
  }
NotifyNaturePresent;

typedef union
  {
     NotifyEvent         notifyevent;
     NotifyDialog        notifydialog;
     NotifyWindow        notifywindow;
     NotifyAttribute     notifyattribute;
     NotifyElement       notifyelement;
     NotifyOnTarget      notifyontarget;
     NotifyOnValue       notifyonvalue;
     NotifyPresentation  notifypresentation;
     NotifyNaturePresent notifynaturepresent;
  }
Notify;

#endif
