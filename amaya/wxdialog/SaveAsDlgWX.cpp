#ifdef _WX

#include "wx/wx.h"
#include "wx/xrc/xmlres.h"              // XRC XML resouces

#include "AmayaApp.h"
#include "SaveAsDlgWX.h"

#define THOT_EXPORT extern
#include "amaya.h"
#include "print.h"
#include "appdialogue_wx.h"

#define APPFILENAMEFILTER _T("HTML files (*.*htm[l])|*.*htm*|XML files (*.xml)|*.xml|MathML Files (*.mml)|*.mml|SVG files (*.svg)|*.svg|CSS files (*.css)|*.css|All files (*.*)|*.*")
#define APPCSSNAMEFILTER _T("CSS files (*.css)|*.css|All files (*.*)|*.*")
#define APPSVGNAMEFILTER _T("SVG files (*.svg)|*.svg|All files (*.*)|*.*")
#define APPMATHNAMEFILTER _T("MathML files (*.mml)|*.mml|All files (*.*)|*.*")
#define APPHTMLNAMEFILTER _T("HTML files (*.*htm[l])|*.*htm*|All files (*.*)|*.*")
#define APPIMAGENAMEFILTER _T("All files (*.*)|*.*|Image files (*.png)|*.png|Image files (*.jpg)|*.jpg|Image files (*.gif)|*.gif|Image files (*.bmp)|*.bmp")
#define APPLIBRARYNAMEFILTER _T("Library files (*.lhtml)|*.lhtml|All files (*.*)|*.*")
#define APPALLFILESFILTER _T("All files (*.*)|*.*")

//-----------------------------------------------------------------------------
// Event table: connect the events to the handler functions to process them
//-----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(SaveAsDlgWX, AmayaDialog)
  EVT_BUTTON( XRCID("wxID_CONFIRMBUTTON"),       SaveAsDlgWX::OnConfirmButton )
  EVT_BUTTON( XRCID("wxID_CANCELBUTTON"),        SaveAsDlgWX::OnCancelButton )
  EVT_BUTTON( XRCID("wxID_BROWSEBUTTON"),        SaveAsDlgWX::OnBrowseButton )
  EVT_BUTTON( XRCID("wxID_CLEARBUTTON"),         SaveAsDlgWX::OnClearButton )
  EVT_RADIOBOX( XRCID("wxID_DOC_FORMAT"),        SaveAsDlgWX::OnDocFormatBox )
  EVT_CHECKBOX( XRCID("wxID_CPY_IMAGES_CHK"),    SaveAsDlgWX::OnImagesChkBox )
  EVT_CHECKBOX( XRCID("wxID_TRANSFORM_URLS_CHK"),SaveAsDlgWX::OnUrlsChkBox )
  EVT_TEXT( XRCID("wxID_DOC_LOCATION_CTRL"),     SaveAsDlgWX::OnDocLocation )
  EVT_TEXT( XRCID("wxID_IMG_LOCATION_CTRL"),     SaveAsDlgWX::OnImgLocation )
  EVT_COMBOBOX( XRCID("wxID_CHARSET_CB"),        SaveAsDlgWX::OnCharsetCbx )
  EVT_COMBOBOX( XRCID("wxID_MIME_TYPE_CB"),      SaveAsDlgWX::OnMimeTypeCbx )
END_EVENT_TABLE()

/*----------------------------------------------------------------------
  SaveAsDlgWX create the SaveAs dialog 
  params:
    + parent : parent window
    + pathname : document location
  ----------------------------------------------------------------------*/
