#ifdef _WX

#include "wx/wx.h"
#include "wx/string.h"
#include "wx/xrc/xmlres.h"

// Thotlib includes
#include "thot_gui.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "appdialogue.h"
#include "dialog.h"
#include "selection.h"
#include "application.h"
#include "dialog.h"
#include "document.h"
#include "message.h"
#include "libmsg.h"
#include "logdebug.h"

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "edit_tv.h"
#include "displayview_f.h"
#include "editcommands_f.h"
#include "message_wx.h"

#include "AmayaParams.h"
#include "appdialogue_wx_f.h"

#include "AmayaToolBar.h"
#include "AmayaWindow.h"
#include "AmayaFrame.h"

#ifdef _MACOS
/* Wrap-up to prevent an event when the tabs are switched on Mac */
static ThotBool  UpdateFrameUrl = TRUE;
#endif /* _MACOS */

#ifdef _WINDOWS
static  char      BufUrl[512];
static  ThotBool  isBufUrl = 0;
#endif /* _WINDOWS */

IMPLEMENT_DYNAMIC_CLASS(AmayaToolBar, wxPanel)

/*----------------------------------------------------------------------
 *       Class:  AmayaToolBar
 *      Method:  AmayaToolBar
 * Description:  create a new toolbar
  -----------------------------------------------------------------------*/
AmayaToolBar::AmayaToolBar( wxWindow * p_parent, AmayaWindow * p_amaya_window_parent ) : 
  wxPanel()
{
  wxXmlResource::Get()->LoadPanel(this, p_parent, wxT("AmayaToolbar"));

  m_pAmayaWindowParent = p_amaya_window_parent;
  m_pComboBox          = XRCCTRL(*this, "wxID_TOOL_URL", wxComboBox);


  /* set tooltips */
  XRCCTRL(*this, "wxID_TOOL_BACK", wxBitmapButton)->SetToolTip(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_BUTTON_PREVIOUS)));
  XRCCTRL(*this, "wxID_TOOL_FORWARD", wxBitmapButton)->SetToolTip(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_BUTTON_NEXT)));
  XRCCTRL(*this, "wxID_TOOL_RELOAD", wxBitmapButton)->SetToolTip(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_BUTTON_RELOAD)));
  XRCCTRL(*this, "wxID_TOOL_STOP", wxBitmapButton)->SetToolTip(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_BUTTON_INTERRUPT)));
  XRCCTRL(*this, "wxID_TOOL_HOME", wxBitmapButton)->SetToolTip(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_BUTTON_HOME)));
  XRCCTRL(*this, "wxID_TOOL_SAVE", wxBitmapButton)->SetToolTip(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_BUTTON_SAVE)));
  XRCCTRL(*this, "wxID_TOOL_PRINT", wxBitmapButton)->SetToolTip(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_BUTTON_PRINT)));
  XRCCTRL(*this, "wxID_TOOL_FIND", wxBitmapButton)->SetToolTip(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_BUTTON_SEARCH)));
  XRCCTRL(*this, "wxID_TOOL_LOGO", wxBitmapButton)->SetToolTip(TtaConvMessageToWX(TtaGetAppName())+_T(" ")+TtaConvMessageToWX(TtaGetAppVersion()));

#ifdef _WINDOWS
  XRCCTRL(*this, "wxID_TOOL_BACK", wxBitmapButton)->SetWindowStyle( wxNO_BORDER );
  XRCCTRL(*this, "wxID_TOOL_FORWARD", wxBitmapButton)->SetWindowStyle( wxNO_BORDER );
  XRCCTRL(*this, "wxID_TOOL_RELOAD", wxBitmapButton)->SetWindowStyle( wxNO_BORDER );
  XRCCTRL(*this, "wxID_TOOL_STOP", wxBitmapButton)->SetWindowStyle( wxNO_BORDER );
  XRCCTRL(*this, "wxID_TOOL_HOME", wxBitmapButton)->SetWindowStyle( wxNO_BORDER );
  XRCCTRL(*this, "wxID_TOOL_SAVE", wxBitmapButton)->SetWindowStyle( wxNO_BORDER );
  XRCCTRL(*this, "wxID_TOOL_PRINT", wxBitmapButton)->SetWindowStyle( wxNO_BORDER );
  XRCCTRL(*this, "wxID_TOOL_FIND", wxBitmapButton)->SetWindowStyle( wxNO_BORDER );
  XRCCTRL(*this, "wxID_TOOL_LOGO", wxBitmapButton)->SetWindowStyle( wxNO_BORDER );
