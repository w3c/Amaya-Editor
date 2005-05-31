#ifdef _WX

#include "wx/wx.h"
#include "wx/xrc/xmlres.h"              // XRC XML resouces

#include "AmayaApp.h"
#include "DocInfoDlgWX.h"

#define THOT_EXPORT extern
#include "amaya.h"
#include "appdialogue_wx.h"
#include "message_wx.h"
#include "init_f.h"

//-----------------------------------------------------------------------------
// Event table: connect the events to the handler functions to process them
//-----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(DocInfoDlgWX, AmayaDialog)
  EVT_BUTTON( XRCID("wxID_CANCEL"), DocInfoDlgWX::OnDoneButton )
END_EVENT_TABLE()

/*----------------------------------------------------------------------
  DocInfoDlgWX create the Document Info dialog 
  params:
    + parent : parent window
    + doc : the document
  ----------------------------------------------------------------------*/
  DocInfoDlgWX::DocInfoDlgWX( int ref, 
			      wxWindow* parent,
			      int doc ) : 
    AmayaDialog( parent, ref )
{
  char         *content;

  wxXmlResource::Get()->LoadDialog(this, parent, wxT("DocInfoDlgWX"));

  wxString wx_title = TtaConvMessageToWX(TtaGetMessage(AMAYA, AM_DOCINFO_TITLE));
  SetTitle( wx_title );

   /* Document URL */
  XRCCTRL(*this, "wxID_URL_LABEL", wxStaticText)->SetLabel(TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_HREF_VALUE)));
  if (DocumentURLs[doc] != NULL)
    content = DocumentURLs[doc];
  else
  content = TtaGetMessage (AMAYA, AM_UNKNOWN);
  XRCCTRL(*this, "wxID_URL_CONTENT", wxStaticText)->SetLabel(TtaConvMessageToWX(content));

   /* Document type */
  XRCCTRL(*this, "wxID_DOCUMENT_TYPE_LABEL", wxStaticText)->SetLabel(TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_DOCINFO_TYPE)));
  content = DocumentTypeString (doc);
  if (!content)
    content = TtaGetMessage (AMAYA, AM_UNKNOWN);
  XRCCTRL(*this, "wxID_DOCUMENT_TYPE_CONTENT", wxStaticText)->SetLabel(TtaConvMessageToWX(content));

   /* Mime Type */
  XRCCTRL(*this, "wxID_MIME_TYPE_LABEL", wxStaticText)->SetLabel(TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_DOCINFO_TYPEMIME)));
  if (DocumentMeta[doc] && DocumentMeta[doc]->content_type)
    content = DocumentMeta[doc]->content_type;
  else
    content = TtaGetMessage (AMAYA, AM_UNKNOWN);
  XRCCTRL(*this, "wxID_MIME_TYPE_CONTENT", wxStaticText)->SetLabel(TtaConvMessageToWX(content));

   /* Charset */
  XRCCTRL(*this, "wxID_CHARSET_LABEL", wxStaticText)->SetLabel(TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_DOCINFO_CHARSET)));
  if (DocumentMeta[doc] && DocumentMeta[doc]->charset != NULL)
    content = DocumentMeta[doc]->charset;
  else
    content = TtaGetMessage (AMAYA, AM_UNKNOWN);
  XRCCTRL(*this, "wxID_CHARSET_CONTENT", wxStaticText)->SetLabel(TtaConvMessageToWX(content));

   /* Content Length */
  XRCCTRL(*this, "wxID_LENGTH_LABEL", wxStaticText)->SetLabel(TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_DOCINFO_CLENGTH)));
  if (DocumentMeta[doc] && DocumentMeta[doc]->content_length)
    content = DocumentMeta[doc]->content_length;
  else
    content = TtaGetMessage (AMAYA, AM_UNKNOWN);
  XRCCTRL(*this, "wxID_LENGTH_CONTENT", wxStaticText)->SetLabel(TtaConvMessageToWX(content));

   /* Content Location */
  XRCCTRL(*this, "wxID_LOCATION_LABEL", wxStaticText)->SetLabel(TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_DOCINFO_CLOCATION)));
  if (DocumentMeta[doc] 
      && DocumentMeta[doc]->full_content_location != NULL)
    content = DocumentMeta[doc]->full_content_location;
  else
    content = TtaGetMessage (AMAYA, AM_UNKNOWN);
  XRCCTRL(*this, "wxID_LOCATION_CONTENT", wxStaticText)->SetLabel(TtaConvMessageToWX(content));

  // update dialog labels
  XRCCTRL(*this, "wxID_CANCEL", wxButton)->SetLabel(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_DONE)));

  Layout();
  
  SetAutoLayout( TRUE );
}

/*-----------------------------------------------------------------------------
  Destructor. (Empty, as I don't need anything special done when destructing).
  -----------------------------------------------------------------------------*/
DocInfoDlgWX::~DocInfoDlgWX()
{
}

/*----------------------------------------------------------------------
  OnDoneButton
  ----------------------------------------------------------------------*/
void DocInfoDlgWX::OnDoneButton( wxCommandEvent& event )
{
  Close ();
}

#endif /* _WX */
