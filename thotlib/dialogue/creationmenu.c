
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

/* ====================================================================== */
/* |                                                                    | */
/* |                           THOT                                     | */
/* |                                                                    | */
/* |            Ce module traite les commandes de creation              | */
/* |            appelees par le mediateur.                              | */
/* |                                                                    | */
/* |                    V. Quint        Octobre 1985                    | */
/* |                                                                    | */
/* ====================================================================== */

#include "thot_gui.h"
#include "thot_sys.h"
#include "message.h"
#include "dialog.h"
#include "constmedia.h"
#include"constmenu.h"
#define maxentree 20
#define LgMaxMenu 20
#include "typemedia.h"

typedef enum
  {
     Dedans, ActionAvant, Apres, ElementRef
  }
TypeAction;

#undef EXPORT
#define EXPORT extern
#include "modif.var"
#include "select.var"
#include "environ.var"
#include "edit.var"

static boolean      RetourAskForNew;	/* reponse valide au menu creer/designer */
static boolean      CreerAskForNew;	/* reponse Creer au menu creer/designer */

#include "creation.f"

#ifdef __STDC__
extern int          ConfigMakeDocTypeMenu (char *, int *, boolean);

#else  /* __STDC__ */
extern int          ConfigMakeDocTypeMenu ();

#endif /* __STDC__ */

/* --------------------------------------------------------------------  */
/* AskForNew_RemplRefer Lors de l'etablissement d'une reference, demande */
/* a l'utilisateur s'il veut creer en meme temps l'element reference' */
/* ou s'il veut simplement designer un element existant */
/* Au retour, la fonction retourne Faux si l'utilisateur abandonne la */
/* commande, et sinon Creer vaut Vrai si l'utilisateur veut creer */
/* l'element reference', Faux s'il veut simplement le designer. */
/* --------------------------------------------------------------------  */
#ifdef __STDC__
boolean             AskForNew_RemplRefer (boolean * Creer, Name NomType)
#else  /* __STDC__ */
boolean             AskForNew_RemplRefer (Creer, NomType)
boolean            *Creer;
Name                 NomType;

#endif /* __STDC__ */
{
   int                 i;
   char                BufMenu[MAX_TXT_LEN];

   /* cree et active le menu */
   i = 0;
   sprintf (&BufMenu[i], "%s%s", "B", TtaGetMessage (LIB, LIB_CREATE_ELEM_REF));
   i += strlen (&BufMenu[i]) + 1;
   sprintf (&BufMenu[i], "%s%s", "B", TtaGetMessage (LIB, LIB_SHOW_ELEM_REF));
   TtaNewPopup (NumMenuCreerElemReference, 0,
		TtaGetMessage (LIB, LIB_MODE_INSERT), 2, BufMenu, NULL, 'L');
   TtaShowDialogue (NumMenuCreerElemReference, FALSE);
   /* attend que l'utilisateur aie repondu au menu */
   TtaWaitShowDialogue ();
   *Creer = CreerAskForNew;
   return RetourAskForNew;
}

/* --------------------------------------------------------------------  */
/* | RetMenuAskForNew   traite le retour du menu qui demande a         | */
/* |    l'utilisateur s'il veut designer un element a referencer       | */
/* |    s'il prefere creer un nouvel element.                          | */
/* --------------------------------------------------------------------  */
#ifdef __STDC__
void                RetMenuAskForNew (int Val)
#else  /* __STDC__ */
void                RetMenuAskForNew (Val)
int                 Val;

#endif /* __STDC__ */
{
   if (Val == 0)
     {
	RetourAskForNew = TRUE;
	CreerAskForNew = TRUE;
     }
   if (Val == 1)
     {
	RetourAskForNew = TRUE;
	CreerAskForNew = FALSE;
     }
}


/*  --------------------------------------------------------------------  */
/*  --------------------------------------------------------------------  */
#ifdef __STDC__
void                CreeEtActiveMenuChoix (char *BufMenu, Name TitreMenu, int nbentree, boolean ChoixElemNature)
#else  /* __STDC__ */
void                CreeEtActiveMenuChoix (BufMenu, TitreMenu, nbentree, ChoixElemNature)
char               *BufMenu;
Name                 TitreMenu;
int                 nbentree;
boolean             ChoixElemNature;

