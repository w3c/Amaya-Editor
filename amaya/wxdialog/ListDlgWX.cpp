/*
 *
 *  (c) COPYRIGHT INRIA and W3C, 1996-2008
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

#ifdef _WX
#include "wx/wx.h"
#include "wx/xrc/xmlres.h"              // XRC XML resouces
#include "wx/string.h"

#include "AmayaApp.h"
#include "ListDlgWX.h"

#define THOT_EXPORT extern
#include "amaya.h"
#include "init_f.h"
#include "appdialogue_wx.h"
#include "message_wx.h"

static int MyRef = 0;


//
//
// ListDlgWX
//
//


//-----------------------------------------------------------------------------
// Event table: connect the events to the handler functions to process them
//-----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(ListDlgWX, AmayaDialog)
  EVT_BUTTON(     XRCID("wxID_OK"),       ListDlgWX::OnOkButton )
  EVT_BUTTON(     XRCID("wxID_CANCEL"),   ListDlgWX::OnCancelButton )
  EVT_LISTBOX_DCLICK( XRCID("wxID_LIST"), ListDlgWX::OnOkButton)
END_EVENT_TABLE()

/*----------------------------------------------------------------------
  ListDlgWX create the dialog used to open/disable/enabel.. a CSS file
  params:
    + parent : parent window
    + title : dialog title
  ----------------------------------------------------------------------*/
ListDlgWX::ListDlgWX( int ref, int subref, wxWindow* parent,
                      const wxString & title, const wxArrayString& items ) :
  AmayaDialog( parent, ref ),
  m_SubRef(subref)
{
  wxXmlResource::Get()->LoadDialog(this, parent, wxT("ListDlgWX"));

  // update dialog labels with given ones
  MyRef = ref;
  SetTitle( title );
  XRCCTRL(*this, "wxID_LABEL", wxStaticText)->SetLabel( title );
  XRCCTRL(*this, "wxID_OK", wxButton)->SetLabel( TtaConvMessageToWX(TtaGetMessage(LIB, TMSG_LIB_CONFIRM)) );
  XRCCTRL(*this, "wxID_CANCEL", wxButton)->SetLabel( TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_CANCEL)) );
  XRCCTRL(*this, "wxID_LIST", wxListBox)->Append( items );

  // pre-select the first item
  XRCCTRL(*this, "wxID_LIST", wxListBox)->SetSelection(0);
  Fit();
  SetAutoLayout( TRUE );
}

/*----------------------------------------------------------------------
  Destructor. (Empty, as I don't need anything special done when destructing).
  ----------------------------------------------------------------------*/
ListDlgWX::~ListDlgWX()
{
  ThotCallback (MyRef, INTEGER_DATA, (char*) 0);
}

/*----------------------------------------------------------------------
  OnOkButton called when the user validate his selection
  params:
  returns:
  ----------------------------------------------------------------------*/
void ListDlgWX::OnOkButton( wxCommandEvent& event )
{
  wxString selected_item = XRCCTRL(*this, "wxID_LIST", wxListBox)->GetStringSelection();
  if ( !selected_item.IsEmpty() )
    {  
      // allocate a temporary buffer
      char buffer[512];
      wxASSERT( selected_item.Len() < 512 );
      strcpy( buffer, (const char*)selected_item.mb_str(wxConvUTF8) );
      ThotCallback (m_SubRef, STRING_DATA, buffer);
      // entry
      int i = XRCCTRL(*this, "wxID_LIST", wxListBox)->GetSelection();
      ThotCallback (m_SubRef+1, INTEGER_DATA, (char*) i);
    }
  LoadDefaultOpeningLocation (FALSE);
  ThotCallback (MyRef, INTEGER_DATA, (char*) 1);
  TtaRedirectFocus();
}

/*----------------------------------------------------------------------
  OnCancelButton called when the user wants to exit from this dialog
  params:
  returns:
  ----------------------------------------------------------------------*/
void ListDlgWX::OnCancelButton( wxCommandEvent& event )
{
  ThotCallback (MyRef, INTEGER_DATA, (char*) 0);
  TtaRedirectFocus();
}

//
//
// NonSelListDlgWX
//
//

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
NonSelListDlgWX::NonSelListDlgWX(wxWindow* parent, const wxString & title,
                const wxString & label,
                const wxArrayString& items, const wxString& button ) :
  wxDialog()
{
  wxXmlResource::Get()->LoadDialog(this, parent, wxT("ListDlgWX"));

  // update dialog labels with given ones
  SetTitle( title );
  XRCCTRL(*this, "wxID_LABEL", wxStaticText)->SetLabel( label );
  XRCCTRL(*this, "wxID_OK", wxButton)->SetLabel( button );
  XRCCTRL(*this, "wxID_LIST", wxListBox)->Append( items );
  XRCCTRL(*this, "wxID_CANCEL", wxButton)->Hide();
  
  Fit();
  SetAutoLayout( TRUE );
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
NonSelListDlgWX::~NonSelListDlgWX()
{
  ThotCallback (MyRef, INTEGER_DATA, (char*) 0);
  TtaRedirectFocus();
}


#endif /* _WX */



