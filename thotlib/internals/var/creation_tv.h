/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2000
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 

#include "constmenu.h"

THOT_EXPORT int		NCreatedElements;	/* number of elements created */
THOT_EXPORT PtrElement	CreatedElement[500];	/* pointers to the created elements */

/* data for the CHOICE menu */
THOT_EXPORT ThotBool	NatureChoice;
THOT_EXPORT int		ChoiceMenuTypeNum[MAX_MENU];
THOT_EXPORT PtrSSchema	ChoiceMenuSSchema[MAX_MENU];
THOT_EXPORT PtrDocument ChoiceMenuDocument;

THOT_EXPORT int		ChosenTypeNum;     /* type chosen by the user in the
					      CHOICE menu */
THOT_EXPORT PtrSSchema	ChosenTypeSSchema; /* structure schema defining that
					      type */
