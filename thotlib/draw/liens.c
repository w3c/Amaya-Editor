
/*!BM
   ------------------------------------------------------------------------------
   *
   * Module      : liens.c
   * Language    : C
   * Authors     : Christian Lenne
   * Version     : %I%
   * Creation    : %E% %U%
   * Description : 
   *
   * Defined variables and functions :
   *
   *        <---------  VARIABLES  --------->
   *
   *                    int SaveCoinX, SaveCoinY, SaveHauteur, SaveLargeur
   *
   *           <---------  L O C A L  --------->
   *
   *                    RotationHorizPolyligne
   *                    RotationVertPolyligne
   *                    TracerFleche
   *                    TracerLienSimple
   *                    CalculerFleche
   *
   *           <------- E X P O R T E D ------->
   *
   *                    DetruirePolygone
   *                    TracerLienComposite
   *                    TracerOrigineLien
   *                    TracerDestinationLien
   *                    AjusterBoite
   *                    PositionnerFleche
   *                    PositionnerLien
   *                    RePositionnerLien
   *                    SauverCoordonnees
   *                    DessinerFleche
   *
   ------------------------------------------------------------------------------
   !EM */

#define Xliens

/****************************************************************************/
/*--------------------------- SYSTEM INCLUDES ------------------------------*/
/****************************************************************************/

/****************************************************************************/
/*------------------------ APPLICATION  INCLUDES ---------------------------*/
/****************************************************************************/

#include "Draw3.h"
#include "application.h"
#include "document.h"
#include "attribute.h"
#include "language.h"
#include "content.h"
#include "reference.h"
#include "tree.h"
#include "app.h"
#include "typebase.h"

#include "liens.h"
#include "utilitaires.h"

/****************************************************************************/
/*----------------------- LOCAL DEFINES AND TYPES --------------------------*/
/****************************************************************************/

#define ABS(x)		(((x)<0)?(-(x)):(x))
#define VERS_LE_BAS	1
#define VERS_LA_DROITE	2
#define ROTATION_H 	4
#define ROTATION_V	8

/****************************************************************************/
/*--------------------------- LOCAL VARIABLES ------------------------------*/
/****************************************************************************/

int SaveCoinX, SaveCoinY, SaveHauteur, SaveLargeur;

/****************************************************************************/
/*-------------------------- GLOBAL VARIABLES ------------------------------*/
/****************************************************************************/

extern int GrilleX, GrilleY;

/****************************************************************************/
/*-------------------------- FORWARD FUNCTIONS -----------------------------*/
/****************************************************************************/

#ifdef __STDC__

void AjusterBoite (Element, Document);
void DetruirePolygone (Element Fleche, Document Doc);

#else

void AjusterBoite ( /* Element, Document */ );
void DetruirePolygone ( /* Element Fleche, Document Doc */ );

#endif /* __STDC__ */

/****************************************************************************/
/*--------------------------- LOCAL FUNCTIONS ------------------------------*/
/****************************************************************************/

/*!BF
   ------------------------------------------------------------------------------
   *
   * Function      : RotationHorizPolyligne
   * Result        : void 
   * Parameters
   * Name          Type                 Usage
   * ----          ----                 -----
   * Polyligne  Element         Polyligne a inverser suivant l'axe horizontal
   * Doc                Document        Document de rattachement
   *
   * Functionality : ffectue une rotation suivant l'axe horizontal;
   *
   ------------------------------------------------------------------------------
   !EF */

#ifdef __STDC__

static void 
RotationHorizPolyligne (Element Polyligne, Document Doc,
			int h, int l)

#else /* __STDC__ */

static void 
RotationHorizPolyligne (Polyligne, Doc, h, l)
     Element Polyligne;
     Document Doc;
     int h, l;

#endif /* __STDC__ */

{
  int i, x, y;

  i = TtaGetPolylineLength (Polyligne);
  i++;
  while (i > 1)
    {
      TtaGivePolylinePoint (Polyligne, i, UnPixel, &x, &y);
      TtaModifyPointInPolyline (Polyligne, i - 1, UnPixel, x, h - y, Doc);
      i--;
    }
  TtaChangeLimitOfPolyline (Polyligne, UnPixel, l, h, Doc);
}

/*!BF
   ------------------------------------------------------------------------------
   *
   * Function      : RotationVertPolyligne
   * Result        : void 
   * Parameters
   * Name          Type                 Usage
   * ----          ----                 -----
   * Polyligne  Element         Polyligne a inverser suivant l'axe horizontal
   * Doc                Document        Document de rattachement
   *
   * Functionality : ffectue une rotation suivant l'axe horizontal;
   *
   ------------------------------------------------------------------------------
   !EF */

#ifdef __STDC__

static void 
RotationVertPolyligne (Element Polyligne, Document Doc,
		       int h, int l)

#else /* __STDC__ */

static void 
RotationVertPolyligne (Polyligne, Doc, h, l)
     Element Polyligne;
     Document Doc;
     int h, l;

#endif /* __STDC__ */

{
  int i, x, y;

  i = TtaGetPolylineLength (Polyligne);

  /* ATTENTION  BIDOUILLE ATTENTION  BIDOUILLE

     On incremente car la fct TtaGivePolylinePoint n'est pas
     homogene avec les autres fonctions de gestion des polylignes.
     Le premier couple de valeur correspond a la coordonnee du coin
     bas a droite
     ATTENTION  BIDOUILLE ATTENTION  BIDOUILLE */

  i++;
  while (i > 1)
    {
      TtaGivePolylinePoint (Polyligne, i, UnPixel, &x, &y);

      /* ATTENTION  BIDOUILLE ATTENTION  BIDOUILLE
         Voir commentaire precedent expliquant le -1
       */

      TtaModifyPointInPolyline (Polyligne, i - 1, UnPixel, l - x, y, Doc);
      i--;
    }
  TtaChangeLimitOfPolyline (Polyligne, UnPixel, l, h, Doc);
}

/*!BF
   ------------------------------------------------------------------------------
   *
   * Function      : TracerFleche
   * Result        : void
   * Parameters
   * Name          Type                 Usage
   * ----          ----                 -----
   * Fleche     *NotifyElement  Noeud fleche
   * Type               int             Type de fleche (1) | (2) -> (3) -|
   * PosX               int             Coordonnee X
   * PosY               int             Coordonnee Y
   * Hauteur    int             Hauteur Box
   * Largeur    int             Largeur Box
   *
   * Functionality : Cree le polygone associe au type de fleche creuse.
   *
   ------------------------------------------------------------------------------
   !EF */

#ifdef __STDC__

static void 
TracerFleche (NotifyElement * Fleche, int Type, int Taille,
	      int PosX, int PosY,
	      int Hauteur, int Largeur)

