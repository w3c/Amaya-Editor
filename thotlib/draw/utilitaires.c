/*!BM
   ------------------------------------------------------------------------------
   *
   * Module      : utilitaires.c
   * Language    : C
   * Authors     : Christian Lenne
   * Version     : %I%
   * Creation    : %E% %U%
   * Description : 
   *
   * Defined variables and functions :
   *
   *           <---------  L O C A L  --------->
   *
   *           <------- E X P O R T E D ------->
   *                     CreateAnAttribute
   *                     GetValAttrPosition
   *                     SetAttrPosition
   *                     GetValAttrTaille
   *                     SetAttrTaille
   *                     GetValAttrAncre 
   *                     SetAttrAncre
   *                     RemoveAttr
   *                     AjouterUnNoeud
   *                     GetValAttrEntier
   *                     SetAttrEntier
   *                     ExistAttribute
   *                     GetPRule
   *                     SetPRule
   ChercherContexteInsert
   *
   ------------------------------------------------------------------------------
   !EM */

#define Xutilitaires

/****************************************************************************/
/*--------------------------- SYSTEM INCLUDES ------------------------------*/
/****************************************************************************/

#include <stdlib.h>

/****************************************************************************/
/*------------------------ APPLICATION  INCLUDES ---------------------------*/
/****************************************************************************/
#include "attribute.h"
#include "Draw3.h"
#include "app.h"
#include "utilitaires.h"
#include "boutons.h"
#include "selection.h"

/****************************************************************************/
/*----------------------- LOCAL DEFINES AND TYPES --------------------------*/
/****************************************************************************/


/****************************************************************************/
/*--------------------------- LOCAL VARIABLES ------------------------------*/
/****************************************************************************/

/****************************************************************************/
/*-------------------------- GLOBAL VARIABLES ------------------------------*/
/****************************************************************************/


/****************************************************************************/
/*-------------------------- FORWARD FUNCTIONS -----------------------------*/
/****************************************************************************/


/****************************************************************************/
/*--------------------------- LOCAL FUNCTIONS ------------------------------*/
/****************************************************************************/


/****************************************************************************/
/*------------------------- EXPORTED  FUNCTIONS ----------------------------*/
/****************************************************************************/

/*!BF
   ------------------------------------------------------------------------------
   *
   * Function      : CreateAnAttribute
   * Result        : void
   * Parameters
   * Name          Type                 Usage
   * ----          ----                 -----
   * element    Element         l'element sur lequel porte l'attribut cree
   * attrTypeNum        int             le type de l'attribut cree
   * value              int             la valeur d'initialisation de l'attribut
   * doc                Document        le document on se trouve l'element
   *
   * Functionality : cree un nouvel attribut avec une valeur initiale pour un
   * element.
   *
   ------------------------------------------------------------------------------
   !EF */

#ifdef __STDC__

void 
CreateAnAttribute (Element element, int attrTypeNum, int value,
		   Document doc)

#else /* __STDC__ */

void 
CreateAnAttribute (element, attrTypeNum, value, doc)
     Element element;
     int attrTypeNum;
     int value;
     Document doc;

#endif /* __STDC__ */
{
  AttributeType attrType;
  Attribute attribute;

  attrType.AttrSSchema = TtaGetSSchema ("Draw3", doc);
  attrType.AttrTypeNum = attrTypeNum;
  attribute = TtaNewAttribute (attrType);
  TtaAttachAttribute (element, attribute, doc);
  TtaSetAttributeValue (attribute, value, element, doc);
  return;
}

/*!BF
   ------------------------------------------------------------------------------
   *
   * Function      : SetAttrPosition
   * Result        : void
   * Parameters
   * Name          Type                 Usage
   * ----          ----                 -----
   * element    Element         l'element concerne
   * positionH  int             la nouvelle valeur pour la positino Horiz
   * positonV   int             la nouvelle valeur pour la position Verticale
   * doc                Document        le document contenant element
   *
   * Functionality :change la valeur des attributs PositionH et PositonV d'un
   * element.
   *
   ------------------------------------------------------------------------------
   !EF */

#ifdef __STDC__

void 
SetAttrPosition (Element element, int positionH, int positionV,
		 Document doc)

#else /* __STDC__ */

