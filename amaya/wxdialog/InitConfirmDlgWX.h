#ifdef _WX

#ifndef __INITCONFIRMDLGWX_H__
#define __INITCONFIRMDLGWX_H__

//-----------------------------------------------------------------------------
// Headers
//-----------------------------------------------------------------------------

#include "wx/dialog.h"

//-----------------------------------------------------------------------------
// Class definition: InitConfirmDlgWX
//-----------------------------------------------------------------------------

class InitConfirmDlgWX : public wxDialog
{

public: 
    
  // Constructor.
  InitConfirmDlgWX( wxWindow* parent,
		    const wxString & title,
		    const wxString & extrabutton,
		    const wxString & confirmbutton,
		    const wxString & label );
    
  // Destructor.                  
  ~InitConfirmDlgWX();

private:
    // Override base class functions of a wxDialog.
  void OnExtraButton( wxCommandEvent& event );
  void OnConfirmButton( wxCommandEvent& event );
  void OnCancelButton( wxCommandEvent& event );

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
