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
 
/**************************************************************************
 * genericdriver.h : header file for the presentation driver used to	  *
 *       modify the generic presentation description of a document.	  *
 **************************************************************************
 * genericdriver.h : header file for the presentation driver used to
 *           manipulate presentation using presentation generic attributes.
 */

#ifndef __GENERIC_DRIVER_H__
#define __GENERIC_DRIVER_H__

#include "pschema.h"
#include "presentation.h"
#include "presentdriver.h"

/*
 * A generic presentation target is the PSchema where the presentation
 * rules are to be build or modified !
 */

typedef PSchema     GenericTarget;

/*
 * Changing generic presentation of a generic context requires to
 * fill in the context description befor use.
 * First fields described in presentdriver.h
 */
#define MAX_ANCESTORS 10

typedef struct struct_GenericContext
  {
     PresentationStrategy *drv;	/* pointer to the driver strategy block */
     Document            doc;	/* document number */
     SSchema             schema;	/* associated structure */
     int                 destroy;/* destructive mode ? */

     /*
      * below is the context description.
      */
     int                 box;	/* specific presentation box if any */
     int                 type;	/* type of element */
     int                 attr;	/* or attribute */
     int                 attrval;	/* and the corresponding value */
     char               *class;	/* class or box name */
     int                 classattr;	/* class attribute */
     int                 attrelem;	/* elem porting the attribute */
     int                 ancestors[MAX_ANCESTORS];	/* ancestors type */
     int                 ancestors_nb[MAX_ANCESTORS];	/* number for each */

     /*
      * do not touch these !
      */
     unsigned long       magic1;
     unsigned long       magic2;
  }
GenericContextBlock, *GenericContext;

/*
 * A generic presentation value is made of a value and an unit.
 */

typedef PresentationValue GenericValue;

#define GENERIC_FONT_STYLE_BOLD		StyleBold
#define GENERIC_FONT_STYLE_ROMAN	StyleRoman
#define GENERIC_FONT_STYLE_ITALICS	StyleItalics
#define GENERIC_FONT_STYLE_BOLD_ITALICS	StyleBoldItalics
#define GENERIC_FONT_STYLE_OBLIQUE	StyleOblique
#define GENERIC_FONT_STYLE_BOLD_OBLIQUE	StyleBoldOblique

/*
 * The strategy block for this driver.
 */

extern PresentationStrategy GenericStrategy;

/*
 * browse functions callbacks.
 */

typedef void        (*GenericContextApplyHandler)
                    (GenericTarget target, GenericContext cond, void *param);

typedef void        (*GenericSettingsApplyHandler)
                    (GenericTarget target, GenericContext cond,
		     PresentationSetting setting, void *param);

/*
 * allocation functions.
 */

#ifdef __STDC__
GenericContext      GetGenericContext (Document doc);
void                FreeGenericContext (GenericContext ctxt);
void                ApplyAllGenericContext (Document doc, GenericTarget target,
			   GenericContextApplyHandler handler, void *param);
void                ApplyAllGenericSettings (GenericTarget target,
		   GenericContext cond, GenericSettingsApplyHandler handler,
					     void *param);

#else  /* __STDC__ */
GenericContext      GetGenericContext ( /* doc */ )
void                FreeGenericContext ( /* ctxt */ );
void                ApplyAllGenericContext (	/* Document doc, GenericTarget target,
						   GenericContextApplyHandler handler, void *param */ );
void                ApplyAllGenericSettings (	/* GenericTarget target,
						   GenericContext cond, GenericSettingsApplyHandler handler,
						   void *param */ );

#endif /* __STDC__ */
#endif /* __GENERIC_DRIVER_H__ */
