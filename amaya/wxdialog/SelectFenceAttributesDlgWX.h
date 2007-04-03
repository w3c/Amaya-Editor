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
  void OnInsert( wxCommandEvent& event );
  void OnCancel( wxCommandEvent& event );
  void OnClose( wxCloseEvent& event);

  // Any class wishing to process wxWindows events must use this macro
  DECLARE_EVENT_TABLE()
    
private:
  wxString m_Label;
  wxString m_Title;
};
  

#endif  //__SELECTFENCEATTRIBUTESDLGWX_H__

#endif /* _WX */