void 
SetAttrPosition (element, positionH, positionV, doc)
     Element element;
     int positionH, int positionV;
     Document doc;

#endif /* __STDC__ */

{
  AttributeType PH, PV;
  Attribute TtAttribute;

  PH.AttrSSchema = PV.AttrSSchema = TtaGetSSchema ("Draw3", doc);
  PH.AttrTypeNum = Draw3_ATTR_PositionH;
  PV.AttrTypeNum = Draw3_ATTR_PositionV;

  TtAttribute = TtaGetAttribute (element, PH);
  if (TtAttribute == NULL)
    CreateAnAttribute (element, Draw3_ATTR_PositionH, positionH,
		       doc);
  else
    TtaSetAttributeValue (TtAttribute, positionH, element, doc);

  TtAttribute = TtaGetAttribute (element, PV);
  if (TtAttribute == NULL)
    CreateAnAttribute (element, Draw3_ATTR_PositionV, positionV,
		       doc);
  else
    TtaSetAttributeValue (TtAttribute, positionV, element, doc);
  return;
}

/*!BF
   ------------------------------------------------------------------------------
   *
   * Function      : SetAttrTaille
   * Result        : void
   * Parameters
   * Name          Type                 Usage
   * ----          ----                 -----
   * element    Element         l'element concerne
   * hauteur    int             la nouvelle valeur pour la Hauteur
   * largeur    int             la nouvelle valeur pour la Largeur
   * doc                Document        le document contenant element
   *
   * Functionality : change la valeur des attributs Hauteur et Largeur d'un
   * element
   *
   ------------------------------------------------------------------------------
   !EF */

#ifdef __STDC__

void 
SetAttrTaille (Element element, int hauteur, int largeur, Document doc)

#else /* __STDC__ */

void 
SetAttrTaille (element, hauteur, largeur, doc)
     Element element;
     int hauteur, largeur;
     Document doc;

#endif /* __STDC__ */

{
  AttributeType H, L;
  Attribute TtAttribute;

  H.AttrSSchema = L.AttrSSchema = TtaGetSSchema ("Draw3", doc);
  H.AttrTypeNum = Draw3_ATTR_Hauteur;
  L.AttrTypeNum = Draw3_ATTR_Largeur;

  TtAttribute = TtaGetAttribute (element, L);
  if (TtAttribute == NULL)
    CreateAnAttribute (element, Draw3_ATTR_Largeur, largeur, doc);
  else
    TtaSetAttributeValue (TtAttribute, largeur, element, doc);

  TtAttribute = TtaGetAttribute (element, H);
  if (TtAttribute == NULL)
    CreateAnAttribute (element, Draw3_ATTR_Hauteur, hauteur, doc);
  else
    TtaSetAttributeValue (TtAttribute, hauteur, element, doc);
}

/*!BF
   ------------------------------------------------------------------------------
   *
   * Function      : SetAttrAncre
   * Result        : void
   * Parameters
   * Name          Type                 Usage
   * ----          ----                 -----
   * element    Element         l'element concerne
   * AncreHO    int             la nouvelle valeur pour l'horiz de l'ancre Ori
   * AncreVO    int             la nouvelle valeur pour la vert de l'ancre Ori
   * AncreHD    int             la nouvelle valeur pour l'horiz de l'ancre Dest
   * AncreVD    int             la nouvelle valeur pour la vert de l'ancre Dest
   * doc                Document        le document contenant element
   *
   * Functionality : change la valeur des attributs AncreH? et PositonV?
   * d'un element
   *
   ------------------------------------------------------------------------------
   !EF */

#ifdef __STDC__

void 
SetAttrAncre (Element element, int AncreHO, int AncreVO,
	      int AncreHD, int AncreVD, Document doc)

#else /* __STDC__ */

void 
SetAttrAncre (element, AncreHO, AncreVO, AncreHD, AncreVD, doc)
     Element element;
     int AncreHO;
     int AncreVO;
     int AncreHO;
     int AncreVO;
     Document doc;

