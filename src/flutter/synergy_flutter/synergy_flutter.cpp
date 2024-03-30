#include "DummyClientProxy.h"
#include "MyStream.h"
#include "arch/Arch.h"
#include "base/EventQueue.h"
#include "base/Log.h"
#include "my_args.h"
#include "synergy/DummyServerApp.h"
#include "synergy/ServerArgs.h"
#include "synergy/protocol_types.h"
#include <cstdio>
#include <iostream>

int main(int argc, char **argv) {
  MyArgs myArgs = MyArgs();
  if (!myArgs.parse(argc, argv)) {
    return 1;
  }

  Arch arch;
  arch.init();

  Log log;
  EventQueue events;

  synergy::IStream *adoptedStream = new MyStream();

  ClientInfo clientInfo = ClientInfo();
  clientInfo.m_x = 0;
  clientInfo.m_y = 0;
  clientInfo.m_mx = 0;
  clientInfo.m_my = 0;
  clientInfo.m_w = myArgs.m_w;
  clientInfo.m_h = myArgs.m_h;

  DummyClientProxy client(myArgs.clientName, adoptedStream, &events,
                          clientInfo);

  DummyServerApp app(&events, &client);

  app.args().m_configFile = "/Users/rohitsangwan/Drive/Devlopment/c++/"
                            "synergy_core_clean/synergy.conf";
  app.args().m_daemon = false; // -f

  return app.run(argc, argv);
}

// Create a class to hold args
