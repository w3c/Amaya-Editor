#ifdef _WX

#include "wx/wx.h"

#define THOT_EXPORT extern
#include "amaya.h"
#include "appdialogue_wx.h"

#include "AmayaDialog.h"

//-----------------------------------------------------------------------------
// Event table: connect the events to the handler functions to process them
//-----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(AmayaDialog, wxDialog)
  EVT_CLOSE( AmayaDialog::OnClose )
END_EVENT_TABLE()

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
AmayaDialog::AmayaDialog( wxWindow * p_parent, int ref ) :
  wxDialog(),
  m_Ref(ref)
{
  wxLogDebug( _T("AmayaDialog::AmayaDialog - ref=%d"), ref );
}

/*----------------------------------------------------------------------
  Destructor. (Empty, as I don't need anything special done when destructing).
  ----------------------------------------------------------------------*/
AmayaDialog::~AmayaDialog()
{
  wxLogDebug( _T("AmayaDialog::~AmayaDialog") );
}

/*----------------------------------------------------------------------
  OnClose
  ----------------------------------------------------------------------*/
void AmayaDialog::OnClose( wxCloseEvent& event )
{
  wxLogDebug(_T("AmayaDialog - OnClose") );


  // maybe it's better to use :
  // this callback is usefull for :
  // - Add CSS / Create link
  // because it cancel the link creation process.
  // TODO : verifier que l'appel generique de cette callback ne gene pas pour certains dialogues.
  ThotCallback (m_Ref, INTEGER_DATA, (char*) 0);

  TtaDestroyDialogue( m_Ref );

  //  forward the event to parents
  event.Skip();
}

#endif /* _WX */