#endif /* _WINDOWS */
  
  SetAutoLayout(TRUE);
}

/*----------------------------------------------------------------------
 *       Class:  AmayaToolBar
 *      Method:  ~AmayaToolBar
 * Description:  destructor
  -----------------------------------------------------------------------*/
AmayaToolBar::~AmayaToolBar()
{
}

/*----------------------------------------------------------------------
 *       Class:  AmayaToolBar
 *      Method:  OnURLText
 * Description:  the user has typed ENTER with his keyboard or clicked on validate button =>
 *               simply activate the callback
  -----------------------------------------------------------------------*/
void AmayaToolBar::OnURLTextEnter( wxCommandEvent& event )
{

   // TtaDisplayMessage (INFO, buffer);
	GotoSelectedURL();
  // do not skip this event because we don't want to propagate this event
  // event.Skip();
}

/*----------------------------------------------------------------------
 *       Class:  AmayaToolBar
 *      Method:  GotoSelectedURL
 * Description:  validate the selection
  -----------------------------------------------------------------------*/
void AmayaToolBar::GotoSelectedURL()
{
  Document doc;
  View     view;

  FrameToView (TtaGiveActiveFrame(), &doc, &view);

  CloseTextInsertion ();

     //TtaDisplayMessage (CONFIRM, "GotoSelectedURL");
	 /* call the callback  with the url selected text */
  PtrDocument pDoc = LoadedDocument[doc-1];
  wxASSERT(pDoc);
  if (pDoc && pDoc->Call_Text)
    {
      char buffer[512];
      strcpy(buffer, (m_pComboBox->GetValue()).mb_str(wxConvUTF8));
// patch to go-round a bug on Windows (TEXT_ENTER event called twice)
#ifdef _WINDOWS 
	  if (isBufUrl == FALSE)
	  {
		  isBufUrl = TRUE;
        (*(Proc3)pDoc->Call_Text) ((void *)doc, (void *)view, (void *)buffer);
	     strcpy (BufUrl, buffer);
	  }
	  else if (strcmp (buffer, BufUrl) != 0)
	  {
        (*(Proc3)pDoc->Call_Text) ((void *)doc, (void *)view, (void *)buffer);
	     strcpy (BufUrl, buffer);
	  }
#else /* _WINDOWS */
        (*(Proc3)pDoc->Call_Text) ((void *)doc, (void *)view, (void *)buffer);
#endif /* _WINDOWS */
    }
}

/*----------------------------------------------------------------------
 *       Class:  AmayaToolBar
 *      Method:  Clear
 * Description:  Removes all items from the control.
  -----------------------------------------------------------------------*/
void AmayaToolBar::ClearURL()
{
  m_pComboBox->Clear();
}

/*----------------------------------------------------------------------
 *       Class:  AmayaToolBar
 *      Method:  Append
 * Description:  Adds the item to the end of the combobox.
  -----------------------------------------------------------------------*/
void AmayaToolBar::AppendURL( const wxString & newurl )
{
  m_pComboBox->Append( newurl );
}

/*----------------------------------------------------------------------
 *       Class:  AmayaToolBar
 *      Method:  SetValue
 * Description:  Sets the text for the combobox text field.
  -----------------------------------------------------------------------*/
void AmayaToolBar::SetURLValue( const wxString & newurl )
{
  TTALOGDEBUG_0( TTA_LOG_DIALOG, _T("AmayaToolBar::SetURLValue - ")+newurl);
#ifdef _MACOS
  UpdateFrameUrl = FALSE;
#endif /* _MACOS */
  if (m_pComboBox->FindString(newurl) == wxNOT_FOUND)
    m_pComboBox->Append(newurl);
  // new url should exists into combobox items so just select it.
  m_pComboBox->SetStringSelection( newurl );
  //m_pComboBox->SetSelection(0,-1);
#ifdef _MACOS
  UpdateFrameUrl = TRUE;
#endif /* _MACOS */
}

