#ifdef _WX

#ifndef __PRINTDLGWX_H__
#define __PRINTDLGWX_H__

//-----------------------------------------------------------------------------
// Headers
//-----------------------------------------------------------------------------

#include "wx/dialog.h"

//-----------------------------------------------------------------------------
// Class definition: PrintDlgWX
//-----------------------------------------------------------------------------

class PrintDlgWX : public wxDialog
{
public: 
    
  // Constructor.
  PrintDlgWX( wxWindow* parent,
	      const wxString & ps_file );
  
  // Destructor.                  
  ~PrintDlgWX();

private:
    // Override base class functions of a wxDialog.
  void OnPrintButton( wxCommandEvent& event );
  void OnCancelButton( wxCommandEvent& event );

  // Any class wishing to process wxWindows events must use this macro
  DECLARE_EVENT_TABLE()
    
private:
  int   m_iarea;
  int   m_ireplace;
  bool  m_case;
};

#endif  //__PRINTDLGWX_H__

#endif /* _WX */
