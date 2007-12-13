#ifdef _WX

#ifndef AMAYAHELPWINDOW_H_
#define AMAYAHELPWINDOW_H_

#include "AmayaNormalWindow.h"
#include "wx/aui/aui.h"
#include "wx/hashmap.h"

#include "windowtypes_wx.h"

class AmayaAdvancedNotebook;

/*
 * =====================================================================================
 *        Class:  AmayaHelpWindow
 * 
 *  Description:  TopWindow which embed help pages
 *       Author:  Emilien KIA
 *      Created:  29/11/2007
 *     Revision:  
 * =====================================================================================
 */
class AmayaHelpWindow : public AmayaNormalWindow
{
 public:
  DECLARE_CLASS(AmayaHelpWindow)

AmayaHelpWindow (  wxWindow * parent, wxWindowID id=wxID_ANY
                      ,const wxPoint& pos  = wxDefaultPosition
                      ,const wxSize&  size = wxDefaultSize
                      ,int kind = WXAMAYAWINDOW_HELP);

  virtual ~AmayaHelpWindow();

  virtual bool Initialize();
  virtual void CleanUp();
  
  virtual void LoadConfig();
  virtual void SaveConfig();
  
  // Window decorations :
  virtual AmayaStatusBar * CreateStatusBar();
  virtual void             CreateMenuBar();
  
  virtual void SetPageIcon(int page_id, char *iconpath);

  
protected:
  DECLARE_EVENT_TABLE()

  virtual AmayaPageContainer* GetPageContainer();
  virtual const AmayaPageContainer* GetPageContainer()const;
  
  virtual AmayaPage *    CreatePage( Document doc, bool attach = false, int position = 0 );
  
  
  void OnClose(wxCloseEvent& event);

private:
  AmayaAdvancedNotebook* m_notebook;
};

#endif /*AMAYAHELPWINDOW_H_*/
#endif /* #ifdef _WX */
