/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * Warning:
 * This module is part of the Thot library, which was originally
 * developed in French. That's why some comments are still in
 * French, but their translation is in progress and the full module
 * will be available in English in the next release.
 * 
 */
 
/*
 * 
 * Module for keyboard input handling.
 * Authors: I. Vatton (INRIA)
 *          D. Veillard (INRIA) - Removed X remapping of keys,
 *                                lead to crash in some configurations
 *          R. Guetari (W3C/INRIA) - Windows routines.
 *
 */

#include "thot_gui.h"
#include "thot_sys.h"
#include "thot_key.h"
#include "constmedia.h"
#include "typemedia.h"
#include "libmsg.h"
#include "message.h"
#include "appdialogue.h"

#define MAX_EQUIV 25
#define THOT_NO_MOD	0
#define THOT_MOD_CTRL	1
#define THOT_MOD_META	2
#define THOT_MOD_ALT	4
#define THOT_MOD_SHIFT	10
#define THOT_MOD_S_CTRL	11
#define THOT_MOD_S_META	12
#define THOT_MOD_S_ALT	14
#define MAX_AUTOMATA	80
/* automata structure for the keys */
typedef struct _key
{
  int                 K_EntryCode;    /* input key                           */
  short               K_Command;      /* index in the command list or -1     */
  int                 K_Value;	      /* return key if command = -1          */
  struct _key        *K_Other;	      /* next entry at the same level        */
  union
  {
    struct
    {
      struct _key    *_K_Next_;	      /* 1st complementary touch (1st level) */
    } s0;
    struct
    {
      int             _K_Modifier_;   /* modifier value (2nd level)          */
    } s1;
  } u;
}
KEY;
#define K_Next u.s0._K_Next_
#define K_Modifier u.s1._K_Modifier_

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "boxes_tv.h"
#include "frame_tv.h"
#include "edit_tv.h"
#include "appdialogue_tv.h"

/* Actions table */
#include "appli_f.h"
#include "applicationapi_f.h"
#include "context_f.h"
#include "editcommands_f.h"
#include "input_f.h"
#include "memory_f.h"
#include "message_f.h"
#include "registry_f.h"
#include "structcommands_f.h"
#include "structmodif_f.h"
#include "structselect_f.h"
#include "textcommands_f.h"
#include "windowdisplay_f.h"

/* Default actions for XK_Up, XK_Left, XK_Right, XK_Down keys */
#define MY_KEY_Up        0
#define MY_KEY_Left      1
#define MY_KEY_Right     2
#define MY_KEY_Down      3
#define MY_KEY_BackSpace 4
#define MY_KEY_Delete    5
#define MY_KEY_Prior     6
#define MY_KEY_Next      7
#define MY_KEY_Home      8
#define MY_KEY_End       9
static int          SpecialKeys[] =
{5, 3, 4, 6, 1, 2, 13, 14, 15, 16};

/* the automata */
static KEY         *Automata_normal = NULL;

static KEY         *Automata_ctrl    = NULL;
static KEY         *Automata_meta    = NULL;
static KEY         *Automata_alt     = NULL;
static KEY         *Automata_CTRL    = NULL;
static KEY         *Automata_META    = NULL;
static KEY         *Automata_ALT     = NULL;
static KEY         *Automata_current = NULL;

#ifdef _WINDOWS
static BOOL specialKey;
#endif /* _WINDOWS */

/*----------------------------------------------------------------------
   NameCode
   translates the keynames not supported by the interpreter of
   Motif translations.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static char        *NameCode (char *name)

#else  /* __STDC__ */
static char        *NameCode (name)
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
}				/*NameCode */


/*----------------------------------------------------------------------
   SpecialKey
   translates the name given by the file thot.keyboard into a key value
   which Thot can use.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static unsigned int SpecialKey (char *name)
#else  /* __STDC__ */
static unsigned int SpecialKey (name)
char               *name;

#endif /* __STDC__ */
{
   /* is it the name of a special character? */
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
}				/*SpecialKey */



/*----------------------------------------------------------------------
   MemoKey
   memorizes a keyboard shortcut inside the automata.
   mod1 = 1st modifier                                   
   key1 = 1st key                                         
   key2 = 2nd modifier
   key2 = 2nd key                                       
   key  = keysym value or 0
   command = number of the command in MyActions
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         MemoKey (int mod1, int key1, int mod2, int key2, int key, int command)
#else  /* __STDC__ */
static void         MemoKey (mod1, key1, mod2, key2, key, command)
int                 mod1;
int                 key1;
int                 mod2;
int                 key2;
int                 key;
int                 command;

