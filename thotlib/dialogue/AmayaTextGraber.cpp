#ifdef _WX

#include "wx/wx.h"
#include "AmayaTextGraber.h"
#include "AmayaFrame.h"

#include "thot_gui.h"
#include "thot_sys.h"
#include "constmedia.h"

#include "typemedia.h"
#include "appdialogue.h"
#include "dialog.h"
#include "application.h"
#include "dialog.h"
#include "document.h"
#include "message.h"
#include "libmsg.h"
#include "frame.h"
#include "thot_key.h"

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "frame_tv.h"
#include "select_tv.h"
#include "appli_f.h"
#include "views_f.h"
#include "structselect_f.h"
#include "appdialogue_f.h"
#include "appdialogue_wx_f.h"
#include "input_f.h"


BEGIN_EVENT_TABLE(AmayaTextGraber, wxTextCtrl)
    EVT_KEY_DOWN( AmayaTextGraber::OnKeyDown)
    EVT_KEY_UP(   AmayaTextGraber::OnKeyUp)
    EVT_CHAR(     AmayaTextGraber::OnChar)
    EVT_TEXT(-1,  AmayaTextGraber::OnText)
END_EVENT_TABLE()

int AmayaTextGraber::m_ThotMask = 0;

void AmayaTextGraber::OnText(wxCommandEvent& event)
{
  wxLogDebug( _T("AmayaTextGraber::OnText") );

  wxString s = GetValue();
  SetValue( _T("") );

  // wxkeycodes are directly mapped to thot keysyms :
  // no need to convert the wxwindows keycodes
  wxChar c;
  c = s.GetChar(0);
  int thot_keysym = c;

  // Call the generic function for key events management
  ThotInput (m_AmayaFrameId, thot_keysym, 0, m_ThotMask, thot_keysym);
}

void AmayaTextGraber::OnChar(wxKeyEvent& event)
{

#ifdef __WXDEBUG__
  LogKeyEvent( _T("Char"), event);
#endif /* #ifdef __WXDEBUG__ */

  event.Skip();
}

void AmayaTextGraber::OnKeyUp(wxKeyEvent& event)
{
  // update special keys status
  m_ThotMask = 0;
  if (event.ControlDown())
    m_ThotMask |= THOT_MOD_CTRL;
  if (event.AltDown())
    m_ThotMask |= THOT_MOD_ALT;
  if (event.ShiftDown())
    m_ThotMask |= THOT_MOD_SHIFT;
  
  wxLogDebug( _T("AmayaTextGraber::OnKeyUp thotmask=%x"), m_ThotMask );

#ifdef __WXDEBUG__
  LogKeyEvent( _T("Key up"), event);
#endif /* #ifdef __WXDEBUG__ */

  event.Skip();
}

