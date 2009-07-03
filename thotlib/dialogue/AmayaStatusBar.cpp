/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2008
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

#ifdef _WX
#include "wx/wx.h"
#include "wx/tooltip.h"
#include "wx/xrc/xmlres.h"

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
#include "frame.h"
#include "registry_wx.h"
#include "message_wx.h"
#include "logdebug.h"

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "font_tv.h"
#include "frame_tv.h"
#include "appdialogue_tv.h"

#include "font_f.h"
#include "appli_f.h"
#include "profiles_f.h"
#include "appdialogue_f.h"
#include "boxparams_f.h"
#include "dialogapi_f.h"
#include "callback_f.h"
#include "AmayaParams.h"
#include "appdialogue_wx_f.h"
#include "displayview_f.h"
#include "unstructchange_f.h"

#include "AmayaStatusBar.h"
/* the log button should be shifted on MacOS platforms */
#ifdef _MACOS
#define LOG_SHIFT  12
#else /* _MACOS */
#define LOG_SHIFT  2
#endif /* _MACOS */


class AmayaStatusText : public wxStaticText
{
  DECLARE_EVENT_TABLE();
public:
  AmayaStatusText(wxWindow* parent, wxWindowID id, const wxString& str):
    wxStaticText(parent, id, str, wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE)
    {
      SetToolTip(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_INSERT_MODE)));
    }
private:
  void OnUpdate(wxUpdateUIEvent& event)
  {
    static wxString xml = TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_INSERT_MODE_XML));
    static wxString txt = TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_INSERT_MODE_TEXT));
    
    ThotBool edit;
    TtaGetEnvBoolean ("XML_EDIT_MODE", &edit);
    
    wxString str = edit?xml:txt;
    if(GetLabel()!=str)
      SetLabel(str);
  }
  
  void OnDblClick(wxMouseEvent& event)
  {
    ThotBool edit;
    TtaGetEnvBoolean ("XML_EDIT_MODE", &edit);
    TtaSetEnvBoolean ("XML_EDIT_MODE", !edit, TRUE);
    UpdateWindowUI();
  }
};

BEGIN_EVENT_TABLE(AmayaStatusText, wxStaticText)
  EVT_UPDATE_UI(wxID_ANY, AmayaStatusText::OnUpdate)
  EVT_LEFT_UP(AmayaStatusText::OnDblClick)
END_EVENT_TABLE()


IMPLEMENT_DYNAMIC_CLASS(AmayaStatusBar, wxStatusBar)

//-----------------------------------------------------------------------------
// Event table: connect the events to the handler functions to process them
//-----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(AmayaStatusBar, wxStatusBar)
  EVT_BUTTON(XRCID("wxID_STATUS_LOG_BUTTON"), AmayaStatusBar::OnLogErrorButton )
  EVT_BUTTON(XRCID("wxID_STATUS_LOCK_BUTTON"), AmayaStatusBar::OnLockButton )
  EVT_SIZE( AmayaStatusBar::OnSize )
END_EVENT_TABLE()

