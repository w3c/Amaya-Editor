#ifdef _WX

#include "wx/wx.h"
#include "wx/xrc/xmlres.h"              // XRC XML resouces

#include "AmayaApp.h"
#include "SearchDlgWX.h"

#define THOT_EXPORT extern
#include "amaya.h"
#include "wxinclude.h"

//-----------------------------------------------------------------------------
// Event table: connect the events to the handler functions to process them
//-----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(SearchDlgWX, wxDialog)
  EVT_BUTTON( XRCID("wxID_CONFIRMBUTTON"),   SearchDlgWX::OnConfirmButton )
  EVT_BUTTON( XRCID("wxID_CANCELBUTTON"),    SearchDlgWX::OnCancelButton )
  EVT_BUTTON( XRCID("wxID_NOREPLACEBUTTON"), SearchDlgWX::OnNoReplaceButton )
END_EVENT_TABLE()

/*----------------------------------------------------------------------
  SearchDlgWX create the Search dialog 
  params:
    + parent : parent window
    + titlecaption : dialog caption (including the document name)
  ----------------------------------------------------------------------*/
SearchDlgWX::SearchDlgWX( wxWindow* parent,
			  const wxString & caption ) : wxDialog()
{
  wxXmlResource::Get()->LoadDialog(this, parent, wxT("SearchDlgWX"));

  wxLogDebug( _T("SearchDlgWX::SearchDlgWX - caption=")+caption);
 
  SetTitle( caption );

  // update dialog labels
  XRCCTRL(*this, "wxID_SEARCH_FOR", wxStaticText)->SetLabel(wxString(TtaGetMessage (LIB, TMSG_SEARCH_FOR), AmayaApp::conv_ascii));
  XRCCTRL(*this, "wxID_CHECK_CASE", wxCheckBox)->SetLabel(wxString(TtaGetMessage (LIB, TMSG_UPPERCASE_EQ_LOWERCASE), AmayaApp::conv_ascii));

  XRCCTRL(*this, "wxID_REPLACE_BY", wxStaticText)->SetLabel(wxString(TtaGetMessage (LIB, TMSG_REPLACE_BY), AmayaApp::conv_ascii));

  XRCCTRL(*this, "wxID_REPLACE_BOX", wxRadioBox)->SetLabel(wxString(TtaGetMessage (LIB, TMSG_REPLACE), AmayaApp::conv_ascii));
  XRCCTRL(*this, "wxID_REPLACE_BOX", wxRadioBox)->SetString(0, wxString(TtaGetMessage (LIB, TMSG_NO_REPLACE), AmayaApp::conv_ascii));
  XRCCTRL(*this, "wxID_REPLACE_BOX", wxRadioBox)->SetString(1, wxString(TtaGetMessage (LIB, TMSG_REPLACE_ON_REQU), AmayaApp::conv_ascii));
  XRCCTRL(*this, "wxID_REPLACE_BOX", wxRadioBox)->SetString(2, wxString(TtaGetMessage (LIB, TMSG_AUTO_REPLACE), AmayaApp::conv_ascii));

  XRCCTRL(*this, "wxID_SEARCH_AREA_BOX", wxRadioBox)->SetLabel(wxString(TtaGetMessage (LIB, TMSG_SEARCH_WHERE), AmayaApp::conv_ascii));
  XRCCTRL(*this, "wxID_SEARCH_AREA_BOX", wxRadioBox)->SetString(0, wxString(TtaGetMessage (LIB, TMSG_BEFORE_SEL), AmayaApp::conv_ascii));
  XRCCTRL(*this, "wxID_SEARCH_AREA_BOX", wxRadioBox)->SetString(1, wxString(TtaGetMessage (LIB, TMSG_AFTER_SEL), AmayaApp::conv_ascii));
  XRCCTRL(*this, "wxID_SEARCH_AREA_BOX", wxRadioBox)->SetString(2, wxString(TtaGetMessage (LIB, TMSG_WITHIN_SEL), AmayaApp::conv_ascii));
  XRCCTRL(*this, "wxID_SEARCH_AREA_BOX", wxRadioBox)->SetString(3, wxString(TtaGetMessage (LIB, TMSG_IN_WHOLE_DOC), AmayaApp::conv_ascii));

  // update button labels
  XRCCTRL(*this, "wxID_CONFIRMBUTTON", wxButton)->SetLabel(wxString(TtaGetMessage (LIB, TMSG_LIB_CONFIRM), AmayaApp::conv_ascii));
  XRCCTRL(*this, "wxID_CANCELBUTTON", wxButton)->SetLabel(wxString(TtaGetMessage (LIB, TMSG_CANCEL), AmayaApp::conv_ascii));
  XRCCTRL(*this, "wxID_NOREPLACEBUTTON", wxButton)->SetLabel(wxString(TtaGetMessage (LIB, TMSG_DO_NOT_REPLACE), AmayaApp::conv_ascii)); 

  Layout();
  
  SetAutoLayout( TRUE );
}

/*----------------------------------------------------------------------
  Destructor. (Empty, as I don't need anything special done when destructing).
  ----------------------------------------------------------------------*/
SearchDlgWX::~SearchDlgWX()
{
}

/*----------------------------------------------------------------------
  OnConfirmButton called when clicking on confirmbutton
  ----------------------------------------------------------------------*/
void SearchDlgWX::OnConfirmButton( wxCommandEvent& event )
{
  wxString new_title = XRCCTRL(*this, "wxID_TITLE", wxTextCtrl)->GetValue( );
  wxLogDebug( _T("SearchDlgWX::OnConfirmButton - new_title=")+new_title );
  
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
void SearchDlgWX::OnCancelButton( wxCommandEvent& event )
{
  ThotCallback (BaseDialog + TitleForm, INTEGER_DATA, (char*) 0); 
  EndModal( 0 );
}

/*----------------------------------------------------------------------
  OnNoReplaceButton called when clicking on cancelbutton
  ----------------------------------------------------------------------*/
void SearchDlgWX::OnNoReplaceButton( wxCommandEvent& event )
{
  ThotCallback (BaseDialog + TitleForm, INTEGER_DATA, (char*) 0); 
  EndModal( 0 );
}

#endif /* _WX */
