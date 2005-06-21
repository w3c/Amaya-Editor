/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2004
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
 *          P. Cheyrou-Lagreze (INRIA) - gtk input
 */
#ifdef _WX
  #include "wx/wx.h"
#endif /* _WX */

#include "thot_gui.h"
#include "thot_sys.h"
#include "thot_key.h"
#include "constmedia.h"
#include "typemedia.h"
#include "libmsg.h"
#include "message.h"
#include "appdialogue.h"
#include "fileaccess.h"
#include "logdebug.h"
#define MAX_EQUIV 25
#define MAX_AUTOMATA	80
/* automata structure for the keys */
typedef struct _key
{
  int                 K_EntryCode;    /* input key                           */
  int                 K_Command;      /* index in the command list or -1     */
  int                 K_Value;	      /* return key if command = -1          */
  ThotBool            K_Special;      /* TRUE if it's a special key          */
  struct _key        *K_Other;	      /* next entry at the same level        */
  struct _key        *K_Next;	      /* 1st complementary touch (1st level) */
  int                 K_Modifier;     /* modifier value (2nd level)          */
  void               *K_Param;        /* the parameter of the access key     */
}
KEY;

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "boxes_tv.h"
#include "frame_tv.h"
#include "edit_tv.h"
#include "appdialogue_tv.h"
#include "select_tv.h"

#ifdef _GTK
  #include "gtk-functions.h"
  #include "absboxes_f.h" 
#endif /* _GTK */
#if defined(_WINGUI)
  #include "appli_f.h"
  #include "input_f.h"
#endif /* #if defined(_WINGUI) */
#ifdef _WX
  #include "appdialogue_wx.h"
  #include "AmayaWindow.h"
#endif /* _WX */
/* Actions table */
#include "applicationapi_f.h"
#include "callback_f.h"
#include "context_f.h"
#include "displayview_f.h"
#include "editcommands_f.h"
#include "memory_f.h"
#include "message_f.h"
#include "registry_f.h"
#include "spellchecker_f.h" //function SetCapital
#include "structcommands_f.h"
#include "structmodif_f.h"
#include "structselect_f.h"
#include "textcommands_f.h"
#include "windowdisplay_f.h"
#include "ustring_f.h"
#include "appdialogue_f.h"

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
#define MY_KEY_Insert    11
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
  CMD_CreateElement,
  -1
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
  CMD_CreateElement,
  CMD_Paste
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
  CMD_LineBreak,
  CMD_Copy
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
  CMD_LineBreak,
  -1
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
#ifdef _WINGUI
static ThotBool    Special;
#endif /* _WINGUI */


#if 0 /* This function is not used, could be removed ??? */
/*----------------------------------------------------------------------
   NameCode
   translates the keynames not supported by the interpreter of
   Motif translations.
  ----------------------------------------------------------------------*/
static char *NameCode (char* name)
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
#endif /* 0 */

/*----------------------------------------------------------------------
   KeyName translates a key value into a key name.
  ----------------------------------------------------------------------*/
static char *KeyName (unsigned int key)
{
  static char s[2];

  switch (key)
    {
    case THOT_KEY_Return:
      return "Enter";
    case THOT_KEY_BackSpace:
      return "Backspace";
    case THOT_KEY_Tab:
#if !defined(_WINGUI) && !defined(_WX)
    case THOT_KEY_TAB:
#endif /* !defined(_WINGUI) && !defned(_WX) */
      return "Tab";
    case THOT_KEY_Escape:
      return "Escape";
    case THOT_KEY_Delete:
      return "Delete";
    case THOT_KEY_F1:
      return "F1";
    case THOT_KEY_F2:
      return "F2";
    case THOT_KEY_F3:
      return "F3";
    case THOT_KEY_F4:
      return "F4";
    case THOT_KEY_F5:
      return "F5";
    case THOT_KEY_F6:
      return "F6";
    case THOT_KEY_F7:
      return "F7";
    case THOT_KEY_F8:
      return "F8";
    case THOT_KEY_F9:
      return "F9";
    case THOT_KEY_F10:
      return "F10";
    case THOT_KEY_F11:
      return "F11";
    case THOT_KEY_F12:
      return "F12";
    case THOT_KEY_F13:
      return "F13";
    case THOT_KEY_F14:
      return "F14";
    case THOT_KEY_F15:
      return "F15";
    case THOT_KEY_F16:
      return "F16";
    case THOT_KEY_F17:
      return "F17";
    case THOT_KEY_F18:
      return "F18";
    case THOT_KEY_F19:
      return "F19";
    case THOT_KEY_F20:
      return "F20";
    case THOT_KEY_Up:
      return "Up";
    case THOT_KEY_Down:
      return "Down";
    case THOT_KEY_Left:
      return "Left";
    case THOT_KEY_Right:
      return "Right";
    case THOT_KEY_Home:
      return "Home";
    case THOT_KEY_End:
      return "End";
    case SPACE:
      return "Space";
    default:
      s[0] = key;
      s[1] = EOS;
      return s;
    }
}

