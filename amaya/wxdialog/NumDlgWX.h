#ifdef _WX

#ifndef __NUMDLGWX_H__
#define __NUMDLGWX_H__

//-----------------------------------------------------------------------------
// Headers
//-----------------------------------------------------------------------------

#include "AmayaDialog.h"

//-----------------------------------------------------------------------------
// Class definition: NumDlgWX
//-----------------------------------------------------------------------------

class NumDlgWX : public AmayaDialog
{

public: 
    
  // Constructor.
  NumDlgWX( int ref, int subref,
	     wxWindow* parent,
	     const wxString & title,
	     const wxString & label,
	     int value );
    
  // Destructor.                  
  virtual ~NumDlgWX();

private:

  void OnOk( wxCommandEvent& event );
  void OnCancel( wxCommandEvent& event );

  // Any class wishing to process wxWindows events must use this macro
  DECLARE_EVENT_TABLE()

private:
  int m_SubRef;
};

#endif  // __NUMDLGWX_H__

#endif /* _WX */
