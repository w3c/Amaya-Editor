#ifdef _WX

#ifndef __AUTHENTDLGWX_H__
#define __AUTHENTDLGWX_H__

//-----------------------------------------------------------------------------
// Headers
//-----------------------------------------------------------------------------

#include "AmayaDialog.h"

//-----------------------------------------------------------------------------
// Class definition: AuthentDlgWX
//-----------------------------------------------------------------------------

class AuthentDlgWX : public AmayaDialog
{
public: 
    
  // Constructor.
  AuthentDlgWX( int ref,
	       wxWindow* parent,
	       char *realm,
	       char *server,
	       char *name,
	       char *pwd);
  
  // Destructor.                  
  virtual ~AuthentDlgWX();

private:
    // Override base class functions of a wxDialog.
  void OnConfirmButton( wxCommandEvent& event );
  void OnCancelButton( wxCommandEvent& event );
  void OnSavePasswordButton( wxCommandEvent& event );
  void OnName( wxCommandEvent& event );
  void OnPassword( wxCommandEvent& event );
  // Any class wishing to process wxWindows events must use this macro
  DECLARE_EVENT_TABLE()
    
private:
};

#endif  //__AUTHENTDLGWX_H__

#endif /* _WX */
