/*
 *
 *  (c) COPYRIGHT INRIA and W3C, 1996-2008
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

#ifdef _WX
#include "wx/wx.h"
#include "wx/xrc/xmlres.h"              // XRC XML resouces

#include "AmayaApp.h"
#include "PrintDlgWX.h"

#define THOT_EXPORT extern
#include "amaya.h"
#include "print.h"
#include "appdialogue_wx.h"
#include "message_wx.h"

//-----------------------------------------------------------------------------
// Event table: connect the events to the handler functions to process them
//-----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(PrintDlgWX, AmayaDialog)
  EVT_BUTTON( XRCID("wxID_OK"),        PrintDlgWX::OnPrintButton )
  EVT_BUTTON( XRCID("wxID_CANCEL"),       PrintDlgWX::OnCancelButton )
#ifndef _WINDOWS
  EVT_RADIOBOX( XRCID("wxID_PAPER_FORMAT_BOX"), PrintDlgWX::OnPaperFormatBox )
  EVT_RADIOBOX( XRCID("wxID_ORIENTATION_BOX"),  PrintDlgWX::OnPaperOrientationBox )
  EVT_RADIOBOX( XRCID("wxID_DISPOSITION_BOX"),  PrintDlgWX::OnPaperDispositionBox )
  EVT_RADIOBOX( XRCID("wxID_OUTPUT_BOX"),       PrintDlgWX::OnOutputBox )
  EVT_CHECKBOX( XRCID("wxID_MANUAL_CHK"),       PrintDlgWX::OnManualChkBox )
  EVT_TEXT( XRCID("wxID_FILE_TXT_CTRL"),        PrintDlgWX::OnTypePrinter )
#endif /* _WINDOWS */
  EVT_CHECKBOX( XRCID("wxID_TOC_CHK"),          PrintDlgWX::OnTocChkBox )
  EVT_CHECKBOX( XRCID("wxID_LINKS_CHK"),        PrintDlgWX::OnLinksChkBox )
  EVT_CHECKBOX( XRCID("wxID_PRINT_URL_CHK"),    PrintDlgWX::OnPrintUrlChkBox )
  EVT_CHECKBOX( XRCID("wxID_IGNORE_CSS_CHK"),   PrintDlgWX::OnIgnoreCssChkBox )
END_EVENT_TABLE()

/*----------------------------------------------------------------------
  PrintDlgWX create the SetupAndPrint dialog 
  params:
    + parent : parent window
    + ps_file : postscript file
  ----------------------------------------------------------------------*/
