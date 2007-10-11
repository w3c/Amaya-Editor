#ifdef _WX

#include "wx/wx.h"
#include "wx/xrc/xmlres.h"              // XRC XML resouces

#include "thot_gui.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "appdialogue.h"
#include "dialog.h"
#include "selection.h"
#include "application.h"
#include "dialog.h"
#include "document.h"
#include "message.h"
#include "libmsg.h"
#include "frame.h"
#include "message_wx.h"
#include "paneltypes_wx.h"
#include "appdialogue_wx.h"
#include "appdialogue_wx_f.h"
#include "panel.h"
#include "registry_wx.h"

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "frame_tv.h"
#include "paneltypes_wx.h"
#include "colors_f.h"
#include "containers.h"
#include "inites_f.h"
#include "presentmenu_f.h"

#include "Elemlist.h"

#include "AmayaXMLPanel.h"
#include "AmayaNormalWindow.h"


//
//
// AmayaXMLToolPanel
//
//

IMPLEMENT_DYNAMIC_CLASS(AmayaXMLToolPanel, AmayaToolPanel)

AmayaXMLToolPanel::AmayaXMLToolPanel():
  AmayaToolPanel()
,m_XMLRef(0)
,m_fnCallback(NULL)
{
}

AmayaXMLToolPanel::~AmayaXMLToolPanel()
{
}

bool AmayaXMLToolPanel::Create(wxWindow* parent, wxWindowID id, const wxPoint& pos, 
          const wxSize& size, long style, const wxString& name, wxObject* extra)
{
  if(!wxXmlResource::Get()->LoadPanel((wxPanel*)this, parent, wxT("wxID_TOOLPANEL_XML")))
    return false;
  
  m_pXMLList = XRCCTRL(*this,"wxID_LIST_XML",wxListBox);
  XRCCTRL(*this,"wxID_REFRESH",wxBitmapButton)->SetToolTip(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_REFRESH)));
  XRCCTRL(*this,"wxID_APPLY",wxBitmapButton)->SetToolTip(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_APPLY)));
  return true;
}

wxString AmayaXMLToolPanel::GetToolPanelName()const
{
  return TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_EL_TYPE));
}

/*----------------------------------------------------------------------
 *       Class:  AmayaXMLToolPanel
 *      Method:  GetDefaultAUIConfig
 * Description:  Return a default AUI config for the panel.
 -----------------------------------------------------------------------*/
wxString AmayaXMLToolPanel::GetDefaultAUIConfig()
{
  return wxT("state=18875900;dir=4;layer=0;row=0;pos=3;prop=100000;bestw=68;besth=136;minw=-1;minh=-1;maxw=-1;maxh=-1;floatx=1057;floaty=535;floatw=76;floath=160");
}

/*----------------------------------------------------------------------
 *       Class:  AmayaXMLToolPanel
 *      Method:  SendDataToPanel
 * Description:  refresh the button widgets of the frame's panel
  -----------------------------------------------------------------------*/
void AmayaXMLToolPanel::SendDataToPanel( AmayaParams& p )
{
  int nb_el = (int)p.param1;
  m_fnCallback = (void*) p.param3;
  
  if (nb_el == -1)
    {
      ContainerNode node;
      ElemListElement elem;

      DLList list = (DLList) p.param2;
      DLList reflist = DLList_GetRefList(list, (Container_CompareFunction)ElemListElement_Compare);
      ForwardIterator iter = DLList_GetForwardIterator(reflist);

      m_pXMLList->Clear();    
      node = ForwardIterator_GetFirst(iter);
      while(node)
        {
          elem = (ElemListElement)node->elem;
      
// TODO removed code because not showing other than DefinedComponent
//      wxString str = TtaConvMessageToWX(ElemListElement_GetName(elem));
//      if (elem->typeClass == LanguageElement)
//        str.Prepend(TtaConvMessageToWX( TtaGetSSchemaName(elem->elem.element.type.ElSSchema)) + wxT(":"));
//      if(elem->comment)
//        str.append(wxT(" (") + TtaConvMessageToWX( elem->comment) + wxT(")"));
//      m_pXMLList->Append( str , (void*)elem);

          if (elem && elem->typeClass==DefinedComponent)
            {
              wxString str = TtaConvMessageToWX(ElemListElement_GetName(elem));
              if (elem->comment)
                str.append(wxT(" (") + TtaConvMessageToWX( elem->comment) + wxT(")"));
              m_pXMLList->Append( str , (void*)elem);
            }

          node = ForwardIterator_GetNext(iter);
        }
    
      TtaFreeMemory(iter);
      DLList_Destroy(reflist);
    }
  else
    {
      const char  *listBuffer = (char *)p.param2;
      const char  *currentEl = (char *)p.param3;
      intptr_t     ref = (intptr_t)p.param4;
  
      m_XMLRef = ref;
      /* fill the list */
      m_pXMLList->Clear();
      int i = 0;
      int index = 0;
      int sel = 0;
      if (nb_el == 0)
        return;
      while (i < nb_el && listBuffer[index] != EOS)
        {
          m_pXMLList->Append( TtaConvMessageToWX( &listBuffer[index] ) );
          if (!strcmp (&listBuffer[index], currentEl))
            /* current selected item */
            sel = i;
          index += strlen (&listBuffer[index]) + 1; /* one entry length */
          i++;
        }
      
      /* select the wanted item */
      m_pXMLList->SetSelection(sel);
      if (currentEl[0] != EOS)
        m_pXMLList->SetStringSelection(TtaConvMessageToWX(currentEl));
      
      /* recalculate layout */
      GetParent()->Layout();
      Layout();
    }
}


