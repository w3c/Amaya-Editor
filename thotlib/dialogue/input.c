
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

/*=======================================================================*/
/*|                                                                     | */
/*|               Module de gestion des entrees clavier.                | */
/*|                                                                     | */
/*|                                                                     | */
/*|                     I. Vatton       Novembre 89                     | */
/*|                                                                     | */
/*|                                                                     | */
/*=======================================================================*/

/* D.V. : Removed X remapping of keys, lead to crash in some configurations  */
				   /* #define WITH_X_REBIND_KEYSYMS *//* old X remapping for 1 key translations */

#include "thot_gui.h"
#include "thot_sys.h"
#include "thot_key.h"
#include "constmedia.h"
#include "typemedia.h"
#include "libmsg.h"
#include "message.h"
#include "appdialogue.h"

#ifdef __STDC__
extern void         EndInsert (void);

#else
extern void         EndInsert ();

#endif

#define MAX_EQUIV 25
#define THOT_SANS_MOD	0
#define THOT_MOD_CTRL	1
#define THOT_MOD_META	2
#define THOT_MOD_ALT	4
#define THOT_MOD_SHIFT	10
#define THOT_MOD_S_CTRL	11
#define THOT_MOD_S_META	12
#define THOT_MOD_S_ALT	14
#define MAX_AUTOMATE	80
/* structure d'automate pour les touches */
typedef struct _touche
  {
     int                 T_Cle;	/* cle d'entree                             */
     struct _touche     *T_Suite;	/* 1ere touche complementaire (1er niveau)  */
     struct _touche     *T_Autre;	/* entree suivante de meme niveau           */
     short               T_Commande;	/* indice dans la liste des commandes ou -1 */
     int                 T_Valeur;	/* cle de retour si commande = -1           */
  }
TOUCHE;

#define T_Modifieur T_Suite	/* valeur du modifieur (2eme niveau)          */


#undef EXPORT
#define EXPORT extern
#include "img.var"
#include "frame.var"
#include "edit.var"
#include "appdialogue.var"

/* Table des actions */
#include "appli.f"
#include "cmd.f"
#include "cmdedit.f"
#include "context.f"
#include "editmenu.f"
#include "es.f"
#include "input.f"
#include "memory.f"
#include "message.f"
#include "modif.f"
#include "modifcmd.f"
#include "select.f"


/* Actions associees par defaut aux cles XK_Up, XK_Left, XK_Right, XK_Down */
#define MY_KEY_Up 0
#define MY_KEY_Left 1
#define MY_KEY_Right 2
#define MY_KEY_Down 3
#define MY_KEY_BackSpace 4
#define MY_KEY_Delete 5
#define MY_KEY_Prior 6
#define MY_KEY_Next 7
#define MY_KEY_Home 8
#define MY_KEY_End 9
static int          SpecialKeys[] =
{5, 3, 4, 6, 1, 2, 13, 14, 15, 16};

/* Les automates */
static TOUCHE      *Automate_normal = NULL;

static TOUCHE      *Automate_ctrl = NULL;
static TOUCHE      *Automate_meta = NULL;
static TOUCHE      *Automate_alt = NULL;
static TOUCHE      *Automate_CTRL = NULL;
static TOUCHE      *Automate_META = NULL;
static TOUCHE      *Automate_ALT = NULL;
static TOUCHE      *Automate_current = NULL;

/* ---------------------------------------------------------------------- */
/* |    NomCle traduit les noms de cle's non supporte's par             | */
/* |            l'interpre'teur des translations Motif.                 | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static char        *NomCle (char *name)

#else  /* __STDC__ */
static char        *NomCle (name)
char               *name;

#endif /* __STDC__ */

{
   if (strlen (name) < 5)
      if (name[0] == ',')
	 return ("0x2c");
      else
	 return (name);
   else if (!strcmp (name, "Return"))
      return ("0x0d");
   else if (!strcmp (name, "BackSpace"))
      return ("0x08");
   else if (!strcmp (name, "Space"))
      return ("0x20");
   else if (!strcmp (name, "Escape"))
      return ("0x18");
   else if (!strcmp (name, "Delete"))
      return ("0x7f");
   else
      return (name);
}				/*NomCle */


/* ---------------------------------------------------------------------- */
/* |    CleSpecial traduit le nom fourni dans le fichier thot.keyboard  | */
/* |            en une valeur de cle' exploitable par Thot.             | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static unsigned int CleSpecial (char *name)
#else  /* __STDC__ */
static unsigned int CleSpecial (name)
char               *name;

