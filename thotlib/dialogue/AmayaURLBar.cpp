#ifdef _WX

#include "wx/wx.h"

// Thotlib includes
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

#include "appdialogue_wx_f.h"

#include "AmayaURLBar.h"
#include "AmayaWindow.h"
#include "AmayaFrame.h"

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

  // setup the machine state to identify when a url is really selected and activated
  m_NewURLSelectedState = URL_NOT_SELECTED;
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

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaURLBar
 *      Method:  OnURLSelected
 * Description:  the user has selected a new url with the mouse
 *               so be ready to activate the selected one =>
 *               setup a flag to remember that user has selected something
 *--------------------------------------------------------------------------------------
 */
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
  if ( event.IsChecked() )
      m_NewURLSelectedState = URL_SELECTED;

  event.Skip();
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaURLBar
 *      Method:  OnURLText
 * Description:  the user has typed some text with his keyboard into the url text area
 *               just remember the url text area value
 *               or activate the callback if the flags saying that user has selected
 *               a new url is setup (this is maybe a dirty solution but there is no
 *               event saying that the user has finished with the url selection)
 *--------------------------------------------------------------------------------------
 */
void AmayaURLBar::OnURLText( wxCommandEvent& event )
{
  wxLogDebug( _T("AmayaURLBar::OnURLText:")+
	      wxString(_T(" getstring="))+event.GetString()+
	      _T(" IsSelection=")+(event.IsSelection()?_T("yes"):_T("no"))+
	      _T(" IsChecked=")+(event.IsChecked()?_T("yes"):_T("no"))+
	      _T(" GetSelection=%d"),event.GetSelection() );

  AmayaFrame * p_frame = m_pAmayaWindowParent->GetActiveFrame();
  if ( p_frame )
    {
      if ( !event.GetString().IsEmpty() )
	{
	  // if this flag (m_NewURLSelectedState) is ACTIVATED it's because the user want to activate an url
	  // so we activate the corresponding callback
	  // a second flag m_URLLocked is used to block possible other events during the callback activation
	  if ( m_NewURLSelectedState == URL_ACTIVATED && !m_URLLocked )
	    {
	      m_URLLocked = TRUE;
	      APP_Callback_URLActivate ( p_frame->GetFrameId(),
					 event.GetString().mb_str(AmayaWindow::conv_ascii) );
	      m_URLLocked = FALSE;
	      
	      // this url is activated, setup the machine state to init
	      m_NewURLSelectedState = URL_NOT_SELECTED;
	    }
	  else
	    {
	      // if this is not a new url activation
	      // it's a simple url edition
	      // juste need to update the internal url string for the current frame
	      p_frame->SetFrameURL( event.GetString() );
	    }
	}
      else
	{
	  // this is a empty event which follow a URL_SELECTED event
	  // this sequence of event gives us the possibility to know if the url is really activated or not
	  // this is dirty but it works.
	  if ( m_NewURLSelectedState == URL_SELECTED )
	    m_NewURLSelectedState = URL_ACTIVATED;
	}
    }

  event.Skip();
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaURLBar
 *      Method:  OnURLText
 * Description:  the user has typed ENTER with his keyboard =>
 *               simply activate the callback
 *--------------------------------------------------------------------------------------
 */
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

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaURLBar
 *      Method:  OnSize
 * Description:  the window is resized, we must recalculate by hand the new urlbar size
 *               (wxWidgets is not able to do that itself ...)
 *--------------------------------------------------------------------------------------
 */
void AmayaURLBar::OnSize( wxSizeEvent& event )
{
  wxLogDebug( _T("AmayaURLBar::OnSize - ")+
	      wxString(_T(" w=%d h=%d")),
	      event.GetSize().GetWidth(),
	      event.GetSize().GetHeight() );

  event.Skip();
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaURLBar
 *      Method:  ReSize
 * Description:  the window is resized, we must recalculate by hand the new urlbar size
 *               (wxWidgets is not able to do that itself ...)
 *               the value received by ReSize has been calculated by parent window,
 *               it depends of other controls into the toolbar
 *--------------------------------------------------------------------------------------
 */
void AmayaURLBar::ReSize( int width, int height )
{
  wxLogDebug( _T("AmayaURLBar::ReSize - ")+
	      wxString(_T(" w=%d h=%d")),
	      width, height );

  int id = GetId();
  

  SetSize( 550, -1 );
  //  SetSizeHints( width, height, width, height);
  //SetVirtualSize( width, height );
}

/*----------------------------------------------------------------------
 *  this is where the event table is declared
 *  the callbacks are assigned to an event type
 *----------------------------------------------------------------------*/
BEGIN_EVENT_TABLE(AmayaURLBar, wxComboBox)
  EVT_COMBOBOX( -1,         AmayaURLBar::OnURLSelected )
  EVT_TEXT( -1,             AmayaURLBar::OnURLText )
  EVT_TEXT_ENTER( -1,       AmayaURLBar::OnURLTextEnter )
  EVT_SIZE( 		    AmayaURLBar::OnSize )
END_EVENT_TABLE()

#endif /* #ifdef _WX */
