#ifdef _WX

#ifndef __SEARCHDLGWX_H__
#define __SEARCHDLGWX_H__

//-----------------------------------------------------------------------------
// Headers
//-----------------------------------------------------------------------------

#include "AmayaDialog.h"

//-----------------------------------------------------------------------------
// Class definition: SearchDlgWX
//-----------------------------------------------------------------------------

class SearchDlgWX : public AmayaDialog
{
public: 
    
  // Constructor.
  SearchDlgWX( int ref, 
               wxWindow* parent,
               const wxString & titlecaption,
               const wxString & searched,
               const wxString & replace,
               bool do_replace,
               bool anycase,
               bool searchAfter);
  // Destructor.                  
  virtual ~SearchDlgWX();

private:
  // Override base class functions of a wxDialog.
  void OnConfirmButton( wxCommandEvent& event );
  void OnCancelButton( wxCommandEvent& event );
  void OnNoReplaceButton( wxCommandEvent& event );
  void OnReplaceBox( wxCommandEvent& event );
  void OnTextChanged ( wxCommandEvent& event );
  void OnReplaceChanged ( wxCommandEvent& event );
  void OnSearchAreaBox( wxCommandEvent& event );
  void OnCheckCaseBox( wxCommandEvent& event );

  // Any class wishing to process wxWindows events must use this macro
  DECLARE_EVENT_TABLE()

private:
  int   m_iarea;
  int   m_ireplace;
  bool  m_case;
};

#endif  //__SEARCHDLGWX_H__

#endif /* _WX */
