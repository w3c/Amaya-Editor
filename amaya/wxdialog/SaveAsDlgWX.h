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
               const wxString & pathname,
               int doc,
               bool saveImgs);
  
  // Destructor.                  
  virtual ~SaveAsDlgWX();

private:
    // Override base class functions of a wxDialog.
  void OnConfirmButton( wxCommandEvent& event );
  void OnCancelButton( wxCommandEvent& event );
  void OnBrowseButton( wxCommandEvent& event );
  void OnDirButton( wxCommandEvent& event );
  void OnClearButton( wxCommandEvent& event );
  void OnCharsetCbx( wxCommandEvent& event );
  void OnMimeTypeCbx( wxCommandEvent& event );
  void OnDocFormatBox( wxCommandEvent& event );
  void OnImagesChkBox( wxCommandEvent& event );
  void OnUrlsChkBox( wxCommandEvent& event );
  void OnDocLocation( wxCommandEvent& event );
  void OnImgLocation( wxCommandEvent& event );
  // Any class wishing to process wxWindows events must use this macro
  DECLARE_EVENT_TABLE()
    
private:
};

#endif  //__SAVEASDLGWX_H__

#endif /* _WX */
