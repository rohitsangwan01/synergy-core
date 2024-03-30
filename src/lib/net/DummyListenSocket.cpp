

#include "net/DummyListenSocket.h"

#include "arch/Arch.h"
#include "arch/XArch.h"
#include "base/IEventQueue.h"
#include "base/Log.h"
#include "io/XIO.h"
#include "mt/Lock.h"
#include "mt/Mutex.h"
#include "net/DummySocket.h"
#include "net/NetworkAddress.h"
#include "net/SocketMultiplexer.h"
#include "net/TSocketMultiplexerMethodJob.h"
#include "net/XSocket.h"

//
// DummyListenSocket
//

DummyListenSocket::DummyListenSocket(IEventQueue *events,
                                     SocketMultiplexer *socketMultiplexer,
                                     IArchNetwork::EAddressFamily family)
    : m_events(events), m_socketMultiplexer(socketMultiplexer) {
  m_mutex = new Mutex;
  try {
    LOG((CLOG_INFO "Initializing Dummy Socket"));
    m_socket = ARCH->newSocket(family, IArchNetwork::kSTREAM);
  } catch (XArchNetwork &e) {
    throw XSocketCreate(e.what());
  }
}

DummyListenSocket::~DummyListenSocket() {
  try {
    if (m_socket != NULL) {
      m_socketMultiplexer->removeSocket(this);
      ARCH->closeSocket(m_socket);
    }
  } catch (...) {
    // ignore
    LOG((CLOG_WARN "error while closing TCP socket"));
  }
  delete m_mutex;
}

void DummyListenSocket::bind(const NetworkAddress &addr) {
  try {
    LOG((CLOG_INFO "Binding address to socket"));
    Lock lock(m_mutex);
    ARCH->setReuseAddrOnSocket(m_socket, true);
    ARCH->bindSocket(m_socket, addr.getAddress());
    ARCH->listenOnSocket(m_socket);
    m_socketMultiplexer->addSocket(
        this,
        new TSocketMultiplexerMethodJob<DummyListenSocket>(
            this, &DummyListenSocket::serviceListening, m_socket, true, false));
  } catch (XArchNetworkAddressInUse &e) {
    throw XSocketAddressInUse(e.what());
  } catch (XArchNetwork &e) {
    throw XSocketBind(e.what());
  }
}

void DummyListenSocket::close() {
  LOG((CLOG_INFO "Closing Socket"));

  Lock lock(m_mutex);
  if (m_socket == NULL) {
    throw XIOClosed();
  }
  try {
    m_socketMultiplexer->removeSocket(this);
    ARCH->closeSocket(m_socket);
    m_socket = NULL;
  } catch (XArchNetwork &e) {
    throw XSocketIOClose(e.what());
  }
}

void *DummyListenSocket::getEventTarget() const {
  LOG((CLOG_INFO "Get Event from socket"));

  return const_cast<void *>(static_cast<const void *>(this));
}

IDataSocket *DummyListenSocket::accept() {
  IDataSocket *socket = NULL;
  try {
    LOG((CLOG_INFO "Accept Socket"));

    socket = new DummySocket(m_events, m_socketMultiplexer,
                             ARCH->acceptSocket(m_socket, NULL));
    if (socket != NULL) {
      setListeningJob();
    }
    return socket;
  } catch (XArchNetwork &) {
    if (socket != NULL) {
      delete socket;
      setListeningJob();
    }
    return NULL;
  } catch (std::exception &ex) {
    if (socket != NULL) {
      delete socket;
      setListeningJob();
    }
    throw ex;
  }
}

void DummyListenSocket::setListeningJob() {
  LOG((CLOG_INFO "Setting Listener Job"));

  m_socketMultiplexer->addSocket(
      this,
      new TSocketMultiplexerMethodJob<DummyListenSocket>(
          this, &DummyListenSocket::serviceListening, m_socket, true, false));
}

ISocketMultiplexerJob *
DummyListenSocket::serviceListening(ISocketMultiplexerJob *job, bool read, bool,
                                    bool error) {
  LOG((CLOG_INFO "serviceListening"));

  if (error) {
    close();
    return NULL;
  }
  if (read) {
    m_events->addEvent(Event(m_events->forIListenSocket().connecting(), this));
    // stop polling on this socket until the client accepts
    return NULL;
  }
  return job;
}