#endif /* __STDC__ */
{
  AttributeType PHO, PVO, PHD, PVD;
  Attribute TtAttribute;

  PHO.AttrSSchema =
    PVO.AttrSSchema =
    PHD.AttrSSchema =
    PVD.AttrSSchema = TtaGetSSchema ("Draw3", doc);
  PHO.AttrTypeNum = Draw3_ATTR_AncreHO;
  PVO.AttrTypeNum = Draw3_ATTR_AncreVO;
  PHD.AttrTypeNum = Draw3_ATTR_AncreHD;
  PVD.AttrTypeNum = Draw3_ATTR_AncreVD;

  TtAttribute = TtaGetAttribute (element, PHO);
  if (TtAttribute == NULL)
    CreateAnAttribute (element, Draw3_ATTR_AncreHO, AncreHO, doc);
  else
    TtaSetAttributeValue (TtAttribute, AncreHO, element, doc);

  TtAttribute = TtaGetAttribute (element, PVO);
  if (TtAttribute == NULL)
    CreateAnAttribute (element, Draw3_ATTR_AncreVO, AncreVO, doc);
  else
    TtaSetAttributeValue (TtAttribute, AncreVO, element, doc);

  TtAttribute = TtaGetAttribute (element, PHD);
  if (TtAttribute == NULL)
    CreateAnAttribute (element, Draw3_ATTR_AncreHD, AncreHD, doc);
  else
    TtaSetAttributeValue (TtAttribute, AncreHD, element, doc);

  TtAttribute = TtaGetAttribute (element, PVD);
  if (TtAttribute == NULL)
    CreateAnAttribute (element, Draw3_ATTR_AncreVD, AncreVD, doc);
  else
    TtaSetAttributeValue (TtAttribute, AncreVD, element, doc);
}

/*!BF
   ------------------------------------------------------------------------------
   *
   * Function      : GetValAttrPosition
   * Result        : void
   * Parameters
   * Name          Type                 Usage
   * ----          ----                 -----
   * element    Element         l'element dont on veut connaitre la position
   * posH               *int            la valeur de l'attribut PositionH de l'element
   * posV               *int            la valeur de l'attribut PositionV de l'element
   * doc                Document        le document qui contient l'element
   *
   * Functionality :retourne la valeur des attributs PositionH et PositionV
   *
   ------------------------------------------------------------------------------
   !EF */

#ifdef __STDC__

void 
GetValAttrPosition (Element element, int *posH, int *posV, Document doc)

#else /* __STDC__ */

void 
GetValAttrPosition (element, posH, posV, doc)
     Element element;
     int *posH, *posV;
     Document doc;

#endif /* __STDC__ */

{
  AttributeType PH, PV;

  PH.AttrSSchema = PV.AttrSSchema = TtaGetSSchema ("Draw3", doc);
  PH.AttrTypeNum = Draw3_ATTR_PositionH;
  PV.AttrTypeNum = Draw3_ATTR_PositionV;

  if (TtaGetAttribute (element, PH) == NULL)
    *posH = 0;
  else
    *posH = TtaGetAttributeValue (TtaGetAttribute (element, PH));
  if (TtaGetAttribute (element, PV) == NULL)
    *posV = 0;
  else
    *posV = TtaGetAttributeValue (TtaGetAttribute (element, PV));
}

/*!BF
   ------------------------------------------------------------------------------
   *
   * Function      : GetValAttrTaille
   * Result        : void
   * Parameters
   * Name          Type                 Usage
   * ----          ----                 -----
   * element    Element         l'element dont on veut connaitre la position
   * hauteur    *int            la valeur de l'attribut  Hauteur de l'element
   * largeur    *int            la valeur de l'attribut  Largeur de l'element
   * doc                Document        le document qui contient l'element
   *
   * Functionality : retourne la valeur des attributs Hauteur et Largeur d'un
   * element
   *
   ------------------------------------------------------------------------------
   !EF */


#ifdef __STDC__

void 
GetValAttrTaille (Element element, int *hauteur, int *largeur,
		  Document doc)

#else /* __STDC__ */

void 
GetValAttrTaille (element, hauteur, largeur, doc)
     Element element;
     int *hauteur, *largeur;
     Document doc;

#endif /* __STDC__ */
{
  AttributeType H, L;

  H.AttrSSchema = L.AttrSSchema = TtaGetSSchema ("Draw3", doc);
  H.AttrTypeNum = Draw3_ATTR_Hauteur;
  L.AttrTypeNum = Draw3_ATTR_Largeur;

  if (TtaGetAttribute (element, L) == NULL)
    *largeur = 0;
  else
    *largeur = TtaGetAttributeValue (TtaGetAttribute (element, L));
  if (TtaGetAttribute (element, H) == NULL)
    *hauteur = 0;
  else
    *hauteur = TtaGetAttributeValue (TtaGetAttribute (element, H));
}

