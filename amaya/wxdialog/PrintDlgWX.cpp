#ifdef _WX

#include "wx/wx.h"
#include "wx/xrc/xmlres.h"              // XRC XML resouces

#include "AmayaApp.h"
#include "PrintDlgWX.h"

#define THOT_EXPORT extern
#include "amaya.h"
#include "print.h"
#include "appdialogue_wx.h"

//-----------------------------------------------------------------------------
// Event table: connect the events to the handler functions to process them
//-----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(PrintDlgWX, AmayaDialog)
  EVT_BUTTON( XRCID("wxID_PRINTBUTTON"),        PrintDlgWX::OnPrintButton )
  EVT_BUTTON( XRCID("wxID_CANCELBUTTON"),       PrintDlgWX::OnCancelButton )
  EVT_RADIOBOX( XRCID("wxID_PAPER_FORMAT_BOX"), PrintDlgWX::OnPaperFormatBox )
  EVT_RADIOBOX( XRCID("wxID_ORIENTATION_BOX"),  PrintDlgWX::OnPaperOrientationBox )
  EVT_RADIOBOX( XRCID("wxID_DISPOSITION_BOX"),  PrintDlgWX::OnPaperDispositionBox )
  EVT_RADIOBOX( XRCID("wxID_OUTPUT_BOX"),       PrintDlgWX::OnOutputBox )
  EVT_CHECKBOX( XRCID("wxID_MANUAL_CHK"),       PrintDlgWX::OnManualChkBox )
  EVT_CHECKBOX( XRCID("wxID_TOC_CHK"),          PrintDlgWX::OnTocChkBox )
  EVT_CHECKBOX( XRCID("wxID_LINKS_CHK"),        PrintDlgWX::OnLinksChkBox )
  EVT_CHECKBOX( XRCID("wxID_PRINT_URL_CHK"),    PrintDlgWX::OnPrintUrlChkBox )
  EVT_CHECKBOX( XRCID("wxID_IGNORE_CSS_CHK"),   PrintDlgWX::OnIgnoreCssChkBox )
  EVT_TEXT( XRCID("wxID_FILE_TXT_CTRL"),        PrintDlgWX::OnTypePrinter )
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
  int  page_size;
  m_PPrinter = TtaConvMessageToWX( TtaGetEnvString ("THOTPRINT") );
  m_PS = ps_file;

  wxXmlResource::Get()->LoadDialog(this, parent, wxT("PrintDlgWX"));
  wxLogDebug( _T("PrintDlgWX::PrintDlgWX - ps_file=")+ps_file);
  wxString wx_title = TtaConvMessageToWX( TtaGetMessage (LIB, TMSG_LIB_PRINT) );
  SetTitle( wx_title );

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
  XRCCTRL(*this, "wxID_FILE_TXT_CTRL", wxTextCtrl)->SetValue(m_PPrinter);
  m_printer = 0;

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

  // buttons
  XRCCTRL(*this, "wxID_PRINTBUTTON", wxButton)->SetLabel(TtaConvMessageToWX( TtaGetMessage(AMAYA, AM_BUTTON_PRINT) ));
  XRCCTRL(*this, "wxID_CANCELBUTTON", wxButton)->SetLabel(TtaConvMessageToWX( TtaGetMessage(LIB, TMSG_CANCEL) ));

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
  OnPrintButton called when clicking the Print button
  ----------------------------------------------------------------------*/
void PrintDlgWX::OnPrintButton( wxCommandEvent& event )
{
  wxLogDebug( _T("PrintDlgWX::OnPrintButton") );

  // print callback
  ThotCallback (BasePrint + FormPrint, INTEGER_DATA, (char*) 1);
}

/*----------------------------------------------------------------------
  OnCancelButton called when clicking on cancelbutton
  ----------------------------------------------------------------------*/
void PrintDlgWX::OnCancelButton( wxCommandEvent& event )
{
  ThotCallback (BasePrint + FormPrint, INTEGER_DATA, (char*) 0);
}

/*----------------------------------------------------------------------
  OnPaperFormatBox called when clicking the Paper Format radio box
  ----------------------------------------------------------------------*/
void PrintDlgWX::OnPaperFormatBox ( wxCommandEvent& event )
{
  wxLogDebug( _T("PrintDlgWX::OnPaperFormatBox") );
  ThotCallback (BasePrint + PaperFormat, INTEGER_DATA,
		(char*) (XRCCTRL(*this, "wxID_PAPER_FORMAT_BOX", wxRadioBox)->GetSelection( )));
}

/*----------------------------------------------------------------------------
  OnPaperOrientationBox called when clicking the Paper Orientation radio box
  ----------------------------------------------------------------------------*/
