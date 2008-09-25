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
#include "ListEditDlgWX.h"

#define THOT_EXPORT extern
#include "amaya.h"
#include "appdialogue_wx.h"
#include "message_wx.h"
static int MyRef = 0;

//-----------------------------------------------------------------------------
// Event table: connect the events to the handler functions to process them
//-----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(ListEditDlgWX, AmayaDialog)
  EVT_BUTTON(         XRCID("wxID_OK"),       ListEditDlgWX::OnOkButton )
  EVT_BUTTON(         XRCID("wxID_CANCEL"),   ListEditDlgWX::OnCancelButton )
  EVT_LISTBOX_DCLICK( XRCID("wxID_LIST"),     ListEditDlgWX::OnOkButton )
  EVT_TEXT_ENTER(     XRCID("wxID_TEXT"),     ListEditDlgWX::OnOkButton )
  EVT_LISTBOX(        XRCID("wxID_LIST"),     ListEditDlgWX::OnSelected ) 
END_EVENT_TABLE()

/*----------------------------------------------------------------------
  ListEditDlgWX 
  params:
    + parent : parent window
    + title : dialog title
  ----------------------------------------------------------------------*/
ListEditDlgWX::ListEditDlgWX( int ref, wxWindow* parent, const wxString & title,
                              const wxString & list_title, const wxArrayString& items,
                              const wxString & selected_item ) :
  AmayaDialog( parent, ref )
{
  wxXmlResource::Get()->LoadDialog(this, parent, wxT("ListEditDlgWX"));
  MyRef = ref;

  // update dialog labels with given ones
  SetTitle( title );
  XRCCTRL(*this, "wxID_LABEL", wxStaticText)->SetLabel( list_title );
  XRCCTRL(*this, "wxID_OK", wxButton)->SetLabel( TtaConvMessageToWX(TtaGetMessage(LIB, TMSG_LIB_CONFIRM)) );
  XRCCTRL(*this, "wxID_CANCEL", wxButton)->SetLabel( TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_CANCEL)) );
  XRCCTRL(*this, "wxID_LIST", wxListBox)->Append( items );
  // pre-select item
  XRCCTRL(*this, "wxID_LIST", wxListBox)->SetStringSelection(selected_item);
  XRCCTRL(*this, "wxID_TEXT", wxTextCtrl)->SetValue(selected_item);
  XRCCTRL(*this, "wxID_TEXT", wxTextCtrl)->SetSelection(0,-1);
  Fit();
  SetAutoLayout( TRUE );
}

/*----------------------------------------------------------------------
  Destructor. (Empty, as I don't need anything special done when destructing).
  ----------------------------------------------------------------------*/
ListEditDlgWX::~ListEditDlgWX()
{
  ThotCallback (MyRef, INTEGER_DATA, (char*) 0);
}

/*----------------------------------------------------------------------
  OnOkButton called when the user validate his selection
  params:
  returns:
  ----------------------------------------------------------------------*/
void ListEditDlgWX::OnOkButton( wxCommandEvent& event )
{
  wxString selected_item = XRCCTRL(*this, "wxID_TEXT", wxTextCtrl)->GetValue();
  if ( !selected_item.IsEmpty() )
    {  
      // allocate a temporary buffer
      char buffer[512];
      wxASSERT( selected_item.Len() < 512 );
      strcpy( buffer, (const char*)selected_item.mb_str(wxConvUTF8) );
      ThotCallback (MyRef + 1, STRING_DATA, buffer);
    }
  ThotCallback (MyRef, INTEGER_DATA, (char*) 1);
}

/*----------------------------------------------------------------------
  OnCancelButton called when the user wants to exit from this dialog
  params:
  returns:
  ----------------------------------------------------------------------*/
void ListEditDlgWX::OnCancelButton( wxCommandEvent& event )
{
  ThotCallback (MyRef, INTEGER_DATA, (char*) 0);
}

/*----------------------------------------------------------------------
  OnSelected
  params:
  returns:
  ----------------------------------------------------------------------*/
void ListEditDlgWX::OnSelected( wxCommandEvent& event )
{
  wxString s_selected = XRCCTRL(*this, "wxID_LIST", wxListBox)->GetStringSelection();
  XRCCTRL(*this, "wxID_TEXT", wxTextCtrl)->SetValue( s_selected );
  XRCCTRL(*this, "wxID_TEXT", wxTextCtrl)->SetInsertionPointEnd();
}
#endif /* _WX */