static int widths[5] = {-1, -1, -1, -1, -1};

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
AmayaStatusBar::AmayaStatusBar( wxWindow * p_parent ) 
  : wxStatusBar(p_parent, wxID_ANY)
    ,m_LogErrorBmp_Green(TtaGetResourcePathWX( WX_RESOURCES_ICON_16X16, "ledgreen.png"), wxBITMAP_TYPE_PNG)
    ,m_LogErrorBmp_Red(TtaGetResourcePathWX( WX_RESOURCES_ICON_16X16, "ledred.png"), wxBITMAP_TYPE_PNG)
    ,m_Lock_Yes(TtaGetResourcePathWX( WX_RESOURCES_ICON_16X16, "no_edit.png"), wxBITMAP_TYPE_PNG)
    ,m_Lock_No(TtaGetResourcePathWX( WX_RESOURCES_ICON_16X16, "edit.png"), wxBITMAP_TYPE_PNG)
    ,m_pathCtrl(NULL)
{
  TTALOGDEBUG_0( TTA_LOG_DIALOG, _T("AmayaStatusBar::AmayaStatusBar"));

  // setup the logerror button
  m_pLogErrorButton = new wxBitmapButton( this, XRCID("wxID_STATUS_LOG_BUTTON"),
                                          m_LogErrorBmp_Green,
                                          wxDefaultPosition, wxDefaultSize,
                                          wxBU_EXACTFIT | wxNO_BORDER);
  wxASSERT(m_pLogErrorButton);
  m_pLockButton = new wxBitmapButton( this, XRCID("wxID_STATUS_LOCK_BUTTON"),
                                      m_Lock_No,
                                      wxDefaultPosition, wxDefaultSize,
                                      wxBU_EXACTFIT | wxNO_BORDER);
  m_pLockButton->SetToolTip(TtaConvMessageToWX(TtaGetMessage(LIB,NO_WEBDAV)));
  m_pathCtrl = new AmayaPathControl(this, wxID_ANY);
  m_insertMode = new AmayaStatusText(this, wxID_ANY, wxT(""));

  // setup statusbar attributes
  widths[Field_InsertMode] = 60;//m_insertMode->GetSize().GetWidth()
  widths[Field_LogError] = m_pLogErrorButton->GetSize().GetWidth() + LOG_SHIFT;
  widths[Field_Lock] = m_pLockButton->GetSize().GetWidth() + LOG_SHIFT;
  SetFieldsCount(Field_Max);
  SetStatusWidths(Field_Max, widths);
  SetMinHeight(m_pLogErrorButton->GetSize().GetHeight() + 4);
}

/*----------------------------------------------------------------------
  Destructor.
  ----------------------------------------------------------------------*/
AmayaStatusBar::~AmayaStatusBar()
{
  TTALOGDEBUG_0( TTA_LOG_DIALOG, _T("AmayaStatusBar::~AmayaStatusBar"));
}

/*----------------------------------------------------------------------
  OnLogErrorButton
  this callback show the logerror window only when there is detected errors in the document
  ----------------------------------------------------------------------*/
void AmayaStatusBar::OnLogErrorButton( wxCommandEvent& event )
{
  TTALOGDEBUG_0( TTA_LOG_DIALOG, _T("AmayaStatusBar::OnLogErrorButton"));
  Document doc;
  View view;
  FrameToView (TtaGiveActiveFrame(), &doc, &view);
  TtaExecuteMenuAction ("ShowLogFile", doc, view, FALSE);
}

/*----------------------------------------------------------------------
  OnLockButton
  this callback lock/unlock the webdav resource
  ----------------------------------------------------------------------*/
void AmayaStatusBar::OnLockButton( wxCommandEvent& event )
{
  TTALOGDEBUG_0( TTA_LOG_DIALOG, _T("AmayaStatusBar::OnLockButton"));
  Document doc;
  View view;

  FrameToView (TtaGiveActiveFrame(), &doc, &view);
  TtaExecuteMenuAction ("LockUnlock", doc, view, FALSE);
}

/*----------------------------------------------------------------------
  OnSize moves the logerror button at the right place
  ----------------------------------------------------------------------*/
void AmayaStatusBar::OnSize(wxSizeEvent& event)
{
  wxRect rect;

  if (m_pLogErrorButton)
  {
    GetFieldRect(Field_LogError, rect);
    //size.x += widths[Field_LogError];
#ifdef _MACOS
    m_pLogErrorButton->Move(rect.x, rect.y-1);
#else
    m_pLogErrorButton->Move(rect.x, rect.y-3);
#endif
  }
  if (m_pLockButton)
  {
    GetFieldRect(Field_Lock, rect);
#ifdef _MACOS
    m_pLockButton->Move(rect.x, rect.y-1);
#else
    m_pLockButton->Move(rect.x, rect.y-3);
#endif
  }

  GetFieldRect(Field_Path, rect);
  m_pathCtrl->SetSize(rect.x+1, rect.y+1, rect.width-1, rect.height-2);

  GetFieldRect(Field_InsertMode, rect);
  m_insertMode->SetSize(rect.x+1, rect.y+3, rect.width-1, rect.height-2);
  
  event.Skip();
}

