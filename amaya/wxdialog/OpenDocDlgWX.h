#ifdef _WX

#ifndef __OPENDOCDLGWX_H__
#define __OPENDOCDLGWX_H__

//-----------------------------------------------------------------------------
// Headers
//-----------------------------------------------------------------------------

#include "wx/dialog.h"

//-----------------------------------------------------------------------------
// Class definition: OpenDocDlgWX
//-----------------------------------------------------------------------------

class OpenDocDlgWX : public wxDialog
{

public: 
    
  // Constructor.
  OpenDocDlgWX( wxWindow* parent,
		const wxString & title,
		const wxString & docName );
    
  // Destructor.                  
  ~OpenDocDlgWX();

  void AppendURL( const wxString & url );
  void SetCurrentURL( const wxString & url );

private:

  void OnOpenButton( wxCommandEvent& event );
  void OnBrowseButton( wxCommandEvent& event );
  void OnCancelButton( wxCommandEvent& event );

  
  // Any class wishing to process wxWindows events must use this macro
  DECLARE_EVENT_TABLE()
    
private:
};

#endif  //__OPENDOCDLGWX_H__

#endif /* _WX */
