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
 *          R. Guetari (W3C/INRIA) - Unicode and Windows version
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
#include "unicodeconsts.h"
#define MAX_EQUIV 25
#define THOT_NO_MOD	0
#define THOT_MOD_CTRL	1
#define THOT_MOD_ALT	2
#define THOT_MOD_SHIFT	4
#define THOT_MOD_S_CTRL	5
#define THOT_MOD_S_ALT	6
#define MAX_AUTOMATA	80
/* automata structure for the keys */
typedef struct _key
{
  int                 K_EntryCode;    /* input key                           */
  int                 K_Command;      /* index in the command list or -1     */
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

#ifdef _GTK
#include "gtk-functions.h"
#else /* !_GTK */
#include "appli_f.h"
#include "input_f.h"
#endif /* !_GTK */


/* Actions table */
#include "applicationapi_f.h"
#include "context_f.h"
#include "editcommands_f.h"
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
static int          SpecialKeys[] = {
  CMD_PreviousLine,
  CMD_PreviousChar,
  CMD_NextChar,
  CMD_NextLine,
  CMD_DeletePrevChar,
  CMD_DeleteSelection,
  CMD_PageUp,
  CMD_PageDown,
  CMD_PageTop,
  CMD_PageEnd
};
static int          SpecialShiftKeys[] = {
  CMD_PreviousSelLine,
  CMD_PreviousSelChar,
  CMD_NextSelChar,
  CMD_NextSelLine,
  CMD_DeletePrevChar,
  CMD_DeleteSelection,
  CMD_PageUp,
  CMD_PageDown,
  CMD_PageTop,
  CMD_PageEnd
};
static int          SpecialCtrlKeys[] = {
  CMD_LineUp,
  CMD_ScrollLeft,
  CMD_ScrollRight,
  CMD_LineDown,
  CMD_DeletePrevChar,
  CMD_DeleteSelection,
  CMD_PageUp,
  CMD_PageDown,
  CMD_PageTop,
  CMD_PageEnd
};

/* the automata */
static KEY         *Automata_normal = NULL;

static KEY         *Automata_ctrl    = NULL;
static KEY         *Automata_alt     = NULL;
static KEY         *Automata_CTRL    = NULL;
static KEY         *Automata_ALT     = NULL;
static KEY         *Automata_current = NULL;

#ifdef _WINDOWS
static BOOL specialKey;
static BOOL escChar = FALSE;
#endif /* _WINDOWS */

/*----------------------------------------------------------------------
   NameCode
   translates the keynames not supported by the interpreter of
   Motif translations.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static STRING       NameCode (STRING name)

#else  /* __STDC__ */
static STRING       NameCode (name)
STRING              name;

#endif /* __STDC__ */

{
   if (ustrlen (name) < 5)
      if (name[0] == TEXT(','))
	 return (TEXT("0x2c"));
      else
	 return (name);
   else if (!ustrcasecmp (name, _ReturnKEY_))
      return (TEXT("0x0d"));
   else if (!ustrcasecmp (name, _BackspaceKEY_))
      return (TEXT("0x08"));
   else if (!ustrcasecmp (name, _SpaceKEY_))
      return (TEXT("0x20"));
   else if (!ustrcasecmp (name, _EscapeKEY_))
      return (TEXT("0x18"));
   else if (!ustrcasecmp (name, _DeleteKEY_))
      return (TEXT("0x7f"));
   else
      return (name);
}


/*----------------------------------------------------------------------
   SpecialKey
   translates the name given by the file thot.keyboard into a key value
   which Thot can use.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static unsigned int SpecialKey (STRING name)
#else  /* __STDC__ */
static unsigned int SpecialKey (name)
STRING              name;

#endif /* __STDC__ */
{
   /* is it the name of a special character? */
   if (!ustrcasecmp (name, _ReturnKEY_))
      return (unsigned int) THOT_KEY_Return;
   else if (!ustrcasecmp (name, _BackspaceKEY_))
      return (unsigned int) THOT_KEY_BackSpace;
   else if (!ustrcasecmp (name, _SpaceKEY_))
      return 32;
   else if (!ustrcasecmp (name, _EscapeKEY_))
      return (unsigned int) THOT_KEY_Escape;
   else if (!ustrcasecmp (name, _DeleteKEY_))
      return (unsigned int) THOT_KEY_Delete;
   else if (!ustrcasecmp (name, TEXT("F1")))
      return (unsigned int) THOT_KEY_F1;
   else if (!ustrcasecmp (name, TEXT("F2")))
      return (unsigned int) THOT_KEY_F2;
   else if (!ustrcasecmp (name, TEXT("F3")))
      return (unsigned int) THOT_KEY_F3;
   else if (!ustrcasecmp (name, TEXT("F4")))
      return (unsigned int) THOT_KEY_F4;
   else if (!ustrcasecmp (name, TEXT("F5")))
      return (unsigned int) THOT_KEY_F5;
   else if (!ustrcasecmp (name, TEXT("F6")))
      return (unsigned int) THOT_KEY_F6;
   else if (!ustrcasecmp (name, TEXT("F7")))
      return (unsigned int) THOT_KEY_F7;
   else if (!ustrcasecmp (name, TEXT("F8")))
      return (unsigned int) THOT_KEY_F8;
   else if (!ustrcasecmp (name, TEXT("F9")))
      return (unsigned int) THOT_KEY_F9;
   else if (!ustrcasecmp (name, TEXT("F10")))
      return (unsigned int) THOT_KEY_F10;
   else if (!ustrcasecmp (name, TEXT("F11")) || !ustrcasecmp (name, TEXT("l1")))
      return (unsigned int) THOT_KEY_F11;
   else if (!ustrcasecmp (name, TEXT("F12")) || !ustrcasecmp (name, TEXT("l2")))
      return (unsigned int) THOT_KEY_F12;
   else if (!ustrcasecmp (name, TEXT("F13")) || !ustrcasecmp (name, TEXT("l3")))
      return (unsigned int) THOT_KEY_F13;
   else if (!ustrcasecmp (name, TEXT("F14")) || !ustrcasecmp (name, TEXT("l4")))
      return (unsigned int) THOT_KEY_F14;
   else if (!ustrcasecmp (name, TEXT("F15")) || !ustrcasecmp (name, TEXT("l5")))
      return (unsigned int) THOT_KEY_F15;
   else if (!ustrcasecmp (name, TEXT("F16")) || !ustrcasecmp (name, TEXT("l6")))
      return (unsigned int) THOT_KEY_F16;
   else if (!ustrcasecmp (name, TEXT("F17")) || !ustrcasecmp (name, TEXT("l7")))
      return (unsigned int) THOT_KEY_F17;
   else if (!ustrcasecmp (name, TEXT("F18")) || !ustrcasecmp (name, TEXT("l8")))
      return (unsigned int) THOT_KEY_F18;
   else if (!ustrcasecmp (name, TEXT("F19")) || !ustrcasecmp (name, TEXT("l9")))
      return (unsigned int) THOT_KEY_F19;
   else if (!ustrcasecmp (name, TEXT("F20")) || !ustrcasecmp (name, TEXT("l10")))
      return (unsigned int) THOT_KEY_F20;
#ifdef THOT_KEY_R1
   else if (!ustrcasecmp (name, TEXT("F21")) || !ustrcasecmp (name, "r1"))
      return (unsigned int) THOT_KEY_R1;
   else if (!ustrcasecmp (name, TEXT("F22")) || !ustrcasecmp (name, "r2"))
      return (unsigned int) THOT_KEY_R2;
   else if (!ustrcasecmp (name, TEXT("F23")) || !ustrcasecmp (name, "r3"))
      return (unsigned int) THOT_KEY_R3;
   else if (!ustrcasecmp (name, TEXT("F24")) || !ustrcasecmp (name, "r4"))
      return (unsigned int) THOT_KEY_R4;
   else if (!ustrcasecmp (name, TEXT("F25")) || !ustrcasecmp (name, "r5"))
      return (unsigned int) THOT_KEY_R5;
   else if (!ustrcasecmp (name, TEXT("F26")) || !ustrcasecmp (name, "r6"))
      return (unsigned int) THOT_KEY_R6;
   else if (!ustrcasecmp (name, TEXT("F27")) || !ustrcasecmp (name, "r7"))
      return (unsigned int) THOT_KEY_R7;
   else if (!ustrcasecmp (name, TEXT("F28")) || !ustrcasecmp (name, "r8"))
      return (unsigned int) THOT_KEY_R8;
   else if (!ustrcasecmp (name, TEXT("F29")) || !ustrcasecmp (name, "r9"))
      return (unsigned int) THOT_KEY_R9;
   else if (!ustrcasecmp (name, TEXT("F30")) || !ustrcasecmp (name, "r10"))
      return (unsigned int) THOT_KEY_R10;
   else if (!ustrcasecmp (name, TEXT("F31")) || !ustrcasecmp (name, "r11"))
      return (unsigned int) THOT_KEY_R11;
   else if (!ustrcasecmp (name, TEXT("F32")) || !ustrcasecmp (name, "r12"))
      return (unsigned int) THOT_KEY_R12;
   else if (!ustrcasecmp (name, TEXT("F33")) || !ustrcasecmp (name, "r13"))
      return (unsigned int) THOT_KEY_R13;
   else if (!ustrcasecmp (name, TEXT("F34")) || !ustrcasecmp (name, "r14"))
      return (unsigned int) THOT_KEY_R14;
   else if (!ustrcasecmp (name, TEXT("F35")) || !ustrcasecmp (name, "r15"))
      return (unsigned int) THOT_KEY_R15;
#endif /* THOT_KEY_R1 */
   else if (!ustrcasecmp (name, _UpKEY_))
      return (unsigned int) THOT_KEY_Up;
   else if (!ustrcasecmp (name, _DownKEY_))
      return (unsigned int) THOT_KEY_Down;
   else if (!ustrcasecmp (name, _LeftKEY_))
      return (unsigned int) THOT_KEY_Left;
   else if (!ustrcasecmp (name, _RightKEY_))
      return (unsigned int) THOT_KEY_Right;
   else if (!ustrcasecmp (name, _HomeKEY_))
      return (unsigned int) THOT_KEY_Home;
   else if (!ustrcasecmp (name, _EndKEY_))
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
   ThotBool            exists;
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
           else if (key1 >= 'A' && key1 <= 'Z')
                key1 = key1 - 'A' + 1;
#          endif /* _WINDOWS */
	       break;
	    case THOT_MOD_ALT:
	       addFirst = &Automata_alt;
	       break;
	    case THOT_MOD_S_CTRL:
	       addFirst = &Automata_CTRL;
#          ifdef _WINDOWS
           if (key1 >= 'a' && key1 <= 'z')
              key1 = key1 - 'a' + 1;
           else if (key1 >= 'A' && key1 <= 'Z')
                key1 = key1 - 'A' + 1;
#          endif /* _WINDOWS */
	       break;
	    case THOT_MOD_S_ALT:
	       addFirst = &Automata_ALT;
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
   int  keyboard_mask = 0;   
   int  status;
   CHAR_T string[2];
   int  len = 0;

   if (frame < 0)
      return;

   /* Is the Shift key pressed ?? */
   status = GetKeyState (VK_SHIFT);
   if (HIBYTE (status)) 
      /* Yes it is */
      keyboard_mask |= THOT_MOD_SHIFT;

   /* Is the Control key pressed ?? */
   status = GetKeyState (VK_CONTROL);
   if (HIBYTE (status)) {
      /* Yes it is */
      keyboard_mask |= THOT_MOD_CTRL;
      escChar = TRUE;
   } 

   /* Alt key is a particular key for Windows. It generates a WM_SYSKEYDOWN and */
   /* usulally we have not to trap this event and let Windows do.  In our case, */
   /* we do not use the standard accelerator tables as in common Windows appli. */
   /* Is the Alt key pressed ?? */
   status = GetKeyState (VK_MENU);
   if (HIBYTE (status)) {
      /* Yes it is */
      keyboard_mask |= THOT_MOD_ALT;
      escChar = TRUE;
   }

   if ((msg == WM_KEYDOWN) && (wParam == VK_RETURN) && !(keyboard_mask & THOT_MOD_ALT))
	   return;

   /* If the message is WM_CHAR then this is a character to insert and  not a */
   /* particular key as a short cut for example                               */
   if (msg == WM_CHAR)
      specialKey = FALSE;
   else
      /* if the message is WM_KEYDOWN or WM_SYSKEYDOWN then we have to suppose */
	  /* that we have to deal with something else than a character to  insert: */
	  /* a short cut for example                                               */
      specialKey = TRUE;


   if (msg == WM_SYSCHAR)
      len = 0;
   else
        len = 1;

   if (wParam == VK_MENU)
      return;

   string[0] = (CHAR_T) wParam;
   if (msg == WM_SYSCHAR)
      if (wParam >= 'a' && wParam <= 'z')
         wParam -= 'a' - 'A';

   ThotInput (frame, &string[0], len, keyboard_mask, wParam);
}
#endif /* _WINDOWS */

#ifdef _GTK
#ifdef __STDC__
void                XCharTranslation (GdkEventKey * event, gpointer * data)
#else  /* __STDC__ */
void                XCharTranslation (event, data)
GdkEventKey             *event;
gpointer *data;
#endif /* __STDC__ */
{
   int                 status;
   int                 PicMask;
   int                 frame;
   unsigned int        state, save;
   UCHAR_T       string[2];
   ThotComposeStatus      ComS;
   KeySym              KS;

   frame = (int) data;
   if (frame > MAX_FRAME)
      frame = 0;

   status = 0;
   /* control, alt and mouse status bits of the state are ignored */

   state = event->state & (GDK_SHIFT_MASK | GDK_LOCK_MASK | GDK_MOD3_MASK );
   if (event->state == state) {
       /* status = XLookupString ((ThotKeyEvent *) event, string, 2, &KS, &ComS); */
     strncpy(string, event->string, 2);
     KS = event->keyval;
     /*ComS = NULL ; */

  } else
     {
       save = event->state;
       event->state = state;
       state = save;
       /* status = XLookupString ((ThotKeyEvent *) event, string, 2, &KS, &ComS);*/
       strncpy(string, event->string, 2);
       KS = event->keyval;
       /* ComS = NULL ;*/

     }

   PicMask = 0;
   if (state & GDK_SHIFT_MASK)
      PicMask |= THOT_MOD_SHIFT;
   if (state & GDK_LOCK_MASK)
      PicMask |= THOT_MOD_SHIFT;
   if (state & GDK_CONTROL_MASK)
      PicMask |= THOT_MOD_CTRL;
   if (state & GDK_MOD1_MASK || state & GDK_MOD4_MASK)
      PicMask |= THOT_MOD_ALT;

   ThotInput (frame, &string[0], event->length, PicMask, KS);
}


#else /* _GTK */
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
   int                 status;
   int                 PicMask;
   int                 frame;
   unsigned int        state, save;
   UCHAR_T       string[2];
   ThotComposeStatus      ComS;
   KeySym              KS;

   frame = GetWindowFrame (event->xany.window);
   if (frame > MAX_FRAME)
      frame = 0;

   status = 0;
   /* control, alt and mouse status bits of the state are ignored */
   state = event->xkey.state & (ShiftMask | LockMask | Mod3Mask | ButtonMotionMask);
   if (event->xkey.state == state)
     status = XLookupString ((ThotKeyEvent *) event, string, 2, &KS, &ComS);
   else
     {
       save = event->xkey.state;
       event->xkey.state = state;
       state = save;
       status = XLookupString ((ThotKeyEvent *) event, string, 2, &KS, &ComS);
     }

   PicMask = 0;
   if (state & ShiftMask)
      PicMask |= THOT_MOD_SHIFT;
   if (state & LockMask)
      PicMask |= THOT_MOD_SHIFT;
   if (state & ControlMask)
      PicMask |= THOT_MOD_CTRL;
   if (state & Mod1Mask || state & Mod4Mask)
      PicMask |= THOT_MOD_ALT;

   ThotInput (frame, &string[0], status, PicMask, KS);
}
#endif /* !_WINDOWS */
#endif /* _GTK */


