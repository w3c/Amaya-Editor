/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2009
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

#ifdef _WX
#include "wx/wx.h"
#include "wx/string.h"

#include "thot_gui.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "AmayaCanvas.h"

#include "typemedia.h"
#include "appdialogue.h"
#include "dialog.h"
#include "application.h"
#include "dialog.h"
#include "document.h"
#include "message.h"
#include "libmsg.h"
#include "frame.h"
#include "appdialogue_wx.h"
#include "registry_wx.h"
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
#include "appdialogue_wx_f.h"
#include "appdialogue_f.h"
#include "boxparams_f.h"
#include "displayview_f.h"

#include "wx/log.h"

#include "AmayaWindow.h"
#include "AmayaFrame.h"
#include "AmayaPage.h"
#include "AmayaClassicNotebook.h"
#include "AmayaQuickSplitButton.h"

//
//
//
// AmayaPage
//
//
//

AmayaPage* AmayaPage::CreateAmayaPage(wxWindow* parent, AmayaWindow* window, Document doc)
{
  switch(TtaGetDocumentPageType(doc))
  {
    case WXAMAYAPAGE_SPLITTABLE:
      return new AmayaSplittablePage(parent, window);
    default:
      return new AmayaSimplePage(parent, window);
  }
}

IMPLEMENT_ABSTRACT_CLASS(AmayaPage, wxPanel)

BEGIN_EVENT_TABLE(AmayaPage, wxPanel)
END_EVENT_TABLE()

/*----------------------------------------------------------------------
 *       Class:  AmayaPage
 *      Method:  AmayaPage
 * Description:  constructor of the page.
  -----------------------------------------------------------------------*/
AmayaPage::AmayaPage( wxWindow * p_parent_window, AmayaWindow * p_amaya_parent_window ):
wxPanel( p_parent_window, -1, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, wxT("AmayaPage")),
m_pContainer( NULL ),
m_PageId(-1),
m_IsClosed( FALSE ),
m_IsSelected(false),
m_MasterFrameId(-1)
{
  
}

/*----------------------------------------------------------------------
 *       Class:  AmayaPage
 *      Method:  ~AmayaPage
  -----------------------------------------------------------------------*/
AmayaPage::~AmayaPage()
{
  
}

/*----------------------------------------------------------------------
 *       Class:  AmayaPage
 *      Method:  GetWindowParent
 * Description:  set/get the top window parent
  -----------------------------------------------------------------------*/
AmayaWindow * AmayaPage::GetWindowParent()
{
  return wxDynamicCast(wxGetTopLevelParent(this), AmayaWindow);
}

/*----------------------------------------------------------------------
 *       Class:  AmayaPage
 *      Method:  SetPageId
 * Description:  update the page id (page position in the notebook)
  -----------------------------------------------------------------------*/
void AmayaPage::SetPageId( int page_id )
{
  // update each owned frames
  int frame_id = 1;
  for(frame_id=1; frame_id<MAX_FRAME; frame_id++)
    {
      if (!TtaFrameIsClosed(frame_id))
        {
          // if this frame is owned by this page
          if (FrameTable[frame_id].FrPageId == m_PageId)
            {
              FrameTable[frame_id].FrPageId = page_id;
            }
        }
    }

  // update the internal page_id with the new one
  m_PageId = page_id;
}

/*----------------------------------------------------------------------
 *       Class:  AmayaPage
 *      Method:  SetSelected
 * Description:  the notebook has a new selected page then it calls SetSelected(TRUE)
 *               on the selected page and SetSelected(FALSE) on the other one
  -----------------------------------------------------------------------*/
void AmayaPage::SetSelected( bool isSelected )
{
  m_IsSelected = isSelected;
  if (isSelected)
    {
      // if there is an active frame
      if ( GetActiveFrame() )
        {
          // activate it : setup the corresponding menu and update internal boolean
          GetActiveFrame()->SetActive( TRUE );
        }
    }
}

/*----------------------------------------------------------------------
 *       Class:  AmayaPage
 *      Method:  RaisePage
 * Description:  raise the page and the window if necessary
  -----------------------------------------------------------------------*/
void AmayaPage::RaisePage()
{
  AmayaPageContainer * p_container = GetContainer();

  if (p_container)
    {
      // raise the notebook page
      p_container->SetSelection(GetPageId());
      SetSelected( TRUE );
    }
  // force the window's parent to raises to the top of the window hierarchy 
  // if it is a managed window (dialog or frame).
  if ( GetWindowParent() )
    GetWindowParent()->Raise();
}







//
//
//
// AmayaSimplePage
//
//
//

IMPLEMENT_DYNAMIC_CLASS(AmayaSimplePage, AmayaPage)

BEGIN_EVENT_TABLE(AmayaSimplePage, AmayaPage)
  EVT_CLOSE(AmayaSimplePage::OnClose )
