#ifdef _WX

#ifndef __SAVEASDLGWX_H__
#define __SAVEASDLGWX_H__

//-----------------------------------------------------------------------------
// Headers
//-----------------------------------------------------------------------------

#include "AmayaDialog.h"

//-----------------------------------------------------------------------------
// Class definition: SaveAsDlgWX
//-----------------------------------------------------------------------------

class SaveAsDlgWX : public AmayaDialog
{
public: 
    
  // Constructor.
  SaveAsDlgWX( int ref,
	       wxWindow* parent,
	       const wxString & pathname );
  
  // Destructor.                  
  virtual ~SaveAsDlgWX();

private:
    // Override base class functions of a wxDialog.
  void OnConfirmButton( wxCommandEvent& event );
  void OnCancelButton( wxCommandEvent& event );
  void OnBrowseButton( wxCommandEvent& event );
  void OnClearButton( wxCommandEvent& event );
  void OnEncodingButton( wxCommandEvent& event );
  void OnMimeTypeButton( wxCommandEvent& event );
  void OnDocFormatBox( wxCommandEvent& event );
  void OnImagesChkBox( wxCommandEvent& event );
  void OnUrlsChkBox( wxCommandEvent& event );
  void OnDocLocation( wxCommandEvent& event );
  void OnImgLocation( wxCommandEvent& event );
  // Any class wishing to process wxWindows events must use this macro
  DECLARE_EVENT_TABLE()
    
private:
  wxString   m_Printer;
  wxString   m_PS;
  int        m_print;
};

#endif  //__SAVEASDLGWX_H__

#endif /* _WX */