#else /* __STDC__ */

static void 
TracerFleche (Fleche, Type, Taille, PosX, PosY, Hauteur, Largeur)
     NotifyElement *Fleche;
     int Type, Taille, PosX, PosY, Hauteur, Largeur;

#endif /* __STDC__ */

{
  Element GraphicNode;
  int HSave, LSave;
  int UnTier, Demi, DeuxTiers;

  HSave = Hauteur;
  LSave = Largeur;
  if (Hauteur < 0)
    Hauteur = Hauteur * -1;
  if (Largeur < 0)
    Largeur = Largeur * -1;
  GraphicNode = TtaGetFirstChild (Fleche->element);
  if (TtaGetGraphicsShape (GraphicNode) != 'p')
    TtaSetGraphicsShape (GraphicNode, 'p', Fleche->document);
  DetruirePolygone (GraphicNode, Fleche->document);

  TtaChangeLimitOfPolyline (GraphicNode, UnPixel, Largeur, Hauteur, Fleche->document);

  Demi = Taille;
  Taille = Taille * 2;
  UnTier = Taille / 3;
  DeuxTiers = UnTier * 2;

  switch (Type & (VERS_LA_DROITE | VERS_LE_BAS))
    {
    case VERS_LE_BAS:
      TtaAddPointInPolyline (GraphicNode, 1, UnPixel, Largeur / 4, 0,
			     Fleche->document);
      TtaAddPointInPolyline (GraphicNode, 2, UnPixel, Largeur / 4,
			     Hauteur - Largeur, Fleche->document);
      TtaAddPointInPolyline (GraphicNode, 3, UnPixel, 0,
			     Hauteur - Largeur, Fleche->document);
      TtaAddPointInPolyline (GraphicNode, 4, UnPixel, Largeur / 2,
			     Hauteur, Fleche->document);
      TtaAddPointInPolyline (GraphicNode, 5, UnPixel, Largeur,
			     Hauteur - Largeur, Fleche->document);
      TtaAddPointInPolyline (GraphicNode, 6, UnPixel, (Largeur * 3) / 4,
			     Hauteur - Largeur, Fleche->document);
      TtaAddPointInPolyline (GraphicNode, 7, UnPixel, (Largeur * 3) / 4, 0,
			     Fleche->document);
      break;
    case VERS_LA_DROITE:
      TtaAddPointInPolyline (GraphicNode, 1, UnPixel, 0, Hauteur / 4,
			     Fleche->document);
      TtaAddPointInPolyline (GraphicNode, 2, UnPixel, 0, (Hauteur * 3) / 4,
			     Fleche->document);
      TtaAddPointInPolyline (GraphicNode, 3, UnPixel, Largeur - Hauteur,
			     (Hauteur * 3) / 4, Fleche->document);
      TtaAddPointInPolyline (GraphicNode, 4, UnPixel, Largeur - Hauteur,
			     Hauteur, Fleche->document);
      TtaAddPointInPolyline (GraphicNode, 5, UnPixel, Largeur, Hauteur / 2,
			     Fleche->document);
      TtaAddPointInPolyline (GraphicNode, 6, UnPixel, Largeur - Hauteur, 0,
			     Fleche->document);
      TtaAddPointInPolyline (GraphicNode, 7, UnPixel, Largeur - Hauteur,
			     Hauteur / 4, Fleche->document);
      break;
    case VERS_LE_BAS | VERS_LA_DROITE:
      TtaAddPointInPolyline (GraphicNode, 1, UnPixel, 0, 0,
			     Fleche->document);
      TtaAddPointInPolyline (GraphicNode, 2, UnPixel, 0,
			     UnTier, Fleche->document);
      TtaAddPointInPolyline (GraphicNode, 3, UnPixel, Largeur - DeuxTiers,
			     UnTier, Fleche->document);
      TtaAddPointInPolyline (GraphicNode, 4, UnPixel, Largeur - DeuxTiers,
			     Hauteur - Taille, Fleche->document);
      TtaAddPointInPolyline (GraphicNode, 5, UnPixel, Largeur - Taille,
			     Hauteur - Taille, Fleche->document);
      TtaAddPointInPolyline (GraphicNode, 6, UnPixel, Largeur - Demi, Hauteur,
			     Fleche->document);
      TtaAddPointInPolyline (GraphicNode, 7, UnPixel, Largeur,
			     Hauteur - Taille, Fleche->document);
      TtaAddPointInPolyline (GraphicNode, 8, UnPixel, Largeur - UnTier,
			     Hauteur - Taille, Fleche->document);
      TtaAddPointInPolyline (GraphicNode, 9, UnPixel, Largeur - UnTier, 0,
			     Fleche->document);
      break;
    }

  switch (Type & (ROTATION_H + ROTATION_V))
    {
    case ROTATION_H:
      RotationHorizPolyligne (GraphicNode, Fleche->document,
			      Hauteur, Largeur);
      break;
    case ROTATION_V:
      RotationVertPolyligne (GraphicNode, Fleche->document,
			     Hauteur, Largeur);
      break;
    case (ROTATION_H + ROTATION_V):
      RotationHorizPolyligne (GraphicNode, Fleche->document,
			      Hauteur, Largeur);
      RotationVertPolyligne (GraphicNode, Fleche->document,
			     Hauteur, Largeur);
      break;
    }

  RemoveAttr (Fleche->element, Draw3_ATTR_Largeur, Fleche->document);
  SetAttrPosition (Fleche->element, PosX, PosY, Fleche->document);
  SetAttrTaille (Fleche->element, HSave, LSave, Fleche->document);
}

/*!BF
   ------------------------------------------------------------------------------
   *
   * Function      : TracerLienSimple
   * Result        : void
   * Parameters
   * Name          Type                 Usage
   * ----          ----                 -----
   * Fleche     *NotifyElement  Noeud fleche
   * Type               int             Type de fleche (1) | (2) -> (3) -|
   * PosX               int             Coordonnee X
   * PosY               int             Coordonnee Y
   * Hauteur    int             Hauteur Box
   * Largeur    int             Largeur Box
   *
   * Functionality : Cree le polygone associe au type de fleche creuse.
   *
   ------------------------------------------------------------------------------
   !EF */

#ifdef __STDC__

static void 
TracerLienSimple (NotifyElement * Fleche, int Type, int Taille,
		  int PosX, int PosY,
		  int Hauteur, int Largeur)

#else /* __STDC__ */

static void 
TracerLienSimple (Fleche, Type, Taille, PosX, PosY, Hauteur,
		  Largeur)
     NotifyElement *Fleche;
     int Type, Taille, PosX, PosY, Hauteur, Largeur;

#endif /* __STDC__ */