void AmayaTextGraber::OnKeyDown(wxKeyEvent& event)
{
#ifdef __WXDEBUG__
  switch ( event.GetKeyCode() )
    {
    case WXK_F1:
      // show current position and text length
      {
	long line, column, pos = GetInsertionPoint();
	PositionToXY(pos, &column, &line);

	wxLogDebug(_T("Current position: %ld\nCurrent line, column: (%ld, %ld)\nNumber of lines: %ld\nCurrent line length: %ld\nTotal text length: %u (%ld)"),
		   pos,
		   line, column,
		   (long) GetNumberOfLines(),
		   (long) GetLineLength(line),
		   GetValue().length(),
		   GetLastPosition());

	long from, to;
	GetSelection(&from, &to);
	
	wxString sel = GetStringSelection();
	
	wxLogDebug(_T("Selection: from %ld to %ld."), from, to);
	wxLogDebug(_T("Selection = '%s' (len = %u)"),
		   sel.c_str(), sel.length());
      }
      break;
      
    case WXK_F2:
      // go to the end
      SetInsertionPointEnd();
      break;
      
    case WXK_F3:
      // go to position 10
      SetInsertionPoint(10);
      break;
      
    case WXK_F5:
      // insert a blank line
      WriteText(_T("\n"));
      break;
      
    case WXK_F6:
      wxLogDebug(_T("IsModified() before SetValue(): %d"),
		 IsModified());
      SetValue(_T("SetValue() has been called"));
      wxLogDebug(_T("IsModified() after SetValue(): %d"),
		 IsModified());
      break;
      
    case WXK_F7:
      wxLogDebug(_T("Position 10 should be now visible."));
      ShowPosition(10);
      break;
      
    case WXK_F8:
      wxLogDebug(_T("Control has been cleared"));
      Clear();
      break;
      
    case WXK_F9:
      WriteText(_T("WriteText() has been called"));
      break;
      
    case WXK_F10:
      AppendText(_T("AppendText() has been called"));
      break;
    }
  
  LogKeyEvent( wxT("Key down"), event);
#endif /* #ifdef __WXDEBUG__ */

  // update special keys status
  // convert wx key stats to thot key stats 
  m_ThotMask = 0;
  if (event.ControlDown())
    m_ThotMask |= THOT_MOD_CTRL;
  if (event.AltDown())
    m_ThotMask |= THOT_MOD_ALT;
  if (event.ShiftDown())
    m_ThotMask |= THOT_MOD_SHIFT;

  // get keycode of the current pressed key
  int thot_keysym = event.GetKeyCode();
  
  wxLogDebug( _T("AmayaTextGraber::OnKeyDown thotmask=%x, thot_keysym=%x"), m_ThotMask, thot_keysym );
  
  if ( event.ControlDown() || event.AltDown() )
    {      
      // le code suivant permet de convertire les majuscules
      // en minuscules pour les racourcis clavier specifiques a amaya.
      // OnKeyDown recoit tout le temps des majuscule que Shift soit enfonce ou pas.
      if (!event.ShiftDown())
	{
	  // shift key was not pressed
	  // force the lowercase
	  wxString s((wxChar)thot_keysym);
	  if (s.IsAscii())
	    {
	      wxLogDebug( _T("AmayaFrame::OnKeyDown : thot_keysym=%x s=")+s, thot_keysym );
	      s.MakeLower();
	      wxChar c = s.GetChar(0);
	      thot_keysym = (int)c;
	    }
	}
      // Call the generic function for key events management
      ThotInput (m_AmayaFrameId, thot_keysym, 0, m_ThotMask, thot_keysym);
    }
  else if (  thot_keysym == WXK_INSERT ||
	     thot_keysym == WXK_DELETE ||
	     thot_keysym == WXK_HOME   ||
	     thot_keysym == WXK_PRIOR  ||
	     thot_keysym == WXK_NEXT   ||
	     thot_keysym == WXK_END    ||
	     thot_keysym == WXK_LEFT   ||
	     thot_keysym == WXK_RIGHT  ||
	     thot_keysym == WXK_UP     ||
	     thot_keysym == WXK_DOWN   ||
	     thot_keysym == WXK_ESCAPE ||
	     thot_keysym == WXK_BACK
	     )
    {
      wxLogDebug( _T("AmayaTextGraber::SpecialKey thot_keysym=%x"), thot_keysym );
      // Call the generic function for key events management
      ThotInput (m_AmayaFrameId, thot_keysym, 0, m_ThotMask, thot_keysym);
    }
#if 0
  else if ( event.ControlDown() || event.AltDown() )
    {      
      // le code suivant permet de convertire les majuscules
      // en minuscules pour les racourcis clavier specifiques a amaya.
      // OnKeyDown recoit tout le temps des majuscule que Shift soit enfonce ou pas.
      if (!event.ShiftDown())
	{
	  // shift key was not pressed
	  // force the lowercase
	  wxString s((wxChar)thot_keysym);
	  if ( wxIsprint(thot_keysym) )
	    //	    if (s.IsAscii())
	    {
	      wxLogDebug( _T("AmayaFrame::OnKeyDown : thot_keysym=%x s=")+s, thot_keysym );
	      s.MakeLower();
	      wxChar c = s.GetChar(0);
	      thot_keysym = (int)c;
	    }
	}
      // Call the generic function for key events management
      ThotInput (m_AmayaFrameId, thot_keysym, 0, m_ThotMask, thot_keysym);
    }
#endif /* 0 */
  else
    event.Skip();
}




