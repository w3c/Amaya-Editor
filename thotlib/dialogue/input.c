/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2001.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

 /*
 * 
 * Module for keyboard input handling.
 * Authors: I. Vatton (INRIA)
 *          D. Veillard (INRIA) - Removed X remapping of keys,
 *                                lead to crash in some configurations
 *          R. Guetari (W3C/INRIA) - Previous Windows version
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
  ThotBool            K_Special;      /* TRUE if it's a special key          */
  struct _key        *K_Other;	      /* next entry at the same level        */
  union
  {
    struct _key    *_K_Next_;	      /* 1st complementary touch (1st level) */
    int             _K_Modifier_;     /* modifier value (2nd level)          */
    void           *_K_Param_;     /* the parameter of the access key     */
  } u;
  struct _key        *K_Next;	      /* 1st complementary touch (1st level) */
  int                 K_Modifier;     /* modifier value (2nd level)          */
}
KEY;
#define K_Next u._K_Next_
#define K_Modifier u._K_Modifier_
#define K_Param u._K_Param_

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "boxes_tv.h"
#include "frame_tv.h"
#include "edit_tv.h"
#include "appdialogue_tv.h"
#include "select_tv.h"

#ifdef _GTK
#include "gtk-functions.h"
#else /* _GTK */
#include "appli_f.h"
#include "input_f.h"
#endif /* _GTK */
/* Actions table */
#include "applicationapi_f.h"
#include "callback_f.h"
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
#define MY_KEY_Return    10
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
  CMD_PageEnd,
  CMD_CreateElement
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
  CMD_PageEnd,
  CMD_CreateElement
};
static int          SpecialCtrlKeys[] = {
  CMD_LineUp,
  CMD_PreviousWord,
  CMD_NextWord,
  CMD_LineDown,
  CMD_DeletePrevChar,
  CMD_DeleteSelection,
  CMD_PageUp,
  CMD_PageDown,
  CMD_PageTop,
  CMD_PageEnd,
  CMD_LineBreak
};
static int          SpecialShiftCtrlKeys[] = {
  CMD_LineUp,
  CMD_PreviousSelWord,
  CMD_NextSelWord,
  CMD_LineDown,
  CMD_DeletePrevChar,
  CMD_DeleteSelection,
  CMD_PageUp,
  CMD_PageDown,
  CMD_PageTop,
  CMD_PageEnd,
  CMD_LineBreak
};

/* the automata */
static KEY         *Automata_normal = NULL;
static KEY         *Automata_ctrl    = NULL;
static KEY         *Automata_alt     = NULL;
static KEY         *Automata_SHIFT   = NULL;
static KEY         *Automata_CTRL    = NULL;
static KEY         *Automata_ALT     = NULL;
static KEY         *Automata_current = NULL;
/* Access key table for loaded documents */
static Proc         AccessKeyFunction = NULL;
static KEY         *DocAccessKey[MAX_DOCUMENTS];
#ifdef _WINDOWS
static ThotBool    Special;
#endif /* _WINDOWS */

/*----------------------------------------------------------------------
   NameCode
   translates the keynames not supported by the interpreter of
   Motif translations.
  ----------------------------------------------------------------------*/
static char        *NameCode (char* name)
{
   if (strlen (name) < 5)
      if (name[0] == ',')
	 return ("0x2c");
      else
	 return (name);
   else if (!strcasecmp (name, "Return"))
      return ("0x0d");
   else if (!strcasecmp (name, "Backspace"))
      return ("0x08");
   else if (!strcasecmp (name, "Space"))
      return ("0x20");
   else if (!strcasecmp (name, "Escape"))
      return ("0x18");
   else if (!strcasecmp (name, "Delete"))
      return ("0x7f");
   else
      return (name);
}


/*----------------------------------------------------------------------
   SpecialKey
   translates the name given by the file thot.keyboard into a key value
   which Thot can use.
   The parameter shifted is TRUE when a shifted key is selected.
   Returns TRUE in the parameter isSpecial when it's a special key.
  ----------------------------------------------------------------------*/
