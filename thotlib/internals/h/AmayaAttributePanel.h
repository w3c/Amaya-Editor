#ifdef _WX

#ifndef __AMAYAATTRIBUTEPANEL_H__
#define __AMAYAATTRIBUTEPANEL_H__

#include "wx/wx.h"
#include "AmayaSubPanel.h"
#include "wx/listctrl.h"
#include "wx/spinctrl.h"

#include "containers.h"

class AmayaNormalWindow;
class AmayaAttributeSubpanel;
typedef struct _ElementDescr *PtrElement;
typedef struct AttrListElem* PtrAttrListElem;

#define AmayaAttributeSubpanelNumber  4 

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
      wxATTR_TYPE_NONE = -1,
      wxATTR_TYPE_ENUM = 0,
      wxATTR_TYPE_TEXT,
      wxATTR_TYPE_NUM,
      wxATTR_TYPE_LANG,
      wxATTR_TYPE_MAX
    } wxATTR_TYPE;

  typedef enum
  {
    wxATTR_INTTYPE_NUM,  /* = AtNumAttr */
    wxATTR_INTTYPE_TEXT, /* = AtTextAttr */
    wxATTR_INTTYPE_REF,  /* = AtReferenceAttr*/
    wxATTR_INTTYPE_ENUM, /* = AtEnumAttr */
    wxATTR_INTTYPE_LANG,
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

 protected:
  void ShowAttributValue( wxATTR_TYPE type );
  
  void SetupListValue(DLList attrList);

  void SetupAttr(PtrAttrListElem elem, wxATTR_TYPE);
  
  bool IsMandatory()const;
  bool IsReadOnly()const;

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
  
  wxSizer *               m_pSubpanelSizer;
  AmayaAttributeSubpanel* m_subpanels[AmayaAttributeSubpanelNumber];
  static wxString s_subpanelClassNames[AmayaAttributeSubpanelNumber];
  
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
  virtual int GetIntValue(){return 0;}
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
  virtual int GetIntValue();
  
  virtual wxWindow* GetEditionControl(){return m_pChoice;}  
private:
  wxChoice* m_pChoice;
  AmayaAttributePanel::wxATTR_INTTYPE m_type;
};


/**
 * AmayaStringAttributeSubpanel
 * Attribute subpanel for string values.
 * Can only use AtTextAttr non enumerated attributes.
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
  virtual wxWindow* GetEditionControl(){return m_pText;}  
private:
  wxTextCtrl* m_pText;
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
  virtual int GetIntValue();
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
  virtual int GetIntValue();

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
