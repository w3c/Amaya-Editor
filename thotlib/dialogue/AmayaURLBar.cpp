#ifdef _WX

#include "wx/wx.h"

#include "thot_gui.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "appdialogue.h"
#include "dialog.h"
#include "selection.h"
#include "application.h"
#include "dialog.h"
#include "document.h"
#include "message.h"
#include "libmsg.h"


#include "AmayaURLBar.h"
#include "AmayaWindow.h"
#include "AmayaFrame.h"

#include "appdialogue_wx_f.h"

IMPLEMENT_DYNAMIC_CLASS(AmayaURLBar, wxComboBox)

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaURLBar
 *      Method:  AmayaURLBar
 * Description:  create a new urlbar
 *--------------------------------------------------------------------------------------
 */
AmayaURLBar::AmayaURLBar ( wxWindow *     parent
			   ,AmayaWindow *  amaya_window_parent ) :
  wxComboBox( parent, -1, _T(""), wxDefaultPosition, wxSize(500,-1)/*wxDefaultSize*/ )
{
  m_pAmayaWindowParent = amaya_window_parent;
  m_URLLocked = FALSE;
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaURLBar
 *      Method:  ~AmayaURLBar
 * Description:  destructor
 *--------------------------------------------------------------------------------------
 */
AmayaURLBar::~AmayaURLBar()
{

}

void AmayaURLBar::OnURLSelected( wxCommandEvent& event )
{
  wxLogDebug( _T("AmayaURLBar::OnURLSelected:")+
	      wxString(_T(" getstring="))+event.GetString()+
	      _T(" IsSelection=")+(event.IsSelection()?_T("yes"):_T("no"))+
	      _T(" IsChecked=")+(event.IsChecked()?_T("yes"):_T("no"))+
	      _T(" GetSelection=%d"),event.GetSelection() );

  // if a selected event occurs, it's because the mouse is over an item, but the item is not really selected
  // so I setup a flag because the user will select something when he will release the mouse button
  // and I test this flag in the next generated event : OnURLText which is generated everytime an url item has been selected
  m_NewURLSelected = TRUE;

  event.Skip();
}

void AmayaURLBar::OnURLSelectedd( wxMouseEvent& event )
{
  wxLogDebug( _T("AmayaURLBar::MouseUP:"));
  event.Skip();
}

void AmayaURLBar::OnURLText( wxCommandEvent& event )
{
  wxLogDebug( _T("AmayaURLBar::OnURLText:")+
	      wxString(_T(" getstring="))+event.GetString()+
	      _T(" IsSelection=")+(event.IsSelection()?_T("yes"):_T("no"))+
	      _T(" IsChecked=")+(event.IsChecked()?_T("yes"):_T("no"))+
	      _T(" GetSelection=%d"),event.GetSelection() );

  AmayaFrame * p_frame = m_pAmayaWindowParent->GetActiveFrame();
  if ( p_frame && event.GetString().Len() > 0 )
    {     
      // if this flag is true it's because the user want to select an url
      // so we activate the corresponding callback
      if (m_NewURLSelected && !m_URLLocked)
	{
	  m_URLLocked = TRUE;
	  APP_Callback_URLActivate ( p_frame->GetFrameId(),
				     event.GetString().mb_str(AmayaWindow::conv_ascii) );
	  m_URLLocked = FALSE;

	  m_NewURLSelected = FALSE;
	}
      else
	{
	  // if this is not a new url activation
	  // it's a simple url edition
	  // juste need to update the internal url string for the current frame
	  p_frame->SetFrameURL( event.GetString() );
	}
    }

  event.Skip();
}

void AmayaURLBar::OnURLTextEnter( wxCommandEvent& event )
{
  wxLogDebug( _T("AmayaURLBar::OnURLTextEnter:")+
	      wxString(_T(" getstring="))+event.GetString()+
	      _T(" IsSelection=")+(event.IsSelection()?_T("yes"):_T("no"))+
	      _T(" IsChecked=")+(event.IsChecked()?_T("yes"):_T("no"))+
	      _T(" GetSelection=%d"),event.GetSelection() );

  AmayaFrame * p_frame = m_pAmayaWindowParent->GetActiveFrame();
  if (p_frame)
    {
      APP_Callback_URLActivate ( p_frame->GetFrameId(),
				 event.GetString().mb_str(AmayaWindow::conv_ascii) );
    }
  event.Skip();
}

/*----------------------------------------------------------------------
 *  this is where the event table is declared
 *  the callbacks are assigned to an event type
 *----------------------------------------------------------------------*/
BEGIN_EVENT_TABLE(AmayaURLBar, wxComboBox)
  EVT_COMBOBOX( -1,         AmayaURLBar::OnURLSelected )
  EVT_TEXT( -1,             AmayaURLBar::OnURLText )
  EVT_TEXT_ENTER( -1,       AmayaURLBar::OnURLTextEnter )

  EVT_LEFT_UP(              AmayaURLBar::OnURLSelectedd )
END_EVENT_TABLE()

#endif /* #ifdef _WX */
