/*
 * Copyright (c) 1996 INRIA, All rights reserved
 */

/*----------------------------------------------------------------------
   
   THOT                                     
   
   Ce module traite les commandes de creation              
   appelees par le mediateur.                              
   
  ----------------------------------------------------------------------*/

#include "thot_gui.h"
#include "thot_sys.h"
#include "message.h"
#include "dialog.h"
#include "constmedia.h"
#include"constmenu.h"
#define MAX_ENTRIES 20
#include "typemedia.h"

typedef enum
  {
     Dedans, ActionAvant, Apres, ElementRef
  }
TypeAction;

#undef EXPORT
#define EXPORT extern
#include "modif_tv.h"
#include "select_tv.h"
#include "platform_tv.h"
#include "edit_tv.h"

static boolean      AnswerMenuAskForNew;	/* reponse valide au menu creer/designer */
static boolean      AnswerCreateAskForNew;	/* reponse Creer au menu creer/designer */

#include "structcreation_f.h"

#ifdef __STDC__
extern int          ConfigMakeDocTypeMenu (char *, int *, boolean);

#else  /* __STDC__ */
extern int          ConfigMakeDocTypeMenu ();

#endif /* __STDC__ */

/*----------------------------------------------------------------------
   AskForNew_RemplRefer Lors de l'etablissement d'une reference, demande 
   a l'utilisateur s'il veut creer en meme temps l'element reference' 
   ou s'il veut simplement designer un element existant 
   Au retour, la fonction retourne Faux si l'utilisateur abandonne la 
   commande, et sinon Creer vaut Vrai si l'utilisateur veut creer 
   l'element reference', Faux s'il veut simplement le designer. 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
boolean             AskForNew_RemplRefer (boolean * generate, Name typeName)
#else  /* __STDC__ */
boolean             AskForNew_RemplRefer (generate, typeName)
boolean            *generate;
Name                typeName;

#endif /* __STDC__ */
{
   int                 i;
   char                bufMenu[MAX_TXT_LEN];

   /* cree et active le menu */
   i = 0;
   sprintf (&bufMenu[i], "%s%s", "B", TtaGetMessage (LIB, TMSG_CREATE_EL_REF));
   i += strlen (&bufMenu[i]) + 1;
   sprintf (&bufMenu[i], "%s%s", "B", TtaGetMessage (LIB, TMSG_SHOW_EL_REF));
   TtaNewPopup (NumMenuCreateReferenceElem, 0,
	      TtaGetMessage (LIB, TMSG_MODE_INSERT), 2, bufMenu, NULL, 'L');
   TtaShowDialogue (NumMenuCreateReferenceElem, FALSE);
   /* attend que l'utilisateur aie repondu au menu */
   TtaWaitShowDialogue ();
   *generate = AnswerCreateAskForNew;
   return AnswerMenuAskForNew;
}

/*----------------------------------------------------------------------
   CallbackAskForNew   traite le retour du menu qui demande a         
   l'utilisateur s'il veut designer un element a referencer       
   s'il prefere creer un nouvel element.                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CallbackAskForNew (int Val)
#else  /* __STDC__ */
void                CallbackAskForNew (Val)
int                 Val;

