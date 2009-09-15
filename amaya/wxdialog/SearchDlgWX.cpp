/*
 *
 *  (c) COPYRIGHT INRIA and W3C, 1996-2009
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

#ifdef _WX
#include "wx/wx.h"
#include "wx/xrc/xmlres.h"              // XRC XML resouces

#include "AmayaApp.h"
#include "SearchDlgWX.h"

#define THOT_EXPORT extern
#include "amaya.h"
#include "appdialogue_wx.h"
#include "message_wx.h"
#include "dialog.h"

//-----------------------------------------------------------------------------
// Event table: connect the events to the handler functions to process them
//-----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(SearchDlgWX, AmayaDialog)
  EVT_BUTTON( XRCID("wxID_OK"),     SearchDlgWX::OnConfirmButton )
  EVT_BUTTON( XRCID("wxID_CANCEL"),      SearchDlgWX::OnCancelButton )
  EVT_BUTTON( XRCID("wxID_NOREPLACEBUTTON"),   SearchDlgWX::OnNoReplaceButton )
  EVT_RADIOBOX( XRCID("wxID_REPLACE_BOX"),     SearchDlgWX::OnReplaceBox )
  EVT_RADIOBOX( XRCID("wxID_SEARCH_AREA_BOX"), SearchDlgWX::OnSearchAreaBox )
  EVT_CHECKBOX( XRCID("wxID_CHECK_CASE"),      SearchDlgWX::OnCheckCaseBox )
  EVT_TEXT( XRCID("wxID_SEARCH_FOR_TXT"),      SearchDlgWX::OnTextChanged )
  EVT_TEXT( XRCID("wxID_REPLACE_BY_TXT"),      SearchDlgWX::OnReplaceChanged )
  EVT_TEXT_ENTER( XRCID("wxID_SEARCH_FOR_TXT"),SearchDlgWX::OnConfirmButton )
END_EVENT_TABLE()

/*----------------------------------------------------------------------
  SearchDlgWX create the Search dialog 
  params:
    + parent: parent window
    + caption: dialog caption (including the document name)
    + searched: the initial searched string
    + replace: the initial replacing string
    + do_replace: true if the replace should be done
    + anycase: true if not case sensitive
    + searchAfter: true is the search goes forward
  ----------------------------------------------------------------------*/
  SearchDlgWX::SearchDlgWX( int ref, wxWindow* parent, const wxString & caption,
                            const wxString & searched, const wxString & replace,
                            bool do_replace, bool anycase, bool searchAfter) : 
    AmayaDialog( parent, ref )
{
  wxXmlResource::Get()->LoadDialog(this, parent, wxT("SearchDlgWX"));
  SetTitle( caption );

  // update dialog labels
  XRCCTRL(*this, "wxID_SEARCH_FOR", wxStaticText)->SetLabel(TtaConvMessageToWX( TtaGetMessage(LIB, TMSG_SEARCH_FOR) ));

  XRCCTRL(*this, "wxID_CHECK_CASE", wxCheckBox)->SetLabel(TtaConvMessageToWX( TtaGetMessage(LIB, TMSG_UPPERCASE_EQ_LOWERCASE) ));

  XRCCTRL(*this, "wxID_REPLACE_BY", wxStaticText)->SetLabel(TtaConvMessageToWX( TtaGetMessage(LIB, TMSG_REPLACE_BY) ));

  XRCCTRL(*this, "wxID_REPLACE_BOX", wxRadioBox)->SetLabel(TtaConvMessageToWX( TtaGetMessage (LIB, TMSG_REPLACE) ));
  XRCCTRL(*this, "wxID_REPLACE_BOX", wxRadioBox)->SetString(0, TtaConvMessageToWX( TtaGetMessage (LIB, TMSG_NO_REPLACE) ));
  XRCCTRL(*this, "wxID_REPLACE_BOX", wxRadioBox)->SetString(1, TtaConvMessageToWX( TtaGetMessage (LIB, TMSG_REPLACE_ON_REQU) ));
  XRCCTRL(*this, "wxID_REPLACE_BOX", wxRadioBox)->SetString(2, TtaConvMessageToWX( TtaGetMessage (LIB, TMSG_AUTO_REPLACE) ));

  XRCCTRL(*this, "wxID_SEARCH_AREA_BOX", wxRadioBox)->SetLabel(TtaConvMessageToWX( TtaGetMessage (LIB, TMSG_SEARCH_WHERE) ));
  XRCCTRL(*this, "wxID_SEARCH_AREA_BOX", wxRadioBox)->SetString(0, TtaConvMessageToWX( TtaGetMessage(LIB, TMSG_BEFORE_SEL) ));
  XRCCTRL(*this, "wxID_SEARCH_AREA_BOX", wxRadioBox)->SetString(1, TtaConvMessageToWX( TtaGetMessage(LIB, TMSG_WITHIN_SEL) ));
  XRCCTRL(*this, "wxID_SEARCH_AREA_BOX", wxRadioBox)->SetString(2, TtaConvMessageToWX( TtaGetMessage(LIB, TMSG_AFTER_SEL) ));
  XRCCTRL(*this, "wxID_SEARCH_AREA_BOX", wxRadioBox)->SetString(3, TtaConvMessageToWX( TtaGetMessage(LIB, TMSG_IN_WHOLE_DOC) ));
  XRCCTRL(*this, "wxID_SEARCH_COMPLETE", wxStaticText)->SetLabel(TtaConvMessageToWX( "" ));

  // update button labels
  XRCCTRL(*this, "wxID_OK", wxButton)->SetLabel(TtaConvMessageToWX( TtaGetMessage(LIB, TMSG_LIB_CONFIRM) ));
  XRCCTRL(*this, "wxID_CANCEL", wxButton)->SetLabel(TtaConvMessageToWX( TtaGetMessage(LIB, TMSG_CANCEL) ));
  XRCCTRL(*this, "wxID_NOREPLACEBUTTON", wxButton)->SetLabel(TtaConvMessageToWX( TtaGetMessage(LIB, TMSG_DO_NOT_REPLACE) ));

  // Default values
  XRCCTRL(*this, "wxID_SEARCH_FOR_TXT", wxTextCtrl)->SetValue(searched);
  XRCCTRL(*this, "wxID_REPLACE_BY_TXT", wxTextCtrl)->SetValue(replace);
  XRCCTRL(*this, "wxID_CHECK_CASE", wxCheckBox)->SetValue(anycase);
  m_case = XRCCTRL(*this, "wxID_CHECK_CASE", wxCheckBox)->GetValue( );
  if (do_replace)
    XRCCTRL(*this, "wxID_REPLACE_BOX", wxRadioBox)->SetSelection(1);
  else
    XRCCTRL(*this, "wxID_REPLACE_BOX", wxRadioBox)->SetSelection(0);
  m_ireplace = XRCCTRL(*this, "wxID_REPLACE_BOX", wxRadioBox)->GetSelection( );
  if (searchAfter)
    XRCCTRL(*this, "wxID_SEARCH_AREA_BOX", wxRadioBox)->SetSelection(2);
  else
    XRCCTRL(*this, "wxID_SEARCH_AREA_BOX", wxRadioBox)->SetSelection(3);
  m_iarea = XRCCTRL(*this, "wxID_SEARCH_AREA_BOX", wxRadioBox)->GetSelection( );

  // Give focus to first text control
  //XRCCTRL(*this, "wxID_SEARCH_FOR_TXT", wxTextCtrl)->SetFocus();
  Layout();
  
  SetAutoLayout( TRUE );
}


