#ifdef _WX

#include "wx/wx.h"
#include "wx/xrc/xmlres.h"              // XRC XML resouces
#include "wx/statline.h"

#include "AmayaApp.h"
#include "SaveAsDlgWX.h"

#define THOT_EXPORT extern
#include "amaya.h"
#include "print.h"
#include "appdialogue_wx.h"
#include "message_wx.h"
#include "wxdialog/file_filters.h"

int MyRef;

//-----------------------------------------------------------------------------
// Event table: connect the events to the handler functions to process them
//-----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(SaveAsDlgWX, AmayaDialog)
  EVT_BUTTON( XRCID("wxID_SAVE"),       SaveAsDlgWX::OnConfirmButton )
  EVT_BUTTON( XRCID("wxID_CANCEL"),        SaveAsDlgWX::OnCancelButton )
  EVT_BUTTON( XRCID("wxID_BROWSE"),        SaveAsDlgWX::OnBrowseButton )
  EVT_BUTTON( XRCID("wxID_CLEAR"),         SaveAsDlgWX::OnClearButton )
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
  AmayaDialog( parent, ref )
{
  int  doc_type;
  
  doc_type = DocumentTypes[doc];
  MyRef = ref;
  wxXmlResource::Get()->LoadDialog(this, parent, wxT("SaveAsDlgWX"));
  wxString wx_title = TtaConvMessageToWX( TtaGetMessage (AMAYA, AM_SAVE_AS) );
  SetTitle( wx_title );

  // Document format radio box
  if ( doc_type == docSource || doc_type == docCSS ||  doc_type == docText)
    {
      // now hide unused field
      wxWindow * p_obj;
      p_obj = XRCCTRL(*this, "wxID_IMG_LOCATION", wxStaticText);
      p_obj->GetContainingSizer()->Show(p_obj, FALSE);
      p_obj = XRCCTRL(*this, "wxID_IMG_LOCATION_CTRL", wxTextCtrl);
      p_obj->GetContainingSizer()->Show(p_obj, FALSE);
      p_obj = XRCCTRL(*this, "wxID_DOC_FORMAT", wxRadioBox);
      p_obj->GetContainingSizer()->Show(p_obj, FALSE);
      p_obj = XRCCTRL(*this, "wxID_OPTIONS", wxStaticText);
      p_obj->GetContainingSizer()->Show(p_obj, FALSE);
      p_obj = XRCCTRL(*this, "wxID_CPY_IMAGES_CHK", wxCheckBox);
      p_obj->GetContainingSizer()->Show(p_obj, FALSE);
      p_obj = XRCCTRL(*this, "wxID_TRANSFORM_URLS_CHK", wxCheckBox);
      p_obj->GetContainingSizer()->Show(p_obj, FALSE);
      p_obj = XRCCTRL(*this, "wxID_STATICLINE", wxStaticLine);
      p_obj->GetContainingSizer()->Show(p_obj, FALSE);
      if ( doc_type == docSource)
        {
          p_obj = XRCCTRL(*this, "wxID_CHARSET", wxStaticText);
          p_obj->GetContainingSizer()->Show(p_obj, FALSE);
          p_obj = XRCCTRL(*this, "wxID_CHARSET_CB", wxComboBox);
          p_obj->GetContainingSizer()->Show(p_obj, FALSE);
        }
      GetSizer()->SetSizeHints( this );
    }
  else
    {
      XRCCTRL(*this, "wxID_DOC_FORMAT", wxRadioBox)->SetLabel(TtaConvMessageToWX( TtaGetMessage (LIB, TMSG_DOCUMENT_FORMAT) ));
      XRCCTRL(*this, "wxID_DOC_FORMAT", wxRadioBox)->SetString(0, TtaConvMessageToWX( "HTML"  ) );
      XRCCTRL(*this, "wxID_DOC_FORMAT", wxRadioBox)->SetString(1, TtaConvMessageToWX( "XML" ));
      XRCCTRL(*this, "wxID_DOC_FORMAT", wxRadioBox)->SetString(2, TtaConvMessageToWX( TtaGetMessage (AMAYA, AM_TEXT) ));
      if ((DocumentMeta[doc] && DocumentMeta[doc]->xmlformat) ||
          doc_type == docMath ||
          doc_type == docSVG ||
          doc_type == docXml)
        XRCCTRL(*this, "wxID_DOC_FORMAT", wxRadioBox)->SetSelection(1);
      else if (doc_type == docHTML)
        XRCCTRL(*this, "wxID_DOC_FORMAT", wxRadioBox)->SetSelection(0);

      // Options check box
      XRCCTRL(*this, "wxID_OPTIONS", wxStaticText)->SetLabel(TtaConvMessageToWX( TtaGetMessage(LIB, TMSG_OPTIONS) ));
      XRCCTRL(*this, "wxID_CPY_IMAGES_CHK", wxCheckBox)->SetLabel(TtaConvMessageToWX( TtaGetMessage(AMAYA, AM_BCOPY_IMAGES) ));
      XRCCTRL(*this, "wxID_TRANSFORM_URLS_CHK", wxCheckBox)->SetLabel(TtaConvMessageToWX( TtaGetMessage(AMAYA, AM_BTRANSFORM_URL) ));
      XRCCTRL(*this, "wxID_CPY_IMAGES_CHK", wxCheckBox)->SetValue(CopyImages);
      XRCCTRL(*this, "wxID_TRANSFORM_URLS_CHK", wxCheckBox)->SetValue(UpdateURLs);

      // Image directory
      XRCCTRL(*this, "wxID_IMG_LOCATION", wxStaticText)->SetLabel(TtaConvMessageToWX( TtaGetMessage(AMAYA, AM_IMAGES_LOCATION) ));
      XRCCTRL(*this, "wxID_IMG_LOCATION_CTRL", wxTextCtrl)->SetValue(TtaConvMessageToWX( SaveImgsURL));
    }
  
  // Document location
  XRCCTRL(*this, "wxID_DOC_LOCATION", wxStaticText)->SetLabel(TtaConvMessageToWX( TtaGetMessage(AMAYA, AM_DOC_LOCATION) ));
  XRCCTRL(*this, "wxID_DOC_LOCATION_CTRL", wxTextCtrl)->SetValue(pathname);

  // Charset  
  wxString wx_label = TtaConvMessageToWX( "Charset :" );
  wxString wx_ascii = TtaConvMessageToWX( "us-ascii" );
  wxString wx_iso_8859_1 = TtaConvMessageToWX( "iso-8859-1" );
  wxString wx_utf8 = TtaConvMessageToWX( "UTF-8" );
  XRCCTRL(*this, "wxID_CHARSET", wxStaticText)->SetLabel(wx_label);

  if (!strcmp (UserCharset, "us-ascii"))
    {
      XRCCTRL(*this, "wxID_CHARSET_CB",
              wxComboBox)->Append ( TtaConvMessageToWX( "us-ascii" ) );
      XRCCTRL(*this, "wxID_CHARSET_CB",
              wxComboBox)->Append ( TtaConvMessageToWX( "iso-8859-1" ) );
      XRCCTRL(*this, "wxID_CHARSET_CB",
              wxComboBox)->Append ( TtaConvMessageToWX( "UTF-8" ) );
    }
  else if (!strcmp (UserCharset, "iso-8859-1"))
    {
      XRCCTRL(*this, "wxID_CHARSET_CB",
              wxComboBox)->Append ( TtaConvMessageToWX( "iso-8859-1" ) );
      XRCCTRL(*this, "wxID_CHARSET_CB",
              wxComboBox)->Append ( TtaConvMessageToWX( "us-ascii" ) );
      XRCCTRL(*this, "wxID_CHARSET_CB",
              wxComboBox)->Append ( TtaConvMessageToWX( "UTF-8" ) );
    }
  else
    {
      XRCCTRL(*this, "wxID_CHARSET_CB",
              wxComboBox)->Append ( TtaConvMessageToWX( UserCharset ) );
      XRCCTRL(*this, "wxID_CHARSET_CB",
              wxComboBox)->Append ( TtaConvMessageToWX( "UTF-8" ) );
      XRCCTRL(*this, "wxID_CHARSET_CB",
              wxComboBox)->Append ( TtaConvMessageToWX( "us-ascii" ) );
      XRCCTRL(*this, "wxID_CHARSET_CB",
              wxComboBox)->Append ( TtaConvMessageToWX( "iso-8859-1" ) );
    }
  XRCCTRL(*this, "wxID_CHARSET_CB",
          wxComboBox)->SetSelection (0);

  // Mime Type
  wx_label = TtaConvMessageToWX( " Mime Type :" );
  XRCCTRL(*this, "wxID_MIME_TYPE", wxStaticText)->SetLabel( wx_label );

  wxString wx_mime_type = TtaConvMessageToWX( UserMimeType );
  if (doc_type == docImage)
    {
      XRCCTRL(*this, "wxID_MIME_TYPE_CB",
              wxComboBox)->Append( wx_mime_type );
      if (strcmp (UserMimeType, "image/png"))
        XRCCTRL(*this, "wxID_MIME_TYPE_CB",
                wxComboBox)->Append( TtaConvMessageToWX( "image/png" ) );
      if (strcmp (UserMimeType, "image/jpeg"))
        XRCCTRL(*this, "wxID_MIME_TYPE_CB",
                wxComboBox)->Append( TtaConvMessageToWX( "image/jpeg" ) );
      if (strcmp (UserMimeType, "image/gif"))
        XRCCTRL(*this, "wxID_MIME_TYPE_CB",
                wxComboBox)->Append( TtaConvMessageToWX( "image/gif" ) );
      if (strcmp (UserMimeType, "image/x-bitmap"))
        XRCCTRL(*this, "wxID_MIME_TYPE_CB",
                wxComboBox)->Append( TtaConvMessageToWX( "image/x-bitmap" ) );
      if (strcmp (UserMimeType, "image/x-xpicmap"))
        XRCCTRL(*this, "wxID_MIME_TYPE_CB",
                wxComboBox)->Append( TtaConvMessageToWX( "image/x-xpicmap" ) );
    }
  else if (doc_type == docSVG)
    {
      XRCCTRL(*this, "wxID_MIME_TYPE_CB",
              wxComboBox)->Append( wx_mime_type );
      if (strcmp (UserMimeType, AM_SVG_MIME_TYPE))
        XRCCTRL(*this, "wxID_MIME_TYPE_CB",
                wxComboBox)->Append( TtaConvMessageToWX( AM_SVG_MIME_TYPE ) );
      if (strcmp (UserMimeType, "application/xml"))
        XRCCTRL(*this, "wxID_MIME_TYPE_CB",
                wxComboBox)->Append( TtaConvMessageToWX( "application/xml" ) );
      if (strcmp (UserMimeType, "text/xml"))
        XRCCTRL(*this, "wxID_MIME_TYPE_CB",
                wxComboBox)->Append( TtaConvMessageToWX( "text/xml" ) );
    }
  else if (doc_type == docMath)
    {
      XRCCTRL(*this, "wxID_MIME_TYPE_CB",
              wxComboBox)->Append( wx_mime_type );
      if (strcmp (UserMimeType, AM_MATHML_MIME_TYPE))
        XRCCTRL(*this, "wxID_MIME_TYPE_CB",
                wxComboBox)->Append( TtaConvMessageToWX( AM_MATHML_MIME_TYPE ) );
      if (strcmp (UserMimeType, "application/xml"))
        XRCCTRL(*this, "wxID_MIME_TYPE_CB",
                wxComboBox)->Append( TtaConvMessageToWX( "application/xml" ) );
      if (strcmp (UserMimeType, "text/xml"))
        XRCCTRL(*this, "wxID_MIME_TYPE_CB",
                wxComboBox)->Append( TtaConvMessageToWX( "text/xml" ) );
    }
  else if (doc_type == docHTML && DocumentMeta[doc] &&
           DocumentMeta[doc]->xmlformat)
    {
      XRCCTRL(*this, "wxID_MIME_TYPE_CB",
              wxComboBox)->Append( wx_mime_type );
      if (strcmp (UserMimeType, AM_XHTML_MIME_TYPE))
        XRCCTRL(*this, "wxID_MIME_TYPE_CB",
                wxComboBox)->Append( TtaConvMessageToWX( AM_XHTML_MIME_TYPE ) );
      if (strcmp (UserMimeType, "text/html"))
        XRCCTRL(*this, "wxID_MIME_TYPE_CB",
                wxComboBox)->Append( TtaConvMessageToWX( "text/html" ) );
      if (strcmp (UserMimeType, "application/xml"))
        XRCCTRL(*this, "wxID_MIME_TYPE_CB",
                wxComboBox)->Append( TtaConvMessageToWX( "application/xml" ) );
      if (strcmp (UserMimeType, "text/xml"))
        XRCCTRL(*this, "wxID_MIME_TYPE_CB",
                wxComboBox)->Append( TtaConvMessageToWX( "text/xml" ) );
    }
  else
    {
      XRCCTRL(*this, "wxID_MIME_TYPE_CB",
              wxComboBox)->Append( wx_mime_type );
      if (strcmp (UserMimeType, "text/html"))
        XRCCTRL(*this, "wxID_MIME_TYPE_CB",
                wxComboBox)->Append( TtaConvMessageToWX( "text/html" ) );
      if (strcmp (UserMimeType, AM_XHTML_MIME_TYPE))
        XRCCTRL(*this, "wxID_MIME_TYPE_CB",
                wxComboBox)->Append( TtaConvMessageToWX( AM_XHTML_MIME_TYPE ) );
      if (strcmp (UserMimeType, "application/xml"))
        XRCCTRL(*this, "wxID_MIME_TYPE_CB",
                wxComboBox)->Append( TtaConvMessageToWX( "application/xml" ) );
      if (strcmp (UserMimeType, "text/xml"))
        XRCCTRL(*this, "wxID_MIME_TYPE_CB",
                wxComboBox)->Append( TtaConvMessageToWX( "text/xml" ) );
      if (strcmp (UserMimeType, "text/plain"))
        XRCCTRL(*this, "wxID_MIME_TYPE_CB",
                wxComboBox)->Append( TtaConvMessageToWX( "text/plain" ) );
      if (strcmp (UserMimeType, "text/css"))
        XRCCTRL(*this, "wxID_MIME_TYPE_CB",
                wxComboBox)->Append( TtaConvMessageToWX( "text/css" ) );
      if (strcmp (UserMimeType, "application/smil"))
        XRCCTRL(*this, "wxID_MIME_TYPE_CB",
                wxComboBox)->Append( TtaConvMessageToWX( "application/smil" ) );
    }
  XRCCTRL(*this, "wxID_MIME_TYPE_CB", wxComboBox)->SetValue( wx_mime_type );

  // buttons
  XRCCTRL(*this, "wxID_SAVE", wxButton)->SetLabel(TtaConvMessageToWX( TtaGetMessage(LIB, TMSG_BUTTON_SAVE) ));
  XRCCTRL(*this, "wxID_CANCEL", wxButton)->SetLabel(TtaConvMessageToWX( TtaGetMessage(LIB, TMSG_CANCEL) ));
  XRCCTRL(*this, "wxID_BROWSE", wxButton)->SetLabel(TtaConvMessageToWX( TtaGetMessage(AMAYA, AM_BROWSE) ));
  XRCCTRL(*this, "wxID_CLEAR", wxButton)->SetLabel(TtaConvMessageToWX( TtaGetMessage(AMAYA, AM_CLEAR) ));
  
  // Set focus to ...
  //  XRCCTRL(*this, "wxID_DOC_LOCATION_CTRL", wxTextCtrl)->SetFocus();

  Fit();
  Refresh();
  //Layout();
  
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
  // print callback
  ThotCallback (MyRef, INTEGER_DATA, (char*) 1);
}

