#ifdef _WX

#include "wx/wx.h"
#include "wx/xrc/xmlres.h"              // XRC XML resouces

#include "AmayaApp.h"
#include "TitleDlgWX.h"

#define THOT_EXPORT extern
#include "amaya.h"
#include "wxinclude.h"

//-----------------------------------------------------------------------------
// Event table: connect the events to the handler functions to process them
//-----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(TitleDlgWX, wxDialog)
  EVT_BUTTON(     XRCID("wxID_CONFIRMBUTTON"), TitleDlgWX::OnConfirmButton )
  EVT_BUTTON(     XRCID("wxID_CANCELBUTTON"),  TitleDlgWX::OnCancelButton )
  EVT_TEXT_ENTER( XRCID("wxID_TITLE"),         TitleDlgWX::OnConfirmButton )
END_EVENT_TABLE()

/*----------------------------------------------------------------------
  TitleDlgWX create the dialog to change the title
  params:
    + parent : parent window
    + title : dialog title
    + doc_title : the current document title
  returns:
  ----------------------------------------------------------------------*/
TitleDlgWX::TitleDlgWX( wxWindow* parent,
			const wxString & title,
			const wxString & doc_title ) : wxDialog()
{
  wxXmlResource::Get()->LoadDialog(this, parent, wxT("TitleDlgWX"));

  wxLogDebug( _T("TitleDlgWX::TitleDlgWX - title=")+title);
  wxLogDebug( _T("TitleDlgWX::TitleDlgWX - doc_title=")+doc_title);
 
  SetTitle( title );

  // update dialog labels
  XRCCTRL(*this, "wxID_CANCELBUTTON", wxButton)->SetLabel(wxString(TtaGetMessage (LIB, TMSG_CANCEL), AmayaApp::conv_ascii));
  XRCCTRL(*this, "wxID_CONFIRMBUTTON", wxButton)->SetLabel(wxString(TtaGetMessage (LIB, TMSG_LIB_CONFIRM), AmayaApp::conv_ascii));
  XRCCTRL(*this, "wxID_TITLE", wxTextCtrl)->SetValue(doc_title);

  Layout();
  
  SetAutoLayout( TRUE );
}

/*----------------------------------------------------------------------
  Destructor. (Empty, as I don't need anything special done when destructing).
  ----------------------------------------------------------------------*/
TitleDlgWX::~TitleDlgWX()
{
}

/*----------------------------------------------------------------------
  OnConfirmButton called when clicking on confirmbutton
  ----------------------------------------------------------------------*/
void TitleDlgWX::OnConfirmButton( wxCommandEvent& event )
{
  wxString new_title = XRCCTRL(*this, "wxID_TITLE", wxTextCtrl)->GetValue( );
  wxLogDebug( _T("TitleDlgWX::OnConfirmButton - new_title=")+new_title );
  
  // allocate a temporary buffer to copy the 'const char *' new_title buffer 
  char buffer[512];
  wxASSERT( new_title.Len() < 512 );
  strcpy( buffer, new_title.ToAscii() );

  // give the new title to Amaya
  ThotCallback (BaseDialog + TitleText,  STRING_DATA, (char *)buffer );
  // set the new title
  ThotCallback (BaseDialog + TitleForm, INTEGER_DATA, (char*) 1);

  EndModal( 0 );
}

/*----------------------------------------------------------------------
  OnCancelButton called when clicking on cancelbutton
  ----------------------------------------------------------------------*/
void TitleDlgWX::OnCancelButton( wxCommandEvent& event )
{
  ThotCallback (BaseDialog + TitleForm, INTEGER_DATA, (char*) 0); 
  EndModal( 0 );
}

#endif /* _WX */
