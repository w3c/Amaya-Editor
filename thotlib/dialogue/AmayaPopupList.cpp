#ifdef _WX

#include "AmayaPopupList.h"



IMPLEMENT_DYNAMIC_CLASS(AmayaPopupL, wxListBox)

/*----------------------------------------------------------------------
 *  this is where the event table is declared
 *  the callbacks are assigned to an event type
 *----------------------------------------------------------------------*/
BEGIN_EVENT_TABLE(AmayaPopupL, wxListBox)
  EVT_KILL_FOCUS( AmayaPopupL::OnKillFocus )
  EVT_SET_FOCUS(  AmayaPopupL::OnSetFocus )
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
  //  EVT_KILL_FOCUS( AmayaPopupList::OnKillFocus )
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
{
  wxLogDebug( _T("AmayaPopupList::AmayaPopupList") );

  wxSizer * p_sizer = new wxBoxSizer(wxVERTICAL);
  SetSizer( p_sizer );
  m_pList = new AmayaPopupL( this );
  //  m_pList->Connect( -1, wxEVT_KILL_FOCUS, (wxObjectEventFunction)(wxEventFunction)(wxFocusEventFunction)&AmayaPopupList::OnKillFocus );
  p_sizer->Add( m_pList, 1, wxEXPAND );

  // m_pList->SetFocus();
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

  //  event.Skip();
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

#endif /* #ifdef _WX */
