#include "DummyClientProxy.h"

#include "base/IEventQueue.h"
#include "base/Log.h"
#include "base/TMethodEventJob.h"
#include "io/IStream.h"
#include "synergy/ProtocolUtil.h"
#include "synergy/XSynergy.h"

#include <cstdio>
#include <cstring>
#include <iostream>

DummyClientProxy::DummyClientProxy(const String &name, synergy::IStream *stream,
                                   IEventQueue *events)
    : ClientProxy(name, stream), m_heartbeatTimer(NULL),
      m_parser(&DummyClientProxy::parseHandshakeMessage), m_events(events) {

  // install event handlers
  m_events->adoptHandler(m_events->forIStream().inputReady(),
                         stream->getEventTarget(),
                         new TMethodEventJob<DummyClientProxy>(
                             this, &DummyClientProxy::handleData, NULL));
  m_events->adoptHandler(m_events->forIStream().outputError(),
                         stream->getEventTarget(),
                         new TMethodEventJob<DummyClientProxy>(
                             this, &DummyClientProxy::handleWriteError, NULL));
  m_events->adoptHandler(m_events->forIStream().inputShutdown(),
                         stream->getEventTarget(),
                         new TMethodEventJob<DummyClientProxy>(
                             this, &DummyClientProxy::handleDisconnect, NULL));
  m_events->adoptHandler(m_events->forIStream().outputShutdown(),
                         stream->getEventTarget(),
                         new TMethodEventJob<DummyClientProxy>(
                             this, &DummyClientProxy::handleWriteError, NULL));
  m_events->adoptHandler(Event::kTimer, this,
                         new TMethodEventJob<DummyClientProxy>(
                             this, &DummyClientProxy::handleFlatline, NULL));

  setHeartbeatRate(kHeartRate, kHeartRate * kHeartBeatsUntilDeath);

  LOG((CLOG_INFO "querying client \"%s\" info", getName().c_str()));
  ProtocolUtil::writef(getStream(), kMsgQInfo);
  setDeviceInfo();

  // while (!std::cin.eof()) {
  //  unsigned int len = 0;
  //  std::cin.read(reinterpret_cast<char *>(&len), 4);
  //  if (len > 0) {
  //    char *msgBuf = new char[len];
  //    std::cin.read(msgBuf, len);
  //    std::cout << "Received: " << msgBuf << std::endl;
  //  }
  // }
}

DummyClientProxy::~DummyClientProxy() { removeHandlers(); }

void DummyClientProxy::disconnect() {
  removeHandlers();
  getStream()->close();
  m_events->addEvent(
      Event(m_events->forClientProxy().disconnected(), getEventTarget()));
}

void DummyClientProxy::removeHandlers() {
  // uninstall event handlers
  m_events->removeHandler(m_events->forIStream().inputReady(),
                          getStream()->getEventTarget());
  m_events->removeHandler(m_events->forIStream().outputError(),
                          getStream()->getEventTarget());
  m_events->removeHandler(m_events->forIStream().inputShutdown(),
                          getStream()->getEventTarget());
  m_events->removeHandler(m_events->forIStream().outputShutdown(),
                          getStream()->getEventTarget());
  m_events->removeHandler(Event::kTimer, this);

  // remove timer
  removeHeartbeatTimer();
}

void DummyClientProxy::addHeartbeatTimer() {
  if (m_heartbeatAlarm > 0.0) {
    m_heartbeatTimer = m_events->newOneShotTimer(m_heartbeatAlarm, this);
  }
}

void DummyClientProxy::removeHeartbeatTimer() {
  if (m_heartbeatTimer != NULL) {
    m_events->deleteTimer(m_heartbeatTimer);
    m_heartbeatTimer = NULL;
  }
}

void DummyClientProxy::resetHeartbeatTimer() {
  // reset the alarm
  removeHeartbeatTimer();
  addHeartbeatTimer();
}

void DummyClientProxy::resetHeartbeatRate() {
  setHeartbeatRate(kHeartRate, kHeartRate * kHeartBeatsUntilDeath);
}

void DummyClientProxy::setHeartbeatRate(double, double alarm) {
  m_heartbeatAlarm = alarm;
}