END_EVENT_TABLE()


/*----------------------------------------------------------------------
 *       Class:  AmayaSimplePage
 *      Method:  AmayaSimplePage
 * Description:  constructor of the page 
  -----------------------------------------------------------------------*/
AmayaSimplePage::AmayaSimplePage( wxWindow * p_parent_window, AmayaWindow * p_amaya_parent_window):
AmayaPage( p_parent_window, p_amaya_parent_window),
m_pFrame(NULL)
{
  SetSizer(new wxBoxSizer(wxVERTICAL));
  SetAutoLayout(true);
}

/*----------------------------------------------------------------------
 *       Class: AmayaSimplePage 
 *      Method: ~AmayaSimplePage 
 * Description: destructor of AmayaSimplePage (do nothing)
  -----------------------------------------------------------------------*/
AmayaSimplePage::~AmayaSimplePage()
{
  
}

/*----------------------------------------------------------------------
*       Class: AmayaSimplePage 
*      Method: AttachFrame
* Description: attache a AmayaFrame to the page (top or bottom)
*      params:
*        + AmayaFrame * p_frame : the frame to attach
*        + int position : the position identifier - top (1) or bottom (2)
*      return:
*        + AmayaFrame * : the old frame or NULL if there was no old frame at this place
 -----------------------------------------------------------------------*/
AmayaFrame *AmayaSimplePage::AttachFrame (AmayaFrame *p_frame, int pos, int split)
{
  AmayaFrame* res = m_pFrame;
  
  wxSizer *sz = GetSizer();
  if(m_pFrame)
    sz->Detach(m_pFrame);
  
  p_frame->Reparent(this);
  sz->Add(p_frame, 1, wxEXPAND);

  m_pFrame = p_frame;
  SetMasterFrameId(p_frame->GetFrameId());
  
  
  if (p_frame)
    p_frame->SetPageParent( this ); // I'm your parent
  // update the page title (same as bottom frame)
  if (p_frame)
    p_frame->SetFrameTitle(p_frame->GetFrameTitle());

  
  sz->Layout();
  SetAutoLayout(true);
  return res;
}

/*----------------------------------------------------------------------
 *       Class:  AmayaSimplePage
 *      Method:  DetachFrame
 * Description:  detache the frame (hide it but don't delete it)
 *      params:
 *        + 1 the top frame
 *        + 2 the bottom frame 
 *      return:
 *        + AmayaFrame* : the detached frame
  -----------------------------------------------------------------------*/
AmayaFrame * AmayaSimplePage::DetachFrame( int )
{
  AmayaFrame* res = m_pFrame;
  GetSizer()->Detach(m_pFrame);
  m_pFrame = NULL;
  SetMasterFrameId(-1);
  return res;
}

/*----------------------------------------------------------------------
 *       Class:  AmayaSimplePage
 *      Method:  CleanUp
 * Description:  check that there is no empty pages
 *      Return:  true if the page can be removed from parent
  -----------------------------------------------------------------------*/
bool AmayaSimplePage::CleanUp()
{
  return !m_pFrame;
}

/*----------------------------------------------------------------------
 *       Class:  AmayaSimplePage
 *      Method:  GetFramePosition
 * Description:  return the frame position if it exist in the current page
  -----------------------------------------------------------------------*/
int AmayaSimplePage::GetFramePosition( const AmayaFrame * p_frame ) const
{
  return p_frame==m_pFrame?1:0;
}

/*----------------------------------------------------------------------
 *       Class:  AmayaSimplePage
 *      Method:  OnClose
 * Description:  Intercept the CLOSE event and prevent it if ncecessary.
  -----------------------------------------------------------------------*/
void AmayaSimplePage::OnClose(wxCloseEvent& event)
{
  TTALOGDEBUG_1( TTA_LOG_DIALOG, _T("AmayaSimplePage::OnClose frame=%d"), 
                                     m_pFrame?m_pFrame->GetFrameId():-1);
  
  SetIsClosed(true);
  
  if(m_pFrame)
    {
      int frame_id = m_pFrame->GetFrameId();
      int page_id  = FrameTable[frame_id].FrPageId;

      if (page_id > 0)
        page_id--;

      // try to close the frame : the user can choose to close or not with a dialog
      m_pFrame->Close();
      
      // if the user doesn't want to close then just reattach the frame
      if ( !TtaFrameIsClosed (frame_id) )
        {
          // if the frame didnt die, just re-attach it
          AttachFrame(m_pFrame, 1, 1);
          SetIsClosed(false);
        }
    }

  if(!IsClosed())
    event.Veto();
}


//
//
//
// AmayaSplittablePage
//
//
//

IMPLEMENT_DYNAMIC_CLASS(AmayaSplittablePage, AmayaPage)

