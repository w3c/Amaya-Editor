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
		       const wxString & docName,
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
       

  void OnFilenameButton( wxCommandEvent& event );
  void OnDirButton( wxCommandEvent& event );
  void OnTemplatename_BrowseButton ( wxCommandEvent& event );

  void OnTemplatenameSelected ( wxCommandEvent& event );

  void OnText_Templatename_Browse( wxCommandEvent& event );
  void OnText_Filename( wxCommandEvent& event );
  void OnText_Dirname( wxCommandEvent& event );
  void OnText_Combobox( wxCommandEvent& event );

 private:

  void UpdateTemplateFromDir();
  void UpdateDirAndFilenameFromString(const wxString & full_path);

 private:
  wxString m_Filter;
  bool     m_LockUpdateFlag;
  int *    m_pLastUsedFilter;
  wxString m_DirSep;
  int m_doc;
};

#endif  //__OPENDOCDLGWX_H__

#endif /* _WX */
