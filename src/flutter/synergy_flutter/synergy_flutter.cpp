#include "DummyClientProxy.h"
#include "MyStream.h"
#include "arch/Arch.h"
#include "base/EventQueue.h"
#include "base/Log.h"
#include "synergy/DummyServerApp.h"
#include "synergy/ServerArgs.h"
#include "synergy/protocol_types.h"
#include <cstdio>
#include <iostream>

int main(int argc, char **argv) {

  ClientInfo clientInfo = ClientInfo();
  clientInfo.m_x = 0;
  clientInfo.m_y = 0;
  clientInfo.m_mx = 0;
  clientInfo.m_my = 0;
  clientInfo.m_w = 1920;
  clientInfo.m_h = 1080;

  // for (int i = 0; i < argc; i++) {
  //   if (strcmp(argv[i], "-w") == 0) {
  //     clientInfo.m_w = std::stoi(argv[i + 1]);
  //   }
  //   if (strcmp(argv[i], "-h") == 0) {
  //     clientInfo.m_h = std::stoi(argv[i + 1]);
  //   }
  // }

  // // if w and h not given throw error
  // if (clientInfo.m_w == 0 || clientInfo.m_h == 0) {
  //   std::cerr << "Width (-w) or Height(-h) not provided" << std::endl;
  //   return 1;
  // }

  // remove -w  and -h argument from argv
  // for (int i = 0; i < argc - 2; i++) {
  //   if (strcmp(argv[i], "-w") == 0 || strcmp(argv[i], "-h") == 0) {
  //     for (int j = i; j < argc - 2; j++) {
  //       argv[j] = argv[j + 2];
  //     }
  //   }
  // }

  Arch arch;
  arch.init();

  Log log;
  EventQueue events;

  synergy::IStream *adoptedStream = new MyStream();

  DummyClientProxy client("flutter", adoptedStream, &events, clientInfo);

  DummyServerApp app(&events, &client);

  // client.setDeviceInfo(clientInfo);

  app.args().m_configFile = "/Users/rohitsangwan/Drive/Devlopment/c++/"
                            "synergy_core_clean/synergy.conf";
  app.args().m_daemon = false; // -f

  return app.run(argc, argv);
}