#endif /* __STDC__ */
{
   /* Est-ce le nom d'un caractere special */
   if (!strcmp (name, "Return"))
      return (unsigned int) THOT_KEY_Return;
   else if (!strcmp (name, "BackSpace"))
      return (unsigned int) THOT_KEY_BackSpace;
   else if (!strcmp (name, "Space"))
      return 32;
   else if (!strcmp (name, "Escape"))
      return (unsigned int) THOT_KEY_Escape;
   else if (!strcmp (name, "Delete"))
      return (unsigned int) THOT_KEY_Delete;
   else if (!strcmp (name, "F1"))
      return (unsigned int) THOT_KEY_F1;
   else if (!strcmp (name, "F2"))
      return (unsigned int) THOT_KEY_F2;
   else if (!strcmp (name, "F3"))
      return (unsigned int) THOT_KEY_F3;
   else if (!strcmp (name, "F4"))
      return (unsigned int) THOT_KEY_F4;
   else if (!strcmp (name, "F5"))
      return (unsigned int) THOT_KEY_F5;
   else if (!strcmp (name, "F6"))
      return (unsigned int) THOT_KEY_F6;
   else if (!strcmp (name, "F7"))
      return (unsigned int) THOT_KEY_F7;
   else if (!strcmp (name, "F8"))
      return (unsigned int) THOT_KEY_F8;
   else if (!strcmp (name, "F9"))
      return (unsigned int) THOT_KEY_F9;
   else if (!strcmp (name, "F10"))
      return (unsigned int) THOT_KEY_F10;
   else if (!strcmp (name, "F11") || !strcmp (name, "L1"))
      return (unsigned int) THOT_KEY_F11;
   else if (!strcmp (name, "F12") || !strcmp (name, "L2"))
      return (unsigned int) THOT_KEY_F12;
   else if (!strcmp (name, "F13") || !strcmp (name, "L3"))
      return (unsigned int) THOT_KEY_F13;
   else if (!strcmp (name, "F14") || !strcmp (name, "L4"))
      return (unsigned int) THOT_KEY_F14;
   else if (!strcmp (name, "F15") || !strcmp (name, "L5"))
      return (unsigned int) THOT_KEY_F15;
   else if (!strcmp (name, "F16") || !strcmp (name, "L6"))
      return (unsigned int) THOT_KEY_F16;
   else if (!strcmp (name, "F17") || !strcmp (name, "L7"))
      return (unsigned int) THOT_KEY_F17;
   else if (!strcmp (name, "F18") || !strcmp (name, "L8"))
      return (unsigned int) THOT_KEY_F18;
   else if (!strcmp (name, "F19") || !strcmp (name, "L9"))
      return (unsigned int) THOT_KEY_F19;
   else if (!strcmp (name, "F20") || !strcmp (name, "L10"))
      return (unsigned int) THOT_KEY_F20;
#ifdef THOT_KEY_R1
   else if (!strcmp (name, "F21") || !strcmp (name, "R1"))
      return (unsigned int) THOT_KEY_R1;
   else if (!strcmp (name, "F22") || !strcmp (name, "R2"))
      return (unsigned int) THOT_KEY_R2;
   else if (!strcmp (name, "F23") || !strcmp (name, "R3"))
      return (unsigned int) THOT_KEY_R3;
   else if (!strcmp (name, "F24") || !strcmp (name, "R4"))
      return (unsigned int) THOT_KEY_R4;
   else if (!strcmp (name, "F25") || !strcmp (name, "R5"))
      return (unsigned int) THOT_KEY_R5;
   else if (!strcmp (name, "F26") || !strcmp (name, "R6"))
      return (unsigned int) THOT_KEY_R6;
   else if (!strcmp (name, "F27") || !strcmp (name, "R7"))
      return (unsigned int) THOT_KEY_R7;
   else if (!strcmp (name, "F28") || !strcmp (name, "R8"))
      return (unsigned int) THOT_KEY_R8;
   else if (!strcmp (name, "F29") || !strcmp (name, "R9"))
      return (unsigned int) THOT_KEY_R9;
   else if (!strcmp (name, "F30") || !strcmp (name, "R10"))
      return (unsigned int) THOT_KEY_R10;
   else if (!strcmp (name, "F31") || !strcmp (name, "R11"))
      return (unsigned int) THOT_KEY_R11;
   else if (!strcmp (name, "F32") || !strcmp (name, "R12"))
      return (unsigned int) THOT_KEY_R12;
   else if (!strcmp (name, "F33") || !strcmp (name, "R13"))
      return (unsigned int) THOT_KEY_R13;
   else if (!strcmp (name, "F34") || !strcmp (name, "R14"))
      return (unsigned int) THOT_KEY_R14;
   else if (!strcmp (name, "F35") || !strcmp (name, "R15"))
      return (unsigned int) THOT_KEY_R15;
#endif /* THOT_KEY_R1 */
   else if (!strcmp (name, "Up"))
      return (unsigned int) THOT_KEY_Up;
   else if (!strcmp (name, "Down"))
      return (unsigned int) THOT_KEY_Down;
   else if (!strcmp (name, "Left"))
      return (unsigned int) THOT_KEY_Left;
   else if (!strcmp (name, "Right"))
      return (unsigned int) THOT_KEY_Right;
   else if (!strcmp (name, "Home"))
      return (unsigned int) THOT_KEY_Home;
   else if (!strcmp (name, "End"))
      return (unsigned int) THOT_KEY_End;
   else
      return (unsigned int) name[0];
}				/*CleSpecial */



