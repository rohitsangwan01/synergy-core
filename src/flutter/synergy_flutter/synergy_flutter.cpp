#include "DummyClientProxy.h"
#include "MyStream.h"
#include "arch/Arch.h"
#include "base/EventQueue.h"
#include "base/Log.h"
#include "my_args.h"
#include "synergy/DummyServerApp.h"
#include "synergy/ServerArgs.h"
#include "synergy/protocol_types.h"
#include <condition_variable>
#include <cstdio>
#include <deque>
#include <future>
#include <iostream>
#include <memory>
#include <mutex>
#include <string>
#include <thread>

void startListeningToStdin() {}

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

  // Start listening to client input
  // std::mutex clientMutex;
  // auto io_thread = std::thread([&] {
  //   std::string s;
  //   while (true) {
  //     try {
  //       if (!std::getline(std::cin, s, '\n')) {
  //         break;
  //       }
  //     } catch (const std::exception &e) {
  //       std::cerr << "Error reading from stdin: " << e.what() << std::endl;
  //       break;
  //     }
  //     std::string new_string = std::move(s);
  //     try {
  //       std::lock_guard<std::mutex> lock(clientMutex);
  //       client.handleDataFromClient(new_string);
  //     } catch (const std::exception &e) {
  //       std::cerr << "Error handling data from client: " << e.what()
  //                 << std::endl;
  //     }
  //   }
  // });
  // std::thread joinThread([&io_thread]() { io_thread.join(); });
  // joinThread.detach();

  // Start Server App
  DummyServerApp app(&events, &client);
  app.args().m_daemon = false; // -f
  return app.run(argc, argv);
}
