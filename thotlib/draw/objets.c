/*!BM
   ------------------------------------------------------------------------------
   *
   * Module      : objets.c
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
   *                    DetermineSens
   *                    CreerOmbre
   *                    RetaillerGroupe
   *
   ------------------------------------------------------------------------------
   !EM */

#define Xobjets

/****************************************************************************/
/*--------------------------- SYSTEM INCLUDES ------------------------------*/
/****************************************************************************/

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

/****************************************************************************/
/*------------------------ APPLICATION  INCLUDES ---------------------------*/
/****************************************************************************/

#include "objets.h"
#include "Draw3.h"
#include "application.h"
#include "document.h"
#include "attribute.h"
#include "language.h"
#include "content.h"
#include "reference.h"
#include "tree.h"
#include "app.h"

#include "liens.h"
#include "utilitaires.h"

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
   * Function      : DetermineSens
   * Result        : char
   * Parameters
   * Name          Type                 Usage
   * ----          ----                 -----
   * Noeud              Element         Noeud de type Graphique
   * objet              char            Fleche ou trait a modifier
   * Doc                Document        Document de rattachement
   *
   * Functionality : retourne le code thot du graphique a creer.
   *
   ------------------------------------------------------------------------------
   !EF */

#ifdef __STDC__

char 
DetermineSens (Element Noeud, char objet, Document Doc)

#else /* __STDC__ */

char 
DetermineSens (Noeud, objet, Doc)
     Element Noeud;
     char objet;
     Document Doc;

#endif /* __STDC__ */

{
  int h, l;
  View VuePrincipale;
  Document doc;

  TtaGiveActiveView(&doc,&VuePrincipale);
  /*   VuePrincipale = TtaGetViewFromName (Doc, "The_draw"); */
  if (!VuePrincipale || doc!=Doc)
    return (objet);
  TtaGiveBoxSize (Noeud, Doc, VuePrincipale, UnPixel, &l, &h);
  if ((l < 5) || (h < 5))	/* Diagonale */
    {
      switch (objet)
	{
	case 'h':
	  return ('\\');	/* Trait Diagonale */
	case '>':
	  return ('e');		/* Fleche Diagonale */
	}
    }
  else
    {
      if (l > h)		/* Horizontale */
	{
	  switch (objet)
	    {
	    case 'h':
	      return ('h');	/* Trait Diagonale */
	    case '>':
	      return ('>');	/* Fleche Diagonale */
	    }
	}
      else
	/* Verticale */
	{
	  switch (objet)
	    {
	    case 'h':
	      return ('v');	/* Trait Diagonale */
	    case '>':
	      return ('V');	/* Fleche Diagonale */
	    }
	}
    }
  return ('h');
}

/*!BF
   ------------------------------------------------------------------------------
   *
   * Function      : CreerOmbre
   * Result        : void
   * Parameters
   * Name          Type          Usage
   * ----          ----          -----
   * Fleche     Element         Modele de base
   * Doc                Document        Document englobant
   *
   * Functionality : Cree l'ombre associe a la fleche par recopie du polygone.
   *
   ------------------------------------------------------------------------------
   !EF */

#ifdef __STDC__

void 
CreerOmbre (Element Fleche, Document Doc)

#else /* __STDC__ */

void 
CreerOmbre (Fleche, Doc)
     element Fleche;
     Document Doc;

#endif /* __STDC__ */