/*----------------------------------------------------------------------
  OnCancelButton called when clicking on Cancel button
  ----------------------------------------------------------------------*/
void SaveAsDlgWX::OnCancelButton( wxCommandEvent& event )
{
  ThotCallback (MyRef, INTEGER_DATA, (char*) 0);
}

/*----------------------------------------------------------------------
  OnBrowseButton called when clicking on Browse button
  ----------------------------------------------------------------------*/
void SaveAsDlgWX::OnBrowseButton( wxCommandEvent& event )
{
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
     wx_filter,
     wxSAVE | wxCHANGE_DIR /* wxCHANGE_DIR -> remember the last directory used. */
     );
  
  // do not force the directory, let wxWidgets choose for the current one
  //  p_dlg->SetDirectory(wxGetHomeDir());
  
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
  ThotCallback (MyRef, INTEGER_DATA, (char*) 3);
  XRCCTRL(*this, "wxID_DOC_LOCATION_CTRL", wxTextCtrl)->SetValue(TtaConvMessageToWX( SavePath));
  XRCCTRL(*this, "wxID_IMG_LOCATION_CTRL", wxTextCtrl)->SetValue(TtaConvMessageToWX( SaveImgsURL));
}

/*----------------------------------------------------------------------
  OnCharsetCbx called when selecting a Charset Combobox entry
  ----------------------------------------------------------------------*/