/*----------------------------------------------------------------------
 *       Class:  AmayaSplittablePage
 *      Method:  AmayaSplittablePage
 * Description:  constructor of the page :
 *   wxBoxSizer
 *   [
 *       wxSplitterWindow
 *       [
 *           AmayaFrame
 *           AmayaFrame
 *       ]
 *   ]
  -----------------------------------------------------------------------*/
AmayaSplittablePage::AmayaSplittablePage( wxWindow * p_parent_window,
                                          AmayaWindow * p_amaya_parent_window )
  :  AmayaPage( p_parent_window, p_amaya_parent_window)
  ,m_pWindowParent( p_amaya_parent_window )
  ,m_SlashRatio( 0.6 )
  ,m_ActiveFrame(1) // by default, frame 1 is selected
{
  // Insert a forground sizer
  wxBoxSizer * p_sizerTop = new wxBoxSizer ( wxHORIZONTAL );
  SetSizer( p_sizerTop );

  wxBoxSizer * p_sizerTop2 = new wxBoxSizer ( wxVERTICAL );
  p_sizerTop->Add( p_sizerTop2, 1, wxEXPAND, 0 );
 
  // Insert a windows splitter 
  m_pSplitterWindow = new wxSplitterWindow( this, -1
					    ,wxDefaultPosition
					    ,wxDefaultSize
#if 0
					    ,/*wxSP_FULLSASH |*/
					    wxSP_3DSASH |
					    wxSP_BORDER |
					    wxSP_3D 
					    /* | wxSP_NOBORDER*/
					    /*| wxSP_PERMIT_UNSPLIT*/
#endif /* 0 */
					    );
  SetSplitMode(0); // undefined
  p_sizerTop2->Add( m_pSplitterWindow, 1, wxEXPAND, 0 );


  // Split button creation
  // this button is used to quickly split the page horizontaly
  m_pSplitButtonBottom = new AmayaQuickSplitButton( this, wxID_ANY, AmayaQuickSplitButton::wxAMAYA_QS_HORIZONTAL);
  p_sizerTop2->Add( m_pSplitButtonBottom, 0, wxEXPAND, 0 );
  // this button is used to quickly split the page verticaly
  m_pSplitButtonRight = new AmayaQuickSplitButton( this, wxID_ANY, AmayaQuickSplitButton::wxAMAYA_QS_VERTICAL);
  p_sizerTop->Add( m_pSplitButtonRight, 0, wxEXPAND, 0 );

  /// Insert to area : Top / bottom
  m_pTopFrame     = NULL;
  m_pBottomFrame  = NULL;
  m_pSplitterWindow->SetMinimumPaneSize( 50 );

  // Create a dummy panel to initilize the splitter window with something
  m_DummyPanel = new wxPanel( m_pSplitterWindow, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, wxT("Empty default panel"));
  m_pSplitterWindow->Initialize( m_DummyPanel );

  // initialize the last open view name
  //strcpy(m_LastOpenViewName, "Formatted_view");
  SetAutoLayout(TRUE);
}


/*----------------------------------------------------------------------
 *       Class: AmayaSplittablePage 
 *      Method: ~AmayaSplittablePage 
 * Description: destructor of AmayaSplittablePage (do nothing)
  -----------------------------------------------------------------------*/
AmayaSplittablePage::~AmayaSplittablePage()
{
}

/*----------------------------------------------------------------------
 *       Class: AmayaSplittablePage 
 *      Method: GetQuickSplitButton
 * Description: destructor of AmayaPage (do nothing)
  -----------------------------------------------------------------------*/
AmayaQuickSplitButton * AmayaSplittablePage::GetQuickSplitButton (ThotBool horizontal)
{
  if (horizontal)
	 return m_pSplitButtonBottom;
  else
     return m_pSplitButtonRight;
}

 /*----------------------------------------------------------------------
   Class: AmayaSplittablePage 
   Method: AttachFrame
   Description: attache a AmayaFrame to the page (top or bottom)
   params:
   + AmayaFrame * p_frame : the frame to attach
   + int position : the position identifier - top (1) or bottom (2)
   + split: 0 for horizontal, 1 if for vertical
   return:
   + AmayaFrame * : the old frame or NULL if there was no old frame at this place
  -----------------------------------------------------------------------*/
