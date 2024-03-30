#pragma once
#include "io/IStream.h"

class MyStream : public synergy::IStream {
public:
  MyStream() {}
  virtual ~MyStream() {}

  virtual void close();
  virtual void *getEventTarget() const;
  virtual UInt32 read(void *buffer, UInt32 n);
  virtual void write(const void *buffer, UInt32 n);
  virtual void flush();
  virtual void shutdownInput();
  virtual void shutdownOutput();
  virtual bool isReady() const;
  virtual UInt32 getSize() const;
};