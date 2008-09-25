/*
 *
 *  (c) COPYRIGHT INRIA and W3C, 1996-2008
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

#ifdef _WX
#include "wx/wx.h"
#include "logdebug.h"
#include "wxAmayaSocketEvent.h"
#include "wxAmayaSocketEventLoop.h"

#include <sys/types.h>
#ifndef _WINDOWS
#include <sys/time.h>
#include <unistd.h>
#endif /* _WINDOWS */

/* at begining m_RegistredSocket must be initialized */
bool wxAmayaSocketEvent::m_IsInitialized = false;

/* at begining, nothing is registred */
int wxAmayaSocketEvent::m_NbRegistredSocket = 0;

/* the socket filedescriptor max is null at begining because there is no socket */
int wxAmayaSocketEvent::m_SocketFDMax = 0;

/* this is the max number of sockets */
const int wxAmayaSocketEvent::MAX_SOCKET = 50;

/* this is where socket are stored */
wxAmayaSocketContext wxAmayaSocketEvent::m_RegistredSocket[MAX_SOCKET];
int                  wxAmayaSocketEvent::m_UsedSocket[MAX_SOCKET];

/* this is a reference to the event loop to be able to start/stop the polling */
wxAmayaSocketEventLoop * wxAmayaSocketEvent::m_pEventLoop = NULL;


/*----------------------------------------------------------------------
 *       Class:  wxAmayaSocketEvent
 *      Method:  InitSocketEvent
 * Description:  used to init the socket eventloop
 *               when the eventloop is known, it's possible to optimize the polling
 *               if a socket is active, the polling is enabled
 *               if nothing is active, the polling is disabled
  -----------------------------------------------------------------------*/
void wxAmayaSocketEvent::InitSocketEvent( wxAmayaSocketEventLoop * p_eventloop )
{
  TTALOGDEBUG_0( TTA_LOG_SOCKET | TTA_LOG_INIT, _T("wxAmayaSocketEvent::InitSocketEvent") );
  m_pEventLoop = p_eventloop;
  /* if there is a socket, start the pulling */
  if (m_SocketFDMax > 0)
    m_pEventLoop->Start();
}

/*----------------------------------------------------------------------
 *       Class:  wxAmayaSocketEvent
 *      Method:  RegisterSocket
 * Description:  register a socket for a given condition
 *               if the condition occure, the given callback is called
  -----------------------------------------------------------------------*/
int wxAmayaSocketEvent::RegisterSocket( int socket,
                                        wxAmayaSocketCondition condition,
                                        wxAmayaSocketCallback  callback )
{
  TTALOGDEBUG_2( TTA_LOG_SOCKET, _T("wxAmayaSocketEvent::RegisterSocket %d %d"), socket, condition );

  /* the socket array must be initialized before do something */
  Initialize();

  int new_entry = AddSocketEntry();
  /* no more free entry ? */
  if (!new_entry)
    return 0;

  /* fdmax update */
  if( !SocketExists( socket ) )
    m_SocketFDMax += socket;

  /* assigne the new socket to the reserved entry */
  /* register_id-1 because the entry 0 is invalide for the interface */
  /* but internaly, it's a valide entry */
  m_RegistredSocket[new_entry-1].socket    = socket;
  m_RegistredSocket[new_entry-1].condition = condition;
  m_RegistredSocket[new_entry-1].callback  = callback;

  /* a new socket has been created, then start the polling */
  if (m_SocketFDMax - socket == 0)
    {
      if (m_pEventLoop)
        m_pEventLoop->Start();
    }

  return new_entry;
}

/*----------------------------------------------------------------------
 *       Class:  wxAmayaSocketEvent
 *      Method:  UnregisterSocket
 * Description:  unregister a socket 
 *               this methode free an entry 
 *               and stop the pooling if nomore socket si active
  -----------------------------------------------------------------------*/
