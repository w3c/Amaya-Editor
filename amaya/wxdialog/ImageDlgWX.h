#ifdef _WX

#ifndef __IMAGEDLGWX_H__
#define __IMAGEDLGWX_H__

//-----------------------------------------------------------------------------
// Headers
//-----------------------------------------------------------------------------

#include "AmayaDialog.h"

//-----------------------------------------------------------------------------
// Class definition: ImageDlgWX
//-----------------------------------------------------------------------------

class ImageDlgWX : public AmayaDialog
{

public: 
    
  // Constructor.
  ImageDlgWX( int ref,
              wxWindow* parent,
              const wxString & title,
              const wxString & urlToOpen,
              const wxString & alt,
              const wxString & filter,
              int *p_last_used_filter, bool isSvg );
    
  // Destructor.                  
  virtual ~ImageDlgWX();

private:

  void OnClearButton( wxCommandEvent& event );
  void OnOpenButton( wxCommandEvent& event );
  void OnBrowseButton( wxCommandEvent& event );
  void OnCancelButton( wxCommandEvent& event );
  void OnPosition( wxCommandEvent& event );

  // Any class wishing to process wxWindows events must use this macro
  DECLARE_EVENT_TABLE()

private:
  wxString     m_Filter;
  int         *m_pLastUsedFilter;

};

#endif  //__IMAGEDLGWX_H__

#endif /* _WX */
