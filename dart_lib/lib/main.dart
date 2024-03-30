import 'dart:convert';
import 'dart:ffi';
import 'dart:io';
import 'dart:typed_data';
import 'package:ffi/ffi.dart';
import 'package:synergy_client_dart/synergy_client_dart.dart';
import 'generated_bindings.dart';

void main(List<String> args) async {
  // executeNative();
  // return;

  BasicScreen screen = BasicScreen();

  SynergyClientDart.setLogLevel(LogLevel.debug1);

  await SynergyClientDart.connect(
    screen: screen,
    synergyServer: CmdServer(),
    // synergyServer: SocketServer("0.0.0.0", 24800),
    clientName: "flutter",
  );
}

class CmdServer extends ServerInterface {
  Process? _process;

  @override
  Future<void> connect(
    Function(Uint8List data) onData,
    Function(String? error) onError,
    Function() onDone,
  ) async {
    _process = await Process.start(
      "/Users/rohitsangwan/Drive/Devlopment/c++/synergy_core_clean/build/bin/synergy_flutter",
      [],
    );

    _process?.stdout.listen((event) {
      String data = utf8.decode(event);
      data.split("\n").forEach((element) {
        if (element.contains("Buffer")) {
          var bytes = element.replaceFirst("Buffer:", "").trim();
          List<String> stringByes = bytes.split(",");
          stringByes.removeWhere((element) => element.isEmpty);
          List<int> intBytes = [];
          for (var e in stringByes) {
            List<String> chars = e.split("");
            for (String char in chars) {
              if (char == "0") {
                intBytes.add(0);
              } else {
                String remaining = chars.sublist(chars.indexOf(char)).join();
                intBytes.add(int.tryParse(remaining) ?? 0);
                break;
              }
            }
          }

          onData(Uint8List.fromList(intBytes));
        }
      });
    });

    _process?.stderr.listen((event) {
      print("error ${utf8.decode(event)}");
    });
  }

  @override
  Future<void> disconnect() async {
    _process?.kill();
  }

  @override
  Future<void> write(Uint8List data) async {
    print("WRITING TO SERVER: $data");
    _process?.stdin.write(data);
  }
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

void executeNative() async {
  NativeLibrary nativeLibrary = NativeLibrary(DynamicLibrary.open(
    '/Users/rohitsangwan/Drive/Devlopment/c++/synergy_core_clean/build/lib/libapi.dylib',
  ));

  Pointer<Pointer<Char>> argv = calloc<Pointer<Char>>();
  nativeLibrary.startServer(0, argv);
  await Future.delayed(const Duration(seconds: 5));
  print("Done");
}