void SaveAsDlgWX::OnCharsetCbx( wxCommandEvent& event )
{
  wxString wx_charset_sel = XRCCTRL(*this, "wxID_CHARSET_CB", wxComboBox)->GetValue ();

  wxASSERT( wx_charset_sel.Len() < MAX_LENGTH );
  strcpy( UserCharset, (const char*)wx_charset_sel.mb_str(wxConvUTF8) );
}

/*----------------------------------------------------------------------
  OnMimeTypeCbx called when selecting a MimeType Combobox entry
  ----------------------------------------------------------------------*/
void SaveAsDlgWX::OnMimeTypeCbx( wxCommandEvent& event )
{
  wxString wx_mimetype = XRCCTRL(*this, "wxID_MIME_TYPE_CB", wxComboBox)->GetValue ();

  wxASSERT( wx_mimetype.Len() < MAX_LENGTH );
  strcpy( UserMimeType, (const char*)wx_mimetype.mb_str(wxConvUTF8) );
}

/*----------------------------------------------------------------------
  OnDocFormatBox called when clicking the Document Format radio box
  ----------------------------------------------------------------------*/
void SaveAsDlgWX::OnDocFormatBox ( wxCommandEvent& event )
{
  ThotCallback (BaseDialog + RadioSave, INTEGER_DATA,
                (char*) (XRCCTRL(*this, "wxID_DOC_FORMAT", wxRadioBox)->GetSelection( )));
  if (SaveFileName[0] != EOS)
    // update the filename (the suffix can change)
    XRCCTRL(*this, "wxID_DOC_LOCATION_CTRL", wxTextCtrl)->SetValue(TtaConvMessageToWX(SaveFileName));
}