/* ---------------------------------------------------------------------- */
/* |    MemoTouche memorise un raccourci clavier dans les automates.    | */
/* |            mod1 = 1ere modifieur                                   | */
/* |            cle1 = 1ere cle                                         | */
/* |            mod2 = 2eme modifieur                                   | */
/* |            cle2 = 2eme cle                                         | */
/* |            cle  = valeur du keysym ou 0                            | */
/* |            commande = numero de commande dans MesActions           | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         MemoTouche (int mod1, int cle1, int mod2, int cle2, int cle, int commande)
#else  /* __STDC__ */
static void         MemoTouche (mod1, cle1, mod2, cle2, cle, commande)
int                 mod1;
int                 cle1;
int                 mod2;
int                 cle2;
int                 cle;
int                 commande;

#endif /* __STDC__ */
{
   boolean             existe;
   TOUCHE             *ptr = NULL;
   TOUCHE             *oldptr;
   TOUCHE            **addebut;

   if (cle1 == 0)
      return;

   /* Point d'entree dans l'automate */
   switch (mod1)
	 {
	    case THOT_SANS_MOD:
	       addebut = &Automate_normal;
	       break;
	    case THOT_MOD_CTRL:
	       addebut = &Automate_ctrl;
	       break;
	    case THOT_MOD_ALT:
	       addebut = &Automate_alt;
	       break;
	    case THOT_MOD_META:
	       addebut = &Automate_meta;
	       break;
	    case THOT_MOD_S_CTRL:
	       addebut = &Automate_CTRL;
	       break;
	    case THOT_MOD_S_ALT:
	       addebut = &Automate_ALT;
	       break;
	    case THOT_MOD_S_META:
	       addebut = &Automate_META;
	       break;
	    default:
	       return;
	 }

   /* Initialisations */
   ptr = (TOUCHE *) TtaGetMemory (sizeof (TOUCHE));	/* nouvelle entree */
   oldptr = *addebut;		/* debut chainage entrees existantes */
   /* Regarde si l'on a deja une touche ctrl */
   if (oldptr == NULL)
     {
	/* C'est la premiere touche crtl cree */
	*addebut = ptr;
	existe = False;		/* la cle1 n'est pas encore connue */
     }
   else
     {
	/* Regarde si cette touche est deja enregistree */
	existe = False;
	do
	  {
	     /* Est-ce la meme cle d'entree */
	     if (oldptr->T_Cle == cle1)
		existe = True;	/* L'entree cle1 existe deja */
	     else if (oldptr->T_Autre != NULL)
	       {
		  oldptr = oldptr->T_Autre;
		  if (oldptr->T_Cle == cle1)
		     existe = True;	/* Il faut en plus verifier la derniere entree */
	       }
	  }
	while (oldptr->T_Autre != NULL && !existe);
     }

   /* Est-ce une sequence a 2 cles avec modifieur sur la 1ere ? */
   if ((cle2 != 0) && (mod1 != THOT_SANS_MOD))
     {
	/* Est-ce que l'entree de 1er niveau existe deja ? */
	if (!existe)
	  {
	     /* Creation d'une entree d'automate de 1er niveau */
	     ptr->T_Cle = cle1;
	     ptr->T_Suite = NULL;
	     ptr->T_Autre = NULL;
	     ptr->T_Commande = 0;
	     ptr->T_Valeur = 0;

	     /* Chainage a l'entree precedente */
	     if (oldptr != NULL)
		oldptr->T_Autre = ptr;

	     oldptr = ptr;
	     ptr = NULL;
	  }

	/* Il faut parcourir les entrees de 2eme niveau */
	if (oldptr->T_Suite == NULL)
	  {
	     /* creation d'une 1ere entree de 2eme niveau */
	     ptr = (TOUCHE *) TtaGetMemory (sizeof (TOUCHE));
	     ptr->T_Cle = cle2;
	     ptr->T_Modifieur = (TOUCHE *) mod2;
	     ptr->T_Autre = NULL;
	     ptr->T_Commande = commande;
	     ptr->T_Valeur = cle;
	     oldptr->T_Suite = ptr;
	     existe = True;
	  }
	else
	  {
	     oldptr = oldptr->T_Suite;
	     existe = False;
	     do
	       {
		  /* Est-ce la meme cle d'entree */
		  if (oldptr->T_Cle == cle2)
		     /* L'entree existe deja */
		     existe = True;
		  else if (oldptr->T_Autre != NULL)
		    {
		       oldptr = oldptr->T_Autre;
		       /* Il faut en plus verifier la derniere entree */
		       if (oldptr->T_Cle == cle2)
			  existe = True;
		    }
	       }
	     while (oldptr->T_Autre != NULL && !existe);
	  }

	/* Si l'entree de 2eme niveau n'existe pas deja ? */
	if (!existe)
	  {
	     /* Creation d'une entree d'automate de 2eme niveau */
	     ptr = (TOUCHE *) TtaGetMemory (sizeof (TOUCHE));
	     ptr->T_Cle = cle2;
	     ptr->T_Modifieur = (TOUCHE *) mod2;
	     ptr->T_Autre = NULL;
	     ptr->T_Commande = commande;
	     ptr->T_Valeur = cle;

	     /* Chainage a l'entree precedente */
	     if (oldptr != NULL)
		oldptr->T_Autre = ptr;
	  }
     }
   else
     {
	/* on cree une entree de premier niveau */
	ptr->T_Cle = cle1;
	ptr->T_Modifieur = (TOUCHE *) mod1;
	ptr->T_Autre = NULL;
	ptr->T_Suite = NULL;
	ptr->T_Commande = commande;
	ptr->T_Valeur = cle;

	/* Chainage a l'entree precedente */
	if (oldptr != NULL)
	   oldptr->T_Autre = ptr;
     }
}				/*MemoTouche */


