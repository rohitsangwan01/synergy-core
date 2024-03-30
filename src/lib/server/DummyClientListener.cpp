#include "server/DummyClientListener.h"
#include "base/IEventQueue.h"
#include "base/Log.h"
#include "base/TMethodEventJob.h"
#include "server/ClientProxy.h"
#include "server/ClientProxyUnknown.h"
#include "server/Server.h"
#include "synergy/PacketStreamFilter.h"

//
// DummyClientListener
//

DummyClientListener::DummyClientListener(ClientProxy *client,
                                         IEventQueue *events, bool enableCrypto)
    : m_server(NULL), m_clientProxy(client), m_events(events),
      m_useSecureNetwork(enableCrypto) {

  try {
    start();
  } catch (XBase &) {
    cleanupListenSocket();
    throw;
  }
  LOG((CLOG_DEBUG1 "listening for clients"));
}

DummyClientListener::~DummyClientListener() { stop(); }

void DummyClientListener::setServer(Server *server) {
  assert(server != NULL);
  m_server = server;
}

ClientProxy *DummyClientListener::getNextClient() {
  ClientProxy *client = NULL;
  if (!m_waitingClients.empty()) {
    client = m_waitingClients.front();
    m_waitingClients.pop_front();
    m_events->removeHandler(m_events->forClientProxy().disconnected(), client);
  }
  return client;
}

void DummyClientListener::start() {
  LOG((CLOG_NOTE "Adding Dummy client"));
  auto client = m_clientProxy;

  // handshake was successful
  m_waitingClients.push_back(client);
  m_events->addEvent(Event(m_events->forClientListener().connected(), this));

  // watch for client to disconnect while it's in our queue
  m_events->adoptHandler(
      m_events->forClientProxy().disconnected(), client,
      new TMethodEventJob<DummyClientListener>(
          this, &DummyClientListener::handleClientDisconnected, client));
}

void DummyClientListener::stop() {
  LOG((CLOG_INFO "stop listening for clients"));

  // discard already connected clients
  for (NewClients::iterator index = m_newClients.begin();
       index != m_newClients.end(); ++index) {
    ClientProxyUnknown *client = *index;
    m_events->removeHandler(m_events->forClientProxyUnknown().success(),
                            client);
    m_events->removeHandler(m_events->forClientProxyUnknown().failure(),
                            client);
    m_events->removeHandler(m_events->forClientProxy().disconnected(), client);
    delete client;
  }

  // discard waiting clients
  ClientProxy *client = getNextClient();
  while (client != nullptr) {
    delete client;
    client = getNextClient();
  }

  cleanupListenSocket();
  cleanupClientSockets();
}

void DummyClientListener::removeUnknownClient(
    ClientProxyUnknown *unknownClient) {
  if (unknownClient) {
    m_events->removeHandler(m_events->forClientProxyUnknown().success(),
                            unknownClient);
    m_events->removeHandler(m_events->forClientProxyUnknown().failure(),
                            unknownClient);
    m_newClients.erase(unknownClient);
    delete unknownClient;
  }
}

void DummyClientListener::restart() {
  if (m_server && m_server->isClientMode()) {
    stop();
    start();
  }
}

void DummyClientListener::handleClientConnecting(const Event &, void *) {
  // TODO: handle accepted client
  // &DummyClientListener::handleClientAccepted
}

void DummyClientListener::handleClientAccepted(const Event &, void *vsocket) {
  LOG((CLOG_NOTE "accepted client connection"));
}

void DummyClientListener::handleUnknownClient(const Event &, void *vclient) {
  auto unknownClient = static_cast<ClientProxyUnknown *>(vclient);

  // we should have the client in our new client list
  assert(m_newClients.count(unknownClient) == 1);

  // get the real client proxy and install it
  auto client = unknownClient->orphanClientProxy();
  if (client) {
    // handshake was successful
    m_waitingClients.push_back(client);
    m_events->addEvent(Event(m_events->forClientListener().connected(), this));

    // watch for client to disconnect while it's in our queue
    m_events->adoptHandler(
        m_events->forClientProxy().disconnected(), client,
        new TMethodEventJob<DummyClientListener>(
            this, &DummyClientListener::handleClientDisconnected, client));
  }

  // now finished with unknown client
  removeUnknownClient(unknownClient);
}

void DummyClientListener::handleUnknownClientFailure(const Event &,
                                                     void *vclient) {
  auto unknownClient = static_cast<ClientProxyUnknown *>(vclient);
  removeUnknownClient(unknownClient);
  restart();
}

void DummyClientListener::handleClientDisconnected(const Event &,
                                                   void *vclient) {
  ClientProxy *client = static_cast<ClientProxy *>(vclient);

  // find client in waiting clients queue
  for (WaitingClients::iterator i = m_waitingClients.begin(),
                                n = m_waitingClients.end();
       i != n; ++i) {
    if (*i == client) {
      m_waitingClients.erase(i);
      m_events->removeHandler(m_events->forClientProxy().disconnected(),
                              client);

      // pull out the socket before deleting the client so
      // we know which socket we no longer need

      delete client;
      break;
    }
  }
}

void DummyClientListener::cleanupListenSocket() {}

void DummyClientListener::cleanupClientSockets() {
  ClientSockets::iterator it;
  for (it = m_clientSockets.begin(); it != m_clientSockets.end(); it++) {
    // delete *it;
  }
  m_clientSockets.clear();
}