#endif /* __STDC__ */
{
   boolean             exists;
   KEY                *ptr = NULL;
   KEY                *oldptr;
   KEY               **addFirst;

   if (key1 == 0)
      return;

   /* Entry point into the automata */
   switch (mod1)
	 {
	    case THOT_NO_MOD:
	       addFirst = &Automata_normal;
	       break;
	    case THOT_MOD_CTRL:
	       addFirst = &Automata_ctrl;
#          ifdef _WINDOWS
           if (key1 >= 'a' && key1 <= 'z')
              key1 = key1 - 'a' + 1;
#          endif /* _WINDOWS */
	       break;
	    case THOT_MOD_ALT:
	       addFirst = &Automata_alt;
	       break;
	    case THOT_MOD_META:
	       addFirst = &Automata_meta;
	       break;
	    case THOT_MOD_S_CTRL:
	       addFirst = &Automata_CTRL;
	       break;
	    case THOT_MOD_S_ALT:
	       addFirst = &Automata_ALT;
	       break;
	    case THOT_MOD_S_META:
	       addFirst = &Automata_META;
	       break;
	    default:
	       return;
	 }

   /* Initializations */
   ptr = (KEY *) TtaGetMemory (sizeof (KEY));	/* nouvelle entree */
   oldptr = *addFirst;		/* debut chainage entrees existantes */
   /* Verifies if we already have a ctrl key */
   if (oldptr == NULL)
     {
	/* It's the first ctrl key we'll create */
	*addFirst = ptr;
	exists = FALSE;		/* key1 isn't yet known */
     }
   else
     {
	/* verifies if this key is already recorded */
	exists = FALSE;
	do
	  {
	     /* is it the same entry key ? */
	     if (oldptr->K_EntryCode == key1)
		exists = TRUE;	/* the key1 entry already exists */
	     else if (oldptr->K_Other != NULL)
	       {
		  oldptr = oldptr->K_Other;
		  if (oldptr->K_EntryCode == key1)
		     exists = TRUE;	/* we must also verify the last entry */
	       }
	  }
	while (oldptr->K_Other != NULL && !exists);
     }

   /* is it a two key sequence with a modifier on the first one? */
   if ((key2 != 0) && (mod1 != THOT_NO_MOD))
     {
	/* Does the first level entry already exists ? */
	if (!exists)
	  {
	     /* Creation d'une entree d'automate de 1er niveau */
	     ptr->K_EntryCode = key1;
	     ptr->K_Next = NULL;
	     ptr->K_Other = NULL;
	     ptr->K_Command = 0;
	     ptr->K_Value = 0;

	     /* Chainage a l'entree precedente */
	     if (oldptr != NULL)
		oldptr->K_Other = ptr;

	     oldptr = ptr;
	     ptr = NULL;
	  }

	/* Il faut parcourir les entrees de 2eme niveau */
	if (oldptr->K_Next == NULL)
	  {
	     /* creation d'une 1ere entree de 2eme niveau */
	     if (ptr == NULL)
	       ptr = (KEY *) TtaGetMemory (sizeof (KEY));
	     ptr->K_EntryCode = key2;
	     ptr->K_Modifier = mod2;
	     ptr->K_Other = NULL;
	     ptr->K_Command = command;
	     ptr->K_Value = key;
	     oldptr->K_Next = ptr;
	     exists = TRUE;
	  }
	else
	  {
	     oldptr = oldptr->K_Next;
	     exists = FALSE;
	     do
	       {
		  /* Est-ce la meme cle d'entree */
		  if (oldptr->K_EntryCode == key2)
		     /* L'entree existe deja */
		     exists = TRUE;
		  else if (oldptr->K_Other != NULL)
		    {
		       oldptr = oldptr->K_Other;
		       /* Il faut en plus verifier la derniere entree */
		       if (oldptr->K_EntryCode == key2)
			  exists = TRUE;
		    }
	       }
	     while (oldptr->K_Other != NULL && !exists);
	  }

	/* Si l'entree de 2eme niveau n'existe pas deja ? */
	if (!exists)
	  {
	     /* Creation d'une entree d'automate de 2eme niveau */
	     if (ptr == NULL)
	       ptr = (KEY *) TtaGetMemory (sizeof (KEY));
	     ptr->K_EntryCode = key2;
	     ptr->K_Modifier = mod2;
	     ptr->K_Other = NULL;
	     ptr->K_Command = command;
	     ptr->K_Value = key;

	     /* Chainage a l'entree precedente */
	     if (oldptr != NULL)
		oldptr->K_Other = ptr;
	  }
     }
   else
     {
	/* on cree une entree de premier niveau */
	ptr->K_EntryCode = key1;
	ptr->K_Modifier = mod1;
	ptr->K_Other = NULL;
	ptr->K_Next = NULL;
	ptr->K_Command = command;
	ptr->K_Value = key;

	/* Chainage a l'entree precedente */
	if (oldptr != NULL)
	   oldptr->K_Other = ptr;
     }
}


