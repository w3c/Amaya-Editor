#ifdef _WX

#ifndef __AMAYAATTRIBUTEPANEL_H__
#define __AMAYAATTRIBUTEPANEL_H__

#include "wx/wx.h"
#include "AmayaSubPanel.h"

class AmayaNormalWindow;

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

  /*  void UpdateAttributeList( const char * p_attr_list, int nb_attr, const int * p_active_attr,
      const char * p_attr_evt_list, int nb_attr_evt, const int * p_active_attr_evt  );*/

  bool IsFreezed();
  void SelectAttribute( int position );

 protected:
  // Any class wishing to process wxWindows events must use this macro
  DECLARE_EVENT_TABLE()
  void OnListSelectItem( wxCommandEvent& event );
  void OnListCheckItem( wxCommandEvent& event );
  void OnApply( wxCommandEvent& event );
  void OnCancel( wxCommandEvent& event );
  void OnAutoRefresh( wxCommandEvent& event );
  void OnDelAttr( wxCommandEvent& event );

 protected:
  virtual void SendDataToPanel( AmayaParams& params );
  virtual void DoUpdate();

  void CreateCurrentAttribute();
  void RemoveCurrentAttribute();

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
  void SetupListValue( const char * p_attr_list, int nb_attr, const int * p_active_attr,
		       const char * p_attr_evt_list, int nb_attr_evt, const int * p_active_attr_evt );
  void SetupLangValue( const char * selected_lang, 
		       const char * inherited_lang,
		       const char * lang_list,
		       int lang_list_nb,
		       int default_lang_id );
  void SetupTextValue( const char * text );
  void SetupEnumValue( const char * enums, int nb_enum, int selected );
  void SetupNumValue( int num );

  void SetMandatoryState( bool is_mandatory );

 protected:
  wxPanel *           m_pVPanelParent;
  wxSizer *           m_pVPanelSizer;
  wxCheckListBox *    m_pAttrList;
  wxCheckBox *        m_pAutoRefresh;
  wxPanel *           m_pPanel_ApplyArea;
  wxPanel *           m_pPanel_Lang;
  wxPanel *           m_pPanel_Num;
  wxPanel *           m_pPanel_Text;
  wxPanel *           m_pPanel_Enum;
  wxRadioBox *        m_pRBEnum;
 
 
  int m_NbAttr;
  int m_NbAttr_evt;

  wxATTR_TYPE m_CurrentAttType;
  bool        m_CurrentAttMandatory;
};

#endif // __AMAYAATTRIBUTEPANEL_H__

#endif /* #ifdef _WX */
