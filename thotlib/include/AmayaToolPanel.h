#ifdef _WX

#ifndef AMAYATOOLPANEL_H_
#define AMAYATOOLPANEL_H_

#include "wx/wx.h"
#include "AmayaParams.h"
#include "paneltypes_wx.h"
#include "wx/panel.h"

class AmayaNormalWindow;
class AmayaToolPanelBar;

/**
 * New class for tool panel.
 */
class AmayaToolPanel : public wxPanel
{
  DECLARE_ABSTRACT_CLASS(AmayaToolPanel)
  DECLARE_EVENT_TABLE()
public:
  AmayaToolPanel();
  virtual ~AmayaToolPanel();
  
  virtual bool Create(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, 
            const wxSize& size = wxDefaultSize, long style = 0,
            const wxString& name = wxT("AmayaToolPanel"), wxObject* extra=NULL)=0;
  
  /** Panel title. */
  virtual wxString GetToolPanelName()const=0;
  /** Panel type code. */
  virtual int      GetToolPanelType()const;
  /** Panel state config key name. */
  virtual wxString GetToolPanelConfigKeyName()const;
  
  /** Return the default visibility state of panel.*/
  virtual bool GetDefaultVisibilityState()const{return false;}
  
  /** Return a default AUI config for the panel.*/
  virtual wxString GetDefaultAUIConfig() {return wxT("");}

  /** Return if the panel can be resized or not.*/
  virtual bool CanResize() {return true;}
  
  bool IsExpanded() {return m_bExpanded;}
  bool IsFloating() {return m_bFloating;}
  bool IsVisible()  {return m_bVisible;}

  void SetExpandedFlag(bool bExpanded) {m_bExpanded = bExpanded;}
  void SetFloatingFlag(bool bFloating) {m_bFloating = bFloating;}
  void SetVisibleFlag(bool bVisible) {m_bVisible = bVisible;}
  
  /**
   * Setup a flag used to force DoUpdate call when the
   * sub panel is ready to be updated (when it is visible).
   */
  void ShouldBeUpdated( bool should_update = true );

  /**
   * Refresh the button widgets of the frame's panel,
   * should be override in inherited class
   */
  virtual void SendDataToPanel( AmayaParams& params );
  virtual void SetColor(int color);

  /**
   * Retrieve the window to which the panel is attached.
   */
  AmayaNormalWindow* GetWindow(){return m_window;}
  const AmayaNormalWindow* GetWindow()const{return m_window;}
  
  /**
   * Set the window to which the panel is attached.
   */
  void SetWindow(AmayaNormalWindow* window){m_window = window;}
  
  /**
   * Force the layout update.
   */
  bool Layout();
  
protected:
  /**
   * This method is called when the sub-panel is ready to be updated
   * it should be redefined into inherited classes but do not forget to call
   * AmayaToolPanel::DoUpdate() in order to update the flags. */
  virtual void DoUpdate();
  
private:
  AmayaNormalWindow* m_window;
  AmayaToolPanelBar* m_bar;
  
  bool m_ShouldBeUpdated;
  bool m_bExpanded, m_bFloating, m_bVisible;
};


/**
 * Function to register a new AmayaToolPanel class.
 */
void RegisterToolPanelClass(wxClassInfo* ci);


#endif /*AMAYATOOLPANEL_H_*/

#endif /* _WX */
