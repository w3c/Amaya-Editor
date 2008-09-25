/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2008
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

#ifdef _WX
#include "wx/wx.h"

#include "thot_gui.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "frame.h"
#include "registry.h"
#include "appdialogue.h"
#include "message.h"
#include "dialogapi.h"
#include "application.h"
#include "appdialogue_wx.h"
#include "windowtypes_wx.h"
#include "paneltypes_wx.h"
#include "message_wx.h"
#include "registry_wx.h"
#include "panel.h"
#include "thot_key.h"
#include "appdialogue_f.h"
#include "appdialogue_wx_f.h"
#include "applicationapi_f.h"
#include "font_f.h"
#include "editcommands_f.h"
#include "profiles_f.h"
#include "displayview_f.h"
#include "input_f.h"

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "edit_tv.h"
#include "frame_tv.h"
#include "boxparams_f.h"
#include "thotmsg_f.h"
#include "dialogapi_f.h"

#include "AmayaApp.h"
#include "AmayaCanvas.h"
#include "AmayaFrame.h"
#include "AmayaNormalWindow.h"
#include "AmayaSimpleWindow.h"
#include "AmayaPage.h"
#include "AmayaToolBar.h"
#include "AmayaPanel.h"
#include "AmayaXHTMLPanel.h"

#include "AmayaFileDropTarget.h"
#include "AmayaFrame.h"

/*----------------------------------------------------------------------
  -----------------------------------------------------------------------*/
bool AmayaFileDropTarget::OnDropFiles( wxCoord x, wxCoord y, const wxArrayString& filenames )
{
  Document            doc;
  View                view;
  int                 frame_id = 0;
  char                buffer[512];

  if (filenames.IsEmpty())
    return false;
  if (m_pOwner)
    frame_id = m_pOwner->GetFrameId();
  if (frame_id > 0 && frame_id <= MAX_FRAME)
  {
    FrameToView (frame_id, &doc, &view);
    PtrDocument pDoc = LoadedDocument[doc-1];
    //wxLogDebug(_T("AmayaFileDropTarget::OnDropFiles = ")+filenames[0]);
    strcpy (buffer, (const char*)filenames[0].mb_str(wxConvUTF8));
    
    if (pDoc->Call_Text)
      (*(Proc4)pDoc->Call_Text) ((void *)doc, (void *)view, (void *)buffer, (void *)TRUE);
  }
  return true;
}

/*----------------------------------------------------------------------
  -----------------------------------------------------------------------*/
AmayaFileDropTarget::~AmayaFileDropTarget ()
{
	m_pOwner = NULL;
}
#endif /* _WX */
