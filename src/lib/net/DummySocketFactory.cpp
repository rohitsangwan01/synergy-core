
#include "net/DummySocketFactory.h"
#include "arch/Arch.h"
#include "base/Log.h"
#include "net/DummyListenSocket.h"
#include "net/SecureListenSocket.h"
#include "net/SecureSocket.h"
#include "net/TCPListenSocket.h"
#include "net/TCPSocket.h"

//
// DummySocketFactory
//

DummySocketFactory::DummySocketFactory(IEventQueue *events,
                                       SocketMultiplexer *socketMultiplexer)
    : m_events(events), m_socketMultiplexer(socketMultiplexer) {
  // do nothing
}

DummySocketFactory::~DummySocketFactory() {
  // do nothing
}

IDataSocket *
DummySocketFactory::create(bool secure,
                           IArchNetwork::EAddressFamily family) const {
  LOG((CLOG_INFO "Creating Dummy SocketFactory"));
  return new TCPSocket(m_events, m_socketMultiplexer, family);
}

IListenSocket *
DummySocketFactory::createListen(bool secure,
                                 IArchNetwork::EAddressFamily family) const {
  LOG((CLOG_INFO "Creating Dummy Listener"));
  return new DummyListenSocket(m_events, m_socketMultiplexer, family);
}
