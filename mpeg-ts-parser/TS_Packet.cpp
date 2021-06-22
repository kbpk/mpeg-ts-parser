#include "TS_Packet.h"

#include <cstring>

namespace ts
{
  Packet::Packet(const uint8_t* buffer)
  {
    memcpy(this->buffer, buffer, LENGTH);

    header = new PacketHeader{buffer};
  }
}
