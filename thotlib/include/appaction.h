/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * Warning:
 * This module is part of the Thot library, which was originally
 * developed in French. That's why some comments are still in
 * French, but their translation is in progress and the full module
 * will be available in English in the next release.
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
/* Le tableau RegisteredAppEvents du module appana.c doit etre coherent avec */
/* le type APPevent. */
/* La constante NUMBER_OF_APP_EVENTS indique le nombre de valeurs de */
/* l'enum APPevent */

#define NUMBER_OF_APP_EVENTS 45
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
     TteElemTransform,
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
  }
NotifyAttribute;

typedef struct
  {
     APPevent            event;
     Document            document;
     Element             element;
     ElementType         elementType;
     int                 position;
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
     ElementType         elementType;
     ElementType         targetElementType;
  }
NotifyOnElementType;

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
     NotifyOnElementType notifyonelementtype;
     NotifyOnTarget      notifyontarget;
     NotifyOnValue       notifyonvalue;
     NotifyPresentation  notifypresentation;
     NotifyNaturePresent notifynaturepresent;
  }
Notify;

#endif
