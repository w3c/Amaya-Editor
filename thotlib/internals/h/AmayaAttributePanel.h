#ifdef _WX

#ifndef __AMAYAATTRIBUTEPANEL_H__
#define __AMAYAATTRIBUTEPANEL_H__

#include "wx/wx.h"
#include "AmayaSubPanel.h"
#include "wx/listctrl.h"

#include "containers.h"

class AmayaNormalWindow;
typedef struct _ElementDescr *PtrElement;
typedef struct AttrListElem* PtrAttrListElem;

/*
 *  Description:  - AmayaAttributePanel is a specific sub-panel
 *       Author:  Stephane GULLY
 *      Created:  13/09/2004 04:45:34 PM CET
 *     Revision:  none
*/

class AmayaAttributePanel : public AmayaSubPanel
{
public:
  DECLARE_DYNAMIC_CLASS(AmayaAttributePanel)

  AmayaAttributePanel( wxWindow * p_parent_window = NULL
             ,AmayaNormalWindow * p_parent_nwindow = NULL );
  virtual ~AmayaAttributePanel();

  virtual bool IsActive();
  virtual int GetPanelType();

  void ForceAttributeUpdate();

  void SelectAttribute( int position);
  
  wxString GetCurrentSelectedAttrName()const;

 protected:
  // Any class wishing to process wxWindows events must use this macro
  DECLARE_EVENT_TABLE()
  
  void OnListItemSelected(wxListEvent& event);
  void OnListItemDeselected(wxListEvent& event);
  
  void OnApply( wxCommandEvent& event );
  void OnCancel( wxCommandEvent& event );
  void OnDelAttr( wxCommandEvent& event );
  void OnInsert( wxCommandEvent& event );

  virtual void SendDataToPanel( AmayaParams& params );
  virtual void DoUpdate();

  void CreateCurrentAttribute();
  void RemoveCurrentAttribute();
  void QueryRemoveCurrentAttribute();
  
  void ModifyListAttrValue(const wxString& attrName, const wxString& attrVal);

  void RedirectFocusToEditableControl();
  void UpdateListColumnWidth();
 public:
  typedef enum
    {
      wxATTR_TYPE_NONE,
      wxATTR_TYPE_TEXT,
      wxATTR_TYPE_ENUM,
      wxATTR_TYPE_LANG,
      wxATTR_TYPE_NUM
    } wxATTR_TYPE;

  typedef enum
    {
      wxATTR_ACTION_UNKNOWN,
      wxATTR_ACTION_LISTUPDATE,
      wxATTR_ACTION_SETUPLANG,
      wxATTR_ACTION_SETUPTEXT,
      wxATTR_ACTION_SETUPENUM,
      wxATTR_ACTION_SETUPNUM
    } wxATTR_ACTION;

 protected:
  void ShowAttributValue( wxATTR_TYPE type );
  
  void SetupListValue(DLList attrList);

  void SetupLangAttr(PtrAttrListElem elem);
  
  void SetupTextValue( const char * text );
  void SetupTextAttr(PtrAttrListElem elem);
  
  void SetupEnumValue( wxArrayString& enums, int selected );
  void SetupEnumAttr(PtrAttrListElem elem);

  void SetupNumValue( int num, int begin, int end );
  void SetupNumAttr(PtrAttrListElem elem);

  bool IsMandatory()const;

  void DesactivatePanel(){m_disactiveCount++;}
  void ActivePanel(){m_disactiveCount--;if(m_disactiveCount<0)m_disactiveCount=0;}
  bool IsPanelActive()const{return m_disactiveCount==0;}

  void OnUpdateDeleteButton(wxUpdateUIEvent& event);
protected:
  wxPanel *           m_pVPanelParent;
  wxSizer *           m_pVPanelSizer;
  wxListCtrl *        m_pAttrList;
  wxPanel *           m_pPanel_Lang;
  wxPanel *           m_pPanel_Num;
  wxPanel *           m_pPanel_Text;
  wxPanel *           m_pPanel_Enum;
  wxPanel *           m_pPanel_NewAttr;
  wxChoice *          m_pChoiceEnum;
  wxChoice *          m_pNewAttrChoice;
  
  DLList              m_attrList;
  PtrAttrListElem     m_currentAttElem;
  PtrElement          m_firstSel, m_lastSel;
  int                 m_firstChar, m_lastChar;

  int m_NbAttr;
  int m_NbAttr_evt;

  wxATTR_TYPE m_CurrentAttType;
  wxWindow*   m_pCurrentlyEditedControl;
  
  int        m_disactiveCount; // 0 to activate panel (handle events)
};

#endif // __AMAYAATTRIBUTEPANEL_H__

#endif /* #ifdef _WX */
