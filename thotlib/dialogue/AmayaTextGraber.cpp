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
#include "AmayaParams.h"
#include "appdialogue_wx_f.h"
#include "input_f.h"


BEGIN_EVENT_TABLE(AmayaTextGraber, wxTextCtrl)
    EVT_KEY_DOWN( AmayaTextGraber::OnKeyDown)
  //    EVT_KEY_UP(   AmayaTextGraber::OnKeyUp)
  //    EVT_CHAR(     AmayaTextGraber::OnChar)
    EVT_TEXT(-1,  AmayaTextGraber::OnText)
END_EVENT_TABLE()

int  AmayaTextGraber::m_ThotMask = 0;
bool AmayaTextGraber::m_Lock = false;

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaTextGraber
 *      Method:  OnText
 * Description:  manage simple utf8 text characteres
 *--------------------------------------------------------------------------------------
 */
void AmayaTextGraber::OnText(wxCommandEvent& event)
{
  // to avoid recursive call
  // because Clear() generate a OnText event
  if (!m_Lock)
    {
      m_Lock = true;
      wxString s = GetValue();
      Clear(); // SetValue( _T("") ); // crash on windows ...
      
      wxLogDebug( _T("AmayaTextGraber::OnText s=")+s );
      
      // wxkeycodes are directly mapped to thot keysyms :
      // no need to convert the wxwindows keycodes
      wxChar c;
      c = s.GetChar(0);
      int thot_keysym = c;
      
      // Call the generic function for key events management
      ThotInput (m_AmayaFrameId, thot_keysym, 0, m_ThotMask, thot_keysym);
      m_Lock = false;
    }
}

void AmayaTextGraber::OnChar(wxKeyEvent& event)
{
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

  event.Skip();
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaTextGraber
 *      Method:  OnKeyDown
 * Description:  manage special characteres : arrow keys, ESC ...
 *--------------------------------------------------------------------------------------
 */
void AmayaTextGraber::OnKeyDown(wxKeyEvent& event)
{
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

  if (
	   thot_keysym == WXK_F2       ||
	   thot_keysym == WXK_INSERT ||
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
	   thot_keysym == WXK_BACK   ||
	   thot_keysym == WXK_RETURN ||
	   thot_keysym == WXK_TAB
	   )
    {
      wxLogDebug( _T("AmayaTextGraber::SpecialKey thot_keysym=%x"), thot_keysym );
      // Call the generic function for key events management
      ThotInput (m_AmayaFrameId, thot_keysym, 0, m_ThotMask, thot_keysym);
    }
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
	  if (s.IsAscii())
	    {
	      wxLogDebug( _T("AmayaTextGraber::OnKeyDown : thot_keysym=%x s=")+s, thot_keysym );
	      s.MakeLower();
	      wxChar c = s.GetChar(0);
	      thot_keysym = (int)c;
	    }
	}
      // Call the generic function for key events management
      ThotInput (m_AmayaFrameId, thot_keysym, 0, m_ThotMask, thot_keysym);
    }
  else
    event.Skip();
}

#endif /* _WX */
