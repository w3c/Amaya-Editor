#ifdef _WX

#ifndef __LISTDLGWX_H__
#define __LISTDLGWX_H__

//-----------------------------------------------------------------------------
// Headers
//-----------------------------------------------------------------------------
#include "AmayaDialog.h"

//-----------------------------------------------------------------------------
// Class definition: ListDlgWX
//-----------------------------------------------------------------------------
class ListDlgWX : public AmayaDialog
{
public: 
  // Constructor.
  ListDlgWX( int ref,
	    wxWindow* parent,
	    const wxString & title,
	    const wxArrayString& items );
    
  // Destructor.                  
  virtual ~ListDlgWX();

private:
  void OnOkButton( wxCommandEvent& event );
  void OnCancelButton( wxCommandEvent& event );

  // Any class wishing to process wxWindows events must use this macro
  DECLARE_EVENT_TABLE()

private:
  wxString m_Filter;

};

#endif  //__LISTDLGWX_H__

#endif /* _WX */
