#ifdef _WX

#include "wx/wx.h"
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

#include "appdialogue_wx_f.h"

#include "AmayaURLBar.h"
#include "AmayaWindow.h"
#include "AmayaFrame.h"

#include "wx/combobox.h"
#include "wx/bmpbuttn.h"

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
  
  // create the sizer to contains these widgets (combo)
  // it's possible to add others widgets here - exemple : validate button
  wxBoxSizer * p_sizer = new wxBoxSizer ( wxHORIZONTAL );
  p_sizer->Add( m_pComboBox, 1, wxEXPAND );
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

  AmayaFrame * p_frame = m_pAmayaWindowParent->GetActiveFrame();
  if (p_frame)
    {
      APP_Callback_URLActivate ( p_frame->GetFrameId(),
				 GetValue().mb_str(*wxConvCurrent) );
    }

  // do not skip this event because we don't want to propagate this event
  //  event.Skip();
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
  m_pComboBox->SetValue( newurl );
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


/*----------------------------------------------------------------------
 *  this is where the event table is declared
 *  the callbacks are assigned to an event type
 *----------------------------------------------------------------------*/
BEGIN_EVENT_TABLE(AmayaURLBar, wxPanel)
  EVT_TEXT_ENTER( -1,       AmayaURLBar::OnURLTextEnter )
  EVT_BUTTON( -1,           AmayaURLBar::OnURLTextEnter )
END_EVENT_TABLE()

#endif /* #ifdef _WX */