/*----------------------------------------------------------------------
 *       Class:  AmayaToolBar
 *      Method:  GetValue
 * Description:  Returns the current value in the combobox text field.
  -----------------------------------------------------------------------*/
wxString AmayaToolBar::GetURLValue()
{
  return m_pComboBox->GetValue( );
}


/*----------------------------------------------------------------------
 *       Class:  AmayaToolBar
 *      Method:  OnURLSelected
 * Description:  Called when the user select a new url
 *               there is a bug in wxWidgets on GTK version, this event is 
 *               called to often : each times user move the mouse with button pressed.
  -----------------------------------------------------------------------*/
void AmayaToolBar::OnURLSelected( wxCommandEvent& event )
{
  GotoSelectedURL();
}

/*----------------------------------------------------------------------
 *       Class:  AmayaToolBar
 *      Method:  OnURLText
 * Description:  Called when the url text is changed
 *               Just update the current frame internal url variable
  -----------------------------------------------------------------------*/
void AmayaToolBar::OnURLText( wxCommandEvent& event )
{
#ifdef _MACOS
  if (UpdateFrameUrl)
  {
    AmayaFrame * p_frame = m_pAmayaWindowParent->GetActiveFrame();
    if (p_frame)
      p_frame->UpdateFrameURL(m_pComboBox->GetValue());
    event.Skip();
  }
#else /* _MACOS */
    AmayaFrame * p_frame = m_pAmayaWindowParent->GetActiveFrame();
    if (p_frame)
      p_frame->UpdateFrameURL(m_pComboBox->GetValue());
    event.Skip();
#endif /* _MACOS */
}

/*----------------------------------------------------------------------
 *       Class:  AmayaToolBar
 *      Method:  OnButton_Back
 * Description:  
  -----------------------------------------------------------------------*/
void AmayaToolBar::OnButton_Back( wxCommandEvent& event )
{
  Document doc;
  View view;
  FrameToView (TtaGiveActiveFrame(), &doc, &view);
  TtaExecuteMenuAction ("GotoPreviousHTML", doc, view, FALSE);
}

/*----------------------------------------------------------------------
 *       Class:  AmayaToolBar
 *      Method:  OnButton_Forward
 * Description:  
  -----------------------------------------------------------------------*/
void AmayaToolBar::OnButton_Forward( wxCommandEvent& event )
{
  Document doc;
  View view;
  FrameToView (TtaGiveActiveFrame(), &doc, &view);
  TtaExecuteMenuAction ("GotoNextHTML", doc, view, FALSE);
}

/*----------------------------------------------------------------------
 *       Class:  AmayaToolBar
 *      Method:  OnButton_Reload
 * Description:  
  -----------------------------------------------------------------------*/
void AmayaToolBar::OnButton_Reload( wxCommandEvent& event )
{
  Document doc;
  View view;
  FrameToView (TtaGiveActiveFrame(), &doc, &view);
  TtaExecuteMenuAction ("Reload", doc, view, FALSE);
}

/*----------------------------------------------------------------------
 *       Class:  AmayaToolBar
 *      Method:  OnButton_Stop
 * Description:  
  -----------------------------------------------------------------------*/
void AmayaToolBar::OnButton_Stop( wxCommandEvent& event )
{
  Document doc;
  View view;
  FrameToView (TtaGiveActiveFrame(), &doc, &view);
  TtaExecuteMenuAction ("StopTransfer", doc, view, FALSE);
}

/*----------------------------------------------------------------------
 *       Class:  AmayaToolBar
 *      Method:  OnButton_Home
 * Description:  
  -----------------------------------------------------------------------*/
void AmayaToolBar::OnButton_Home( wxCommandEvent& event )
{
  Document doc;
  View view;
  FrameToView (TtaGiveActiveFrame(), &doc, &view);
  TtaExecuteMenuAction ("GoToHome", doc, view, FALSE);
}