{
  Element GraphicNode;
  int PtX, PtY, NbPoint;
  char TypeFleche;

  if (Hauteur < 0)
    Hauteur = Hauteur * -1;
  if (Largeur < 0)
    Largeur = Largeur * -1;
  GraphicNode = TtaGetFirstChild (Fleche->element);
  TypeFleche = TtaGetGraphicsShape (GraphicNode);
  if (TypeFleche == 'U')
    {
      NbPoint = TtaGetPolylineLength (GraphicNode);
      if (NbPoint > 2)
	TtaGivePolylinePoint (GraphicNode, 2, UnPixel, &PtX, &PtY);
      DetruirePolygone (GraphicNode, Fleche->document);
    }
  else
    NbPoint = PtX = PtY = 0;

  TtaSetGraphicsShape (GraphicNode, 'U', Fleche->document);
  TtaAddPointInPolyline (GraphicNode, 1, UnPixel, 0, 0, Fleche->document);
  TtaAddPointInPolyline (GraphicNode, 2, UnPixel, Largeur / 2, Hauteur / 2,
			 Fleche->document);
  TtaAddPointInPolyline (GraphicNode, 3, UnPixel, Largeur, Hauteur, Fleche->document);

  switch (Type & (ROTATION_H + ROTATION_V))
    {
    case ROTATION_H:
      RotationHorizPolyligne (GraphicNode, Fleche->document,
			      Hauteur, Largeur);
      break;
    case ROTATION_V:
      RotationVertPolyligne (GraphicNode, Fleche->document,
			     Hauteur, Largeur);
      break;
    case (ROTATION_H | ROTATION_V):
      RotationHorizPolyligne (GraphicNode, Fleche->document,
			      Hauteur, Largeur);
      RotationVertPolyligne (GraphicNode, Fleche->document,
			     Hauteur, Largeur);
      break;
    }

  TtaChangeLimitOfPolyline (GraphicNode, UnPixel, Largeur, Hauteur, Fleche->document);
  RemoveAttr (Fleche->element, Draw3_ATTR_Largeur, Fleche->document);
  SetAttrPosition (Fleche->element, PosX, PosY, Fleche->document);
  SetAttrTaille (Fleche->element, Hauteur, Largeur, Fleche->document);

#ifdef TRACE
  printf ("=================================\n");
  TtaListAbstractTree (Fleche->element, stdout);
  printf ("=================================\n");
#endif /* TRACE */
  return;
}

/*!BF
   ------------------------------------------------------------------------------
   *
   * Function      : CalculerFleche
   * Result        : void
   * Parameters
   * Name          Type                 Usage
   * ----          ----                 -----
   * Fleche     *NotifyElement  Noeud FlecheCreuse
   * AncreOrig  Element         Polyligne de depart
   * AncreDest  Element         Polyligne destination
   *
   * Functionality : Calcule la position et la forme optimale de la fleche,
   * compte tenu de la position des boites et des ancres.
   *
   ------------------------------------------------------------------------------
   !EF */

#ifdef __STDC__

static void 
CalculerFleche (NotifyElement * Fleche, char TypeLien,
		Element AncreOrig, Element AncreDest)

#else /* __STDC__ */

static void 
CalculerFleche (Fleche, TypeLien, AncreOrig, AncreDest)

     NotifyElement *Fleche;
     char TypeLien;
     Element AncreOrig, AncreDest;


#endif /* __STDC__ */

