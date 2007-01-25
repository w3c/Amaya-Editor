#ifdef _WX

#ifndef __SELECTFENCEATTRIBUTESDLGWX_H__
#define __SELECTFENCEATTRIBUTESDLGWX_H__

//-----------------------------------------------------------------------------
// Headers
//-----------------------------------------------------------------------------

#include "AmayaDialog.h"

//-----------------------------------------------------------------------------
// Class definition: SelectFenceAttributesDlgWX
//-----------------------------------------------------------------------------

class SelectFenceAttributesDlgWX : public AmayaDialog
{

public:    
  // Constructor.
  SelectFenceAttributesDlgWX( int ref,
                       wxWindow* parent);
  virtual ~SelectFenceAttributesDlgWX();

private:
    // Override base class functions of a wxDialog.
  void OnOpenSelected( wxCommandEvent& event );
  void OnSeparatorsSelected( wxCommandEvent& event );
  void OnCloseSelected( wxCommandEvent& event );
  void OnInsert( wxCommandEvent& event );

  // Any class wishing to process wxWindows events must use this macro
  DECLARE_EVENT_TABLE()
    
private:
  wxString m_Label;
  wxString m_Title;
};
  

#endif  //__SELECTFENCEATTRIBUTESDLGWX_H__

#endif /* _WX */