#ifdef _WINDOWS
/*----------------------------------------------------------------------
   MSCharTranslation
   MS-Window front-end to the character translation and handling.
   Decodes the MS-Window callback parameters and calls the
   generic character handling function. 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void WIN_CharTranslation (HWND hWnd, int frame, UINT msg, WPARAM wParam, LPARAM lParam)
#else  /* !__STDC__ */
void WIN_CharTranslation (hWnd, frame, msg, wParam, lParam)
HWND   hWnd; 
int    frame; 
UINT   msg; 
WPARAM wParam; 
LPARAM lParam;
#endif /* __STDC__ */
{   
   int  keyboard_mask = 0;   int  status;   char string[2];
   int  len = 0;

   if ((msg != WM_KEYDOWN) && (msg != WM_CHAR))
      return;

   if (msg == WM_CHAR)
      specialKey = FALSE;
   else
      specialKey = TRUE;

   if (frame < 0) {
	fprintf (stderr, "unable to get frame of window %X\n", hWnd);
	return;
   }

   status = GetKeyState (VK_SHIFT);
   if (HIBYTE (status))
      keyboard_mask |= THOT_MOD_SHIFT;

   status = GetKeyState (VK_CONTROL);
   if (HIBYTE (status))
      keyboard_mask |= THOT_MOD_CTRL;

   status = GetKeyState (VK_MENU);
   if (HIBYTE (status))
      keyboard_mask |= THOT_MOD_ALT;

   if ((wParam == VK_CANCEL) ||
       (wParam == VK_BACK)   ||
       (wParam == VK_RETURN) ||
       (wParam == VK_ESCAPE) ||
       (wParam == VK_PRIOR)  ||
       (wParam == VK_NEXT)   ||
       (wParam == VK_END)    ||
       (wParam == VK_HOME)   ||
       (wParam == VK_LEFT)   ||
       (wParam == VK_UP)     ||
       (wParam == VK_RIGHT)  ||
       (wParam == VK_DOWN)   ||
       (wParam == VK_INSERT) ||
       (wParam == VK_DELETE))
      len = 0;
   else
       len = 1;

   if (msg == WM_CHAR){
      len = 1;
      string[0] = (char) wParam;
      ThotInput (frame, &string[0], len, keyboard_mask, wParam);
   } else if ((wParam == VK_CANCEL)  ||
			  (wParam == VK_RETURN)  ||
			  (wParam == VK_ESCAPE)  ||
			  (wParam == VK_PRIOR)   ||
			  (wParam == VK_NEXT)    ||
			  (wParam == VK_END)     ||
			  (wParam == VK_HOME)    ||
			  (wParam == VK_LEFT)    ||
			  (wParam == VK_UP)      ||
			  (wParam == VK_RIGHT)   ||
			  (wParam == VK_DOWN)    ||
			  (wParam == VK_INSERT)  ||
			  (wParam == VK_DELETE))   
   {
	  string[0] = (char) wParam;
	  ThotInput (frame, &string[0], len, keyboard_mask, wParam);
   }
}
#endif /* _WINDOWS */


#ifndef _WINDOWS
/*----------------------------------------------------------------------
   XCharTranslation
   X-Window front-end to the character translation and handling.
   Decodes the X-Window event  and calls the generic character
   handling function.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                XCharTranslation (ThotEvent * event)
#else  /* __STDC__ */
void                XCharTranslation (event)
ThotEvent             *event;