#ifdef NEW_WILLOWS
/* ---------------------------------------------------------------------- */
/* |    MSCharTranslation is the MS-Window front-end to the character   | */
/* |       translation and handling. Decodes the MS-Window callback     | */
/* |       parameters and calls the generic character handling routine. | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                MSCharTranslation (HWND hWnd, int frame, UINT msg,
				       WPARAM wParam, LPARAM lParam)
#endif				/* __STDC__ */
{
   int                 keyboard_mask = 0;
   char                chaine[2];
   int                 len = 0;

   if ((msg != WM_KEYDOWN) && (msg != WM_CHAR))
      return;

   if (frame < 0)
     {
	fprintf (stderr, "unable to get frame of window %X\n", hWnd);
	DebugBreak ();
	return;
     }
   if (GetKeyState (VK_SHIFT))
      keyboard_mask |= THOT_MOD_SHIFT;
   if (GetKeyState (VK_CONTROL))
      keyboard_mask |= THOT_MOD_CTRL;
   if (GetKeyState (VK_MENU))
      keyboard_mask |= THOT_MOD_ALT;

   if (msg == WM_CHAR)
     {
	chaine[0] = (char) wParam;
	len = 1;
     }

   MaTranslation (frame, &chaine[0], len, keyboard_mask, wParam);
}
#endif /* NEW_WILLOWS */


#ifdef WWW_XWINDOWS
/* ---------------------------------------------------------------------- */
/* |    XCharTranslation is the X-Window front-end to the character     | */
/* |         translation and handling. Decodes the X-Window event       | */
/* |         and calls the generic character handling routine.          | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                XCharTranslation (XEvent * event)
#else  /* __STDC__ */
void                XCharTranslation (event)
XEvent             *event;

#endif /* __STDC__ */
{
   int                 i, retour;
   int                 mask;
   int                 frame;
   unsigned char       chaine[2];
   XComposeStatus      ComS;
   KeySym              KS, KS1;

   frame = GetFenetre (event->xany.window);
   if (frame > MAX_FRAME)
      frame = 0;

   i = event->xkey.state;
   if (Automate_current == NULL)
     {
	retour = XLookupString ((XKeyEvent *) event, chaine, 2, &KS, &ComS);
	if (!retour)
	  {
	     event->xkey.state = i & (ShiftMask);
	     retour = XLookupString ((XKeyEvent *) event, chaine, 2, &KS1, &ComS);
	     if (retour)
		KS = KS1;
	  }
     }
   else
     {
	/* On est entre dans l'automate, pas d'interpretation des modifieurs */
	event->xkey.state = i & ShiftMask;
	retour = XLookupString ((XKeyEvent *) event, chaine, 2, &KS, &ComS);
     }

   mask = 0;
   if (i & ShiftMask)
      mask |= THOT_MOD_SHIFT;
   if (i & LockMask)
      mask |= THOT_MOD_SHIFT;
   if (i & ControlMask)
      mask |= THOT_MOD_CTRL;
   if (i & Mod1Mask)
      mask |= THOT_MOD_META;
   if (i & Mod2Mask)
      mask |= THOT_MOD_ALT;

   MaTranslation (frame, &chaine[0], retour, mask, KS);
}
#endif /* WWW_XWINDOWS */