/*----------------------------------------------------------------------
   SpecialKey
   translates the name given by the file thot.keyboard into a key value
   which Thot can use.
   The parameter shifted is TRUE when a shifted key is selected.
   Returns TRUE in the parameter isSpecial when it's a special key.
  ----------------------------------------------------------------------*/
static unsigned int SpecialKey (char *name, ThotBool shifted, ThotBool *isSpecial)
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
   else if (!exists)
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


#ifdef _WINGUI
/*----------------------------------------------------------------------
   MSCharTranslation
   MS-Window front-end to the character translation and handling.
   Decodes the MS-Window callback parameters and calls the
   generic character handling function.
   Returns TRUE if an access key was executed.
  ----------------------------------------------------------------------*/
ThotBool WIN_CharTranslation (HWND hWnd, int frame, UINT msg, WPARAM wParam,
			      LPARAM lParam, ThotBool isSpecial)
{
   int  keyboard_mask = 0;   
   int  status, ret;

   if (frame < 0)
     return FALSE;
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
   {
      if (keyboard_mask == THOT_MOD_CTRL)
		/* ctrl + alt = altgr */
		keyboard_mask = 0;
	  else
       /* the Alt key is pressed */
       keyboard_mask |= THOT_MOD_ALT;
   }

   status = GetKeyState (VK_SHIFT);
   if (HIBYTE (status)) 
     /* the Shift key is pressed */
     keyboard_mask |= THOT_MOD_SHIFT;

   if (msg == WM_KEYDOWN && wParam == VK_RETURN && 
       !(keyboard_mask & THOT_MOD_ALT))
     return FALSE;

   /* check if it's a special key */
   Special = isSpecial;
#ifdef OLD_VERSION
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
#else /* OLD_VERSION */
    if (HIBYTE (GetKeyState (VK_RETURN)))
       Special = TRUE;
	else if (keyboard_mask & THOT_MOD_CTRL && wParam < 32)
	{
        /* Windows translates Ctrl a-z */
        if (keyboard_mask & THOT_MOD_SHIFT)
          wParam += 64;
        else
	      wParam += 96;
	}
#endif /* OLD_VERSION */

   if (msg != WM_SYSCHAR && msg != WM_SYSKEYDOWN &&
       wParam == 0x0A)
	 /* Return should generate a linefeed key
        Removing this test will break Ctrl Return */
	 wParam = 0x0D;
   ret = ThotInput (frame, (unsigned int) wParam, 0, keyboard_mask, wParam);
   return (ret == 1);
}

#endif /* _WINGUI */

#ifdef _GTK
/*----------------------------------------------------------------------
   CharTranslationGTK
   GTK front-end to the character translation and handling.
   Decodes the GTK key press event  and calls the generic character
   handling function.
  ----------------------------------------------------------------------*/