AmayaFrame *AmayaSplittablePage::AttachFrame (AmayaFrame *p_frame, int position, int split)
{
  // check if this is the first frame or not
  // the first one will be considered ad the master
  // the master frame is the on which control the urlbar, buttons (TODO : and menus)
  if ( !m_pTopFrame && !m_pBottomFrame )
    SetMasterFrameId(p_frame->GetFrameId());

  // Select the right frame
  AmayaFrame ** pp_frame_container = NULL;
  switch (position)
    {
    case 1:
      pp_frame_container = &m_pTopFrame;
      break;
    case 2:
      pp_frame_container = &m_pBottomFrame;
      break;
    default:
      wxASSERT_MSG(FALSE, _T("AmayaSplittablePage::AttachFrame -> Bad position"));
      return NULL;
      break;
    }

  // remember what is the oldframe
  AmayaFrame * oldframe = *pp_frame_container;
  if (p_frame == NULL || p_frame == oldframe )
  {
    if (!m_pSplitterWindow->IsSplit())
  	{
  	  // on passe ici qd on essaye de charger un nouveau document dans la meme fenetre
  	  // je ne sais pas si c necessaire de forcer le split
  	  // de toute facon si on le fait il faut verifier que le top et le bottom ne sont pas null
  	  // m_pSplitterWindow->SplitHorizontally( m_pTopFrame, m_pBottomFrame );
    	  AdjustSplitterPos();
  	}
    return NULL;
  }

  /* p_frame is the new top frame */  
  *pp_frame_container = p_frame;

  bool ok = false;
  if (oldframe != NULL)
    ok = m_pSplitterWindow->ReplaceWindow( oldframe, p_frame );
  else if (m_pBottomFrame == NULL || m_pTopFrame == NULL)
    {
    ok = m_pSplitterWindow->ReplaceWindow( m_DummyPanel, p_frame );
    m_DummyPanel->Hide();
    }
  else
    {
    AmayaFrame * p_frame = GetFrame(1);
    Document document = FrameTable[p_frame->GetFrameId()].FrDoc;
    View view         = FrameTable[p_frame->GetFrameId()].FrView;
    int mode = m_pSplitterWindow->GetSplitMode();
    if ((split == 2 && mode != wxSPLIT_HORIZONTAL) || mode == wxSPLIT_VERTICAL)
      {
  	  ok = m_pSplitterWindow->SplitVertically( m_pTopFrame, m_pBottomFrame );
  	  TtaExecuteMenuAction ("ShowVSplitToggle", document, view, FALSE);
      }
    else
      {
  	  ok = m_pSplitterWindow->SplitHorizontally( m_pTopFrame, m_pBottomFrame );
  	  TtaExecuteMenuAction ("ShowHSplitToggle", document, view, FALSE);
      }
    AdjustSplitterPos();
    }
  wxASSERT_MSG(ok, _T("AmayaSplittablePage::AttachFrame -> Impossible d'attacher la frame") );

  // update old and new AmayaFrame parents
  if (oldframe)
    oldframe->SetPageParent( NULL ); // no more parent
  if (p_frame)
    p_frame->SetPageParent( this ); // I'm your parent
 
  // update the page title (same as bottom frame)
  if (p_frame)
    p_frame->SetFrameTitle(p_frame->GetFrameTitle());

  SetAutoLayout(TRUE);

  // remember the last open view
  //strcpy(m_LastOpenViewName, FrameTable[p_frame->GetFrameId()].FrViewName);

  // return the old frame : needs to be manualy deleted ..
  return oldframe;
}

/*----------------------------------------------------------------------
 *       Class:  AmayaSplittablePage
 *      Method:  DetachFrame
 * Description:  detache the frame (hide it but don't delete it)
 *      params:
 *        + 1 the top frame
 *        + 2 the bottom frame 
 *      return:
 *        + AmayaFrame* : the detached frame
  -----------------------------------------------------------------------*/