#endif /* __STDC__ */
{
   int                 i, status;
   int                 PicMask;
   int                 frame;
   unsigned char       string[2];
   ThotComposeStatus      ComS;
   KeySym              KS, KS1;

   frame = GetWindowFrame (event->xany.window);
   if (frame > MAX_FRAME)
      frame = 0;

   i = event->xkey.state;
   if (Automata_current == NULL)
     {
	status = XLookupString ((ThotKeyEvent *) event, string, 2, &KS, &ComS);
	if (!status)
	  {
	     event->xkey.state = i & (ShiftMask);
	     status = XLookupString ((ThotKeyEvent *) event, string, 2, &KS1, &ComS);
	     if (status)
		KS = KS1;
	  }
     }
   else
     {
	/* On est entre dans l'automate, pas d'interpretation des modifieurs */
	event->xkey.state = i & ShiftMask;
	status = XLookupString ((ThotKeyEvent *) event, string, 2, &KS, &ComS);
     }

   PicMask = 0;
   if (i & ShiftMask)
      PicMask |= THOT_MOD_SHIFT;
   if (i & LockMask)
      PicMask |= THOT_MOD_SHIFT;
   if (i & ControlMask)
      PicMask |= THOT_MOD_CTRL;
   if (i & Mod1Mask)
      PicMask |= THOT_MOD_META;
   if (i & Mod2Mask)
      PicMask |= THOT_MOD_ALT;

   ThotInput (frame, &string[0], status, PicMask, KS);
}
#endif /* !_WINDOWS */

/*----------------------------------------------------------------------
   ThotInput
   handles the character encoding.                     
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ThotInput (int frame, unsigned char *string, unsigned int nb,
			       int PicMask, int key)
#else  /* __STDC__ */
void                ThotInput (frame, string, nb, PicMask, key)
int                 frame;
unsigned char      *string;
unsigned int        nb;
int                 PicMask;
int                 key;

