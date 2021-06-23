#include "TS_PacketHeader.h"

#include <exception>
#include <iomanip>
#include <sstream>

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

  PacketHeader::PacketHeader(const PacketHeader* other)
  {
    sync_byte = other->sync_byte;
    transport_error_indicator = other->transport_error_indicator;
    payload_unit_start_indicator=other->payload_unit_start_indicator;
    transport_priority=other->transport_priority;
    packet_identifier = other->packet_identifier;
    transport_scrambling_control = other->transport_scrambling_control;
    adaptation_field_control = other->adaptation_field_control;
    continuity_counter = other->continuity_counter;
  }

  std::string PacketHeader::to_string() const
  {
    std::stringstream ss;

    ss
      << "SB=" << static_cast<unsigned>(sync_byte)
      << " E=" << transport_error_indicator
      << " S=" << payload_unit_start_indicator
      << " P=" << static_cast<unsigned>(transport_priority)
      << " PID=" << std::setw(4) << packet_identifier
      << " TSC=" << static_cast<unsigned>(transport_scrambling_control)
      << " AF=" << static_cast<unsigned>(adaptation_field_control)
      << " CC=" << std::setw(2) << static_cast<unsigned>(continuity_counter);

    return ss.str();
  }
}
