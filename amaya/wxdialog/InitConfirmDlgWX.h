#ifdef _WX

#ifndef __INITCONFIRMDLGWX_H__
#define __INITCONFIRMDLGWX_H__

//-----------------------------------------------------------------------------
// Headers
//-----------------------------------------------------------------------------

#include "AmayaDialog.h"

//-----------------------------------------------------------------------------
// Class definition: InitConfirmDlgWX
//-----------------------------------------------------------------------------

class InitConfirmDlgWX : public AmayaDialog
{

public:    
  // Constructor.
  InitConfirmDlgWX( int ref,
		    wxWindow* parent,
		    const wxString & title,
		    const wxString & extrabutton,
		    const wxString & confirmbutton,
		    const wxString & label,
		    const wxString & label2,
		    const wxString & label3 );
  virtual ~InitConfirmDlgWX();

private:
    // Override base class functions of a wxDialog.
  void OnExtraButton( wxCommandEvent& event );
  void OnConfirmButton( wxCommandEvent& event );
  void OnCancelButton( wxCommandEvent& event );
  void OnClose( wxCloseEvent& event);

  // Any class wishing to process wxWindows events must use this macro
  DECLARE_EVENT_TABLE()
    
private:
  wxString m_Label;
  wxString m_Title;
  wxString m_ExtraButton;
  wxString m_ConfirmButton;
  wxString m_CancelButton;
};

#endif  //__INITCONFIRMDLGWX_H__

#endif /* _WX */