/*----------------------------------------------------------------------
 *       Class:  AmayaToolBar
 *      Method:  OnButton_Save
 * Description:  
  -----------------------------------------------------------------------*/
void AmayaToolBar::OnButton_Save( wxCommandEvent& event )
{
  Document doc;
  View view;
  FrameToView (TtaGiveActiveFrame(), &doc, &view);
  TtaExecuteMenuAction ("SaveDocument", doc, view, FALSE);
}

/*----------------------------------------------------------------------
 *       Class:  AmayaToolBar
 *      Method:  OnButton_Print
 * Description:  
  -----------------------------------------------------------------------*/
void AmayaToolBar::OnButton_Print( wxCommandEvent& event )
{
  Document doc;
  View view;
  FrameToView (TtaGiveActiveFrame(), &doc, &view);
  TtaExecuteMenuAction ("SetupAndPrint", doc, view, FALSE);
}

/*----------------------------------------------------------------------
 *       Class:  AmayaToolBar
 *      Method:  OnButton_Find
 * Description:  
  -----------------------------------------------------------------------*/
void AmayaToolBar::OnButton_Find( wxCommandEvent& event )
{
  Document doc;
  View view;
  FrameToView (TtaGiveActiveFrame(), &doc, &view);
  TtaExecuteMenuAction ("TtcSearchText", doc, view, FALSE);
}

/*----------------------------------------------------------------------
 *       Class:  AmayaToolBar
 *      Method:  OnButton_Logo
 * Description:  
  -----------------------------------------------------------------------*/
void AmayaToolBar::OnButton_Logo( wxCommandEvent& event )
{
  Document doc;
  View view;
  FrameToView (TtaGiveActiveFrame(), &doc, &view);
  TtaExecuteMenuAction ("HelpLocal", doc, view, FALSE);
}

/*----------------------------------------------------------------------
 *       Class:  AmayaToolBar
 *      Method:  EnableTool
 * Description:  enable/disable toolbar tools
  -----------------------------------------------------------------------*/
void AmayaToolBar::EnableTool( const wxString & xrc_id, bool enable )
{
  wxWindow * p_window = FindWindow(wxXmlResource::GetXRCID(xrc_id));
  wxASSERT(p_window);

  if (p_window)
    p_window->Enable(enable);
}

/*----------------------------------------------------------------------
 *  this is where the event table is declared
 *  the callbacks are assigned to an event type
 *----------------------------------------------------------------------*/
BEGIN_EVENT_TABLE(AmayaToolBar, wxPanel)
  EVT_BUTTON( XRCID("wxID_TOOL_BACK"),           AmayaToolBar::OnButton_Back )
  EVT_BUTTON( XRCID("wxID_TOOL_FORWARD"),        AmayaToolBar::OnButton_Forward )
  EVT_BUTTON( XRCID("wxID_TOOL_RELOAD"),         AmayaToolBar::OnButton_Reload )
  EVT_BUTTON( XRCID("wxID_TOOL_STOP"),           AmayaToolBar::OnButton_Stop )
  EVT_BUTTON( XRCID("wxID_TOOL_HOME"),           AmayaToolBar::OnButton_Home )
  EVT_BUTTON( XRCID("wxID_TOOL_SAVE"),           AmayaToolBar::OnButton_Save )
  EVT_BUTTON( XRCID("wxID_TOOL_PRINT"),          AmayaToolBar::OnButton_Print )
  EVT_BUTTON( XRCID("wxID_TOOL_FIND"),           AmayaToolBar::OnButton_Find )
  EVT_BUTTON( XRCID("wxID_TOOL_LOGO"),           AmayaToolBar::OnButton_Logo )
  EVT_COMBOBOX( XRCID("wxID_TOOL_URL"),          AmayaToolBar::OnURLSelected )
  EVT_TEXT_ENTER( XRCID("wxID_TOOL_URL"),        AmayaToolBar::OnURLTextEnter )
  EVT_TEXT( XRCID("wxID_TOOL_URL"),              AmayaToolBar::OnURLText )
END_EVENT_TABLE()

#endif /* #ifdef _WX */