{
  Document doc;
  Element NoeudOrig, NoeudDest, Temp;
  View VuePrincipale;
  int CentreXO, CentreXD, CentreYO, CentreYD;
  int CoinXO, CoinXD, CoinYO, CoinYD;
  int HO, HD, LO, LD;
  int FlecheX, FlecheY, FlecheH, FlecheL;
  int Epaisseur, Type;
  int AHO, AVO, AHD, AVD;

  enum
    {
      ADroite, AGauche, UnPeuADroite, UnPeuAGauche
    }
  PositionHoriz;
  enum
    {
      EnDessus, EnDessous, UnPeuEnDessus, UnPeuEnDessous
    }
  PositionVert;

  NoeudOrig = AncreOrig;
  NoeudDest = AncreDest;
  TtaGiveActiveView(&doc,&VuePrincipale);
/*   VuePrincipale = TtaGetViewFromName (Fleche->document, "The_draw"); */

  GetValAttrPosition (NoeudOrig, &CoinXO, &CoinYO, Fleche->document);
  GetValAttrPosition (NoeudDest, &CoinXD, &CoinYD, Fleche->document);

  /*
     TtaGiveBoxPosition (NoeudOrig, VuePrincipale, UnPixel, &CoinXO, &CoinYO);
     TtaGiveBoxPosition (NoeudDest, VuePrincipale, UnPixel, &CoinXD, &CoinYD);
   */

  /* calcul les coordonees relatives a la boite Draw3 englobante */
  /* des NoeudOrig et NoeudDest */
  if (TtaGetParent (NoeudOrig) != TtaGetParent (NoeudDest))
    {
      Temp = TtaGetParent (NoeudOrig);
      while (TtaGetElementType (Temp).ElTypeNum == Draw3_EL_Groupe)
	{
	  GetValAttrPosition (Temp, &AHO, &AVO, Fleche->document);
	  /*    TtaGiveBoxPosition (Temp, VuePrincipale, UnPixel, &AHO, &AVO); */
	  CoinXO += AHO;
	  CoinYO += AVO;
	  Temp = TtaGetParent (Temp);
	}

      Temp = TtaGetParent (NoeudDest);
      while (TtaGetElementType (Temp).ElTypeNum == Draw3_EL_Groupe)
	{
	  GetValAttrPosition (Temp, &AHO, &AVO, Fleche->document);
	  /*    TtaGiveBoxPosition (Temp, VuePrincipale, UnPixel, &AHO, &AVO); */
	  CoinXD += AHO;
	  CoinYD += AVO;
	  Temp = TtaGetParent (Temp);
	}
      AHO = AVO = 0;
    }
  GetValAttrTaille (NoeudOrig, &HO, &LO, Fleche->document);
  GetValAttrTaille (NoeudDest, &HD, &LD, Fleche->document);

  /*
     TtaGiveBoxSize (NoeudOrig, VuePrincipale, UnPixel, &HO, &LO);
     TtaGiveBoxSize (NoeudDest, VuePrincipale, UnPixel, &HD, &LD);
   */

  CentreXO = CoinXO + (LO / 2);
  CentreYO = CoinYO + (HO / 2);
  CentreXD = CoinXD + (LD / 2);
  CentreYD = CoinYD + (HD / 2);

  Epaisseur = GetValAttrEntier (Fleche->element, Draw3_ATTR_Epaisseur,
				Fleche->document);
  Epaisseur = Epaisseur * 2;
  Type = 0;
  if (CentreXO < CentreXD)
    if ((CoinXO + LO + 40) > CentreXD)
      {
	PositionHoriz = UnPeuADroite;
	Type |= VERS_LE_BAS;
	if (TypeLien == 'c')
	  {
	    FlecheX = (CentreXO + CentreXD - Epaisseur) / 2;
	    FlecheL = Epaisseur;
	  }
	else
	  {
	    Type |= VERS_LA_DROITE;
	    FlecheX = CentreXO;
	    FlecheL = CentreXD - FlecheX;
	  }
      }
    else
      {
	PositionHoriz = ADroite;
	Type |= VERS_LA_DROITE;
	if (TypeLien == 'c')
	  {
	    FlecheX = CoinXO + LO;
	    FlecheL = CoinXD - FlecheX;
	  }
	else
	  {
	    FlecheX = CoinXO + LO;
	    FlecheL = CoinXD - FlecheX;
	  }
      }
  else if ((CoinXD + LD + Epaisseur + 4) > CentreXO)
    {
      PositionHoriz = UnPeuAGauche;
      Type = VERS_LE_BAS;
      if (TypeLien == 'c')
	{
	  FlecheX = (CentreXO + CentreXD - Epaisseur) / 2;
	  FlecheL = Epaisseur;
	}
      else
	{
	  FlecheX = CentreXD;
	  FlecheL = CentreXO - FlecheX;
	  Type |= ROTATION_V;
	}
    }
  else
    {
      PositionHoriz = AGauche;
      Type |= ROTATION_V;
      if (TypeLien == 'c')
	{
	  Type |= VERS_LA_DROITE;
	  FlecheX = CoinXD + LD;
	  FlecheL = CoinXO - FlecheX;
	}
      else
	{
	  FlecheX = CoinXD + LD;
	  FlecheL = CoinXO - FlecheX;
	}
    }

  if (CentreYO < CentreYD)
    if ((CoinYO + HO + Epaisseur + 4) > CentreYD)
      {
	PositionVert = UnPeuEnDessous;
	Type |= VERS_LA_DROITE;
	if (TypeLien == 'c')
	  {
	    FlecheY = (CentreYO + CentreYD - Epaisseur) / 2;
	    FlecheH = Epaisseur;
	  }
	else
	  {
	    FlecheY = CentreYO;
	    FlecheH = ABS (CentreYD - FlecheY);
	  }
      }
    else
      {
	PositionVert = EnDessous;
	if (Type & VERS_LE_BAS)	/* UnPeuAGauche ou a Droite */
	  {
	    if (TypeLien == 'c')
	      {
		FlecheY = CoinYO + HO;
		FlecheH = CoinYD - FlecheY;
	      }
	    else
	      {
		FlecheY = CoinYO + HO;
		FlecheH = CoinYD - FlecheY;
	      }
	  }
	else
	  {
	    if (PositionHoriz == ADroite)
	      {
		if (TypeLien == 'c')
		  {
		    Type |= VERS_LE_BAS;
		    FlecheY = CoinYO + (HO / 2);
		    FlecheH = CoinYD - FlecheY;
		    FlecheL = CentreXD + (Epaisseur / 2)
		      - FlecheX;
		  }
		else
		  {
		    FlecheX = CentreXO;
		    FlecheY = CoinYO + HO;
		    FlecheH = CoinYD - FlecheY;
		    FlecheL = CentreXD - FlecheX;
		  }
	      }
	    else
	      {
		Type |= VERS_LE_BAS;
		if (TypeLien == 'c')
		  {
		    FlecheY = CentreYO;
		    FlecheH = CoinYD - FlecheY;
		    FlecheX = CentreXD - (Epaisseur / 2);
		    FlecheL = CoinXO - FlecheX;
		  }
		else
		  {
		    FlecheY = CoinYO + HO;
		    FlecheH = CoinYD - FlecheY;
		    FlecheX = CentreXD;
		    FlecheL = CentreXO - CentreXD;
		  }
	      }
	  }
      }
  else if ((CoinYD + HD + Epaisseur + 4) > CentreYO)
    {
      PositionVert = UnPeuEnDessus;
      Type |= VERS_LA_DROITE;
      if (TypeLien == 'c')
	{
	  FlecheY = (CentreYO + CentreYD - Epaisseur) / 2;
	  FlecheH = Epaisseur;
	}
      else
	{
	  Type |= ROTATION_H;
	  if (PositionHoriz == AGauche)
	    Type |= ROTATION_V;
	  FlecheY = CentreYD;
	  FlecheH = ABS (FlecheY - CentreYO);
	}
    }
  else
    {
      PositionVert = EnDessus;
      FlecheY = CoinYD + HD;
      if (Type & VERS_LE_BAS)	/* UnPeuAGauche ou a Droite */
	{
	  Type |= ROTATION_H;
	  if (TypeLien == 'c')
	    {
	      FlecheY = CoinYD + HD;
	      FlecheH = CoinYO - FlecheY;
	    }
	  else
	    {
	      FlecheY = CoinYD + HD;
	      FlecheH = CoinYO - FlecheY;
	    }
	}
      else
	{
	  Type |= ROTATION_H;
	  if (PositionHoriz == ADroite)
	    {
	      Type |= VERS_LE_BAS;
	      FlecheH = CentreYO - FlecheY;
	      FlecheL = CentreXD - FlecheX + (Epaisseur / 2);
	    }
	  else
	    {
	      Type |= VERS_LE_BAS;
	      if (TypeLien == 'c')
		{
		  FlecheY = CoinYD + HD;
		  FlecheH = CentreYO - FlecheY;
		  FlecheX = CentreXD - (Epaisseur / 2);
		  FlecheL = CoinXO - FlecheX;
		}
	      else
		{
		  FlecheX = CentreXD;
		  FlecheY = CoinYD + HD;
		  FlecheH = CoinYO - FlecheY;
		  FlecheL = CentreXO - FlecheX;
		}
	    }
	}
    }

  GetValAttrAncre (Fleche->element, &AHO, &AVO, &AHD, &AVD,
		   Fleche->document);

  SetAttrPosition (Fleche->element, FlecheX, FlecheY,
		   Fleche->document);
  SetAttrTaille (Fleche->element, FlecheH, FlecheL,
		 Fleche->document);
  if (TypeLien == 'c')
    TracerFleche (Fleche, Type, Epaisseur / 2,
		  FlecheX + AHO, FlecheY + AVO,
		  FlecheH + AVD, FlecheL + AHD);
  else
    TracerLienSimple (Fleche, Type, Epaisseur / 2,
		      FlecheX + AHO, FlecheY + AVO,
		      FlecheH + AVD, FlecheL + AHD);


  return;
}

