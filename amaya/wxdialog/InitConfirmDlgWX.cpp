#ifdef _WX

#include "wx/wx.h"
#include "wx/xrc/xmlres.h"              // XRC XML resouces
#include "wx/valgen.h"

#define THOT_EXPORT extern
#include "amaya.h"
#include "appdialogue_wx.h"

#include "InitConfirmDlgWX.h"

//-----------------------------------------------------------------------------
// Event table: connect the events to the handler functions to process them
//-----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(InitConfirmDlgWX, AmayaDialog)
  EVT_BUTTON( XRCID("wxID_EXTRABUTTON"), InitConfirmDlgWX::OnExtraButton )
  EVT_BUTTON( XRCID("wxID_CONFIRMBUTTON"), InitConfirmDlgWX::OnConfirmButton )
  EVT_BUTTON( XRCID("wxID_CANCELBUTTON"), InitConfirmDlgWX::OnCancelButton )
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
				    const wxString & label ) :
  AmayaDialog( NULL, ref )
{
  wxXmlResource::Get()->LoadDialog(this, parent, wxT("InitConfirmDlgWX"));
  m_Label         = label;
  m_Title         = title;
  m_ExtraButton   = extrabutton;
  m_ConfirmButton = confirmbutton;
  // TODO : aller chercher le label 'Cancel' dans une ressources thot
  m_CancelButton  = _T("Cancel");

  wxLogDebug( _T("InitConfirmDlgWX::InitConfirmDlgWX - title=")+m_Title+
	      _T("\textrabutton=")+m_ExtraButton+
	      _T("\tconfirmbutton=")+m_ConfirmButton+
	      _T("\tlabel=")+m_Label );
 
  // update dialog labels with given ones
  SetTitle( m_Title );
  //  XRCCTRL(*this, "wxID_LABEL",         wxStaticText)->SetValidator( wxGenericValidator(&m_Label) );
  XRCCTRL(*this, "wxID_LABEL",         wxStaticText)->SetLabel( m_Label );
  XRCCTRL(*this, "wxID_EXTRABUTTON",   wxButton)->SetLabel( m_ExtraButton );
  XRCCTRL(*this, "wxID_CONFIRMBUTTON", wxButton)->SetLabel( m_ConfirmButton );
  XRCCTRL(*this, "wxID_CANCELBUTTON",  wxButton)->SetLabel( m_CancelButton );
  
  // now hide unused button (destroy it)
  if (m_ExtraButton.IsEmpty())
    XRCCTRL(*this, "wxID_EXTRABUTTON",     wxButton)->Destroy();
  if (m_ConfirmButton.IsEmpty())
    XRCCTRL(*this, "wxID_CONFIRMBUTTON",   wxButton)->Destroy();
  if (m_CancelButton.IsEmpty())
    XRCCTRL(*this, "wxID_CANCELBUTTON",    wxButton)->Destroy();

  // TODO : trouver une facon de redimensionner le dialogue pour qu'il prenne la taille du nouveau label
  //wxSize s1 = XRCCTRL(*this, "wxID_SIZER1",         wxSizer)->GetSize();
  //wxSize s2 = XRCCTRL(*this, "wxID_SIZER2",         wxSizer)->GetSize();
  //SetSize( 100, -1 );
  //GetSizer()->Fit( this );
  //GetSizer()->SetSizeHints( this );  

  Layout();
  
  SetAutoLayout( TRUE );
}

/*----------------------------------------------------------------------
  Destructor. (Empty, as I don't need anything special done when destructing).
  ----------------------------------------------------------------------*/
InitConfirmDlgWX::~InitConfirmDlgWX()
{
  wxLogDebug( _T("InitConfirmDlgWX::~InitConfirmDlgWX") );

  /* when the dialog is destroyed it's important
     to restore these global variable to default value 
     because if the user click on X (OnClose callback),
     the dialog is just destroyed so precedent global stats will be used */
  UserAnswer = 0;
  ExtraChoice = 0;
}

/*----------------------------------------------------------------------
  OnExtraButton called when clicking on extrabutton
  ----------------------------------------------------------------------*/
void InitConfirmDlgWX::OnExtraButton( wxCommandEvent& event )
{
  ThotCallback (m_Ref, INTEGER_DATA, (char*) 1);
}

/*----------------------------------------------------------------------
  OnConfirmButton called when clicking on confirmbutton
  ----------------------------------------------------------------------*/
void InitConfirmDlgWX::OnConfirmButton( wxCommandEvent& event )
{
  ThotCallback (m_Ref, INTEGER_DATA, (char*) 2);
}

/*----------------------------------------------------------------------
  OnCancelButton called when clicking on cancelbutton
  ----------------------------------------------------------------------*/
void InitConfirmDlgWX::OnCancelButton( wxCommandEvent& event )
{
  ThotCallback (m_Ref, INTEGER_DATA, (char*) 0); 
}

#endif /* _WX */