AmayaFrame * AmayaSplittablePage::DetachFrame( int position )
{
  // Select the right frame : top or bottom
  AmayaFrame **pp_frame_container = NULL;
  AmayaFrame  *p_other_frame = NULL, *oldframe;
  int          active_frame_position;
  bool         isUnsplited;

  switch (position)
    {
    case 1:
      pp_frame_container = &m_pTopFrame;
      p_other_frame      = m_pBottomFrame;
      break;
    case 2:
      pp_frame_container = &m_pBottomFrame;
      p_other_frame      = m_pTopFrame;
      break;
    default:
      return NULL;
      break;
    }

  oldframe = *pp_frame_container;
  if (oldframe == NULL)
    return NULL;

  isUnsplited = m_pSplitterWindow->Unsplit (oldframe);
  if (!isUnsplited)
    {
      // The frame is alone and can't be unsplit
      // => replace it with a dummy panel
      m_DummyPanel->Show();
#ifdef __WXDEBUG__
      bool isReplaced = m_pSplitterWindow->ReplaceWindow( oldframe, m_DummyPanel );
      wxASSERT_MSG( isReplaced, _T("La frame n'a pas pu etre remplacee") );
#else /* __WXDEBUG__ */
      m_pSplitterWindow->ReplaceWindow( oldframe, m_DummyPanel );
#endif /* __WXDEBUG__ */
    }
  if (oldframe)
    oldframe->SetActive( FALSE );

  *pp_frame_container = NULL;
  // This frame is not anymore active
  // activate the other frame
  active_frame_position = 1;
  while (!GetFrame( active_frame_position ) && active_frame_position <= MAX_MULTI_FRAME )
    active_frame_position++;
  if ( active_frame_position <= MAX_MULTI_FRAME )
    {
      // there is another frame
      // so activate it
      AmayaFrame * p_frame = GetFrame(active_frame_position );
      SetActiveFrame( p_frame );
      p_other_frame = GetActiveFrame();
      if (p_other_frame != p_frame)
        p_other_frame->SetActive( TRUE );
    }
  else
    {
      // there is no more frame
      SetActiveFrame( NULL );
    }

#if defined(_WINDOWS) || defined(_MACOS)
  // simulate a size event to refresh the canvas ...
  // this is usefull when a document is modified and there is many open views :
  // if nothing is done here when a view is detached from the page, a undraw
  // zone appears into the canvas on the other frame...
  // this is not very clean but it works (maybe try to remove this on further wxWidgets version)
  if (p_other_frame && p_other_frame->GetCanvas())
    {
      wxSizeEvent event( p_other_frame->GetCanvas()->GetSize() );
      wxPostEvent(p_other_frame->GetCanvas(), event );
    }
#endif /* _WINDOWS */

  // check if there is no more frame in the page
  // if there is no more frame, the master frame must be erased 
  // the master frame is the on which control the urlbar, buttons (TODO : and menus)
  if ( !m_pTopFrame && !m_pBottomFrame )
    SetMasterFrameId(-1);
  return oldframe;
}

/*----------------------------------------------------------------------
 *       Class:  AmayaSplittablePage
 *      Method:  DoBottomSplitButtonAction
 * Description:  
  -----------------------------------------------------------------------*/
void AmayaSplittablePage::DoBottomSplitButtonAction()
{
  // Update toggle menu item
  AmayaFrame * p_frame = GetFrame(1);
  Document document = FrameTable[p_frame->GetFrameId()].FrDoc;
  View view         = FrameTable[p_frame->GetFrameId()].FrView;

  // check if we must switch the split orientation
  if ( m_pSplitterWindow->IsSplit() &&
       m_pSplitterWindow->GetSplitMode() == wxSPLIT_VERTICAL )
    {
      DoSwitchHoriVert();
      // Update toggle buttons
      TtaExecuteMenuAction ("HideVSplitToggle", document, view, FALSE);
      TtaExecuteMenuAction ("ShowHSplitToggle", document, view, FALSE);
    }
  else
    {
      // Update toggle buttons
      if ( m_pSplitterWindow->IsSplit() )
        TtaExecuteMenuAction ("HideHSplitToggle", document, view, FALSE);
      else
        TtaExecuteMenuAction ("ShowHSplitToggle", document, view, FALSE);
      // store the wanted orientation for the next split action
      SetSplitMode (wxSPLIT_HORIZONTAL);
      // do the split/unsplit action
      DoSplitUnsplit (wxSPLIT_HORIZONTAL);
    }
}

/*----------------------------------------------------------------------
 *       Class:  AmayaSplittablePage
 *      Method:  DoRightSplitButtonAction
 * Description:  
  -----------------------------------------------------------------------*/
void AmayaSplittablePage::DoRightSplitButtonAction()
{
  AmayaFrame * p_frame = GetFrame(1);
  Document document = FrameTable[p_frame->GetFrameId()].FrDoc;
  View view         = FrameTable[p_frame->GetFrameId()].FrView;

  // check if we must switch the split orientation
  if ( m_pSplitterWindow->IsSplit() &&
       m_pSplitterWindow->GetSplitMode() == wxSPLIT_HORIZONTAL )
    {
      DoSwitchHoriVert();
      // Update toggle buttons
      TtaExecuteMenuAction ("HideHSplitToggle", document, view, FALSE);
      TtaExecuteMenuAction ("ShowVSplitToggle", document, view, FALSE);
    }
  else
    {
      // Update toggle buttons
      if ( m_pSplitterWindow->IsSplit() )
        TtaExecuteMenuAction ("HideVSplitToggle", document, view, FALSE);
      else
        TtaExecuteMenuAction ("ShowVSplitToggle", document, view, FALSE);
      // store the wanted orientation for the next split action
      SetSplitMode (wxSPLIT_VERTICAL);
      // do the split/unsplit action
      DoSplitUnsplit (wxSPLIT_VERTICAL);
    }
}

/*----------------------------------------------------------------------
 *       Class:  AmayaSplittablePage
 *      Method:  OnSplitButton
 * Description:  this method is called when the button for quickly split is pushed
  -----------------------------------------------------------------------*/
