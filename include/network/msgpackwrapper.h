/*
 * Copyright 2017 CodiLime
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#ifndef VELES_MSGPACK_H
#define VELES_MSGPACK_H

#include <msgpack.hpp>
#include <QtNetwork/QTcpSocket>

#include "network/msgpackobject.h"
#include "proto/exceptions.h"
#include "messages.h"

namespace veles {
namespace messages {

MSGPACK_CLASSES_HEADER

class MsgpackWrapper {
  msgpack::unpacker unp;
  static const int read_size = 1024;

 public:
  static std::shared_ptr<MsgpackMsg> parseMessage(msgpack::object_handle *handle) {
    msgpack::object obj = handle->get();
    return MsgpackMsg::polymorphicLoad(obj);
  }

  template <class Packer, class T>
  static void dumpObject(Packer& pk, std::shared_ptr<T> ptr) {
    auto mss = toMsgpackObject(ptr);
    pk.pack(mss);
  }

  std::shared_ptr<MsgpackMsg> loadMessage(QTcpSocket *connection) {
    // This method can throw msgpack::type_error when malformed message is read
    msgpack::object_handle handle;
    if (unp.next(handle)) {
      return parseMessage(&handle);
    }
    unp.reserve_buffer(read_size);
    qint64 read = connection->read(unp.buffer(), read_size);
    if (read == -1) {
        return nullptr;
    }
    unp.buffer_consumed(read);
    if (unp.next(handle)) {
      return parseMessage(&handle);
    } else {
      return nullptr;
    }
  }
};

}  // namespace messages
}  // namespace veles

#endif // VELES_MSGPACK_H
