#ifdef _WX

#ifndef __CREATETABLEDLGWX_H__
#define __CREATETABLEDLGWX_H__

//-----------------------------------------------------------------------------
// Headers
//-----------------------------------------------------------------------------

#include "AmayaDialog.h"

//-----------------------------------------------------------------------------
// Class definition: CreateTableDlgWX
//-----------------------------------------------------------------------------

class CreateTableDlgWX : public AmayaDialog
{
public: 
    
  // Constructor.
  CreateTableDlgWX( int ref, 
		    wxWindow* parent,
		    int def_cols,
		    int def_raws,
		    int def_border,
		    const wxString & titlecaption );
  
  // Destructor.                  
  virtual ~CreateTableDlgWX();

private:
    // Override base class functions of a wxDialog.
  void OnConfirmButton( wxCommandEvent& event );
  void OnCancelButton( wxCommandEvent& event );
  void OnSelectFormat( wxCommandEvent& event );

  // Any class wishing to process wxWindows events must use this macro
  DECLARE_EVENT_TABLE()
    
};

#endif  //__CREATETABLEDLGWX_H__

#endif /* _WX */
