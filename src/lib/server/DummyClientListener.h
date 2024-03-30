
#pragma once

#include "base/Event.h"
#include "base/EventTypes.h"
#include "common/stddeque.h"
#include "common/stdset.h"
#include "server/Config.h"

class ClientProxy;
class ClientProxyUnknown;
class NetworkAddress;
class IListenSocket;
class ISocketFactory;
class Server;
class IEventQueue;
class IDataSocket;

class DummyClientListener {
public:
  // The factories are adopted.
  DummyClientListener(ClientProxy *client, IEventQueue *events,
                      bool enableCrypto);
  DummyClientListener(DummyClientListener const &) = delete;
  DummyClientListener(DummyClientListener &&) = delete;
  ~DummyClientListener();

  DummyClientListener &operator=(DummyClientListener const &) = delete;
  DummyClientListener &operator=(DummyClientListener &&) = delete;

  //! @name manipulators
  //@{

  void setServer(Server *server);

  //@}

  //! @name accessors
  //@{

  //! Get next connected client
  /*!
  Returns the next connected client and removes it from the internal
  list.  The client is responsible for deleting the returned client.
  Returns NULL if no clients are available.
  */
  ClientProxy *getNextClient();

  //! Get server which owns this listener
  Server *getServer() { return m_server; }

  //! This method restarts the listener
  void restart();

  //@}

private:
  // client connection event handlers
  void handleClientConnecting(const Event &, void *);
  void handleClientAccepted(const Event &, void *);
  void handleUnknownClient(const Event &, void *);
  void handleUnknownClientFailure(const Event &, void *);
  void handleClientDisconnected(const Event &, void *);

  void cleanupListenSocket();
  void cleanupClientSockets();
  void start();
  void stop();
  void removeUnknownClient(ClientProxyUnknown *unknownClient);

private:
  typedef std::set<ClientProxyUnknown *> NewClients;
  typedef std::deque<ClientProxy *> WaitingClients;
  typedef std::set<IDataSocket *> ClientSockets;

  NewClients m_newClients;
  WaitingClients m_waitingClients;
  Server *m_server;
  ClientProxy *m_clientProxy;
  IEventQueue *m_events;
  bool m_useSecureNetwork;
  ClientSockets m_clientSockets;
};
