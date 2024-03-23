// #include "synergy/ServerApp.h"
// #include "arch/Arch.h"
// #include "base/Log.h"
// #include "base/EventQueue.h"

// extern "C"
// {

//     ServerApp *createServer()
//     {
//         Arch *arch = new Arch();
//         arch->init();

//         Log *log = new Log();
//         EventQueue *events = new EventQueue();

//         ServerApp *app = new ServerApp(events, nullptr);

//         return app;
//     }

//     void startServer(ServerApp app)
//     {
//         app.startServer();
//     }

//     void stopServer(ServerApp app)
//     {
//         app.stopServer();
//     }
// }