static unsigned int  SpecialKey (char *name, ThotBool shifted, ThotBool *isSpecial)
{
   *isSpecial = TRUE;
   /* is it the name of a special character? */
   if (!strcasecmp (name, "Return"))
     {
       strcpy (name, "Enter");
       return (unsigned int) THOT_KEY_Return;
     }
   else if (!strcasecmp (name, "Backspace"))
     return (unsigned int) THOT_KEY_BackSpace;
   else if (!strcasecmp (name, "Space"))
   {
     *isSpecial = FALSE;
     return 32;
   }
   else if (!strcasecmp (name, "Tab"))
   {
     *isSpecial = FALSE;
     if (shifted)
       return (unsigned int) THOT_KEY_TAB;
     else
       return (unsigned int) THOT_KEY_Tab;
   }
   else if (!strcasecmp (name, "Escape"))
     return (unsigned int) THOT_KEY_Escape;
   else if (!strcasecmp (name, "Delete"))
     return (unsigned int) THOT_KEY_Delete;
   else if (!strcasecmp (name, "F1"))
      return (unsigned int) THOT_KEY_F1;
   else if (!strcasecmp (name, "F2"))
      return (unsigned int) THOT_KEY_F2;
   else if (!strcasecmp (name, "F3"))
      return (unsigned int) THOT_KEY_F3;
   else if (!strcasecmp (name, "F4"))
      return (unsigned int) THOT_KEY_F4;
   else if (!strcasecmp (name, "F5"))
      return (unsigned int) THOT_KEY_F5;
   else if (!strcasecmp (name, "F6"))
      return (unsigned int) THOT_KEY_F6;
   else if (!strcasecmp (name, "F7"))
      return (unsigned int) THOT_KEY_F7;
   else if (!strcasecmp (name, "F8"))
      return (unsigned int) THOT_KEY_F8;
   else if (!strcasecmp (name, "F9"))
      return (unsigned int) THOT_KEY_F9;
   else if (!strcasecmp (name, "F10"))
      return (unsigned int) THOT_KEY_F10;
   else if (!strcasecmp (name, "F11") || !strcasecmp (name, "l1"))
      return (unsigned int) THOT_KEY_F11;
   else if (!strcasecmp (name, "F12") || !strcasecmp (name, "l2"))
      return (unsigned int) THOT_KEY_F12;
   else if (!strcasecmp (name, "F13") || !strcasecmp (name, "l3"))
      return (unsigned int) THOT_KEY_F13;
   else if (!strcasecmp (name, "F14") || !strcasecmp (name, "l4"))
      return (unsigned int) THOT_KEY_F14;
   else if (!strcasecmp (name, "F15") || !strcasecmp (name, "l5"))
      return (unsigned int) THOT_KEY_F15;
   else if (!strcasecmp (name, "F16") || !strcasecmp (name, "l6"))
      return (unsigned int) THOT_KEY_F16;
   else if (!strcasecmp (name, "F17") || !strcasecmp (name, "l7"))
      return (unsigned int) THOT_KEY_F17;
   else if (!strcasecmp (name, "F18") || !strcasecmp (name, "l8"))
      return (unsigned int) THOT_KEY_F18;
   else if (!strcasecmp (name, "F19") || !strcasecmp (name, "l9"))
      return (unsigned int) THOT_KEY_F19;
   else if (!strcasecmp (name, "F20") || !strcasecmp (name, "l10"))
      return (unsigned int) THOT_KEY_F20;
   else if (!strcasecmp (name, "Up"))
     return (unsigned int) THOT_KEY_Up;
   else if (!strcasecmp (name, "Down"))
     return (unsigned int) THOT_KEY_Down;
   else if (!strcasecmp (name, "Left"))
     return (unsigned int) THOT_KEY_Left;
   else if (!strcasecmp (name, "Right"))
     return (unsigned int) THOT_KEY_Right;
   else if (!strcasecmp (name, "Home"))
     return (unsigned int) THOT_KEY_Home;
   else if (!strcasecmp (name, "End"))
     return (unsigned int) THOT_KEY_End;
   else
   {
     *isSpecial = FALSE;
     return (unsigned int) name[0];
   }
}



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
static void MemoKey (int mod1, int key1, ThotBool spec1, int mod2, int key2,
		     ThotBool spec2, int key, int command)
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
	    case THOT_MOD_SHIFT:
	       addFirst = &Automata_SHIFT;
	       break;
	    case THOT_MOD_CTRL:
	       addFirst = &Automata_ctrl;
	       break;
	    case THOT_MOD_ALT:
	       addFirst = &Automata_alt;
	       break;
	    case THOT_MOD_S_CTRL:
	       addFirst = &Automata_CTRL;
	       break;
	    case THOT_MOD_S_ALT:
	       addFirst = &Automata_ALT;
	       break;
	    default:
	       return;
	 }

   /* Initializations */
   ptr = (KEY *) TtaGetMemory (sizeof (KEY));
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
	     if (oldptr->K_EntryCode == key1 && oldptr->K_Special == spec1)
		exists = TRUE;	/* the key1 entry already exists */
	     else if (oldptr->K_Other != NULL)
	       {
		  oldptr = oldptr->K_Other;
		  if (oldptr->K_EntryCode == key1 && oldptr->K_Special == spec1)
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
		 ptr->K_Special = spec1;
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
		 ptr->K_Special = spec2;
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
		 ptr->K_Special = spec2;
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
	ptr->K_Special = spec1;
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
void WIN_CharTranslation (HWND hWnd, int frame, UINT msg, WPARAM wParam,
			  LPARAM lParam, ThotBool isSpecial)
{
   char string[2];
   int  keyboard_mask = 0;   
   int  status;
   int  len;

   if (frame < 0)
     return;

   len = 1;
   status = GetKeyState (VK_SHIFT);
   if (HIBYTE (status)) 
     /* the Shift key is pressed */
     keyboard_mask |= THOT_MOD_SHIFT;
   status = GetKeyState (VK_CONTROL);
   if (HIBYTE (status))
     /* the Control key is pressed */
     keyboard_mask |= THOT_MOD_CTRL;
   /* Alt key is a particular key for Windows. It generates a WM_SYSKEYDOWN and */
   /* usulally we have not to trap this event and let Windows do.  In our case, */
   /* we do not use the standard accelerator tables as in common Windows appli. */
   /* Is the Alt key pressed ?? */
   status = GetKeyState (VK_MENU);
   if (HIBYTE (status))
      /* the Alt key is pressed */
      keyboard_mask |= THOT_MOD_ALT;

   if (msg == WM_KEYDOWN && wParam == VK_RETURN && 
	   !(keyboard_mask & THOT_MOD_ALT))
	   return;
   /* check if it's a special key */
   Special = isSpecial;
   if (HIBYTE (GetKeyState (VK_UP)))
     Special = TRUE;
   else if (HIBYTE (GetKeyState (VK_LEFT)))
     Special = TRUE;
   else if (HIBYTE (GetKeyState (VK_RIGHT)))
     Special = TRUE;
   else if (HIBYTE (GetKeyState (VK_UP)))
     Special = TRUE;
   else if (HIBYTE (GetKeyState (VK_DOWN)))
     Special = TRUE;
   else if (HIBYTE (GetKeyState (VK_BACK)))
     Special = TRUE;
   else if (HIBYTE (GetKeyState (VK_DELETE)))
     Special = TRUE;
   else if (HIBYTE (GetKeyState (VK_PRIOR)))
     Special = TRUE;
   else if (HIBYTE (GetKeyState (VK_NEXT)))
     Special = TRUE;
   else if (HIBYTE (GetKeyState (VK_HOME)))
     Special = TRUE;
   else if (HIBYTE (GetKeyState (VK_END)))
     Special = TRUE;
   else if (HIBYTE (GetKeyState (VK_RETURN)))
     Special = TRUE;
   else if (keyboard_mask & THOT_MOD_CTRL && wParam < 32)
     {
       /* Windows translates Ctrl a-z */
       if (keyboard_mask & THOT_MOD_SHIFT)
         wParam += 64;
       else
	 wParam += 96;
     }
   
   string[0] = (char) wParam;
   if (msg == WM_SYSCHAR || msg == WM_SYSKEYDOWN)
     len = 0;
   else
     {
       if (wParam == 0x0A)
	 /* Linefeed key */
	 wParam = 0x0D;
     }
   ThotInput (frame, &string[0], len, keyboard_mask, wParam);
}
#else /* _WINDOWS */

/*----------------------------------------------------------------------
   CharTranslation
   X-Window front-end to the character translation and handling.
   Decodes the X-Window event  and calls the generic character
   handling function.
  ----------------------------------------------------------------------*/
#ifdef _GTK
void CharTranslation (GdkEventKey * event, gpointer * data)
{
   int                 status;
   int                 PicMask;
   int                 frame;
   unsigned int        state, save;
   unsigned char             string[2];
   ThotComposeStatus   ComS;
   KeySym              KS;

   frame = (int) data;
   if (frame > MAX_FRAME)
      frame = 0;

   status = 0;
   /* control, alt and mouse status bits of the state are ignored */
   state = event->state & (GDK_SHIFT_MASK | GDK_LOCK_MASK | GDK_MOD3_MASK);
   if (event->state == state)
     {
       strncpy (string, event->string, 2);
       KS = event->keyval;
     }
   else
     {
       save = event->state;
       event->state = state;
       state = save;
       strncpy (string, event->string, 2);
       KS = event->keyval;
     }

   PicMask = 0;
   if (state & GDK_SHIFT_MASK)
      PicMask |= THOT_MOD_SHIFT;
   /*if (state & GDK_LOCK_MASK)
     PicMask |= THOT_MOD_SHIFT;*/
   if (state & GDK_CONTROL_MASK)
      PicMask |= THOT_MOD_CTRL;
   if (state & GDK_MOD1_MASK || state & GDK_MOD4_MASK)
      PicMask |= THOT_MOD_ALT;
   ThotInput (frame, &string[0], event->length, PicMask, KS);
}
#else /* _GTK */
void CharTranslation (ThotKeyEvent *event)
{
   KeySym              KS;
   unsigned char             string[2];
   ThotComposeStatus   ComS;
   int                 status;
   int                 PicMask;
   int                 frame;
   unsigned int        state;

   frame = GetWindowFrame (event->window);
   if (frame > MAX_FRAME)
      frame = 0;

   status = 0;
   /* control, alt and mouse status bits of the state are ignored */
   state = event->state & 127;
   if (event->state == 127)
     status = TtaXLookupString (event, string, 2, &KS, &ComS);
   else
     status = XLookupString (event, string, 2, &KS, &ComS);

   PicMask = 0;
   if (state & ShiftMask)
      PicMask |= THOT_MOD_SHIFT;
   /*if (state & LockMask)
     PicMask |= THOT_MOD_SHIFT;*/
   if (state & ControlMask)
      PicMask |= THOT_MOD_CTRL;
   if (state & Mod1Mask || state & Mod4Mask)
      PicMask |= THOT_MOD_ALT;

   ThotInput (frame, &string[0], status, PicMask, KS);
}
#endif /* _GTK */
#endif /* !_WINDOWS */

/*----------------------------------------------------------------------
   APPKey send a message msg to the application.   
  ----------------------------------------------------------------------*/
static ThotBool APPKey (int msg, PtrElement pEl, Document doc, ThotBool pre)
{
   PtrElement          pParentEl;
   NotifyOnTarget      notifyEl;
   ThotBool            result;
   ThotBool            ok;

   result = FALSE;
   pParentEl = pEl;
   notifyEl.event = msg;
   notifyEl.document = doc;
   notifyEl.targetdocument = doc;
   while (pParentEl != NULL)
     {
       notifyEl.element = (Element) pParentEl;
       notifyEl.target = (Element) pEl;
       ok = CallEventType ((NotifyEvent *) & notifyEl, pre);
       result = result || ok;
       pParentEl = pParentEl->ElParent;
     }
   return result;
}


/*----------------------------------------------------------------------
   ThotInput
   handles the character encoding.                     
  ----------------------------------------------------------------------*/
void ThotInput (int frame, unsigned char *string, unsigned int nb,
		int PicMask, int key)
{
  KEY                *ptr;
  Document            document;
  View                view;
  int                 value;
  int                 modtype;
  int                 command;
  int                 mainframe;
  int                 index;
  ThotBool            found, done;
  
  if (frame > MAX_FRAME)
    frame = 0;
  FrameToView (frame, &document, &view);
  value = string[0];
  found = FALSE;
  if (nb == 2)
    {
      /* It's a Thot action call */
      command = (int) string[1];
      found = TRUE;
    }
  else
    {
      command = 0;	   
      /* Set the right indicator */
      if (PicMask & THOT_MOD_CTRL)
	{
	if (PicMask & THOT_MOD_SHIFT)
	  modtype = THOT_MOD_S_CTRL;
	else
	  modtype = THOT_MOD_CTRL;
	}
      else if (PicMask & THOT_MOD_ALT)
	{
	if (PicMask & THOT_MOD_SHIFT)
	  modtype = THOT_MOD_S_ALT;
	else
	  modtype = THOT_MOD_ALT;
	}
      else if (PicMask & THOT_MOD_SHIFT)
	modtype = THOT_MOD_SHIFT;
      else
	modtype = THOT_NO_MOD;
    
      /* Is it a second level of the current automata? */
      if (Automata_current != NULL)
	{
	  /* search a second level entry */
	  ptr = Automata_current;
	  Automata_current = NULL;
	  while (!found && ptr != NULL)
	    {
	      if (ptr != NULL)
		{
		  if (ptr->K_EntryCode == key &&
#ifdef _WINDOWS
		      ptr->K_Special == Special &&
#endif /* _WINDOWS */
		      modtype == ptr->K_Modifier)
		    found = TRUE;
		  else
		    ptr = ptr->K_Other;
		}
	    }
      
	  if (found)
	    {
	      value = (unsigned char) ptr->K_Value;
	      command = ptr->K_Command;
	    }
	}
      else
	{
	  /* Search a first level entry? */
	  if (modtype == THOT_MOD_S_CTRL)
	    ptr = Automata_CTRL;
	  else if (modtype == THOT_MOD_CTRL)
	    {
	      /* check if it's an access key */
	      if (!strcasecmp (TtaGetEnvString ("ACCESSKEY_MOD"), "ctrl") &&
		  AccessKeyFunction && document && DocAccessKey[document - 1])
		{
		  ptr = DocAccessKey[document - 1];
		  while (ptr != NULL && ptr->K_EntryCode != key)
		    ptr = ptr->K_Other;
		  if (ptr)
		    {
		      /* close the current insertion */
		      CloseInsertion ();
		      (*AccessKeyFunction) (document, ptr->K_Param);
		      return;
		    }
		}
	      ptr = Automata_ctrl;
	    }
	  else if (modtype == THOT_MOD_S_ALT)
	    ptr = Automata_ALT;
	  else if (modtype == THOT_MOD_ALT)
	    {
	      /* check if it's an access key */
	      if (!strcasecmp (TtaGetEnvString ("ACCESSKEY_MOD"), "alt") &&
		  AccessKeyFunction && document && DocAccessKey[document - 1])
		{
		  ptr = DocAccessKey[document - 1];
		  while (ptr != NULL && ptr->K_EntryCode != key)
		    ptr = ptr->K_Other;
		  if (ptr)
		    {
		      /* close the current insertion */
		      CloseInsertion ();
		      (*AccessKeyFunction) (document, ptr->K_Param);
		      return;
		    }
		}
	      ptr = Automata_alt;
	    }
	  else if (modtype == THOT_MOD_SHIFT)
	    ptr = Automata_SHIFT;
	  else
	    ptr = Automata_normal;

	    while (!found && ptr != NULL)
	      {
		if (ptr != NULL)
		  {
#ifdef _WINDOWS
		    if (ptr->K_EntryCode == key
			&& ptr->K_Special == Special)
#else /* _WINDOWS */
		    if (ptr->K_EntryCode == key)
#endif /* _WINDOWS */
		      {
			/* first level entry found */
			found = TRUE;
			Automata_current = ptr->K_Next;
			if (Automata_current == NULL)
			  {
			    /* one key shortcut */
			    value = (unsigned char) ptr->K_Value;
			    command = ptr->K_Command;
			  }
		      }
		    else
		      ptr = ptr->K_Other;
		  }
	      }
	}
    }
  
#ifdef _WINDOWS
  if (Special && !found)
#else /* !_WINDOWS */
  if (!found)
#endif /* _WINDOWS */
    {
      /* Handling special keys */
      switch (key)
	{
	case THOT_KEY_Up:
	  index = MY_KEY_Up;
	  break;
	case THOT_KEY_Return:
	  index = MY_KEY_Return;
	  break;
	case THOT_KEY_Left:
	  index = MY_KEY_Left;
	  break;
	case THOT_KEY_Right:
	  index = MY_KEY_Right;
	  break;
	case THOT_KEY_Down:
	  index = MY_KEY_Down;
	  break;
	case THOT_KEY_Prior:
	  index = MY_KEY_Prior;
	  break;
	case THOT_KEY_Next:
	  index = MY_KEY_Next;
	  break;
	case THOT_KEY_Home:
	  index = MY_KEY_Home;
	  break;
	case THOT_KEY_End:
	  index = MY_KEY_End;
	  break;
	case THOT_KEY_BackSpace:
	  index = MY_KEY_BackSpace;
	  break;
	case THOT_KEY_Delete:
	  index = MY_KEY_Delete;
	  break;
	default:
	  index = -1;
	  break;
	}

      if (index >= 0)
	{
	  if (modtype == THOT_MOD_SHIFT)
	    command = SpecialShiftKeys[index];
	  else if (modtype == THOT_MOD_CTRL)
	    command = SpecialCtrlKeys[index];
	  else if (modtype == THOT_MOD_S_CTRL)
	    command = SpecialShiftCtrlKeys[index];
	  else
	    command = SpecialKeys[index];
	  Automata_current = NULL;
	}
    } 

  if (Automata_current == NULL)
    {
      /* Appel d'une action Thot */
      mainframe = GetWindowNumber (document, 1);
      if (command > 0)
	{
	  if (command != CMD_DeletePrevChar)
	    /* It's not a delete, close the current insertion */
	    CloseInsertion ();

	  /* ***Check events TteElemReturn and TteElemTab*** */
	  if (LoadedDocument[document - 1] == SelectedDocument &&
	      command == CMD_CreateElement)
	    /* check if the application wants to handle the return */
	    done = APPKey (TteElemReturn, FirstSelectedElement, document, TRUE);
	  else if (LoadedDocument[document - 1] == DocSelectedAttr &&
	      command == CMD_CreateElement)
	    /* check if the application wants to handle the return */
	    done = APPKey (TteElemReturn, AbsBoxSelectedAttr->AbElement, document, TRUE);
	  else if (LoadedDocument[document - 1] == SelectedDocument &&
		   value == TAB)
	    /* check if the application wants to handle the Tab */
	    done = APPKey (TteElemTab, FirstSelectedElement, document, TRUE);
	  else if (LoadedDocument[document - 1] == DocSelectedAttr &&
		   value == TAB)
	    /* check if the application wants to handle the return */
	    done = APPKey (TteElemTab, AbsBoxSelectedAttr->AbElement, document, TRUE);
	  else
	    done = FALSE;
	  /* Call action if it's active */
	  if (!done &&
	      (MenuActionList[command].ActionActive[frame] ||
	       MenuActionList[command].ActionActive[mainframe]))
	    {
	      /* available action for this frame or the main frame */
	      if (MenuActionList[command].Call_Action)
		(*MenuActionList[command].Call_Action) (document, view);

	      /* ***Check events TteElemReturn and TteElemTab*** */
	      if (LoadedDocument[document - 1] == SelectedDocument &&
		  command == CMD_CreateElement)
		/* post treatment for the application */
		APPKey (TteElemReturn, FirstSelectedElement, document, FALSE);
	      else if (LoadedDocument[document - 1] == DocSelectedAttr &&
		       command == CMD_CreateElement)
		/* check if the application wants to handle the return */
		APPKey (TteElemReturn, AbsBoxSelectedAttr->AbElement, document, FALSE);
	      else if (LoadedDocument[document - 1] == SelectedDocument &&
		       value == TAB)
		/* post treatment for the application */
		APPKey (TteElemTab, FirstSelectedElement, document, FALSE);
	      else if (LoadedDocument[document - 1] == DocSelectedAttr &&
		       value == TAB)
		/* check if the application wants to handle the return */
		APPKey (TteElemTab, AbsBoxSelectedAttr->AbElement, document, FALSE);
	    }
	}
     else if (nb == 0)
       {
	 /* Rien a inserer */ 
	 Automata_current = NULL;
	 return;
       }
      /* Traitement des caracteres au cas par cas */
      else
	{
	  if (value == 8 || value == 127)
	    {
	      /* Par defaut BackSpace detruit le caractere precedent */
	      /* sans se soucier de la langue courante */
	      if (MenuActionList[CMD_DeletePrevChar].Call_Action)
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
		(*MenuActionList[0].Call_Action) (document, view, SPACE);
	    }
	  else if (value == 9 ||
		   ( value >= 32 && value < 128) || (value >= 144 && value < 256))
	    {
	      if (LoadedDocument[document - 1] == SelectedDocument &&
		  value == TAB)
		/* check if the application wants to handle the Tab */
		done = APPKey (TteElemTab, FirstSelectedElement, document, TRUE);
	      else if (LoadedDocument[document - 1] == DocSelectedAttr &&
		       value == TAB)
		/* check if the application wants to handle the return */
		done = APPKey (TteElemTab, AbsBoxSelectedAttr->AbElement, document, TRUE);
	      else
		done = FALSE;
	      /* on insere un caractere valide quelque soit la langue */
	      if (!done && MenuActionList[0].Call_Action)
		(*MenuActionList[0].Call_Action) (document, view, value);
	      if (LoadedDocument[document - 1] == SelectedDocument &&
		  value == TAB)
		/* post treatment for the application */
		APPKey (TteElemTab, FirstSelectedElement, document, FALSE);
	      else if (LoadedDocument[document - 1] == DocSelectedAttr &&
		       value == TAB)
		/* check if the application wants to handle the return */
		APPKey (TteElemTab, AbsBoxSelectedAttr->AbElement, document, FALSE);
	    }
	}
    }
}


/*----------------------------------------------------------------------
  FreeOneTranslationsTable frees a translation table.
  ----------------------------------------------------------------------*/
static void FreeOneTranslationsTable (KEY *current)
{
   KEY                *ptr, *subkey;

   while (current != NULL)
     {
       ptr = current;
       while (ptr->K_Next != NULL)
	 {
	   subkey = ptr->K_Next;
	   ptr->K_Next = subkey->K_Other;
	   TtaFreeMemory (subkey);
	 }
       current = ptr->K_Other;
       TtaFreeMemory (ptr);
     }
}

/*----------------------------------------------------------------------
   FreeTranslations removes all translation structures.
  ----------------------------------------------------------------------*/
void FreeTranslations ()
{
   int                 i;

   /* free all document access keys */
   for (i = 1; i <= MAX_DOCUMENTS; i++)
     TtaRemoveDocAccessKeys (i);
   FreeOneTranslationsTable (Automata_normal);
   Automata_normal = NULL;
   FreeOneTranslationsTable (Automata_ctrl);
   Automata_ctrl = NULL;
   FreeOneTranslationsTable (Automata_alt);
   Automata_alt = NULL;
   FreeOneTranslationsTable (Automata_SHIFT);
   Automata_SHIFT = NULL;
   FreeOneTranslationsTable (Automata_CTRL);
   Automata_CTRL = NULL;
   FreeOneTranslationsTable (Automata_ALT);
   Automata_ALT = NULL;
}


/*----------------------------------------------------------------------
   EndOfString check wether string end by suffix.
  ----------------------------------------------------------------------*/
static int      EndOfString (char *string, char *suffix)
{
   int             string_lenght, suffix_lenght;

   string_lenght = strlen (string);
   suffix_lenght = strlen (suffix);
   if (string_lenght < suffix_lenght)
      return 0;
   else
      return (strcmp (string + string_lenght - suffix_lenght, suffix) == 0);
}


/*----------------------------------------------------------------------
  TtaSetAccessKeyFunction registers the access key function.
  ----------------------------------------------------------------------*/
void      TtaSetAccessKeyFunction (Proc procedure)
{
  AccessKeyFunction = procedure;
}


/*----------------------------------------------------------------------
  TtaAddAccessKey registers a new access key for the document doc
  The parameter param which will be returned when the access key will be
  activated.
  ----------------------------------------------------------------------*/
void      TtaAddAccessKey (Document doc, unsigned int key, void *param)
{
  KEY                *ptr, *next;
  unsigned int        k;

  if (doc)
    {
      /* looks for the current access key in the table */
      k = tolower (key);
      next = DocAccessKey[doc - 1];
      ptr = NULL;
      while (next != NULL && next->K_EntryCode != (int) k)
	{
	  ptr = next;
	  next = next->K_Other;
	}
      if (next == NULL)
	{
	  /* not found: add a new entry */
	  next = (KEY *) TtaGetMemory (sizeof (KEY));
	  if (ptr)
	    ptr->K_Other = next;
	  else
	    /* the first entry */
	    DocAccessKey[doc - 1] = next;
	  next->K_EntryCode = k;
	  next->K_Special = FALSE;
	  next->K_Other = NULL;
	  next->K_Param = param;
	  next->K_Command = -1;
	  next->K_Value = key;
	}
    }
}


/*----------------------------------------------------------------------
  TtaRemoveDocAccessKeys removes all access keys of a document.
  ----------------------------------------------------------------------*/
void      TtaRemoveDocAccessKeys (Document doc)
{
  KEY                *ptr, *next;

  if (doc)
    {
      next = DocAccessKey[doc - 1];
      while (next != NULL)
	{
	  ptr = next;
	  next = ptr->K_Other;
	  TtaFreeMemory (ptr);
	}
      DocAccessKey[doc - 1] = NULL;
    }
}


/*----------------------------------------------------------------------
  TtaRemoveAccessKey removes an access key of a document.
  ----------------------------------------------------------------------*/
void      TtaRemoveAccessKey (Document doc, unsigned int key)
{
  KEY                *ptr, *next;
  unsigned int        k;

  if (doc)
    {
      /* looks for the current access key in the table */
      k = tolower (key);
      next = DocAccessKey[doc - 1];
      ptr = NULL;
      while (next != NULL && next->K_EntryCode != (int) k)
	{
	  ptr = next;
	  next = next->K_Other;
	}
      if (next)
	{
	  /* found: remove it */
	  if (ptr)
	    ptr->K_Other = next->K_Other;
	  else
	    /* the first entry */
	    DocAccessKey[doc - 1] = next->K_Other;
	  TtaFreeMemory (next);
	}
    }
}


/*----------------------------------------------------------------------
   InitTranslations
   intializes the keybord encoding.
  ----------------------------------------------------------------------*/
ThotTranslations      InitTranslations (char *appliname)
{
  char*             appHome;	   /* fichier de translation */
  char              fullName[200];  /* ligne en construction pour motif */
  char              home[200]; 
  char              name[80]; 
  char*               text;	   
  char*               adr;
  char                transText [MAX_LENGTH];
  char                ch[80]; 
  char                line[200];  /* ligne en construction pour motif */
  char                equiv[MAX_EQUIV]; /* equivalents caracteres pour motif */
  unsigned int        key1, key2; /* 1ere & 2eme cles sous forme de keysym X */
  int                 e, i;
  int                 mod1, mod2; /* 1er/ 2eme modifieurs : voir THOT_MOD_xx */
  int                 len, max;
  FILE               *file;
  ThotTranslations    table = 0;
  ThotBool            isSpecialKey1, isSpecialKey2;

  /* clean up the access key table */
  for (i = 0; i < MAX_DOCUMENTS; i++)
    DocAccessKey[i] = NULL;

  appHome = TtaGetEnvString ("APP_HOME");
  strcpy (name, appliname);
#ifdef _WINDOWS
  strcat (name, ".kb");
#else  /* _WINDOWS */
  strcat (name, ".keyboard");
#endif /* _WINDOWS */

  strcpy (home, appHome);
  strcat (home, DIR_STR);
  strcat (home, name);
  if (!SearchFile (home, 0, fullName))
    SearchFile (name, 2, fullName);

  file = fopen (fullName, "r");
  if (!file)
    /*Fichier inaccessible */
    return NULL;
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
      len = strlen ("TtcInsertChar") + 2;
      
      /* FnCopy la premiere ligne du fichier (#override, ou #...) */
      strcpy (text, "#override\n");
      ch[0] = 0;
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
	    {
	      /* it is not a comment */
	      /* -------> Lecture des autres champs */
	      if (!strcasecmp (ch, "shift"))
		{
		  mod1 = THOT_MOD_SHIFT;
		  /* copie 1er modifieur */
		  strcpy (equiv, "Shift");
		  strcat (equiv, " ");
		  strcat (line, "Shift");
		  strcat (line, " ");
		  /* Lecture enventuelle d'un deuxieme modifieur */
		  ch[0] = 0;
		  fscanf (file, "%80s", ch);
		}
	      else
		{
		  mod1 = THOT_NO_MOD;
		  equiv[0] = EOS;
		}
	      
	      if (!strcasecmp (ch, "ctrl"))
		{
		  mod1 += THOT_MOD_CTRL;
		  /* copie 2eme modifieur */
		  strcat (equiv, "Ctrl");
		  strcat (equiv, " ");
		  strcat (line, "Ctrl");
		  strcat (line, " ");
		  /* Lecture de la cle */
		  ch[0] = 0;
		  fscanf (file, "%80s", ch);
		}
	      else if (!strcasecmp (ch, "alt") || !strcasecmp (ch, "meta"))
		{
		  mod1 += THOT_MOD_ALT;
		  /* copie 2eme modifieur */
		  strcat (equiv, "Alt");
		  strcat (equiv, " ");
		  strcat (line, "Alt");
		  strcat (line, " ");
		  /* Lecture de la cle */
		  ch[0] = 0;
		  fscanf (file, "%80s", ch);
		} 

	      /* Extrait la valeur de la cle */
	      sscanf (ch, "<Key>%80s", transText);
	      if (name[0] != EOS)
		{
		  /* copie de la cle */
		  strcat (line, "<Key>");
		  i = strlen (transText);
		  /* Elimine le : a la fin du nom */
		  if ((transText[i - 1] == ':') && i != 1)
		    {
		      /* Il faut engendrer un : apres le nom */
		      transText[i - 1] = EOS;
		      i = 1;
		    }
		  else
		    i = 0;
		  /* copie le nom normalise */
		  strcat (line, NameCode (transText));
		  if (i == 1)
		    strcat (line, ": ");
		  else
		    strcat (line, " ");
		}

	      /* convert to keysym for the automata */
	      key1 = SpecialKey (transText, mod1 & THOT_MOD_SHIFT, &isSpecialKey1);
	      strcat (equiv, transText);

	      /* Lecture eventuelle d'une deuxieme composition */
	      fscanf (file, "%80s", transText);
	      if (transText[0] == ',')
		{
		  /* copie du separateur */
		  strcat (line, ", ");
		  ch[0] = 0;
		  fscanf (file, "%80s", ch);
		      
		  if (!strcasecmp (ch, "shift"))
		    {
		      mod2 = THOT_MOD_SHIFT;
		      /* copie du 2eme modifieur */
		      strcat (equiv, "Shift");
		      strcat (equiv, " ");
		      strcat (line, "Shift");
		      strcat (line, " ");
		      /* Lecture enventuelle d'un deuxieme modifieur */
		      ch[0] = 0;
		      fscanf (file, "%80s", ch);
		    }
		  else
		    {
                      mod2 = THOT_NO_MOD;
                      strcat (equiv, " ");
		    }

		  if (!strcasecmp (ch, "ctrl"))
		    {
		      mod2 += THOT_MOD_CTRL;
		      /* copie 2eme modifieur */
		      strcat (equiv, "Ctrl");
		      strcat (equiv, " ");
		      strcat (line, "Ctrl");
		      strcat (line, " ");
		      /* copie de la cle */
		      ch[0] = 0;
		      fscanf (file, "%80s", ch);
		      strcat (line, ch);
		      strcat (line, " ");
		    }
		  else if (!strcasecmp (ch, "alt") || !strcasecmp (ch, "meta"))
		    {
                      mod2 += THOT_MOD_ALT;
                      /* copie 2eme modifieur */
                      strcat (equiv, "Alt");
                      strcat (equiv, " ");
                      strcat (line, "Alt");
                      strcat (line, " ");
                      /* copie de la cle */
		      ch[0] = 0;
                      fscanf (file, "%80s", ch);
                      strcat (line, ch);
                      strcat (line, " ");
		    }

		  /* Extrait la valeur de la cle */
		  sscanf (ch, "<Key>%80s", transText);
		  if (transText [0] != EOS)
		    {
		      strcat (line, "<Key>");   /* copie de la cle */
		      i = strlen (transText);
		      /* Elimine le : a la fin du nom */
		      if (transText[i - 1] == ':' && i != 1)
			{
			  /* Il faut engendrer un : apres le nom */
			  transText[i - 1] = EOS;
			  i = 1;
			}
		      else
                        i = 0;
		      /* copie le nom normalise */
		      strcat (line, NameCode (transText));
		      if (i == 1)
			strcat (line, ": ");
		      else
			strcat (line, " ");
		    } 
		  key2 = SpecialKey (transText, mod2 & THOT_MOD_SHIFT, &isSpecialKey2);
		  strcat (equiv, transText);

		  /* Lecture de l'action */
		  fscanf (file, "%80s", transText);
		}

	      /* Isole l'intitule de la commande */
	      strncpy (ch, transText, 80);
	      adr = strchr (ch, '(');
	      if (adr == NULL)
		adr = strchr (ch, SPACE);
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
		  if (!strcmp (ch, "TtcInsertChar"))
		    {
		      strcat (text, "insert-string(");
		      strcat (text, AsciiTranslate (&adr[1]));
		    }
		  else if (!strcmp (ch, "TtcDeleteSelection"))
		    strcat (text, "delete-selection()");
		  else if (!strcmp (ch, "TtcDeletePreviousChar"))
                    strcat (text, "delete-prev-char()");
		  else if (!strcmp (ch, "TtcPreviousChar"))
                    strcat (text, "backward-char()");
		  else if (!strcmp (ch, "TtcNextChar"))
                    strcat (text, "forward-char()");
		  else if (!strcmp (ch, "TtcPreviousLine"))
                    strcat (text, "previous-line()");
		  else if (!strcmp (ch, "TtcNextLine"))
                    strcat (text, "next-line()");
		  else if (!strcmp (ch, "TtcStartOfLine"))
                    strcat (text, "beginning-of-line()");
		  else if (!strcmp (ch, "TtcEndOfLine"))
                    strcat (text, "end-of-line()");
		  strcat (text, "\n");
		}

	      if (i == 0)
		{
		  /* C'est l'action insert-string */
		  /* FnCopy la ligne dans le source de la table de translations */
		  strcat (text, line);
		  strcat (text, AsciiTranslate (transText));
		  strcat (text, "\n");
		  /* C'est un encodage de caractere */
		  adr = AsciiTranslate (&transText[len]);
		  MemoKey (mod1, key1, isSpecialKey1,
			   mod2, key2, isSpecialKey2, (unsigned int) adr[0], 0);
		}
	      else if (i < max)
		{
		  /* C'est une autre action Thot */
		  MemoKey (mod1, key1, isSpecialKey1,
			   mod2, key2, isSpecialKey2, /*255+i */ 0, i);
		  /* On met a jour l'equivalent clavier */
		  TtaFreeMemory (MenuActionList[i].ActionEquiv);
		  MenuActionList[i].ActionEquiv = TtaStrdup (equiv);
		}
	    }
	  else
	    {
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
#ifndef _WINDOWS
      table = XtParseTranslationTable (text);
#endif  /* !_WINDOWS */
      TtaFreeMemory (text);
      return table;
    }
}
