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
 
/*
 * specificdriver.h : header file for the presentation driver used to
 *           manipulate presentation using presentation specific attributes.
 */

#ifndef __SPECIFIC_DRIVER_H__
#define __SPECIFIC_DRIVER_H__

#include "presentation.h"
#include "presentdriver.h"

/*
 * A specific presentation target is an Element in the graph.
 */

typedef Element     SpecificTarget;

/*
 * Changing specific presentation of an element is just dependant on
 * its document, the schemas information is just used as a cache.
 * Fields described in presentdriver.h
 */

typedef struct struct_SpecificContext
  {
     PresentationStrategy *drv;
     Document            doc;
     SSchema             schema;
     int                 destroy;/* destructive mode ? */
     unsigned long       magic1;
     unsigned long       magic2;
  }
SpecificContextBlock, *SpecificContext;

/*
 * A specific presentation value is made of a value and an unit.
 */

typedef PresentationValue SpecificValue;

#define SPECIFIC_FONT_STYLE_BOLD		StyleBold
#define SPECIFIC_FONT_STYLE_ROMAN		StyleRoman
#define SPECIFIC_FONT_STYLE_ITALICS		StyleItalics
#define SPECIFIC_FONT_STYLE_BOLD_ITALICS	StyleBoldItalics
#define SPECIFIC_FONT_STYLE_OBLIQUE		StyleOblique
#define SPECIFIC_FONT_STYLE_BOLD_OBLIQUE	StyleBoldOblique

/*
 * The strategy block for this driver.
 */

extern PresentationStrategy SpecificStrategy;

/*
 * browse functions callbacks.
 */

typedef void        (*SpecificContextApplyHandler)
                    (SpecificTarget target, SpecificContext cond, void *param);

typedef void        (*SpecificSettingsApplyHandler)
                    (SpecificTarget target, SpecificContext cond,
		     PresentationSetting setting, void *param);

/*
 * functions definitions.
 */

#ifdef __STDC__
SpecificContext     GetSpecificContext (Document doc);
void                FreeSpecificContext (SpecificContext ctxt);
void                ApplyAllSpecificContext (Document doc, SpecificTarget target,
			  SpecificContextApplyHandler handler, void *param);
void                ApplyAllSpecificSettings (SpecificTarget target,
		 SpecificContext cond, SpecificSettingsApplyHandler handler,
					      void *param);

#else  /* __STDC__ */
SpecificContext     GetSpecificContext ( /* doc */ )
void                FreeSpecificContext ( /* ctxt */ );
void                ApplyAllSpecificContext (	/* Document doc, SpecificTarget target,
						   SpecificContextApplyHandler handler, void *param */ );
void                ApplyAllSpecificSettings (	/* SpecificTarget target,
						   SpecificContext cond, SpecificSettingsApplyHandler handler,
						   void *param */ );

#endif /* __STDC__ */
#endif /* __SPECIFIC_DRIVER_H__ */
