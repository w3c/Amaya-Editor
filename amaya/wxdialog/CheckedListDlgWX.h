#ifdef _WX

#ifndef __CHECKEDLISTDLGWX_H__
#define __CHECKEDLISTDLGWX_H__

//-----------------------------------------------------------------------------
// Headers
//-----------------------------------------------------------------------------

#include "AmayaDialog.h"

//-----------------------------------------------------------------------------
// Class definition: CheckedListDlgWX
//-----------------------------------------------------------------------------

class CheckedListDlgWX : public AmayaDialog
{
public: 
  // Constructor.
  CheckedListDlgWX( int ref,
		    wxWindow* parent,
		    const wxString & title,
		    int nb,
		    const wxArrayString& items,
		    bool *checks);
  
  // Destructor.                  
  virtual ~CheckedListDlgWX();

private:
    // Override base class functions of a wxDialog.
  void OnConfirmButton( wxCommandEvent& event );
  void OnCancelButton( wxCommandEvent& event );
  // Any class wishing to process wxWindows events must use this macro
  DECLARE_EVENT_TABLE()
    
private:
};

#endif  //__CHECKEDLISTDLGWX_H__

#endif /* _WX */
