import 'dart:ffi';
import 'package:ffi/ffi.dart';
import 'generated_bindings.dart';

void main(List<String> args) {
  NativeLibrary nativeLibrary = NativeLibrary(DynamicLibrary.open(
    '/Users/rohitsangwan/Drive/Devlopment/c++/synergy_core_clean/build/lib/libapi.dylib',
  ));

  String configFile =
      '/Users/rohitsangwan/Drive/Devlopment/c++/synergy_core_clean/synergy.conf';

  Pointer<Pointer<Char>> argv = calloc<Pointer<Char>>();
  //  add argv -c configFile
  List<String> argList = ['-c $configFile', '-f'];
  for (int i = 0; i < argList.length; i++) {
    argv[i] = argList[i].toNativeUtf8() as Pointer<Char>;
  }
  String argF = (argv[0] as Pointer<Utf8>).toDartString();
  print('Argv: $argF');
  print('Starting server');
  nativeLibrary.startServer(0, argv);

  print('Done');
}
