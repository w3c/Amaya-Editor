#ifdef _WX

#ifndef __PREFERENCEDLGWX_H__
#define __PREFERENCEDLGWX_H__

//-----------------------------------------------------------------------------
// Headers
//-----------------------------------------------------------------------------
#include "AmayaDialog.h"
#include "MENUconf.h"

//-----------------------------------------------------------------------------
// Class definition: HRefDlgWX
//-----------------------------------------------------------------------------

class PreferenceDlgWX : public AmayaDialog
{

public: 
    
  // Constructor.
  PreferenceDlgWX( int ref,
		   wxWindow* parent,
		   const wxArrayString & url_list );

  // Destructor.
  virtual ~PreferenceDlgWX();

private:

  // "General" tab
  void SetupDialog_General( const Prop_General & prop );
  Prop_General GetValueDialog_General();

  int GetPagePosFromXMLID( const wxString & xml_id );

  void OnOk( wxCommandEvent& event );
  void OnCancel( wxCommandEvent& event );
  void OnDefault( wxCommandEvent& event );

  // Any class wishing to process wxWindows events must use this macro
  DECLARE_EVENT_TABLE()

private:

};

#endif  // __PREFERENCEDLGWX_H__

#endif /* _WX */
