
#include "constmenu.h"

EXPORT int		NCreatedElements;	/* number of elements created */
EXPORT PtrElement	CreatedElement[500];	/* pointers to the created elements */
EXPORT boolean		IgnoreReadOnlyParent;

/* data for the CHOICE menu */
EXPORT boolean		NatureChoice;
EXPORT int		ChoiceMenuTypeNum[LgMaxChoiceMenu];
EXPORT PtrSSchema	ChoiceMenuSSchema[LgMaxChoiceMenu];

EXPORT int		ChosenTypeNum;    /* type chosen by the user in the CHOICE
					  menu */
EXPORT PtrSSchema	ChosenTypeSSchema; /* structure schema defining that
					      type */