gboolean CharTranslationGTK (GtkWidget *w, GdkEventKey* event, gpointer data)
{
  CHARSET             charset;
  Document            document;
  View                view;
  KeySym              key;
  GtkWidget          *drawing_area;
  GtkEntry           *textzone;
  wchar_t            *str, *p;
  unsigned int        state, save;
  int                 status;
  int                 PicMask;
  int                 frame;

  frame = (int) data;
  if (frame > MAX_FRAME)
    frame = 0;
  FrameToView (frame, &document, &view);

  /* the drawing area is the main zone where keypress event must be active */
  drawing_area = FrameTable[frame].WdFrame;
  /* Focus is on all the drawing frame : 
     Drawing area and his hiden text catcher (for multikey),
     and the URL text textzone, 
     so we must now know where is the focus, 
     to analyse the meaning of the keypress
     and setting it on one of the  textfields*/
  if (FrameTable[frame].Text_Zone)
    {
      if (GTK_WIDGET_HAS_FOCUS (FrameTable[frame].Text_Zone))
	/* We're in the url zone*/
	return FALSE;
      else
	{ 
	  /* We're in the drawing so get the hidden textfield adress*/		 
	  textzone = (GtkEntry*)gtk_object_get_data (GTK_OBJECT (drawing_area), "Text_catcher");
	  gtk_widget_grab_focus (GTK_WIDGET(textzone));
	}
    }
  status = 0;
  /* control, alt and mouse status bits of the state are ignored */
  state = event->state & (GDK_SHIFT_MASK | GDK_LOCK_MASK | GDK_MOD3_MASK);

  key = event->keyval;
  if (event->state != state)
    {
      save = event->state;
      event->state = state;
      state = save;
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
  if (event->keyval == GDK_VoidSymbol)
    {
      /******* Not sure this code makes sense */
      charset = TtaGetCharset (TtaGetEnvString ("Input_Charset"));
      if (charset != UNDEFINED_CHARSET)
	{
	  str = TtaConvertByteToWC ((unsigned char*)event->string, charset);
	  p = str;
	  while (*p)
	    {
	      if (MenuActionList[0].Call_Action)
		(*(Proc3)MenuActionList[0].Call_Action) (
			(void *)document,
			(void *)view,
			(void *)*p);
	      p++;
	    }
	  TtaFreeMemory (str);
	  return FALSE;
	}
    }
  if ( strlen(event->string) > 0 )
    {
      /* event->string is encoded in system locale charset */
      CHARSET local_charset = TtaGetLocaleCharset();
      wchar_t * value = TtaConvertByteToWC((unsigned char *)event->string, local_charset);
      ThotInput (frame, (unsigned int)value[0], 0, PicMask, key);
    }
  else
    ThotInput (frame, (unsigned int)EOS, 0, PicMask, key);
  gtk_signal_emit_stop_by_name (GTK_OBJECT(w), "key_press_event");
  return TRUE;
}

/*----------------------------------------------------------------------
  GtkLining
  When user hold clicked a button or pressed a key
  those functions are called by a timer each 100 ms
  in order to repeat user action until he released the button
  or move away from the widget.
----------------------------------------------------------------------*/
gboolean GtkLiningDown (gpointer data)
{
  int       frame;
  Document  doc; 
  int       view;
  
   frame = (int) data; 
   FrameToView (frame, &doc, &view);
   TtcLineDown (doc, view);
   /* As this is a timeout function, return TRUE so that it
     continues to get called */
   return TRUE;
}

/*----------------------------------------------------------------------
----------------------------------------------------------------------*/
gboolean GtkLiningUp (gpointer data)
{
  int       frame;
  Document  doc; 
  int       view;
  
  frame = (int) data; 
  FrameToView (frame, &doc, &view);
  TtcLineUp (doc, view);
  /* As this is a timeout function, return TRUE so that it
     continues to get called */
  return TRUE;
}

/*----------------------------------------------------------------------
   KeyScrolledGTK
   Capture click on the scrollbar in order to enable
   Crtl + click event, that let user go on top or bottom of the doc
   Click on button in order to move only for one line and without calling
   recursivly the FrameVScrolled callback.   
  ----------------------------------------------------------------------*/
gboolean KeyScrolledGTK (GtkWidget *w, GdkEvent* event, gpointer data)
{
  int                 frame;
  GdkEventKey         *eventkey;
  GdkEventButton      *eventmouse;
  Document            doc; 
  int                 view;
  int                 x, y;
  int                 height;
  static int          timer = None; 
  GdkModifierType state;
  GtkEntry           *textzone;

  frame = (int) data; 
  FrameToView (frame, &doc, &view);
  textzone = 0;
  if (timer != None)
    {
      gtk_timeout_remove (timer);
      timer = None;
    } 
  if (event->type != GDK_LEAVE_NOTIFY)
    gtk_widget_grab_focus (GTK_WIDGET(w));  
  if (event->type == GDK_BUTTON_PRESS)
    {
      eventmouse = (GdkEventButton*) event;
      if (eventmouse->button == 1) 
	{
	  /* 16 is the pixel size of the scrollbar buttons*/
	  height = w->allocation.height - 16;
	  gdk_window_get_pointer (w->window, &x, &y, &state);
	  if (eventmouse->state & GDK_CONTROL_MASK)
	    {
	      if (y > height)
		JumpIntoView (frame, 100);			
	      else if (y < 16)
		JumpIntoView (frame, 0);
	    }
	  else if  (y > height){
	    TtcLineDown (doc, view); 
	    timer = gtk_timeout_add (100, GtkLiningDown, (gpointer) frame);
	  }
	  else if  (y < 16){
	    TtcLineUp (doc, view);	    
	    timer = gtk_timeout_add (100, GtkLiningUp, (gpointer) frame);
	  }
	  else
	    return FALSE;
	  /* this prevent GTK Callback to act !!*/
	  gtk_signal_emit_stop_by_name (GTK_OBJECT(w), "button_press_event");
	  return TRUE;
	}
		 
    } 
  /* Code is here, but didn't manage to always catch those events..
    But as CRTL + UP and CTRL + DOWN work like UP and DOWN should...
    The problem is that Key event are catched before, by the Main Window 
    In CharTranslation GTK... We need to redesign all Key catching if we 
    wanna catch UP and DOWN... As we sadly cannot get mouse position in a key event... */
  else if (event->type == GDK_KEY_PRESS)
    {
      eventkey = (GdkEventKey*) event;
      if (eventkey->keyval == GDK_Up) 
	{ 
	  TtcLineDown (doc, view); 
	  timer = gtk_timeout_add (50, GtkLiningDown, (gpointer) frame);
	  /* this prevent GTK Callback to act !!*/
	  gtk_signal_emit_stop_by_name (GTK_OBJECT(w), "key_press_event");
	  return TRUE;
	}
      else if (eventkey->keyval == GDK_Down)
	{ 
	  TtcLineDown (doc, view); 
	  timer = gtk_timeout_add (50, GtkLiningDown, (gpointer) frame);
	   /* this prevent GTK Callback to act !!*/
	  gtk_signal_emit_stop_by_name (GTK_OBJECT(w), "key_press_event");
	  return TRUE;
	}
    }

  return FALSE;
}
#endif /* _GTK */

/*----------------------------------------------------------------------
   CharTranslationWX
   WX front-end to the character translation and handling.
   Decodes the WX key press event  and calls the generic character
   handling function.
  ----------------------------------------------------------------------*/
void CharTranslationWX ( int frame, int thot_mask, ThotKeySym thot_keysym,
			 unsigned int value )
{
#if 0
#ifdef _WX
  CHARSET             charset;
  wchar_t            *str, *p;

  Document            document;
  View                view;

#ifdef __WXDEBUG__
  printf( "KeyPressed :%s\n", KeyName (thot_keysym) );
#endif /* #ifdef __WXDEBUG__ */
  
  if (frame > MAX_FRAME)
    frame = 0;
  FrameToView (frame, &document, &view);
 
  if(thot_keysym == 0)
    {
      /******* Not sure this code makes sense */
      charset = TtaGetCharset (TtaGetEnvString ("Default_Charset"));
      if (charset != UNDEFINED_CHARSET)
	{
	  str = TtaConvertByteToWC ((unsigned char*)&value, charset);
	  p = str;
	  while (*p)
	    {
	      if (MenuActionList[0].Call_Action)
		(*(Proc3)MenuActionList[0].Call_Action) (
			(void *)document,
			(void *)view,
			(void *)*p);
	      p++;
	    }
	  TtaFreeMemory (str);
	  return FALSE;
	}
    }

  ThotInput (frame, value, 0, thot_mask, thot_keysym);
  
  return TRUE;
#endif /* _WX */
#endif
}


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
   notifyEl.event = (APPevent)msg;
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
  ThotInput handles the unicode character v and the command command.
  The parameter PicMask gives current modifiers.
  Returns:
  - 0 when nothing is done
   - 1 when an access key is handled
   - 2 when an action is done
   - 3 when a character is inserted
  ----------------------------------------------------------------------*/
int ThotInput (int frame, unsigned int value, int command, int PicMask, int key)
{
  KEY                *ptr;
  Document            document;
  View                view;
  int                 modtype;
  int                 mainframe;
  int                 index = -1;
  ThotBool            found, done = FALSE;
  
#ifdef _WX
  TTALOGDEBUG_5( TTA_LOG_KEYINPUT, _T("ThotInput: frame=%d\tvalue=%d\tcommand=%d\tPicMask=%d\tkey=%d"),
                 frame,
                 value,
                 command,
                 PicMask,
                 key  );
#endif /* _WX */
  
  modtype = THOT_NO_MOD;
  if (frame > MAX_FRAME)
    frame = 0;
  FrameToView (frame, &document, &view);
  found = FALSE;
  if (command)
    /* It's a Thot action call */
    found = TRUE;
  else
    {
      if (ClickIsDone == 1 &&
          (key == THOT_KEY_Escape || key == THOT_KEY_Delete))
        /* Amaya is waiting for a clickselection */
        {
          ClickIsDone = 0;
          ClickFrame = 0;
          ClickX = 0;
          ClickY = 0;
          return 0;
        }
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
#ifdef _WINGUI
                      ptr->K_Special == Special &&
#endif /* _WINGUI */
                      modtype == ptr->K_Modifier)
                    found = TRUE;
                  else
                    ptr = ptr->K_Other;
                }
            }
          
          if (found)
            {
              value = ptr->K_Value;
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
                      CloseTextInsertion ();
                      (*(Proc2)AccessKeyFunction) ((void *)document,
                                                   (void *)ptr->K_Param);
                      return 1;
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
                      CloseTextInsertion ();
                      (*(Proc2)AccessKeyFunction) ((void *)document,
                                                   (void *)ptr->K_Param);
                      return 1;
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
#ifdef _WINGUI
                  if (ptr->K_EntryCode == key
                      && ptr->K_Special == Special)
#endif /* _WINGUI */
#if defined(_GTK) || defined(_WX)
                    if (ptr->K_EntryCode == key)
#endif /* #if defined(_GTK) || defined(_WX) */
                      {
                        /* first level entry found */
                        found = TRUE;
                        Automata_current = ptr->K_Next;
                        if (Automata_current == NULL)
                          {
                            /* one key shortcut */
                            value = ptr->K_Value;
                            command = ptr->K_Command;
                          }
                      }
#if defined(_GTK) || defined(_WINGUI) || defined(_WX)
                    else
                      ptr = ptr->K_Other;
#endif /* #if defined(_GTK) || defined(_WINGUI) || defined(_WX) */
                }
            }
        }
    }
  /* found, key */
  
  /* @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ */
