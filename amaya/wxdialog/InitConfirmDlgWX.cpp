#ifdef _WX

#include "wx/wx.h"
#include "wx/xrc/xmlres.h"              // XRC XML resouces
#include "wx/valgen.h"

#define THOT_EXPORT extern
#include "amaya.h"
#include "appdialogue_wx.h"
#include "message_wx.h"

#include "InitConfirmDlgWX.h"

//-----------------------------------------------------------------------------
// Event table: connect the events to the handler functions to process them
//-----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(InitConfirmDlgWX, AmayaDialog)
  EVT_BUTTON( XRCID("wxID_EXTRABUTTON"), InitConfirmDlgWX::OnExtraButton )
  EVT_BUTTON( XRCID("wxID_OK"), InitConfirmDlgWX::OnConfirmButton )
  EVT_BUTTON( XRCID("wxID_CANCEL"), InitConfirmDlgWX::OnCancelButton )
END_EVENT_TABLE()

/*----------------------------------------------------------------------
  InitConfirmDlgWX create the dialog for document changes (save/not save) comfirmation.
  params:
    + parent : parent window
    + title : dialog title
    + extrabutton : if it is present there is 3 button
    + confirmbutton : 
    + label : the message to show at dialog center
  returns:
  ----------------------------------------------------------------------*/
InitConfirmDlgWX::InitConfirmDlgWX( int ref,
				    wxWindow* parent,
				    const wxString & title,
				    const wxString & extrabutton,
				    const wxString & confirmbutton,
				    const wxString & label,
				    const wxString & label2,
				    const wxString & label3 ) :
  AmayaDialog( parent, ref )
{
wxString cancelbutton;

  wxXmlResource::Get()->LoadDialog(this, parent, wxT("InitConfirmDlgWX"));
  // update dialog labels with given ones
  SetTitle( title );
  XRCCTRL(*this, "wxID_LABEL", wxStaticText)->SetLabel( label );
  
  // now hide unused field and buttons (destroy it)
  wxSizer * p_sizer = XRCCTRL(*this, "wxID_LABEL", wxStaticText)->GetContainingSizer();
  if (label2.IsEmpty())
    XRCCTRL(*this, "wxID_LABEL_2", wxStaticText)->Hide();
  else
    XRCCTRL(*this, "wxID_LABEL_2", wxStaticText)->SetLabel( label2 );
  if (label3.IsEmpty())
    XRCCTRL(*this, "wxID_LABEL_3", wxStaticText)->Hide();
  else
    XRCCTRL(*this, "wxID_LABEL_3", wxStaticText)->SetLabel( label3 );
  p_sizer->Layout();

  p_sizer = XRCCTRL(*this, "wxID_EXTRABUTTON", wxButton)->GetContainingSizer();
  if (extrabutton.IsEmpty())
    p_sizer->Show(XRCCTRL(*this, "wxID_EXTRABUTTON", wxButton), false);
  else
    XRCCTRL(*this, "wxID_EXTRABUTTON", wxButton)->SetLabel( extrabutton );
  if (confirmbutton.IsEmpty())
    {
    p_sizer->Show(XRCCTRL(*this, "wxID_OK", wxButton), false);
    cancelbutton = TtaConvMessageToWX(TtaGetMessage(LIB, TMSG_LIB_CONFIRM));
    }
  else
    {
    XRCCTRL(*this, "wxID_OK", wxButton)->SetLabel( confirmbutton );
    cancelbutton = TtaConvMessageToWX(TtaGetMessage(LIB, TMSG_CANCEL));
    }
  if (cancelbutton.IsEmpty())
    p_sizer->Show(XRCCTRL(*this, "wxID_CANCELBUTTON", wxButton), false);
  else
    XRCCTRL(*this, "wxID_CANCEL",  wxButton)->SetLabel( cancelbutton );
  p_sizer->Layout();
  
  // give default focus to ... 
  //XRCCTRL(*this, "wxID_EXTRABUTTON",     wxButton)->SetDefault();
  
  // TODO : trouver une facon de redimensionner le dialogue pour qu'il prenne la taille du nouveau label
  //wxSize s1 = XRCCTRL(*this, "wxID_SIZER1",         wxSizer)->GetSize();
  //wxSize s2 = XRCCTRL(*this, "wxID_SIZER2",         wxSizer)->GetSize();
  //SetSize( 100, -1 );
  //GetSizer()->Fit( this );
  //GetSizer()->SetSizeHints( this );  

  Fit();
  Refresh();
  

  SetAutoLayout( TRUE );
}

/*----------------------------------------------------------------------
  Destructor. (Empty, as I don't need anything special done when destructing).
  ----------------------------------------------------------------------*/
InitConfirmDlgWX::~InitConfirmDlgWX()
{
}

/*----------------------------------------------------------------------
  OnExtraButton called when clicking on extrabutton
  ----------------------------------------------------------------------*/
void InitConfirmDlgWX::OnExtraButton( wxCommandEvent& event )
{
  ThotCallback (m_Ref, INTEGER_DATA, (char*) 2);
}

/*----------------------------------------------------------------------
  OnConfirmButton called when clicking on confirmbutton
  ----------------------------------------------------------------------*/
void InitConfirmDlgWX::OnConfirmButton( wxCommandEvent& event )
{
  ThotCallback (m_Ref, INTEGER_DATA, (char*) 1);
}

/*----------------------------------------------------------------------
  OnCancelButton called when clicking on cancelbutton
  ----------------------------------------------------------------------*/
void InitConfirmDlgWX::OnCancelButton( wxCommandEvent& event )
{
  ThotCallback (m_Ref, INTEGER_DATA, (char*) 0); 
}

#endif /* _WX */
