#ifdef _WX

#ifndef __SELECTINTEGRALDLGWX_H__
#define __SELECTINTEGRALDLGWX_H__

//-----------------------------------------------------------------------------
// Headers
//-----------------------------------------------------------------------------

#include "AmayaDialog.h"

//-----------------------------------------------------------------------------
// Class definition: SelectIntegralDlgWX
//-----------------------------------------------------------------------------

class SelectIntegralDlgWX : public AmayaDialog
{

public:    
  // Constructor.
  SelectIntegralDlgWX( int ref,
                       wxWindow* parent);
  virtual ~SelectIntegralDlgWX();

private:
    // Override base class functions of a wxDialog.
  void OnInsert( wxCommandEvent& event );
  void OnClose( wxCloseEvent& event);
  void OnCancel( wxCommandEvent& event );
  // Any class wishing to process wxWindows events must use this macro
  DECLARE_EVENT_TABLE()
    
private:
  wxString m_Label;
  wxString m_Title;
};
  

#endif  //__SELECTINTEGRALDLGWX_H__

#endif /* _WX */