void DummyClientProxy::handleData(const Event &, void *) {
  // handle messages until there are no more.  first read message code.
  UInt8 code[4];
  UInt32 n = getStream()->read(code, 4);
  while (n != 0) {
    // verify we got an entire code
    if (n != 4) {
      LOG((CLOG_ERR "incomplete message from \"%s\": %d bytes",
           getName().c_str(), n));
      disconnect();
      return;
    }

    // parse message
    LOG((CLOG_INFO "msg from \"%s\": %c%c%c%c", getName().c_str(), code[0],
         code[1], code[2], code[3]));
    if (!(this->*m_parser)(code)) {
      LOG((CLOG_ERR "invalid message from client \"%s\": %c%c%c%c",
           getName().c_str(), code[0], code[1], code[2], code[3]));
      while (getStream()->read(nullptr, 4))
        ;
    }

    // next message
    n = getStream()->read(code, 4);
  }

  // restart heartbeat timer
  resetHeartbeatTimer();
}

bool DummyClientProxy::parseHandshakeMessage(const UInt8 *code) {
  if (memcmp(code, kMsgCNoop, 4) == 0) {
    // discard no-ops
    LOG((CLOG_INFO "no-op from", getName().c_str()));
    return true;
  } else if (memcmp(code, kMsgDInfo, 4) == 0) {
    // future messages get parsed by parseMessage
    m_parser = &DummyClientProxy::parseMessage;
    if (recvInfo()) {
      m_events->addEvent(
          Event(m_events->forClientProxy().ready(), getEventTarget()));
      addHeartbeatTimer();
      return true;
    }
  }
  return false;
}

bool DummyClientProxy::parseMessage(const UInt8 *code) {
  if (memcmp(code, kMsgDInfo, 4) == 0) {
    if (recvInfo()) {
      m_events->addEvent(
          Event(m_events->forIScreen().shapeChanged(), getEventTarget()));
      return true;
    }
    return false;
  } else if (memcmp(code, kMsgCNoop, 4) == 0) {
    // discard no-ops
    LOG((CLOG_INFO "no-op from", getName().c_str()));
    return true;
  } else if (memcmp(code, kMsgCClipboard, 4) == 0) {
    return recvGrabClipboard();
  } else if (memcmp(code, kMsgDClipboard, 4) == 0) {
    return recvClipboard();
  }
  return false;
}

void DummyClientProxy::handleDisconnect(const Event &, void *) {
  LOG((CLOG_NOTE "client \"%s\" has disconnected", getName().c_str()));
  disconnect();
}

void DummyClientProxy::handleWriteError(const Event &, void *) {
  LOG((CLOG_WARN "error writing to client \"%s\"", getName().c_str()));
  disconnect();
}

void DummyClientProxy::handleFlatline(const Event &, void *) {
  // didn't get a heartbeat fast enough.  assume client is dead.
  LOG((CLOG_NOTE "client \"%s\" is dead", getName().c_str()));
  disconnect();
}

bool DummyClientProxy::getClipboard(ClipboardID id,
                                    IClipboard *clipboard) const {
  Clipboard::copy(clipboard, &m_clipboard[id].m_clipboard);
  return true;
}

void DummyClientProxy::getShape(SInt32 &x, SInt32 &y, SInt32 &w,
                                SInt32 &h) const {
  x = m_info.m_x;
  y = m_info.m_y;
  w = m_info.m_w;
  h = m_info.m_h;
}

void DummyClientProxy::getCursorPos(SInt32 &x, SInt32 &y) const {
  // note -- this returns the cursor pos from when we last got client info
  x = m_info.m_mx;
  y = m_info.m_my;
}

void DummyClientProxy::enter(SInt32 xAbs, SInt32 yAbs, UInt32 seqNum,
                             KeyModifierMask mask, bool) {
  LOG((CLOG_INFO "send enter to \"%s\", %d,%d %d %04x", getName().c_str(), xAbs,
       yAbs, seqNum, mask));
  ProtocolUtil::writef(getStream(), kMsgCEnter, xAbs, yAbs, seqNum, mask);
}

bool DummyClientProxy::leave() {
  LOG((CLOG_INFO "send leave to \"%s\"", getName().c_str()));
  ProtocolUtil::writef(getStream(), kMsgCLeave);

  // we can never prevent the user from leaving
  return true;
}

