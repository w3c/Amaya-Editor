#ifdef _WX

#include "wx/wx.h"
#include "wx/combobox.h"
#include "wx/bmpbuttn.h"
#include "wx/string.h"

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

#include "AmayaParams.h"
#include "appdialogue_wx_f.h"

#include "AmayaURLBar.h"
#include "AmayaWindow.h"
#include "AmayaFrame.h"


IMPLEMENT_DYNAMIC_CLASS(AmayaURLBar, wxPanel)

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaURLBar
 *      Method:  AmayaURLBar
 * Description:  create a new urlbar
 *--------------------------------------------------------------------------------------
 */
AmayaURLBar::AmayaURLBar ( wxWindow *     parent
			   ,AmayaWindow *  amaya_window_parent ) :
  wxPanel( parent )
{
  m_pAmayaWindowParent = amaya_window_parent;

  // create the combobox and the ok button
  m_pComboBox = new wxComboBox( this, -1, _T(""), wxDefaultPosition, wxDefaultSize );
  m_pComboBox->SetWindowStyleFlag(wxPROCESS_ENTER);
  //m_pComboBox->Connect( -1, wxEVT_CHAR, (wxObjectEventFunction)(wxEventFunction)(wxCharEventFunction)&AmayaURLBar::OnChar );
  //m_pComboBox->Connect( -1, wxEVT_KEY_DOWN, (wxObjectEventFunction)(wxEventFunction)(wxCharEventFunction)&AmayaURLBar::OnChar );
  //m_pComboBox->Connect( -1, wxEVT_COMMAND_TEXT_ENTER, (wxObjectEventFunction)(wxEventFunction)(wxCommandEventFunction)&AmayaURLBar::OnURLTextEnter );
  //m_pComboBox->Connect( -1, wxEVT_SET_FOCUS, (wxObjectEventFunction)(wxEventFunction)(wxFocusEventFunction)&AmayaPopupList::OnSetFocus );

  // create the sizer to contains these widgets (combo)
  // it's possible to add others widgets here - exemple : validate button
  wxBoxSizer * p_sizer = new wxBoxSizer ( wxHORIZONTAL );
  p_sizer->Add( m_pComboBox, 1, wxALIGN_CENTER );
  SetSizer(p_sizer);
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
 *      Method:  OnURLText
 * Description:  the user has typed ENTER with his keyboard or clicked on validate button =>
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

  GotoSelectedURL();
  
  // do not skip this event because we don't want to propagate this event
  //  event.Skip();
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaURLBar
 *      Method:  GotoSelectedURL
 * Description:  validate the selection
 *--------------------------------------------------------------------------------------
 */
void AmayaURLBar::GotoSelectedURL()
{
  AmayaFrame * p_frame = m_pAmayaWindowParent->GetActiveFrame();
  if (p_frame)
    {
      APP_Callback_URLActivate ( p_frame->GetFrameId(),
				 GetValue().mb_str(wxConvUTF8) );
    }
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaURLBar
 *      Method:  Clear
 * Description:  Removes all items from the control.
 *--------------------------------------------------------------------------------------
 */
void AmayaURLBar::Clear()
{
  m_pComboBox->Clear();
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaURLBar
 *      Method:  Append
 * Description:  Adds the item to the end of the combobox.
 *--------------------------------------------------------------------------------------
 */
void AmayaURLBar::Append( const wxString & newurl )
{
  m_pComboBox->Append( newurl );
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaURLBar
 *      Method:  SetValue
 * Description:  Sets the text for the combobox text field.
 *--------------------------------------------------------------------------------------
 */
void AmayaURLBar::SetValue( const wxString & newurl )
{
  if (m_pComboBox->FindString(newurl) == wxNOT_FOUND)
    m_pComboBox->Append(newurl);
  // new url should exists into combobox items so just select it.
  m_pComboBox->SetStringSelection( newurl );
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaURLBar
 *      Method:  GetValue
 * Description:  Returns the current value in the combobox text field.
 *--------------------------------------------------------------------------------------
 */
wxString AmayaURLBar::GetValue()
{
  return m_pComboBox->GetValue( );
}


/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaURLBar
 *      Method:  OnURLSelected
 * Description:  Called when the user select a new url
 *               there is a bug in wxWidgets on GTK version, this event is 
 *               called to often : each times user move the mouse with button pressed.
 *--------------------------------------------------------------------------------------
 */
void AmayaURLBar::OnURLSelected( wxCommandEvent& event )
{
  wxLogDebug( _T("AmayaURLBar::OnURLSelected") );

  // because of a bug on wxGTK with OnSelected event, the url activation is only avalaible for windows
#ifdef _WINDOWS
  GotoSelectedURL();
#endif /* _WINDOWS */
}

#if 0
void AmayaURLBar::OnChar( wxKeyEvent& event )
{
  wxLogDebug( _T("AmayaURLBar::OnChar : char=%x"),
	      event.GetKeyCode() );
  event.Skip();
}

void AmayaURLBar::OnURLText( wxCommandEvent& event )
{
  // event.GetString()
  wxString s = m_pComboBox->GetValue( );
  wxLogDebug( s );
  //  event.SetString(_T(""));
  m_pComboBox->SetValue( _T("") );
  event.Skip();
}
#endif /* 0 */

/*----------------------------------------------------------------------
 *  this is where the event table is declared
 *  the callbacks are assigned to an event type
 *----------------------------------------------------------------------*/
BEGIN_EVENT_TABLE(AmayaURLBar, wxPanel)
  EVT_COMBOBOX( -1,         AmayaURLBar::OnURLSelected )
  EVT_TEXT_ENTER( -1,       AmayaURLBar::OnURLTextEnter )
  //  EVT_BUTTON( -1,           AmayaURLBar::OnURLTextEnter )
  //  EVT_TEXT(-1,              AmayaURLBar::OnURLText )
  // EVT_CHAR(		    AmayaURLBar::OnChar) // Process a wxEVT_CHAR event. 
END_EVENT_TABLE()

#endif /* #ifdef _WX */