SaveAsDlgWX::SaveAsDlgWX( int ref,
			  wxWindow* parent,
			  const wxString & pathname,
			  int doc ) :
  AmayaDialog( NULL, ref )
{
  int  doc_type;

  wxXmlResource::Get()->LoadDialog(this, parent, wxT("SaveAsDlgWX"));
  wxLogDebug( _T("SaveAsDlgWX::SaveAsDlgWX - pathname=")+pathname);

  wxString wx_title = TtaConvMessageToWX( TtaGetMessage (AMAYA, AM_SAVE_AS) );
  SetTitle( wx_title );

  // Document format radio box
  XRCCTRL(*this, "wxID_DOC_FORMAT", wxRadioBox)->SetLabel(TtaConvMessageToWX( TtaGetMessage (LIB, TMSG_DOCUMENT_FORMAT) ));
  XRCCTRL(*this, "wxID_DOC_FORMAT", wxRadioBox)->SetString(0, TtaConvMessageToWX( "HTML"  ) );
  XRCCTRL(*this, "wxID_DOC_FORMAT", wxRadioBox)->SetString(1, TtaConvMessageToWX( "XML" ));
  XRCCTRL(*this, "wxID_DOC_FORMAT", wxRadioBox)->SetString(2, TtaConvMessageToWX( "Text" ));
  XRCCTRL(*this, "wxID_DOC_FORMAT", wxRadioBox)->SetSelection(1);

  // Options check box
  XRCCTRL(*this, "wxID_OPTIONS", wxStaticText)->SetLabel(TtaConvMessageToWX( TtaGetMessage(LIB, TMSG_OPTIONS) ));
  XRCCTRL(*this, "wxID_CPY_IMAGES_CHK", wxCheckBox)->SetLabel(TtaConvMessageToWX( TtaGetMessage(AMAYA, AM_BCOPY_IMAGES) ));
  XRCCTRL(*this, "wxID_TRANSFORM_URLS_CHK", wxCheckBox)->SetLabel(TtaConvMessageToWX( TtaGetMessage(AMAYA, AM_BTRANSFORM_URL) ));
  XRCCTRL(*this, "wxID_CPY_IMAGES_CHK", wxCheckBox)->SetValue(TRUE);
  XRCCTRL(*this, "wxID_TRANSFORM_URLS_CHK", wxCheckBox)->SetValue(FALSE);

  // Document location
  XRCCTRL(*this, "wxID_DOC_LOCATION", wxStaticText)->SetLabel(TtaConvMessageToWX( TtaGetMessage(AMAYA, AM_DOC_LOCATION) ));
  XRCCTRL(*this, "wxID_DOC_LOCATION_CTRL", wxTextCtrl)->SetValue(pathname);

  // Image directory
  XRCCTRL(*this, "wxID_IMG_LOCATION", wxStaticText)->SetLabel(TtaConvMessageToWX( TtaGetMessage(AMAYA, AM_IMAGES_LOCATION) ));
  XRCCTRL(*this, "wxID_IMG_LOCATION_CTRL", wxTextCtrl)->SetValue(TtaConvMessageToWX( SaveImgsURL));
  
  // Charset  
  wxString wx_label = TtaConvMessageToWX( "Charset : " );
  wxString wx_ascii = TtaConvMessageToWX( "us-ascii" );
  wxString wx_iso_8859_1 = TtaConvMessageToWX( "iso-8859-1" );
  wxString wx_utf8 = TtaConvMessageToWX( "UTF-8" );
  XRCCTRL(*this, "wxID_CHARSET", wxStaticText)->SetLabel(wx_label);

  if (!strcmp (UserCharset, "us-ascii"))
    {
      XRCCTRL(*this, "wxID_CHARSET_CB",
	      wxComboBox)->SetString (0, TtaConvMessageToWX( "us-ascii" ) );
      XRCCTRL(*this, "wxID_CHARSET_CB",
	      wxComboBox)->SetString (1, TtaConvMessageToWX( "iso-8859-1" ) );
      XRCCTRL(*this, "wxID_CHARSET_CB",
	      wxComboBox)->SetString (2, TtaConvMessageToWX( "UTF-8" ) );
    }
  else if (!strcmp (UserCharset, "iso-8859-1"))
    {
      XRCCTRL(*this, "wxID_CHARSET_CB",
	      wxComboBox)->SetString (0, TtaConvMessageToWX( "iso-8859-1" ) );
      XRCCTRL(*this, "wxID_CHARSET_CB",
	      wxComboBox)->SetString (1, TtaConvMessageToWX( "us-ascii" ) );
      XRCCTRL(*this, "wxID_CHARSET_CB",
	      wxComboBox)->SetString (2, TtaConvMessageToWX( "UTF-8" ) );
    }
  else
    {
      XRCCTRL(*this, "wxID_CHARSET_CB",
	      wxComboBox)->SetString (0, TtaConvMessageToWX( "UTF-8" ) );
      XRCCTRL(*this, "wxID_CHARSET_CB",
	      wxComboBox)->SetString (1, TtaConvMessageToWX( "us-ascii" ) );
      XRCCTRL(*this, "wxID_CHARSET_CB",
	      wxComboBox)->SetString (2, TtaConvMessageToWX( "iso-8859-1" ) );
    }
      XRCCTRL(*this, "wxID_CHARSET_CB",
	      wxComboBox)->SetSelection (0);

  // Mime Type
  wx_label = TtaConvMessageToWX( "   Mime Type : " );
  XRCCTRL(*this, "wxID_MIME_TYPE", wxStaticText)->SetLabel( wx_label );

  doc_type = DocumentTypes[doc];
  if (doc_type == docImage)
    {
  XRCCTRL(*this, "wxID_MIME_TYPE_CB",
	  wxComboBox)->SetString(0, TtaConvMessageToWX( "image/png" ) );
  XRCCTRL(*this, "wxID_MIME_TYPE_CB",
	  wxComboBox)->SetString(1, TtaConvMessageToWX( "image/jpeg" ) );
  XRCCTRL(*this, "wxID_MIME_TYPE_CB",
	  wxComboBox)->SetString(2, TtaConvMessageToWX( "image/gif" ) );
  XRCCTRL(*this, "wxID_MIME_TYPE_CB",
	  wxComboBox)->SetString(3, TtaConvMessageToWX( "image/x-bitmap" ) );
  XRCCTRL(*this, "wxID_MIME_TYPE_CB",
	  wxComboBox)->SetString(4, TtaConvMessageToWX( "image/x-xpicmap" ) );
    }
  else if (doc_type == docSVG)
    {
  XRCCTRL(*this, "wxID_MIME_TYPE_CB",
	  wxComboBox)->SetString(0, TtaConvMessageToWX( AM_SVG_MIME_TYPE ) );
  XRCCTRL(*this, "wxID_MIME_TYPE_CB",
	  wxComboBox)->SetString(1, TtaConvMessageToWX( "application/xml" ) );
  XRCCTRL(*this, "wxID_MIME_TYPE_CB",
	  wxComboBox)->SetString(2, TtaConvMessageToWX( "text/xml" ) );
    }
  else if (doc_type == docMath)
    {
  XRCCTRL(*this, "wxID_MIME_TYPE_CB",
	  wxComboBox)->SetString(0, TtaConvMessageToWX( AM_MATHML_MIME_TYPE ) );
  XRCCTRL(*this, "wxID_MIME_TYPE_CB",
	  wxComboBox)->SetString(1, TtaConvMessageToWX( "application/xml" ) );
  XRCCTRL(*this, "wxID_MIME_TYPE_CB",
	  wxComboBox)->SetString(2, TtaConvMessageToWX( "text/xml" ) );
    }
  else if (doc_type == docHTML && DocumentMeta[doc] &&
	   DocumentMeta[doc]->xmlformat)
    {
  XRCCTRL(*this, "wxID_MIME_TYPE_CB",
	  wxComboBox)->SetString(0, TtaConvMessageToWX( AM_XHTML_MIME_TYPE ) );
  XRCCTRL(*this, "wxID_MIME_TYPE_CB",
	  wxComboBox)->SetString(1, TtaConvMessageToWX( "text/html" ) );
  XRCCTRL(*this, "wxID_MIME_TYPE_CB",
	  wxComboBox)->SetString(2, TtaConvMessageToWX( "application/xml" ) );
  XRCCTRL(*this, "wxID_MIME_TYPE_CB",
	  wxComboBox)->SetString(3, TtaConvMessageToWX( "text/xml" ) );
    }
  else
    {
      XRCCTRL(*this, "wxID_MIME_TYPE_CB",
	      wxComboBox)->SetString(0, TtaConvMessageToWX( "text/html" ) );
      XRCCTRL(*this, "wxID_MIME_TYPE_CB",
	      wxComboBox)->SetString(1, TtaConvMessageToWX( AM_XHTML_MIME_TYPE ) );
      XRCCTRL(*this, "wxID_MIME_TYPE_CB",
	      wxComboBox)->SetString(2, TtaConvMessageToWX( "application/xml" ) );
      XRCCTRL(*this, "wxID_MIME_TYPE_CB",
	      wxComboBox)->SetString(3, TtaConvMessageToWX( "text/xml" ) );
      XRCCTRL(*this, "wxID_MIME_TYPE_CB",
	      wxComboBox)->SetString(4, TtaConvMessageToWX( "text/plain" ) );
      XRCCTRL(*this, "wxID_MIME_TYPE_CB",
	      wxComboBox)->SetString(5, TtaConvMessageToWX( "text/css" ) );
      XRCCTRL(*this, "wxID_MIME_TYPE_CB",
	      wxComboBox)->SetString(6, TtaConvMessageToWX( "application/smil" ) );
    }
  wxString wx_mime_type = TtaConvMessageToWX( UserMimeType );
  XRCCTRL(*this, "wxID_MIME_TYPE_CB", wxComboBox)->SetValue( wx_mime_type );

  // buttons
  XRCCTRL(*this, "wxID_CONFIRMBUTTON", wxButton)->SetLabel(TtaConvMessageToWX( TtaGetMessage(LIB, TMSG_LIB_CONFIRM) ));
  XRCCTRL(*this, "wxID_CANCELBUTTON", wxButton)->SetLabel(TtaConvMessageToWX( TtaGetMessage(LIB, TMSG_CANCEL) ));
  XRCCTRL(*this, "wxID_BROWSEBUTTON", wxButton)->SetLabel(TtaConvMessageToWX( TtaGetMessage(AMAYA, AM_BROWSE) ));
  XRCCTRL(*this, "wxID_CLEARBUTTON", wxButton)->SetLabel(TtaConvMessageToWX( TtaGetMessage(AMAYA, AM_CLEAR) ));
  
  // Set focus to ...
  XRCCTRL(*this, "wxID_DOC_LOCATION_CTRL", wxTextCtrl)->SetFocus();

  Layout();
  
  SetAutoLayout( TRUE );
}