/*!BF
   ------------------------------------------------------------------------------
   *
   * Function      : GetValAttrAncre
   * Result        : void
   * Parameters
   * Name          Type                 Usage
   * ----          ----                 -----
   * element    Element         l'element dont on veut connaitre la position
   * posHO              *int            la valeur de l'attribut AncreH de l'element Ori
   * posVO              *int            la valeur de l'attribut AncreV de l'element Ori
   * posHD              *int            la valeur de l'attribut AncreH de l'element Des
   * posVD              *int            la valeur de l'attribut AncreV de l'element Des
   *
   * Functionality : retourne la valeur des attributs AncreH et AncreV pour les
   * elements d'origine et destination
   *
   ------------------------------------------------------------------------------
   !EF */

#ifdef __STDC__

void 
GetValAttrAncre (Element element, int *posHO, int *posVO,
		 int *posHD, int *posVD, Document doc)

#else /* __STDC__ */

void 
GetValAttrAncre (element, posHO, posVO,, posHD, posVD, doc)
     Element element;
     int *posHO, *posVO, *posHD, *posVD;
     Document doc;

#endif /* __STDC__ */

{
  AttributeType PHO, PVO, PHD, PVD;

  PHO.AttrSSchema = PVO.AttrSSchema =
    PHD.AttrSSchema = PVD.AttrSSchema = TtaGetSSchema ("Draw3", doc);
  PHO.AttrTypeNum = Draw3_ATTR_AncreHO;
  PVO.AttrTypeNum = Draw3_ATTR_AncreVO;
  PHD.AttrTypeNum = Draw3_ATTR_AncreHD;
  PVD.AttrTypeNum = Draw3_ATTR_AncreVD;

  if (TtaGetAttribute (element, PHO) == NULL)
    *posHO = 0;
  else
    *posHO = TtaGetAttributeValue (TtaGetAttribute (element, PHO));

  if (TtaGetAttribute (element, PVO) == NULL)
    *posVO = 0;
  else
    *posVO = TtaGetAttributeValue (TtaGetAttribute (element, PVO));
}

/*!BF
   ------------------------------------------------------------------------------
   *
   * Function      : RemoveAttr
   * Result        : void
   * Parameters
   * Name          Type                 Usage
   * ----          ----                 -----
   * element    Element         l'element sur lequel est rattache l'attribut
   * attrTypeNum        int             le numero du type de l'attribut dans le schema
   * doc                Document        le document contenant element
   *
   * Functionality : detruit un attribut d'un type donne porte par un element
   *
   ------------------------------------------------------------------------------
   !EF */

#ifdef __STDC__

void 
RemoveAttr (Element element, int attrTypeNum, Document doc)

#else /* __STDC__ */

void 
RemoveAttr (element, attrTypeNum, doc)
     Element element;
     int attrTypeNum;
     Document doc;

#endif /* __STDC__ */

{
  AttributeType attrType;

  attrType.AttrSSchema = TtaGetSSchema ("Draw3", doc);
  attrType.AttrTypeNum = attrTypeNum;
  TtaRemoveAttribute (element, TtaGetAttribute (element, attrType), doc);
}

/*!BF
   ------------------------------------------------------------------------------
   *
   * Function      : AjouterUnNoeud
   * Result        : void
   * Parameters
   * Name          Type                 Usage
   * ----          ----                 -----
   * RlRoot     Element         Noeud frere precedent
   * Doc                Document        Document de rattachement
   * ElemCree   *Element        Le noeud a creer
   * TypeNoeud  int             Le type du noeud
   *
   * Functionality : Cree un noeud de type TypeNoeud et l'ajoute apres RlRoot si
   * celle-ci n'est pas egale a NULL. Retourne l'ement cree dans ElemCree
   *
   ------------------------------------------------------------------------------
   !EF */

#ifdef __STDC__

void 
AjouterUnNoeud (Element RlRoot, Document Doc,
		Element * ElemCree, int TypeNoeud)

#else /* __STDC__ */

