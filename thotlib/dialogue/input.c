/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2008
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * 
 * Module for keyboard input handling.
 * Authors: I. Vatton (INRIA)
 *          D. Veillard (INRIA) - Removed X remapping of keys,
 *                                lead to crash in some configurations
 */
#include "wx/wx.h"

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

#include "appdialogue_wx.h"
#include "AmayaWindow.h"
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
  CMD_CutSelection,
  CMD_SelPageUp,
  CMD_SelPageDown,
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
static Proc2        AccessKeyFunction = NULL;
static KEY         *DocAccessKey[MAX_DOCUMENTS];



/*----------------------------------------------------------------------
  KeyName translates a key value into a key name.
  ----------------------------------------------------------------------*/
static const char *KeyName (unsigned int key)
{
  static char s[2];

  switch (key)
    {
    case THOT_KEY_Return:
      return "Enter";
    case THOT_KEY_BackSpace:
      return "Backspace";
    case THOT_KEY_Tab:
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
  unsigned char c;

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
      c = (unsigned char) name[0];
      return (unsigned int) c;
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
  Return TRUE if the key is not registred (already declared )
  ----------------------------------------------------------------------*/
static ThotBool MemoKey (int mod1, int key1, ThotBool spec1, int mod2,
                         int key2, ThotBool spec2, int key, int command)
{
  ThotBool            isNew;
  KEY                *ptr = NULL;
  KEY                *oldptr;
  KEY               **addFirst;

  if (key1 == 0)
    return FALSE;

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
      return FALSE;
    }

  oldptr = *addFirst;		/* debut chainage entrees existantes */
  /* Verifies if we already have a ctrl key */
  isNew = TRUE;
  if (oldptr)
    {
      /* verifies if this key is already recorded */
      do
        {
          /* is it the same entry key ? */
          if (oldptr->K_EntryCode == key1 && oldptr->K_Special == spec1)
            isNew = FALSE;	/* the key1 entry already exists */
          else if (oldptr->K_Other)
            {
              oldptr = oldptr->K_Other;
              if (oldptr->K_EntryCode == key1 && oldptr->K_Special == spec1)
                isNew = FALSE;	/* we must also verify the last entry */
            }
        }
      while (oldptr->K_Other && isNew);
    }

  /* is it a two key sequence with a modifier on the first one? */
  if ((key2 != 0) && (mod1 != THOT_NO_MOD))
    {
      /* Does the first level entry already exists ? */
      if (isNew)
        {
          /* Create a first level entry */
          ptr = (KEY *) TtaGetMemory (sizeof (KEY));
          ptr->K_EntryCode = key1;
          ptr->K_Special = spec1;
          ptr->K_Next = NULL;
          ptr->K_Other = NULL;
          ptr->K_Command = 0;
          ptr->K_Value = 0;

          /* Chainage a l'entree precedente */
          if (oldptr)
            oldptr->K_Other = ptr;
          else
            *addFirst = ptr;

          oldptr = ptr;
          ptr = NULL;
        }

      /* check the list of second level entries */
      isNew = TRUE;
      if (oldptr->K_Next == NULL)
        {
          /* create a second level entry */
          ptr = (KEY *) TtaGetMemory (sizeof (KEY));
          ptr->K_EntryCode = key2;
          ptr->K_Special = spec2;
          ptr->K_Modifier = mod2;
          ptr->K_Other = NULL;
          ptr->K_Command = command;
          ptr->K_Value = key;
          oldptr->K_Next = ptr;
        }
      else
        {
          oldptr = oldptr->K_Next;
          do
            {
              /* Est-ce la meme cle d'entree */
              if (oldptr->K_EntryCode == key2)
                /* L'entree existe deja */
                isNew = FALSE;
              else if (oldptr->K_Other)
                {
                  oldptr = oldptr->K_Other;
                  /* Il faut en plus verifier la derniere entree */
                  if (oldptr->K_EntryCode == key2)
                    isNew = FALSE;
                }
            }
          while (oldptr->K_Other && isNew);

          /* Si l'entree de 2eme niveau n'existe pas deja ? */
          if (isNew)
            {
              /* create a second level entry */
              ptr = (KEY *) TtaGetMemory (sizeof (KEY));
              ptr->K_EntryCode = key2;
              ptr->K_Special = spec2;
              ptr->K_Modifier = mod2;
              ptr->K_Other = NULL;
              ptr->K_Command = command;
              ptr->K_Value = key;
              
              /* Chainage a l'entree precedente */
              if (oldptr)
                oldptr->K_Other = ptr;
            }
        }
    }
  else if (isNew)
    {
      /* Create a first level entry */
      ptr = (KEY *) TtaGetMemory (sizeof (KEY));
      ptr->K_EntryCode = key1;
      ptr->K_Special = spec1;
      ptr->K_Other = NULL;
      ptr->K_Next = NULL;
      ptr->K_Command = command;
      ptr->K_Value = key;

      /* Chainage a l'entree precedente */
      if (oldptr)
        oldptr->K_Other = ptr;
      else
        *addFirst = ptr;
    }
  return isNew;
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
  The parameter modifiers gives current modifiers.
  Returns:
  - 0 when nothing is done
  - 1 when an access key is handled
  - 2 when an action is done
  - 3 when a character is inserted
  ----------------------------------------------------------------------*/
int ThotInput (int frame, unsigned int value, int command, int modifiers,
               int key, ThotBool isKey)
{
  KEY                *ptr;
  Document            document;
  View                view;
  int                 modtype;
  int                 mainframe;
  int                 index = -1;
  ThotBool            found, done = FALSE;
  
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
  TTALOGDEBUG_5( TTA_LOG_KEYINPUT, _T("ThotInput: frame=%d value=%d modifiers=%d key=%d isKey=%d"),
                 frame, value, modifiers, key, isKey);
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
      if (modifiers & THOT_MOD_CTRL)
        {
          if (modifiers & THOT_MOD_SHIFT)
            modtype = THOT_MOD_S_CTRL;
          else
            modtype = THOT_MOD_CTRL;
        }
      else if (modifiers & THOT_MOD_ALT)
        {
          if (modifiers & THOT_MOD_SHIFT)
            modtype = THOT_MOD_S_ALT;
          else
            modtype = THOT_MOD_ALT;
        }
      else if (modifiers & THOT_MOD_SHIFT)
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
                    if (ptr->K_EntryCode == key)
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
                    else
                      ptr = ptr->K_Other;
                }
            }
        }
    }
  /* found, key */
  
  /* @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ */
  if (isKey && !found)
      {
        /* Handling special keys */
        switch (key)
          {
          case THOT_KEY_Up:
            index = MY_KEY_Up;
            break;
          case WXK_NUMPAD_ENTER:
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
          (modtype == THOT_MOD_CTRL || modtype == THOT_MOD_S_CTRL
#ifndef _MACOS
           || modtype == THOT_MOD_ALT || modtype == THOT_MOD_S_ALT
#endif /* _MACOS */
))
        return 0;
      
      mainframe = GetWindowNumber (document, 1);
      if (key == TAB && LoadedDocument[document - 1] &&
          LoadedDocument[document - 1]->DocSSchema &&
          LoadedDocument[document - 1]->DocSSchema->SsName &&
          !strcmp (LoadedDocument[document - 1]->DocSSchema->SsName, "TextFile"))
        {
          /* Tab is considered as a simple character in text files */
          command = 0;
          value = TAB;
        }
      if (command > 0)
        {
          /* Call a registered Thot action */
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
              MenuActionList[command].ActionActive[document])
        {
          /* available action for this frame or the main frame */
          if (MenuActionList[command].Call_Action)
            {
              if (strstr (MenuActionList[command].ActionName, "Close") != NULL ||
                  strstr (MenuActionList[command].ActionName, "Split") != NULL)
              /* I just generate an event which contains "command, doc, and view"
               * then I post it on current window eventhandler in order
               * to differe the shortcut action to avoid crash which can occurs 
               * when an action destroy something which is used further by wxWidgets. */
                AmayaWindow::DoAmayaAction( command, document, view );
              else
                TtaExecuteMenuActionFromActionId(command, document, view, FALSE);
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
          // check the fullscreen state is enable or not
          // if yes, just disable fullscreen
          if (TtaGetFullScreenState(frame))
            TtaToggleOnOffFullScreen(frame);
          else if (MenuActionList[CMD_ParentElement].Call_Action)
            {
              /* close the current insertion */
              CloseTextInsertion ();
              (*(Proc2)MenuActionList[CMD_ParentElement].Call_Action) ((void *)document,
                                                                       (void *)view);
              return 2;
            }
          else
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
#ifdef _MACOS
      else if ( modtype == THOT_MOD_ALT &&
                (/* value == 230 || */ value == 37650 || value == '^' ||
               value == '`'))
        return 0;
#endif /* _MACOS */
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
static int EndOfString (const char *string, const char *suffix)
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
void TtaSetAccessKeyFunction (Proc2 procedure)
{
  AccessKeyFunction = (Proc2)procedure;
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
    Automata_current = NULL;
}

/*----------------------------------------------------------------------
  TtaInShortcutSequence
  Return TRUE if there is an open shortcut sequence.
  ----------------------------------------------------------------------*/
ThotBool TtaInShortcutSequence ()
{
  return (Automata_current != NULL);
}

/*----------------------------------------------------------------------
  TtaListShortcuts
  Produces in a file a human-readable the list of current shortcuts.  
  ----------------------------------------------------------------------*/
void TtaListShortcuts (Document doc, FILE *fileDescriptor)
{
  KEY                *next, *ptr;
  char               sk1[10];
  const char         *s, *k1, *k2;
  int                 i;

  s = TtaGetEnvString ("ACCESSKEY_MOD");
  k1 = &sk1[0];
  k2 = k1;
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
              strcpy (sk1, KeyName (next->K_EntryCode));
              ptr = next->K_Next;
              if (ptr == NULL)
                {
                  /* display the shortcut */
                  if (MenuActionList[next->K_Command].ActionName)
                    fprintf (fileDescriptor, " %s%s -> %s\n", s, sk1,
                             MenuActionList[next->K_Command].ActionName);
                }
              else
                /* two levels */
                while (ptr)
                  {
                    /* display the shortcut sequence */
                    if (MenuActionList[ptr->K_Command].ActionName)
                      {
                        k2 = KeyName (ptr->K_EntryCode);
                        fprintf (fileDescriptor, " %s%s %s%s -> %s\n", s, sk1,
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
  char               *addr;
  char                transText[MAX_LENGTH], ch[MAX_LENGTH];
  char                equiv[MAX_LENGTH]; /* equivalents caracteres pour motif */
  unsigned int        key1, key2; /* 1ere & 2eme cles sous forme de keysym X */
  int                 e, i, j;
  int                 mod1, mod2; /* 1er/ 2eme modifieurs : voir THOT_MOD_xx */
  int                 len, max, value;
  FILE               *file;
  ThotBool            isSpecialKey1, isSpecialKey2, show_sequence = FALSE;

  /* clean up the access key table */
  memset(DocAccessKey, 0, sizeof(KEY *) * MAX_DOCUMENTS);
#ifdef _MACOS
  TtaSetEnvBoolean ("SHOW_SEQUENCES", FALSE, FALSE);
#else /* _MACOS */
  TtaSetEnvBoolean ("SHOW_SEQUENCES", TRUE, FALSE);
#endif /* _MACOS */
  TtaGetEnvBoolean ("SHOW_SEQUENCES", &show_sequence);
  appHome = TtaGetEnvString ("APP_HOME");
  strcpy (name, appliname);

#ifdef _WINDOWS
  strcat (name, ".kb");
#else  /* _WINDOWS */
#ifdef _MACOS
  // change the unix choice for MacOS platforms
  strcat (name, ".kb-mac");
#else /* _MACOS */
  strcat (name, ".keyboard");
#endif /* _MACOS */
#endif  /* _WINDOWS */

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
              if (show_sequence || ch[0] != ',')
                {
#ifdef _MACOS
                  if (ch[0] != ',')
                  /* it's not a sequence */
                    strcpy (equiv, "\t");
                  if ((mod1 & THOT_MOD_CTRL) && (mod1 & THOT_MOD_ALT))
                    /* specific to MacOS */
                    strcat (equiv, "Ctrl-Alt");
#else /* _MACOS */
                  if (!(mod1 & THOT_MOD_CTRL))
                    /* it's not a sequence */
                    strcpy (equiv, "\t");
#endif /* _MACOS */
                  if (ch[0] == ',')
                     strcpy (equiv, "(");
                  if (mod1 & THOT_MOD_CTRL)
                    strcat (equiv, "Ctrl");
                  else if (mod1 & THOT_MOD_ALT)
                    strcat (equiv, "Alt");
                  if (mod1 & THOT_MOD_SHIFT)
                    {
                      if (mod1 != THOT_MOD_SHIFT)
                         strcat (equiv, "-");
                       strcat (equiv, "Shift");
                    }
                  if (mod1 != THOT_NO_MOD)
                    {
                      if (ch[0] == ',')
                       strcat (equiv, " ");
                      else
                       strcat (equiv, "-");
                    }
                  if (ch[0] != ',' && transText[0] >= 'a' && transText[0] <= 'z')
                    SetCapital (transText);
                  strcat (equiv, transText);
                }

              if (ch[0] == ',')
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
                  /* register the second part of the equiv string */

                  if (show_sequence)
                    {
                      strcat (equiv, " ");
                      strcat (equiv, transText);
                      strcat (equiv, ")");
                    }
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

              /* Locate the action name */
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
                  /* another action */
                  if (MemoKey (mod1, key1, isSpecialKey1,
                               mod2, key2, isSpecialKey2, /*255+i */ 0, i))
                    {
                      /* display the equiv shortcut */
                      TtaFreeMemory (MenuActionList[i].ActionEquiv);
                      MenuActionList[i].ActionEquiv = TtaStrdup (equiv);
                    }
                }
            }
          else
            {
              /* comment line */
              //fscanf (file, "%80s", ch);
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
