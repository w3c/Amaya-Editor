#ifdef _WX

#ifndef __OPENDOCDLGWX_H__
#define __OPENDOCDLGWX_H__

//-----------------------------------------------------------------------------
// Headers
//-----------------------------------------------------------------------------

#include "AmayaDialog.h"

//-----------------------------------------------------------------------------
// Class definition: OpenDocDlgWX
//-----------------------------------------------------------------------------

class OpenDocDlgWX : public AmayaDialog
{

public: 
    
  // Constructor.
  OpenDocDlgWX( int ref, 
		wxWindow* parent,
		const wxString & title,
		const wxString & docName,
		const wxString & filtre );
    
  // Destructor.                  
  virtual ~OpenDocDlgWX();

  void AppendURL( const wxString & url );
  void SetCurrentURL( const wxString & url );

private:

  void OnOpenButton( wxCommandEvent& event );
  void OnBrowseButton( wxCommandEvent& event );
  void OnCancelButton( wxCommandEvent& event );

  // Any class wishing to process wxWindows events must use this macro
  DECLARE_EVENT_TABLE()

private:
  wxString m_Filter;

};

#endif  //__OPENDOCDLGWX_H__

#endif /* _WX */
