/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2008
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

#ifdef _WX
#include "wx/wx.h"
#include "wx/aui/auibook.h"
#include "wx/tglbtn.h"
#include "wx/string.h"
#include "wx/spinctrl.h"
#include "wx/tokenzr.h"
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
#include "message_wx.h"
#include "libmsg.h"
#include "frame.h"
#include "logdebug.h"

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "edit_tv.h"
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
#include "appdialogue_wx.h"
#include "input_f.h"
#include "editcommands_f.h"

#include "AmayaHelpWindow.h"

#include "AmayaPanel.h"

#include "AmayaMathMLPanel.h"
#include "AmayaSpeCharPanel.h"
#include "AmayaStylePanel.h"

#include "AmayaAdvancedNotebook.h"
#include "AmayaPage.h"
#include "AmayaFrame.h"
#include "AmayaCallback.h"
#include "AmayaQuickSplitButton.h"
#include "AmayaStatusBar.h"


IMPLEMENT_CLASS(AmayaHelpWindow, AmayaNormalWindow)

#include "archives.h"

/*----------------------------------------------------------------------
 *       Class:  AmayaHelpWindow
 *      Method:  AmayaHelpWindow
 * Description:  create a new AmayaHelpWindow
 -----------------------------------------------------------------------*/
AmayaHelpWindow::AmayaHelpWindow ( wxWindow * parent, wxWindowID id
                                      ,const wxPoint& pos
                                      ,const wxSize&  size
                                      ,int kind
                                      ) :
AmayaNormalWindow( parent, id, pos, size, kind ),
m_notebook(NULL)
{
  Initialize();
}

/*----------------------------------------------------------------------
 *       Class:  AmayaHelpWindow
 *      Method:  ~AmayaHelpWindow
 * Description:  destructor
 -----------------------------------------------------------------------*/
AmayaHelpWindow::~AmayaHelpWindow()
{
  SetAutoLayout(FALSE);
}

/*----------------------------------------------------------------------
 *       Class:  AmayaHelpWindow
 *      Method:  Initialize
 * Description:  Initialize the specific part of AmayaHelpWindow
 -----------------------------------------------------------------------*/
bool AmayaHelpWindow::Initialize()
{
  // Create the notebook
  m_notebook = new AmayaAdvancedNotebook(this, wxID_ANY );

  wxSizer* sz = new wxBoxSizer(wxVERTICAL);
  sz->Add(m_notebook, 1, wxEXPAND);
  SetSizer(sz);

  return AmayaNormalWindow::Initialize();
}


/*----------------------------------------------------------------------
 *       Class:  AmayaHelpWindow
 *      Method:  CleanUp
 * Description:  check that there is no empty pages
 -----------------------------------------------------------------------*/
void AmayaHelpWindow::CleanUp()
{
  if(m_notebook)
    m_notebook->CleanUp();
  AmayaNormalWindow::CleanUp();
}

/*----------------------------------------------------------------------
 *       Class:  AmayaHelpWindow
 *      Method:  LoadConfig
 * Description:  Load the config from registry and initialize dependancies
 -----------------------------------------------------------------------*/
void AmayaHelpWindow::LoadConfig()
{
  AmayaNormalWindow::LoadConfig();
}

/*----------------------------------------------------------------------
 *       Class:  AmayaHelpWindow
 *      Method:  SaveConfig
 * Description:  Save config to registry
 -----------------------------------------------------------------------*/
void AmayaHelpWindow::SaveConfig()
{
  //AmayaNormalWindow::SaveConfig();
}

/*----------------------------------------------------------------------
 *       Class:  AmayaHelpWindow
 *      Method:  CreateStatusBar
 * Description:  
  -----------------------------------------------------------------------*/
AmayaStatusBar * AmayaHelpWindow::CreateStatusBar()
{
  return NULL;
}

/*----------------------------------------------------------------------
 *       Class:  AmayaHelpWindow
 *      Method:  CreateMenuBar
 * Description:  
  -----------------------------------------------------------------------*/
void AmayaHelpWindow::CreateMenuBar()
{
}

/*----------------------------------------------------------------------
 *       Class:  AmayaHelpWindow
 *      Method:  SetPageIcon
 * Description:  
 -----------------------------------------------------------------------*/
void AmayaHelpWindow::SetPageIcon(int page_id, char *iconpath)
{
  int index = TtaGetIconIndex (iconpath);
  if (m_notebook && index > 0)
    m_notebook->SetPageImage (page_id, index);
}

/*----------------------------------------------------------------------
 *       Class:  AmayaHelpWindow
 *      Method:  CreatePage
 * Description:  create a new AmayaPage.
 *               it's possible to attach automaticaly this page to the window or not
 -----------------------------------------------------------------------*/
AmayaPage * AmayaHelpWindow::CreatePage( Document doc, bool attach, int position )
{
  // Force using AmayaSimplePage : help pages are not code-viewable
  AmayaPage * page = new AmayaSimplePage( m_notebook, this );
  
  if (attach)
    AttachPage( position, page );
  
  return page;
}

/*----------------------------------------------------------------------
 *       Class:  AmayaHelpWindow
 *      Method:  GetPageContainer
 * Description:  Return the container of AmayaPages
 -----------------------------------------------------------------------*/
AmayaPageContainer* AmayaHelpWindow::GetPageContainer()
{
  return m_notebook;
}

/*----------------------------------------------------------------------
 *       Class:  AmayaHelpWindow
 *      Method:  GetPageContainer
 * Description:  Return the container of AmayaPages
 -----------------------------------------------------------------------*/
const AmayaPageContainer* AmayaHelpWindow::GetPageContainer()const
{
  return m_notebook;
}

/*----------------------------------------------------------------------
 *       Class:  AmayaHelpWindow
 *      Method:  OnClose
 * Description:  Intercept the CLOSE event and prevent it if ncecessary.
  -----------------------------------------------------------------------*/
void AmayaHelpWindow::OnClose(wxCloseEvent& event)
{
  if(m_notebook)
  {
    // Intend to close the notebook
    if(!m_notebook->Close())
    {
      event.Veto();
      return;
    }
    m_notebook->Destroy();
    m_notebook = NULL;
  }
  event.Skip();
}


/*----------------------------------------------------------------------
 *  this is where the event table is declared
 *  the callbacks are assigned to an event type
 *----------------------------------------------------------------------*/
BEGIN_EVENT_TABLE(AmayaHelpWindow, AmayaNormalWindow)
  EVT_CLOSE(AmayaHelpWindow::OnClose )
END_EVENT_TABLE()


#endif /* #ifdef _WX */