/*----------------------------------------------------------------------
  EnableLogError
  change the button color and tooltips (error/noerror)
  ----------------------------------------------------------------------*/
void AmayaStatusBar::EnableLogError( bool enable )
{
  if (enable)
    {
      m_pLogErrorButton->SetToolTip(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_LIB_ERROR_DOC)));
      m_pLogErrorButton->SetBitmapLabel(m_LogErrorBmp_Red);
      m_pLogErrorButton->SetBitmapDisabled(m_LogErrorBmp_Red);
    }
  else
    {
      m_pLogErrorButton->SetToolTip(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_LIB_NOERROR_DOC)));
      m_pLogErrorButton->SetBitmapLabel(m_LogErrorBmp_Green);
      m_pLogErrorButton->SetBitmapDisabled(m_LogErrorBmp_Green);
    }
}

/*----------------------------------------------------------------------
  EnableLock
  change the button status and tooltips (lock/unlock)
  - 0 if not a webdav resource
  - 1 if the webdav resource is locked
  - 2 if the webdave resource is not locked
  ----------------------------------------------------------------------*/
void AmayaStatusBar::EnableLock (int status)
{
  if (status == 2)
    {
      m_pLockButton->SetToolTip(TtaConvMessageToWX(TtaGetMessage(LIB,LOCK)));
      m_pLockButton->SetBitmapLabel(m_Lock_Yes);
      m_pLockButton->SetBitmapDisabled(m_Lock_No);
    }
  else
    {
      if (status == 0)
        m_pLockButton->SetToolTip(TtaConvMessageToWX(TtaGetMessage(LIB,NO_WEBDAV)));
      else
        m_pLockButton->SetToolTip(TtaConvMessageToWX(TtaGetMessage(LIB,UNLOCK)));
      m_pLockButton->SetBitmapLabel(m_Lock_No);
      m_pLockButton->SetBitmapDisabled(m_Lock_Yes);
    }
}

/*----------------------------------------------------------------------
  SetSelectedElement
  change the path of the selected element (2nd field).
  ----------------------------------------------------------------------*/
void AmayaStatusBar::SetSelectedElement(Element elem)
{
  if (m_pathCtrl)
    m_pathCtrl->SetSelection(elem);
}

static wxString s_dots = wxT("...");
/*----------------------------------------------------------------------
  SetStatusText
  Set the text of the status bar.
  ----------------------------------------------------------------------*/
void AmayaStatusBar::SetStatusText(const wxString& text, int i)
{
  wxClientDC dc(this);
  wxSize     size, sz, szDots;
  size_t     s;

  dc.GetTextExtent(text, &size.x, &size.y);
  sz = GetClientSize();
  sz.x -= widths[Field_LogError];
  sz.x -= widths[Field_Lock];
  sz.x -= widths[Field_InsertMode];
  sz.x /= 2;
  if (size.x > sz.x)
    {
      // display the end of the text
      s = 3;
      dc.GetTextExtent(s_dots, &szDots.x, &szDots.y);
      sz.x -= szDots.x;
      sz.x -= 2;
      do
        {
          s++;
          dc.GetTextExtent(text.Mid(s), &size.x, &size.y);
        }
      while (size.x > sz.x);
      
      wxStatusBar::SetStatusText(s_dots + text.Mid(s), Field_Text);
    }
  else
    wxStatusBar::SetStatusText(text, Field_Text);
}


#endif /* _WX */
