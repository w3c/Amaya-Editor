#ifdef _WX

#include "wx/wx.h"
#include "wx/xrc/xmlres.h"              // XRC XML resouces

#include "AmayaApp.h"
#include "SaveAsDlgWX.h"

#define THOT_EXPORT extern
#include "amaya.h"
#include "print.h"
#include "appdialogue_wx.h"

//-----------------------------------------------------------------------------
// Event table: connect the events to the handler functions to process them
//-----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(SaveAsDlgWX, AmayaDialog)
  EVT_BUTTON( XRCID("wxID_CONFIRMBUTTON"),      SaveAsDlgWX::OnConfirmButton )
  EVT_BUTTON( XRCID("wxID_CANCELBUTTON"),       SaveAsDlgWX::OnCancelButton )
  /*
  EVT_CHECKBOX( XRCID("wxID_IGNORE_CSS_CHK"),   SaveAsDlgWX::OnIgnoreCssChkBox )
  EVT_TEXT( XRCID("wxID_FILE_TXT_CTRL"),        SaveAsDlgWX::OnTypePrinter )
  */
END_EVENT_TABLE()

/*----------------------------------------------------------------------
  SaveAsDlgWX create the SaveAs dialog 
  params:
    + parent : parent window
    + pathname : document location
  ----------------------------------------------------------------------*/
SaveAsDlgWX::SaveAsDlgWX( int ref,
			  wxWindow* parent,
			  const wxString & pathname ) :
  AmayaDialog( NULL, ref )
{
  int  page_size;

  wxXmlResource::Get()->LoadDialog(this, parent, wxT("SaveAsDlgWX"));
  wxLogDebug( _T("PrintDlgWX::SaveAsDlgWX - pathname=")+pathname);
  wxString wx_title = TtaConvMessageToWX( TtaGetMessage (LIB, TMSG_LIB_PRINT) );
  SetTitle( wx_title );

#ifdef KK
  // paper format radio box
  XRCCTRL(*this, "wxID_PAPER_FORMAT_BOX", wxRadioBox)->SetLabel(TtaConvMessageToWX( TtaGetMessage (LIB, TMSG_PAPER_SIZE) ));
  XRCCTRL(*this, "wxID_PAPER_FORMAT_BOX", wxRadioBox)->SetString(0, TtaConvMessageToWX( TtaGetMessage (LIB, TMSG_A4) ));
  XRCCTRL(*this, "wxID_PAPER_FORMAT_BOX", wxRadioBox)->SetString(1, TtaConvMessageToWX( TtaGetMessage (LIB, TMSG_US) ));
  page_size = TtaGetPrintParameter (PP_PaperSize);
  if (page_size == PP_US)
    XRCCTRL(*this, "wxID_PAPER_FORMAT_BOX", wxRadioBox)->SetSelection(1);
  else
    XRCCTRL(*this, "wxID_PAPER_FORMAT_BOX", wxRadioBox)->SetSelection(0);

  // output radio box 
  XRCCTRL(*this, "wxID_OUTPUT_BOX", wxRadioBox)->SetLabel(TtaConvMessageToWX( TtaGetMessage (LIB, TMSG_OUTPUT) ));
  XRCCTRL(*this, "wxID_OUTPUT_BOX", wxRadioBox)->SetString(0, TtaConvMessageToWX( TtaGetMessage (LIB, TMSG_PRINTER) ));
  XRCCTRL(*this, "wxID_OUTPUT_BOX", wxRadioBox)->SetString(1, TtaConvMessageToWX( TtaGetMessage (LIB, TMSG_PS_FILE) ));
  XRCCTRL(*this, "wxID_OUTPUT_BOX", wxRadioBox)->SetSelection(0);
  m_print = 0;
  XRCCTRL(*this, "wxID_FILE_TXT_CTRL", wxTextCtrl)->SetValue(m_Printer);

  // orientation radio box  
  XRCCTRL(*this, "wxID_ORIENTATION_BOX", wxRadioBox)->SetLabel(TtaConvMessageToWX( TtaGetMessage (AMAYA, AM_ORIENTATION) ));
  XRCCTRL(*this, "wxID_ORIENTATION_BOX", wxRadioBox)->SetString(0, TtaConvMessageToWX( TtaGetMessage (AMAYA, AM_PORTRAIT) ));
  XRCCTRL(*this, "wxID_ORIENTATION_BOX", wxRadioBox)->SetString(1, TtaConvMessageToWX( TtaGetMessage (AMAYA, AM_LANDSCAPE) ));
  XRCCTRL(*this, "wxID_ORIENTATION_BOX", wxRadioBox)->SetSelection(0);

  // disposition radio box
  XRCCTRL(*this, "wxID_DISPOSITION_BOX", wxRadioBox)->SetLabel(TtaConvMessageToWX( TtaGetMessage (LIB, TMSG_REDUCTION) ));
  XRCCTRL(*this, "wxID_DISPOSITION_BOX", wxRadioBox)->SetString(0, TtaConvMessageToWX( TtaGetMessage (LIB, TMSG_1_PAGE_SHEET) ));
  XRCCTRL(*this, "wxID_DISPOSITION_BOX", wxRadioBox)->SetString(1, TtaConvMessageToWX( TtaGetMessage (LIB, TMSG_2_PAGE_SHEET) ));
  XRCCTRL(*this, "wxID_DISPOSITION_BOX", wxRadioBox)->SetString(2, TtaConvMessageToWX( TtaGetMessage (LIB, TMSG_4_PAGE_SHEET) ));
  XRCCTRL(*this, "wxID_DISPOSITION_BOX", wxRadioBox)->SetSelection(0);

  // options check list
  XRCCTRL(*this, "wxID_OPTIONS_TXT", wxStaticText)->SetLabel(TtaConvMessageToWX( TtaGetMessage(LIB, TMSG_OPTIONS) ));
  XRCCTRL(*this, "wxID_MANUAL_CHK", wxCheckBox)->SetLabel(TtaConvMessageToWX( TtaGetMessage(LIB, TMSG_MANUAL_FEED) ));
  XRCCTRL(*this, "wxID_TOC_CHK", wxCheckBox)->SetLabel(TtaConvMessageToWX( TtaGetMessage(AMAYA, AM_PRINT_TOC) ));
  XRCCTRL(*this, "wxID_LINKS_CHK", wxCheckBox)->SetLabel(TtaConvMessageToWX( TtaGetMessage(AMAYA, AM_NUMBERED_LINKS) ));
  XRCCTRL(*this, "wxID_PRINT_URL_CHK", wxCheckBox)->SetLabel(TtaConvMessageToWX( TtaGetMessage(AMAYA, AM_PRINT_URL) ));
  XRCCTRL(*this, "wxID_IGNORE_CSS_CHK", wxCheckBox)->SetLabel(TtaConvMessageToWX( TtaGetMessage(AMAYA, AM_WITH_CSS) ));
  XRCCTRL(*this, "wxID_MANUAL_CHK", wxCheckBox)->SetValue(FALSE);
  XRCCTRL(*this, "wxID_TOC_CHK", wxCheckBox)->SetValue(FALSE);
  XRCCTRL(*this, "wxID_LINKS_CHK", wxCheckBox)->SetValue(FALSE);
  XRCCTRL(*this, "wxID_PRINT_URL_CHK", wxCheckBox)->SetValue(TRUE);
  XRCCTRL(*this, "wxID_IGNORE_CSS_CHK", wxCheckBox)->SetValue(FALSE);

#endif

  // buttons
  XRCCTRL(*this, "wxID_CONFIRMBUTTON", wxButton)->SetLabel(TtaConvMessageToWX( TtaGetMessage(AMAYA, AM_BUTTON_PRINT) ));
  XRCCTRL(*this, "wxID_CANCELBUTTON", wxButton)->SetLabel(TtaConvMessageToWX( TtaGetMessage(LIB, TMSG_CANCEL) ));

  Layout();
  
  SetAutoLayout( TRUE );
}