#ifdef _WINGUI
  if (Special && !found)
#endif /* _WINGUI */
#if defined(_GTK) || defined(_WX)
    if (!found)
#endif /* #if defined(_GTK) || defined(_WX) */
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
          case THOT_KEY_Insert:
            index = MY_KEY_Insert;
            break;
          default:
            index = -1;
#ifdef _WINGUI
            /* Nothing to do */ 
            Automata_current = NULL;
            return 0;
#endif /* _WINGUI */
            break;
          }
        if (index >= 0)
          {
            found = TRUE;
            if (modtype == THOT_MOD_SHIFT)
              command = SpecialShiftKeys[index];
            else if (modtype == THOT_MOD_CTRL)
              command = SpecialCtrlKeys[index];
            else if (modtype == THOT_MOD_S_CTRL)
              command = SpecialShiftCtrlKeys[index];
            else
              command = SpecialKeys[index];
            Automata_current = NULL;
            if (command == -1)
              /* NOP */
              return 0;
          }
      }
  
  /* @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ */
  if (Automata_current == NULL)
    {
      /* don't accept to insert a character when there is CTRL
         or ALT active and no shortcut is found */
      if (!found &&
          (modtype == THOT_MOD_CTRL || modtype == THOT_MOD_S_CTRL ||
           modtype == THOT_MOD_ALT || modtype == THOT_MOD_S_ALT))
        return 0;
      
      /* Appel d'une action Thot */
      mainframe = GetWindowNumber (document, 1);
      if (command > 0)
        {
          if (command != CMD_DeletePrevChar &&
              command != CMD_DeleteSelection &&
              command != CMD_PasteFromClipboard &&
              command != CMD_Paste)
            /* It's not a delete, close the current insertion */
            CloseTextInsertion ();
          
          /* ***Check events TteElemReturn and TteElemTab*** */
          if (LoadedDocument[document - 1] == SelectedDocument &&
              command == CMD_CreateElement)
            /* check if the application wants to handle the return */
            done = APPKey (TteElemReturn, FirstSelectedElement, document,
                           TRUE);
          else if (LoadedDocument[document - 1] == DocSelectedAttr &&
                   command == CMD_CreateElement)
            /* check if the application wants to handle the return */
            done = APPKey (TteElemReturn, AbsBoxSelectedAttr->AbElement,
                           document, TRUE);
          else if (LoadedDocument[document - 1] == SelectedDocument &&
                   value == TAB)
            /* check if the application wants to handle the Tab */
            done = APPKey (TteElemTab, FirstSelectedElement, document, TRUE);
          else if (LoadedDocument[document - 1] == DocSelectedAttr &&
                   value == TAB)
            /* check if the application wants to handle the Tab */
            done = APPKey (TteElemTab, AbsBoxSelectedAttr->AbElement,
                           document, TRUE);
          else
            done = FALSE;
          /* Call action if it's active */
          if (!done &&
#ifdef _WX
              MenuActionList[command].ActionActive[document])
#else /* _WX */
            (MenuActionList[command].ActionActive[frame] ||
             MenuActionList[command].ActionActive[mainframe]))
#endif /* _WX */
        {
          /* available action for this frame or the main frame */
          if (MenuActionList[command].Call_Action)
            {
#ifdef _WX
              /* I just generate an event which contains "command, doc, and view"
               * then I post it on current window eventhandler in order
               * to differe the shortcut action to avoid crash which can occurs 
               * when an action destroy something which is used further by wxWidgets. */
              AmayaWindow::DoAmayaAction( command, document, view );
#else /* _WX */
              (*(Proc2)MenuActionList[command].Call_Action) ((void *)document,
                                                             (void *)view);
#endif /* _WX */
              done = TRUE;
            }
          
          /* ***Check events TteElemReturn and TteElemTab*** */
          if (LoadedDocument[document - 1] == SelectedDocument &&
              command == CMD_CreateElement)
            /* post treatment for the application */
            done = APPKey (TteElemReturn, FirstSelectedElement, document, FALSE);
          else if (LoadedDocument[document - 1] == DocSelectedAttr &&
                   command == CMD_CreateElement)
            /* check if the application wants to handle the return */
            done = APPKey (TteElemReturn, AbsBoxSelectedAttr->AbElement,
                           document, FALSE);
          else if (LoadedDocument[document - 1] == SelectedDocument &&
                   value == TAB)
            /* post treatment for the application */
            done = APPKey (TteElemTab, FirstSelectedElement, document, FALSE);
          else if (LoadedDocument[document - 1] == DocSelectedAttr &&
                   value == TAB)
            /* check if the application wants to handle the Tab */
            done = APPKey (TteElemTab, AbsBoxSelectedAttr->AbElement, document,
                           FALSE);
        }
      if (done)
        return 2;
    }
  /* Traitement des caracteres au cas par cas */
  else
    {
      if (key/*value*/ == THOT_KEY_Escape)
        {
#ifdef _WX
          // check the fullscreen state is enable or not
          // if yes, just disable fullscreen
          if (TtaGetFullScreenState(frame))
            TtaToggleOnOffFullScreen(frame);
          else
            TtaDisplayMessage (CONFIRM, TtaGetMessage (LIB, TMSG_USE_F2));
#else /* _WX */
          TtaDisplayMessage (CONFIRM, TtaGetMessage (LIB, TMSG_USE_F2));
#endif /* _WX */
          return 0;
        }
      else if (value == 8 || value == 127)
        {
          /* Par defaut BackSpace detruit le caractere precedent */
          /* sans se soucier de la langue courante */
          if (MenuActionList[CMD_DeletePrevChar].Call_Action)
            {
              (*(Proc2)MenuActionList[CMD_DeletePrevChar].Call_Action) ((void *)document,
                                                                        (void *)view);
              return 2;
            }
          else
            return 0;
        }
      
      /*** Sequence de traitement des espaces ***/
      if (value == BREAK_LINE || value == THIN_SPACE ||
          value == FOUR_PER_EM || value == UNBREAKABLE_SPACE)
        {
          if (MenuActionList[0].Call_Action)
            {
              (*(Proc3)MenuActionList[0].Call_Action) ((void *)document,
                                                       (void *)view,
                                                       (void *)value);
              done = TRUE;
            }
        }
      else if (value == 9 || value >= 32)
        {
          if (LoadedDocument[document - 1] == SelectedDocument &&
              value == TAB)
            /* check if the application wants to handle the Tab */
            done = APPKey (TteElemTab, FirstSelectedElement, document,
                           TRUE);
          else if (LoadedDocument[document - 1] == DocSelectedAttr &&
                   value == TAB)
            /* check if the application wants to handle the TAB */
            done = APPKey (TteElemTab, AbsBoxSelectedAttr->AbElement,
                           document, TRUE);
          else
            done = FALSE;
          /* on insere un caractere valide quelque soit la langue */
          if (!done && MenuActionList[0].Call_Action)
            {
              (*(Proc3)MenuActionList[0].Call_Action) ((void *)document,
                                                       (void *)view,
                                                       (void *)value);
              done = TRUE;
            }
          if (LoadedDocument[document - 1] == SelectedDocument &&
              value == TAB)
            /* post treatment for the application */
            done = APPKey (TteElemTab, FirstSelectedElement, document, FALSE);
          else if (LoadedDocument[document - 1] == DocSelectedAttr &&
                   value == TAB)
            /* check if the application wants to handle the TAB */
            done = APPKey (TteElemTab, AbsBoxSelectedAttr->AbElement, document,
                           FALSE);
        }
      if (done)
        return 3;
    }
}
return 0;
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
void TtaSetAccessKeyFunction (Proc procedure)
{
  AccessKeyFunction = (Proc)procedure;
}


