#include "TS_PacketHeader.h"

#include <exception>

namespace ts
{
  PacketHeader::PacketHeader(const uint8_t* buffer)
  {
    sync_byte = buffer[0];
    if (sync_byte != SYNC_BYTE_CORRECT)
      throw std::exception("Sync byte value is not valid");
    transport_error_indicator = buffer[1] & 0x80;
    payload_unit_start_indicator = buffer[1] & 0x40;
    transport_priority = (buffer[1] >> 5) & 0x01;
    packet_identifier = ((static_cast<uint16_t>(buffer[1]) << 8) | buffer[2]) & 0x1FFF;
    transport_scrambling_control = buffer[3] >> 6;
    adaptation_field_control = (buffer[3] >> 4) & 0x03;
    continuity_counter = buffer[3] & 0x0F;
  }
}
