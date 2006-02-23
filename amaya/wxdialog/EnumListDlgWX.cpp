#ifdef _WX

#include "wx/wx.h"
#include "wx/xrc/xmlres.h"              // XRC XML resouces
#include "wx/string.h"

#include "AmayaApp.h"
#include "EnumListDlgWX.h"

#define THOT_EXPORT extern
#include "amaya.h"
#include "appdialogue_wx.h"
#include "message_wx.h"
static int MyRef = 0;


//-----------------------------------------------------------------------------
// Event table: connect the events to the handler functions to process them
//-----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(EnumListDlgWX, AmayaDialog)
  EVT_BUTTON(     XRCID("wxID_OK"),       EnumListDlgWX::OnOkButton )
  EVT_BUTTON(     XRCID("wxID_CANCEL"),   EnumListDlgWX::OnCancelButton )
  //  EVT_LISTBOX_DCLICK( XRCID("wxID_LIST"), EnumListDlgWX::OnOkButton)
END_EVENT_TABLE()

/*----------------------------------------------------------------------
  EnumListDlgWX create the dialog used to open/disable/enabel.. a CSS file
  params:
    + parent : parent window
    + title : dialog title
  ----------------------------------------------------------------------*/
EnumListDlgWX::EnumListDlgWX( int ref, int subref, wxWindow* parent,
                              const wxString & title, const wxString & label,
                              const wxArrayString& items, int selection ) :
  AmayaDialog( parent, ref )
  ,m_SubRef(subref)
{
  wxXmlResource::Get()->LoadDialog(this, parent, wxT("EnumListDlgWX"));
  MyRef = ref;

  // update dialog labels with given ones
  SetTitle( title );
  XRCCTRL(*this, "wxID_OK", wxButton)->SetLabel( TtaConvMessageToWX(TtaGetMessage(LIB, TMSG_LIB_CONFIRM)) );
  XRCCTRL(*this, "wxID_CANCEL", wxButton)->SetLabel( TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_CANCEL)) );

  // setup the radiobox
  wxPanel * p_panel = XRCCTRL(*this, "wxID_PANEL_LIST", wxPanel);
  wxSizer * p_sizer = p_panel->GetSizer();
  m_pRadiobox = new wxRadioBox( p_panel, -1, label,
				wxDefaultPosition, wxDefaultSize,
				items );
  m_pRadiobox->SetSelection( selection );
  p_sizer->Prepend(m_pRadiobox, 1, wxALL|wxEXPAND, 0);
  Fit(); /* tell the top window to resize */

  SetAutoLayout( TRUE );
}

/*----------------------------------------------------------------------
  Destructor. (Empty, as I don't need anything special done when destructing).
  ----------------------------------------------------------------------*/
EnumListDlgWX::~EnumListDlgWX()
{
  ThotCallback (MyRef, INTEGER_DATA, (char*) 0);
}

/*----------------------------------------------------------------------
  OnOkButton called when the user validate his selection
  params:
  returns:
  ----------------------------------------------------------------------*/
void EnumListDlgWX::OnOkButton( wxCommandEvent& event )
{
  int selected_item = m_pRadiobox->GetSelection();
  ThotCallback (m_SubRef, INTEGER_DATA, (char*)selected_item);
  ThotCallback (MyRef, INTEGER_DATA, (char*)1);
}

/*----------------------------------------------------------------------
  OnCancelButton called when the user wants to exit from this dialog
  params:
  returns:
  ----------------------------------------------------------------------*/
void EnumListDlgWX::OnCancelButton( wxCommandEvent& event )
{
  Close();
  //  ThotCallback (MyRef, INTEGER_DATA, (char*) 0);
}

#endif /* _WX */



