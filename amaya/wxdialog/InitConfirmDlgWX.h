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
    InitConfirmDlgWX( wxWindow* parent, const wxString & title, const wxString & label );
    
    // Destructor.                  
    ~InitConfirmDlgWX();

private:
    // Override base class functions of a wxDialog.
    void OnOK( wxCommandEvent &event );
    void OnCancel( wxCommandEvent& event );

    void OnInit( wxInitDialogEvent& event );
    void OnSize( wxSizeEvent& event );

    // Any class wishing to process wxWindows events must use this macro
    DECLARE_EVENT_TABLE()

private:
      wxString m_Label;
      wxString m_Title;
};

#endif  //__INITCONFIRMDLGWX_H__

#endif /* _WX */
