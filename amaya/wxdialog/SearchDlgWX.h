#ifdef _WX

#ifndef __SEARCHDLGWX_H__
#define __SEARCHDLGWX_H__

//-----------------------------------------------------------------------------
// Headers
//-----------------------------------------------------------------------------

#include "wx/dialog.h"

//-----------------------------------------------------------------------------
// Class definition: TitleDlgWX
//-----------------------------------------------------------------------------

class SearchDlgWX : public wxDialog
{

public: 
    
  // Constructor.
  SearchDlgWX( wxWindow* parent,
	       const wxString & titlecaption );
  
  // Destructor.                  
  ~SearchDlgWX();

private:
    // Override base class functions of a wxDialog.
  void OnConfirmButton( wxCommandEvent& event );
  void OnCancelButton( wxCommandEvent& event );
  void OnNoReplaceButton( wxCommandEvent& event );

  // Any class wishing to process wxWindows events must use this macro
  DECLARE_EVENT_TABLE()
    
private:
};

#endif  //__SEARCHDLGWX_H__

#endif /* _WX */
