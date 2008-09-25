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
#include "SpellCheckDlgWX.h"

#define THOT_EXPORT extern
#include "amaya.h"
#include "appdialogue_wx.h"
#include "message_wx.h"

//-----------------------------------------------------------------------------
// Event table: connect the events to the handler functions to process them
//-----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(SpellCheckDlgWX, AmayaDialog)
  EVT_BUTTON( XRCID("wxID_CANCEL"),      SpellCheckDlgWX::OnCancelButton )
  EVT_BUTTON( XRCID("wxID_SEARCH_SKIP_BUTTON"), SpellCheckDlgWX::OnSkipWithoutButton )
  EVT_BUTTON( XRCID("wxID_SKIP_DIC_BUTTON"),    SpellCheckDlgWX::OnSkipWithButton )
  EVT_BUTTON( XRCID("wxID_REPLACE_NEXT_BUTTON"),SpellCheckDlgWX::OnReplaceWithoutButton )
  EVT_CHECKBOX( XRCID("wxID_IGNORE1_CHK"),      SpellCheckDlgWX::OnIgnoreCapitalsChkBox )
  EVT_CHECKBOX( XRCID("wxID_IGNORE2_CHK"),      SpellCheckDlgWX::OnIgnoreArabicsChkBox )
  EVT_CHECKBOX( XRCID("wxID_IGNORE3_CHK"),      SpellCheckDlgWX::OnIgnoreRomansChkBox )
  EVT_CHECKBOX( XRCID("wxID_IGNORE4_CHK"),      SpellCheckDlgWX::OnIgnoreSpecialsChkBox )
  EVT_RADIOBOX( XRCID("wxID_CHECKING_AREA"),    SpellCheckDlgWX::OnAreaRadioBox )
  EVT_TEXT( XRCID("wxID_IGNORE_CHAR"),          SpellCheckDlgWX::OnSpecialChar )
  EVT_LISTBOX( XRCID("wxID_PROPOSALS_LIST"),    SpellCheckDlgWX::OnChangeProposals)
  EVT_LISTBOX_DCLICK( XRCID("wxID_PROPOSALS_LIST"), SpellCheckDlgWX::OnReplaceWithoutButton ) 
 END_EVENT_TABLE()


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void SpellCheckDlgWX::Set_Proposals ( )
{
  char  *proposal;
  char  *lang;
  int    i;

  //update informations 
  // selected text
  TtaGetProposal (&proposal, 0);
  XRCCTRL(*this, "wxID_SELECTED_TXT", wxButton)->SetLabel(TtaConvMessageToWX( proposal ) );
  if (proposal[0] == EOS || !strcmp (proposal, " "))
    XRCCTRL(*this, "wxID_SPELL_FINISHED", wxStaticText)->SetLabel( TtaConvMessageToWX (TtaGetMessage (LIB, TMSG_END_CHECK)));
  else
    {
      XRCCTRL(*this, "wxID_SPELL_FINISHED", wxStaticText)->SetLabel( TtaConvMessageToWX (""));
      // list of proposals
      for (i = 1; i <= m_max_proposals; i++)
        {
          TtaGetProposal (&proposal, i);
          if (proposal && strcmp (proposal, "$") != 0)
            XRCCTRL(*this, "wxID_PROPOSALS_LIST", wxListBox)->SetString (i-1, TtaConvMessageToWX( proposal ));
	  else
            XRCCTRL(*this, "wxID_PROPOSALS_LIST", wxListBox)->SetString (i-1, TtaConvMessageToWX(""));
        }
      XRCCTRL(*this, "wxID_PROPOSALS_LIST", wxListBox)->SetSelection(0);
      // default correction
      TtaGetProposal (&proposal, 1);
      if (proposal && strcmp (proposal, "$") != 0)
        XRCCTRL(*this, "wxID_FIRST_PROPOSAL", wxTextCtrl)->SetValue(TtaConvMessageToWX( proposal ) );
      else
        XRCCTRL(*this, "wxID_FIRST_PROPOSAL", wxTextCtrl)->SetValue(TtaConvMessageToWX("") );
      //checker language
      TtaGetChkrLanguageName (&lang);
      char buffer[100];
      strcpy (buffer, TtaGetMessage (LIB, TMSG_LANGUAGE) );
      strcat (buffer, ": ");
      strcat (buffer, lang);
      XRCCTRL(*this, "wxID_SPELL_LANGUAGE_TXT", wxStaticText)->SetLabel(TtaConvMessageToWX( buffer ));
    }
}