/****************************************************************************/
/*------------------------- EXPORTED  FUNCTIONS ----------------------------*/
/****************************************************************************/

/*!BF
   ------------------------------------------------------------------------------
   *
   * Function      : DetruirePolygone
   * Result        : void
   * Parameters
   * Name          Type          Usage
   * ----          ----          -----
   * Fleche     Element       Polygone pour lequel on veut detruire les points
   * Doc                Document      Document contenant le polygone
   *
   * Functionality : DeleteElement tous les points du polygone en vue d'une mise a jour
   *
   ------------------------------------------------------------------------------
   !EF */

#ifdef __STDC__

void 
DetruirePolygone (Element Fleche, Document Doc)

#else /* __STDC__ */

void 
DetruirePolygone (Fleche, Doc)
     Element Fleche;
     Document Doc;

#endif /* __STDC__ */

{
  int i;

  i = TtaGetPolylineLength (Fleche);

  /* ATTENTION  BIDOUILLE ATTENTION  BIDOUILLE
     On incremente pour unifier les parametres de designation de point qui
     ne sont pas identiques dans les differentes fct sur les polylignes
   */
  /* if (i) i++; */
  while ( /* i>1 */ i >= 1)
    {
      TtaDeletePointInPolyline (Fleche, i, Doc);
      i--;
    }
}

/*!BF
   ------------------------------------------------------------------------------
   *
   * Function      : TracerLienComposite
   * Result        : void
   * Parameters
   * Name          Type          Usage
   * ----          ----          -----
   * Lien               Element         Lien a tracer
   * Doc                Document        Document qui inclue le lien
   *
   * Functionality : Calcule la taille de la boite devant contenir la partie
   * centrale du lien en fonction des differentes references, et trace un trait
   * horizontal au milieu de de la boite
   *
   ------------------------------------------------------------------------------
   !EF */

#ifdef __STDC__

void 
TracerLienComposite (Element Lien, Document Doc)

#else /* __STDC__ */

void 
TracerLienComposite (Lien, Doc)
     Element Lien;
     Document Doc;

#endif /* __STDC__ */

{
  Element GraphicNode, Elem;
  Attribute Att;
  Document doc,AutreDoc;
  View VuePrincipale;
  int Largeur, Hauteur, PosX, PosY, TempX, TempY, TempH, TempL, X, Y, H,
    L, NBLien;

  GraphicNode = TtaGetFirstChild (Lien);
  AjusterBoite (Lien, Doc);

  X = Y = 99999;
  H = L = 0;
  AutreDoc = 0;
  Att = NULL;
  Elem = NULL;
  AutreDoc = 0;

  TtaGiveActiveView(&doc,&VuePrincipale);
/*   VuePrincipale = TtaGetViewFromName (Doc, "The_draw"); */
  if (!VuePrincipale)
    return;

  /* TtaGiveBoxPosition (Lien, VuePrincipale, UnPixel, &PosX, &PosY); */
  GetValAttrPosition (Lien, &PosX, &PosY, Doc);
  /* TtaGiveBoxSize (Lien, VuePrincipale, UnPixel, &Largeur, &Hauteur); */
  GetValAttrTaille (Lien, &Hauteur, &Largeur, Doc);
  if (Largeur > Hauteur)
    Hauteur = 10;		/* On determine si le trait est */
  else
    Largeur = 10;		/* horizontal ou vertical       */

  if (TtaIsElementReferred (Lien))
    {
      TtaNextLoadedReference (Lien, Doc, &Elem, &Att, &AutreDoc);
      NBLien = 0;
      while (Elem != NULL)
	{
	  if ((TtaGetElementType (Elem).ElTypeNum
	       == Draw3_EL_PartieOrigineLien) ||
	      (TtaGetElementType (Elem).ElTypeNum
	       == Draw3_EL_PartieDestinationLien))
	    {
	      NBLien++;
	      GetValAttrPosition (Elem, &TempX, &TempY, Doc);
	      GetValAttrTaille (Elem, &TempH, &TempL, Doc);
	      if (Hauteur == 10)	/* C'est la Hauteur a determiner */
		{
		  if ((TempX + (TempL / 2)) < X)
		    X = TempX + (TempL / 2);
		  if ((TempX + (TempL / 2)) > L)
		    L = TempX + (TempL / 2);
		}
	      else
		{
		  if ((TempY + (TempH / 2)) < Y)
		    Y = TempY + (TempH / 2);
		  if ((TempY + (TempH / 2)) > H)
		    H = TempY + (TempH / 2);
		}
	    }
	  TtaNextLoadedReference (Lien, Doc, &Elem, &Att, &AutreDoc);
	}
      if (Hauteur == 10)
	{
	  PosX = X + 1;
	  PosY = PosY + 3;
	  Largeur = L - PosX;
	}
      else
	{
	  PosY = Y + 3;
	  PosX = PosX + 1;
	  Hauteur = H - PosY;
	}
      if (NBLien < 2)
	{
	  /*            TtaGiveBoxPosition (Lien, VuePrincipale, UnPixel, &PosX, &PosY); */
	  GetValAttrPosition (Lien, &PosX, &PosY, Doc);
	  /*            TtaGiveBoxSize (Lien, VuePrincipale, UnPixel, &Largeur, &Hauteur); */
	  GetValAttrTaille (Lien, &Hauteur, &Largeur, Doc);
	}
    }
  else
    {
      /*        TtaGiveBoxPosition (Lien, VuePrincipale, UnPixel, &PosX, &PosY); */
      GetValAttrPosition (Lien, &PosX, &PosY, Doc);
      /*        TtaGiveBoxSize (Lien, VuePrincipale, UnPixel, &Largeur, &Hauteur); */
      GetValAttrTaille (Lien, &Hauteur, &Largeur, Doc);
    }

  if (Largeur > Hauteur)
    {
      Hauteur = 10;
      TtaSetGraphicsShape (GraphicNode, 'h', Doc);
    }
  else
    {
      Largeur = 10;
      TtaSetGraphicsShape (GraphicNode, 'v', Doc);
    }

  RemoveAttr (Lien, Draw3_ATTR_Largeur, Doc);
  SetAttrPosition (Lien, PosX - 1, PosY - 3, Doc);	/* +1+3 */
  SetAttrTaille (Lien, Hauteur, Largeur, Doc);

  return;
}

/*!BF
   ------------------------------------------------------------------------------
   *
   * Function      : TracerOrigineLien
   * Result        : void
   * Parameters
   * Name          Type          Usage
   * ----          ----          -----
   * Lien               Element         Lien a tracer
   * Doc                Document        Document qui inclue le lien
   *
   * Functionality : Trace le trait associe au lien origine entre une boite et
   * la partie centrale du lien.
   ------------------------------------------------------------------------------
   !EF */

