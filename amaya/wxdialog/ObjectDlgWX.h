#ifdef _WX

#ifndef __OBJECTDLGWX_H__
#define __OBJECTDLGWX_H__

//-----------------------------------------------------------------------------
// Headers
//-----------------------------------------------------------------------------

#include "AmayaDialog.h"

//-----------------------------------------------------------------------------
// Class definition: ObjectDlgWX
//-----------------------------------------------------------------------------

class ObjectDlgWX : public AmayaDialog
{

public: 
    
  // Constructor.
  ObjectDlgWX( int ref,
               wxWindow* parent,
               const wxString & title,
               const wxString & urlToOpen,
               const wxString & type,
               const wxString & filter,
               int *p_last_used_filter);
    
  // Destructor.                  
  virtual ~ObjectDlgWX();

private:

  void OnMimeTypeCbx( wxCommandEvent& event );
  void OnClearButton( wxCommandEvent& event );
  void OnOpenButton( wxCommandEvent& event );
  void OnBrowseButton( wxCommandEvent& event );
  void OnCancelButton( wxCommandEvent& event );
  void OnPosition( wxCommandEvent& event );

  // Any class wishing to process wxWindows events must use this macro
  DECLARE_EVENT_TABLE()

private:
  wxString m_Filter;
  int *    m_pLastUsedFilter;

};

#endif  //__OBJECTDLGWX_H__

#endif /* _WX */