/*---------------------------------------------------------------------------
  Destructor. (Empty, as we don't need anything special done when destructing).
  ---------------------------------------------------------------------------*/
SaveAsDlgWX::~SaveAsDlgWX()
{
}

/*----------------------------------------------------------------------
  OnConfirmButton called when clicking the Confirm button
  ----------------------------------------------------------------------*/
void SaveAsDlgWX::OnConfirmButton( wxCommandEvent& event )
{
  wxLogDebug( _T("SaveAsDlgWX::OnConfirmButton") );
  // print callback
  ThotCallback (BasePrint + FormPrint, INTEGER_DATA, (char*) 1);
}

/*----------------------------------------------------------------------
  OnCancelButton called when clicking on cancelbutton
  ----------------------------------------------------------------------*/
void SaveAsDlgWX::OnCancelButton( wxCommandEvent& event )
{
  wxLogDebug( _T("SaveAsDlgWX::OnCancelButton") );
  ThotCallback (BasePrint + FormPrint, INTEGER_DATA, (char*) 0);
}


/*---------------------------------------------------------------
  OnTocChkBox
  ---------------------------------------------------------------*/
/*
void PrintDlgWX::OnTocChkBox ( wxCommandEvent& event )
{
  wxLogDebug( _T("PrintDlgWX::OnTocChkBox") );
  ThotCallback (BasePrint + PrintOptions, INTEGER_DATA,	(char*) 1);
}

ThotCallback (BasePrint + PPrinterName,  STRING_DATA, (char *)buffer );
}
*/

#endif /* _WX */
