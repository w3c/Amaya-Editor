
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

/*
 * Declarations de constantes pour la representation interne des documents.
 * V. Quint	Juin 1984
 *
 */

#include "thot_sys.h"

#define MAX_ASSOC_DOC 8  	/* nombre max. de types d'elements associes pour un document */
#define MAX_PARAM_DOC 7		/* nombre max de parametres pour un document*/
#define MAX_NATURES 7		/* nombre max de natures simultanees */
#define MAX_PSCHEMAS 10		/* nombre max de schemas de presentation
				   charges */
#define MAX_DOCUMENTS 10		/* nombre max de documents simultanes */
#define MAX_CHAR 100		/* nombre de caracteres par buffer de texte */
#define DEF_SAVE_INTVL 500	/* nombre de caracteres saisis qui
				   declenchent la sauvegarde automatique */
#define MAX_VIEW_DOC 6		/* nombre max de vues pour un document */

#define MAX_LABEL_LEN 12       	/* longueur maximum des labels chaine de car.*/
#define MAX_SEL_ELEM 150	/* nombre max d'elements que l'on peut
				   selectionner d'un coup */
#define MAX_VIEW_OPEN 50	/* >= MAX_VIEW_DOC + MAX_ASSOC_DOC +
				      (MAX_NATURES  MAX_VIEW_DOC) */
#define MAX_DOC_NAME_LEN 32		/* longueur max d'un nom de document */
#define MAX_DOC_IDENT_LEN 32	/* longueur max d'un identificateur de document */
#include "conststr.h"

#include "constprs.h"