/*----------------------------------------------------------------------
 *       Class:  AmayaXMLToolPanel
 *      Method:  OnApply
 * Description:  
  -----------------------------------------------------------------------*/
void AmayaXMLToolPanel::OnApply( wxCommandEvent& event )
{
  ElemListElement elem = NULL;
  
  ThotCallback(m_XMLRef, INTEGER_DATA, (char*) 1);
  if(m_pXMLList && m_pXMLList->GetSelection()!=wxNOT_FOUND)
  {
    elem = (ElemListElement)m_pXMLList->GetClientData(m_pXMLList->GetSelection());
    if(elem && m_fnCallback)
      ((ElemListElement_DoInsertElementFunction)m_fnCallback)(elem);
  }
}

/*----------------------------------------------------------------------
 *       Class:  AmayaXMLToolPanel
 *      Method:  OnSelected
 * Description:  
  -----------------------------------------------------------------------*/
void AmayaXMLToolPanel::OnSelected( wxCommandEvent& event )
{
  wxString s_selected = XRCCTRL(*this, "wxID_LIST_XML", wxListBox)->GetStringSelection();
  
  // allocate a temporary buffer
  char buffer[512];
  wxASSERT( s_selected.Len() < 512 );
  strcpy( buffer, (const char*)s_selected.mb_str(wxConvUTF8) );
  
  // call the callback : send the selected string 
  ThotCallback(m_XMLRef+1, STRING_DATA, buffer);
}



/*----------------------------------------------------------------------
 *       Class:  AmayaXMLToolPanel
 *      Method:  OnRefresh
 * Description:  refresh the panel from current selection
  -----------------------------------------------------------------------*/
void AmayaXMLToolPanel::OnRefresh( wxCommandEvent& event )
{
  RefreshXMLPanel();
}

/*----------------------------------------------------------------------
 *       Class:  AmayaXMLToolPanel
 *      Method:  RefreshXMLPanel
 * Description:  refresh the panel from current selection
  -----------------------------------------------------------------------*/
void AmayaXMLToolPanel::RefreshXMLPanel()
{
  Document doc;
  View view;
  TtaGetActiveView( &doc, &view );
  if (doc > 0)
    TtaRefreshElementMenu (doc, view);
}

/*----------------------------------------------------------------------
 *  this is where the event table is declared
 *  the callbacks are assigned to an event type
 *----------------------------------------------------------------------*/
BEGIN_EVENT_TABLE(AmayaXMLToolPanel, AmayaToolPanel)
  EVT_BUTTON( XRCID("wxID_APPLY"), AmayaXMLToolPanel::OnApply )
  EVT_BUTTON( XRCID("wxID_REFRESH"), AmayaXMLToolPanel::OnRefresh )
  EVT_LISTBOX( XRCID("wxID_LIST_XML"), AmayaXMLToolPanel::OnSelected ) 
  EVT_LISTBOX_DCLICK( XRCID("wxID_LIST_XML"), AmayaXMLToolPanel::OnApply )
END_EVENT_TABLE()
#endif /* #ifdef _WX */
