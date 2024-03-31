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
  std::vector<UInt8> byteBuffer(4);
  byteBuffer[0] = static_cast<UInt8>((n >> 24) & 0xff);
  byteBuffer[1] = static_cast<UInt8>((n >> 16) & 0xff);
  byteBuffer[2] = static_cast<UInt8>((n >> 8) & 0xff);
  byteBuffer[3] = static_cast<UInt8>(n & 0xff);
  const UInt8 *originalBuffer = static_cast<const UInt8 *>(buffer);
  byteBuffer.insert(byteBuffer.end(), originalBuffer, originalBuffer + n);
  // std::cout << "BS:" << originalBuffer << "-S:" << n << std::endl;
  std::cout << "BS:";
  for (UInt8 byte : byteBuffer) {
    std::cout << static_cast<int>(byte) << ",";
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