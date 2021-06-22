#include "TS_Packet.h"

#include <cstring>
#include <sstream>

namespace ts
{
  Packet::Packet(const uint8_t* buffer)
  {
    memcpy(this->buffer, buffer, LENGTH);

    header = new PacketHeader{buffer};
  }

  std::string Packet::to_string() const
  {
    std::stringstream ss;

    ss << "TS: " << header->to_string();

    return ss.str();
  }
}