void PrintDlgWX::OnPaperOrientationBox ( wxCommandEvent& event )
{
  wxLogDebug( _T("PrintDlgWX::OnPaperOrientationBox") );
  ThotCallback (BasePrint + PaperOrientation, INTEGER_DATA,
		(char*) (XRCCTRL(*this, "wxID_ORIENTATION_BOX", wxRadioBox)->GetSelection( )));
}

/*---------------------------------------------------------------------------
  OnPaperDispositionBox called when clicking the Paper Disposition radio box
  ----------------------------------------------------------------------------*/
void PrintDlgWX::OnPaperDispositionBox ( wxCommandEvent& event )
{
  wxLogDebug( _T("PrintDlgWX::OnPaperDispositionBox") );
  ThotCallback (BasePrint + PPagesPerSheet, INTEGER_DATA,
		(char*) (XRCCTRL(*this, "wxID_DISPOSITION_BOX", wxRadioBox)->GetSelection( )));
}

/*----------------------------------------------------------------------------
  OnOutputBox called when selecting the output radio box
  ----------------------------------------------------------------------------*/
void PrintDlgWX::OnOutputBox ( wxCommandEvent& event )
{
  int m_output;

  wxLogDebug( _T("PrintDlgWX::OnOutputBox") );

  m_output = XRCCTRL(*this, "wxID_OUTPUT_BOX", wxRadioBox)->GetSelection( );
  ThotCallback (BasePrint + PrintSupport, INTEGER_DATA,
		(char*) m_output);
  if (m_output == 0)
    {
      if (m_printer == 1)
	XRCCTRL(*this, "wxID_FILE_TXT_CTRL", wxTextCtrl)->SetValue(m_PPrinter);
    }
  else
    {
      if (m_printer == 0)
	XRCCTRL(*this, "wxID_FILE_TXT_CTRL", wxTextCtrl)->SetValue(m_PS);
    }
  m_printer = m_output;
}

/*---------------------------------------------------------------
  OnManualChkBox
  ---------------------------------------------------------------*/
void PrintDlgWX::OnManualChkBox ( wxCommandEvent& event )
{
  wxLogDebug( _T("PrintDlgWX::OnManualChkBox") );
  ThotCallback (BasePrint + PrintOptions, INTEGER_DATA,	(char*) 0);
}

/*---------------------------------------------------------------
  OnTocChkBox
  ---------------------------------------------------------------*/
void PrintDlgWX::OnTocChkBox ( wxCommandEvent& event )
{
  wxLogDebug( _T("PrintDlgWX::OnTocChkBox") );
  ThotCallback (BasePrint + PrintOptions, INTEGER_DATA,	(char*) 1);
}

/*---------------------------------------------------------------
  OnLinksChkBox
  ---------------------------------------------------------------*/
void PrintDlgWX::OnLinksChkBox ( wxCommandEvent& event )
{
  wxLogDebug( _T("PrintDlgWX::OnLinksChkBox") );
  ThotCallback (BasePrint + PrintOptions, INTEGER_DATA,	(char*) 2);
}

/*---------------------------------------------------------------
  OnPrintUrlChkBox
  ---------------------------------------------------------------*/
void PrintDlgWX::OnPrintUrlChkBox ( wxCommandEvent& event )
{
  wxLogDebug( _T("PrintDlgWX::OnPrintUrlChkBox") );
  ThotCallback (BasePrint + PrintOptions, INTEGER_DATA,	(char*) 3);
}

/*---------------------------------------------------------------
  OnIgnoreCssChkBox
  ---------------------------------------------------------------*/
void PrintDlgWX::OnIgnoreCssChkBox ( wxCommandEvent& event )
{
  wxLogDebug( _T("PrintDlgWX::OnIgnoreCssChkBox") );
  ThotCallback (BasePrint + PrintOptions, INTEGER_DATA,	(char*) 4);
}

/*---------------------------------------------------------------
  OnTypePrinter Set the printer name
  ---------------------------------------------------------------*/
void PrintDlgWX::OnTypePrinter ( wxCommandEvent& event )
{

  wxString printer_name = XRCCTRL(*this, "wxID_FILE_TXT_CTRL", wxTextCtrl)->GetValue( );
  wxLogDebug( _T("PrintDlgWX::OnTypePrinter -printer_name =")+printer_name );

  if (m_printer == 0)
    m_PPrinter = printer_name;
  else
    m_PS = printer_name;
  
  // set the printer name
  // allocate a temporary buffer to copy the 'const char *' printer name buffer 
  char buffer[100];
  wxASSERT( printer_name.Len() < 100 );
  strcpy( buffer, printer_name.ToAscii() );
  ThotCallback (BasePrint + PPrinterName,  STRING_DATA, (char *)buffer );
}

#endif /* _WX */
