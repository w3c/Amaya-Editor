#ifdef _WX

#ifndef __HREFDLGWX_H__
#define __HREFDLGWX_H__

//-----------------------------------------------------------------------------
// Headers
//-----------------------------------------------------------------------------

#include "AmayaDialog.h"

//-----------------------------------------------------------------------------
// Class definition: HRefDlgWX
//-----------------------------------------------------------------------------

class HRefDlgWX : public AmayaDialog
{

public: 
    
  // Constructor.
  HRefDlgWX( int ref,
	     wxWindow* parent,
	     const wxArrayString & url_list,
	     const wxString & wx_init_value,
	     const wxString & title,
	     const wxString & filter );
    
  // Destructor.                  
  virtual ~HRefDlgWX();

private:

  void OnOk( wxCommandEvent& event );
  void OnBrowse( wxCommandEvent& event );
  void OnClick( wxCommandEvent& event );
  void OnClear( wxCommandEvent& event );
  void OnCancel( wxCommandEvent& event );

  // Any class wishing to process wxWindows events must use this macro
  DECLARE_EVENT_TABLE()

private:
  wxString m_Filter;

};

#endif  // __HREFDLGWX_H__

#endif /* _WX */
