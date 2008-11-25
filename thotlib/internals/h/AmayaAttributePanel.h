#ifdef _WX

#ifndef __AMAYAATTRIBUTEPANEL_H__
#define __AMAYAATTRIBUTEPANEL_H__

#include "wx/wx.h"
#include "AmayaToolPanel.h"
#include "wx/listctrl.h"
#include "wx/spinctrl.h"

#include "containers.h"
#include "interface.h"

class AmayaNormalWindow;
class AmayaAttributeSubpanel;
typedef struct _ElementDescr *PtrElement;
typedef struct AttrListElem* PtrAttrListElem;

/*
 *  Description:  - AmayaAttributePanel is a specific sub-panel
 *       Author:  Stephane GULLY
 *      Created:  13/09/2004 04:45:34 PM CET
 *     Revision:  none
*/
class AmayaAttributeToolPanel : public AmayaToolPanel
{
  DECLARE_DYNAMIC_CLASS(AmayaAttributeToolPanel)
public:
  /**
   * Panel identificators.
   */
  typedef enum
    {
      wxATTR_PANEID_NONE    = wxID_ANY,
      wxATTR_PANEID_ENUM    = 0,
      wxATTR_PANEID_TEXT,
      wxATTR_PANEID_NUM,
      wxATTR_PANEID_LANG,
      wxATTR_PANEID_MAX
    }wxATTR_PANEID;

  /**
   * Internal type deducts from attribute type and attribute num.
   */
  typedef enum
  {
    wxATTR_INTTYPE_NONE = -1,
    wxATTR_INTTYPE_NUM  = 0,  /* = AtNumAttr */
    wxATTR_INTTYPE_TEXT, /* = AtTextAttr */
    wxATTR_INTTYPE_REF,  /* = AtReferenceAttr*/
    wxATTR_INTTYPE_ENUM, /* = AtEnumAttr */
    wxATTR_INTTYPE_LANG, /* = AtTextAttr && num==1 */
    wxATTR_INTTYPE_MAX
  }wxATTR_INTTYPE;
    
  typedef enum
    {
      wxATTR_ACTION_UNKNOWN,
      wxATTR_ACTION_LISTUPDATE,
      wxATTR_ACTION_SETUPLANG,
      wxATTR_ACTION_SETUPTEXT,
      wxATTR_ACTION_SETUPENUM,
      wxATTR_ACTION_SETUPNUM
    } wxATTR_ACTION;

  AmayaAttributeToolPanel();
  virtual ~AmayaAttributeToolPanel();
  
  virtual bool Create(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, 
            const wxSize& size = wxDefaultSize, long style = 0,
            const wxString& name = wxT("AmayaAttributeToolPanel"), wxObject* extra=NULL);
  
  virtual wxString GetToolPanelName()const;
  virtual int      GetToolPanelType()const{return WXAMAYA_PANEL_ATTRIBUTE;}
  virtual wxString GetToolPanelConfigKeyName()const{return wxT("PANEL_ATTRIBUTE");}

  /** Return a default AUI config for the panel.*/
  virtual wxString GetDefaultAUIConfig();

  
  void ForceAttributeUpdate();

  void SelectAttribute( int position);
  
  wxString GetCurrentSelectedAttrName()const;

protected:
  // Any class wishing to process wxWindows events must use this macro
  DECLARE_EVENT_TABLE()

  /**
   * Analyse elem param to find correct internal type.
   */
  static wxATTR_INTTYPE GetInternalTypeFromAttrElem(PtrAttrListElem elem);
  
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

  void ShowAttributValue( wxATTR_PANEID type );
  void ShowAttributeBar(bool bShow);
  
  void SetupListValue(DLList attrList);

  void SetupAttr(PtrAttrListElem elem, wxATTR_PANEID type);
  
  bool IsMandatory()const;
  bool IsReadOnly()const;

  void DesactivatePanel(){m_disactiveCount++;}
  void ActivePanel(){m_disactiveCount--;if(m_disactiveCount<0)m_disactiveCount=0;}
  bool IsPanelActive()const{return m_disactiveCount==0;}

  void OnUpdateDeleteButton(wxUpdateUIEvent& event);

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
  int                 m_NbAttr, m_NbAttr_evt;
  int                 m_sz0, m_sz1;
  wxATTR_PANEID       m_currentPane;
  wxWindow           *m_pCurrentlyEditedControl;
  
  wxSizer            *m_pSubpanelSizer;
  AmayaAttributeSubpanel* m_subpanels[wxATTR_PANEID_MAX];
  
  static wxString      s_subpanelClassNames[wxATTR_PANEID_MAX];
  static wxATTR_PANEID s_subpanelAssoc[restr_content_max][wxATTR_INTTYPE_MAX];
  
  int        m_disactiveCount; // 0 to activate panel (handle events)
};



/*
 * AmayaAttributeSubpanel
 * Base class for all attribute subpanel.
 */
class AmayaAttributeSubpanel : public wxPanel
{
  DECLARE_ABSTRACT_CLASS(AmayaAttributeSubpanel)
  DECLARE_EVENT_TABLE()
public:
  AmayaAttributeSubpanel();
  virtual ~AmayaAttributeSubpanel();
  
  virtual bool Create(wxWindow* parent, wxWindowID id){return true;}

  /**
   * Set the new attribute element description.
   * Used to fill the panel before showing it.
   */
  virtual bool SetAttrListElem(PtrAttrListElem elem){return false;}
  
