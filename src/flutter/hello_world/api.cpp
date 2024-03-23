#define EXPORT                                                                 \
  extern "C" __attribute__((visibility("default"))) __attribute__((used))
#include "arch/Arch.h"
#include "base/EventQueue.h"
#include "base/Log.h"
#include "synergy/ServerApp.h"
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

  // Listen to event updates

  ServerApp app(&events, nullptr);
  // app.args().m_configFile = "config.json";
  // add -c and -d flags
  // argv[1] = "-c";
  // argv[2] =
  // "/Users/rohitsangwan/Drive/Devlopment/c++/synergy_core_clean/synergy.conf";
  // argv[3] = "-f";

  // app.stopServer();
  // app.startServer();
  app.run(argc, argv);
}