/*----------------------------------------------------------------------
  SpellCheckDlgWX create the Spell Checker dialog 
  params:
    + parent : parent window
    + ps_file : postscript file
  ----------------------------------------------------------------------*/
SpellCheckDlgWX::SpellCheckDlgWX( int ref, int base, wxWindow* parent,
                                  int checkingArea) :
  AmayaDialog( parent, ref )
{
  m_base = base;
  m_ref = ref;

  wxXmlResource::Get()->LoadDialog(this, parent, wxT("SpellCheckDlgWX"));
  wxString wx_title = TtaConvMessageToWX( TtaGetMessage (LIB, TMSG_Correct) );
  SetTitle( wx_title );

  // proposals
  XRCCTRL(*this, "wxID_SELECTED_TXT",   wxButton)->SetLabel(TtaConvMessageToWX( "") );
  XRCCTRL(*this, "wxID_PROPOSALS_LIST", wxListBox)->SetString(0, TtaConvMessageToWX( "" ));
  XRCCTRL(*this, "wxID_PROPOSALS_LIST", wxListBox)->SetString(1, TtaConvMessageToWX( "" ));
  XRCCTRL(*this, "wxID_PROPOSALS_LIST", wxListBox)->SetString(2, TtaConvMessageToWX( "" ));
  XRCCTRL(*this, "wxID_PROPOSALS_LIST", wxListBox)->SetString(3, TtaConvMessageToWX( "" ));
  XRCCTRL(*this, "wxID_PROPOSALS_LIST", wxListBox)->SetString(4, TtaConvMessageToWX( "" ));
  XRCCTRL(*this, "wxID_PROPOSALS_LIST", wxListBox)->SetSelection(-1);
  XRCCTRL(*this, "wxID_FIRST_PROPOSAL", wxTextCtrl)->SetValue(TtaConvMessageToWX( "" ) );
  m_max_proposals = 5;

  // search area radio box
  XRCCTRL(*this, "wxID_CHECKING_AREA", wxRadioBox)->SetLabel(TtaConvMessageToWX( TtaGetMessage (LIB, TMSG_What) ));
  XRCCTRL(*this, "wxID_CHECKING_AREA", wxRadioBox)->SetString(0, TtaConvMessageToWX( TtaGetMessage (LIB, TMSG_BEFORE_SEL) ));
  XRCCTRL(*this, "wxID_CHECKING_AREA", wxRadioBox)->SetString(1, TtaConvMessageToWX( TtaGetMessage (LIB, TMSG_WITHIN_SEL) ));
  XRCCTRL(*this, "wxID_CHECKING_AREA", wxRadioBox)->SetString(2, TtaConvMessageToWX( TtaGetMessage (LIB, TMSG_AFTER_SEL) ));
  XRCCTRL(*this, "wxID_CHECKING_AREA", wxRadioBox)->SetString(3, TtaConvMessageToWX( TtaGetMessage (LIB, TMSG_IN_WHOLE_DOC) ));
  XRCCTRL(*this, "wxID_CHECKING_AREA", wxRadioBox)->SetSelection(checkingArea);

  // 'ignore' check list and text ctrl
  XRCCTRL(*this, "wxID_IGNORE_TXT",  wxStaticText)->SetLabel(TtaConvMessageToWX( TtaGetMessage(LIB, TMSG_Ignore) ));
  XRCCTRL(*this, "wxID_IGNORE1_CHK", wxCheckBox)->SetLabel(TtaConvMessageToWX( TtaGetMessage(LIB, TMSG_Capitals) ));
  XRCCTRL(*this, "wxID_IGNORE2_CHK", wxCheckBox)->SetLabel(TtaConvMessageToWX( TtaGetMessage(LIB, TMSG_Arabics) ));
  XRCCTRL(*this, "wxID_IGNORE3_CHK", wxCheckBox)->SetLabel(TtaConvMessageToWX( TtaGetMessage(LIB, TMSG_Romans) ));
  XRCCTRL(*this, "wxID_IGNORE4_CHK", wxCheckBox)->SetLabel(TtaConvMessageToWX( TtaGetMessage(LIB, TMSG_Specials) ));
  XRCCTRL(*this, "wxID_IGNORE1_CHK", wxCheckBox)->SetValue(TRUE);
  XRCCTRL(*this, "wxID_IGNORE2_CHK", wxCheckBox)->SetValue(TRUE);
  XRCCTRL(*this, "wxID_IGNORE3_CHK", wxCheckBox)->SetValue(TRUE);
  XRCCTRL(*this, "wxID_IGNORE4_CHK", wxCheckBox)->SetValue(TRUE);
  m_special_char = TtaConvMessageToWX( "@#$&+~" );
  XRCCTRL(*this, "wxID_IGNORE_CHAR", wxTextCtrl)->SetValue( m_special_char );
  XRCCTRL(*this, "wxID_SPELL_LANGUAGE_TXT", wxStaticText)->SetLabel(TtaConvMessageToWX( "" ));
  XRCCTRL(*this, "wxID_SPELL_FINISHED", wxStaticText)->SetLabel(TtaConvMessageToWX( "" ));

  // buttons
  XRCCTRL(*this, "wxID_CANCEL", wxButton)->SetLabel(TtaConvMessageToWX( TtaGetMessage(LIB, TMSG_CANCEL) ));
  XRCCTRL(*this, "wxID_SEARCH_SKIP_BUTTON", wxButton)->SetLabel(TtaConvMessageToWX( TtaGetMessage(LIB, TMSG_Pass_Without) ));
  XRCCTRL(*this, "wxID_SKIP_DIC_BUTTON", wxButton)->SetLabel(TtaConvMessageToWX( TtaGetMessage(LIB, TMSG_Pass_With) ));
  XRCCTRL(*this, "wxID_REPLACE_NEXT_BUTTON", wxButton)->SetLabel(TtaConvMessageToWX( TtaGetMessage(LIB, TMSG_Replace_Without) ));

  // Set focus - The winner is ...
  XRCCTRL(*this, "wxID_SEARCH_SKIP_BUTTON", wxButton)->SetFocus();

  Layout();
  
  SetAutoLayout( TRUE );
}

