#ifndef Eedition
#define Eedition

/****************************************************************************/
/*---------------------------- NESTED INCLUDES -----------------------------*/
/****************************************************************************/


/****************************************************************************/
/*------------------------- DEFINE AND TYPEDEF AREA ------------------------*/
/****************************************************************************/


#ifndef Xedition
/****************************************************************************/
/*-------------------------- EXTERNAL  VARIABLES ---------------------------*/
/****************************************************************************/


/****************************************************************************/
/*-------------------------- EXTERNAL  FUNCTIONS ---------------------------*/
/****************************************************************************/
#ifdef __STDC__
/****************************************************************************/
/*----------------------------- C++ INTERFACE ------------------------------*/
/****************************************************************************/


extern void ObjetsAligner 	(char Plan, char Axe, Document Doc);
extern void ObjetsGrouper 	(Document Doc);
extern void ObjetsDegrouper 	(Document Doc);
extern void ObjetsHorsGroupe 	(Document Doc);
extern void ObjetsMemeGroupe	(Document Doc);
extern void ObjetsMemeTaille 	(Document Doc);
extern void ObjetsDupliquer 	(Document Doc);
extern void ObjetsEquidistants 	(Document Doc);
extern void ObjetsModifierPlan 	(char Plan, Document Doc);
extern void CopieVersPalette 	(Document Doc);

#else
/****************************************************************************/
/*-------------------------- SIMPLE  C INTERFACE ---------------------------*/
/****************************************************************************/

extern void ObjetsAligner 	(/* char Plan, char Axe , Document Doc*/);
extern void ObjetsGrouper 	(/* Document Doc */);
extern void ObjetsDegrouper 	(/* Document Doc */);
extern void ObjetsHorsGroupe 	(/* Document Doc */);
extern void ObjetsMemeGroupe	(/* Document Doc */);
extern void ObjetsMemeTaille 	(/* Document Doc */);
extern void ObjetsDupliquer 	(/* Document Doc */);
extern void ObjetsEquidistants 	(/* Document Doc */);
extern void ObjetsModifierPlan 	(/* char Plan, Document Doc */);
extern void CopieVersPalette 	(/* Document Doc */);

#endif /* __STDC__ */
#endif /* Xedition */
#endif /* Eedition */
