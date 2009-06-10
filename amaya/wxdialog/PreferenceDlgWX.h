#ifdef _WX

#ifndef __PREFERENCEDLGWX_H__
#define __PREFERENCEDLGWX_H__

//-----------------------------------------------------------------------------
// Headers
//-----------------------------------------------------------------------------
#include "wx/notebook.h"
#include "wx/listbook.h"

#include "AmayaDialog.h"
#include "MENUconf.h"

#include "ListBoxBook.h"

//-----------------------------------------------------------------------------
// Class definition: HRefDlgWX
//-----------------------------------------------------------------------------

class PreferenceDlgWX : public AmayaDialog
{

public: 
    
  // Constructor.
  PreferenceDlgWX( int ref,
		   wxWindow* parent,
		   const wxArrayString & url_list,
		   const wxArrayString & rdfa_list );

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
  void OnGeomToggle( wxCommandEvent& event );
  void OnGeomSave( wxCommandEvent& event );
  void OnGeomRestor( wxCommandEvent& event );

  // "Annot" tab 
  void SetupLabelDialog_Annot();
  void SetupDialog_Annot( const Prop_Annot & prop );
  Prop_Annot GetValueDialog_Annot();

  // "DAV" tab
  void SetupLabelDialog_DAV();
  void SetupDialog_DAV( const Prop_DAV & prop );
  void OnDAVDel(wxCommandEvent& event);
  void OnDAVUpdate( wxCommandEvent& event );
  Prop_DAV GetValueDialog_DAV();

  // "Templates" tab
  void SetupLabelDialog_Templates();
  void SetupDialog_Templates();
  void UpdateTemplateList();
  void GetValueDialog_Templates();
  void OnTemplateChoose(wxCommandEvent& event);
  void OnTemplateAdd(wxCommandEvent& event);
  void OnTemplateDel(wxCommandEvent& event);
  void OnTemplateMoveUp(wxCommandEvent& event);
  void OnTemplateMoveDown(wxCommandEvent& event);
  void OnUpdateTemplateAdd(wxUpdateUIEvent& event);
  void OnUpdateTemplateDel(wxUpdateUIEvent& event);
  void OnUpdateTemplateMoveUp(wxUpdateUIEvent& event);
  void OnUpdateTemplateMoveDown(wxUpdateUIEvent& event);
  void OnTemplateSelected(wxCommandEvent& event);
  void OnTemplateKey(wxCommandEvent& event);

  // "Emails" tab
  void SetupLabelDialog_Emails();
  void SetupDialog_Emails( const Prop_Emails & prop );
  Prop_Emails GetValueDialog_Emails();
  int GetPagePosFromXMLID( const wxString & xml_id );
  void OnOk( wxCommandEvent& event );
  void OnCancel( wxCommandEvent& event );
  void OnDefault( wxCommandEvent& event );
  void OnPageChanged( wxListbookEvent& event );
  void OnClose(wxCloseEvent& event);

  // "Passwords" tab
  void SetupLabelDialog_Passwords();
  void SetupDialog_Passwords( const Prop_Passwords & prop );
  Prop_Passwords GetValueDialog_Passwords();
  void OnEmptyPasswords( wxCommandEvent& event );
  void OnPasswordDeleted(wxCommandEvent& event);
  void OnPasswordSelected(wxCommandEvent& event);

  // "RDFa" tab
  void SetupLabelDialog_RDFa();
  void SetupDialog_RDFa( const Prop_RDFa & prop );
  void UpdateRDFaNsList();
  void GetValueDialog_RDFa();
  void OnNSAdd(wxCommandEvent& event);
  void OnNSDelete(wxCommandEvent& event);
  void OnNSSelected(wxCommandEvent& event);

  // Any class wishing to process wxWindows events must use this macro
  DECLARE_EVENT_TABLE()

private:
  wxListBoxBook *m_book;
  
  wxArrayString m_UrlList;
  wxArrayString m_RDFaNSList;
  
  // "Color" tab
  wxColourData colour_data;

  static bool m_OnApplyLock;
  
  bool m_IsInitialized;
};

#endif  // __PREFERENCEDLGWX_H__

#endif /* _WX */