void DummyClientProxy::setClipboard(ClipboardID id,
                                    const IClipboard *clipboard) {
  // ignore -- deprecated in protocol 1.0
}

void DummyClientProxy::grabClipboard(ClipboardID id) {
  LOG((CLOG_INFO "send grab clipboard %d to \"%s\"", id, getName().c_str()));
  ProtocolUtil::writef(getStream(), kMsgCClipboard, id, 0);

  // this clipboard is now dirty
  m_clipboard[id].m_dirty = true;
}

void DummyClientProxy::setClipboardDirty(ClipboardID id, bool dirty) {
  m_clipboard[id].m_dirty = dirty;
}

void DummyClientProxy::keyDown(KeyID key, KeyModifierMask mask, KeyButton,
                               const String &) {
  LOG((CLOG_INFO "send key down to \"%s\" id=%d, mask=0x%04x",
       getName().c_str(), key, mask));
  ProtocolUtil::writef(getStream(), kMsgDKeyDown1_0, key, mask);
}

void DummyClientProxy::keyRepeat(KeyID key, KeyModifierMask mask, SInt32 count,
                                 KeyButton, const String &) {
  LOG((CLOG_INFO "send key repeat to \"%s\" id=%d, mask=0x%04x, count=%d",
       getName().c_str(), key, mask, count));
  ProtocolUtil::writef(getStream(), kMsgDKeyRepeat1_0, key, mask, count);
}

void DummyClientProxy::keyUp(KeyID key, KeyModifierMask mask, KeyButton) {
  LOG((CLOG_INFO "send key up to \"%s\" id=%d, mask=0x%04x", getName().c_str(),
       key, mask));
  ProtocolUtil::writef(getStream(), kMsgDKeyUp1_0, key, mask);
}

void DummyClientProxy::mouseDown(ButtonID button) {
  LOG((CLOG_INFO "send mouse down to \"%s\" id=%d", getName().c_str(), button));
  ProtocolUtil::writef(getStream(), kMsgDMouseDown, button);
}

void DummyClientProxy::mouseUp(ButtonID button) {
  LOG((CLOG_INFO "send mouse up to \"%s\" id=%d", getName().c_str(), button));
  ProtocolUtil::writef(getStream(), kMsgDMouseUp, button);
}

void DummyClientProxy::mouseMove(SInt32 xAbs, SInt32 yAbs) {
  LOG((CLOG_INFO "send mouse move to \"%s\" %d,%d", getName().c_str(), xAbs,
       yAbs));
  ProtocolUtil::writef(getStream(), kMsgDMouseMove, xAbs, yAbs);
}

void DummyClientProxy::mouseRelativeMove(SInt32, SInt32) {
  // ignore -- not supported in protocol 1.0
}

void DummyClientProxy::mouseWheel(SInt32, SInt32 yDelta) {
  // clients prior to 1.3 only support the y axis
  LOG((CLOG_INFO "send mouse wheel to \"%s\" %+d", getName().c_str(), yDelta));
  ProtocolUtil::writef(getStream(), kMsgDMouseWheel1_0, yDelta);
}

void DummyClientProxy::sendDragInfo(UInt32 fileCount, const char *info,
                                    size_t size) {
  // ignore -- not supported in protocol 1.0
  LOG((CLOG_INFO "draggingInfoSending not supported"));
}

void DummyClientProxy::fileChunkSending(UInt8 mark, char *data,
                                        size_t dataSize) {
  // ignore -- not supported in protocol 1.0
  LOG((CLOG_INFO "fileChunkSending not supported"));
}

String DummyClientProxy::getSecureInputApp() const {
  // ignore -- not supported on clients
  LOG((CLOG_INFO "getSecureInputApp not supported"));
  return "";
}

void DummyClientProxy::secureInputNotification(const String &app) const {
  // ignore -- not supported in protocol 1.0
  LOG((CLOG_INFO "secureInputNotification not supported"));
}

void DummyClientProxy::screensaver(bool on) {
  LOG((CLOG_INFO "send screen saver to \"%s\" on=%d", getName().c_str(),
       on ? 1 : 0));
  ProtocolUtil::writef(getStream(), kMsgCScreenSaver, on ? 1 : 0);
}