#endif /* __STDC__ */
{
   if (Val == 0)
     {
	AnswerMenuAskForNew = TRUE;
	AnswerCreateAskForNew = TRUE;
     }
   if (Val == 1)
     {
	AnswerMenuAskForNew = TRUE;
	AnswerCreateAskForNew = FALSE;
     }
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                BuildChoiceMenu (char *bufMenu, Name menuTitle, int nbEntries, boolean natureChoice)
#else  /* __STDC__ */
void                BuildChoiceMenu (bufMenu, menuTitle, nbEntries, natureChoice)
char               *bufMenu;
Name                menuTitle;
int                 nbEntries;
boolean             natureChoice;

#endif /* __STDC__ */
{
   int                 menu;
   char                bufMenuB[MAX_TXT_LEN];
   char               *src;
   char               *dest;
   int                 k, l, nbitem, length;

   if (natureChoice)
     {
	menu = NumFormNature;
	/* selecteur de saisie de la nature de l'element a creer (ou zone de saisie */
	/* s'il n'y a pas de natures definies dans les fichiers de config.). */
	TtaNewForm (NumFormNature, 0, 0, 0,
	       TtaGetMessage (LIB, TMSG_OBJECT_TYPE), TRUE, 1, 'L', D_DONE);
	nbitem = ConfigMakeDocTypeMenu (bufMenuB, &length, FALSE);
	if (nbitem > 0)
	   /* le fichier Start Up definit des natures */
	  {
	     /* calcule la hauteur de la partie menu du selecteur */
	     if (nbitem < 5)
		length = nbitem;
	     else
		length = 5;
	     /* cree le selecteur */
	     TtaNewSelector (NumSelectNatureName, NumFormNature,
			     TtaGetMessage (LIB, TMSG_OBJECT_TYPE), nbitem, bufMenuB, length, NULL, TRUE, FALSE);
	     /* initialise le selecteur sur sa premiere entree */
	     TtaSetSelector (NumSelectNatureName, 0, "");
	  }
	else
	   /* on n'a pas cree' de selecteur, on cree une zone de saisie */
	   /* zone de saisie de la nature de l'element a creer */
	   TtaNewTextForm (NumSelectNatureName, NumFormNature,
		       TtaGetMessage (LIB, TMSG_OBJECT_TYPE), 30, 1, FALSE);
     }
   else
     {
	menu = NumMenuElChoice;
	/* ajoute 'B' au debut de chaque entree */
	dest = &bufMenuB[0];
	src = &bufMenu[0];
	for (k = 1; k <= nbEntries; k++)
	  {
	     strcpy (dest, "B");
	     dest++;
	     l = strlen (src);
	     strcpy (dest, src);
	     dest += l + 1;
	     src += l + 1;
	  }
	TtaNewPopup (NumMenuElChoice, 0, menuTitle, nbEntries, bufMenuB, NULL, 'L');
     }
   TtaShowDialogue (menu, FALSE);
   /* attend que l'utilisateur ait repondu au menu et que le */
   /* mediateur ait appele' ChoiceMenuCallback */
   TtaWaitShowDialogue ();
}


/*----------------------------------------------------------------------
   InsertSeparatorInMenu      met un separateur dans le menu Inserer/ 
   Coller / Inclure                        
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                InsertSeparatorInMenu (int *prevMenuInd, int *nbEntries, int *menuInd, char *bufMenu)
#else  /* __STDC__ */
void                InsertSeparatorInMenu (prevMenuInd, nbEntries, menuInd, bufMenu)
int                *prevMenuInd;
int                *nbEntries;
int                *menuInd;
char               *bufMenu;

#endif /* __STDC__ */
{
   *prevMenuInd = *menuInd;
   /* indique qu'il s'agit d'un separateur */
   bufMenu[*menuInd] = 'S';
   (*menuInd)++;
   bufMenu[*menuInd] = '\0';
   (*menuInd)++;
   (*nbEntries)++;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                BuildPasteMenu (int RefMenu, char *bufMenu, Name title, int nbEntries, char button)
#else  /* __STDC__ */
void                BuildPasteMenu (RefMenu, bufMenu, title, nbEntries, button)
int                 RefMenu;
char               *bufMenu;
Name                title;
int                 nbEntries;
char                button;

#endif /* __STDC__ */
{
   char                bufMenuB[MAX_TXT_LEN];
   char               *src;
   char               *dest;
   int                 k, l;

   /* ajoute 'B' au debut de chaque entree du menu */
   dest = &bufMenuB[0];
   src = &bufMenu[0];
   for (k = 1; k <= nbEntries; k++)
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
   TtaNewPopup (RefMenu, 0, title, nbEntries, bufMenuB, NULL, button);
   TtaShowDialogue (RefMenu, FALSE);
   /* attend la reponse de l'utilisateur */
   TtaWaitShowDialogue ();
}
