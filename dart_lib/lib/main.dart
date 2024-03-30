import 'package:synergy_client_dart/synergy_client_dart.dart';
import 'package:synergy_test/std_server.dart';

void main(List<String> args) async {
  BasicScreen screen = BasicScreen();

  SynergyClientDart.setLogLevel(LogLevel.debug1);

  await SynergyClientDart.connect(
    screen: screen,
    synergyServer: StdServer(),
    // synergyServer: SocketServer("0.0.0.0", 24800),
    clientName: "flutter",
  );
}

class BasicScreen extends ScreenInterface {
  @override
  void onConnect() {
    print("Connected");
  }

  @override
  void onDisconnect() {
    print("Disconnected");
  }

  @override
  void onError(String error) {
    print("Error $error");
  }

  @override
  CursorPosition getCursorPos() {
    return CursorPosition(0, 0);
  }

  @override
  RectObj getShape() {
    return RectObj(width: 1920, height: 1080);
  }

  @override
  void enter(int x, int y, int sequenceNumber, int toggleMask) {
    print('Enter');
  }

  @override
  bool leave() {
    print('Leave');
    return false;
  }

  @override
  void mouseMove(int x, int y) {
    print("MouseMove: $x $y");
  }

  @override
  void mouseDown(int buttonID) {
    print("MouseDown: $buttonID");
  }

  @override
  void mouseUp(int buttonID) {
    print("MouseUp: $buttonID");
  }

  @override
  void mouseWheel(int x, int y) {
    print("MouseWheel: $x $y");
  }

  @override
  void keyDown(int keyEventID, int mask, int button) {
    print("KeyDowin: $keyEventID");
  }

  @override
  void keyRepeat(int keyEventID, int mask, int count, int button) {
    print("KeyRepeat $keyEventID");
  }
}
