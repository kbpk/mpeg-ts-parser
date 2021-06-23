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

  Packet::Packet(const Packet* other)
  {
    memcpy(buffer, other->buffer, LENGTH);

    header = new PacketHeader{other->header};
    adaptation_field = other->adaptation_field
                         ? new PacketAdaptationField{buffer, header, other->adaptation_field}
                         : nullptr;
  }

  size_t Packet::get_payload_length() const
  {
    const int payload_length = static_cast<int>(Packet::LENGTH)
      - PacketHeader::LENGTH
      - (adaptation_field ? adaptation_field->length : 0);

    if (payload_length < 0)
      throw std::exception("TS packet payload length cannot be < 0");

    return payload_length;
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
