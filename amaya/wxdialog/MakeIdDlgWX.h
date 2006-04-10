#ifdef _WX

#ifndef __MAKEIDDLGWX_H__
#define __MAKEIDDLGWX_H__

//-----------------------------------------------------------------------------
// Headers
//-----------------------------------------------------------------------------

#include "AmayaDialog.h"

//-----------------------------------------------------------------------------
// Class definition: MakeIdDlgWX
//-----------------------------------------------------------------------------

class MakeIdDlgWX : public AmayaDialog
{

public: 
    
  // Constructor.
  MakeIdDlgWX( int ref, wxWindow* parent );
    
  // Destructor.                  
  virtual ~MakeIdDlgWX();

private:
  // Any class wishing to process wxWindows events must use this macro
  DECLARE_EVENT_TABLE()
  void OnAddButton( wxCommandEvent& event );
  void OnRemoveButton( wxCommandEvent& event );
  void OnCancelButton( wxCommandEvent& event );
};

#endif  //__MAKEIDDLGWX_H__

#endif /* _WX */
