#ifdef _WX

#ifndef __TEXTDLGWX_H__
#define __TEXTDLGWX_H__

//-----------------------------------------------------------------------------
// Headers
//-----------------------------------------------------------------------------

#include "AmayaDialog.h"

//-----------------------------------------------------------------------------
// Class definition: TextDlgWX
//-----------------------------------------------------------------------------

class TextDlgWX : public AmayaDialog
{

public: 
    
  // Constructor.
  TextDlgWX( int ref, int subref,
	     wxWindow* parent,
	     const wxString & title,
	     const wxString & label,
	     const wxString & value );
    
  // Destructor.                  
  virtual ~TextDlgWX();

private:

  void OnOk( wxCommandEvent& event );
  void OnCancel( wxCommandEvent& event );

  // Any class wishing to process wxWindows events must use this macro
  DECLARE_EVENT_TABLE()

private:
  wxString m_Filter;

};

#endif  // __TEXTDLGWX_H__

#endif /* _WX */