/* ---------------------------------------------------------------------- */
/* |    MaTranslation handle the caracter encoding.                     | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                MaTranslation (int frame, unsigned char *chaine, unsigned int nb,
				   int mask, int key)
#else  /* __STDC__ */
void                MaTranslation (frame, chaine, nb, mask, key)
int                 frame;
unsigned char      *chaine;
unsigned int        nb;
int                 mask;
int                 key;

#endif /* __STDC__ */
{
   int                 valeur;
   int                 modtype;
   int                 commande;
   TOUCHE             *ptr;
   boolean             trouve;
   Document            document;
   View                vue;

   if (frame > MAX_FRAME)
      frame = 0;

   valeur = chaine[0];
   trouve = False;
   if (nb == 2)
     {
	/* C'est l'appel d'une action Thot */
	commande = (int) chaine[1];
	trouve = True;
     }
   else
     {
	commande = 0;
	/* Est-on entre dans un automate ? */
	if (Automate_current != NULL)
	  {
	     /* Recheche l'entree de 2eme niveau */
	     ptr = Automate_current;
	     Automate_current = NULL;

	     /* Teste s'il y a un modifieur en jeu */
	     if (mask & THOT_MOD_CTRL)
		if (mask & THOT_MOD_SHIFT)
		   modtype = THOT_MOD_S_CTRL;
		else
		   modtype = THOT_MOD_CTRL;
	     else if (mask & THOT_MOD_META)
		if (mask & THOT_MOD_SHIFT)
		   modtype = THOT_MOD_S_META;
		else
		   modtype = THOT_MOD_META;
	     else if (mask & THOT_MOD_ALT)
		if (mask & THOT_MOD_SHIFT)
		   modtype = THOT_MOD_S_ALT;
		else
		   modtype = THOT_MOD_ALT;
	     else if (mask & THOT_MOD_SHIFT)
		modtype = THOT_MOD_SHIFT;
	     else
		modtype = THOT_SANS_MOD;

	     /* Recherche l'entree de 1er niveau */
	     while (!trouve && ptr != NULL)
		if (ptr->T_Cle == key && modtype == (int) ptr->T_Modifieur)
		   trouve = True;
		else
		   ptr = ptr->T_Autre;

	     if (trouve)
	       {
		  valeur = (unsigned char) ptr->T_Valeur;
		  commande = ptr->T_Commande;
	       }
	  }
	else
	  {
	     /* Faut-il parcourir un automate de 1er niveau ? */
	     /* Teste s'il y a un modifieur en jeu */
	     if (mask & THOT_MOD_CTRL)
		if (mask & THOT_MOD_SHIFT)
		   ptr = Automate_CTRL;
		else
		   ptr = Automate_ctrl;
	     else if (mask & THOT_MOD_META)
		if (mask & THOT_MOD_SHIFT)
		   ptr = Automate_META;
		else
		   ptr = Automate_meta;
	     else if (mask & THOT_MOD_ALT)
		if (mask & THOT_MOD_SHIFT)
		   ptr = Automate_ALT;
		else
		   ptr = Automate_alt;
	     else
		ptr = Automate_normal;

	     /* Recherche l'entree de 1er niveau */
	     while (!trouve && ptr != NULL)
		if (ptr->T_Cle == key)
		  {
		     /* On entre dans un automate */
		     trouve = True;
		     Automate_current = ptr->T_Suite;
		     if (Automate_current == NULL)
		       {
			  /* il s'agit d'une valeur definie a premier niveau */
			  valeur = (unsigned char) ptr->T_Valeur;
			  commande = ptr->T_Commande;
		       }
		  }
		else
		   ptr = ptr->T_Autre;
	  }
     }

   if (!trouve)
      /* Traitement des cles speciales */
      switch (key)
	    {
	       case THOT_KEY_Up:
#ifdef THOT_KEY_R8
	       case THOT_KEY_R8:
#endif
		  commande = SpecialKeys[MY_KEY_Up];
		  Automate_current = NULL;
		  break;
	       case THOT_KEY_Left:
#ifdef THOT_KEY_R10
	       case THOT_KEY_R10:
#endif
		  commande = SpecialKeys[MY_KEY_Left];
		  Automate_current = NULL;
		  break;
	       case THOT_KEY_Right:
#ifdef THOT_KEY_R12
	       case THOT_KEY_R12:
#endif
		  commande = SpecialKeys[MY_KEY_Right];
		  Automate_current = NULL;
		  break;
	       case THOT_KEY_Down:
#ifdef THOT_KEY_R14
	       case THOT_KEY_R14:
#endif
		  commande = SpecialKeys[MY_KEY_Down];
		  Automate_current = NULL;
		  break;
	       case THOT_KEY_Prior:
#ifdef THOT_KEY_R9
	       case THOT_KEY_R9:
#endif
		  commande = SpecialKeys[MY_KEY_Prior];
		  Automate_current = NULL;
		  break;
	       case THOT_KEY_Next:
#ifdef THOT_KEY_R15
	       case THOT_KEY_R15:
#endif
		  commande = SpecialKeys[MY_KEY_Next];
		  Automate_current = NULL;
		  break;
	       case THOT_KEY_Home:
#ifdef THOT_KEY_R7
	       case THOT_KEY_R7:
#endif
		  commande = SpecialKeys[MY_KEY_Home];
		  Automate_current = NULL;
		  break;
	       case THOT_KEY_End:
#ifdef THOT_KEY_R13
	       case THOT_KEY_R13:
#endif
		  commande = SpecialKeys[MY_KEY_End];
		  Automate_current = NULL;
		  break;
	       case THOT_KEY_BackSpace:
		  commande = SpecialKeys[MY_KEY_BackSpace];
		  Automate_current = NULL;
		  break;
	       case THOT_KEY_Delete:
		  commande = SpecialKeys[MY_KEY_Delete];
		  Automate_current = NULL;
		  break;
	       default:
		  break;
	    }

   if (Automate_current == NULL)
     {
	/* Appel d'une action Thot */
	VueDeFenetre (frame, &document, &vue);
	if (commande > 0)
	  {
	     /* Termine l'insertion eventuellement en cours */
	     if (commande != CMD_DeletePrevChar)
		/* Ce n'est pas un delete, il faut terminer l'insertion courante */
		EndInsert ();
	     /* Faut-il passer un parametre ? */
	     if (MenuActionList[commande].ActionActive[frame])
	       {
		  /* l'action est active pour la fenetre courante */
		  /* prepare les parametres */
		  if (MenuActionList[commande].Call_Action != NULL)
		     (*MenuActionList[commande].Call_Action) (document, vue);
	       }
	  }
	/* Rien a inserer */
	else if (nb == 0)
	   return;
	/* Traitemet des caracteres au cas par cas */
	else
	  {
	     if (!StructSelectionMode && !FntrTable[frame - 1].FrSelectOnePosition)
	       {
		  /* Delete the current selection */
		  EndInsert ();
		  if (MenuActionList[CMD_DeleteSelection].Call_Action != NULL)
		     (*MenuActionList[CMD_DeleteSelection].Call_Action) (document, vue);
	       }
	     else if (valeur == 8 || valeur == 127)
	       {
		  /* Par defaut BackSpace detruit le caractere precedent */
		  /* sans se soucier de la langue courante */
		  if (MenuActionList[CMD_DeletePrevChar].Call_Action != NULL)
		     (*MenuActionList[CMD_DeletePrevChar].Call_Action) (document, vue);
		  return;
	       }

/*** Sequence de traitement des espaces ***/
	     if (valeur == SAUT_DE_LIGNE
		 || (InputSpace && valeur == SeeCtrlRC))
	       {
		  if (MenuActionList[0].Call_Action)
		     (*MenuActionList[0].Call_Action) (document, vue, SAUT_DE_LIGNE);
	       }
	     else if (valeur == FINE || (InputSpace && valeur == SeeFine))
	       {
		  if (MenuActionList[0].Call_Action)
		     (*MenuActionList[0].Call_Action) (document, vue, FINE);
	       }
	     else if (valeur == DEMI_CADRATIN
		      || (InputSpace && valeur == SeeDemiCadratin))
	       {
		  if (MenuActionList[0].Call_Action)
		     (*MenuActionList[0].Call_Action) (document, vue, DEMI_CADRATIN);
	       }
	     else if (valeur == BLANC_DUR
		      || (InputSpace && valeur == SeeBlancDur))
	       {
		  if (MenuActionList[0].Call_Action)
		     (*MenuActionList[0].Call_Action) (document, vue, BLANC_DUR);
	       }
	     else if ((InputSpace && valeur == SeeBlanc))
	       {
		  if (MenuActionList[0].Call_Action)
		     (*MenuActionList[0].Call_Action) (document, vue, BLANC);
	       }

	     else if ((valeur >= 32 && valeur < 128)
		      || (valeur >= 144 && valeur < 256))
		/* on insere un caractere valide quelque soit la langue */
	       {
		  if (MenuActionList[0].Call_Action)
		     (*MenuActionList[0].Call_Action) (document, vue, valeur);
	       }
	  }
     }
}				/*MaTranslation */


