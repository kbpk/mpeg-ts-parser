#include "TS_Packet.h"

#include <cstring>
#include <sstream>

namespace ts
{
  Packet::Packet(const uint8_t* buffer)
  {
    memcpy(this->buffer, buffer, LENGTH);

    header = new PacketHeader{buffer};

    const auto& afc = header->adaptation_field_control;
    adaptation_field = afc == 0b10 || afc == 0b11 ? new PacketAdaptationField{this->buffer, header} : nullptr;
  }

  std::string Packet::to_string() const
  {
    std::stringstream ss;

    ss << "TS: " << header->to_string();
    if (adaptation_field)
      ss << " AF: " << adaptation_field->to_string();

    return ss.str();
  }
}
