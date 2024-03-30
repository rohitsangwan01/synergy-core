import 'dart:convert';
import 'dart:io';
import 'dart:typed_data';

import 'package:synergy_client_dart/synergy_client_dart.dart';

class StdServer extends ServerInterface {
  Process? _process;

  @override
  Future<void> connect(
    Function(Uint8List data) onData,
    Function(String? error) onError,
    Function() onDone,
  ) async {
    _process = await Process.start(
      "/Users/rohitsangwan/Drive/Devlopment/c++/synergy_core_clean/build/bin/synergy_flutter",
      [
        "-wi",
        "1920",
        "-hi",
        "1080",
        "-c",
        "/Users/rohitsangwan/Drive/Devlopment/c++/synergy_core_clean/synergy.conf"
      ],
    );

    _process?.stdout.listen((event) {
      String data = utf8.decode(event);
      data.split("\n").forEach((element) {
        if (element.contains("Buffer")) {
          var bytes = element.replaceFirst("Buffer:", "").trim();
          List<String> stringByes = bytes.split(",");
          List<int> intBytes = [];
          for (var e in stringByes) {
            intBytes.add(int.tryParse(e) ?? 0);
          }
          onData(Uint8List.fromList(intBytes));
        } else if (element.isNotEmpty) {
          print("Data: $element");
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
    _process?.stdin.writeln(data);
  }
}