/*----------------------------------------------------------------------
   ThotInput
   handles the character encoding.                     
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ThotInput (int frame, USTRING string, unsigned int nb,
			       int PicMask, int key)
#else  /* __STDC__ */
void                ThotInput (frame, string, nb, PicMask, key)
int                 frame;
USTRING             string;
unsigned int        nb;
int                 PicMask;
int                 key;

#endif /* __STDC__ */
{
  KEY                *ptr;
  Document            document;
  View                view;
  int                 value;
  int                 modtype;
  int                 command;
  ThotBool            found;
#  ifdef _WINDOWS
  ThotBool            endOfSearch = FALSE;
#  endif /* _WINDOWS */
  
  if (frame > MAX_FRAME)
    frame = 0;
  
#  ifdef _WINDOWS
  if (key == 0xD && nb == 1)
    specialKey = TRUE;
  else if ((key >= 0x1 && key <= 0x1A) && nb == 1)
    specialKey = TRUE;
  else if (escChar) {
    specialKey = TRUE;
    escChar = FALSE;
  }
#  endif /* _WINDOWS */
  
  value = string[0];
  found = FALSE;
  if (nb == 2) {
    /* It's a Thot action call */
    command = (int) string[1];
    found = TRUE;
  } else {
    command = 0;	   
    /* Set the right indicator */
    if (PicMask & THOT_MOD_CTRL)
      if (PicMask & THOT_MOD_SHIFT)
	modtype = THOT_MOD_S_CTRL;
      else
	modtype = THOT_MOD_CTRL;
    else if (PicMask & THOT_MOD_ALT)
      if (PicMask & THOT_MOD_SHIFT)
	modtype = THOT_MOD_S_ALT;
      else
	modtype = THOT_MOD_ALT;
    else if (PicMask & THOT_MOD_SHIFT)
      modtype = THOT_MOD_SHIFT;
    else
      modtype = THOT_NO_MOD;
    
    /* Is it a second level of the current automata? */
    if (Automata_current != NULL) {
      /* search a second level entry */
      ptr = Automata_current;
      Automata_current = NULL;
      while (!found && ptr != NULL) {
	if (ptr != NULL) {
	  if (ptr->K_EntryCode == key && modtype == ptr->K_Modifier)
	    found = TRUE;
	  else
	    ptr = ptr->K_Other;
	}
      }
      
      if (found) {
	value = (UCHAR_T) ptr->K_Value;
	command = ptr->K_Command;
      }
    } else {
      /* Search a first level entry? */
      if (modtype == THOT_MOD_S_CTRL)
	ptr = Automata_CTRL;
      else if (modtype == THOT_MOD_CTRL)
	ptr = Automata_ctrl;
      else if (modtype == THOT_MOD_S_ALT)
	ptr = Automata_ALT;
      else if (modtype == THOT_MOD_ALT)
	ptr = Automata_alt;
      else
	ptr = Automata_normal;
      
#               ifdef _WINDOWS
      endOfSearch = FALSE;
      while (!endOfSearch && ptr != NULL)
#               else  /* !_WINDOWS */
	while (!found && ptr != NULL)
#               endif /* _WINDOWS */
	  {
	    if (ptr != NULL) {
	      if (ptr->K_EntryCode == key) {
#ifdef _WINDOWS
		endOfSearch = TRUE;
		if (specialKey) {
#endif /* _WINDOWS */
		  /* On entre dans un automate */
		  found = TRUE;
		  Automata_current = ptr->K_Next;
		  if (Automata_current == NULL) {
		    /* il s'agit d'une valeur definie a premier niveau */
		    value = (UCHAR_T) ptr->K_Value;
		    command = ptr->K_Command;
		  }
#ifdef _WINDOWS
		}
#endif /* _WINDOWS */
	      } else
		ptr = ptr->K_Other;
	    } 
	  } 
    } 
  } 
  
#  ifdef _WINDOWS
  if (specialKey && !found)
#  else /* !_WINDOWS */
    if (!found)
#  endif /* _WINDOWS */
      /* Mangement of special keys */
      switch (key) {
      case THOT_KEY_Up:
#            ifdef THOT_KEY_R8
      case THOT_KEY_R8:
#            endif
	if (modtype == THOT_MOD_SHIFT)
	  command = SpecialShiftKeys[MY_KEY_Up];
	else if (modtype == THOT_MOD_CTRL)
	  command = SpecialCtrlKeys[MY_KEY_Up];
	else
	  command = SpecialKeys[MY_KEY_Up];
	Automata_current = NULL;
	break;
	
      case THOT_KEY_Left:
#            ifdef THOT_KEY_R10
      case THOT_KEY_R10:
#            endif
	if (modtype == THOT_MOD_SHIFT)
	  command = SpecialShiftKeys[MY_KEY_Left];
	else if (modtype == THOT_MOD_CTRL)
	  command = SpecialCtrlKeys[MY_KEY_Left];
	else
	  command = SpecialKeys[MY_KEY_Left];
	Automata_current = NULL;
	break;
	
      case THOT_KEY_Right:
#            ifdef THOT_KEY_R12
      case THOT_KEY_R12:
#            endif
	if (modtype == THOT_MOD_SHIFT)
	  command = SpecialShiftKeys[MY_KEY_Right];
	else if (modtype == THOT_MOD_CTRL)
	  command = SpecialCtrlKeys[MY_KEY_Right];
	else
	  command = SpecialKeys[MY_KEY_Right];
	Automata_current = NULL;
	break;
	
      case THOT_KEY_Down:
#            ifdef THOT_KEY_R14
      case THOT_KEY_R14:
#            endif
	if (modtype == THOT_MOD_SHIFT)
	  command = SpecialShiftKeys[MY_KEY_Down];
	else if (modtype == THOT_MOD_CTRL)
	  command = SpecialCtrlKeys[MY_KEY_Down];
	else
	  command = SpecialKeys[MY_KEY_Down];
	Automata_current = NULL;
	break;
	
      case THOT_KEY_Prior:
#            ifdef THOT_KEY_R9
      case THOT_KEY_R9:
#            endif
	if (modtype == THOT_MOD_SHIFT)
	  command = SpecialShiftKeys[MY_KEY_Prior];
	else if (modtype == THOT_MOD_CTRL)
	  command = SpecialCtrlKeys[MY_KEY_Prior];
	else
	  command = SpecialKeys[MY_KEY_Prior];
	Automata_current = NULL;
	break;
	
      case THOT_KEY_Next:
#            ifdef THOT_KEY_R15
      case THOT_KEY_R15:
#            endif
	if (modtype == THOT_MOD_SHIFT)
	  command = SpecialShiftKeys[MY_KEY_Next];
	else if (modtype == THOT_MOD_CTRL)
	  command = SpecialCtrlKeys[MY_KEY_Next];
	else
	  command = SpecialKeys[MY_KEY_Next];
	Automata_current = NULL;
	break;
	
      case THOT_KEY_Home:
#            ifdef THOT_KEY_R7
      case THOT_KEY_R7:
#            endif
	if (modtype == THOT_MOD_SHIFT)
	  command = SpecialShiftKeys[MY_KEY_Home];
	else if (modtype == THOT_MOD_CTRL)
	  command = SpecialCtrlKeys[MY_KEY_Home];
	else
	  command = SpecialKeys[MY_KEY_Home];
	Automata_current = NULL;
	break;
	
      case THOT_KEY_End:
#            ifdef THOT_KEY_R13
      case THOT_KEY_R13:
#            endif
	if (modtype == THOT_MOD_SHIFT)
	  command = SpecialShiftKeys[MY_KEY_End];
	else if (modtype == THOT_MOD_CTRL)
	  command = SpecialCtrlKeys[MY_KEY_End];
	else
	  command = SpecialKeys[MY_KEY_End];
	Automata_current = NULL;
	break;
	
      case THOT_KEY_BackSpace:
	if (modtype == THOT_MOD_SHIFT)
	  command = SpecialShiftKeys[MY_KEY_BackSpace];
	else if (modtype == THOT_MOD_CTRL)
	  command = SpecialCtrlKeys[MY_KEY_BackSpace];
	else
	  command = SpecialKeys[MY_KEY_BackSpace];
	Automata_current = NULL;
	break;
	
      case THOT_KEY_Delete:
	if (modtype == THOT_MOD_SHIFT)
	  command = SpecialShiftKeys[MY_KEY_Delete];
	else if (modtype == THOT_MOD_CTRL)
	  command = SpecialCtrlKeys[MY_KEY_Delete];
	else
	  command = SpecialKeys[MY_KEY_Delete];
	Automata_current = NULL;
	break;
	
      default: break;
      } 
  
  if (Automata_current == NULL) {
    /* Appel d'une action Thot */
    FrameToView (frame, &document, &view);
    if (command > 0) {
      /* Termine l'insertion eventuellement en cours */
      if (command != CMD_DeletePrevChar)
	/* Ce n'est pas un delete, il faut terminer l'insertion courante */
	CloseInsertion ();
      
      /* Call action if it's active */
      if (MenuActionList[command].ActionActive[frame])
	{
	  /* l'action est active pour la fenetre courante */
	  /* prepare les parametres */
	  if (MenuActionList[command].User_Action != NULL) {
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
    else {
      if (value == 8 || value == 127) {
	/* Par defaut BackSpace detruit le caractere precedent */
	/* sans se soucier de la langue courante */
	if (MenuActionList[CMD_DeletePrevChar].User_Action != NULL) {
	  if (((*MenuActionList[CMD_DeletePrevChar].User_Action) (MenuActionList[CMD_DeletePrevChar].User_Arg, document, view)) &&
	      (MenuActionList[CMD_DeletePrevChar].Call_Action != NULL))
	    (*MenuActionList[CMD_DeletePrevChar].Call_Action) (document, view);
	} else if (MenuActionList[CMD_DeletePrevChar].Call_Action != NULL)
	  (*MenuActionList[CMD_DeletePrevChar].Call_Action) (document, view);
	return;
      }
      
      /*** Sequence de traitement des espaces ***/
      if (value == BREAK_LINE || (InputSpace && value == SHOWN_BREAK_LINE)) {
	if (MenuActionList[0].Call_Action)
	  (*MenuActionList[0].Call_Action) (document, view, BREAK_LINE);
      } else if (value == THIN_SPACE || (InputSpace && value == SHOWN_THIN_SPACE)) {
	if (MenuActionList[0].Call_Action)
	  (*MenuActionList[0].Call_Action) (document, view, THIN_SPACE);
      } else if (value == HALF_EM || (InputSpace && value == SHOWN_HALF_EM)) {
	if (MenuActionList[0].Call_Action)
	  (*MenuActionList[0].Call_Action) (document, view, HALF_EM);
      } else if (value == UNBREAKABLE_SPACE || (InputSpace && value == SHOWN_UNBREAKABLE_SPACE)) {
	if (MenuActionList[0].Call_Action)
	  (*MenuActionList[0].Call_Action) (document, view, UNBREAKABLE_SPACE);
      } else if ((InputSpace && value == SHOWN_SPACE)) {
	if (MenuActionList[0].Call_Action)
	  (*MenuActionList[0].Call_Action) (document, view, _SPACE_);
      } else if ((value >= 32 && value < 128) || (value >= 144 && value < 256)) {
#                    ifdef _WINDOWS
	if (!specialKey)
#                    endif /* _WINDOWS */
	  /* on insere un caractere valide quelque soit la langue */
	  if (MenuActionList[0].Call_Action)
	    (*MenuActionList[0].Call_Action) (document, view, value);
      }
    } 
  }  
} 



/*----------------------------------------------------------------------
   FreeTranslations remove all translation structures.
  ----------------------------------------------------------------------*/
void FreeTranslations ()
{
   KEY                *ptr, *subkey;

   while (Automata_current != NULL)
     {
       ptr = Automata_current;
       while (ptr->K_Next != NULL)
	 {
	   subkey = ptr->K_Next;
	   ptr->K_Next = subkey->K_Other;
	   TtaFreeMemory (subkey);
	 }
       Automata_current = ptr->K_Other;
       TtaFreeMemory (ptr);
     }

   while (Automata_ctrl != NULL)
     {
       ptr = Automata_ctrl;
       while (ptr->K_Next != NULL)
	 {
	   subkey = ptr->K_Next;
	   ptr->K_Next = subkey->K_Other;
	   TtaFreeMemory (subkey);
	 }
       Automata_ctrl = ptr->K_Other;
       TtaFreeMemory (ptr);
     }

   while (Automata_alt != NULL)
     {
       ptr = Automata_alt;
       while (ptr->K_Next != NULL)
	 {
	   subkey = ptr->K_Next;
	   ptr->K_Next = subkey->K_Other;
	   TtaFreeMemory (subkey);
	 }
       Automata_alt = ptr->K_Other;
       TtaFreeMemory (ptr);
     }

   while (Automata_CTRL != NULL)
     {
       ptr = Automata_CTRL;
       while (ptr->K_Next != NULL)
	 {
	   subkey = ptr->K_Next;
	   ptr->K_Next = subkey->K_Other;
	   TtaFreeMemory (subkey);
	 }
       Automata_CTRL = ptr->K_Other;
       TtaFreeMemory (ptr);
     }

   while (Automata_ALT != NULL)
     {
       ptr = Automata_ALT;
       while (ptr->K_Next != NULL)
	 {
	   subkey = ptr->K_Next;
	   ptr->K_Next = subkey->K_Other;
	   TtaFreeMemory (subkey);
	 }
       Automata_ALT = ptr->K_Other;
       TtaFreeMemory (ptr);
     }
}


/*----------------------------------------------------------------------
   InitTranslations
   intializes the keybord encoding.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotTranslations      InitTranslations (STRING appliname)
#else  /* __STDC__ */
ThotTranslations      InitTranslations (appliname)
STRING              appliname;

#endif /* __STDC__ */
{
   char*               text;	   /* fichier de translation transmis a motif */
   char                line[200];  /* ligne en construction pour motif */
   char                home[200], name[80]; 
   char                ch[80]; 
   char*               adr;
   char                equiv[MAX_EQUIV]; /* equivalents caracteres pour motif */
   unsigned int        key1, key2; /* 1ere & 2eme cles sous forme de keysym X */
   int                 e, i;
   int                 mod1, mod2; /* 1er/ 2eme modifieurs : voir THOT_MOD_xx */
   int                 len, max;
   FILE               *file;
   ThotTranslations    table = 0;

   text = TtaGetEnvString ("APP_HOME");
   ustrcpy (name, appliname);
#  ifdef _WINDOWS
   ustrcat (name, KB_EXT);
#  else  /* _WINDOWS */
   ustrcat (name, Keyboard_EXT);
#  endif /* _WINDOWS */

   ustrcpy (home, text);
   ustrcat (home, DIR_STR);
   ustrcat (home, name);
   if (!SearchFile (home, 0, line))
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
	text = TtaAllocString (len);
	fseek (file, 0L, 0);	/* debut du fichier */

	/* Initialise la lecture du fichier */
	e = 1;
	max = MaxMenuAction;
	len = ustrlen ("TtcInsertChar") + 2;

	/* FnCopy la premiere ligne du fichier (#override, ou #...) */
	ustrcpy (text, TEXT("#override\n"));
	ch[0] = 0;
	fscanf (file, "%80s", ch);
	do {
       /* Initialisations */
       mod1 = mod2 = THOT_NO_MOD;
       key1 = key2 = 0;
       ustrcpy (line, TEXT("!"));	/* initialisation de la ligne */

       /* Est-ce la fin de fichier ? */
       if (ustrlen (ch) == 0 || EndOfString (ch, TEXT("^")))
          e = 0;
       else if (ch[0] != TEXT('#')) {
            /* it is not a comment */
            /* -------> Lecture des autres champs */
            if (!ustrcasecmp (ch, TEXT("shift"))) {
               mod1 = THOT_MOD_SHIFT;
               /* copie 1er modifieur */
               ustrcpy (equiv, TEXT("Shift"));
               ustrcat (equiv, TEXT(" "));
               ustrcat (line, TEXT("Shift"));
               ustrcat (line, TEXT(" "));
               /* Lecture enventuelle d'un deuxieme modifieur */
			   ch[0] = 0;
               fscanf (file, "%80s", ch);
			} else {
                   mod1 = THOT_NO_MOD;
                   equiv[0] = EOS;
			}

            if (!ustrcasecmp (ch, TEXT("ctrl"))) {
               mod1 += THOT_MOD_CTRL;
               /* copie 2eme modifieur */
               ustrcat (equiv, TEXT("Ctrl"));
               ustrcat (equiv, TEXT(" "));
               ustrcat (line, TEXT("Ctrl"));
               ustrcat (line, TEXT(" "));
               /* Lecture de la cle */
			   ch[0] = 0;
               fscanf (file, "%80s", ch);
			} else if (!ustrcasecmp (ch, TEXT("alt")) || !ustrcasecmp (ch, TEXT("meta"))) {
                   mod1 += THOT_MOD_ALT;
                   /* copie 2eme modifieur */
                   ustrcat (equiv, TEXT("Alt"));
                   ustrcat (equiv, TEXT(" "));
                   ustrcat (line, TEXT("Alt"));
                   ustrcat (line, TEXT(" "));
                   /* Lecture de la cle */
				   ch[0] = 0;
                   fscanf (file, "%80s", ch);
			} 

            /* Extrait la valeur de la cle */
            sscanf (ch, "<Key>%80s", name);
            if (name[0] != EOS) {
               /* copie de la cle */
               ustrcat (line, TEXT("<Key>"));
               i = ustrlen (name);
               /* Elimine le : a la fin du nom */
               if ((name[i - 1] == TEXT(':')) && i != 1) {
                  /* Il faut engendrer un : apres le nom */
                  name[i - 1] = EOS;
                  i = 1;
			   } else
                     i = 0;
               /* copie le nom normalise */
               ustrcat (line, NameCode (name));
               if (i == 1)
                  ustrcat (line, TEXT(": "));
               else
                   ustrcat (line, TEXT(" "));
			}

            /* convertion vers keysym pour l'automate */
            key1 = SpecialKey (name);
            ustrcat (equiv, name);

            /* Lecture eventuelle d'une deuxieme composition */
            fscanf (file, "%80s", name);
            if (name[0] == ',') {
               /* copie du separateur */
               ustrcat (line, TEXT(", "));
			   ch[0] = 0;
               fscanf (file, "%80s", ch);
		      
               if (!ustrcasecmp (ch, TEXT("shift"))) {
                  mod2 = THOT_MOD_SHIFT;
                  /* copie du 2eme modifieur */
                  ustrcat (equiv, TEXT("Shift"));
                  ustrcat (equiv, TEXT(" "));
                  ustrcat (line, TEXT("Shift"));
                  ustrcat (line, TEXT(" "));
                  /* Lecture enventuelle d'un deuxieme modifieur */
				  ch[0] = 0;
                  fscanf (file, "%80s", ch);
			   } else {
                      mod2 = THOT_NO_MOD;
                      ustrcat (equiv, TEXT(" "));
			   }

               if (!ustrcasecmp (ch, TEXT("ctrl"))) {
                  mod2 += THOT_MOD_CTRL;
                  /* copie 2eme modifieur */
                  ustrcat (equiv, TEXT("Ctrl"));
                  ustrcat (equiv, TEXT(" "));
                  ustrcat (line, TEXT("Ctrl"));
                  ustrcat (line, TEXT(" "));
                  /* copie de la cle */
				  ch[0] = 0;
                  fscanf (file, "%80s", ch);
                  ustrcat (line, ch);
                  ustrcat (line, TEXT(" "));
			   } else if (!ustrcasecmp (ch, TEXT("alt")) || !ustrcasecmp (ch, TEXT("meta"))) {
                      mod2 += THOT_MOD_ALT;
                      /* copie 2eme modifieur */
                      ustrcat (equiv, TEXT("Alt"));
                      ustrcat (equiv, TEXT(" "));
                      ustrcat (line, TEXT("Alt"));
                      ustrcat (line, TEXT(" "));
                      /* copie de la cle */
					  ch[0] = 0;
                      fscanf (file, "%80s", ch);
                      ustrcat (line, ch);
                      ustrcat (line, TEXT(" "));
			   }

               /* Extrait la valeur de la cle */
               sscanf (ch, "<Key>%80s", name);
               if (name [0] != EOS) {
                  ustrcat (line, TEXT("<Key>"));	/* copie de la cle */
                  i = ustrlen (name);
                  /* Elimine le : a la fin du nom */
                  if (name[i - 1] == TEXT(':') && i != 1) {
                     /* Il faut engendrer un : apres le nom */
                     name[i - 1] = EOS;
                     i = 1;
                  } else
                        i = 0;
                  ustrcat (line, NameCode (name));	/* copie le nom normalise */
                  if (i == 1)
                     ustrcat (line, TEXT(": "));
                  else
                      ustrcat (line, TEXT(" "));
	       } 
               key2 = SpecialKey (name);
               ustrcat (equiv, name);

               /* Lecture de l'action */
               fscanf (file, "%80s", name);
	    }

            /* Isole l'intitule de la commande */
            strncpy (ch, name, 80);
            adr = ustrchr (ch, TEXT('('));
            if (adr == NULL)
               adr = ustrchr (ch, SPACE);
            if (adr == NULL)
               i = max;
            else {
                 adr[0] = EOS;
                 /* Selection de la bonne commande */
                 for (i = 0; i < max; i++)
                 if (!ustrcmp (ch, MenuActionList[i].ActionName))
                    break;
			} 

            /* Est-ce une translation valable pour le texte Motif */
            if (i <= 8) {
               /* FnCopy la ligne dans le source de la table de translations */
               ustrcat (text, line);
               if (!ustrcmp (ch, "TtcInsertChar"))
		 {
                  ustrcat (text, TEXT("insert-string("));
                  ustrcat (text, AsciiTranslate (&adr[1]));
		 }
	       else if (!ustrcmp (ch, "TtcDeleteSelection"))
		 ustrcat (text, TEXT("delete-selection()"));
               else if (!ustrcmp (ch, "TtcDeletePreviousChar"))
                    ustrcat (text, TEXT("delete-prev-char()"));
               else if (!ustrcmp (ch, "TtcPreviousChar"))
                    ustrcat (text, TEXT("backward-char()"));
               else if (!ustrcmp (ch, "TtcNextChar"))
                    ustrcat (text, TEXT("forward-char()"));
               else if (!ustrcmp (ch, "TtcPreviousLine"))
                    ustrcat (text, TEXT("previous-line()"));
               else if (!ustrcmp (ch, "TtcNextLine"))
                    ustrcat (text, TEXT("next-line()"));
               else if (!ustrcmp (ch, "TtcStartOfLine"))
                    ustrcat (text, TEXT("beginning-of-line()"));
               else if (!ustrcmp (ch, "TtcEndOfLine"))
                    ustrcat (text, TEXT("end-of-line()"));
               ustrcat (text, TEXT("\n"));
	    }

            if (i == 0)
	      {
		/* C'est l'action insert-string */
		/* FnCopy la ligne dans le source de la table de translations */
		ustrcat (text, line);
		ustrcat (text, AsciiTranslate (name));
		ustrcat (text, TEXT("\n"));
		/* C'est un encodage de caractere */
		adr = AsciiTranslate (&name[len]);
		MemoKey (mod1, key1, mod2, key2, (unsigned int) adr[0], 0);
	      }
	    else if (i < max)
	      {
		/* C'est une autre action Thot */
		MemoKey (mod1, key1, mod2, key2, /*255+i */ 0, i);
		/* On met a jour l'equivalent clavier */
		MenuActionList[i].ActionEquiv = TtaStrdup (equiv);
	      }  
       } else {
	 /* skip this line */
	 do
	   i = fgetc (file);
	 while (i != NEW_LINE);
       }
       ch[0] = 0;
       fscanf (file, "%80s", ch);
	} while (e != 0);

    fclose (file);

    /* Creation de la table de translation */
#   ifndef _WINDOWS
	table = XtParseTranslationTable (text);
#   endif  /* !_WINDOWS */
    TtaFreeMemory (text);
    return table;
  }				/*else */
}				/*InitTranslation */