/*---------------------------------------------------------------------------
  Destructor. (Empty, as we don't need anything special done when destructing).
  ---------------------------------------------------------------------------*/
SaveAsDlgWX::~SaveAsDlgWX()
{
}

/*----------------------------------------------------------------------
  OnConfirmButton called when clicking the Confirm button
  ----------------------------------------------------------------------*/
void SaveAsDlgWX::OnConfirmButton( wxCommandEvent& event )
{
  wxLogDebug( _T("SaveAsDlgWX::OnConfirmButton") );
  // print callback
  ThotCallback (BaseDialog + SaveForm, INTEGER_DATA, (char*) 1);
}

/*----------------------------------------------------------------------
  OnCancelButton called when clicking on Cancel button
  ----------------------------------------------------------------------*/
void SaveAsDlgWX::OnCancelButton( wxCommandEvent& event )
{
  wxLogDebug( _T("SaveAsDlgWX::OnCancelButton") );
  ThotCallback (BaseDialog + SaveForm, INTEGER_DATA, (char*) 0);
}

/*----------------------------------------------------------------------
  OnBrowseButton called when clicking on Browse button
  ----------------------------------------------------------------------*/
void SaveAsDlgWX::OnBrowseButton( wxCommandEvent& event )
{
  wxLogDebug( _T("SaveAsDlgWX::OnBrowseButton") );

  wxString wx_filter;

  /*  if (doc_type == docHTML)
    wx_filter = APPHTMLNAMEFILTER;
  else 
  */
    wx_filter = APPFILENAMEFILTER;

  // Create a generic filedialog
  wxFileDialog * p_dlg = new wxFileDialog
    (
     this,
     TtaConvMessageToWX( "Save ..." ),
     _T(""),
     _T(""), 
     wx_filter
     );

  p_dlg->SetDirectory(wxGetHomeDir());
  
  if (p_dlg->ShowModal() == wxID_OK)
    {
      XRCCTRL(*this, "wxID_DOC_LOCATION_CTRL", wxTextCtrl)->SetValue( p_dlg->GetPath() );
      // destroy the dlg before calling thotcallback because it's a child of this
      // dialog and thotcallback will delete the dialog...
      // so if I do not delete it manualy here it will be deleted twice
      p_dlg->Destroy();
    }
  else
      p_dlg->Destroy();
}

