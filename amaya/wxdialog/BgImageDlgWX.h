#ifdef _WX

#ifndef __BGIMAGEDLGWX_H__
#define __BGIMAGEDLGWX_H__

//-----------------------------------------------------------------------------
// Headers
//-----------------------------------------------------------------------------

#include "AmayaDialog.h"

//-----------------------------------------------------------------------------
// Class definition: BgImageDlgWX
//-----------------------------------------------------------------------------

class BgImageDlgWX : public AmayaDialog
{

public: 
    
  // Constructor.
  BgImageDlgWX( int ref, wxWindow* parent, const wxString & urlToOpen, int RepeatValue );
    
  // Destructor.                  
  virtual ~BgImageDlgWX();

 protected:
  void RedrawRepeatButtons();
  
 protected:
  // Any class wishing to process wxWindows events must use this macro
  DECLARE_EVENT_TABLE()
  void OnOpenButton( wxCommandEvent& event );
  void OnClearButton( wxCommandEvent& event );
  void OnBrowseButton( wxCommandEvent& event );
  void OnCancelButton( wxCommandEvent& event );
  void OnRepeatButton( wxCommandEvent& event );
  void OnRepeatXButton( wxCommandEvent& event );
  void OnRepeatYButton( wxCommandEvent& event );
  void OnNoRepeatButton( wxCommandEvent& event );

 protected:
  int m_RepeatMode;
  wxColour m_OffColour;
  wxColour m_OnColour;
};

#endif  //__BGIMAGEDLGWX_H__

#endif /* _WX */
