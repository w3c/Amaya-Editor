#ifdef _WX

#ifndef __LISTDLGWX_H__
#define __LISTDLGWX_H__

//-----------------------------------------------------------------------------
// Headers
//-----------------------------------------------------------------------------
#include "AmayaDialog.h"

//-----------------------------------------------------------------------------
// Class definition: ListDlgWX
//-----------------------------------------------------------------------------
class ListDlgWX : public AmayaDialog
{
public: 
  // Constructor.
  ListDlgWX( int ref,
	     int subref,
	     wxWindow* parent,
	     const wxString & title,
	     const wxArrayString& items );
    
  // Destructor.                  
  virtual ~ListDlgWX();

private:
  void OnOkButton( wxCommandEvent& event );
  void OnCancelButton( wxCommandEvent& event );

  // Any class wishing to process wxWindows events must use this macro
  DECLARE_EVENT_TABLE()

private:
  wxString m_Filter;
  int m_SubRef;
};




//-----------------------------------------------------------------------------
// Class definition: NonSelListDlgWX
//-----------------------------------------------------------------------------
class NonSelListDlgWX : public wxDialog
{
public: 
  // Constructor.
  NonSelListDlgWX( wxWindow* parent,
       const wxString & title,
       const wxString & label,
       const wxArrayString& items,
       const wxString& button);
    
  // Destructor.                  
  virtual ~NonSelListDlgWX();
};



#endif  //__LISTDLGWX_H__

#endif /* _WX */