/*----------------------------------------------------------------------
  OnClearButton called when clicking on Clear button
  ----------------------------------------------------------------------*/
void SaveAsDlgWX::OnClearButton( wxCommandEvent& event )
{
  wxLogDebug( _T("SaveAsDlgWX::OnClearButton") );
  ThotCallback (BaseDialog + SaveForm, INTEGER_DATA, (char*) 3);
  XRCCTRL(*this, "wxID_DOC_LOCATION_CTRL", wxTextCtrl)->SetValue(TtaConvMessageToWX( SavePath));
  XRCCTRL(*this, "wxID_IMG_LOCATION_CTRL", wxTextCtrl)->SetValue(TtaConvMessageToWX( SaveImgsURL));
  }

/*----------------------------------------------------------------------
  OnCharsetCbx called when selecting a Charset Combobox entry
  ----------------------------------------------------------------------*/
void SaveAsDlgWX::OnCharsetCbx( wxCommandEvent& event )
{
  wxString wx_charset_sel = XRCCTRL(*this, "wxID_CHARSET_CB", wxComboBox)->GetValue ();
  wxLogDebug( _T("SaveAsDlgWX::OnCharsetCbx - wx_charset=")+wx_charset_sel );

  // allocate a temporary buffer to copy the 'const char *' url buffer 
  char buf_charset[50];
  wxASSERT( wx_charset_sel.Len() < 50 );
  strcpy( buf_charset, wx_charset_sel.ToAscii() );
  strcpy (UserCharset, buf_charset);
}

