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
BEGIN_EVENT_TABLE(PrintDlgWX, AmayaDialog)
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
PrintDlgWX::PrintDlgWX( int ref,
			wxWindow* parent,
			const wxString & ps_file ) :
  AmayaDialog( NULL, ref )
{
  wxXmlResource::Get()->LoadDialog(this, parent, wxT("PrintDlgWX"));
  wxLogDebug( _T("PrintDlgWX::PrintDlgWX - ps_file=")+ps_file);
  wxString wx_title = TtaConvMessageToWX( TtaGetMessage (LIB, TMSG_LIB_PRINT) );
  SetTitle( wx_title );

  // paper format radio box
  XRCCTRL(*this, "wxID_PAPER_FORMAT_BOX", wxRadioBox)->SetLabel(TtaConvMessageToWX( TtaGetMessage (LIB, TMSG_PAPER_SIZE) ));
  XRCCTRL(*this, "wxID_PAPER_FORMAT_BOX", wxRadioBox)->SetString(0, TtaConvMessageToWX( TtaGetMessage (LIB, TMSG_A4) ));
  XRCCTRL(*this, "wxID_PAPER_FORMAT_BOX", wxRadioBox)->SetString(1, TtaConvMessageToWX( TtaGetMessage (LIB, TMSG_US) ));

  // output radio box 
  XRCCTRL(*this, "wxID_OUTPUT_BOX", wxRadioBox)->SetLabel(TtaConvMessageToWX( TtaGetMessage (LIB, TMSG_OUTPUT) ));
  XRCCTRL(*this, "wxID_OUTPUT_BOX", wxRadioBox)->SetString(0, TtaConvMessageToWX( TtaGetMessage (LIB, TMSG_PRINTER) ));
  XRCCTRL(*this, "wxID_OUTPUT_BOX", wxRadioBox)->SetString(1, TtaConvMessageToWX( TtaGetMessage (LIB, TMSG_PS_FILE) ));

  // orientation radio box  
  XRCCTRL(*this, "wxID_ORIENTATION_BOX", wxRadioBox)->SetLabel(TtaConvMessageToWX( TtaGetMessage (AMAYA, AM_ORIENTATION) ));
  XRCCTRL(*this, "wxID_ORIENTATION_BOX", wxRadioBox)->SetString(0, TtaConvMessageToWX( TtaGetMessage (AMAYA, AM_PORTRAIT) ));
  XRCCTRL(*this, "wxID_ORIENTATION_BOX", wxRadioBox)->SetString(1, TtaConvMessageToWX( TtaGetMessage (AMAYA, AM_LANDSCAPE) ));

  // disposition radio box
  XRCCTRL(*this, "wxID_DISPOSITION_BOX", wxRadioBox)->SetLabel(TtaConvMessageToWX( TtaGetMessage (LIB, TMSG_REDUCTION) ));
  XRCCTRL(*this, "wxID_DISPOSITION_BOX", wxRadioBox)->SetString(0, TtaConvMessageToWX( TtaGetMessage (LIB, TMSG_1_PAGE_SHEET) ));
  XRCCTRL(*this, "wxID_DISPOSITION_BOX", wxRadioBox)->SetString(1, TtaConvMessageToWX( TtaGetMessage (LIB, TMSG_2_PAGE_SHEET) ));
  XRCCTRL(*this, "wxID_DISPOSITION_BOX", wxRadioBox)->SetString(2, TtaConvMessageToWX( TtaGetMessage (LIB, TMSG_4_PAGE_SHEET) ));

  // options check list
  XRCCTRL(*this, "wxID_OPTIONS_TXT", wxStaticText)->SetLabel(TtaConvMessageToWX( TtaGetMessage(LIB, TMSG_OPTIONS) ));
  XRCCTRL(*this, "wxID_MANUAL_CHK", wxCheckBox)->SetLabel(TtaConvMessageToWX( TtaGetMessage(LIB, TMSG_MANUAL_FEED) ));
  XRCCTRL(*this, "wxID_TOC_CHK", wxCheckBox)->SetLabel(TtaConvMessageToWX( TtaGetMessage(AMAYA, AM_PRINT_TOC) ));
  XRCCTRL(*this, "wxID_LINKS_CHK", wxCheckBox)->SetLabel(TtaConvMessageToWX( TtaGetMessage(AMAYA, AM_NUMBERED_LINKS) ));
  XRCCTRL(*this, "wxID_PRINT_URL_CHK", wxCheckBox)->SetLabel(TtaConvMessageToWX( TtaGetMessage(AMAYA, AM_PRINT_URL) ));
  XRCCTRL(*this, "wxID_IGNORE_CSS_CHK", wxCheckBox)->SetLabel(TtaConvMessageToWX( TtaGetMessage(AMAYA, AM_WITH_CSS) ));

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
