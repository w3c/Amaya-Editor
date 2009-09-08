#ifdef _WX

#ifndef __AMAYAPAGE_H__
#define __AMAYAPAGE_H__

#include "wx/wx.h"
#include "wx/panel.h"
#include "wx/splitter.h"
#include "wx/imaglist.h"


class AmayaFrame;
class AmayaClassicNotebook;
class AmayaQuickSplitButton;
class AmayaWindow;

class AmayaPageContainer;

// there is a maximum of 2 frames into a page
#define MAX_MULTI_FRAME 2

/*
 *  Description:  - a page contains 1 or 2 frames which are splited
 *                into a wxSplitterWindow
 *                - the m_SlashRatio is used to control the percentage givent to the
 *                top and bottom areas.
 *  
 *                  + A window can contains several documents.
 *                  + Each document is placed into a page.
 *                  + A page is a "tab" placed in
 *                  a "notebook" widget.
 *                  + A page can be splited in 2 parts, each parts
 *                  represent a document's view (AmayaFrame).
 *                  + A frame is a OpenGL canvas with its own
 *                  scrollbars.
 * 
 * +[AmayaWindow]-----------------------------------------+
 * |+----------------------------------------------------+|
 * || MenuBar                                            ||
 * |+----------------------------------------------------+|
 * |+----------------------------------------------------+|
 * || ToolBar                                            ||
 * |+----------------------------------------------------+|
 * |+[AmayaPanel]--+ +[AmayaNoteBook]-------------------+ |
 * ||              | |+-----------+                     | |
 * ||              | ||[AmayaPage]+--------------------+| |
 * ||              | ||+------------------------------+|| |
 * ||              | |||[AmayaFrame]                  ||| |
 * ||              | |||                              ||| |
 * ||              | |||                              ||| |
 * ||              | |||  (view container)     'Top'  ||| |
 * ||              | |||---------SplitBar-------------||| |
 * ||              | |||[AmayaFrame]          'Bottom'||| |
 * ||              | |||                              ||| |
 * ||              | |||                              ||| |
 * ||              | ||+------------------------------+|| |
 * ||              | |+--------------------------------+| |
 * |+--------------+ +----------------------------------+ |
 * |+----------------------------------------------------+|
 * || StatusBar                                          ||
 * |+----------------------------------------------------+|
 * +------------------------------------------------------+
 *       Author:  Stephane GULLY
 *      Created:  12/10/2003 04:45:34 PM CET
 *     Revision:  none
*/

/**
 * AmayaPage is the base class for pages.
 */
class AmayaPage : public wxPanel
{
  DECLARE_ABSTRACT_CLASS(AmayaPage)
  DECLARE_EVENT_TABLE()
public:
  static AmayaPage *CreateAmayaPage(wxWindow *parent, AmayaWindow *window, Document doc);
  
  AmayaPage(wxWindow *p_parent_window = NULL, AmayaWindow *p_amaya_parent_window = NULL );
  virtual ~AmayaPage();

  virtual AmayaWindow *GetWindowParent();

  virtual AmayaFrame *AttachFrame( AmayaFrame *p_frame, int position, int split ) {return NULL;}
  virtual AmayaFrame *DetachFrame( int position ) {return NULL;}

  virtual void        SetActiveFrame( const AmayaFrame *p_frame ) {}
  virtual AmayaFrame *GetActiveFrame() const {return NULL;}

  virtual void SetContainer( AmayaPageContainer *container ){m_pContainer = container;} 
  virtual AmayaPageContainer *GetContainer() {return m_pContainer;}
  
  virtual void SetPageId( int page_id );
  virtual int  GetPageId()const{return m_PageId;}

  virtual bool IsClosed()const{return m_IsClosed;}
  virtual void SetIsClosed(bool closed){m_IsClosed=closed;}
  
  virtual bool IsSelected(){return m_IsSelected;}
  virtual void SetSelected( bool isSelected );
  
  virtual bool CleanUp() =0;

  virtual void RaisePage();
  virtual int GetMasterFrameId()const{return m_MasterFrameId;}
  
  
  virtual AmayaFrame *GetFrame( int frame_position ) const=0;
  virtual int GetFramePosition( const AmayaFrame * p_frame ) const=0;

protected:
  void SetMasterFrameId(int id){m_MasterFrameId = id;}
  
private:
  AmayaPageContainer *m_pContainer;
  int                 m_PageId;
  bool                m_IsClosed;
  bool                m_IsSelected;
  int                 m_MasterFrameId;
};


/**
 * AmayaSimplePage is a page which can only be wiewed (no split). 
 */
class AmayaSimplePage : public AmayaPage
{
public:
  DECLARE_DYNAMIC_CLASS(AmayaSimplePage)

  AmayaSimplePage( wxWindow * p_parent_window = NULL, AmayaWindow * p_amaya_parent_window = NULL );
  virtual ~AmayaSimplePage();

