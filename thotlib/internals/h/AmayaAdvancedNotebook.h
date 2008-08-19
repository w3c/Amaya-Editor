#ifdef _WX

#ifndef AMAYAADVANCEDNOTEBOOK_H_
#define AMAYAADVANCEDNOTEBOOK_H_


#include "wx/wx.h"
#include "wx/aui/auibook.h"
#include "wx/imaglist.h"

#include "AmayaPage.h"

class AmayaWindow;

/*
 *  Description:  - AmayaAdvancedNotebook is a AmayaPage container
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
 * |+[AmayaPanel]--+ +[AmayaAdvancedNotebook]-----------+ |
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

class AmayaAdvancedNotebook : public wxAuiNotebook, public AmayaPageContainer
{
 public:
  DECLARE_CLASS(AmayaAdvancedNotebook)

  AmayaAdvancedNotebook( wxWindow * window , wxWindowID id=wxID_ANY);
  virtual ~AmayaAdvancedNotebook( );
 
  int GetPageId( const AmayaPage * p_page );
  virtual AmayaWindow * GetAmayaWindow();
  
  void UpdatePageId();
  
  bool ClosePage(int page_id);
  bool CloseAllButPage(int position);

  void CleanUp();
  
  AmayaPage*  GetPage(size_t page)const {return wxDynamicCast(wxAuiNotebook::GetPage(page), AmayaPage);}
  size_t      GetPageCount() const{return wxAuiNotebook::GetPageCount();}
  
  int GetSelection()const {return wxAuiNotebook::GetSelection();}
  int SetSelection(size_t page) {return wxAuiNotebook::SetSelection(page);}

  bool SetPageText(size_t page, const wxString& text){return wxAuiNotebook::SetPageText(page, text);}
  bool SetPageImage(size_t page, int image);
  void SetImageList(wxImageList* imageList){m_imageList = imageList;}

  bool InsertPage(size_t index, AmayaPage* page, const wxString& text, bool select, int imageId);

  virtual operator wxWindow *(){return (wxWindow*)(wxAuiNotebook*)this;}
  
  virtual bool Layout(){return wxAuiNotebook::Layout();}

protected:
  DECLARE_EVENT_TABLE()
#ifdef __WXDEBUG__
  void OnPageChanging(wxAuiNotebookEvent& event);
#endif /* __WXDEBUG__ */
  
  void OnMouseRightDown(wxAuiNotebookEvent& event);
  
  void OnPageChanged(wxAuiNotebookEvent& event);
  void OnContextMenuItem( wxCommandEvent& event );
  
  void OnClose(wxCloseEvent& event);
  void OnClosePage(wxAuiNotebookEvent& event);

protected:
  wxImageList*   m_imageList;
};


#endif /*AMAYAADVANCEDNOTEBOOK_H_*/

#endif /* #ifdef _WX */

