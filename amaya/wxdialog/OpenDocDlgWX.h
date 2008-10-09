#ifdef _WX

#ifndef __OPENDOCDLGWX_H__
#define __OPENDOCDLGWX_H__

//-----------------------------------------------------------------------------
// Headers
//-----------------------------------------------------------------------------

#include "AmayaDialog.h"

//-----------------------------------------------------------------------------
// Class definition: OpenDocDlgWX
//-----------------------------------------------------------------------------

class OpenDocDlgWX : public AmayaDialog
{

public: 
    
  // Constructor.
  OpenDocDlgWX( int ref,
                wxWindow* parent,
                const wxString & title,
                const wxArrayString & urlList,
                const wxString & urlToOpen,
                const wxString & filter,
                int * p_last_used_filter,
                const wxString & profiles,
                int doc, ThotBool newfile);
    
  // Destructor.                  
  virtual ~OpenDocDlgWX();
  
  bool TemplateUsed()const;
  void UseTemplate(bool use);

private:
  // Any class wishing to process wxWindows events must use this macro
  DECLARE_EVENT_TABLE()
  void UpdateTemplateList ();
  void OnOpenWithTemplate( wxCommandEvent& event );
  void OnOpenButton( wxCommandEvent& event );
  void OnClearButton( wxCommandEvent& event );
  void OnCancelButton( wxCommandEvent& event );
  void OnDirButton( wxCommandEvent& event );
  void OnFilenameButton( wxCommandEvent& event );
  void OnURLSelected( wxCommandEvent& event );
  void OnProfileSelected( wxCommandEvent& event );
  void OnUseTemplate( wxCommandEvent& event );
  void OnTemplateSelected( wxCommandEvent& event );
  void OnTemplateButton( wxCommandEvent& event );
  
 private:
  wxString m_Filter;
  bool     m_LockUpdateFlag;
  int *    m_pLastUsedFilter;
  wxString m_DirSep;
};

#endif  //__OPENDOCDLGWX_H__

#endif /* _WX */