#ifdef __WXDEBUG__
void AmayaTextGraber::LogKeyEvent(const wxChar *name, wxKeyEvent& event) const
{
  wxString key;
  long keycode = event.GetKeyCode();
  {
    switch ( keycode )
      {
      case WXK_BACK: key = _T("BACK"); break;
      case WXK_TAB: key = _T("TAB"); break;
      case WXK_RETURN: key = _T("RETURN"); break;
      case WXK_ESCAPE: key = _T("ESCAPE"); break;
      case WXK_SPACE: key = _T("SPACE"); break;
      case WXK_DELETE: key = _T("DELETE"); break;
      case WXK_START: key = _T("START"); break;
      case WXK_LBUTTON: key = _T("LBUTTON"); break;
      case WXK_RBUTTON: key = _T("RBUTTON"); break;
      case WXK_CANCEL: key = _T("CANCEL"); break;
      case WXK_MBUTTON: key = _T("MBUTTON"); break;
      case WXK_CLEAR: key = _T("CLEAR"); break;
      case WXK_SHIFT: key = _T("SHIFT"); break;
      case WXK_ALT: key = _T("ALT"); break;
      case WXK_CONTROL: key = _T("CONTROL"); break;
      case WXK_MENU: key = _T("MENU"); break;
      case WXK_PAUSE: key = _T("PAUSE"); break;
      case WXK_CAPITAL: key = _T("CAPITAL"); break;
      case WXK_PRIOR: key = _T("PRIOR"); break;
      case WXK_NEXT: key = _T("NEXT"); break;
      case WXK_END: key = _T("END"); break;
      case WXK_HOME: key = _T("HOME"); break;
      case WXK_LEFT: key = _T("LEFT"); break;
      case WXK_UP: key = _T("UP"); break;
      case WXK_RIGHT: key = _T("RIGHT"); break;
      case WXK_DOWN: key = _T("DOWN"); break;
      case WXK_SELECT: key = _T("SELECT"); break;
      case WXK_PRINT: key = _T("PRINT"); break;
      case WXK_EXECUTE: key = _T("EXECUTE"); break;
      case WXK_SNAPSHOT: key = _T("SNAPSHOT"); break;
      case WXK_INSERT: key = _T("INSERT"); break;
      case WXK_HELP: key = _T("HELP"); break;
      case WXK_NUMPAD0: key = _T("NUMPAD0"); break;
      case WXK_NUMPAD1: key = _T("NUMPAD1"); break;
      case WXK_NUMPAD2: key = _T("NUMPAD2"); break;
      case WXK_NUMPAD3: key = _T("NUMPAD3"); break;
      case WXK_NUMPAD4: key = _T("NUMPAD4"); break;
      case WXK_NUMPAD5: key = _T("NUMPAD5"); break;
      case WXK_NUMPAD6: key = _T("NUMPAD6"); break;
      case WXK_NUMPAD7: key = _T("NUMPAD7"); break;
      case WXK_NUMPAD8: key = _T("NUMPAD8"); break;
      case WXK_NUMPAD9: key = _T("NUMPAD9"); break;
      case WXK_MULTIPLY: key = _T("MULTIPLY"); break;
      case WXK_ADD: key = _T("ADD"); break;
      case WXK_SEPARATOR: key = _T("SEPARATOR"); break;
      case WXK_SUBTRACT: key = _T("SUBTRACT"); break;
      case WXK_DECIMAL: key = _T("DECIMAL"); break;
      case WXK_DIVIDE: key = _T("DIVIDE"); break;
      case WXK_F1: key = _T("F1"); break;
      case WXK_F2: key = _T("F2"); break;
      case WXK_F3: key = _T("F3"); break;
      case WXK_F4: key = _T("F4"); break;
      case WXK_F5: key = _T("F5"); break;
      case WXK_F6: key = _T("F6"); break;
      case WXK_F7: key = _T("F7"); break;
      case WXK_F8: key = _T("F8"); break;
      case WXK_F9: key = _T("F9"); break;
      case WXK_F10: key = _T("F10"); break;
      case WXK_F11: key = _T("F11"); break;
      case WXK_F12: key = _T("F12"); break;
      case WXK_F13: key = _T("F13"); break;
      case WXK_F14: key = _T("F14"); break;
      case WXK_F15: key = _T("F15"); break;
      case WXK_F16: key = _T("F16"); break;
      case WXK_F17: key = _T("F17"); break;
      case WXK_F18: key = _T("F18"); break;
      case WXK_F19: key = _T("F19"); break;
      case WXK_F20: key = _T("F20"); break;
      case WXK_F21: key = _T("F21"); break;
      case WXK_F22: key = _T("F22"); break;
      case WXK_F23: key = _T("F23"); break;
      case WXK_F24: key = _T("F24"); break;
      case WXK_NUMLOCK: key = _T("NUMLOCK"); break;
      case WXK_SCROLL: key = _T("SCROLL"); break;
      case WXK_PAGEUP: key = _T("PAGEUP"); break;
      case WXK_PAGEDOWN: key = _T("PAGEDOWN"); break;
      case WXK_NUMPAD_SPACE: key = _T("NUMPAD_SPACE"); break;
      case WXK_NUMPAD_TAB: key = _T("NUMPAD_TAB"); break;
      case WXK_NUMPAD_ENTER: key = _T("NUMPAD_ENTER"); break;
      case WXK_NUMPAD_F1: key = _T("NUMPAD_F1"); break;
      case WXK_NUMPAD_F2: key = _T("NUMPAD_F2"); break;
      case WXK_NUMPAD_F3: key = _T("NUMPAD_F3"); break;
      case WXK_NUMPAD_F4: key = _T("NUMPAD_F4"); break;
      case WXK_NUMPAD_HOME: key = _T("NUMPAD_HOME"); break;
      case WXK_NUMPAD_LEFT: key = _T("NUMPAD_LEFT"); break;
      case WXK_NUMPAD_UP: key = _T("NUMPAD_UP"); break;
      case WXK_NUMPAD_RIGHT: key = _T("NUMPAD_RIGHT"); break;
      case WXK_NUMPAD_DOWN: key = _T("NUMPAD_DOWN"); break;
      case WXK_NUMPAD_PRIOR: key = _T("NUMPAD_PRIOR"); break;
      case WXK_NUMPAD_PAGEUP: key = _T("NUMPAD_PAGEUP"); break;
      case WXK_NUMPAD_PAGEDOWN: key = _T("NUMPAD_PAGEDOWN"); break;
      case WXK_NUMPAD_END: key = _T("NUMPAD_END"); break;
      case WXK_NUMPAD_BEGIN: key = _T("NUMPAD_BEGIN"); break;
      case WXK_NUMPAD_INSERT: key = _T("NUMPAD_INSERT"); break;
      case WXK_NUMPAD_DELETE: key = _T("NUMPAD_DELETE"); break;
      case WXK_NUMPAD_EQUAL: key = _T("NUMPAD_EQUAL"); break;
      case WXK_NUMPAD_MULTIPLY: key = _T("NUMPAD_MULTIPLY"); break;
      case WXK_NUMPAD_ADD: key = _T("NUMPAD_ADD"); break;
      case WXK_NUMPAD_SEPARATOR: key = _T("NUMPAD_SEPARATOR"); break;
      case WXK_NUMPAD_SUBTRACT: key = _T("NUMPAD_SUBTRACT"); break;
      case WXK_NUMPAD_DECIMAL: key = _T("NUMPAD_DECIMAL"); break;
	
      default:
	{
	  if ( wxIsprint((int)keycode) )
	    key.Printf(_T("'%c'"), (char)keycode);
	  else if ( keycode > 0 && keycode < 27 )
	    key.Printf(_T("Ctrl-%c"), _T('A') + keycode - 1);
	  else
	    key.Printf(_T("unknown (%ld)"), keycode);
	}
      }
  }
  
  wxLogDebug( _T("%s event: %s (flags = %c%c%c%c)"),
	      name,
	      key.c_str(),
	      GetChar( event.ControlDown(), _T('C') ),
	      GetChar( event.AltDown(), _T('A') ),
	      GetChar( event.ShiftDown(), _T('S') ),
	      GetChar( event.MetaDown(), _T('M') ) );
}

#endif /* #ifdef __WXDEBUG__ */




#endif /* _WX */