#ifdef WWW_XWINDOWS
/* ---------------------------------------------------------------------- */
/* |    InitTranslations initialise l'encodage du clavier.              | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
XtTranslations      InitTranslations (char *appliname)
#else  /* __STDC__ */
XtTranslations      InitTranslations (appliname)
char               *appliname;

#endif /* __STDC__ */
{
   char               *texte;	/* fichier de translation transmis a motif */
   char                ligne[200];	/* ligne en construction pour motif */
   char                home[200], name[80], ch[80], *adr;
   char                equiv[MAX_EQUIV];	/* chaine des equivalents caracteres pour motif */
   unsigned int        cle1, cle2;	/* 1ere et 2 eme cles sous forme de keysym X */
   int                 e, i;
   int                 mod1, mod2;	/* 1er et 2eme modifieurs : voir definitions THOT_MOD_xx */
   int                 lg, max;
   FILE               *fich;
   XtTranslations      table;

   texte = TtaGetEnvString ("HOME");
   strcpy (name, appliname);
   strcat (name, ".keyboard");

   if (texte != NULL)
     {
	strcpy (home, texte);
	strcat (home, DIR_STR);
	strcat (home, name);
	if (!SearchFile (home, 0, ligne))
	   SearchFile (name, 2, ligne);
     }
   else
      SearchFile (name, 2, ligne);

   fich = fopen (ligne, "r");
   if (!fich)
     {
	/*Fichier inaccessible */
	TtaDisplaySimpleMessage (INFO, LIB, LIB_NO_THOT_KEYBOARD);
	return NULL;
     }
   else
     {
	/* Lecture du fichier des translations */
	fseek (fich, 0L, 2);	/* fin du fichier */
	lg = ftell (fich) * 2 + 10;	/* pour assurer de la marge */
	texte = TtaGetMemory (lg);
	fseek (fich, 0L, 0);	/* debut du fichier */

	/* Initialise la lecture du fichier */
	e = 1;
	max = MaxMenuAction;
	lg = strlen (CST_InsertChar) + 2;

	/* FnCopy la premiere ligne du fichier (#override, ou #...) */
	strcpy (texte, "#override\n");
	fscanf (fich, "%80s", ch);
	do
	  {
	     /* Initialisations */
	     mod1 = mod2 = THOT_SANS_MOD;
	     cle1 = cle2 = 0;
	     strcpy (ligne, "!");	/* initialisation de la ligne */

	     /* Est-ce la fin de fichier ? */
	     if (strlen (ch) == 0 || FinDeChaine (ch, "^"))
		e = 0;
	     else if (ch[0] != '#')
		/* it is not a comment */
		/* -------> Lecture des autres champs */
	       {
		  if (!strcmp (ch, "Shift"))
		    {
		       mod1 = THOT_MOD_SHIFT;
		       strcpy (equiv, "Shift+");
		       strcat (ligne, ch);	/* copie 1er modifieur */
		       strcat (ligne, " ");
		       /* Lecture enventuelle d'un deuxieme modifieur */
		       fscanf (fich, "%80s", ch);
		    }
		  else
		    {
		       mod1 = THOT_SANS_MOD;
		       equiv[0] = '\0';
		    }

		  if (!strcmp (ch, "Ctrl"))
		     mod1 += THOT_MOD_CTRL;
		  else if (!strcmp (ch, "Alt"))
		     mod1 += THOT_MOD_ALT;
		  else if (!strcmp (ch, "Meta"))
		     mod1 += THOT_MOD_META;

		  /* Lecture de la cle */
		  if (mod1 != THOT_SANS_MOD && mod1 != THOT_MOD_SHIFT)
		    {
		       strcat (equiv, ch);
		       strcat (equiv, "+");
		       strcat (ligne, ch);	/* copie 2eme modifieur */
		       strcat (ligne, " ");
		       strcpy (ch, "");
		       fscanf (fich, "%80s", ch);
		    }

		  /* Extrait la valeur de la cle */
		  name[0] = '\0';
		  sscanf (ch, "<Key>%80s", name);
		  if (name[0] != '\0')
		    {
		       strcat (ligne, "<Key>");		/* copie de la cle */
		       i = strlen (name);
		       /* Elimine le : a la fin du nom */
		       if ((name[i - 1] == ':') && i != 1)
			 {
			    /* Il faut engendrer un : apres le nom */
			    name[i - 1] = '\0';
			    i = 1;
			 }
		       else
			  i = 0;

		       strcat (ligne, NomCle (name));	/* copie le nom normalise */
		       if (i == 1)
			  strcat (ligne, ": ");
		       else
			  strcat (ligne, " ");
		    }

		  /* convertion vers keysym pour l'automate */
		  cle1 = CleSpecial (name);
		  strcat (equiv, name);

		  /* Lecture eventuelle d'une deuxieme composition */
		  strcpy (name, "");
		  fscanf (fich, "%80s", name);
		  if (name[0] == ',')
		    {
		       strcat (ligne, ", ");	/* copie du separateur */
		       strcpy (ch, "");
		       fscanf (fich, "%80s", ch);

		       if (!strcmp (ch, "Shift"))
			 {
			    mod2 = THOT_MOD_SHIFT;
			    strcat (ligne, ch);		/* copie du 1er modifieur */
			    strcat (ligne, " ");
			    strcat (equiv, " Shift+");
			    /* Lecture enventuelle d'un deuxieme modifieur */
			    strcpy (ch, "");
			    fscanf (fich, "%80s", ch);
			 }
		       else
			 {
			    mod2 = THOT_SANS_MOD;
			    strcat (equiv, " ");
			 }

		       if (!strcmp (ch, "Ctrl"))
			  mod2 += THOT_MOD_CTRL;
		       else if (!strcmp (ch, "Alt"))
			  mod2 += THOT_MOD_ALT;
		       else if (!strcmp (ch, "Meta"))
			  mod2 += THOT_MOD_META;

		       /* Lecture de la cle */
		       if (mod2 != THOT_SANS_MOD && mod2 != THOT_MOD_SHIFT)
			 {
			    strcat (equiv, ch);
			    strcat (equiv, "+");
			    strcat (ligne, ch);		/* copie 2eme modifieur */
			    strcat (ligne, " ");
			    strcpy (ch, "");
			    fscanf (fich, "%80s", ch);
			    strcat (ligne, ch);		/* copie de la cle */
			    strcat (ligne, " ");
			 }
		       /* Extrait la valeur de la cle */
		       strcpy (name, "");
		       sscanf (ch, "<Key>%80s", name);
		       if (name != "")
			 {
			    strcat (ligne, "<Key>");	/* copie de la cle */
			    i = strlen (name);
			    /* Elimine le : a la fin du nom */
			    if (name[i - 1] == ':' && i != 1)
			      {
				 /* Il faut engendrer un : apres le nom */
				 name[i - 1] = '\0';
				 i = 1;
			      }
			    else
			       i = 0;
			    strcat (ligne, NomCle (name));	/* copie le nom normalise */
			    if (i == 1)
			       strcat (ligne, ": ");
			    else
			       strcat (ligne, " ");
			 }
		       cle2 = CleSpecial (name);
		       strcat (equiv, name);

		       /* Lecture de l'action */
		       strcpy (name, "");
		       fscanf (fich, "%80s", name);
		    }

		  /* Isole l'intitule de la commande */
		  strncpy (ch, name, 80);
		  adr = strchr (ch, '(');
		  if (adr == NULL)
		     i = max;
		  else
		    {
		       adr[0] = '\0';
		       /* Selection de la bonne commande */
		       for (i = 0; i < max; i++)
			  if (!strcmp (ch, MenuActionList[i].ActionName))
			     break;
		    }

		  /* Est-ce une translation valable pour le texte Motif */
		  if (i <= 8)
		    {
		       /* FnCopy la ligne dans le source de la table de translations */
		       strcat (texte, ligne);
		       if (!strcmp (ch, CST_InsertChar))
			 {
			    strcat (texte, "insert-string(");
			    strcat (texte, TransCani (&adr[1]));
			 }
		       else if (!strcmp (ch, CST_DeleteSelection))
			  strcat (texte, "delete-selection()");
		       else if (!strcmp (ch, CST_DeletePrevChar))
			  strcat (texte, "delete-prev-char()");
		       else if (!strcmp (ch, CST_BackwardChar))
			  strcat (texte, "backward-char()");
		       else if (!strcmp (ch, CST_ForwardChar))
			  strcat (texte, "forward-char()");
		       else if (!strcmp (ch, CST_PreviousLine))
			  strcat (texte, "previous-line()");
		       else if (!strcmp (ch, CST_NextLine))
			  strcat (texte, "next-line()");
		       else if (!strcmp (ch, CST_BeginningOfLine))
			  strcat (texte, "beginning-of-line()");
		       else if (!strcmp (ch, CST_EndOfLine))
			  strcat (texte, "end-of-line()");
		       strcat (texte, "\n");
		    }

		  if (i == 0)
		    {
		       /* C'est l'action insert-string */
		       /* FnCopy la ligne dans le source de la table de translations */
		       strcat (texte, ligne);
		       strcat (texte, TransCani (name));
		       strcat (texte, "\n");
		       /* C'est un encodage de caractere */
		       adr = TransCani (&name[lg]);
		       MemoTouche (mod1, cle1, mod2, cle2, (unsigned int) adr[0], 0);
		    }
		  else if (i < max)
		    {
		       /* C'est une autre action Thot */
		       MemoTouche (mod1, cle1, mod2, cle2, /*255+i */ 0, i);
		       /* On met a jour l'equivalent clavier */
		       MenuActionList[i].ActionEquiv = TtaGetMemory (strlen (equiv) + 1);
		       strcpy (MenuActionList[i].ActionEquiv, equiv);
		    }
	       }
	     strcpy (ch, "");
	     fscanf (fich, "%80s", ch);
	  }
	while (e != 0);

	fclose (fich);

	/* Creation de la table de translation */
	table = XtParseTranslationTable (texte);
	TtaFreeMemory (texte);
	return table;
     }				/*else */
}				/*InitTranslation */

#endif /* WWW_XWINDOWS */
