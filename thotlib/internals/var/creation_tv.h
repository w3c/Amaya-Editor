/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 

#include "constmenu.h"

THOT_EXPORT int		NCreatedElements;	/* number of elements created */
THOT_EXPORT PtrElement	CreatedElement[500];	/* pointers to the created elements */

/* data for the CHOICE menu */
THOT_EXPORT boolean		NatureChoice;
THOT_EXPORT int		ChoiceMenuTypeNum[LgMaxChoiceMenu];
THOT_EXPORT PtrSSchema	ChoiceMenuSSchema[LgMaxChoiceMenu];

THOT_EXPORT int		ChosenTypeNum;    /* type chosen by the user in the CHOICE
					  menu */
THOT_EXPORT PtrSSchema	ChosenTypeSSchema; /* structure schema defining that
					      type */
