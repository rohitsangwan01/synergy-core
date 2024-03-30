#include "base/String.h"
#include <MacTypes.h>
#include <iostream>
#include <string>

class MyArgs {
public:
  MyArgs() {}

  SInt32 m_w, m_h = 0;
  String clientName = "flutter";

  bool parse(int argc, char **argv) {
    for (int i = 0; i < argc; i++) {
      if (strcmp(argv[i], "-wi") == 0) {
        m_w = std::stoi(argv[i + 1]);
      }
      if (strcmp(argv[i], "-hi") == 0) {
        m_h = std::stoi(argv[i + 1]);
      }
      if (strcmp(argv[i], "-cname") == 0) {
        clientName = argv[i + 1];
      }
    }

    // if w and h not given throw error
    if (m_w == 0 || m_h == 0) {
      std::cerr << "Width (-wi) or Height(-hi) not provided" << std::endl;
      return false;
    }

    return true;
  }
};