/*---------------------------------------------------------------
  OnImagesChkBox
  ---------------------------------------------------------------*/
void SaveAsDlgWX::OnImagesChkBox ( wxCommandEvent& event )
{
  CopyImages = XRCCTRL(*this, "wxID_CPY_IMAGES_CHK", wxCheckBox)->GetValue();
}

/*---------------------------------------------------------------
  OnUrlsChkBox
  ---------------------------------------------------------------*/
void SaveAsDlgWX::OnUrlsChkBox ( wxCommandEvent& event )
{
  UpdateURLs = XRCCTRL(*this, "wxID_TRANSFORM_URLS_CHK", wxCheckBox)->GetValue();
}

/*---------------------------------------------------------------
  OnDocLocation 
  ---------------------------------------------------------------*/
void SaveAsDlgWX::OnDocLocation ( wxCommandEvent& event )
{
  wxString doc_location = XRCCTRL(*this, "wxID_DOC_LOCATION_CTRL", wxTextCtrl)->GetValue( );

  // allocate a temporary buffer to convert wxString to (char *) UTF-8 buffer
  char buffer[512];
  wxASSERT( doc_location.Len() < 512 );
  strcpy( buffer, (const char*)doc_location.mb_str(wxConvUTF8) );

  ThotCallback (BaseDialog + NameSave,  STRING_DATA, (char *)buffer );
}

/*---------------------------------------------------------------
  OnImgLocation 
  ---------------------------------------------------------------*/
void SaveAsDlgWX::OnImgLocation ( wxCommandEvent& event )
{
  wxString img_location = XRCCTRL(*this, "wxID_IMG_LOCATION_CTRL", wxTextCtrl)->GetValue( );

  // allocate a temporary buffer to convert wxString to (char *) UTF-8 buffer
  char buffer[512];
  wxASSERT( img_location.Len() < 512 );
  strcpy( buffer, (const char*)img_location.mb_str(wxConvUTF8) );

  ThotCallback (BaseDialog + ImgDirSave,  STRING_DATA, (char *)buffer );
}

#endif /* _WX */