void AmayaSplittablePage::OnSplitButton( wxCommandEvent& event )
{
  // do nothing if the button is not a quicksplit one
  if ( event.GetId() == m_pSplitButtonBottom->GetId() )
    DoBottomSplitButtonAction();
  else if ( event.GetId() == m_pSplitButtonRight->GetId() )
    DoRightSplitButtonAction();
  else 
    {
      event.Skip();
      return;
    }
}


/*----------------------------------------------------------------------
 *       Class:  AmayaSplittablePage
 *      Method:  SetSplitMode
 * Description:  used to control the split orientation (call it just before DoSplitUnsplit())
  -----------------------------------------------------------------------*/
void AmayaSplittablePage::SetSplitMode( int mode )
{
  m_pSplitterWindow->SetSplitMode(mode);
}

/*----------------------------------------------------------------------
 *       Class:  AmayaSplittablePage
 *      Method:  GetSplitterWindow
 * Description:  return the page's splitter window pointer 
  -----------------------------------------------------------------------*/
wxSplitterWindow * AmayaSplittablePage::GetSplitterWindow()
{
  return m_pSplitterWindow;
}

/*----------------------------------------------------------------------
 *       Class:  AmayaSplittablePage
 *      Method:  DoSplitUnsplit
 * Description:  toggle split/unsplit state
  -----------------------------------------------------------------------*/
void AmayaSplittablePage::DoSplitUnsplit(int mode)
{
    AmayaFrame * p_frame = GetFrame(1);
    if (p_frame == NULL)
      return;
    Document document = FrameTable[p_frame->GetFrameId()].FrDoc;
    View view         = FrameTable[p_frame->GetFrameId()].FrView;

  if (!m_pSplitterWindow->IsSplit())
    {
#ifdef IV
      if ( !strcmp(m_LastOpenViewName, "Links_view") )
        TtaExecuteMenuAction ("ShowLinks", document, view, FALSE);
      else if ( !strcmp(m_LastOpenViewName, "Alternate_view") )
        TtaExecuteMenuAction ("ShowAlternate", document, view, FALSE);
      else if ( !strcmp(m_LastOpenViewName, "Table_of_contents") )
        TtaExecuteMenuAction ("ShowToC", document, view, FALSE);
      else
#endif
        {
          if (mode == wxSPLIT_HORIZONTAL)
            TtaExecuteMenuAction ("ShowSource", document, view, FALSE);
          else // if ( !strcmp(m_LastOpenViewName, "Structure_view") )
            TtaExecuteMenuAction ("ShowStructure", document, view, FALSE); 
        }
    }
  else
    {
	  FrameToView(TtaGiveActiveFrame(), &document, &view);
    // remember the last open view
    //if (FrameTable[view].FrViewName)
    //strcpy(m_LastOpenViewName, FrameTable[view].FrViewName);
	  TtaExecuteMenuAction("Synchronize", document, view, TRUE);

	  if(m_pBottomFrame)
        m_pBottomFrame->Close();
    }
}

/*----------------------------------------------------------------------
 *       Class:  AmayaSplittablePage
 *      Method:  DoSwitchHoriVert
 * Description:  switch horizontal/vertical split direction
  -----------------------------------------------------------------------*/
void AmayaSplittablePage::DoSwitchHoriVert()
{
  if (m_pSplitterWindow->IsSplit())
    {
      m_pSplitterWindow->Unsplit();
      m_pTopFrame->Show(true);
      m_pBottomFrame->Show(true); 
      if (m_pSplitterWindow->GetSplitMode() == wxSPLIT_VERTICAL)
        m_pSplitterWindow->SplitHorizontally( m_pTopFrame, m_pBottomFrame );
      else
        m_pSplitterWindow->SplitVertically( m_pTopFrame, m_pBottomFrame );
      AdjustSplitterPos();
      TtaCheckLostFocus(); // sometimes the focus is lost at this point
    }
}

/*----------------------------------------------------------------------
 *       Class:  AmayaSplittablePage
 *      Method:  OnSplitterPosChanged
 * Description:  this method is called when the splitter position has changed
 *               this method calculate the new ratio (m_SlashRatio)
  -----------------------------------------------------------------------*/
void AmayaSplittablePage::OnSplitterPosChanged( wxSplitterEvent& event )
{
    // calculate the new ratio (depending of window size)
    float new_slash_pos = event.GetSashPosition();
    float height = GetSize().GetHeight();
    float width = GetSize().GetWidth();
    int split_mode = m_pSplitterWindow->GetSplitMode();
    if ( split_mode == 1 ) // horizontally
      m_SlashRatio = new_slash_pos / height;
    else if ( split_mode == 2 ) // vertically
      m_SlashRatio = new_slash_pos / width;
    if ( m_SlashRatio <= 0 || m_SlashRatio >= 1 )
      m_SlashRatio = 0.6;
    
    // do not forward this event because the panel/notebook splitter should not receive it
    //    event.Skip();
}

