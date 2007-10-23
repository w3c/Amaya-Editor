#ifdef _WX

#ifndef __HREFDLGWX_H__
#define __HREFDLGWX_H__

//-----------------------------------------------------------------------------
// Headers
//-----------------------------------------------------------------------------

#include "AmayaDialog.h"

//-----------------------------------------------------------------------------
// Class definition: HRefDlgWX
//-----------------------------------------------------------------------------

class HRefDlgWX : public AmayaDialog
{

public: 
    
  // Constructor.
  HRefDlgWX( int ref,
             wxWindow* parent,
             const wxString & title,
             const wxArrayString & url_list,
             const wxString & wx_init_value,
             const wxString & filter,
             int *p_last_used_filter,
             int doc);
    
  // Destructor.                  
  virtual ~HRefDlgWX();

private:
  void OnURLSelected( wxCommandEvent& event );
  void OnOk( wxCommandEvent& event );
  void OnBrowse( wxCommandEvent& event );
  void OnClick( wxCommandEvent& event );
  void OnClear( wxCommandEvent& event );
  void OnDelete( wxCommandEvent& event );
  void OnCancel( wxCommandEvent& event );

  // Any class wishing to process wxWindows events must use this macro
  DECLARE_EVENT_TABLE()

private:
  wxString m_Filter;
  int *    m_pLastUsedFilter;

};

#endif  // __HREFDLGWX_H__

#endif /* _WX */