void 
AjouterUnNoeud (RlRoot, Doc, ElemCree, TypeNoeud)
     Element RlRoot;
     Document Doc;
     Element *ElemCree;
     int TypeNoeud;

#endif /* __STDC__ */

{
  ElementType Elem;

  Elem.ElSSchema = TtaGetSSchema ("Draw3", Doc);
  Elem.ElTypeNum = TypeNoeud;
  *ElemCree = TtaNewTree (Doc, Elem, "");
  if (RlRoot != NULL)
    TtaInsertSibling (*ElemCree, RlRoot, FALSE, Doc);
  return;
}

/*!BF
   ------------------------------------------------------------------------------
   *
   * Function      : GetValAttrEntier
   * Result        : int
   * Parameters
   * Name          Type                 Usage
   * ----          ----                 -----
   * element    Element         l'element dont on veut connaitre la position
   * TtAttribute   int             l'attribut dont on veut connaitre la valeur
   * doc                Document        le document qui contient l'element
   *
   * Functionality : retourne la valeur de l'attribut donne
   *
   ------------------------------------------------------------------------------
   !EF */

#ifdef __STDC__

int 
GetValAttrEntier (Element element, int TtAttribute, Document doc)

#else /* __STDC__ */

int 
GetValAttrEntier (element, TtAttribute, doc)
     Element element;
     int TtAttribute;
     Document doc;

#endif /* __STDC__ */

{
  AttributeType P;
  Attribute A;

  P.AttrSSchema = TtaGetSSchema ("Draw3", doc);
  P.AttrTypeNum = TtAttribute;

  A = TtaGetAttribute (element, P);
  if (A == (int) NULL)
    return (int) NULL;
  else
    return (TtaGetAttributeValue (A));
}

/*!BF
   ------------------------------------------------------------------------------
   *
   * Function      : ExistAttribute
   * Result        : Attribute
   * Parameters
   * Name          Type                 Usage
   * ----          ----                 -----
   * element    Element         l'element concerne
   * TtAttribute   int             l'attribut dont on veut connaitre l'existence
   * doc                Document        le document contenant element
   *
   * Functionality : Retourne l'attribut si il existe, NULL sinon
   *
   ------------------------------------------------------------------------------
   !EF */

#ifdef __STDC__

Attribute 
ExistAttribute (Element element, int TtAttribute, Document doc)

#else /* __STDC__ */

Attribute 
ExistAttribute (element, TtAttribute, doc)
     Element element;
     int TtAttribute;
     Document doc;

#endif /* __STDC__ */

{
  AttributeType P;

  P.AttrSSchema = TtaGetSSchema ("Draw3", doc);
  P.AttrTypeNum = TtAttribute;

  return (TtaGetAttribute (element, P));
}

/*!BF
   ------------------------------------------------------------------------------
   *
   * Function      : SetAttrEntier
   * Result        : void
   * Parameters
   * Name          Type                 Usage
   * ----          ----                 -----
   * element    Element         l'element concerne
   * AttributName       int             TtAttribute a modifier
   * valeur     int             Valeur a affecter a l'attribut
   * doc                Document        le document contenant element
   *
   * Functionality : change la valeur de l'attribut passe en parametre. Le cree si
   * il n'existe pas deja.
   *
   ------------------------------------------------------------------------------
   !EF */

#ifdef __STDC__

void 
SetAttrEntier (Element element, int AttributName, int valeur,
	       Document doc)

#else /* __STDC__ */

void 
SetAttrEntier (element, AttributName, valeur, doc)
     Element element;
     int AttributName;
     int valeur;
     Document doc;

#endif /* __STDC__ */

{
  AttributeType PH;
  Attribute TtAttribute;

  PH.AttrSSchema = TtaGetSSchema ("Draw3", doc);
  PH.AttrTypeNum = AttributName;
  TtAttribute = TtaGetAttribute (element, PH);
  if (TtAttribute == NULL)
    CreateAnAttribute (element, AttributName, valeur, doc);
  else
    TtaSetAttributeValue (TtAttribute, valeur, element, doc);
  return;
}