/*----------------------------------------------------------------------
  TtaAddAccessKey registers a new access key for the document doc
  The parameter param which will be returned when the access key will be
  activated.
  ----------------------------------------------------------------------*/
void TtaAddAccessKey (Document doc, unsigned int key, void *param)
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
void TtaRemoveDocAccessKeys (Document doc)
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
void TtaRemoveAccessKey (Document doc, unsigned int key)
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
   TtaCloseShortcutSequence
   Reset any open shortcut sequence.
  ----------------------------------------------------------------------*/
void TtaCloseShortcutSequence ()
{
  if (Automata_current)
    printf ("Close the current sequence\n");
  Automata_current = NULL;
}

/*----------------------------------------------------------------------
   TtaListShortcuts
   Produces in a file a human-readable the list of current shortcuts.  
  ----------------------------------------------------------------------*/
void TtaListShortcuts (Document doc, FILE *fileDescriptor)
{
  KEY                *next, *ptr;
  char               *s, *k1, sk1[50], *k2, sk2[50];
  int                 i;

  s = TtaGetEnvString ("ACCESSKEY_MOD");
  k1 = &sk1[0];
  k2 = &sk2[0];
  if (doc)
    {
      /* display current access keys table */
      next = DocAccessKey[doc - 1];
      if (next)
	fprintf (fileDescriptor, "Access keys\n");
      while (next)
	{
	  /* display the access key */
	  k1 = KeyName (next->K_EntryCode);
	  fprintf (fileDescriptor, " %s %s\n", s, k1);
	  next = next->K_Other;
	}
      fprintf (fileDescriptor, "\nShortcuts\n");
      i = 0;
      while (i < 6)
	{
	  if (i == 0)
	    {
	      next = Automata_SHIFT;
	      s = "Shift ";
	    }
	  else if (i == 1)
	    {
	      next = Automata_normal;
	      s = "";
	    }
	  else if (i == 2)
	    {
	      next = Automata_ALT;
	      s = "Shift Alt ";
	    }
	  else if (i == 3)
	    {
	      next = Automata_alt;
	      s = "Alt ";
	    }
	  else if (i == 4)
	    {
	      next = Automata_CTRL;
	      s = "Shift Ctrl ";
	    }
	  else
	    {
	      next = Automata_ctrl;
	      s = "Ctrl ";
	    }

	  while (next)
	    {
	      strcpy (k1, KeyName (next->K_EntryCode));
	      ptr = next->K_Next;
	      if (ptr == NULL)
		{
		  /* display the shortcut */
		  if (MenuActionList[next->K_Command].ActionName)
		    fprintf (fileDescriptor, " %s%s -> %s\n", s, k1,
			     MenuActionList[next->K_Command].ActionName);
		}
	      else
		/* two levels */
		while (ptr)
		  {
		    /* display the shortcut sequence */
		    if (MenuActionList[ptr->K_Command].ActionName)
		      {
			strcpy (k2, KeyName (ptr->K_EntryCode));
			fprintf (fileDescriptor, " %s%s %s%s -> %s\n", s, k1,
				 s, k2,
				 MenuActionList[ptr->K_Command].ActionName);
		      }
		    ptr = ptr->K_Other;
		  }
	      next = next->K_Other;
	    }
	  i++;
	}
    }
}

