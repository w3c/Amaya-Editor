/*
 *
 *  (c) COPYRIGHT INRIA and W3C, 1996-2009
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

#ifdef _WX
#include "wx/wx.h"
#include "wx/xrc/xmlres.h"              // XRC XML resouces
#include "wx/string.h"

#include "AmayaApp.h"
#include "MetaDlgWX.h"

#define THOT_EXPORT extern
#include "amaya.h"
#include "wxinclude.h"
#include "appdialogue_wx.h"
#include "message_wx.h"
static   int Myref;

//-----------------------------------------------------------------------------
// Event table: connect the events to the handler functions to process them
//-----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(MetaDlgWX, AmayaDialog)
  EVT_BUTTON(     XRCID("wxID_OK"),      MetaDlgWX::OnConfirmButton )
  EVT_BUTTON(     XRCID("wxID_CANCEL"),  MetaDlgWX::OnCancelButton )
  EVT_TEXT_ENTER( XRCID("wxID_COTENT"),   MetaDlgWX::OnConfirmButton )
  EVT_TEXT_ENTER( XRCID("wxID_NAME"),   MetaDlgWX::OnConfirmButton )
  EVT_TEXT_ENTER( XRCID("wxID_HTTPEQUIV"),   MetaDlgWX::OnConfirmButton )
END_EVENT_TABLE()

/*----------------------------------------------------------------------
  MetaDlgWX create the dialog to change the title
  params:
    + parent : parent window
    + title : dialog title
    + doc_title : the current document title
  returns:
  ----------------------------------------------------------------------*/
  MetaDlgWX::MetaDlgWX( int ref, wxWindow* parent) :
    AmayaDialog( parent, ref )
{
  Myref = ref;
  wxXmlResource::Get()->LoadDialog(this, parent, wxT("MetaDlgWX"));
  // update dialog labels
  XRCCTRL(*this, "wxID_CANCEL", wxButton)->SetLabel( TtaConvMessageToWX( TtaGetMessage (LIB, TMSG_CANCEL) ));
  XRCCTRL(*this, "wxID_OK", wxButton)->SetLabel( TtaConvMessageToWX( TtaGetMessage (LIB, TMSG_LIB_CONFIRM) ));
  SetAutoLayout( TRUE );
}

/*----------------------------------------------------------------------
  Destructor. (Empty, as I don't need anything special done when destructing).
  ----------------------------------------------------------------------*/
MetaDlgWX::~MetaDlgWX()
{
}

/*----------------------------------------------------------------------
  OnConfirmButton called when clicking on confirmbutton
  ----------------------------------------------------------------------*/
void MetaDlgWX::OnConfirmButton( wxCommandEvent& event )
{
  wxString value = XRCCTRL(*this, "wxID_CONTENT", wxTextCtrl)->GetValue( );
  if (value.Len() > 0)
    {
      MetaContent = TtaStrdup ((const char*)value.mb_str(wxConvUTF8));
      value = XRCCTRL(*this, "wxID_HTTPEQUIV", wxTextCtrl)->GetValue( );
      if (value.Len() > 0)
        MetaEquiv = TtaStrdup ((const char*)value.mb_str(wxConvUTF8));
      else
        {
          value = XRCCTRL(*this, "wxID_NAME", wxTextCtrl)->GetValue( );
          if (value.Len() > 0)
            MetaName = TtaStrdup ((const char*)value.mb_str(wxConvUTF8));
        }
    }
  ThotCallback (Myref, INTEGER_DATA, (char*) 0); 
  TtaRedirectFocus();
}

/*----------------------------------------------------------------------
  OnCancelButton called when clicking on cancelbutton
  ----------------------------------------------------------------------*/
void MetaDlgWX::OnCancelButton( wxCommandEvent& event )
{
  ThotCallback (Myref, INTEGER_DATA, (char*) 0); 
  TtaRedirectFocus();
}

#endif /* _WX */
