#ifdef _WX

#ifndef __PREFERENCEDLGWX_H__
#define __PREFERENCEDLGWX_H__

//-----------------------------------------------------------------------------
// Headers
//-----------------------------------------------------------------------------
#include "wx/notebook.h"
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
  void SetupLabelDialog_General();
  void SetupDialog_General( const Prop_General & prop );
  Prop_General GetValueDialog_General();

  // "Browse" tab
  void SetupLabelDialog_Browse();
  void SetupDialog_Browse( const Prop_Browse & prop );
  Prop_Browse GetValueDialog_Browse();
  void OnClearUrlList( wxCommandEvent& event );

  // "Publish" tab
  void SetupLabelDialog_Publish();
  void SetupDialog_Publish( const Prop_Publish & prop );
  Prop_Publish GetValueDialog_Publish();

  // "Cache" tab
  void SetupLabelDialog_Cache();
  void SetupDialog_Cache( const Prop_Cache & prop );
  Prop_Cache GetValueDialog_Cache();
  void OnEmptyCache( wxCommandEvent& event );

  // "Proxy" tab
  void SetupLabelDialog_Proxy();
  void SetupDialog_Proxy( const Prop_Proxy & prop );
  Prop_Proxy GetValueDialog_Proxy();

  // "Color" tab
  void SetupLabelDialog_Color();
  void SetupDialog_Color( const Prop_Color & prop );
  Prop_Color GetValueDialog_Color();
  void OnColorPalette( wxCommandEvent& event );
  void OnColorChanged( wxCommandEvent& event );
  void OnColorTextChanged( wxCommandEvent& event );

  // "Geometry" tab
  void SetupLabelDialog_Geometry();
  void OnGeomSave( wxCommandEvent& event );
  void OnGeomRestor( wxCommandEvent& event );

  // "Annot" tab 
  void SetupLabelDialog_Annot();
  void SetupDialog_Annot( const Prop_Annot & prop );
  Prop_Annot GetValueDialog_Annot();



  // "DAV" tab
  void SetupLabelDialog_DAV();
  void SetupDialog_DAV( const Prop_DAV & prop );
  Prop_DAV GetValueDialog_DAV();


  int GetPagePosFromXMLID( const wxString & xml_id );

  void OnOk( wxCommandEvent& event );
  void OnCancel( wxCommandEvent& event );
  void OnDefault( wxCommandEvent& event );
  void OnPageChanged( wxNotebookEvent& event );
  void OnClose(wxCloseEvent& event);

  // Any class wishing to process wxWindows events must use this macro
  DECLARE_EVENT_TABLE()

private:
  wxArrayString m_UrlList;
  
  // "Color" tab
  wxColourData colour_data;

  static bool m_OnApplyLock;
  
  bool m_IsInitialized;
};

#endif  // __PREFERENCEDLGWX_H__

#endif /* _WX */
