#ifdef _WX

#include "wx/wx.h"
#include "wx/xrc/xmlres.h"              // XRC XML resouces
#include "wx/string.h"

#include "AmayaApp.h"
#include "CSSDlgWX.h"

#define THOT_EXPORT extern
#include "amaya.h"
#include "appdialogue_wx.h"
#include "message_wx.h"

static int      MyRef;


//-----------------------------------------------------------------------------
// Event table: connect the events to the handler functions to process them
//-----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(CSSDlgWX, AmayaDialog)
  EVT_BUTTON(     XRCID("wxID_OK"),       CSSDlgWX::OnOkButton )
  EVT_BUTTON(     XRCID("wxID_CANCEL"),   CSSDlgWX::OnCancelButton )
  EVT_LISTBOX_DCLICK( XRCID("wxID_LIST"), CSSDlgWX::OnOkButton)
END_EVENT_TABLE()

/*----------------------------------------------------------------------
  CSSDlgWX create the dialog used to open/disable/enabel.. a CSS file
  params:
    + parent : parent window
    + title : dialog title
  ----------------------------------------------------------------------*/
CSSDlgWX::CSSDlgWX( int ref,
		    wxWindow* parent,
		    const wxString & title,
		    const wxArrayString& items ) :
  AmayaDialog( parent, ref )
{
  wxXmlResource::Get()->LoadDialog(this, parent, wxT("CSSDlgWX"));
  MyRef = ref;

  // update dialog labels with given ones
  SetTitle( title );
  XRCCTRL(*this, "wxID_LABEL", wxStaticText)->SetLabel( TtaConvMessageToWX(TtaGetMessage(AMAYA, AM_CSS_FILE)) );
  XRCCTRL(*this, "wxID_OK", wxButton)->SetLabel( TtaConvMessageToWX(TtaGetMessage(LIB, TMSG_LIB_CONFIRM)) );
  XRCCTRL(*this, "wxID_CANCEL", wxButton)->SetLabel( TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_CANCEL)) );
  XRCCTRL(*this, "wxID_LIST", wxListBox)->Append( items );

  // pre-select the first item
  XRCCTRL(*this, "wxID_LIST", wxListBox)->SetSelection(0);

  SetAutoLayout( TRUE );
}

/*----------------------------------------------------------------------
  Destructor. (Empty, as I don't need anything special done when destructing).
  ----------------------------------------------------------------------*/
CSSDlgWX::~CSSDlgWX()
{
  ThotCallback (MyRef, INTEGER_DATA, (char*) 0);
}

/*----------------------------------------------------------------------
  OnOkButton called when the user validate his selection
  params:
  returns:
  ----------------------------------------------------------------------*/
void CSSDlgWX::OnOkButton( wxCommandEvent& event )
{
  wxString selected_item = XRCCTRL(*this, "wxID_LIST", wxListBox)->GetStringSelection();
  if ( !selected_item.IsEmpty() )
    {  
      wxLogDebug( _T("CSSDlgWX::OnOkButton - selected=") + selected_item );

      // allocate a temporary buffer
      char buffer[512];
      wxASSERT( selected_item.Len() < 512 );
      strcpy( buffer, (const char*)selected_item.mb_str(wxConvUTF8) );

      ThotCallback (BaseCSS + CSSSelect, STRING_DATA, buffer);
    }

  ThotCallback (MyRef, INTEGER_DATA, (char*) 1);
}

/*----------------------------------------------------------------------
  OnCancelButton called when the user wants to exit from this dialog
  params:
  returns:
  ----------------------------------------------------------------------*/
void CSSDlgWX::OnCancelButton( wxCommandEvent& event )
{
  wxLogDebug( _T("CSSDlgWX::OnCancelButton") );
  ThotCallback (MyRef, INTEGER_DATA, (char*) 0);
}

#endif /* _WX */