/*----------------------------------------------------------------------
  Destructor. (Empty, as I don't need anything special done when destructing).
  ----------------------------------------------------------------------*/
SpellCheckDlgWX::~SpellCheckDlgWX()
{
  /* when the dialog is destroyed, It important to cleanup context */
  ThotCallback (m_ref, INTEGER_DATA, (char*) 0);
  TtaRedirectFocus();
}

/*----------------------------------------------------------------------
  OnCancelButton called when clicking on cancel button
  ----------------------------------------------------------------------*/
void SpellCheckDlgWX::OnCancelButton( wxCommandEvent& event )
{
  ThotCallback (m_ref, INTEGER_DATA, (char*) 0);
  TtaRedirectFocus();
}

/*----------------------------------------------------------------------
  OnSkipWithoutButton called when clicking on skip button
  ----------------------------------------------------------------------*/
void SpellCheckDlgWX::OnSkipWithoutButton( wxCommandEvent& event )
{
  ThotCallback (m_ref, INTEGER_DATA, (char*) 1);
  SpellCheckDlgWX::Set_Proposals ();
}

/*----------------------------------------------------------------------
  OnSkipWithButton called when clicking on skip+dic button
  ----------------------------------------------------------------------*/
void SpellCheckDlgWX::OnSkipWithButton( wxCommandEvent& event )
{
  ThotCallback (m_ref, INTEGER_DATA, (char*) 2);
  SpellCheckDlgWX::Set_Proposals ();
}

/*----------------------------------------------------------------------
  OnReplaceWithoutButton called when clicking on replace/next  button
  ----------------------------------------------------------------------*/
