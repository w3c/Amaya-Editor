#ifndef Eutilitaires
#define Eutilitaires

/****************************************************************************/
/*---------------------------- NESTED INCLUDES -----------------------------*/
/****************************************************************************/


/****************************************************************************/
/*------------------------- DEFINE AND TYPEDEF AREA ------------------------*/
/****************************************************************************/


#ifndef Xutilitaires
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

extern void CreateAnAttribute	(Element element, int attrTypeNum,
				 int value, Document doc);
extern void GetValAttrPosition 	(Element element, int *posH, int *posV,
			    	 Document doc);
extern void SetAttrPosition 	(Element element, int positionH, int positionV,
			    	 Document doc);
extern void GetValAttrTaille	(Element element, int *hauteur, int *largeur,
			    	 Document doc);
extern void SetAttrTaille	(Element element, int hauteur, int largeur,
			    	 Document doc);
extern void GetValAttrAncre 	(Element element, int *PosHO, int *PosVO,
						  int *PosHD, int *PosVD,
				 Document doc);
extern void SetAttrAncre 	(Element element, int AncreHO, int AncreVO,
						  int AncreHD, int AncreVD,
				 Document doc);
extern void RemoveAttr		(Element element, int attrTypeNum,
				 Document doc);
extern void AjouterUnNoeud 	(Element element, Document Doc,
				 Element *ElemCree, int TypeNoeud);
extern int  GetValAttrEntier	(Element element, int TtAttribute, Document Doc);
extern void SetAttrEntier	(Element element, int TtAttribute, int valeur,
				 Document Doc);
extern Attribute ExistAttribute (Element element, int TtAttribute, Document Doc);
extern int GetPRule 		(Element element, int rule);
extern void SetPRule 		(Element element, int rule, int value,
				 Document Doc);
extern Element ChercherContexteInsert (Document Doc, boolean *inside);

#else
/****************************************************************************/
/*-------------------------- SIMPLE  C INTERFACE ---------------------------*/
/****************************************************************************/

extern void CreateAnAttribute	();
extern void GetValAttrPosition 	();
extern void SetAttrPosition 	();
extern void GetValAttrTaille	();
extern void SetAttrTaille	();
extern void GetValAttrAncre 	();
extern void SetAttrAncre 	();
extern void RemoveAttr		();
extern void AjouterUnNoeud 	();
extern int  GetValAttrEntier	();
extern void SetAttrEntier	();
extern Attribute ExistAttribute ();
extern int GetPRule 		();
extern void SetPRule 		();
extern Element ChercherContexteInsert ();

#endif /* __STDC__ */
#endif /* Xutilitaires */
#endif /* Eutilitaires */