#ifdef __STDC__

void 
TracerOrigineLien (Element Lien, Document Doc)

#else /* __STDC__ */

void 
TracerOrigineLien (Lien, Doc)
     Element Lien;
     Document Doc;

#endif /* __STDC__ */

{
  Element GraphicNode, Origine, Destination;
  Attribute AttOrigine, AttDestination;
  View VuePrincipale;
  AttributeType AttRef;
  Document TargetDoc = 0;
  char DocName[1];
  int LC_Largeur, LC_Hauteur, LC_PosX, LC_PosY;
  int Largeur, Hauteur, PosX, PosY;

  DocName[0] = '\0';

  /* On regarde dans quel sens est le Lien Composite (Horizontal ou Vertical) */

  AttRef.AttrSSchema = TtaGetSSchema ("Draw3", Doc);
  AttRef.AttrTypeNum = Draw3_ATTR_Origine_lien;
  AttOrigine = TtaGetAttribute (Lien, AttRef);
  AttRef.AttrTypeNum = Draw3_ATTR_Terminaison;
  AttDestination = TtaGetAttribute (Lien, AttRef);

  TtaGiveReferenceAttributeValue (AttOrigine, &Origine, DocName,
				  &TargetDoc);
  TtaGiveReferenceAttributeValue (AttDestination, &Destination, DocName,
				  &TargetDoc);
  TtaGiveActiveView(&TargetDoc,&VuePrincipale);
/*   VuePrincipale = TtaGetViewFromName (Doc, "The_draw"); */

  AjusterBoite (Origine, Doc);

  GetValAttrPosition (Origine, &PosX, &PosY, Doc);
  TtaGiveBoxSize (Origine, Doc, VuePrincipale, UnPixel, &Largeur, &Hauteur);
  GetValAttrPosition (Destination, &LC_PosX, &LC_PosY, Doc);
  TtaGiveBoxSize (Destination, Doc, VuePrincipale, UnPixel, &LC_Largeur, &LC_Hauteur);

  if (LC_Largeur > LC_Hauteur)
    LC_Hauteur = 10;
  else
    LC_Largeur = 10;

  if (LC_Largeur == 10)		/* LienComposite Vertical donc on trace  */
    {				/* un trait Horizontal (perpendiculaire) */
      PosX = PosX + Largeur;
      PosY = PosY + (Hauteur / 2) - 5;	/* -5 car 10 d'epaisseur */
      Hauteur = 10;
      Largeur = LC_PosX + 5 - PosX;
    }
  else
    {
      PosX = PosX + (Largeur / 2) - 5;
      PosY = PosY + Hauteur;
      Largeur = 10;
      Hauteur = LC_PosY + 5 - PosY;
    }

  GraphicNode = TtaGetFirstChild (Lien);
  DetruirePolygone (GraphicNode, Doc);
  TtaSetGraphicsShape (GraphicNode, 'S', Doc);

  if (LC_Largeur == 10)
    {
      TtaAddPointInPolyline (GraphicNode, 1, UnPixel, 0, 5, Doc);
      TtaAddPointInPolyline (GraphicNode, 2, UnPixel, Largeur / 2, 5, Doc);
      TtaAddPointInPolyline (GraphicNode, 3, UnPixel, Largeur, 5, Doc);
    }
  else
    {
      TtaAddPointInPolyline (GraphicNode, 1, UnPixel, 5, 0, Doc);
      TtaAddPointInPolyline (GraphicNode, 2, UnPixel, 5, Hauteur / 2, Doc);
      TtaAddPointInPolyline (GraphicNode, 3, UnPixel, 5, Hauteur, Doc);
    }

  TtaChangeLimitOfPolyline (GraphicNode, UnPixel, Largeur, Hauteur, Doc);
  RemoveAttr (Lien, Draw3_ATTR_Largeur, Doc);
  SetAttrPosition (Lien, PosX, PosY - 1, Doc);	/* +1+3 */
  SetAttrTaille (Lien, Hauteur, Largeur, Doc);
  TracerLienComposite (Destination, Doc);

  return;
}

/*!BF
   ------------------------------------------------------------------------------
   *
   * Function      : TracerDestinationLien
   * Result        : void
   * Parameters
   * Name          Type          Usage
   * ----          ----          -----
   * Lien               Element         Lien a tracer
   * Doc                Document        Document qui inclue le lien
   *
   * Functionality : Trace le trait associe au lien origine entre une boite et
   * la partie centrale du lien.
   ------------------------------------------------------------------------------
   !EF */

#ifdef __STDC__

void 
TracerDestinationLien (Element Lien, Document Doc)

#else /* __STDC__ */

void 
TracerDestinationLien (Lien, Doc)
     Element Lien;
     Document Doc;

#endif /* __STDC__ */

