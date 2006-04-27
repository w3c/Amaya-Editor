#ifdef _WX

#ifndef __NEWTEMPLATEDOCDLGWX_H__
#define __NEWTEMPLATEDOCDLGWX_H__

//-----------------------------------------------------------------------------
// Headers
//-----------------------------------------------------------------------------

#include "AmayaDialog.h"

//-----------------------------------------------------------------------------
// Class definition: NewTemplateDocDlgWX
//-----------------------------------------------------------------------------

class NewTemplateDocDlgWX : public AmayaDialog
{

public: 
    
  // Constructor.
  NewTemplateDocDlgWX( int ref,
		       wxWindow* parent,
		       int doc,
		       const wxString & title,
		       const wxString & templateDir,
		       const wxString & filter,
		       int * p_last_used_filter
		       );
    
  // Destructor.                  
  virtual ~NewTemplateDocDlgWX();

private:
  // Any class wishing to process wxWindows events must use this macro
  DECLARE_EVENT_TABLE()
  void OnCreateButton( wxCommandEvent& event );
  void OnClearButton( wxCommandEvent& event );
  void OnCancelButton( wxCommandEvent& event );
       

  void OnInstanceFilenameButton( wxCommandEvent& event );
  void OnTemplateDirNameButton ( wxCommandEvent& event );

  void OnTemplatenameSelected ( wxCommandEvent& event );

  void OnText_TemplateDirName( wxCommandEvent& event );
  void OnText_InstanceFilename( wxCommandEvent& event );
  void OnText_Combobox( wxCommandEvent& event );

 private:

  void UpdateTemplateFromDir();
  void UpdateInstanceFilenameFromString(const wxString & full_path);

 private:
  wxString m_Filter;
  bool     m_LockUpdateFlag;
  int *    m_pLastUsedFilter;
  wxString m_DirSep;
  int m_doc;
};

#endif  //__OPENDOCDLGWX_H__

#endif /* _WX */