#endif /* __STDC__ */
{
   int                 menu;
   char                BufMenuB[MAX_TXT_LEN];
   char               *src;
   char               *dest;
   int                 k, l, nbitem, longueur;

   if (ChoixElemNature)
     {
	menu = NumFormNature;
	/* selecteur de saisie de la nature de l'element a creer (ou zone de saisie */
	/* s'il n'y a pas de natures definies dans les fichiers de config.). */
	TtaNewForm (NumFormNature, 0, 0, 0,
		TtaGetMessage (LIB, LIB_OBJECT_TYPE), TRUE, 1, 'L', D_DONE);
	nbitem = ConfigMakeDocTypeMenu (BufMenuB, &longueur, FALSE);
	if (nbitem > 0)
	   /* le fichier Start Up definit des natures */
	  {
	     /* calcule la hauteur de la partie menu du selecteur */
	     if (nbitem < 5)
		longueur = nbitem;
	     else
		longueur = 5;
	     /* cree le selecteur */
	     TtaNewSelector (NumSelectNomNature, NumFormNature,
			     TtaGetMessage (LIB, LIB_OBJECT_TYPE), nbitem, BufMenuB, longueur, NULL, TRUE, FALSE);
	     /* initialise le selecteur sur sa premiere entree */
	     TtaSetSelector (NumSelectNomNature, 0, "");
	  }
	else
	   /* on n'a pas cree' de selecteur, on cree une zone de saisie */
	   /* zone de saisie de la nature de l'element a creer */
	   TtaNewTextForm (NumSelectNomNature, NumFormNature,
			TtaGetMessage (LIB, LIB_OBJECT_TYPE), 30, 1, FALSE);
     }
   else
     {
	menu = NumMenuChoixEl;
	/* ajoute 'B' au debut de chaque entree */
	dest = &BufMenuB[0];
	src = &BufMenu[0];
	for (k = 1; k <= nbentree; k++)
	  {
	     strcpy (dest, "B");
	     dest++;
	     l = strlen (src);
	     strcpy (dest, src);
	     dest += l + 1;
	     src += l + 1;
	  }
	TtaNewPopup (NumMenuChoixEl, 0, TitreMenu, nbentree, BufMenuB, NULL, 'L');
     }
   TtaShowDialogue (menu, FALSE);
   /* attend que l'utilisateur ait repondu au menu et que le */
   /* mediateur ait appele' RetMenuChoixElem */
   TtaWaitShowDialogue ();
}


/*  --------------------------------------------------------------------  */
/* |    SeparMenuCreeColle      met un separateur dans le menu Inserer/ | */
/* |                            Coller / Inclure                        | */
/*  --------------------------------------------------------------------  */
#ifdef __STDC__
void                SeparMenuCreeColle (int *prevmenuind, int *nbentree, int *menuind, char *BufMenu)
#else  /* __STDC__ */
void                SeparMenuCreeColle (prevmenuind, nbentree, menuind, BufMenu)
int                *prevmenuind;
int                *nbentree;
int                *menuind;
char               *BufMenu;

#endif /* __STDC__ */
{
   *prevmenuind = *menuind;
   /* indique qu'il s'agit d'un separateur */
   BufMenu[*menuind] = 'S';
   (*menuind)++;
   BufMenu[*menuind] = '\0';
   (*menuind)++;
   (*nbentree)++;
}

/*  --------------------------------------------------------------------  */
/*  --------------------------------------------------------------------  */
#ifdef __STDC__
void                CreeEtActiveMenuCreeColle (int RefMenu, char *BufMenu, Name titre, int nbentree, char bouton)
#else  /* __STDC__ */
void                CreeEtActiveMenuCreeColle (RefMenu, BufMenu, titre, nbentree, bouton)
int                 RefMenu;
char               *BufMenu;
Name                 titre;
int                 nbentree;
char                bouton;

#endif /* __STDC__ */
{
   char                BufMenuB[MAX_TXT_LEN];
   char               *src;
   char               *dest;
   int                 k, l;

   /* ajoute 'B' au debut de chaque entree du menu */
   dest = &BufMenuB[0];
   src = &BufMenu[0];
   for (k = 1; k <= nbentree; k++)
     {
	l = strlen (src);
	/* on ne met pas de 'B' devant les separateurs */
	if (*src != 'S' || l != 1)
	  {
	     strcpy (dest, "B");
	     dest++;
	  }
	strcpy (dest, src);
	dest += l + 1;
	src += l + 1;
     }
   TtaNewPopup (RefMenu, 0, titre, nbentree, BufMenuB, NULL, bouton);
   TtaShowDialogue (RefMenu, FALSE);
   /* attend la reponse de l'utilisateur */
   TtaWaitShowDialogue ();
}
