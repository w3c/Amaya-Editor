#ifdef _WX

#ifndef __LISTEDITDLGWX_H__
#define __LISTEDITDLGWX_H__

//-----------------------------------------------------------------------------
// Headers
//-----------------------------------------------------------------------------
#include "AmayaDialog.h"

//-----------------------------------------------------------------------------
// Class definition: ListEditDlgWX
//-----------------------------------------------------------------------------
class ListEditDlgWX : public AmayaDialog
{
public: 
  // Constructor.
  ListEditDlgWX( int ref,
		 wxWindow* parent,
		 const wxString & title,
		 const wxString & list_title,
		 const wxArrayString& items,
		 const wxString & selected_item );
    
  // Destructor.                  
  virtual ~ListEditDlgWX();

protected:
  // Any class wishing to process wxWindows events must use this macro
  DECLARE_EVENT_TABLE()

  void OnOkButton( wxCommandEvent& event );
  void OnCancelButton( wxCommandEvent& event );
  void OnSelected( wxCommandEvent& event );
};

#endif  //__LISTEDITDLGWX_H__

#endif /* _WX */