/*----------------------------------------------------------------------
   InitTranslations
   intializes the keybord encoding.
  ----------------------------------------------------------------------*/
void InitTranslations (char *appliname)
{
  char               *appHome;	   /* fichier de translation */
  char                fullName[200];  /* ligne en construction pour motif */
  char                home[200], name[80];
  char               *addr, *sep = "+";
  char                transText[MAX_LENGTH], ch[MAX_LENGTH];
  char                equiv[MAX_LENGTH]; /* equivalents caracteres pour motif */
  unsigned int        key1, key2; /* 1ere & 2eme cles sous forme de keysym X */
  int                 e, i, j;
  int                 mod1, mod2; /* 1er/ 2eme modifieurs : voir THOT_MOD_xx */
  int                 len, max, value;
  FILE               *file;
  ThotBool            isSpecialKey1, isSpecialKey2, no_sequence = FALSE;

  /* clean up the access key table */
  memset(DocAccessKey, 0, sizeof(KEY *) * MAX_DOCUMENTS);

  appHome = TtaGetEnvString ("APP_HOME");
  strcpy (name, appliname);

#ifdef _WINDOWS
  strcat (name, ".kb");
#ifdef _WX
  sep = "-"; /* shortcut separator generated by WX */
#endif /* _WX */
#endif  /* _WINDOWS */
#if defined(_UNIX)
#if defined (_MACOS) && defined (_WX)
  strcat (name, ".kb-mac");
#else /* _MACOS */
  strcat (name, ".keyboard");
#endif /* _MACOS */
#endif /* defined(_UNIX) */

  strcpy (home, appHome);
  strcat (home, DIR_STR);
  strcat (home, name);
  if (!SearchFile (home, 0, fullName))
    SearchFile (name, 2, fullName);

  file = TtaReadOpen (fullName);
  if (file)
    {
      /* beginning of the file */
      fseek (file, 0L, 0);
      e = 1;
      max = MaxMenuAction;
      len = strlen ("TtcInsertChar") + 2;
      
      /* Test if sequences are allowed */
      ch[0] = EOS;
      fscanf (file, "%80s", ch);
      do
	{
	  /* Initialisation */
	  mod1 = mod2 = THOT_NO_MOD;
	  key1 = key2 = 0;
	  equiv[0] = EOS;
	  /* Est-ce la fin de fichier ? */
	  if (strlen (ch) == 0 || EndOfString (ch, "^"))
	    e = 0;
	  else if (ch[0] != '#')
	    {
	      /* it is not a comment */
              while ((!strcmp (ch, "Shift")) || 
                     (!strcmp (ch, "Ctrl")) ||
                     (!strcmp (ch, "Alt")) )
              {
	        if (!strcmp (ch, "Shift"))
		  mod1 += THOT_MOD_SHIFT;
  	        else if (!strcmp (ch, "Ctrl"))
		  mod1 += THOT_MOD_CTRL;
	        else if (!strcmp (ch, "Alt"))
		  mod1 += THOT_MOD_ALT;
		ch[0] = EOS;
		fscanf (file, "%80s", ch);
               }

	      /* remove the end colon */
	      sscanf (ch, "<Key>%80s", transText);
	      if (transText[0] != EOS)
		{
		  i = strlen (transText) - 1;
		  if (i > 0 && transText[i] == ':')
		    transText[i] = EOS;
		}
	      /* convert to keysym for the automata */
	      key1 = SpecialKey (transText, (mod1 & THOT_MOD_SHIFT) != 0, &isSpecialKey1);

	      /* Get the following word in the line */
	      ch[0] = EOS;
	      fscanf (file, "%80s", ch);

	      /* Register the equiv string */
#ifdef _WX
	      if ((no_sequence || !(mod1 & THOT_MOD_CTRL)) && ch[0] != ',')
		/* the shortcut is not a sequence */
		strcpy (equiv, "\t");
	      if ((mod1 & THOT_MOD_CTRL) && (mod1 & THOT_MOD_ALT))
		/* specific to MacOS */
		strcat (equiv, "Ctrl-Alt");
	      else
#endif /* _WX */
	      if (mod1 & THOT_MOD_CTRL)
		strcat (equiv, "Ctrl");
	      else if (mod1 & THOT_MOD_ALT)
		strcat (equiv, "Alt");
	      if (mod1 & THOT_MOD_SHIFT)
		{
#ifdef _WX
		if ((no_sequence || !(mod1 & THOT_MOD_CTRL)) && ch[0] != ',')
		  strcat (equiv, "-");
		else
#endif /* _WX */
		  strcat (equiv, sep);
		strcat (equiv, "Shift");
		}
	      if (mod1 != THOT_NO_MOD)
#ifdef _WX
		if ((no_sequence || !(mod1 & THOT_MOD_CTRL)) && ch[0] != ',')
		  strcat (equiv, "-");
		else
#endif /* _WX */
		  strcat (equiv, sep);

	      if (transText[0] >= 'a' && transText[0] <= 'z')
		SetCapital (transText);
	      strcat (equiv, transText);

	      if (!no_sequence && ch[0] == ',')
		{
		  /* the shortcut is a sequence */
		  ch[0] = EOS;
		  fscanf (file, "%80s", ch);
		      
                  while ((!strcmp (ch, "Shift")) || 
                         (!strcmp (ch, "Ctrl")) ||
                         (!strcmp (ch, "Alt")) )
                  {
	            if (!strcmp (ch, "Shift"))
		      mod2 += THOT_MOD_SHIFT;
  	            else if (!strcmp (ch, "Ctrl"))
		      mod2 += THOT_MOD_CTRL;
	            else if (!strcmp (ch, "Alt"))
		      mod2 += THOT_MOD_ALT;
		    ch[0] = EOS;
		    fscanf (file, "%80s", ch);
                   }

		  /* remove the end colon */
		  sscanf (ch, "<Key>%80s", transText);
		  if (transText[0] != EOS)
		    {
		      i = strlen (transText) - 1;
		      if (i > 0 && transText[i] == ':')
			transText[i] = EOS;
		    }
		  key2 = SpecialKey (transText, (mod2 & THOT_MOD_SHIFT) != 0, &isSpecialKey2);
		  /* register the equiv string */
		  strcat (equiv, " ");
		  if (mod2 & THOT_MOD_CTRL)
		    strcat (equiv, "Ctrl");
		  else if (mod2 & THOT_MOD_ALT)
		    strcat (equiv, "Alt");
		  if (mod2 & THOT_MOD_SHIFT)
		    {
		      strcat (equiv, sep);
		      strcat (equiv, "Shift");
		    }
		  if (mod2 != THOT_NO_MOD)
		    strcat (equiv, sep);
		  if (transText[0] >= 'a' && transText[0] <= 'z')
		    SetCapital (transText);
		  strcat (equiv, transText);

		  /* Get the next word in the line */
		  fscanf (file, "%80s", ch);
		}

	      /* Get the commande name */
	      addr = strchr (ch, '(');
	      if (addr == NULL)
		addr = strchr (ch, SPACE);
	      if (addr)
		addr[0] = EOS;
	      else
		do
		  i = fgetc (file);
		while (i != ')');

	      /* Selection de la bonne commande */
	      for (i = 0; i < max; i++)
		if (!strcmp (ch, MenuActionList[i].ActionName))
		  break;

	      if (i == 0)
		{
		  /* action insert-string */
		  if (ch[len] == '&' && ch[len + 1] == '#')
		    {
		      /* it's an entity */
		      j = 2;
		      while (ch[len + j] != EOS &&
			     ch[len + j] != ';' &&
			     ch[len + j] != ')' &&
			     ch[len + j] != '"')
			j++;
		      ch[len + j] = EOS;
		      if (ch[len + 2] == 'x')
			sscanf (&ch[len + 3], "%x", &value);
		      else
			sscanf (&ch[len + 2], "%d", &value);
		    }
		  else
		    {
		      addr = (char *)AsciiTranslate (&ch[len]);
		      value = (unsigned char) addr[0];
		    }
		  MemoKey (mod1, key1, isSpecialKey1,
			   mod2, key2, isSpecialKey2, value, 0);
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
	      /* comment line */
	      fscanf (file, "%80s", ch);
	      if (!strcasecmp (ch, "no-sequence"))
		no_sequence = TRUE;
	      /* skip this line */
	      do
		i = fgetc (file);
	      while (i != NEW_LINE);
	    }
	  ch[0] = 0;
	  fscanf (file, "%80s", ch);
	} while (e != 0);

      TtaReadClose (file);
    }
}
