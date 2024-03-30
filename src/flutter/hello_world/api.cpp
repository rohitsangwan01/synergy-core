#include "synergy/ServerArgs.h"
#include <cstdio>
#define EXPORT                                                                 \
  extern "C" __attribute__((visibility("default"))) __attribute__((used))
#include "DummyClientProxy.h"
#include "MyStream.h"
#include "arch/Arch.h"
#include "base/EventQueue.h"
#include "base/Log.h"
#include "synergy/DummyServerApp.h"
#include <cstring>

EXPORT
int add(int a, int b) { return a + b; }

EXPORT
char *capitalize(char *str) {
  static char buffer[1024];
  strcpy(buffer, str);
  return buffer;
}

EXPORT
void startServer(int argc, char **argv) {
  Arch arch;
  arch.init();

  Log log;
  EventQueue events;

  synergy::IStream *adoptedStream = new MyStream();
  ClientInfo clientInfo = ClientInfo();
  clientInfo.m_x = 0;
  clientInfo.m_y = 0;
  clientInfo.m_w = 1920;
  clientInfo.m_h = 1080;
  clientInfo.m_mx = 960;
  clientInfo.m_my = 540;
  DummyClientProxy client("flutter", adoptedStream, &events, clientInfo);

  DummyServerApp app(&events, &client);

  app.args().m_configFile = "/Users/rohitsangwan/Drive/Devlopment/c++/"
                            "synergy_core_clean/synergy.conf";
  app.args().m_daemon = false; // -f

  // Print the arguments
  printf("Starting app\n");
  app.run(argc, argv);
  printf("Closing app\n");
  // app.stopServer();
  // app.startServer();
}
