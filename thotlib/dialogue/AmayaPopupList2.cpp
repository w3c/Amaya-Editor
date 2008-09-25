/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2008
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

#ifdef _WX

#include "wx/wx.h"
#include "wx/popupwin.h"

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
#include "frame.h"
#include "view.h"
#include "logdebug.h"

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "attrmenu_f.h"
#include "frame_tv.h"
#include "views_f.h"

#include "message_wx.h"
#include "paneltypes_wx.h"
#include "appdialogue_wx.h"
#include "appdialogue_wx_f.h"


#include "AmayaPopupList.h"

IMPLEMENT_DYNAMIC_CLASS(AmayaPopupList, wxPopupTransientWindow)

/*----------------------------------------------------------------------
 *  this is where the event table is declared
 *  the callbacks are assigned to an event type
 *----------------------------------------------------------------------*/
BEGIN_EVENT_TABLE(AmayaPopupList, wxPopupTransientWindow)
  EVT_LISTBOX(-1, AmayaPopupList::OnActivateItem)
END_EVENT_TABLE()

/*----------------------------------------------------------------------
 *       Class:  AmayaPopupList
 *      Method:  AmayaPopupList
 * Description:  
  -----------------------------------------------------------------------*/
AmayaPopupList::AmayaPopupList ( wxWindow * p_parent, int ref ) :
  wxPopupTransientWindow( p_parent )
 ,m_Ref(ref)
{
  m_pListBox = new wxListBox(this, -1, wxDefaultPosition, wxDefaultSize, 0, NULL, wxLB_SINGLE|wxLB_NEEDED_SB );
  
  wxBoxSizer *topSizer = new wxBoxSizer( wxVERTICAL );
  topSizer->Add( m_pListBox, 1, wxALL | wxEXPAND, 0 );
  
  SetAutoLayout( true );
  SetSizer( topSizer );
  topSizer->Fit(this);
}

/*----------------------------------------------------------------------
 *       Class:  AmayaPopupList
 *      Method:  ~AmayaPopupList
 * Description:  
  -----------------------------------------------------------------------*/
AmayaPopupList::~AmayaPopupList()
{
}

/*----------------------------------------------------------------------
 *       Class:  AmayaPopupList
 *      Method:  OnMenuItem
 * Description:  
  -----------------------------------------------------------------------*/
void AmayaPopupList::OnActivateItem( wxCommandEvent& event )
{
  int id = (int)event.GetClientData();
  TTALOGDEBUG_1( TTA_LOG_DIALOG, _T("AmayaPopupList::OnActivateItem - id=%d"), id);

  ThotCallback (m_Ref, INTEGER_DATA, (char*)id);

  // finaly destroy the popup
  DismissAndNotify();
}

/*----------------------------------------------------------------------
 *       Class:  AmayaPopupList
 *      Method:  Append
 * Description:  append an item to the list
  -----------------------------------------------------------------------*/
void AmayaPopupList::Append( int i, const wxString & label )
{
  TTALOGDEBUG_1( TTA_LOG_DIALOG, _T("AmayaPopupList::Append - id=%d label=")+label, i );
  m_pListBox->Append(label, (void *)i);
}

/*----------------------------------------------------------------------
 *       Class:  AmayaPopupList
 *      Method:  OnDismiss
 * Description:  called when the popup is closed
  -----------------------------------------------------------------------*/
void AmayaPopupList::OnDismiss()
{
  TtaDestroyDialogue( m_Ref );
}

/*----------------------------------------------------------------------
 *       Class:  AmayaPopupList
 *      Method:  Popup
 * Description:  resize the list before poping it up
  -----------------------------------------------------------------------*/
void AmayaPopupList::Popup(wxWindow *focus)
{
  /*
  wxSize sz = m_pListBox->GetBestSize();
  wxPoint pos = wxGetMousePosition();
  Position( pos, sz );
  */
  m_pListBox->SetFocus();
  wxPopupTransientWindow::Popup(m_pListBox);
}

#endif /* #ifdef _WX */