{
  Element GraphicNode, Origine, Destination;
  Attribute AttOrigine, AttDestination;
  View VuePrincipale;
  AttributeType AttRef;
  Document TargetDoc = 0;
  char DocName[1];
  int LC_Largeur, LC_Hauteur, LC_PosX, LC_PosY;
  int Largeur, Hauteur, PosX, PosY;

  DocName[0] = '\0';

  /* On regarde dans quel sens est le Lien Composite (Horizontal ou Vertical) */

  AttRef.AttrSSchema = TtaGetSSchema ("Draw3", Doc);
  AttRef.AttrTypeNum = Draw3_ATTR_Origine;
  AttOrigine = TtaGetAttribute (Lien, AttRef);
  AttRef.AttrTypeNum = Draw3_ATTR_Terminaison_lien;
  AttDestination = TtaGetAttribute (Lien, AttRef);
  TtaGiveReferenceAttributeValue (AttOrigine, &Origine, DocName,
				  &TargetDoc);
  TtaGiveReferenceAttributeValue (AttDestination, &Destination, DocName,
				  &TargetDoc);
  TtaGiveActiveView(&TargetDoc,&VuePrincipale);
/*   VuePrincipale = TtaGetViewFromName (Doc, "The_draw"); */
  /* TtaGiveBoxPosition (Origine, VuePrincipale, UnPixel, &LC_PosX, &LC_PosY); */
  GetValAttrPosition (Origine, &LC_PosX, &LC_PosY, Doc);
  TtaGiveBoxSize (Origine, Doc, VuePrincipale, UnPixel, &LC_Largeur, &LC_Hauteur);

  AjusterBoite (Destination, Doc);
  /* TtaGiveBoxPosition (Destination, VuePrincipale, UnPixel, &PosX, &PosY); */
  GetValAttrPosition (Destination, &PosX, &PosY, Doc);
  TtaGiveBoxSize (Destination, Doc, VuePrincipale, UnPixel, &Largeur, &Hauteur);

  if (LC_Largeur > LC_Hauteur)
    LC_Hauteur = 10;
  else
    LC_Largeur = 10;

  if (LC_Largeur == 10)		/* LienComposite Vertical donc on trace  */
    {				/* un trait Horizontal (perpendiculaire) */
      LC_PosX = LC_PosX + 5;
      PosY = PosY + (Hauteur / 2) - 5;	/* -5 car 10 d'epaisseur */
      Hauteur = 10;
      Largeur = PosX + 5 - LC_PosX;
      PosX = LC_PosX;
    }
  else
    {
      PosX = PosX + (Largeur / 2) - 5;
      LC_PosY = LC_PosY + 5;
      Largeur = 10;
      Hauteur = PosY + 5 - LC_PosY;
      PosY = LC_PosY;
    }

  GraphicNode = TtaGetFirstChild (Lien);
  DetruirePolygone (GraphicNode, Doc);
  TtaSetGraphicsShape (GraphicNode, 'S', Doc);

  if (LC_Largeur == 10)
    {
      TtaAddPointInPolyline (GraphicNode, 1, UnPixel, 0, 5, Doc);
      TtaAddPointInPolyline (GraphicNode, 2, UnPixel, Largeur / 2, 5, Doc);
      TtaAddPointInPolyline (GraphicNode, 3, UnPixel, Largeur, 5, Doc);
    }
  else
    {
      TtaAddPointInPolyline (GraphicNode, 1, UnPixel, 5, 0, Doc);
      TtaAddPointInPolyline (GraphicNode, 2, UnPixel, 5, Hauteur / 2, Doc);
      TtaAddPointInPolyline (GraphicNode, 3, UnPixel, 5, Hauteur, Doc);
    }

  TtaChangeLimitOfPolyline (GraphicNode, UnPixel, Largeur, Hauteur, Doc);

  RemoveAttr (Lien, Draw3_ATTR_Largeur, Doc);
  SetAttrPosition (Lien, PosX, PosY - 1, Doc);	/* +1+3 */
  SetAttrTaille (Lien, Hauteur, Largeur, Doc);
  TracerLienComposite (Origine, Doc);

  return;
}


/*!BF
   ------------------------------------------------------------------------------
   *
   * Function      : AjusterBoitePos
   * Result        : void
   * Parameters
   * Name          Type          Usage
   * ----          ----          -----
   * Objet              Element         Noeud a repositionner et calculer
   * Doc                Document        Document de rattachement
   *
   * Functionality : Repositionne la boite de l'objet sur la grille.
   *
   ------------------------------------------------------------------------------
   !EF */

#ifdef __STDC__

void 
AjusterBoitePos (Element Objet, Document Doc)

#else /* __STDC__ */

void 
AjusterBoitePos (Objet, Doc)
     Element Objet;
     Document Doc;

#endif /* __STDC__ */

{
  Document doc;
  View VuePrincipale;
  int PosX, PosY, X, Y;

  TtaGiveActiveView(&doc,&VuePrincipale);
  
  if (!VuePrincipale)
    return;
  
  GetValAttrPosition(Objet,  &PosX, &PosY, Doc);
  TtaGiveBoxPosition (Objet, Doc, VuePrincipale, UnPixel, &X, &Y);

  PosX = (PosX / GrilleX) * GrilleX;
  PosY = (PosY / GrilleY) * GrilleY;

  if((X-PosX != 0) || (Y-PosY != 0))
    SetAttrPosition (Objet, PosX, PosY, Doc);

#ifdef TRACE
  printf ("+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+\n");
 TtaGiveBoxPosition (Objet, Doc, VuePrincipale, UnPixel, &PosX, &PosY);
  printf ("Position apres mise a jour des attributs : X = %d, Y = %d, \n", PosX, PosY);
  printf ("+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+\n");
#endif	/* TRACE */

  return;
}

/*!BF
   ------------------------------------------------------------------------------
   *
   * Function      : AjusterBoiteDim
   * Result        : void
   * Parameters
   * Name          Type          Usage
   * ----          ----          -----
   * Objet              Element         Noeud a repositionner et calculer
   * Doc                Document        Document de rattachement
   *
   * Functionality : Redimensionne la boite de l'objet sur la grille.
   *
   ------------------------------------------------------------------------------
   !EF */

#ifdef __STDC__

void 
AjusterBoiteDim (Element Objet, Document Doc)

#else /* __STDC__ */

void 
AjusterBoiteDim (Objet, Doc)
     Element Objet;
     Document Doc;
#endif /* __STDC__ */

{
  Document doc;
  View VuePrincipale;
  int H, L, Hauteur, Largeur, i;
  
   TtaGiveActiveView(&doc,&VuePrincipale);

  if (!VuePrincipale)
    return;
  GetValAttrTaille(Objet, &Hauteur, &Largeur,Doc);
  TtaGiveBoxSize (Objet, Doc, VuePrincipale, UnPixel, &L, &H);

  i = (Largeur / GrilleX) * GrilleX;
  if (Largeur > (i + (GrilleX / 2)))
    Largeur = i + GrilleX;
  else
    Largeur = i;

  i = (Hauteur / GrilleY) * GrilleY;
  if (Hauteur > (i + (GrilleY / 2)))
    Hauteur = i + GrilleY;
  else
    Hauteur = i;

  if((Largeur-L != 0) || (Hauteur-H != 0 ))
    SetAttrTaille (Objet, Hauteur, Largeur, Doc);
  
#ifdef TRACE
  printf ("+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+\n");
  printf ("Position apres mise a jour des attributs : H = %d, L = %d\n", Hauteur, Largeur);
  printf ("+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+\n");
#endif	/* TRACE */

  return;
}
#ifdef __STDC__
void AjusterBoite(Element Objet, Document Doc)
#else /*__STDC__*/
void AjusterBoite(/* Element Objet, Document Doc */)
Element Objet;
 Document Doc;
#endif
{
  AjusterBoitePos( Objet,  Doc);
  AjusterBoiteDim( Objet,  Doc);
}
/*!BF
   ------------------------------------------------------------------------------
   *
   * Function      : PositionnerFleche
   * Result        : void
   * Parameters
   * Name          Type          Usage
   * ----          ----          -----
   * fleche     *NotifyElement  Noeud FlecheCreuse a positionner
   *
   * Functionality : Calcule la forme, la taille et la position de la fleche qui
   *            vient d'etre creee ou dont la boite origine ou destination
   *            vient d'etre deplacee.
   *
   ------------------------------------------------------------------------------
   !EF */

#ifdef __STDC__

