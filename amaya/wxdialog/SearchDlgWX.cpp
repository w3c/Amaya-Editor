#ifdef _WX

#include "wx/wx.h"
#include "wx/xrc/xmlres.h"              // XRC XML resouces

#include "AmayaApp.h"
#include "SearchDlgWX.h"

#define THOT_EXPORT extern
#include "amaya.h"
#include "appdialogue_wx.h"

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
  XRCCTRL(*this, "wxID_SEARCH_FOR", wxStaticText)->SetLabel(TtaConvMessageToWX( TtaGetMessage(LIB, TMSG_SEARCH_FOR) ));
  XRCCTRL(*this, "wxID_CHECK_CASE", wxCheckBox)->SetLabel(TtaConvMessageToWX( TtaGetMessage(LIB, TMSG_UPPERCASE_EQ_LOWERCASE) ));

  XRCCTRL(*this, "wxID_REPLACE_BY", wxStaticText)->SetLabel(TtaConvMessageToWX( TtaGetMessage(LIB, TMSG_REPLACE_BY) ));

  XRCCTRL(*this, "wxID_REPLACE_BOX", wxRadioBox)->SetLabel(TtaConvMessageToWX( TtaGetMessage (LIB, TMSG_REPLACE) ));
  XRCCTRL(*this, "wxID_REPLACE_BOX", wxRadioBox)->SetString(0, TtaConvMessageToWX( TtaGetMessage (LIB, TMSG_NO_REPLACE) ));
  XRCCTRL(*this, "wxID_REPLACE_BOX", wxRadioBox)->SetString(1, TtaConvMessageToWX( TtaGetMessage (LIB, TMSG_REPLACE_ON_REQU) ));
  XRCCTRL(*this, "wxID_REPLACE_BOX", wxRadioBox)->SetString(2, TtaConvMessageToWX( TtaGetMessage (LIB, TMSG_AUTO_REPLACE) ));

  XRCCTRL(*this, "wxID_SEARCH_AREA_BOX", wxRadioBox)->SetLabel(TtaConvMessageToWX( TtaGetMessage (LIB, TMSG_SEARCH_WHERE) ));
  XRCCTRL(*this, "wxID_SEARCH_AREA_BOX", wxRadioBox)->SetString(0, TtaConvMessageToWX( TtaGetMessage(LIB, TMSG_BEFORE_SEL) ));
  XRCCTRL(*this, "wxID_SEARCH_AREA_BOX", wxRadioBox)->SetString(1, TtaConvMessageToWX( TtaGetMessage(LIB, TMSG_AFTER_SEL) ));
  XRCCTRL(*this, "wxID_SEARCH_AREA_BOX", wxRadioBox)->SetString(2, TtaConvMessageToWX( TtaGetMessage(LIB, TMSG_WITHIN_SEL) ));
  XRCCTRL(*this, "wxID_SEARCH_AREA_BOX", wxRadioBox)->SetString(3, TtaConvMessageToWX( TtaGetMessage(LIB, TMSG_IN_WHOLE_DOC) ));

  // update button labels
  XRCCTRL(*this, "wxID_CONFIRMBUTTON", wxButton)->SetLabel(TtaConvMessageToWX( TtaGetMessage(LIB, TMSG_LIB_CONFIRM) ));
  XRCCTRL(*this, "wxID_CANCELBUTTON", wxButton)->SetLabel(TtaConvMessageToWX( TtaGetMessage(LIB, TMSG_CANCEL) ));
  XRCCTRL(*this, "wxID_NOREPLACEBUTTON", wxButton)->SetLabel(TtaConvMessageToWX( TtaGetMessage(LIB, TMSG_DO_NOT_REPLACE) )); 

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
  wxString new_title = XRCCTRL(*this, "wx_ID_SEAR_FOR_TXT", wxTextCtrl)->GetValue( );
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
