#ifdef _WX
#include "logdebug.h"
#include "AmayaAppInstance.h"
#include "message_wx.h"

#include "thot_gui.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "application.h"
#include "fileaccess.h"

static ThotBool A_multiple = FALSE;

/*----------------------------------------------------------------------
 *       Class:  AmayaAppInstance
 *      Method:  AmayaAppInstance
 * Description:  
  -----------------------------------------------------------------------*/
AmayaAppInstance::AmayaAppInstance( AmayaApp * p_amaya_app ) :
  m_pAmayaApp(p_amaya_app)
{
  // check the environment variable
  TtaGetEnvBoolean ("AMAYA_MULTIPLE_INSTANCES", &A_multiple);
#ifdef _WINDOWS
  m_InstanceName = wxString::Format(m_pAmayaApp->GetAppName()+_T("-%s"),
                                    wxGetUserId().c_str());
#else /* _WINDOWS */
  m_InstanceName = wxString::Format(_T(".")+m_pAmayaApp->GetAppName()+_T("-%s"),
                                    wxGetUserId().c_str());
#endif /* _WINDOWS */
  if (!A_multiple)
    m_pSingleInstance_Checker = new wxSingleInstanceChecker(m_InstanceName);
  m_ServicePort = _T("4242");
  m_ServiceTopic = m_InstanceName;
  m_ServiceHostname = _T("localhost");
  m_pURLGrabberServer = NULL;

  TTALOGDEBUG_0( TTA_LOG_INIT, _T("AppInstanceName=") + m_InstanceName );
}

/*----------------------------------------------------------------------
 *       Class:  AmayaAppInstance
 *      Method:  ~AmayaAppInstance
 * Description:  
  -----------------------------------------------------------------------*/
AmayaAppInstance::~AmayaAppInstance()
{
  if (!A_multiple)
    {
      // no multiple instances
      delete m_pSingleInstance_Checker;
      m_pSingleInstance_Checker = NULL;
    }

  delete m_pURLGrabberServer;
  m_pURLGrabberServer = NULL;
}

/*----------------------------------------------------------------------
 *       Class:  AmayaAppInstance
 *      Method:  IsAnotherAmayaRunning
 * Description:  
  -----------------------------------------------------------------------*/
bool AmayaAppInstance::IsAnotherAmayaRunning()
{
  if (A_multiple)
    return false;
  else
    return ( m_pSingleInstance_Checker->IsAnotherRunning() );
}

/*----------------------------------------------------------------------
 *       Class:  AmayaAppInstance
 *      Method:  SendURLToOtherAmayaInstance
 * Description:  
  -----------------------------------------------------------------------*/
void AmayaAppInstance::SendURLToOtherAmayaInstance(const wxString & url)
{
  PathBuffer   execname;

  if (IsAnotherAmayaRunning())
    {
      wxClient *p_client = new wxClient();
      wxConnection *p_connection = (wxConnection *)p_client->MakeConnection(m_ServiceHostname, m_ServicePort, m_ServiceTopic);
      
      if (!p_connection)
        {
          wxLogMessage(_T("Failed to make connection to running Amaya instance."));
          return;
        }
      
      char buffer[512];
      strcpy(buffer, (const char*)url.mb_str(wxConvUTF8) );
      wxLogError(_T("AmayaAppInstance::SendURLToOtherAmayaInstance - url=")+url);
      if (!TtaIsW3Path (buffer))
	{
          /* check if it is an absolute or a relative name */
#ifdef _WINDOWS
          if (buffer[0] == DIR_SEP || buffer[1] == ':')
#else /* _WINDOWS */
          if (buffer[0] == DIR_SEP)
#endif /* _WINDOWS */
	    {
	      /* it is an absolute name */
	      p_connection->Poke(_T("URL"), (wxChar *)buffer, strlen(buffer)+1);
	    }
          else
	    {
	      /* it is a relative name */
	      getcwd (&execname[0], sizeof (execname) / sizeof (char));
	      strcat (execname, DIR_STR);
	      strcat (execname, buffer);
	      p_connection->Poke(_T("URL"), (wxChar *)execname, strlen(execname)+1);
	    }
	}
      else
	p_connection->Poke(_T("URL"), (wxChar *)buffer, strlen(buffer)+1);
      
      delete p_client;
    }
}

/*----------------------------------------------------------------------
 *       Class:  AmayaAppInstance
 *      Method:  StartURLGrabberServer
 * Description:  
  -----------------------------------------------------------------------*/
void AmayaAppInstance::StartURLGrabberServer()
{
  if (!IsAnotherAmayaRunning())
    {
      m_pURLGrabberServer = new AmayaURLGrabberServer(m_ServiceTopic);
      m_pURLGrabberServer->Create(m_ServicePort);
    }
}

/*----------------------------------------------------------------------
 *       Class:  AmayaAppInstance
 *      Method:  RegisterOpenURLCallback
 * Description:  
  -----------------------------------------------------------------------*/
void AmayaAppInstance::RegisterOpenURLCallback( OpenURLCallback callback )
{
  m_pURLGrabberServer->RegisterOpenURLCallback(callback);
}

/*----------------------------------------------------------------------
 *       Class:  AmayaAppInstance
 *      Method:  CallOpenURLCallback
 * Description:  
  -----------------------------------------------------------------------*/
void AmayaAppInstance::CallOpenURLCallback( char * url )
{
  OpenURLCallback callback;
  callback = m_pURLGrabberServer->GetOpenURLCallback();
  (*callback)( url );
}

/*----------------------------------------------------------------------
 *       Class:  AmayaURLGrabberServer
 *      Method:  RegisterOpenURLCallback
 * Description:  
  -----------------------------------------------------------------------*/
void AmayaURLGrabberServer::RegisterOpenURLCallback( OpenURLCallback callback )
{
  m_pURLOpenCallback = callback;
}

/*----------------------------------------------------------------------
 *       Class:  AmayaURLGrabberServer
 *      Method:  RegisterOpenURLCallback
 * Description:  
  -----------------------------------------------------------------------*/
OpenURLCallback AmayaURLGrabberServer::GetOpenURLCallback()
{
  return m_pURLOpenCallback;
}

/*----------------------------------------------------------------------
  Class:  AmayaURLGrabberConnection
  Method:  OnPoke
  Description:  Called when a new amaya instance throw an url to the existing one
  -----------------------------------------------------------------------*/
bool AmayaURLGrabberConnection::OnPoke(const wxString& topic, const wxString& item,
                                       wxChar *data, int size, wxIPCFormat format)
{
  if (topic == m_Owner.m_AcceptedTopic && m_Owner.m_pURLOpenCallback)
    {
      /* copy the possible url argument */
      char buffer[512];
      strcpy(buffer, (char *)data);
      
      /* call the open document callback */
      (*m_Owner.m_pURLOpenCallback)( buffer );
      return true;
    }
  else
    return false;
}


#endif /* _WX */