/*----------------------------------------------------------------------
  Destructor. (Empty, as I don't need anything special done when destructing).
  ----------------------------------------------------------------------*/
SearchDlgWX::~SearchDlgWX()
{
  /* when the dialog is destroyed, It important to cleanup context */
  ThotCallback (NumFormSearchText, INTEGER_DATA, (char*) 0); 
  TtaRedirectFocus();
}


/*----------------------------------------------------------------------
  OnConfirmButton called when clicking on confirmbutton
  ----------------------------------------------------------------------*/
void SearchDlgWX::OnConfirmButton( wxCommandEvent& event )
{
  WX_SearchResult = 0; /* By default the search is OK */
  ThotCallback (NumFormSearchText, INTEGER_DATA, (char *) 1);
  if (m_iarea == 3)
    {
      XRCCTRL(*this, "wxID_SEARCH_AREA_BOX", wxRadioBox)->SetSelection(2);
      m_iarea = 2;
    }
  if (WX_SearchResult == 1)
    XRCCTRL(*this, "wxID_SEARCH_COMPLETE", wxStaticText)->SetLabel(TtaConvMessageToWX( TtaGetMessage (LIB, TMSG_NOTHING_TO_REPLACE) ));
  else if (WX_SearchResult == 2)
    XRCCTRL(*this, "wxID_SEARCH_COMPLETE", wxStaticText)->SetLabel(TtaConvMessageToWX( TtaGetMessage (LIB, TMSG_NOT_FOUND) ));
  else
    XRCCTRL(*this, "wxID_SEARCH_COMPLETE", wxStaticText)->SetLabel(TtaConvMessageToWX( "" ));
  // Give focus to first text control
  XRCCTRL(*this, "wxID_SEARCH_FOR_TXT", wxTextCtrl)->SetFocus();
}