#endif /* __STDC__ */
{
   int                 value;
   int                 modtype;
   int                 command;
   KEY                *ptr;
   boolean             found;
   Document            document;
   View                view;
#  ifdef _WINDOWS
   BOOL                endOfSearch = FALSE;
#  endif /* _WINDOWS */
   if (frame > MAX_FRAME)
      frame = 0;

#  ifdef _WINDOWS
   if (key == 13 && nb == 1)
      specialKey = FALSE;
   else if (key >= 1 && key <= 26 && nb == 1)
        specialKey = TRUE;
#  endif /* _WINDOWS */

   value = string[0];
   found = FALSE;
   if (nb == 2) {
      /* C'est l'appel d'une action Thot */
      command = (int) string[1];
      found = TRUE;
   } else {
        command = 0;
        /* Est-on entre dans un automate ? */
        if (Automata_current != NULL) {
           /* Recheche l'entree de 2eme niveau */
           ptr = Automata_current;
           Automata_current = NULL;
	   
           /* Teste s'il y a un modifieur en jeu */
           if (PicMask & THOT_MOD_CTRL)
              if (PicMask & THOT_MOD_SHIFT)
                 modtype = THOT_MOD_S_CTRL;
              else
                 modtype = THOT_MOD_CTRL;
           else if (PicMask & THOT_MOD_META)
                if (PicMask & THOT_MOD_SHIFT)
                   modtype = THOT_MOD_S_META;
                else
                   modtype = THOT_MOD_META;
           else if (PicMask & THOT_MOD_ALT)
                if (PicMask & THOT_MOD_SHIFT)
                   modtype = THOT_MOD_S_ALT;
                else
                   modtype = THOT_MOD_ALT;
           else if (PicMask & THOT_MOD_SHIFT)
                modtype = THOT_MOD_SHIFT;
           else
               modtype = THOT_NO_MOD;

           /* Recherche l'entree de 1er niveau */
           while (!found && ptr != NULL)
                 if (ptr->K_EntryCode == key && modtype == ptr->K_Modifier)
                    found = TRUE;
                 else
                    ptr = ptr->K_Other;

           if (found) {
              value = (unsigned char) ptr->K_Value;
              command = ptr->K_Command;
           }
        } else {
             /* Faut-il parcourir un automate de 1er niveau ? */
             /* Teste s'il y a un modifieur en jeu */
             if (PicMask & THOT_MOD_CTRL)
                if (PicMask & THOT_MOD_SHIFT)
                   ptr = Automata_CTRL;
                else
                   ptr = Automata_ctrl;
             else if (PicMask & THOT_MOD_META)
                  if (PicMask & THOT_MOD_SHIFT)
                     ptr = Automata_META;
                  else
                     ptr = Automata_meta;
             else if (PicMask & THOT_MOD_ALT)
                  if (PicMask & THOT_MOD_SHIFT)
                     ptr = Automata_ALT;
                  else
                     ptr = Automata_alt;
             else
                 ptr = Automata_normal;

             /* Recherche l'entree de 1er niveau */
#            ifdef _WINDOWS
             endOfSearch = FALSE;
             while (!endOfSearch && ptr != NULL) {
                   if (ptr->K_EntryCode == key) {
                      endOfSearch = TRUE;
                      if (specialKey) {
                         found = TRUE;
                         Automata_current = ptr->K_Next;
                         if (Automata_current == NULL) {
                            /* il s'agit d'une valeur definie a premier niveau */
                            value = (unsigned char) ptr->K_Value;
                            command = ptr->K_Command;
                         }
                      }
                   } else
                         ptr = ptr->K_Other;
             }
#            else  /* !_WINDOWS */
             while (!found && ptr != NULL)
                   if (ptr->K_EntryCode == key) {
                      /* On entre dans un automate */
                      found = TRUE;
                      Automata_current = ptr->K_Next;
                      if (Automata_current == NULL) {
                         /* il s'agit d'une valeur definie a premier niveau */
                         value = (unsigned char) ptr->K_Value;
                         command = ptr->K_Command;
                      }
                   } else
                         ptr = ptr->K_Other;
#            endif /* _WINDOWS */
        }
   }

#ifdef _WINDOWS
   if (specialKey && !found)
#else /* !_WINDOWS */
   if (!found)
#endif /* _WINDOWS */
     /* Traitement des cles speciales */
     switch (key)
       {
       case THOT_KEY_Up:
#      ifdef THOT_KEY_R8
       case THOT_KEY_R8:
#      endif
	 command = SpecialKeys[MY_KEY_Up];
	 Automata_current = NULL;
	 break;

       case THOT_KEY_Left:
#      ifdef THOT_KEY_R10
       case THOT_KEY_R10:
#      endif
	 command = SpecialKeys[MY_KEY_Left];
	 Automata_current = NULL;
	 break;

       case THOT_KEY_Right:
#      ifdef THOT_KEY_R12
       case THOT_KEY_R12:
#      endif
	 command = SpecialKeys[MY_KEY_Right];
	 Automata_current = NULL;
	 break;

       case THOT_KEY_Down:
#      ifdef THOT_KEY_R14
       case THOT_KEY_R14:
#      endif
	 command = SpecialKeys[MY_KEY_Down];
	 Automata_current = NULL;
	 break;

       case THOT_KEY_Prior:
#      ifdef THOT_KEY_R9
       case THOT_KEY_R9:
#      endif
	 command = SpecialKeys[MY_KEY_Prior];
	 Automata_current = NULL;
	 break;

       case THOT_KEY_Next:
#      ifdef THOT_KEY_R15
       case THOT_KEY_R15:
#      endif
	 command = SpecialKeys[MY_KEY_Next];
	 Automata_current = NULL;
	 break;

       case THOT_KEY_Home:
#      ifdef THOT_KEY_R7
       case THOT_KEY_R7:
#      endif
	 command = SpecialKeys[MY_KEY_Home];
	 Automata_current = NULL;
	 break;

       case THOT_KEY_End:
#      ifdef THOT_KEY_R13
       case THOT_KEY_R13:
#      endif
	 command = SpecialKeys[MY_KEY_End];
	 Automata_current = NULL;
	 break;

       case THOT_KEY_BackSpace:
	 command = SpecialKeys[MY_KEY_BackSpace];
	 Automata_current = NULL;
	 break;
	 
       case THOT_KEY_Delete:
	 command = SpecialKeys[MY_KEY_Delete];
	 Automata_current = NULL;
	 break;

       default: break;
       }

   if (Automata_current == NULL)
     {
       /* Appel d'une action Thot */
       FrameToView (frame, &document, &view);
       if (command > 0)
	 {
	   /* Termine l'insertion eventuellement en cours */
	   if (command != CMD_DeletePrevChar)
	     /* Ce n'est pas un delete, il faut terminer l'insertion courante */
	     CloseInsertion ();

	   /* Call action if it's active */
	   if (MenuActionList[command].ActionActive[frame])
	     {
	       /* l'action est active pour la fenetre courante */
	       /* prepare les parametres */
	       if (MenuActionList[command].User_Action != NULL)
		 {
		   if (((*MenuActionList[command].User_Action) (MenuActionList[command].User_Arg, document, view)) &&
		       (MenuActionList[command].Call_Action != NULL))
		     (*MenuActionList[command].Call_Action) (document, view);
		 }
	       else if (MenuActionList[command].Call_Action != NULL)
		 (*MenuActionList[command].Call_Action) (document, view);
	     }
	 }
       else if (nb == 0) /* Rien a inserer */      
	 return;
      /* Traitement des caracteres au cas par cas */
      else
	{
	  if (value == 8 || value == 127)
	    {
              /* Par defaut BackSpace detruit le caractere precedent */
              /* sans se soucier de la langue courante */
              if (MenuActionList[CMD_DeletePrevChar].User_Action != NULL)
		{
		  if (((*MenuActionList[CMD_DeletePrevChar].User_Action) (MenuActionList[CMD_DeletePrevChar].User_Arg, document, view)) &&
		      (MenuActionList[CMD_DeletePrevChar].Call_Action != NULL))
                    (*MenuActionList[CMD_DeletePrevChar].Call_Action) (document, view);
		}
	      else if (MenuActionList[CMD_DeletePrevChar].Call_Action != NULL)
		(*MenuActionList[CMD_DeletePrevChar].Call_Action) (document, view);
              return;
	    }

           /*** Sequence de traitement des espaces ***/
	  if (value == BREAK_LINE || (InputSpace && value == SHOWN_BREAK_LINE))
	    {
              if (MenuActionList[0].Call_Action)
		(*MenuActionList[0].Call_Action) (document, view, BREAK_LINE);
	    }
	  else if (value == THIN_SPACE || (InputSpace && value == SHOWN_THIN_SPACE))
	    {
	      if (MenuActionList[0].Call_Action)
		(*MenuActionList[0].Call_Action) (document, view, THIN_SPACE);
	    }
	  else if (value == HALF_EM || (InputSpace && value == SHOWN_HALF_EM))
	    {
	      if (MenuActionList[0].Call_Action)
		(*MenuActionList[0].Call_Action) (document, view, HALF_EM);
	    }
	  else if (value == UNBREAKABLE_SPACE || (InputSpace && value == SHOWN_UNBREAKABLE_SPACE))
	    {
	      if (MenuActionList[0].Call_Action)
		(*MenuActionList[0].Call_Action) (document, view, UNBREAKABLE_SPACE);
	    }
	  else if ((InputSpace && value == SHOWN_SPACE))
	    {
	      if (MenuActionList[0].Call_Action)
		(*MenuActionList[0].Call_Action) (document, view, _SPACE_);
	    }
	  else if ((value >= 32 && value < 128) || (value >= 144 && value < 256))
	    {
	      /* on insere un caractere valide quelque soit la langue */
	      if (MenuActionList[0].Call_Action)
		(*MenuActionList[0].Call_Action) (document, view, value);
	    }
	}
     }
}



