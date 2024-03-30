#include "DummyClientProxy.h"
#include "MyStream.h"
#include "arch/Arch.h"
#include "base/EventQueue.h"
#include "base/Log.h"
#include "synergy/DummyServerApp.h"
#include "synergy/ServerArgs.h"

int main(int argc, char **argv) {
  Arch arch;
  arch.init();

  Log log;
  EventQueue events;

  synergy::IStream *adoptedStream = new MyStream();
  DummyClientProxy client("flutter", adoptedStream, &events);

  DummyServerApp app(&events, &client);

  app.args().m_configFile = "/Users/rohitsangwan/Drive/Devlopment/c++/"
                            "synergy_core_clean/synergy.conf";
  app.args().m_daemon = false; // -f

  return app.run(argc, argv);
}
