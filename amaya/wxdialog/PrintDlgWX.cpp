#ifdef _WX

#include "wx/wx.h"
#include "wx/xrc/xmlres.h"              // XRC XML resouces

#include "AmayaApp.h"
#include "PrintDlgWX.h"

#define THOT_EXPORT extern
#include "amaya.h"
#include "appdialogue_wx.h"

//-----------------------------------------------------------------------------
// Event table: connect the events to the handler functions to process them
//-----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(PrintDlgWX, wxDialog)
  EVT_BUTTON( XRCID("wxID_CONFIRMBUTTON"),     PrintDlgWX::OnPrintButton )
  EVT_BUTTON( XRCID("wxID_CANCELBUTTON"),      PrintDlgWX::OnCancelButton )
  /*
  EVT_BUTTON( XRCID("wxID_NOREPLACEBUTTON"),   PrintDlgWX::OnNoReplaceButton )
  EVT_RADIOBOX( XRCID("wxID_REPLACE_BOX"),     PrintDlgWX::OnReplaceBox )
  EVT_RADIOBOX( XRCID("wxID_SEARCH_AREA_BOX"), PrintDlgWX::OnSearchAreaBox )
  EVT_CHECKBOX( XRCID("wxID_CHECK_CASE"),      PrintDlgWX::OnCheckCaseBox )
  EVT_TEXT_ENTER( XRCID("wxID_SEARCH_FOR_TXT"),PrintDlgWX::OnConfirmButton )
  */
END_EVENT_TABLE()

/*----------------------------------------------------------------------
  PrintDlgWX create the SetupAndPrint dialog 
  params:
    + parent : parent window
    + ps_file : postscript file
  ----------------------------------------------------------------------*/
PrintDlgWX::PrintDlgWX( wxWindow* parent,
			 const wxString & ps_file ) : wxDialog()
{
  wxXmlResource::Get()->LoadDialog(this, parent, wxT("PrintDlgWX"));
  wxLogDebug( _T("PrintDlgWX::PrintDlgWX - ps_file=")+ps_file);
  wxString wx_title = TtaConvMessageToWX( TtaGetMessage (LIB, TMSG_LIB_PRINT) );
  SetTitle( wx_title );

  // update dialog labels
  /*
  XRCCTRL(*this, "wxID_SEARCH_FOR", wxStaticText)->SetLabel(TtaConvMessageToWX( TtaGetMessage(LIB, TMSG_SEARCH_FOR) ));

  XRCCTRL(*this, "wxID_CHECK_CASE", wxCheckBox)->SetLabel(TtaConvMessageToWX( TtaGetMessage(LIB, TMSG_UPPERCASE_EQ_LOWERCASE) ));

  XRCCTRL(*this, "wxID_REPLACE_BY", wxStaticText)->SetLabel(TtaConvMessageToWX( TtaGetMessage(LIB, TMSG_REPLACE_BY) ));

  XRCCTRL(*this, "wxID_REPLACE_BOX", wxRadioBox)->SetLabel(TtaConvMessageToWX( TtaGetMessage (LIB, TMSG_REPLACE) ));
  XRCCTRL(*this, "wxID_REPLACE_BOX", wxRadioBox)->SetString(0, TtaConvMessageToWX( TtaGetMessage (LIB, TMSG_NO_REPLACE) ));
  XRCCTRL(*this, "wxID_REPLACE_BOX", wxRadioBox)->SetString(1, TtaConvMessageToWX( TtaGetMessage (LIB, TMSG_REPLACE_ON_REQU) ));
  XRCCTRL(*this, "wxID_REPLACE_BOX", wxRadioBox)->SetString(2, TtaConvMessageToWX( TtaGetMessage (LIB, TMSG_AUTO_REPLACE) ));

  XRCCTRL(*this, "wxID_SEARCH_AREA_BOX", wxRadioBox)->SetLabel(TtaConvMessageToWX( TtaGetMessage (LIB, TMSG_SEARCH_WHERE) ));
  XRCCTRL(*this, "wxID_SEARCH_AREA_BOX", wxRadioBox)->SetString(0, TtaConvMessageToWX( TtaGetMessage(LIB, TMSG_BEFORE_SEL) ));
  XRCCTRL(*this, "wxID_SEARCH_AREA_BOX", wxRadioBox)->SetString(1, TtaConvMessageToWX( TtaGetMessage(LIB, TMSG_WITHIN_SEL) ));
  XRCCTRL(*this, "wxID_SEARCH_AREA_BOX", wxRadioBox)->SetString(2, TtaConvMessageToWX( TtaGetMessage(LIB, TMSG_AFTER_SEL) ));
  XRCCTRL(*this, "wxID_SEARCH_AREA_BOX", wxRadioBox)->SetString(3, TtaConvMessageToWX( TtaGetMessage(LIB, TMSG_IN_WHOLE_DOC) ));

  */
  // update button labels
  XRCCTRL(*this, "wxID_PRINTBUTTON", wxButton)->SetLabel(TtaConvMessageToWX( TtaGetMessage(AMAYA, AM_BUTTON_PRINT) ));
  XRCCTRL(*this, "wxID_CANCELBUTTON", wxButton)->SetLabel(TtaConvMessageToWX( TtaGetMessage(LIB, TMSG_CANCEL) ));

  // Default values
  /*
  XRCCTRL(*this, "wxID_CHECK_CASE", wxCheckBox)->SetValue(TRUE);
  m_case = XRCCTRL(*this, "wxID_CHECK_CASE", wxCheckBox)->GetValue( );

  XRCCTRL(*this, "wxID_REPLACE_BOX", wxRadioBox)->SetSelection(0);
  m_ireplace = XRCCTRL(*this, "wxID_REPLACE_BOX", wxRadioBox)->GetSelection( );

  XRCCTRL(*this, "wxID_SEARCH_AREA_BOX", wxRadioBox)->SetSelection(2);
  m_iarea = XRCCTRL(*this, "wxID_SEARCH_AREA_BOX", wxRadioBox)->GetSelection( );
  */
  Layout();
  
  SetAutoLayout( TRUE );
}