/*----------------------------------------------------------------------
  OnNoReplaceButton called when clicking on cancelbutton
  ----------------------------------------------------------------------*/
void SearchDlgWX::OnNoReplaceButton( wxCommandEvent& event )
{
  ThotCallback (NumMenuReplaceMode, INTEGER_DATA, (char *) 0);
  ThotCallback (NumMenuOrSearchText, INTEGER_DATA, (char *) m_iarea);
  ThotCallback (NumFormSearchText, INTEGER_DATA, (char *) 1);
  if (m_iarea == 3)
    {
      XRCCTRL(*this, "wxID_SEARCH_AREA_BOX", wxRadioBox)->SetSelection(2);
      m_iarea = 2;
    }
  if (m_ireplace != 0)
    // retore the value
    ThotCallback (NumMenuReplaceMode, INTEGER_DATA, (char *) m_ireplace);
  // Give focus to first text control
  XRCCTRL(*this, "wxID_SEARCH_FOR_TXT", wxTextCtrl)->SetFocus();
}

/*----------------------------------------------------------------------
  OnCancelButton called when clicking on cancelbutton
  ----------------------------------------------------------------------*/
void SearchDlgWX::OnCancelButton( wxCommandEvent& event )
{
  ThotCallback (NumFormSearchText, INTEGER_DATA, (char*) 0); 
  TtaRedirectFocus();
}

/*----------------------------------------------------------------------
  OnReplaceBox called when clicking on replacebox
  ----------------------------------------------------------------------*/
void SearchDlgWX::OnReplaceBox ( wxCommandEvent& event )
{
  m_ireplace = XRCCTRL(*this, "wxID_REPLACE_BOX", wxRadioBox)->GetSelection( );
  ThotCallback (NumMenuReplaceMode, INTEGER_DATA, (char *) m_ireplace);
}

/*----------------------------------------------------------------------
  OnTextChanged called when changing the replace text
  ----------------------------------------------------------------------*/
void SearchDlgWX::OnTextChanged ( wxCommandEvent& event )
{
  char buf[MAX_LENGTH];

  buf[0] = EOS;
  wxString textToSearch = XRCCTRL(*this, "wxID_SEARCH_FOR_TXT",
                                  wxTextCtrl)->GetValue( );
  if (textToSearch.Len() > 0)
    {
      strncpy (buf, (const char*)textToSearch.mb_str(wxConvUTF8), MAX_LENGTH);
      buf[MAX_LENGTH-1] = EOS;
    }
  ThotCallback (NumZoneTextSearch, STRING_DATA, buf);
}

/*----------------------------------------------------------------------
  OnTextChanged called when changing the replace text
  ----------------------------------------------------------------------*/
void SearchDlgWX::OnReplaceChanged ( wxCommandEvent& event )
{
  char buf[MAX_LENGTH];

  buf[0] = EOS;
  wxString newText = XRCCTRL(*this, "wxID_REPLACE_BY_TXT",
                             wxTextCtrl)->GetValue( );
  
  if (newText.Len() > 0)
    {
      strncpy (buf, (const char*)newText.mb_str(wxConvUTF8), MAX_LENGTH);
      buf[MAX_LENGTH-1] = EOS;
      if (m_ireplace != 2)
        {
          m_ireplace = 1;
          XRCCTRL(*this, "wxID_REPLACE_BOX", wxRadioBox)->SetSelection(1);
          ThotCallback (NumMenuReplaceMode, INTEGER_DATA, (char *) m_ireplace);
        }
    }
  else
    {
      m_ireplace = 0;
      XRCCTRL(*this, "wxID_REPLACE_BOX", wxRadioBox)->SetSelection(0);
      ThotCallback (NumMenuReplaceMode, INTEGER_DATA, (char *) m_ireplace);
    }
  ThotCallback (NumZoneTextReplace, STRING_DATA, buf);
}

/*----------------------------------------------------------------------
  OnSearchAreaBox called when clicking on searchareabox
  ----------------------------------------------------------------------*/
void SearchDlgWX::OnSearchAreaBox ( wxCommandEvent& event )
{
  m_iarea = XRCCTRL(*this, "wxID_SEARCH_AREA_BOX",
                    wxRadioBox)->GetSelection( );
  ThotCallback (NumMenuOrSearchText, INTEGER_DATA, (char *) m_iarea);
}

/*----------------------------------------------------------------------
  OnCheckCaseBox called when clicking on checkcasebutton
  ----------------------------------------------------------------------*/
void SearchDlgWX::OnCheckCaseBox ( wxCommandEvent& event )
{
  if (m_case)
    m_case = FALSE;
  else
    m_case = TRUE;
  ThotCallback (NumToggleUpperEqualLower, INTEGER_DATA, (char *) 0);
}

#endif /* _WX */