{
  int x, y, i, j;
  char shape;
  Element Ombre;

  Ombre = TtaGetLastChild (Fleche);
  Fleche = TtaGetFirstChild (Fleche);
  if (TtaGetElementType (Ombre).ElTypeNum != Draw3_EL_Ombre)
    return;
  Ombre = TtaGetFirstChild (Ombre);
  shape = TtaGetGraphicsShape (Fleche);
  if (TtaGetGraphicsShape (Ombre) == 'p')
    DetruirePolygone (Ombre, Doc);
  else
    TtaSetGraphicsShape (Ombre, shape, Doc);
  if (shape == 'p')
    {
      i = TtaGetPolylineLength (Fleche);

/* ATTENTION  BIDOUILLE ATTENTION  BIDOUILLE

   On incremente car la fct TtaGivePolylinePoint n'est pas
   homogene avec les autres fonctions de gestion des polylignes.
   Le premier couple de valeur correspond a la coordonnee du coin
   bas a droite
   ATTENTION  BIDOUILLE ATTENTION  BIDOUILLE */

      i++;
      for (j = 2; j <= i; j++)
	{
	  TtaGivePolylinePoint (Fleche, j, UnPixel, &x, &y);

	  /* ATTENTION  BIDOUILLE ATTENTION  BIDOUILLE
	     Voir commentaire precedent expliquant le -1
	   */

	  TtaAddPointInPolyline (Ombre, j - 1, UnPixel, x, y, Doc);
	}

      TtaGivePolylinePoint (Fleche, 2, UnPixel, &x, &y);
      TtaChangeLimitOfPolyline (Ombre, UnPixel, x, y, Doc);
    }
  return;
}

/*!BF
   ------------------------------------------------------------------------------
   *
   * Function      : RetaillerGroupe
   * Result        : void
   * Parameters
   * Name          Type                 Usage
   * ----          ----                 -----
   * groupe     Element         Groupe a retailler
   * ah         int             ancienne hauteur
   * nh         int             nouvelle hauteur
   * al         int             ancienne largeur
   * nl         int             nouvelle largeur
   * Doc                Document        Document englobant
   *
   * Functionality : Redimensionne l'ensemble des elements d'un groupe.
   *
   ------------------------------------------------------------------------------
   !EF */

#ifdef __STDC__

void 
RetaillerGroupe (Element groupe, int ah, int nh, int al, int nl,
		 Document Doc, View Vue)

#else /* __STDC__ */

void 
RetaillerGroupe (groupe, ah, nh, al, nl, Doc, Vue)
     Element groupe;
     int ah, nh, al, nl;
     Document Doc;
     View Vue;

#endif /* __STDC__ */

{
  Element Courant, Texte;
  ElementType TextElement;
  int X, Y, H, L, NX, NY, NH, NL, TailleFonte, Basique;

  Basique = 0;
  if (TtaGetElementType (groupe).ElTypeNum == Draw3_EL_Groupe)
    Courant = TtaGetFirstChild (groupe);
  else
    {
      Courant = groupe;
      Basique = 1;
    }

  while (Courant != NULL)
    {
      GetValAttrPosition (Courant, &X, &Y, Doc);
      GetValAttrTaille (Courant, &H, &L, Doc);
      NX = (X * nl) / al;
      NY = (Y * nh) / ah;
      NH = (H * nh) / ah;
      NL = (L * nl) / al;
      if (TtaGetElementType (Courant).ElTypeNum == Draw3_EL_Groupe)
	{
	  RetaillerGroupe (Courant, H, NH, L, NL, Doc, Vue);
	}
      else
	{
	  TextElement.ElSSchema = TtaGetDocumentSSchema (Doc);
	  TextElement.ElTypeNum = Draw3_EL_TEXT_UNIT;
	  Texte = TtaSearchTypedElement (TextElement, SearchInTree,
					 Courant);
	  if (Texte != NULL)
	    {
	      TailleFonte = GetPRule (Texte, PRSize);
	      if (!TailleFonte)
		TailleFonte = 12;
	      TailleFonte = (TailleFonte * nh) / ah;
	      if (TailleFonte < 6)
		TailleFonte = 6;
	      else if (TailleFonte > 72)
		TailleFonte = 72;
	      SetPRule (Texte, PRSize, TailleFonte, Doc);
	    }
	}
      if (!Basique)
	{
	  SetAttrPosition (Courant, NX, NY, Doc);
	  SetAttrTaille (Courant, NH, NL, Doc);
	  TtaNextSibling (&Courant);
	}
      else
	Courant = NULL;
    }
  SetAttrTaille (groupe, nh, nl, Doc);
  return;
}