void DummyClientProxy::resetOptions() {
  LOG((CLOG_INFO "send reset options to \"%s\"", getName().c_str()));
  ProtocolUtil::writef(getStream(), kMsgCResetOptions);

  // reset heart rate and death
  resetHeartbeatRate();
  removeHeartbeatTimer();
  addHeartbeatTimer();
}

void DummyClientProxy::setOptions(const OptionsList &options) {
  LOG((CLOG_INFO "send set options to \"%s\" size=%d", getName().c_str(),
       options.size()));
  ProtocolUtil::writef(getStream(), kMsgDSetOptions, &options);

  // check options
  for (UInt32 i = 0, n = (UInt32)options.size(); i < n; i += 2) {
    if (options[i] == kOptionHeartbeat) {
      double rate = 1.0e-3 * static_cast<double>(options[i + 1]);
      if (rate <= 0.0) {
        rate = -1.0;
      }
      setHeartbeatRate(rate, rate * kHeartBeatsUntilDeath);
      removeHeartbeatTimer();
      addHeartbeatTimer();
    }
  }
}

bool DummyClientProxy::setDeviceInfo() {
  SInt16 x = 0;
  SInt16 y = 0;
  SInt16 w = 1920;
  SInt16 h = 1080;
  SInt16 mx = 960;
  SInt16 my = 540;

  LOG((CLOG_INFO "received client \"%s\" info shape=%d,%d %dx%d at %d,%d",
       getName().c_str(), x, y, w, h, mx, my));

  // validate
  if (w <= 0 || h <= 0) {
    return false;
  }
  if (mx < x || mx >= x + w || my < y || my >= y + h) {
    mx = x + w / 2;
    my = y + h / 2;
  }

  // save
  m_info.m_x = x;
  m_info.m_y = y;
  m_info.m_w = w;
  m_info.m_h = h;
  m_info.m_mx = mx;
  m_info.m_my = my;

  // acknowledge receipt
  LOG((CLOG_INFO "send info ack to \"%s\"", getName().c_str()));
  ProtocolUtil::writef(getStream(), kMsgCInfoAck);
  return true;
}

bool DummyClientProxy::recvInfo() {
  LOG((CLOG_INFO "received client info, Parsing.."));

  // parse the message
  SInt16 x, y, w, h, dummy1, mx, my;
  if (!ProtocolUtil::readf(getStream(), kMsgDInfo + 4, &x, &y, &w, &h, &dummy1,
                           &mx, &my)) {
    return false;
  }
  LOG((CLOG_INFO "received client \"%s\" info shape=%d,%d %dx%d at %d,%d",
       getName().c_str(), x, y, w, h, mx, my));

  // validate
  if (w <= 0 || h <= 0) {
    return false;
  }
  if (mx < x || mx >= x + w || my < y || my >= y + h) {
    mx = x + w / 2;
    my = y + h / 2;
  }

  // save
  m_info.m_x = x;
  m_info.m_y = y;
  m_info.m_w = w;
  m_info.m_h = h;
  m_info.m_mx = mx;
  m_info.m_my = my;

  // acknowledge receipt
  LOG((CLOG_INFO "send info ack to \"%s\"", getName().c_str()));
  ProtocolUtil::writef(getStream(), kMsgCInfoAck);
  return true;
}

bool DummyClientProxy::recvClipboard() {
  // deprecated in protocol 1.0
  return false;
}

bool DummyClientProxy::recvGrabClipboard() {
  // parse message
  ClipboardID id;
  UInt32 seqNum;
  if (!ProtocolUtil::readf(getStream(), kMsgCClipboard + 4, &id, &seqNum)) {
    return false;
  }
  LOG((CLOG_INFO "received client \"%s\" grabbed clipboard %d seqnum=%d",
       getName().c_str(), id, seqNum));

  // validate
  if (id >= kClipboardEnd) {
    return false;
  }

  // notify
  ClipboardInfo *info = new ClipboardInfo;
  info->m_id = id;
  info->m_sequenceNumber = seqNum;
  m_events->addEvent(Event(m_events->forClipboard().clipboardGrabbed(),
                           getEventTarget(), info));

  return true;
}

DummyClientProxy::ClientClipboard::ClientClipboard()
    : m_clipboard(), m_sequenceNumber(0), m_dirty(true) {
  // do nothing
}
