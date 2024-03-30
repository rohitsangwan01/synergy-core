

#pragma once

#include "arch/IArchNetwork.h"
#include "net/ISocketFactory.h"

class IEventQueue;
class SocketMultiplexer;

//! Socket factory for dummy events
class DummySocketFactory : public ISocketFactory {
public:
  DummySocketFactory(IEventQueue *events, SocketMultiplexer *socketMultiplexer);
  virtual ~DummySocketFactory();

  // ISocketFactory overrides
  virtual IDataSocket *
  create(bool secure,
         IArchNetwork::EAddressFamily family = IArchNetwork::kINET) const;
  virtual IListenSocket *
  createListen(bool secure,
               IArchNetwork::EAddressFamily family = IArchNetwork::kINET) const;

private:
  IEventQueue *m_events;
  SocketMultiplexer *m_socketMultiplexer;
};
