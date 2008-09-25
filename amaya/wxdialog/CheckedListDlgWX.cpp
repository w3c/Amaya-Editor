/*
 *
 *  (c) COPYRIGHT INRIA and W3C, 1996-2008
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

#ifdef _WX
#include "wx/wx.h"
#include "wx/xrc/xmlres.h"              // XRC XML resouces

#include "AmayaApp.h"
#include "CheckedListDlgWX.h"

#define THOT_EXPORT extern
#include "amaya.h"
#include "print.h"
#include "appdialogue_wx.h"
#include "message_wx.h"

static int      MyRef;
static int      NbEntries;
static bool     *Checks;

//-----------------------------------------------------------------------------
// Event table: connect the events to the handler functions to process them
//-----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(CheckedListDlgWX, AmayaDialog)
  EVT_BUTTON( XRCID("wxID_OK"),   CheckedListDlgWX::OnConfirmButton )
  EVT_BUTTON( XRCID("wxID_CANCEL"),    CheckedListDlgWX::OnCancelButton )
END_EVENT_TABLE()

/*----------------------------------------------------------------------
  CheckedListDlgWX create the CheckedList dialog 
  params:
    + parent : parent window
    + title : dialog title
  ----------------------------------------------------------------------*/
CheckedListDlgWX::CheckedListDlgWX( int ref,
				    wxWindow* parent,
				    const wxString & title,
				    int nb,
				    const wxArrayString& items,
				    bool *checks) :
  AmayaDialog( parent, ref )
{
  int   i;

  wxXmlResource::Get()->LoadDialog(this, parent, wxT("CheckedListDlgWX"));
  SetTitle( title );

  MyRef = ref;
  NbEntries = nb;
  Checks = checks;
  XRCCTRL(*this, "wxID_LABEL_LIST", wxStaticText)->SetLabel( title );
  XRCCTRL(*this, "wxID_LABEL_COMMENT", wxStaticText)->SetLabel( TtaConvMessageToWX( "") );
  XRCCTRL(*this, "wxID_CHECKED_LIST", wxListBox)->Clear( );
  XRCCTRL(*this, "wxID_CHECKED_LIST", wxListBox)->Append( items );
  for (i = 0; i < NbEntries; i++)
    if (Checks[i])
      XRCCTRL(*this, "wxID_CHECKED_LIST", wxCheckListBox)->Check(i, true);

  // buttons
  XRCCTRL(*this, "wxID_OK", wxButton)->SetLabel(TtaConvMessageToWX( TtaGetMessage(LIB, TMSG_LIB_CONFIRM) ));
  XRCCTRL(*this, "wxID_CANCEL", wxButton)->SetLabel(TtaConvMessageToWX( TtaGetMessage(LIB, TMSG_CANCEL) ));
  
  Layout();
  SetAutoLayout( TRUE );
}

/*---------------------------------------------------------------------------
  Destructor. (Empty, as we don't need anything special done when destructing).
  ---------------------------------------------------------------------------*/
CheckedListDlgWX::~CheckedListDlgWX()
{
}

/*----------------------------------------------------------------------
  OnConfirmButton called when clicking the Confirm button
  ----------------------------------------------------------------------*/
void CheckedListDlgWX::OnConfirmButton( wxCommandEvent& event )
{
  int   i;

  for (i = 0; i < NbEntries; i++)
    Checks[i] = XRCCTRL(*this, "wxID_CHECKED_LIST", wxCheckListBox)->IsChecked(i);

  ThotCallback (MyRef, INTEGER_DATA, (char*) 1);
}

/*----------------------------------------------------------------------
  OnCancelButton called when clicking on Cancel button
  ----------------------------------------------------------------------*/
void CheckedListDlgWX::OnCancelButton( wxCommandEvent& event )
{
  ThotCallback (MyRef, INTEGER_DATA, (char*) 0);
}

#endif /* _WX */











