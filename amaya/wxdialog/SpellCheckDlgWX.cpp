#ifdef _WX

#include "wx/wx.h"
#include "wx/xrc/xmlres.h"              // XRC XML resouces

#include "AmayaApp.h"
#include "SpellCheckDlgWX.h"

#define THOT_EXPORT extern
#include "amaya.h"
#include "appdialogue_wx.h"
#include "message_wx.h"

//-----------------------------------------------------------------------------
// Event table: connect the events to the handler functions to process them
//-----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(SpellCheckDlgWX, AmayaDialog)
  EVT_BUTTON( XRCID("wxID_CANCEL_BUTTON"),      SpellCheckDlgWX::OnCancelButton )
  /*
  EVT_CHECKBOX( XRCID("wxID_MANUAL_CHK"),      SpellCheckDlgWX::OnManualChkBox )
 EVT_TEXT( XRCID("wxID_FILE_TXT_CTRL"),        SpellCheckDlgWX::OnTypePrinter )
  */
 END_EVENT_TABLE()

/*----------------------------------------------------------------------
  SpellCheckDlgWX create the Spell Checker dialog 
  params:
    + parent : parent window
    + ps_file : postscript file
  ----------------------------------------------------------------------*/
SpellCheckDlgWX::SpellCheckDlgWX( int ref,
				  wxWindow* parent ) :
  AmayaDialog( NULL, ref )
{
  int  page_size;

  wxXmlResource::Get()->LoadDialog(this, parent, wxT("SpellCheckDlgWX"));
  wxLogDebug( _T("SpellCheckDlgWX::SpellCheckDlgWX"));
  wxString wx_title = TtaConvMessageToWX( TtaGetMessage (LIB, TMSG_Correct) );
  SetTitle( wx_title );

  // proposals

  // search area radio box

  // 'ignore' check list
  /*
  XRCCTRL(*this, "wxID_OPTIONS_TXT", wxStaticText)->SetLabel(TtaConvMessageToWX( TtaGetMessage(LIB, TMSG_OPTIONS) ));
  XRCCTRL(*this, "wxID_MANUAL_CHK", wxCheckBox)->SetLabel(TtaConvMessageToWX( TtaGetMessage(LIB, TMSG_MANUAL_FEED) ));
  XRCCTRL(*this, "wxID_TOC_CHK", wxCheckBox)->SetLabel(TtaConvMessageToWX( TtaGetMessage(AMAYA, AM_PRINT_TOC) ));
  XRCCTRL(*this, "wxID_LINKS_CHK", wxCheckBox)->SetLabel(TtaConvMessageToWX( TtaGetMessage(AMAYA, AM_NUMBERED_LINKS) ));
  XRCCTRL(*this, "wxID_PRINT_URL_CHK", wxCheckBox)->SetLabel(TtaConvMessageToWX( TtaGetMessage(AMAYA, AM_PRINT_URL) ));
  XRCCTRL(*this, "wxID_IGNORE_CSS_CHK", wxCheckBox)->SetLabel(TtaConvMessageToWX( TtaGetMessage(AMAYA, AM_WITH_CSS) ));
  XRCCTRL(*this, "wxID_MANUAL_CHK", wxCheckBox)->SetValue(FALSE);
  XRCCTRL(*this, "wxID_TOC_CHK", wxCheckBox)->SetValue(FALSE);
  XRCCTRL(*this, "wxID_LINKS_CHK", wxCheckBox)->SetValue(FALSE);
  XRCCTRL(*this, "wxID_PRINT_URL_CHK", wxCheckBox)->SetValue(TRUE);
  XRCCTRL(*this, "wxID_IGNORE_CSS_CHK", wxCheckBox)->SetValue(FALSE);
  */

  // buttons
  XRCCTRL(*this, "wxID_CANCEL_BUTTON", wxButton)->SetLabel(TtaConvMessageToWX( TtaGetMessage(LIB, TMSG_CANCEL) ));

  // Set focus to ...
  //  XRCCTRL(*this, "wxID_FILE_TXT_CTRL", wxTextCtrl)->SetFocus();

  Layout();
  
  SetAutoLayout( TRUE );
}

/*----------------------------------------------------------------------
  Destructor. (Empty, as I don't need anything special done when destructing).
  ----------------------------------------------------------------------*/
SpellCheckDlgWX::~SpellCheckDlgWX()
{
}

/*----------------------------------------------------------------------
  OnCancelButton called when clicking on cancelbutton
  ----------------------------------------------------------------------*/
void SpellCheckDlgWX::OnCancelButton( wxCommandEvent& event )
{
  ThotCallback (BasePrint + FormPrint, INTEGER_DATA, (char*) 0);
}

/*---------------------------------------------------------------
  OnManualChkBox
  ---------------------------------------------------------------*/
/*
void SpellCheckDlgWX::OnManualChkBox ( wxCommandEvent& event )
{
  wxLogDebug( _T("PrintDlgWX::OnManualChkBox") );
  ThotCallback (BasePrint + PrintOptions, INTEGER_DATA,	(char*) 0);
}
*/
#endif /* _WX */