bool wxAmayaSocketEvent::UnregisterSocket( int register_id )
{
  /* the socket array must be initialized before do something */
  Initialize();

  /* register_id-1 because the entry 0 is invalide for the interface */
  /* but internaly, it's a valide entry */
  int socket = m_RegistredSocket[register_id-1].socket;

  TTALOGDEBUG_2( TTA_LOG_SOCKET, _T("wxAmayaSocketEvent::UnregisterSocket %d, socket=%d"), register_id, socket );

  bool removed = RemoveSocketEntry( register_id );

  /* fdmax update */
  if( !SocketExists( socket ) )
    m_SocketFDMax -= socket;

  /* if there is no more socket, stop the pulling */
  if (m_SocketFDMax <= 0)
    {
      if (m_pEventLoop)
        m_pEventLoop->Stop();
    }

  return removed;
}

/*----------------------------------------------------------------------
 *       Class:  wxAmayaSocketEvent
 *      Method:  CheckSocketStatus
 * Description:  used to check periodicaly if something happend on a socket
 *               use the system call "select()" to know if something is comming or not
 *               int bloking_time is the time to wait for (by default 0)
  -----------------------------------------------------------------------*/
bool wxAmayaSocketEvent::CheckSocketStatus( int bloking_time )
{
  if (m_NbRegistredSocket <= 0)
	  return false;

  struct timeval tv;
  fd_set read_fds, write_fds, except_fds;

  /* fill each descriptor to the right value */
  FD_ZERO(&read_fds);
  FD_ZERO(&write_fds);
  FD_ZERO(&except_fds);
  int id_entry = 0;
  int entry = 0;
  while ( id_entry < m_NbRegistredSocket )
    {
      entry = m_UsedSocket[id_entry];
      switch (m_RegistredSocket[entry].condition)
        {
        case WXAMAYASOCKET_READ:
          FD_SET((unsigned int)(m_RegistredSocket[entry].socket), &read_fds);
          break;
        case WXAMAYASOCKET_WRITE:
          FD_SET((unsigned int)(m_RegistredSocket[entry].socket), &write_fds);
          break;
        case WXAMAYASOCKET_EXCEPTION:
          FD_SET((unsigned int)(m_RegistredSocket[entry].socket), &except_fds);
          break;
        }
      id_entry++;
    }

  /* now just check fd status */
  /* this is a non-blocking call to select() */
  tv.tv_sec  = 0;
  tv.tv_usec = bloking_time;
  int status = select( m_SocketFDMax + 1,
                       &read_fds,
                       &write_fds,
                       &except_fds,
                       &tv );
  if (status == -1)
    {
      TTALOGDEBUG_0( TTA_LOG_SOCKET, _T("wxAmayaSocketEvent: Select failed") );
      return false;
    }


  /* now call the right callback for each socket */
  id_entry = 0;
  while ( id_entry < m_NbRegistredSocket )
    {
      entry = m_UsedSocket[id_entry];
      switch (m_RegistredSocket[entry].condition)
        {
        case WXAMAYASOCKET_READ:
          if (FD_ISSET(m_RegistredSocket[entry].socket, &read_fds))
            {
              TTALOGDEBUG_1( TTA_LOG_SOCKET, _T("wxAmayaSocketEvent::WXAMAYASOCKET_READ %d"), m_RegistredSocket[entry].socket );
              (*m_RegistredSocket[entry].callback)(entry,
                                                   m_RegistredSocket[entry].socket,
                                                   m_RegistredSocket[entry].condition);
            }
          break;

        case WXAMAYASOCKET_WRITE:
          if (FD_ISSET(m_RegistredSocket[entry].socket, &write_fds))
            {
              TTALOGDEBUG_1( TTA_LOG_SOCKET, _T("wxAmayaSocketEvent::WXAMAYASOCKET_WRITE %d"), m_RegistredSocket[entry].socket );
              (*m_RegistredSocket[entry].callback)(entry,
                                                   m_RegistredSocket[entry].socket,
                                                   m_RegistredSocket[entry].condition);
            }
          break;

        case WXAMAYASOCKET_EXCEPTION:
          if (FD_ISSET(m_RegistredSocket[entry].socket, &except_fds))
            {
              TTALOGDEBUG_1( TTA_LOG_SOCKET, _T("wxAmayaSocketEvent::WXAMAYASOCKET_EXCEPTION %d"), m_RegistredSocket[entry].socket );
              (*m_RegistredSocket[entry].callback)(entry,
                                                   m_RegistredSocket[entry].socket,
                                                   m_RegistredSocket[entry].condition);
            }
          break;
        }
      id_entry++;
    }

  return true;
}


