#ifdef _WX

#include "wx/wx.h"
#include "wx/tipdlg.h"

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
#include "message_wx.h"
#include "libmsg.h"
#include "frame.h"
#include "logdebug.h"

#undef THOT_EXPORT
#define THOT_EXPORT extern


#include "AmayaTipOfTheDay.h"
#include "tips.h"

static const int s_tips[] = 
{
    LIB, TIP_AMAYA_PROFILES,
    LIB, TIP_PANEL_MODES,
    LIB, TIP_PARENT_SELECTION,
    LIB, TIP_SPLIT_NOTEBOOK,
    LIB, TIP_MOVE_PANELS,
    LIB, TIP_MANY_COLUMNS,
    LIB, TIP_EDITING_MODE,
    LIB, TIP_ERROR_BUTTON,
    LIB, TIP_PATH_CONTROL,
    LIB, TIP_DOCUMENT_LIST_MENU
};

//  TtaGetEnvInt

static const int s_tipsCount = sizeof(s_tips)/sizeof(int)/2;



/*----------------------------------------------------------------------
 -----------------------------------------------------------------------*/
AmayaTipProvider::AmayaTipProvider(size_t currentTip):
  wxTipProvider(currentTip)
{
}

/*----------------------------------------------------------------------
 -----------------------------------------------------------------------*/
AmayaTipProvider::~AmayaTipProvider()
{
}

/*----------------------------------------------------------------------
 -----------------------------------------------------------------------*/
wxString AmayaTipProvider::GetTip()
{
  int num = m_currentTip++;
   m_currentTip %= s_tipsCount;

  return TtaConvMessageToWX(TtaGetMessage(s_tips[num*2],
                                          s_tips[num*2+1]));
}




#endif /* #ifdef _WX */


/*----------------------------------------------------------------------
 * Show TipOfTheDay dialog
 -----------------------------------------------------------------------*/
void TtaShowTipOfTheDay()
{
#ifdef _WX
  int num = 0;
  ThotBool res, show = TtaShowTipOfTheDayAtStartup();
  
  TtaGetEnvInt("TIP_OF_THE_DAY_NUMBER", &num);
  
  AmayaTipProvider prov(num);
  res = wxShowTip(NULL,& prov);
  
  if(res!=show)
    TtaSetShowTipOfTheDayAtStartup(res);
  TtaSetEnvInt("TIP_OF_THE_DAY_NUMBER", (int)prov.GetCurrentTip(), TRUE);
#endif /* _WX */
}

/*----------------------------------------------------------------------
 * Test if TipOfTheDay must be shown at startup
 -----------------------------------------------------------------------*/
ThotBool TtaShowTipOfTheDayAtStartup()
{
  ThotBool show = TRUE;
  TtaGetEnvBoolean("TIP_OF_THE_DAY_STARTUP", &show);
  return show;
}

/*----------------------------------------------------------------------
 * Set if TipOfTheDay must be shown at startup
 -----------------------------------------------------------------------*/
void TtaSetShowTipOfTheDayAtStartup(ThotBool show)
{
  TtaSetEnvBoolean("TIP_OF_THE_DAY_STARTUP", show, TRUE);
}
