#ifdef _WX

#ifndef __CSSDLGWX_H__
#define __CSSDLGWX_H__

//-----------------------------------------------------------------------------
// Headers
//-----------------------------------------------------------------------------

#include "AmayaDialog.h"

//-----------------------------------------------------------------------------
// Class definition: CSSDlgWX
//-----------------------------------------------------------------------------

class CSSDlgWX : public AmayaDialog
{

public: 
    
  // Constructor.
  CSSDlgWX( int ref,
	    wxWindow* parent,
	    const wxString & title,
	    const wxArrayString& items );
    
  // Destructor.                  
  virtual ~CSSDlgWX();

private:

  void OnOkButton( wxCommandEvent& event );
  void OnCancelButton( wxCommandEvent& event );

  // Any class wishing to process wxWindows events must use this macro
  DECLARE_EVENT_TABLE()

private:
  wxString m_Filter;

};

#endif  //__CSSDLGWX_H__

#endif /* _WX */
