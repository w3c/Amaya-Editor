/*
 * Copyright (c) 1996 INRIA, All rights reserved
 */

#ifndef Eliens
#define Eliens

/*----------------------------------------------------------------------
---------------------------- NESTED INCLUDES -----------------------------
  ----------------------------------------------------------------------*/


/*----------------------------------------------------------------------
------------------------- DEFINE AND TYPEDEF AREA ------------------------
  ----------------------------------------------------------------------*/


#ifndef Xliens
/*----------------------------------------------------------------------
-------------------------- EXTERNAL  VARIABLES ---------------------------
  ----------------------------------------------------------------------*/

extern int SaveCoinX, SaveCoinY, SaveHauteur, SaveLargeur;

/*----------------------------------------------------------------------
-------------------------- EXTERNAL  FUNCTIONS ---------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
/*----------------------------------------------------------------------
----------------------------- C++ INTERFACE ------------------------------
  ----------------------------------------------------------------------*/

extern void DetruirePolygone 		(Element Fleche, Document Doc);
extern void TracerLienComposite 	(Element Lien, Document Doc);
extern void TracerOrigineLien 		(Element Lien, Document Doc);
extern void TracerDestinationLien 	(Element Lien, Document Doc);
extern void AjusterBoite 		(Element Box, Document Doc);
extern void PositionnerFleche 		(NotifyElement *fleche);
extern void PositionnerLien 		(NotifyElement *fleche);
extern void RePositionnerLien 		(NotifyElement *fleche);
extern void SauverCoordonnees 		(Element Noeud, Document Doc);
extern void DessinerFleche 		(NotifyElement *fleche, char type);

#else
/*----------------------------------------------------------------------
-------------------------- SIMPLE  C INTERFACE ---------------------------
  ----------------------------------------------------------------------*/

extern void DetruirePolygone 		(/* Element Fleche, Document Doc */);
extern void TracerLienComposite 	(/* Element Lien, Document Doc */);
extern void TracerOrigineLien 		(/* Element Lien, Document Doc */);
extern void TracerDestinationLien 	(/* Element Lien, Document Doc */);
extern void AjusterBoite 		(/* Element Box, Document Doc */);
extern void PositionnerFleche 		(/* NotifyElement *fleche */);
extern void PositionnerLien 		(/* NotifyElement *fleche */);
extern void RePositionnerLien 		(/* NotifyElement *fleche */);
extern void SauverCoordonnees 		(/* Element Noeud, Document Doc */);
extern void DessinerFleche 		(/* NotifyElement *fleche,char type*/);

#endif /* __STDC__ */
#endif /* Xliens */
#endif /* Eliens */