void SpellCheckDlgWX::OnReplaceWithoutButton( wxCommandEvent& event )
{
  char buffer[100];
  wxString selected_item;

  selected_item = XRCCTRL(*this, "wxID_FIRST_PROPOSAL", wxTextCtrl)->GetValue();
  //selected_item = XRCCTRL(*this, "wxID_PROPOSALS_LIST", wxListBox)->GetStringSelection();
  if ( !selected_item.IsEmpty() )
    {  
      // allocate a temporary buffer
      wxASSERT( selected_item.Len() < 100 );
      strcpy( buffer, (const char*)selected_item.mb_str(wxConvUTF8) );
      ThotCallback (m_base + ChkrSelectProp, STRING_DATA, buffer);
    }
  ThotCallback (m_ref, INTEGER_DATA, (char*) 3);
  SpellCheckDlgWX::Set_Proposals ();
}

/*---------------------------------------------------------------------------
  OnChangeProposals
  ----------------------------------------------------------------------------*/
void SpellCheckDlgWX::OnChangeProposals ( wxCommandEvent& event )
{
  wxString selected_item = XRCCTRL(*this, "wxID_PROPOSALS_LIST", wxListBox)->GetStringSelection();
  if ( !selected_item.IsEmpty() )
    {  
      // allocate a temporary buffer
      char buffer[100];
      wxASSERT( selected_item.Len() < 100 );
      strcpy( buffer, (const char*)selected_item.mb_str(wxConvUTF8) );
      ThotCallback (m_base + ChkrSelectProp, STRING_DATA, buffer);
      // update first proposal
      XRCCTRL(*this, "wxID_FIRST_PROPOSAL", wxTextCtrl)->SetValue(selected_item);
    }
}

/*---------------------------------------------------------------
  OnIgnoreCapitalsChkBox
  ---------------------------------------------------------------*/
void SpellCheckDlgWX::OnIgnoreCapitalsChkBox ( wxCommandEvent& event )
{
  ThotCallback (m_base + ChkrMenuIgnore, INTEGER_DATA, (char*) 0);
}

/*---------------------------------------------------------------
  OnIgnoreArabicsChkBox
  ---------------------------------------------------------------*/
void SpellCheckDlgWX::OnIgnoreArabicsChkBox ( wxCommandEvent& event )
{
  ThotCallback (m_base + ChkrMenuIgnore, INTEGER_DATA, (char*) 1);
}

/*---------------------------------------------------------------
  OnIgnoreRomansChkBox
  ---------------------------------------------------------------*/
void SpellCheckDlgWX::OnIgnoreRomansChkBox ( wxCommandEvent& event )
{
  ThotCallback (m_base + ChkrMenuIgnore, INTEGER_DATA, (char*) 2);
}

/*---------------------------------------------------------------
  OnIgnoreSpecialsChkBox
  ---------------------------------------------------------------*/
void SpellCheckDlgWX::OnIgnoreSpecialsChkBox ( wxCommandEvent& event )
{
  ThotCallback (m_base + ChkrMenuIgnore, INTEGER_DATA, (char*) 3);
  if (XRCCTRL(*this, "wxID_IGNORE4_CHK", wxCheckBox)->GetValue() == TRUE)
    XRCCTRL(*this, "wxID_IGNORE_CHAR", wxTextCtrl)->SetEditable( TRUE );
  else
    XRCCTRL(*this, "wxID_IGNORE_CHAR", wxTextCtrl)->SetEditable( FALSE );
}

/*---------------------------------------------------------------------------
  OnAreaRadioBox
  ----------------------------------------------------------------------------*/
void SpellCheckDlgWX::OnAreaRadioBox ( wxCommandEvent& event )
{
  ThotCallback (m_base + ChkrMenuOR, INTEGER_DATA,
		(char*)(XRCCTRL(*this, "wxID_CHECKING_AREA", wxRadioBox)->GetSelection( )) );
}

/*---------------------------------------------------------------
  OnSpecialChar Set the special characters to ignore
  ---------------------------------------------------------------*/
void SpellCheckDlgWX::OnSpecialChar ( wxCommandEvent& event )
{
  wxString special_char = XRCCTRL(*this, "wxID_IGNORE_CHAR", wxTextCtrl)->GetValue( );

  // allocate a temporary buffer to convert wxString to (char *) UTF-8 buffer
  char buffer[512];
  wxASSERT( special_char.Len() < 512 );
  strcpy( buffer, (const char*)special_char.mb_str(wxConvUTF8) );  
  ThotCallback (m_base + ChkrSpecial,  STRING_DATA, (char *)buffer );
}

#endif /* _WX */