/*----------------------------------------------------------------------
 *       Class:  AmayaSplittablePage
 *      Method:  OnSplitterDClick
 * Description:  called when a double click is done on the splitbar
 *               detach the bottom frame
  -----------------------------------------------------------------------*/
void AmayaSplittablePage::OnSplitterDClick( wxSplitterEvent& event )
{
  AmayaFrame * p_frame = GetFrame(1);
  Document document = FrameTable[p_frame->GetFrameId()].FrDoc;
  View view         = FrameTable[p_frame->GetFrameId()].FrView;

  DoSplitUnsplit (wxSPLIT_VERTICAL);
  // Update Toggle buttons
  TtaExecuteMenuAction ("HideHSplitToggle", document, view, FALSE);
  TtaExecuteMenuAction ("HideVSplitToggle", document, view, FALSE);
}

/*----------------------------------------------------------------------
 *       Class:  AmayaSplittablePage
 *      Method:  OnSplitterUnsplit
 * Description:  called when the splitbar is unsplited
 *               TODO
  -----------------------------------------------------------------------*/
void AmayaSplittablePage::OnSplitterUnsplit( wxSplitterEvent& event )
{
  // the frame has been maybe unsplited manualy
  // maybe an update is needed
  int pos = GetFramePosition( wxDynamicCast(event.GetWindowBeingRemoved(), AmayaFrame) );
  switch ( pos )
    {
    case 1:
      m_pTopFrame = NULL;
      break;
    case 2:
      m_pBottomFrame = NULL;
      break;
    }

  event.Skip();
}

/*----------------------------------------------------------------------
 *       Class:  AmayaSplittablePage
 *      Method:  OnSize
 * Description:  called when the AmayaPage is resized.
 *               assign a right value to the splitbar position
 *               depending of m_SlashRatio position
  -----------------------------------------------------------------------*/
void AmayaSplittablePage::OnSize( wxSizeEvent& event )
{
  AdjustSplitterPos();
  event.Skip();
}

/*----------------------------------------------------------------------
 *       Class:  AmayaSplittablePage
 *      Method:  AdjustSplitterPos
 * Description:  adjust the splitbar position,
 *               the split bar pos could be horizontal or vertical
  -----------------------------------------------------------------------*/
void AmayaSplittablePage::AdjustSplitterPos( int height, int width )
{
  if ( height == -1 )
    height = GetSize().GetHeight();
  if ( width == -1 )
    width = GetSize().GetWidth();

  // force the splitter position (depending of window size)
  float new_height    = height;
  float new_width     = width;
  float new_slash_pos = 0;
  int split_mode      = m_pSplitterWindow->GetSplitMode();
  switch (split_mode)
    {
    case 1:
      new_slash_pos = m_SlashRatio * new_height;
      break;
    case 2:
      new_slash_pos = m_SlashRatio * new_width;
      break;
    default:
      wxASSERT(FALSE);
      break;
    }  
  if ( new_slash_pos > 0 && (GetFrame(1) && GetFrame(2)) )
    m_pSplitterWindow->SetSashPosition( (int)new_slash_pos );
}


/*----------------------------------------------------------------------
 *       Class:  AmayaSplittablePage
 *      Method:  OnClose
 * Description:  Intercept the CLOSE event and prevent it if ncecessary.
  -----------------------------------------------------------------------*/
void AmayaSplittablePage::OnClose(wxCloseEvent& event)
{
  TTALOGDEBUG_2( TTA_LOG_DIALOG, _T("AmayaSplittablePage::OnClose topframe=%d bottomframe=%d"),
                 m_pTopFrame ? m_pTopFrame->GetFrameId() : -1,
                 m_pBottomFrame ? m_pBottomFrame->GetFrameId() : -1 );
  
  SetIsClosed(true);
  int frame_id = 0;
  int page_id = 0;
  AmayaFrame * p_AmayaFrame = NULL;
  // Kill top frame
  if ( m_pTopFrame )
    {
      p_AmayaFrame = m_pTopFrame;
      frame_id     = m_pTopFrame->GetFrameId();
      page_id = FrameTable[frame_id].FrPageId;
      if (page_id > 0)
        page_id--;
      // try to close the frame : the user can choose to close or not with a dialog
      p_AmayaFrame->Close();
      
      // if the user doesn't want to close then just reattach the frame
      if ( !TtaFrameIsClosed (frame_id) )
        {
          // if the frame didnt die, just re-attach it
          AttachFrame(p_AmayaFrame, 1, 1);
          SetIsClosed(false);
        }
    }

  // Kill bottom frame
  if ( m_pBottomFrame && IsClosed())
    { 
      p_AmayaFrame = m_pBottomFrame;
      frame_id     = m_pBottomFrame->GetFrameId();
      page_id = FrameTable[frame_id].FrPageId;	
      // try to close the frame : the user can choose to close or not with a dialog
      p_AmayaFrame->Close();
      
      // if the user don't want to close then just reattach the frame
      if ( !TtaFrameIsClosed (frame_id) )
        {
          // if the frame didnt die, just re-attach it
          AttachFrame(p_AmayaFrame, 2, 1);
          SetIsClosed(false);
        }
      
    }

  if(!IsClosed())
    event.Veto();
}