  AmayaFrame * AttachFrame( AmayaFrame * p_frame, int position, int split );
  AmayaFrame * DetachFrame( int position );

  void         SetActiveFrame( const AmayaFrame * p_frame ){/* Do nothing, always one frame always active.*/}
  AmayaFrame * GetActiveFrame() const{return m_pFrame; /* always one frame always active.*/}
  
  bool CleanUp();
  AmayaFrame * GetFrame(int WXUNUSED(frame_position))const{return m_pFrame;}
  int GetFramePosition( const AmayaFrame * p_frame ) const;
private:
  DECLARE_EVENT_TABLE()
  void OnClose(wxCloseEvent& event);
  AmayaFrame *m_pFrame;
};




/**
 * AmayaSplittablePage is the page class which can be
 * splitted horizontally or vertically.
 */ 
class AmayaSplittablePage : public AmayaPage
{
public:
  DECLARE_DYNAMIC_CLASS(AmayaSplittablePage)

  AmayaSplittablePage( wxWindow * p_parent_window = NULL, AmayaWindow * p_amaya_parent_window = NULL );
  virtual ~AmayaSplittablePage();

  AmayaFrame * AttachFrame( AmayaFrame * p_frame, int position, int split );
  AmayaFrame * DetachFrame( int position );

  void SetSplitMode( int mode );
  void DoRightSplitButtonAction();
  void DoBottomSplitButtonAction();
  void DoSplitUnsplit(int mode);
  void DoSwitchHoriVert();

  AmayaQuickSplitButton * GetQuickSplitButton (ThotBool horizontal);

  void         SetActiveFrame( const AmayaFrame * p_frame );
  AmayaFrame * GetActiveFrame() const;

  AmayaFrame * GetFrame( int frame_position ) const;
  int GetFramePosition( const AmayaFrame * p_frame ) const;

  wxSplitterWindow * GetSplitterWindow();

  bool CleanUp();

 protected:
  void AdjustSplitterPos( int height = -1, int width = -1 );
  void RefreshShowPanelToggleMenu();

  DECLARE_EVENT_TABLE()
  void OnSplitterPosChanged( wxSplitterEvent& event );
  void OnSplitterDClick( wxSplitterEvent& event );
  void OnSplitterUnsplit( wxSplitterEvent& event );
  void OnSplitButton( wxCommandEvent& event );

  void OnSize( wxSizeEvent& event );

  void OnClose(wxCloseEvent& event);
  
  wxSplitterWindow *      m_pSplitterWindow;
  AmayaQuickSplitButton * m_pSplitButtonBottom; // button used to quickly split the frame horizontaly
  AmayaQuickSplitButton * m_pSplitButtonRight; // button used to quickly split the frame verticaly
  wxPanel *               m_DummyPanel;
  AmayaFrame *            m_pTopFrame;
  AmayaFrame *            m_pBottomFrame;
  AmayaPageContainer *    m_pContainer;
  AmayaWindow *           m_pWindowParent;
  
  float              m_SlashRatio; // 0.5 => page is half splitted
  int                m_ActiveFrame; // 1 first frame / 2 second frame

  // these 2 atributes are used to remember the type of views on each frames
  wxString           m_FirstViewType;
  wxString           m_SecondViewType;
  char               m_LastOpenViewName[50];
};

/**
 * Base AmayaPageContainer.
 * Do not use directly, use AmayaClassicNotebook or AmayaAdvancedNotebook instead.
 **/
class AmayaPageContainer
{
public:
  virtual ~AmayaPageContainer(){}
  
  virtual int GetPageId( const AmayaPage * p_page ) = 0;
  virtual AmayaWindow * GetAmayaWindow() = 0;
  
  virtual void UpdatePageId() = 0;
  
  virtual bool ClosePage(int page_id) = 0;
  virtual bool CloseAllButPage(int position) = 0;

  virtual AmayaPage* GetPage(size_t WXUNUSED(page))const {return NULL;}
  virtual size_t     GetPageCount() const{return 0;}
  virtual int GetSelection()const{return wxID_ANY;}
  virtual int SetSelection(size_t WXUNUSED(page)) {return wxID_ANY;}
  
  virtual bool SetPageText(size_t WXUNUSED(page), const wxString& WXUNUSED(text)){return false;}
  virtual bool SetPageImage(size_t WXUNUSED(page), int WXUNUSED(image)){return false;}
  virtual void SetImageList(wxImageList* WXUNUSED(imageList)){}
  
  virtual bool InsertPage(size_t index, AmayaPage* page, const wxString& text, bool select = false, int imageId = -1)=0;
  
  virtual void CleanUp() = 0;
  
  virtual bool Layout()=0;

  virtual int GetMContextFrame()const{return m_MContextFrameId;}
  
  virtual operator wxWindow *() = 0;
protected:
  int            m_MContextFrameId;
};

#endif // __AMAYAPAGE_H__

#endif /* #ifdef _WX */
