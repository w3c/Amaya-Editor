#ifdef _WX

#ifndef __TitleDLGWX_H__
#define __TitleDLGWX_H__

//-----------------------------------------------------------------------------
// Headers
//-----------------------------------------------------------------------------

#include "AmayaDialog.h"

//-----------------------------------------------------------------------------
// Class definition: TitleDlgWX
//-----------------------------------------------------------------------------

class TitleDlgWX : public AmayaDialog
{

public: 
    
  // Constructor.
  TitleDlgWX( int ref,
	      wxWindow* parent,
	      const wxString & title,
	      const wxString & doc_title );
  
  // Destructor.                  
  virtual ~TitleDlgWX();

private:
    // Override base class functions of a wxDialog.
  void OnConfirmButton( wxCommandEvent& event );
  void OnCancelButton( wxCommandEvent& event );

  // Any class wishing to process wxWindows events must use this macro
  DECLARE_EVENT_TABLE()
};

#endif  //__TITLEDLGWX_H__

#endif /* _WX */
