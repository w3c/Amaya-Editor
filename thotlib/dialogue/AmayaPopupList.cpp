#ifdef _WX

#include "AmayaPopupList.h"



IMPLEMENT_DYNAMIC_CLASS(AmayaPopupL, wxListBox)

/*----------------------------------------------------------------------
 *  this is where the event table is declared
 *  the callbacks are assigned to an event type
 *----------------------------------------------------------------------*/
BEGIN_EVENT_TABLE(AmayaPopupL, wxListBox)
  //  EVT_KILL_FOCUS( AmayaPopupL::OnKillFocus )
  //  EVT_SET_FOCUS(  AmayaPopupL::OnSetFocus )
  //  EVT_LISTBOX( -1, AmayaPopupL::OnSelect )
END_EVENT_TABLE()

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaPopupL
 *      Method:  AmayaPopupL
 * Description:  
 *--------------------------------------------------------------------------------------
 */
AmayaPopupL::AmayaPopupL ( wxWindow * parent ) : wxListBox( parent, -1 )
{
  wxLogDebug( _T("AmayaPopupL::AmayaPopupL") );
}


/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaPopupList
 *      Method:  ~AmayaPopupList
 * Description:  
 *--------------------------------------------------------------------------------------
 */
AmayaPopupL::~AmayaPopupL()
{
  wxLogDebug( _T("AmayaPopupL::~AmayaPopupL") );
}


/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaPopupL
 *      Method:  OnKillFocus
 * Description:  when the focus is lost, the popup should be destroyed
 *--------------------------------------------------------------------------------------
 */
void AmayaPopupL::OnKillFocus( wxFocusEvent & event )
{
  wxLogDebug( _T("AmayaPopupL::OnKillFocus") );
  //  Destroy();
  event.Skip();
}


/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaPopupL
 *      Method:  OnSelect
 * Description:  
 *--------------------------------------------------------------------------------------
 */
void AmayaPopupL::OnSelect( wxCommandEvent& event )
{
  wxLogDebug( _T("AmayaPopupL::OnSelect") );
  //  Destroy();
  event.Skip();
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaPopupL
 *      Method:  OnSetFocus
 * Description:  when the focus is lost, the popup should be destroyed
 *--------------------------------------------------------------------------------------
 */
void AmayaPopupL::OnSetFocus( wxFocusEvent & event )
{
  wxLogDebug( _T("AmayaPopupL::OnSetFocus") );
  event.Skip();
}

IMPLEMENT_DYNAMIC_CLASS(AmayaPopupList, wxPanel)

/*----------------------------------------------------------------------
 *  this is where the event table is declared
 *  the callbacks are assigned to an event type
 *----------------------------------------------------------------------*/
BEGIN_EVENT_TABLE(AmayaPopupList, wxPanel)
  EVT_SET_FOCUS(   AmayaPopupList::OnSetFocus )
  EVT_KILL_FOCUS(  AmayaPopupList::OnKillFocus )
  EVT_LISTBOX( -1, AmayaPopupList::OnSelect )
  EVT_IDLE(        AmayaPopupList::OnIdle ) // Process a wxEVT_IDLE event  
END_EVENT_TABLE()

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaPopupList
 *      Method:  AmayaPopupList
 * Description:  
 *--------------------------------------------------------------------------------------
 */
AmayaPopupList::AmayaPopupList ( wxWindow * parent, wxPoint pos ) :
  wxPanel( parent, -1, pos )
  ,m_ShouldBeDestroyed(false)
  ,m_HasBeenSelected(false)
{
  wxLogDebug( _T("AmayaPopupList::AmayaPopupList") );

  wxSizer * p_sizer = new wxBoxSizer(wxVERTICAL);
  SetSizer( p_sizer );
  m_pList = new AmayaPopupL( this );
  //  m_pList->Connect( -1, wxEVT_KILL_FOCUS, (wxObjectEventFunction)(wxEventFunction)(wxFocusEventFunction)&AmayaPopupList::OnKillFocus );
  //  m_pList->Connect( -1, wxEVT_SET_FOCUS, (wxObjectEventFunction)(wxEventFunction)(wxFocusEventFunction)&AmayaPopupList::OnSetFocus );
  p_sizer->Add( m_pList, 1, wxEXPAND );

  m_pList->SetFocus();
  m_pList->SetSelection(0);
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaPopupList
 *      Method:  ~AmayaPopupList
 * Description:  
 *--------------------------------------------------------------------------------------
 */
AmayaPopupList::~AmayaPopupList()
{
  wxLogDebug( _T("AmayaPopupList::~AmayaPopupList") );
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaPopupList
 *      Method:  OnKillFocus
 * Description:  when the focus is lost, the popup should be destroyed
 *--------------------------------------------------------------------------------------
 */
void AmayaPopupList::OnKillFocus( wxFocusEvent & event )
{
  wxLogDebug( _T("AmayaPopupList::OnKillFocus") );

  if (m_HasBeenSelected == true)
    m_ShouldBeDestroyed = true;

  event.Skip();
}


/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaPopupList
 *      Method:  OnSetFocus
 * Description:  when the focus is lost, the popup should be destroyed
 *--------------------------------------------------------------------------------------
 */
void AmayaPopupList::OnSetFocus( wxFocusEvent & event )
{
  wxLogDebug( _T("AmayaPopupList::OnSetFocus") );
  event.Skip();
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaPopupList
 *      Method:  Append
 * Description:  just add a string to the list
 *--------------------------------------------------------------------------------------
 */
void AmayaPopupList::Append( const wxString & item )
{
  m_pList->Append( item );
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaPopupList
 *      Method:  SetSize
 * Description:  overload the generic wxPanel::SetSize in order to adapte the panel to the list size
 *--------------------------------------------------------------------------------------
 */
void AmayaPopupList::SetSize( int w, int h )
{
  wxLogDebug( _T("AmayaPopupList::SetSize") );
  m_pList->SetSize( w, h );
  wxPanel::SetSize( m_pList->GetSize() );
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaPopupList
 *      Method:  OnSelect
 * Description:  
 *--------------------------------------------------------------------------------------
 */
void AmayaPopupList::OnSelect( wxCommandEvent& event )
{
  wxLogDebug( _T("AmayaPopupList::OnSelect") );
  m_HasBeenSelected = true;
  GetParent()->SetFocus();
  Hide();
  /*  Destroy();*/
  //event.Skip();
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaPopupList
 *      Method:  OnIdle
 * Description:  called when there is no more event to procced
 *--------------------------------------------------------------------------------------
 */
void AmayaPopupList::OnIdle( wxIdleEvent& event )
{
  if (m_ShouldBeDestroyed)
    {
      wxLogDebug( _T("AmayaPopupList::OnIdle -> Destroy") );
      Destroy();
    }
  else
    wxLogDebug( _T("AmayaPopupList::OnIdle -> !Destroy") );
  event.Skip();
}

#endif /* #ifdef _WX */
