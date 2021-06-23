#pragma once
#include <cstdint>
#include <string>

namespace ts
{
  class PacketHeader
  {
    friend class Packet;
    friend class PacketAdaptationField;
  public:
    enum PacketIdentifier : uint16_t
    {
      PAT = 0x0000,
      CAT = 0x0001,
      TSDT = 0x0002,
      IPMT = 0x0003,
      // DVB specific PID
      NIT = 0x0010,
      // DVB specific PID
      SDT = 0x0011,
      NuLL = 0x1FFF
    };

    static constexpr size_t LENGTH = 4;
    static constexpr uint8_t SYNC_BYTE_CORRECT = 0x47;

  private:
    uint8_t sync_byte;
    bool transport_error_indicator;
    bool payload_unit_start_indicator;
    uint8_t transport_priority : 1;
    uint16_t packet_identifier : 13;
    uint8_t transport_scrambling_control : 2;
    uint8_t adaptation_field_control : 2;
    uint8_t continuity_counter : 4;

  public:
    explicit PacketHeader(const uint8_t* buffer);
    explicit PacketHeader(const PacketHeader* other);

    [[nodiscard]] uint16_t get_packet_identifier() const { return packet_identifier; }
    [[nodiscard]] bool get_payload_unit_start_indicator() const { return payload_unit_start_indicator; }
    [[nodiscard]] uint8_t get_continuity_counter() const { return continuity_counter; }

    [[nodiscard]] std::string to_string() const;
  };
}
