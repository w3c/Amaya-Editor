#ifdef _WX

#ifndef __MetaDLGWX_H__
#define __MetaDLGWX_H__

//-----------------------------------------------------------------------------
// Headers
//-----------------------------------------------------------------------------

#include "AmayaDialog.h"

//-----------------------------------------------------------------------------
// Class definition: MetaDlgWX
//-----------------------------------------------------------------------------

class MetaDlgWX : public AmayaDialog
{

public: 
    
  // Constructor.
  MetaDlgWX( int ref, wxWindow* parent );
  // Destructor.                  
  virtual ~MetaDlgWX();

private:
    // Override base class functions of a wxDialog.
  void OnConfirmButton( wxCommandEvent& event );
  void OnCancelButton( wxCommandEvent& event );

  // Any class wishing to process wxWindows events must use this macro
  DECLARE_EVENT_TABLE()
};

#endif  //__TITLEDLGWX_H__

#endif /* _WX */