void 
PositionnerFleche (NotifyElement * fleche)

#else /* __STDC__ */

void 
PositionnerFleche (fleche)
     NotifyElement *fleche;

#endif /* __STDC__ */

{
  Element Origine, Destination;
  Attribute AttOrigine, AttDestination;
  AttributeType AttRef;
  Document TargetDoc;
  char DocName[1];

  AttRef.AttrSSchema = TtaGetSSchema ("Draw3", fleche->document);
  AttRef.AttrTypeNum = Draw3_ATTR_Origine_lien;
  AttOrigine = TtaGetAttribute (fleche->element, AttRef);
  AttRef.AttrTypeNum = Draw3_ATTR_Terminaison_lien;
  AttDestination = TtaGetAttribute (fleche->element, AttRef);
  TtaGiveReferenceAttributeValue (AttOrigine, &Origine, DocName, &TargetDoc);
  TtaGiveReferenceAttributeValue (AttDestination, &Destination, DocName,
				  &TargetDoc);
/*   AjusterBoite (Origine, fleche->document); */
/*   AjusterBoite (Destination, fleche->document); */
  CalculerFleche (fleche, 'c', Origine, Destination);

  return;
}

/*!BF
   ------------------------------------------------------------------------------
   *
   * Function      : PositionnerLien
   * Result        : void
   * Parameters
   * Name          Type          Usage
   * ----          ----          -----
   * fleche     *NotifyElement  Noeud FlecheCreuse a positionner
   *
   * Functionality : Calcule la forme, la taille et la position de la fleche qui
   *            vient d'etre creee ou dont la boite origine ou destination
   *            vient d'etre deplacee.
   *
   ------------------------------------------------------------------------------
   !EF */

#ifdef __STDC__

void 
PositionnerLien (NotifyElement * fleche)

#else /* __STDC__ */

void 
PositionnerLien (fleche)
     NotifyElement *fleche;

#endif /* __STDC__ */

{
  Element Origine, Destination;
  Attribute AttOrigine, AttDestination;
  AttributeType AttRef;
  Document TargetDoc;
  char DocName[1];

  AttRef.AttrSSchema = TtaGetSSchema ("Draw3", fleche->document);
  AttRef.AttrTypeNum = Draw3_ATTR_Origine_lien;
  AttOrigine = TtaGetAttribute (fleche->element, AttRef);
  AttRef.AttrTypeNum = Draw3_ATTR_Terminaison_lien;
  AttDestination = TtaGetAttribute (fleche->element, AttRef);
  TtaGiveReferenceAttributeValue (AttOrigine, &Origine, DocName, &TargetDoc);
  TtaGiveReferenceAttributeValue (AttDestination, &Destination, DocName,
				  &TargetDoc);
  AjusterBoite (Origine, fleche->document);
  AjusterBoite (Destination, fleche->document);
  CalculerFleche (fleche, 's', Origine, Destination);

  return;
}

/*!BF
   ------------------------------------------------------------------------------
   *
   * Function      : RePositionnerLien
   * Result        : void
   * Parameters
   * Name          Type                 Usage
   * ----          ----                 -----
   * lien               *NotifyElement  lien a repositionner
   *
   * Functionality : En fonction du type de lien, appelle la bonne fonction de
   * calcul.
   *
   ------------------------------------------------------------------------------
   !EF */

#ifdef __STDC__

void 
RePositionnerLien (NotifyElement * lien)

#else /* __STDC__ */

void 
RePositionnerLien (lien)
     NotifyElement *lien;

#endif /* __STDC__ */

{
  switch (TtaGetElementType (lien->element).ElTypeNum)
    {
    case Draw3_EL_LienSimple:
      PositionnerLien (lien);
      break;
    case Draw3_EL_FlecheCreuse:
      PositionnerFleche (lien);
      break;
    case Draw3_EL_PartieOrigineLien:
      TracerOrigineLien (lien->element, lien->document);
      break;
    case Draw3_EL_PartieDestinationLien:
      TracerDestinationLien (lien->element, lien->document);
      break;
    }
  return;
}

/*!BF
   ------------------------------------------------------------------------------
   *
   * Function      : SauverCoordonnees
   * Result        : void
   * Parameters
   * Name          Type          Usage
   * ----          ----          -----
   * Noeud              Element         Noeud venant d'etre deplace ou retaille
   *
   * Functionality : Sauvegarde les coordonnees et la taille d'une boite afin de
   * pouvoir deplacer un lien correctement dans la postaction.
   *
   ------------------------------------------------------------------------------
   !EF */

#ifdef __STDC__

void 
SauverCoordonnees (Element Noeud, Document Doc)

#else /* __STDC__ */

void 
SauverCoordonnees (Noeud, Doc)
     Element Noeud;
     Document Doc;

#endif /* __STDC__ */

{
  GetValAttrPosition (Noeud, &SaveCoinX, &SaveCoinY, Doc);
  GetValAttrTaille (Noeud, &SaveHauteur, &SaveLargeur, Doc);
  return;
}

/*!BF
   ------------------------------------------------------------------------------
   *
   * Function      : DessinerFleche
   * Result        : void
   * Parameters
   * Name          Type                 Usage
   * ----          ----                 -----
   * Fleche     *NotifyElement  Fleche a dessiner
   * type               char            Type de Fleche
   *
   * Functionality : Dessine une Fleche dans la boite determinee par l'utilisateur
   *
   ------------------------------------------------------------------------------
   !EF */

#ifdef __STDC__

void 
DessinerFleche (NotifyElement * Fleche, char type)

#else /* __STDC__ */

void 
DessinerFleche (Fleche, type)
     NotifyElement *Fleche;
     char type;

#endif /* __STDC__ */

{
  Document doc;
  View VuePrincipale;
  int Largeur, Hauteur, PosX, PosY;

  TtaGiveActiveView(&doc,&VuePrincipale);
/*   VuePrincipale = TtaGetViewFromName (Fleche->document, "The_draw"); */
  if (!VuePrincipale)
    return;
  TtaGiveBoxSize (Fleche->element, doc, VuePrincipale, UnPixel, &Largeur, &Hauteur);
  TtaGiveBoxPosition (Fleche->element, doc, VuePrincipale, UnPixel, &PosX, &PosY);

  SetAttrPosition (Fleche->element, PosX + 1, PosY + 3,
		   Fleche->document);
  SetAttrTaille (Fleche->element, Hauteur, Largeur,
		 Fleche->document);

  if (type == 'h')
    TracerFleche (Fleche, VERS_LA_DROITE, 6,
		  PosX, PosY, Hauteur, Largeur);
  else
    TracerFleche (Fleche, VERS_LE_BAS, 6,
		  PosX, PosY, Hauteur, Largeur);
  return;
}
