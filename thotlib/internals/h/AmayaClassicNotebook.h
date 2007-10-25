#ifdef _WX

#ifndef AMAYACLASSICNOTEBOOK_H_
#define AMAYACLASSICNOTEBOOK_H_

#include "wx/wx.h"
#include "wx/notebook.h"

#include "AmayaPage.h"

class AmayaWindow;

/*
 *  Description:  - AmayaClassicNotebook is a AmayaPage container
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
 * |+[AmayaPanel]--+ +[AmayaClassicNotebook]-------------+|
 * ||              | |+-----------+                      ||
 * ||              | ||[AmayaPage]+---------------------+||
 * ||              | ||+-------------------------------+|||
 * ||              | |||[AmayaFrame]                   ||||
 * ||              | |||                               ||||
 * ||              | |||                               ||||
 * ||              | |||  (view container)     'Top'   ||||
 * ||              | |||---------SplitBar--------------||||
 * ||              | |||[AmayaFrame]          'Bottom' ||||
 * ||              | |||                               ||||
 * ||              | |||                               ||||
 * ||              | ||+-------------------------------+|||
 * ||              | |+---------------------------------+||
 * |+--------------+ +-----------------------------------+|
 * |+----------------------------------------------------+|
 * || StatusBar                                          ||
 * |+----------------------------------------------------+|
 * +------------------------------------------------------+
 *       Author:  Stephane GULLY
 *      Created:  12/10/2003 04:45:34 PM CET
 *     Revision:  none
*/

class AmayaClassicNotebook : public wxNotebook, public AmayaPageContainer
{
 public:
  DECLARE_CLASS(AmayaClassicNotebook)

  AmayaClassicNotebook( wxWindow * window , wxWindowID id=wxID_ANY);
  virtual ~AmayaClassicNotebook( );
 
  int GetPageId( const AmayaPage * p_page );
  virtual AmayaWindow * GetAmayaWindow();
  
  void UpdatePageId();
  
  bool ClosePage(int page_id);
  bool CloseAllButPage(int position);

  void CleanUp();
  
  size_t     GetPageCount() const{return wxNotebook::GetPageCount();}
  AmayaPage* GetPage(size_t page)const {return wxDynamicCast(wxNotebook::GetPage(page), AmayaPage);}
  virtual int GetSelection()const{return wxNotebook::GetSelection();}

  int SetSelection(size_t page) {return wxNotebook::SetSelection(page);}

  bool SetPageText(size_t page, const wxString& text){return wxNotebook::SetPageText(page, text);}
  bool SetPageImage(size_t page, int image){return wxNotebook::SetPageImage(page, image);}
  void SetImageList(wxImageList* imageList){wxNotebook::SetImageList(imageList);}

  bool InsertPage(size_t index, AmayaPage* page, const wxString& text, bool select = false, int imageId = -1);

  virtual operator wxWindow *(){return (wxWindow*)(wxNotebook*)this;}

  virtual bool Layout(){ return wxNotebook::Layout();}
  
protected:
  DECLARE_EVENT_TABLE()
#ifdef __WXDEBUG__
  void OnPageChanging(wxNotebookEvent& event);
#endif /* __WXDEBUG__ */
  void OnPageChanged(wxNotebookEvent& event);
  void OnContextMenu( wxContextMenuEvent & event );
  void OnContextMenuItem( wxCommandEvent& event );

  void OnMouseMiddleUp(wxMouseEvent& event);
  
  void OnClose(wxCloseEvent& event);
};

#endif // AMAYACLASSICNOTEBOOK_H_

#endif /* #ifdef _WX */


