#ifdef _WX

#ifndef __PRINTDLGWX_H__
#define __PRINTDLGWX_H__

//-----------------------------------------------------------------------------
// Headers
//-----------------------------------------------------------------------------

#include "AmayaDialog.h"

//-----------------------------------------------------------------------------
// Class definition: PrintDlgWX
//-----------------------------------------------------------------------------

class PrintDlgWX : public AmayaDialog
{
public: 
    
  // Constructor.
  PrintDlgWX( int ref,
	      wxWindow* parent,
	      const wxString & printer_file,
	      const wxString & ps_file,
	      int paper_format,
	      int orientation,
	      int disposition,
	      int paper_print,
	      bool manual_feed,
	      bool with_toc,
	      bool with_links,
	      bool with_url,
	      bool ignore_css );
  // Destructor.                  
  virtual ~PrintDlgWX();

private:
    // Override base class functions of a wxDialog.
  void OnPrintButton( wxCommandEvent& event );
  void OnCancelButton( wxCommandEvent& event );
  void OnPaperFormatBox( wxCommandEvent& event );
  void OnPaperOrientationBox( wxCommandEvent& event );
  void OnPaperDispositionBox( wxCommandEvent& event );
  void OnOutputBox( wxCommandEvent& event );
  void OnManualChkBox( wxCommandEvent& event );
  void OnTocChkBox( wxCommandEvent& event );
  void OnLinksChkBox( wxCommandEvent& event );
  void OnPrintUrlChkBox( wxCommandEvent& event );
  void OnIgnoreCssChkBox( wxCommandEvent& event );
  void OnTypePrinter ( wxCommandEvent& event );
  // Any class wishing to process wxWindows events must use this macro
  DECLARE_EVENT_TABLE()
    
private:
  wxString   m_Printer;
  wxString   m_PS;
  int        m_print;
};

#endif  //__PRINTDLGWX_H__

#endif /* _WX */