/*----------------------------------------------------------------------
 *       Class:  wxAmayaSocketEvent
 *      Method:  SocketExists
 * Description:  private methode used to detect if a given socket is allready registred
  -----------------------------------------------------------------------*/
bool wxAmayaSocketEvent::SocketExists( int socket )
{
  int id_entry = 0;
  while ( id_entry < m_NbRegistredSocket )
    {
      if ( socket == m_RegistredSocket[m_UsedSocket[id_entry]].socket )
        return true;
      id_entry++;
    }
  return false;
}

/*----------------------------------------------------------------------
 *       Class:  wxAmayaSocketEvent
 *      Method:  AddSocketEntry
 * Description:  private methode used to reserve a socket slot
  -----------------------------------------------------------------------*/
int wxAmayaSocketEvent::AddSocketEntry()
{
  /* look for a free place */
  int entry = 0;
  bool found = false;
  while ( entry < MAX_SOCKET && !found )
    {
      found = (m_RegistredSocket[entry].socket == 0);
      entry++;
    }
  entry--;
  if (!found)
    return -1;

  /* add the entry index to the entry index array */
  m_NbRegistredSocket++;
  if (m_NbRegistredSocket < MAX_SOCKET)
    {
      /* recorde the new entry index */
      m_UsedSocket[m_NbRegistredSocket-1] = entry;
    }
  else
    {
      /* not in the range : too much socket */
      m_NbRegistredSocket--;
      return -1;
    }

  /* entry+1 because the entry 0 is invalide for the interface */
  /* but internaly, it's a valide entry */
  return entry+1;
}

/*----------------------------------------------------------------------
 *       Class:  wxAmayaSocketEvent
 *      Method:  RemoveSocketEntry
 * Description:  private methode used to remove a socket slot
  -----------------------------------------------------------------------*/
bool wxAmayaSocketEvent::RemoveSocketEntry( int entry )
{
  /* entry-- because the entry 0 is invalide for the interface */
  /* but internaly, it's a valide entry */
  entry--;

  /* look for this entry */
  int id_entry = 0;
  bool found = false;
  while ( id_entry < m_NbRegistredSocket && !found )
    {
      found = (m_UsedSocket[id_entry] == entry);
      id_entry++;
    }
  id_entry--;
  if (!found)
    return false;

  /* remove the index entry */
  for (int i = id_entry ; i < m_NbRegistredSocket - 1; i++)
    m_UsedSocket[i] = m_UsedSocket[i + 1];
  m_UsedSocket[m_NbRegistredSocket - 1] = 0;
  m_NbRegistredSocket--;

  /* remove the real entry */
  memset(&m_RegistredSocket[entry], 0, sizeof(wxAmayaSocketContext));

  return true;
}

/*----------------------------------------------------------------------
 *       Class:  wxAmayaSocketEvent
 *      Method:  Initialize
 * Description:  private methode used to init the socket's arrays
  -----------------------------------------------------------------------*/
void wxAmayaSocketEvent::Initialize()
{
  if (!m_IsInitialized)
    {
      memset (m_RegistredSocket, 0, sizeof(wxAmayaSocketContext) * MAX_SOCKET);
      memset (m_UsedSocket, 0, sizeof(int) * MAX_SOCKET);
    }
  m_IsInitialized = true;
}

#endif /* _WX */ 
