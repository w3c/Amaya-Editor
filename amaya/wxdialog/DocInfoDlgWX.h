#ifdef _WX

#ifndef __DOCINFODLGWX_H__
#define __DOCINFODLGWX_H__

//-----------------------------------------------------------------------------
// Headers
//-----------------------------------------------------------------------------

#include "AmayaDialog.h"

//-----------------------------------------------------------------------------
// Class definition: DocInfoDlgWX
//-----------------------------------------------------------------------------

class DocInfoDlgWX : public AmayaDialog
{
public: 
    
  // Constructor.
  DocInfoDlgWX( int ref, 
		wxWindow* parent,
		int doc);
  
  // Destructor.                  
  virtual ~DocInfoDlgWX();

private:
    // Override base class functions of a wxDialog.
  void OnDoneButton( wxCommandEvent& event );

  // Any class wishing to process wxWindows events must use this macro
  DECLARE_EVENT_TABLE()
    
private:
};

#endif  //__DOCINFODLGWX_H__

#endif /* _WX */