/*----------------------------------------------------------------------
 *       Class:  AmayaSplittablePage
 *      Method:  CleanUp
 * Description:  check that there is no empty pages
 *      Return:  true if the page can be removed from parent
  -----------------------------------------------------------------------*/
bool AmayaSplittablePage::CleanUp()
{
  return (!m_pTopFrame && !m_pBottomFrame);
}

/*----------------------------------------------------------------------
 *       Class:  AmayaSplittablePage
 *      Method:  GetFrame
 * Description:  return the first or second page frame
  -----------------------------------------------------------------------*/
AmayaFrame * AmayaSplittablePage::GetFrame( int frame_position ) const
{
  if (frame_position == 1)
    {
      return m_pTopFrame;
    }
  else if (frame_position == 2)
    {
      return m_pBottomFrame;
    }
  else
    return m_pTopFrame;
}

/*----------------------------------------------------------------------
 *       Class:  AmayaSplittablePage
 *      Method:  GetFramePosition
 * Description:  return the frame position if it exist in the current page
  -----------------------------------------------------------------------*/
int AmayaSplittablePage::GetFramePosition( const AmayaFrame * p_frame ) const
{
  if ( p_frame == GetFrame(1) )
    return 1;
  else if ( p_frame == GetFrame(2) )
    return 2;
  else
    return 0;
}

/*----------------------------------------------------------------------
 *       Class:  AmayaSplittablePage
 *      Method:  SetActiveFrame
 * Description:  
  -----------------------------------------------------------------------*/
void AmayaSplittablePage::SetActiveFrame( const AmayaFrame * p_frame )
{
  AmayaFrame *n_frame;
  Document    document;
  int         frame_id, old_active;

  old_active = m_ActiveFrame;
  if ( p_frame == GetFrame(1) )
    m_ActiveFrame = 1;
  else if ( p_frame == GetFrame(2) )
    m_ActiveFrame = 2;
  else
    m_ActiveFrame = 1;
 
  // update css list
  n_frame = GetFrame(1);
  if (n_frame)
    {
      frame_id = n_frame->GetFrameId();
      if (frame_id && ActiveFrame == 0)
        // after a unsplit the active frame is lost
        n_frame->SetActive(true);
      if (old_active != m_ActiveFrame)
        {
          document = FrameTable[frame_id].FrDoc;
          TtaExecuteMenuAction ("UpdateStyleList", document, 1, TRUE);
        }
    }
}

/*----------------------------------------------------------------------
 *       Class:  AmayaSplittablePage
 *      Method:  GetActiveFrame
 * Description:  
  -----------------------------------------------------------------------*/
AmayaFrame * AmayaSplittablePage::GetActiveFrame() const
{
  return GetFrame( m_ActiveFrame );
}


/*----------------------------------------------------------------------
 *       Class:  AmayaSplittablePage
 *      Method:  RefreshSplitToggleMenu()
 * Description:  is called to toggle on/off the "Show/Hide panel" menu item depeding on
 *               the panel showing state.
 *               Just forward the request to the parent window.
  -----------------------------------------------------------------------*/
void AmayaSplittablePage::RefreshShowPanelToggleMenu()
{
  wxASSERT(GetWindowParent());

  if (GetWindowParent())
    GetWindowParent()->RefreshShowToolPanelToggleMenu();
}


/*----------------------------------------------------------------------
 *  this is where the event table is declared
 *  the callbacks are assigned to an event type
 *----------------------------------------------------------------------*/
BEGIN_EVENT_TABLE(AmayaSplittablePage, AmayaPage)
  EVT_SPLITTER_SASH_POS_CHANGED( -1, 	AmayaSplittablePage::OnSplitterPosChanged )
  EVT_SPLITTER_DCLICK( -1, 		AmayaSplittablePage::OnSplitterDClick )
  EVT_SPLITTER_UNSPLIT( -1, 		AmayaSplittablePage::OnSplitterUnsplit )
  
  EVT_SIZE( 				AmayaSplittablePage::OnSize )
  EVT_CLOSE( 				AmayaSplittablePage::OnClose )

  EVT_BUTTON( -1,                       AmayaSplittablePage::OnSplitButton)

END_EVENT_TABLE()




#endif /* #ifdef _WX */ 