PrintDlgWX::PrintDlgWX( int ref, wxWindow* parent, const wxString & printer_file,
                        const wxString & ps_file, int paper_format,
                        int orientation, int disposition, int paper_print,
                        bool manual_feed, bool with_toc, bool with_links,
                        bool with_url, bool ignore_css ) :
  AmayaDialog( parent, ref )
{
  m_Printer = printer_file;
  m_PS = ps_file;

#ifdef _WINDOWS
  wxXmlResource::Get()->LoadDialog(this, parent, wxT("WinPrintDlgWX"));
#else /* _WINDOWS */
  wxXmlResource::Get()->LoadDialog(this, parent, wxT("PrintDlgWX"));
#endif /* _WINDOWS */
  wxString wx_title = TtaConvMessageToWX( TtaGetMessage (LIB, TMSG_LIB_PRINT) );
  SetTitle( wx_title );

#ifndef _WINDOWS
  // paper format radio box
  XRCCTRL(*this, "wxID_PAPER_FORMAT_BOX", wxRadioBox)->SetLabel(TtaConvMessageToWX( TtaGetMessage (LIB, TMSG_PAPER_SIZE) ));
  XRCCTRL(*this, "wxID_PAPER_FORMAT_BOX", wxRadioBox)->SetString(0, TtaConvMessageToWX( TtaGetMessage (LIB, TMSG_A4) ));
  XRCCTRL(*this, "wxID_PAPER_FORMAT_BOX", wxRadioBox)->SetString(1, TtaConvMessageToWX( TtaGetMessage (LIB, TMSG_US) ));
  XRCCTRL(*this, "wxID_PAPER_FORMAT_BOX", wxRadioBox)->SetSelection(paper_format);

  // output radio box 
  XRCCTRL(*this, "wxID_OUTPUT_BOX", wxRadioBox)->SetLabel(TtaConvMessageToWX( TtaGetMessage (LIB, TMSG_OUTPUT) ));
  XRCCTRL(*this, "wxID_OUTPUT_BOX", wxRadioBox)->SetString(0, TtaConvMessageToWX( TtaGetMessage (LIB, TMSG_PRINTER) ));
  XRCCTRL(*this, "wxID_OUTPUT_BOX", wxRadioBox)->SetString(1, TtaConvMessageToWX( TtaGetMessage (LIB, TMSG_PS_FILE) ));
  XRCCTRL(*this, "wxID_OUTPUT_BOX", wxRadioBox)->SetSelection(paper_print);
  m_print = paper_print;
  if (paper_print == 0)
    XRCCTRL(*this, "wxID_FILE_TXT_CTRL", wxTextCtrl)->SetValue(m_Printer);
  else
    XRCCTRL(*this, "wxID_FILE_TXT_CTRL", wxTextCtrl)->SetValue(m_PS);

  // orientation radio box  
  XRCCTRL(*this, "wxID_ORIENTATION_BOX", wxRadioBox)->SetLabel(TtaConvMessageToWX( TtaGetMessage (AMAYA, AM_ORIENTATION) ));
  XRCCTRL(*this, "wxID_ORIENTATION_BOX", wxRadioBox)->SetString(0, TtaConvMessageToWX( TtaGetMessage (AMAYA, AM_PORTRAIT) ));
  XRCCTRL(*this, "wxID_ORIENTATION_BOX", wxRadioBox)->SetString(1, TtaConvMessageToWX( TtaGetMessage (AMAYA, AM_LANDSCAPE) ));
  XRCCTRL(*this, "wxID_ORIENTATION_BOX", wxRadioBox)->SetSelection(orientation);

  // disposition radio box
  XRCCTRL(*this, "wxID_DISPOSITION_BOX", wxRadioBox)->SetLabel(TtaConvMessageToWX( TtaGetMessage (LIB, TMSG_REDUCTION) ));
  XRCCTRL(*this, "wxID_DISPOSITION_BOX", wxRadioBox)->SetString(0, TtaConvMessageToWX( TtaGetMessage (LIB, TMSG_1_PAGE_SHEET) ));
  XRCCTRL(*this, "wxID_DISPOSITION_BOX", wxRadioBox)->SetString(1, TtaConvMessageToWX( TtaGetMessage (LIB, TMSG_2_PAGE_SHEET) ));
  XRCCTRL(*this, "wxID_DISPOSITION_BOX", wxRadioBox)->SetString(2, TtaConvMessageToWX( TtaGetMessage (LIB, TMSG_4_PAGE_SHEET) ));
  XRCCTRL(*this, "wxID_DISPOSITION_BOX", wxRadioBox)->SetSelection(disposition);
  XRCCTRL(*this, "wxID_MANUAL_CHK", wxCheckBox)->SetLabel(TtaConvMessageToWX( TtaGetMessage(LIB, TMSG_MANUAL_FEED) ));
  XRCCTRL(*this, "wxID_MANUAL_CHK", wxCheckBox)->SetValue(manual_feed);
#endif /* _WINDOWS */

  // options check list
  XRCCTRL(*this, "wxID_OPTIONS_TXT", wxStaticText)->SetLabel(TtaConvMessageToWX( TtaGetMessage(LIB, TMSG_OPTIONS) ));
  XRCCTRL(*this, "wxID_TOC_CHK", wxCheckBox)->SetLabel(TtaConvMessageToWX( TtaGetMessage(AMAYA, AM_PRINT_TOC) ));
  XRCCTRL(*this, "wxID_LINKS_CHK", wxCheckBox)->SetLabel(TtaConvMessageToWX( TtaGetMessage(AMAYA, AM_NUMBERED_LINKS) ));
  XRCCTRL(*this, "wxID_PRINT_URL_CHK", wxCheckBox)->SetLabel(TtaConvMessageToWX( TtaGetMessage(AMAYA, AM_PRINT_URL) ));
  XRCCTRL(*this, "wxID_IGNORE_CSS_CHK", wxCheckBox)->SetLabel(TtaConvMessageToWX( TtaGetMessage(AMAYA, AM_WITH_CSS) ));
  XRCCTRL(*this, "wxID_TOC_CHK", wxCheckBox)->SetValue(with_toc);
  XRCCTRL(*this, "wxID_LINKS_CHK", wxCheckBox)->SetValue(with_links);
  XRCCTRL(*this, "wxID_PRINT_URL_CHK", wxCheckBox)->SetValue(with_url);
  XRCCTRL(*this, "wxID_IGNORE_CSS_CHK", wxCheckBox)->SetValue(ignore_css);

  // buttons
  XRCCTRL(*this, "wxID_OK", wxButton)->SetLabel(TtaConvMessageToWX( TtaGetMessage(LIB, TMSG_BUTTON_PRINT) ));
  XRCCTRL(*this, "wxID_CANCEL", wxButton)->SetLabel(TtaConvMessageToWX( TtaGetMessage(LIB, TMSG_CANCEL) ));

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
  // print callback
  ThotCallback (BasePrint + FormPrint, INTEGER_DATA, (char*) 1);
  TtaRedirectFocus();
}

/*----------------------------------------------------------------------
  OnCancelButton called when clicking on cancelbutton
  ----------------------------------------------------------------------*/
void PrintDlgWX::OnCancelButton( wxCommandEvent& event )
{
  ThotCallback (BasePrint + FormPrint, INTEGER_DATA, (char*) 0);
  TtaRedirectFocus();
}

