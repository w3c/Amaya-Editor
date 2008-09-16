#ifdef _WX

#ifndef __LISTNSDLGWX_H__
#define __LISTNSDLGWX_H__

//-----------------------------------------------------------------------------
// Headers
//-----------------------------------------------------------------------------
#include "AmayaDialog.h"

//-----------------------------------------------------------------------------
// Class definition: ListNSDlgWX
//-----------------------------------------------------------------------------
class ListNSDlgWX : public AmayaDialog
{
public: 
  // Constructor.
  ListNSDlgWX( int ref,
	       wxWindow* parent,
	       const wxArrayString& items,
	       const wxArrayString& rdfa_items);
    
  // Destructor.                  
  virtual ~ListNSDlgWX();

  // Update NS list
  void NSUpdate(const wxArrayString& items);


protected:


private:
  // Override base class functions of a wxDialog.
  void OnClose( wxCloseEvent& event );
  void OnCancelButton( wxCommandEvent& event );
  void OnNSAdd(wxCommandEvent& event);
  void OnNSDelete(wxCommandEvent& event);
  void OnNSSelected(wxCommandEvent& event);
  // Any class wishing to process wxWindows events must use this macro
  DECLARE_EVENT_TABLE()
};

#endif  //__LISTNSDLGWX_H__

#endif /* _WX */
