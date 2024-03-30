#include "MyStream.h"
#include "base/Log.h"
#include <cstdio>
#include <iostream>

// A proxy Stream class
void MyStream::close() {
  //  LOG((CLOG_INFO "MyStream::close()"));
}

void *MyStream::getEventTarget() const {
  // LOG((CLOG_INFO "MyStream::getEventTarget()"));
  return const_cast<void *>(static_cast<const void *>(this));
}

UInt32 MyStream::read(void *buffer, UInt32 n) {
  LOG((CLOG_INFO "MyStream::read() %s %n", buffer, n));
  return 0;
}

void MyStream::write(const void *buffer, UInt32 n) {
  // write buffer as list int
  const UInt8 *byteBuffer = static_cast<const UInt8 *>(buffer);
  std::vector<UInt8> bufferData(byteBuffer, byteBuffer + n);
  std::cout << "Buffer:0,0,0," << n << ",";
  for (UInt8 byte : bufferData) {
    if (byte == bufferData.back())
      std::cout << static_cast<int>(byte);
    else
      std::cout << static_cast<int>(byte) << ',';
  }
  std::cout << std::endl;
}

void MyStream::flush() {
  // LOG((CLOG_INFO "MyStream::flush()"));
}

void MyStream::shutdownInput() {
  // LOG((CLOG_INFO "MyStream::shutdownInput()"));
}

void MyStream::shutdownOutput() {
  // LOG((CLOG_INFO "MyStream::shutdownOutput()"));
}

bool MyStream::isReady() const {
  // LOG((CLOG_INFO "MyStream::isReady()"));
  return true;
}

UInt32 MyStream::getSize() const {
  // LOG((CLOG_INFO "MyStream::getSize()"));
  return 0; // Replace with actual implementation
}