#ifdef _WX

#ifndef __SELECTOPERATORDLGWX_H__
#define __SELECTOPERATORDLGWX_H__

//-----------------------------------------------------------------------------
// Headers
//-----------------------------------------------------------------------------

#include "AmayaDialog.h"

//-----------------------------------------------------------------------------
// Class definition: SelectOperatorDlgWX
//-----------------------------------------------------------------------------

class SelectOperatorDlgWX : public AmayaDialog
{

public:    
  // Constructor.
  SelectOperatorDlgWX( int ref, wxWindow* parent);
  virtual ~SelectOperatorDlgWX();

private:
    // Override base class functions of a wxDialog.
  void OnOperator0( wxCommandEvent& event );
  void OnOperator1( wxCommandEvent& event );
  void OnOperator2( wxCommandEvent& event );
  void OnClose( wxCloseEvent& event);

  // Any class wishing to process wxWindows events must use this macro
  DECLARE_EVENT_TABLE()
    
private:
  wxString m_Label;
  wxString m_Title;
};
  

#endif  //__SELECTOPERATORDLGWX_H__

#endif /* _WX */