/*----------------------------------------------------------------------
  OnPaperFormatBox called when clicking the Paper Format radio box
  ----------------------------------------------------------------------*/
void PrintDlgWX::OnPaperFormatBox ( wxCommandEvent& event )
{
#ifndef _WINDOWS
  ThotCallback (BasePrint + PaperFormat, INTEGER_DATA,
		(char*) (XRCCTRL(*this, "wxID_PAPER_FORMAT_BOX", wxRadioBox)->GetSelection( )));
#endif /* _WINDOWS */
}

/*----------------------------------------------------------------------------
  OnPaperOrientationBox called when clicking the Paper Orientation radio box
  ----------------------------------------------------------------------------*/
void PrintDlgWX::OnPaperOrientationBox ( wxCommandEvent& event )
{
#ifndef _WINDOWS
  ThotCallback (BasePrint + PaperOrientation, INTEGER_DATA,
		(char*) (XRCCTRL(*this, "wxID_ORIENTATION_BOX", wxRadioBox)->GetSelection( )));
#endif /* _WINDOWS */
}

/*---------------------------------------------------------------------------
  OnPaperDispositionBox called when clicking the Paper Disposition radio box
  ----------------------------------------------------------------------------*/
void PrintDlgWX::OnPaperDispositionBox ( wxCommandEvent& event )
{
#ifndef _WINDOWS
  ThotCallback (BasePrint + PPagesPerSheet, INTEGER_DATA,
		(char*) (XRCCTRL(*this, "wxID_DISPOSITION_BOX", wxRadioBox)->GetSelection( )));
#endif /* _WINDOWS */
}

/*----------------------------------------------------------------------------
  OnOutputBox called when selecting the output radio box
  ----------------------------------------------------------------------------*/
void PrintDlgWX::OnOutputBox ( wxCommandEvent& event )
{
#ifndef _WINDOWS
  int m_output;

  m_output = XRCCTRL(*this, "wxID_OUTPUT_BOX", wxRadioBox)->GetSelection( );
  ThotCallback (BasePrint + PrintSupport, INTEGER_DATA,
		(char*) m_output);
  if (m_output == 0)
    {
      if (m_print == 1)
        {
          m_print = 0;
          XRCCTRL(*this, "wxID_FILE_TXT_CTRL", wxTextCtrl)->SetValue(m_Printer);
        }
    }
  else
    {
      if (m_print == 0)
        {
          m_print = 1;
          XRCCTRL(*this, "wxID_FILE_TXT_CTRL", wxTextCtrl)->SetValue(m_PS);
        }
    }
#endif /* _WINDOWS */
}
  
/*---------------------------------------------------------------
  OnManualChkBox 
  ---------------------------------------------------------------*/
void PrintDlgWX::OnManualChkBox ( wxCommandEvent& event )
{ 
  wxLogDebug( _T("PrintDlgWX::OnManualChkBox") );
  ThotCallback (BasePrint + PrintOptions, INTEGER_DATA, (char*) 0);
} 

/*---------------------------------------------------------------
  OnTocChkBox
  ---------------------------------------------------------------*/
void PrintDlgWX::OnTocChkBox ( wxCommandEvent& event )
{
  ThotCallback (BasePrint + PrintOptions, INTEGER_DATA,	(char*) 1);
}

/*---------------------------------------------------------------
  OnLinksChkBox
  ---------------------------------------------------------------*/
void PrintDlgWX::OnLinksChkBox ( wxCommandEvent& event )
{
  ThotCallback (BasePrint + PrintOptions, INTEGER_DATA,	(char*) 2);
}

/*---------------------------------------------------------------
  OnPrintUrlChkBox
  ---------------------------------------------------------------*/
void PrintDlgWX::OnPrintUrlChkBox ( wxCommandEvent& event )
{
  ThotCallback (BasePrint + PrintOptions, INTEGER_DATA,	(char*) 3);
}

/*---------------------------------------------------------------
  OnIgnoreCssChkBox
  ---------------------------------------------------------------*/
void PrintDlgWX::OnIgnoreCssChkBox ( wxCommandEvent& event )
{
  ThotCallback (BasePrint + PrintOptions, INTEGER_DATA,	(char*) 4);
}

/*---------------------------------------------------------------
  OnTypePrinter Set the printer name
  ---------------------------------------------------------------*/
void PrintDlgWX::OnTypePrinter ( wxCommandEvent& event )
{
#ifndef _WINDOWS
  wxString printer_name = XRCCTRL(*this, "wxID_FILE_TXT_CTRL", wxTextCtrl)->GetValue( );
  if (m_print == 0)
    m_Printer = printer_name;
  else
    m_PS = printer_name;

  // allocate a temporary buffer to convert wxString to (char *) UTF-8 buffer
  char buffer[512];
  wxASSERT( printer_name.Len() < 512 );
  strcpy( buffer, (const char*)printer_name.mb_str(wxConvUTF8) );

  ThotCallback (BasePrint + PPrinterName,  STRING_DATA, (char *)buffer );
#endif /* _WINDOWS */
}

#endif /* _WX */
