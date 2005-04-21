#ifdef _WX

#include "wx/wx.h"

#include "AmayaQuickSplitButton.h"

IMPLEMENT_DYNAMIC_CLASS(AmayaQuickSplitButton, wxButton)

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaQuickSplitButton
 *      Method:  AmayaQuickSplitButton
 *--------------------------------------------------------------------------------------
 */
AmayaQuickSplitButton::AmayaQuickSplitButton( wxWindow * p_parent_window, wxAmayaQuickSplitMode mode, int width )
  :  wxPanel( p_parent_window/*, -1, _T(""), wxDefaultPosition*/ )
     ,m_Width(width)
     ,m_Mode(mode)
{
  if (m_Mode == wxAMAYA_QS_HORIZONTAL )
    SetSize(wxSize(-1, m_Width));
  else if (m_Mode == wxAMAYA_QS_VERTICAL )
    SetSize(wxSize(m_Width, -1));

  SetBackgroundColour(wxColour(255,160,160));

  SetAutoLayout(TRUE);
}


/*
 *--------------------------------------------------------------------------------------
 *       Class: AmayaQuickSplitButton 
 *      Method: ~AmayaQuickSplitButton 
 * Description: destructor of AmayaQuickSplitButton (do nothing)
 *--------------------------------------------------------------------------------------
 */
AmayaQuickSplitButton::~AmayaQuickSplitButton()
{
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaQuickSplitButton
 *      Method:  OnLeaveWindow
 * Description:  change the color when mouse enter the button
 *--------------------------------------------------------------------------------------
 */
void AmayaQuickSplitButton::OnEnterWindow( wxMouseEvent& event )
{
  m_OldColour = GetBackgroundColour();
  SetBackgroundColour(wxColour(255,50,50));
  Refresh();

  event.Skip();
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaQuickSplitButton
 *      Method:  OnLeaveWindow
 * Description:  change the color when mouse leave the button
 *--------------------------------------------------------------------------------------
 */
void AmayaQuickSplitButton::OnLeaveWindow( wxMouseEvent& event )
{
  SetBackgroundColour(m_OldColour);
  Refresh();

  event.Skip();
}


/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaQuickSplitButton
 *      Method:  OnActivate
 * Description:  simulate a button activate event
 *--------------------------------------------------------------------------------------
 */
void AmayaQuickSplitButton::OnActivate( wxMouseEvent& event )
{
  // now just simulate a wxButton event : this event will be catch by its parent.
  wxCommandEvent cmd_event( wxEVT_COMMAND_BUTTON_CLICKED, GetId() );
  // Do not use  wxPostEvent( this, cmd_event ); because on unix there is a layout bug ... but on windows it works fine
  ProcessEvent(cmd_event);

  event.Skip();
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaQuickSplitButton
 *      Method:  ShowQuickSplitButton
 * Description:  this function will show/hide the quick splitbar button
 *--------------------------------------------------------------------------------------
 */
void AmayaQuickSplitButton::ShowQuickSplitButton( bool show )
{
  wxSize new_size;
  if (m_Mode == wxAMAYA_QS_HORIZONTAL )
    if (show)
      new_size = wxSize( GetSize().GetWidth(), m_Width );
    else
      new_size = wxSize( GetSize().GetWidth(), 0 );
  else if (m_Mode == wxAMAYA_QS_VERTICAL )
    if (show)
      new_size = wxSize( m_Width, GetSize().GetHeight() );
    else
      new_size = wxSize( 0, GetSize().GetHeight() );

  // re layout the top sizer and assign the new size
  wxSizer * p_SizerTop = GetContainingSizer();
  p_SizerTop->SetItemMinSize( this, new_size );
  p_SizerTop->Layout();
}


/*----------------------------------------------------------------------
 *  this is where the event table is declared
 *  the callbacks are assigned to an event type
 *----------------------------------------------------------------------*/
BEGIN_EVENT_TABLE(AmayaQuickSplitButton, wxPanel)
  EVT_ENTER_WINDOW(AmayaQuickSplitButton::OnEnterWindow)
  EVT_LEAVE_WINDOW(AmayaQuickSplitButton::OnLeaveWindow)
  EVT_LEFT_DCLICK(AmayaQuickSplitButton::OnActivate)
END_EVENT_TABLE()

#endif /* #ifdef _WX */ 