/*----------------------------------------------------------------------
  Destructor. (Empty, as I don't need anything special done when destructing).
  ----------------------------------------------------------------------*/
PrintDlgWX::~PrintDlgWX()
{
}

/*----------------------------------------------------------------------
  OnConfirmButton called when clicking on confirmbutton
  ----------------------------------------------------------------------*/
void PrintDlgWX::OnPrintButton( wxCommandEvent& event )
{
  /*
  wxString textToSearch = XRCCTRL(*this, "wxID_SEARCH_FOR_TXT",
				  wxTextCtrl)->GetValue( );
  wxString newText = XRCCTRL(*this, "wxID_REPLACE_BY_TXT",
			     wxTextCtrl)->GetValue( );
  */

  wxLogDebug( _T("PrintDlgWX::OnPrintButton") );
  
  // allocate temporary buffers to copy the *text* buffers
  /*
  char buf_old_text[512];
  char buf_new_text[512];
  wxASSERT( textToSearch.Len() < 512 );
  wxASSERT( newText.Len() < 512 );
  strcpy( buf_old_text, textToSearch.ToAscii() );
  strcpy( buf_new_text, newText.ToAscii() );

  if (buf_new_text && buf_new_text[0] != '\0' && m_ireplace == 0)
    {
      m_ireplace = 1;
      XRCCTRL(*this, "wxID_REPLACE_BOX", wxRadioBox)->SetSelection(1);
    }
  if (m_ireplace == 1 || m_ireplace == 2) 
    ThotCallback (NumZoneTextReplace, STRING_DATA, buf_new_text);

  ThotCallback (NumZoneTextSearch, STRING_DATA, buf_old_text);
  ThotCallback (NumMenuReplaceMode, INTEGER_DATA, (char *) m_ireplace);
  ThotCallback (NumMenuOrSearchText, INTEGER_DATA, (char *) m_iarea);
  ThotCallback (NumFormSearchText, INTEGER_DATA, (char *) 1);
  */

}

/*----------------------------------------------------------------------
  OnCancelButton called when clicking on cancelbutton
  ----------------------------------------------------------------------*/
void PrintDlgWX::OnCancelButton( wxCommandEvent& event )
{
  ThotCallback (BasePrint + FormPrint, INTEGER_DATA, (char*)0);
  EndModal( 0 );
}

#ifdef ZOBI
/*----------------------------------------------------------------------
  OnSearchAreaBox called when clicking on cancelbutton
  ----------------------------------------------------------------------*/
void SearchDlgWX::OnSearchAreaBox ( wxCommandEvent& event )
{
  m_iarea = XRCCTRL(*this, "wxID_SEARCH_AREA_BOX",
		    wxRadioBox)->GetSelection( );
  wxLogDebug( _T("SearchDlgWX::OnSearchAreaBox - %d") ,m_iarea);
}

/*----------------------------------------------------------------------
  OnCheckCaseBox called when clicking on cancelbutton
  ----------------------------------------------------------------------*/
void SearchDlgWX::OnCheckCaseBox ( wxCommandEvent& event )
{
  if (m_case)
    m_case = FALSE;
  else
    m_case = TRUE;
  wxLogDebug( _T("SearchDlgWX::OnCheckCaseBox - %d") ,m_case);
  ThotCallback (NumToggleUpperEqualLower, INTEGER_DATA, (char *) 0);
}
#endif

#endif /* _WX */
