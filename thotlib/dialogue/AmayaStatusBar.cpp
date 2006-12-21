#ifdef _WX

#include "wx/wx.h"

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

#include "AmayaStatusBar.h"
/* the log button should be shifted on MacOS platforms */
#ifdef _MACOS
#define LOG_SHIFT  24
#else /* _MACOS */
#define LOG_SHIFT  4
#endif /* _MACOS */

IMPLEMENT_DYNAMIC_CLASS(AmayaStatusBar, wxStatusBar)

//-----------------------------------------------------------------------------
// Event table: connect the events to the handler functions to process them
//-----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(AmayaStatusBar, wxStatusBar)
  EVT_BUTTON( -1, AmayaStatusBar::OnLogErrorButton )
  EVT_SIZE( AmayaStatusBar::OnSize )
END_EVENT_TABLE()

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
AmayaStatusBar::AmayaStatusBar( wxWindow * p_parent ) 
  : wxStatusBar(p_parent, wxID_ANY)
    ,m_LogErrorBmp_Green(TtaGetResourcePathWX( WX_RESOURCES_ICON_16X16, "ledgreen.png"), wxBITMAP_TYPE_PNG)
    ,m_LogErrorBmp_Red(TtaGetResourcePathWX( WX_RESOURCES_ICON_16X16, "ledred.png"), wxBITMAP_TYPE_PNG)
    ,m_pathCtrl(NULL)
{
  TTALOGDEBUG_0( TTA_LOG_DIALOG, _T("AmayaStatusBar::AmayaStatusBar"));


  // setup the logerror button
  m_pLogErrorButton = new wxBitmapButton( this, wxID_ANY, m_LogErrorBmp_Red,
                                          wxDefaultPosition, wxDefaultSize,
                                          wxBU_EXACTFIT | wxNO_BORDER);
  wxASSERT(m_pLogErrorButton);
  
  m_pathCtrl = new AmayaPathControl(this, wxID_ANY);

  // setup statusbar attributes
  static const int widths[Field_Max] = { -1, -1, m_pLogErrorButton->GetSize().GetWidth()+LOG_SHIFT};
  SetFieldsCount(Field_Max);
  SetStatusWidths(Field_Max, widths);
  SetMinHeight(m_pLogErrorButton->GetSize().GetHeight()+4);
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
  OnSize moves the logerror button at the right place
  ----------------------------------------------------------------------*/
void AmayaStatusBar::OnSize(wxSizeEvent& event)
{
  wxRect rect;
  GetFieldRect(Field_LogError, rect);
  
  wxASSERT(m_pLogErrorButton);
  wxSize size = m_pLogErrorButton->GetSize();
  m_pLogErrorButton->Move(rect.x + (rect.width - size.x - LOG_SHIFT + 4) / 2,
                          rect.y + (rect.height - size.y) / 2);

  GetFieldRect(Field_Path, rect);
  m_pathCtrl->SetSize(rect.x, rect.y, rect.width, rect.height);

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
  SetSelectedElement
  change the path of the selected element (2nd field).
  ----------------------------------------------------------------------*/
void AmayaStatusBar::SetSelectedElement(Element elem)
{
  if(m_pathCtrl)
    m_pathCtrl->SetSelection(elem);
}

/*----------------------------------------------------------------------
  SetStatusText
  Set the text of the status bar.
  ----------------------------------------------------------------------*/
void AmayaStatusBar::SetStatusText(const wxString& text, int i)
{
  wxStatusBar::SetStatusText(text, Field_Text);
}

#endif /* _WX */