/*!BF
   ------------------------------------------------------------------------------
   *
   * Function      : GetPRule
   * Result        : int
   * Parameters
   * Name          Type                 Usage
   * ----          ----                 -----
   * elem               Element         l'element concerne
   * rule               int             La PRule dont on veut la valeur
   *
   * Functionality : Retourne la valeur d'une PRule ou 0 si elle n'existe pas
   *
   ------------------------------------------------------------------------------
   !EF */

#ifdef __STDC__

int 
GetPRule (Element elem, int rule)

#else /* __STDC__ */

int 
GetPRule (elem, rule)
     Element elem;
     int rule;

#endif /* __STDC__ */

{
  PRule prule;

  prule = TtaGetPRule (elem, rule);
  if (prule == NULL)
    return (0);
  return (TtaGetPRuleValue (prule));
}

/*!BF
   ------------------------------------------------------------------------------
   *
   * Function      : SetPRule
   * Result        : void
   * Parameters
   * Name          Type                 Usage
   * ----          ----                 -----
   * elem               Element         l'element concerne
   * rule               int             La PRule dont on veut modifier la valeur
   * value              int             Valeur a affecter a la PRule
   * doc                Document        Document contenant l'element
   *
   * Functionality : Changela valeur d'une PRule. La cree si elle n'existe pas.
   *
   ------------------------------------------------------------------------------
   !EF */

#ifdef __STDC__

void 
SetPRule (Element elem, int rule, int value, Document doc)

#else /* __STDC__ */

void 
SetPRule (elem, rule, value, doc)
     Element elem;
     int rule;
     int value;
     Document doc;

#endif /* __STDC__ */

{
  PRule prule;
  View VuePrincipale;
  Document Doc;

  prule = TtaGetPRule (elem, rule);
  if (prule != NULL)
    {
      TtaSetPRuleValue (elem, prule, value, doc);
      return;
    }
  TtaGiveActiveView(&Doc,&VuePrincipale);
  /*   VuePrincipale = TtaGetViewFromName (doc, "The_draw"); */
  prule = TtaNewPRule (rule, VuePrincipale, doc);
  TtaAttachPRule (elem, prule, doc);
  TtaSetPRuleValue (elem, prule, value, doc);
  return;
}

/*!BF
   ------------------------------------------------------------------------------
   *
   * Function      : ChercherContexteInsert
   * Result        : Element
   * Parameters
   * Name          Type                 Usage
   * ----          ----                 -----
   * Doc                Document        Le document dans lequel on cherche le contexte
   *
   * Functionality : Retourne un element avant lequel on pourra inserer l'element
   * cree ou copie a partir de la palette
   *
   ------------------------------------------------------------------------------
   !EF */

#ifdef __STDC__

Element 
ChercherContexteInsert (Document Doc, boolean *inside)

#else /* __STDC__ */

Element 
ChercherContexteInsert (Doc,inside)
     Document Doc;
     boolean *inside;
#endif /* __STDC__ */

{
  Element RlRoot,elem;
  int X, Y;
 
  /* si les icones sont descatives : on n'insere pas */
  if (!DrawTabIDoc[(int)Doc])
    return (NULL);

/* On cherche ou inserer :
   - soit dans le groupe selectionne
   - soit en dernier fils de la racine
 */
  *inside = FALSE;
  RlRoot = CurrentSelect;
  while ((RlRoot != NULL) &&
	 (TtaGetElementType (RlRoot).ElTypeNum != Draw3_EL_Groupe) &&
	 (TtaGetElementType (RlRoot).ElTypeNum != Draw3_EL_Draw3))
    RlRoot = TtaGetParent (RlRoot);

  if (RlRoot != NULL)
    {
      elem = TtaGetLastChild (RlRoot);
      if(elem ==NULL) {
	elem = RlRoot;
	*inside = TRUE;
      }
      return (elem);
    }

  TtaGiveFirstSelectedElement (Doc, &RlRoot, &X, &Y);
  while ((RlRoot != NULL) &&
	 (TtaGetElementType (RlRoot).ElTypeNum != Draw3_EL_Groupe) &&
	 (TtaGetElementType (RlRoot).ElTypeNum != Draw3_EL_Draw3))
    RlRoot = TtaGetParent (RlRoot);

  if (RlRoot != NULL)
    {
      elem = TtaGetLastChild (RlRoot);
      if(elem ==NULL) {
	elem = RlRoot;
	*inside = TRUE;
      }
      return (elem);
    }
  return (NULL);
}
