

#pragma once

#include "arch/IArchNetwork.h"
#include "net/IListenSocket.h"

class Mutex;
class ISocketMultiplexerJob;
class IEventQueue;
class SocketMultiplexer;

// A dummy socket
class DummyListenSocket : public IListenSocket {
public:
  DummyListenSocket(IEventQueue *events, SocketMultiplexer *socketMultiplexer,
                    IArchNetwork::EAddressFamily family);
  DummyListenSocket(DummyListenSocket const &) = delete;
  DummyListenSocket(DummyListenSocket &&) = delete;
  virtual ~DummyListenSocket();

  DummyListenSocket &operator=(DummyListenSocket const &) = delete;
  DummyListenSocket &operator=(DummyListenSocket &&) = delete;

  // ISocket overrides
  virtual void bind(const NetworkAddress &);
  virtual void close();
  virtual void *getEventTarget() const;

  // IListenSocket overrides
  virtual IDataSocket *accept();

protected:
  void setListeningJob();

public:
  ISocketMultiplexerJob *serviceListening(ISocketMultiplexerJob *, bool, bool,
                                          bool);

protected:
  ArchSocket m_socket;
  Mutex *m_mutex;
  IEventQueue *m_events;
  SocketMultiplexer *m_socketMultiplexer;
};
