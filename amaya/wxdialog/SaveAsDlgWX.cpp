#ifdef _WX

#include "wx/wx.h"
#include "wx/xrc/xmlres.h"              // XRC XML resouces

#include "AmayaApp.h"
#include "SaveAsDlgWX.h"

#define THOT_EXPORT extern
#include "amaya.h"
#include "print.h"
#include "appdialogue_wx.h"

//-----------------------------------------------------------------------------
// Event table: connect the events to the handler functions to process them
//-----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(SaveAsDlgWX, AmayaDialog)
  EVT_BUTTON( XRCID("wxID_CONFIRMBUTTON"),       SaveAsDlgWX::OnConfirmButton )
  EVT_BUTTON( XRCID("wxID_CANCELBUTTON"),        SaveAsDlgWX::OnCancelButton )
  EVT_BUTTON( XRCID("wxID_BROWSEBUTTON"),        SaveAsDlgWX::OnBrowseButton )
  EVT_BUTTON( XRCID("wxID_CLEARBUTTON"),         SaveAsDlgWX::OnClearButton )
  EVT_BUTTON( XRCID("wxID_ENCODINGBUTTON"),      SaveAsDlgWX::OnEncodingButton )
  EVT_BUTTON( XRCID("wxID_MIMETYPEBUTTON"),      SaveAsDlgWX::OnMimeTypeButton )
  EVT_RADIOBOX( XRCID("wxID_DOC_FORMAT"),        SaveAsDlgWX::OnDocFormatBox )
  EVT_CHECKBOX( XRCID("wxID_CPY_IMAGES_CHK"),    SaveAsDlgWX::OnImagesChkBox )
  EVT_CHECKBOX( XRCID("wxID_TRANSFORM_URLS_CHK"),SaveAsDlgWX::OnUrlsChkBox )
  EVT_TEXT( XRCID("wxID_DOC_LOCATION_CTRL"),     SaveAsDlgWX::OnDocLocation )
  EVT_TEXT( XRCID("wxID_IMG_LOCATION_CTRL"),     SaveAsDlgWX::OnImgLocation )
END_EVENT_TABLE()

/*----------------------------------------------------------------------
  SaveAsDlgWX create the SaveAs dialog 
  params:
    + parent : parent window
    + pathname : document location
  ----------------------------------------------------------------------*/
SaveAsDlgWX::SaveAsDlgWX( int ref,
			  wxWindow* parent,
			  const wxString & pathname ) :
  AmayaDialog( NULL, ref )
{
  int  page_size;

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
/* XRCCTRL(*this, "wxID_IMG_LOCATION_CTRL", wxTextCtrl)->SetValue(pathname); */

  // Charset and Mime Type infos
  /* + infos charset et type mime */
  XRCCTRL(*this, "wxID_CHARSET", wxStaticText)->SetLabel(TtaConvMessageToWX( "Charset: " ));
  XRCCTRL(*this, "wxID_MIME_TYPE", wxStaticText)->SetLabel(TtaConvMessageToWX( "MIME type:" ));

  // buttons
  XRCCTRL(*this, "wxID_CONFIRMBUTTON", wxButton)->SetLabel(TtaConvMessageToWX( TtaGetMessage(LIB, TMSG_LIB_CONFIRM) ));
  XRCCTRL(*this, "wxID_CANCELBUTTON", wxButton)->SetLabel(TtaConvMessageToWX( TtaGetMessage(LIB, TMSG_CANCEL) ));
  XRCCTRL(*this, "wxID_BROWSEBUTTON", wxButton)->SetLabel(TtaConvMessageToWX( TtaGetMessage(AMAYA, AM_BROWSE) ));
  XRCCTRL(*this, "wxID_CLEARBUTTON", wxButton)->SetLabel(TtaConvMessageToWX( TtaGetMessage(AMAYA, AM_CLEAR) ));
  XRCCTRL(*this, "wxID_ENCODINGBUTTON", wxButton)->SetLabel(TtaConvMessageToWX( TtaGetMessage(AMAYA, AM_CHANGE_CHARSET) ));
  XRCCTRL(*this, "wxID_MIMETYPEBUTTON", wxButton)->SetLabel(TtaConvMessageToWX( TtaGetMessage(AMAYA, AM_CHANGE_MIME_TYPE) ));
  
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
  ThotCallback (BaseDialog + SaveForm, INTEGER_DATA, (char*) 0);
}

/*----------------------------------------------------------------------
  OnClearButton called when clicking on Clear button
  ----------------------------------------------------------------------*/
void SaveAsDlgWX::OnClearButton( wxCommandEvent& event )
{
  wxLogDebug( _T("SaveAsDlgWX::OnClearButton") );
  ThotCallback (BaseDialog + SaveForm, INTEGER_DATA, (char*) 0);
}

/*----------------------------------------------------------------------
  OnEncodingButton called when clicking on Encoding button
  ----------------------------------------------------------------------*/
void SaveAsDlgWX::OnEncodingButton( wxCommandEvent& event )
{
  wxLogDebug( _T("SaveAsDlgWX::OnEncodingButton") );
  ThotCallback (BaseDialog + SaveForm, INTEGER_DATA, (char*) 0);
}

/*----------------------------------------------------------------------
  OnMimeTypeButton called when clicking on MimeType button
  ----------------------------------------------------------------------*/
void SaveAsDlgWX::OnMimeTypeButton( wxCommandEvent& event )
{
  wxLogDebug( _T("SaveAsDlgWX::OnMimeTypeButton") );
  ThotCallback (BaseDialog + SaveForm, INTEGER_DATA, (char*) 0);
}

/*----------------------------------------------------------------------
  OnDocFormatBox called when clicking the Document Format radio box
  ----------------------------------------------------------------------*/
void SaveAsDlgWX::OnDocFormatBox ( wxCommandEvent& event )
{
  wxLogDebug( _T("SaveAsDlgWX::OnDocFormatBox") );
  /*
  ThotCallback (BaseDialog + SaveForm, INTEGER_DATA,
		(char*) (XRCCTRL(*this, "wxID_PAPER_FORMAT_BOX", wxRadioBox)->GetSelection( )));
  */
}

/*---------------------------------------------------------------
  OnImagesChkBox
  ---------------------------------------------------------------*/
void SaveAsDlgWX::OnImagesChkBox ( wxCommandEvent& event )
{
  wxLogDebug( _T("SaveAsDlgWX::OnImagesChkBox") );
  ThotCallback (BaseDialog + SaveForm, INTEGER_DATA, (char*) 4);
}

/*---------------------------------------------------------------
  OnUrlsChkBox
  ---------------------------------------------------------------*/
void SaveAsDlgWX::OnUrlsChkBox ( wxCommandEvent& event )
{
  wxLogDebug( _T("SaveAsDlgWX::OnUrlsChkBox") );
  ThotCallback (BaseDialog + SaveForm, INTEGER_DATA, (char*) 4);
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
  ThotCallback (BaseDialog + SaveForm,  STRING_DATA, (char *)buffer );
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
  ThotCallback (BaseDialog + SaveForm,  STRING_DATA, (char *)buffer );
}

#endif /* _WX */