/*----------------------------------------------------------------------
  OnMimeTypeCbx called when selecting a MimeType Combobox entry
  ----------------------------------------------------------------------*/
void SaveAsDlgWX::OnMimeTypeCbx( wxCommandEvent& event )
{
  wxString wx_mimetype = XRCCTRL(*this, "wxID_MIME_TYPE_CB", wxComboBox)->GetValue ();
  wxLogDebug( _T("SaveAsDlgWX::OnMimeTypeCbx - wx_mimetype=")+wx_mimetype );

  // allocate a temporary buffer to copy the 'const char *' url buffer 
  char buf_mimetype[50];
  wxASSERT( wx_mimetype.Len() < 50 );
  strcpy( buf_mimetype, wx_mimetype.ToAscii() );
  strcpy (UserMimeType, buf_mimetype);

}

/*----------------------------------------------------------------------
  OnDocFormatBox called when clicking the Document Format radio box
  ----------------------------------------------------------------------*/
void SaveAsDlgWX::OnDocFormatBox ( wxCommandEvent& event )
{
  wxLogDebug( _T("SaveAsDlgWX::OnDocFormatBox") );
  ThotCallback (BaseDialog + RadioSave, INTEGER_DATA,
		(char*) (XRCCTRL(*this, "wxID_DOC_FORMAT", wxRadioBox)->GetSelection( )));
}

/*---------------------------------------------------------------
  OnImagesChkBox
  ---------------------------------------------------------------*/
void SaveAsDlgWX::OnImagesChkBox ( wxCommandEvent& event )
{
  wxLogDebug( _T("SaveAsDlgWX::OnImagesChkBox") );
  ThotCallback (BaseDialog + ToggleSave, INTEGER_DATA, (char*) 0);
}

/*---------------------------------------------------------------
  OnUrlsChkBox
  ---------------------------------------------------------------*/
void SaveAsDlgWX::OnUrlsChkBox ( wxCommandEvent& event )
{
  wxLogDebug( _T("SaveAsDlgWX::OnUrlsChkBox") );
  ThotCallback (BaseDialog + ToggleSave, INTEGER_DATA, (char*) 0);
}

/*---------------------------------------------------------------
  OnDocLocation 
  ---------------------------------------------------------------*/
void SaveAsDlgWX::OnDocLocation ( wxCommandEvent& event )
{

  wxString doc_location = XRCCTRL(*this, "wxID_DOC_LOCATION_CTRL", wxTextCtrl)->GetValue( );
  wxLogDebug( _T("SaveAsDlgWX::OnDocLocation - doc_location =")+  doc_location);
  
  // set the printer name
  // allocate a temporary buffer to copy the 'const char *' printer name buffer 
  char buffer[100];
  wxASSERT( doc_location.Len() < 100 );
  strcpy( buffer, doc_location.ToAscii() );
  ThotCallback (BaseDialog + NameSave,  STRING_DATA, (char *)buffer );
}

/*---------------------------------------------------------------
  OnImgLocation 
  ---------------------------------------------------------------*/
void SaveAsDlgWX::OnImgLocation ( wxCommandEvent& event )
{

  wxString img_location = XRCCTRL(*this, "wxID_IMG_LOCATION_CTRL", wxTextCtrl)->GetValue( );
  wxLogDebug( _T("SaveAsDlgWX::OnImgLocation - img_location =")+  img_location);
  
  // set the printer name
  // allocate a temporary buffer to copy the 'const char *' printer name buffer 
  char buffer[100];
  wxASSERT( img_location.Len() < 100 );
  strcpy( buffer, img_location.ToAscii() );
  ThotCallback (BaseDialog + ImgDirSave,  STRING_DATA, (char *)buffer );
}

#endif /* _WX */