  /**
   * Set the selection position.
   */
  bool SetSelectionPosition(PtrElement firstSel, PtrElement lastSel,
                                    int firstChar, int lastChar);

  
  /**
   * Get the string value entered by the user.
   */
  virtual wxString GetStringValue(){return wxT("");}
  /**
   * Get the int/enum value entered by the user.
   */
  virtual intptr_t GetIntValue(){return 0;}
  /**
   * Helper function to send OK button event to parent.
   */
  void SendApplyInfoToParent(wxCommandEvent& event);
  /**
   * Retrieve the address of the edition control.
   */
  virtual wxWindow* GetEditionControl(){return NULL;}
  
  
  static wxString getAttributeStringValue(PtrAttrListElem elem);
  static int      getAttributeNumericValue(PtrAttrListElem elem);
  static void     getAttributeEnumValues(PtrAttrListElem elem, wxArrayString& arr);
protected:
  PtrElement          m_firstSel, m_lastSel;
  int                 m_firstChar, m_lastChar;

};


/**
 * AmayaEnumAttributeSubpanel
 * Attribute subpanel for enum values.
 */
class AmayaEnumAttributeSubpanel : public AmayaAttributeSubpanel
{
  DECLARE_DYNAMIC_CLASS(AmayaEnumAttributeSubpanel)
  DECLARE_EVENT_TABLE()
public:
  AmayaEnumAttributeSubpanel();
  virtual ~AmayaEnumAttributeSubpanel();
  virtual bool Create(wxWindow* parent, wxWindowID id);
  
  /**
   * Set the new attribute element description.
   * Used to fill the panel before showing it.
   */
  virtual bool SetAttrListElem(PtrAttrListElem elem);
  
  /**
   * Get the string value entered by the user.
   */
  virtual wxString GetStringValue();
  /**
   * Get the int/enum value entered by the user.
   */
  virtual intptr_t GetIntValue();
  
  virtual wxWindow* GetEditionControl(){return m_pChoice;}  
private:
  wxChoice* m_pChoice;
  AmayaAttributeToolPanel::wxATTR_INTTYPE m_type;
};


/**
 * AmayaStringAttributeSubpanel
 * Attribute subpanel for string values.
 * Can only use AtTextAttr non enumerated attributes.
 * If (PtrAttrListElem)elem.restr.RestrEnumVal is non-null : it is used as proposed 
 * (but not imposed) values (tab or EOL separated string.
 */
class AmayaStringAttributeSubpanel : public AmayaAttributeSubpanel
{
  DECLARE_DYNAMIC_CLASS(AmayaStringAttributeSubpanel)
  DECLARE_EVENT_TABLE()
public:
  AmayaStringAttributeSubpanel();
  virtual ~AmayaStringAttributeSubpanel();
  virtual bool Create(wxWindow* parent, wxWindowID id);
  
  /**
   * Set the new attribute element description.
   * Used to fill the panel before showing it.
   */
  virtual bool SetAttrListElem(PtrAttrListElem elem);
  
  /**
   * Get the string value entered by the user.
   */
  virtual wxString GetStringValue();
  virtual wxWindow* GetEditionControl(){return m_hasDefaults?(wxWindow*)m_pCombo:(wxWindow*)m_pText;}  
private:
  bool m_hasDefaults;
  wxTextCtrl* m_pText;
  wxComboBox* m_pCombo;
};



/**
 * AmayaNumAttributeSubpanel
 * Attribute subpanel for integer values.
 * Can use AtTextAttr or AtNumAttr non enumerated attributes.
 */
class AmayaNumAttributeSubpanel : public AmayaAttributeSubpanel
{
  DECLARE_DYNAMIC_CLASS(AmayaNumAttributeSubpanel)
  DECLARE_EVENT_TABLE()
public:
  AmayaNumAttributeSubpanel();
  virtual ~AmayaNumAttributeSubpanel();
  virtual bool Create(wxWindow* parent, wxWindowID id);
  
  /**
   * Set the new attribute element description.
   * Used to fill the panel before showing it.
   */
  virtual bool SetAttrListElem(PtrAttrListElem elem);
  
  /**
   * Get the int/enum value entered by the user.
   */
  virtual intptr_t GetIntValue();
  /**
   * Get the string value entered by the user.
   */
  virtual wxString GetStringValue();
  virtual wxWindow* GetEditionControl(){return m_pSpin;}  
private:
  wxSpinCtrl* m_pSpin;
};


/**
 * AmayaLangAttributeSubpanel
 * Attribute subpanel for language values.
 */
class AmayaLangAttributeSubpanel : public AmayaAttributeSubpanel
{
  DECLARE_DYNAMIC_CLASS(AmayaLangAttributeSubpanel)
  DECLARE_EVENT_TABLE()
public:
  AmayaLangAttributeSubpanel();
  virtual ~AmayaLangAttributeSubpanel();
  virtual bool Create(wxWindow* parent, wxWindowID id);
  
  /**
   * Set the new attribute element description.
   * Used to fill the panel before showing it.
   */
  virtual bool SetAttrListElem(PtrAttrListElem elem);
  
  /**
   * Get the string value entered by the user.
   */
  virtual wxString GetStringValue();
  /**
   * Get the int/enum value entered by the user.
   */
  virtual intptr_t GetIntValue();

  virtual wxWindow* GetEditionControl(){return m_pCombo;}  
private:
  
  static void Initialize();
  
  wxChoice     *m_pCombo;
  wxStaticText *m_pText;
  wxArrayString m_arrCodes;
  
  static wxArrayString s_arrLangs;
};

#endif // __AMAYAATTRIBUTEPANEL_H__

#endif /* #ifdef _WX */