/*----------------------------------------------------------------------
   InitTranslations
   intializes the keybord encoding.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotTranslations      InitTranslations (char *appliname)
#else  /* __STDC__ */
ThotTranslations      InitTranslations (appliname)
char               *appliname;

#endif /* __STDC__ */
{
   char               *text;	                          /* fichier de translation transmis a motif */
   char                line[200];	                  /* ligne en construction pour motif */
   char                home[200], name[80], ch[80], *adr;
   char                equiv[MAX_EQUIV];	          /* chaine des equivalents caracteres pour motif */
   unsigned int        key1, key2;	                  /* 1ere et 2 eme cles sous forme de keysym X */
   int                 e, i;
   int                 mod1, mod2;	                  /* 1er et 2eme modifieurs : voir definitions THOT_MOD_xx */
   int                 len, max;
   FILE               *file;
   ThotTranslations      table;

   text = TtaGetEnvString ("HOME");
   strcpy (name, appliname);
   strcat (name, ".keyboard");

   if (text != NULL)
     {
	strcpy (home, text);
	strcat (home, DIR_STR);
	strcat (home, name);
	if (!SearchFile (home, 0, line))
	   SearchFile (name, 2, line);
     }
   else
      SearchFile (name, 2, line);

   file = fopen (line, "r");
   if (!file)
     {
	/*Fichier inaccessible */
	TtaDisplaySimpleMessage (INFO, LIB, TMSG_NO_KBD);
	return NULL;
     }
   else
     {
	/* Lecture du fichier des translations */
	fseek (file, 0L, 2);	/* fin du fichier */
	len = ftell (file) * 2 + 10;	/* pour assurer de la marge */
	text = TtaGetMemory (len);
	fseek (file, 0L, 0);	/* debut du fichier */

	/* Initialise la lecture du fichier */
	e = 1;
	max = MaxMenuAction;
	len = strlen (CST_InsertChar) + 2;

	/* FnCopy la premiere ligne du fichier (#override, ou #...) */
	strcpy (text, "#override\n");
	fscanf (file, "%80s", ch);
	do
	  {
	     /* Initialisations */
	     mod1 = mod2 = THOT_NO_MOD;
	     key1 = key2 = 0;
	     strcpy (line, "!");	/* initialisation de la ligne */

	     /* Est-ce la fin de fichier ? */
	     if (strlen (ch) == 0 || EndOfString (ch, "^"))
		e = 0;
	     else if (ch[0] != '#')
		/* it is not a comment */
		/* -------> Lecture des autres champs */
	       {
		  if (!strcmp (ch, "Shift"))
		    {
		       mod1 = THOT_MOD_SHIFT;
		       strcpy (equiv, "Shift+");
		       strcat (line, ch);	/* copie 1er modifieur */
		       strcat (line, " ");
		       /* Lecture enventuelle d'un deuxieme modifieur */
		       fscanf (file, "%80s", ch);
		    }
		  else
		    {
		       mod1 = THOT_NO_MOD;
		       equiv[0] = EOS;
		    }

		  if (!strcmp (ch, "Ctrl"))
		     mod1 += THOT_MOD_CTRL;
		  else if (!strcmp (ch, "Alt"))
		     mod1 += THOT_MOD_ALT;
		  else if (!strcmp (ch, "Meta"))
		     mod1 += THOT_MOD_META;

		  /* Lecture de la cle */
		  if (mod1 != THOT_NO_MOD && mod1 != THOT_MOD_SHIFT)
		    {
		       strcat (equiv, ch);
		       strcat (equiv, "+");
		       strcat (line, ch);	/* copie 2eme modifieur */
		       strcat (line, " ");
		       strcpy (ch, "");
		       fscanf (file, "%80s", ch);
		    }

		  /* Extrait la valeur de la cle */
		  name[0] = EOS;
		  sscanf (ch, "<Key>%80s", name);
		  if (name[0] != EOS)
		    {
		       strcat (line, "<Key>");	/* copie de la cle */
		       i = strlen (name);
		       /* Elimine le : a la fin du nom */
		       if ((name[i - 1] == ':') && i != 1)
			 {
			    /* Il faut engendrer un : apres le nom */
			    name[i - 1] = EOS;
			    i = 1;
			 }
		       else
			  i = 0;

		       strcat (line, NameCode (name));	/* copie le nom normalise */
		       if (i == 1)
			  strcat (line, ": ");
		       else
			  strcat (line, " ");
		    }

		  /* convertion vers keysym pour l'automate */
		  key1 = SpecialKey (name);
		  strcat (equiv, name);

		  /* Lecture eventuelle d'une deuxieme composition */
		  strcpy (name, "");
		  fscanf (file, "%80s", name);
		  if (name[0] == ',')
		    {
		       strcat (line, ", ");	/* copie du separateur */
		       strcpy (ch, "");
		       fscanf (file, "%80s", ch);

		       if (!strcmp (ch, "Shift"))
			 {
			    mod2 = THOT_MOD_SHIFT;
			    strcat (line, ch);	/* copie du 1er modifieur */
			    strcat (line, " ");
			    strcat (equiv, " Shift+");
			    /* Lecture enventuelle d'un deuxieme modifieur */
			    strcpy (ch, "");
			    fscanf (file, "%80s", ch);
			 }
		       else
			 {
			    mod2 = THOT_NO_MOD;
			    strcat (equiv, " ");
			 }

		       if (!strcmp (ch, "Ctrl"))
			  mod2 += THOT_MOD_CTRL;
		       else if (!strcmp (ch, "Alt"))
			  mod2 += THOT_MOD_ALT;
		       else if (!strcmp (ch, "Meta"))
			  mod2 += THOT_MOD_META;

		       /* Lecture de la cle */
		       if (mod2 != THOT_NO_MOD && mod2 != THOT_MOD_SHIFT)
			 {
			    strcat (equiv, ch);
			    strcat (equiv, "+");
			    strcat (line, ch);	/* copie 2eme modifieur */
			    strcat (line, " ");
			    strcpy (ch, "");
			    fscanf (file, "%80s", ch);
			    strcat (line, ch);	/* copie de la cle */
			    strcat (line, " ");
			 }
		       /* Extrait la valeur de la cle */
		       strcpy (name, "");
		       sscanf (ch, "<Key>%80s", name);
		       if (name != "")
			 {
			    strcat (line, "<Key>");	/* copie de la cle */
			    i = strlen (name);
			    /* Elimine le : a la fin du nom */
			    if (name[i - 1] == ':' && i != 1)
			      {
				 /* Il faut engendrer un : apres le nom */
				 name[i - 1] = EOS;
				 i = 1;
			      }
			    else
			       i = 0;
			    strcat (line, NameCode (name));	/* copie le nom normalise */
			    if (i == 1)
			       strcat (line, ": ");
			    else
			       strcat (line, " ");
			 }
		       key2 = SpecialKey (name);
		       strcat (equiv, name);

		       /* Lecture de l'action */
		       strcpy (name, "");
		       fscanf (file, "%80s", name);
		    }

		  /* Isole l'intitule de la commande */
		  strncpy (ch, name, 80);
		  adr = strchr (ch, '(');
		  if (adr == NULL)
		     i = max;
		  else
		    {
		       adr[0] = EOS;
		       /* Selection de la bonne commande */
		       for (i = 0; i < max; i++)
			  if (!strcmp (ch, MenuActionList[i].ActionName))
			     break;
		    }

		  /* Est-ce une translation valable pour le texte Motif */
		  if (i <= 8)
		    {
		       /* FnCopy la ligne dans le source de la table de translations */
		       strcat (text, line);
		       if (!strcmp (ch, CST_InsertChar))
			 {
			    strcat (text, "insert-string(");
			    strcat (text, AsciiTranslate (&adr[1]));
			 }
		       else if (!strcmp (ch, CST_DeleteSelection))
			  strcat (text, "delete-selection()");
		       else if (!strcmp (ch, CST_DeletePrevChar))
			  strcat (text, "delete-prev-char()");
		       else if (!strcmp (ch, CST_BackwardChar))
			  strcat (text, "backward-char()");
		       else if (!strcmp (ch, CST_ForwardChar))
			  strcat (text, "forward-char()");
		       else if (!strcmp (ch, CST_PreviousLine))
			  strcat (text, "previous-line()");
		       else if (!strcmp (ch, CST_NextLine))
			  strcat (text, "next-line()");
		       else if (!strcmp (ch, CST_BeginningOfLine))
			  strcat (text, "beginning-of-line()");
		       else if (!strcmp (ch, CST_EndOfLine))
			  strcat (text, "end-of-line()");
		       strcat (text, "\n");
		    }

		  if (i == 0)
		    {
		       /* C'est l'action insert-string */
		       /* FnCopy la ligne dans le source de la table de translations */
		       strcat (text, line);
		       strcat (text, AsciiTranslate (name));
		       strcat (text, "\n");
		       /* C'est un encodage de caractere */
		       adr = AsciiTranslate (&name[len]);
		       MemoKey (mod1, key1, mod2, key2, (unsigned int) adr[0], 0);
		    }
		  else if (i < max)
		    {
		       /* C'est une autre action Thot */
		       MemoKey (mod1, key1, mod2, key2, /*255+i */ 0, i);
		       /* On met a jour l'equivalent clavier */
		       MenuActionList[i].ActionEquiv = TtaGetMemory (strlen (equiv) + 1);
		       strcpy (MenuActionList[i].ActionEquiv, equiv);
		    }
	       }
	     strcpy (ch, "");
	     fscanf (file, "%80s", ch);
	  }
	while (e != 0);

	fclose (file);

	/* Creation de la table de translation */
#       ifndef _WINDOWS
	table = XtParseTranslationTable (text);
#       endif  /* !_WINDOWS */
	TtaFreeMemory (text);
	return table;
     }				/*else */
}				/*InitTranslation */
