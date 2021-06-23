#pragma once
#include <cstdint>

#include "TS_PacketHeader.h"

namespace ts
{
  class PacketAdaptationField
  {
  public:
    struct ClockReference
    {
      static constexpr size_t LENGTH = (33 + 6 + 9) / 8; // bytes

      uint64_t base : 33;
      uint16_t extension : 9;

      [[nodiscard]] uint64_t value() const;
      [[nodiscard]] double seconds() const;

      [[nodiscard]] static ClockReference* parse(const uint8_t* buffer, int index);
    };

  private:
    // 4 (header) + 1 (af length) + 1 (af flags) // next will be 7th byte -> idx 6
    static constexpr int CLOCK_REFERENCE_START_INDEX = 6;

    const uint8_t* buffer;
    const PacketHeader* header;

    size_t length;

    uint8_t adaptation_field_length;
    bool discontinuity_indicator;
    bool random_access_indicator;
    bool elementary_stream_priority_indicator;
    bool program_clock_reference_flag;
    bool original_program_clock_reference_flag;
    bool splicing_point_flag;
    bool transport_private_data_flag;
    bool adaptation_field_extension_flag;

    ClockReference* program_clock_reference = nullptr;
    ClockReference* original_program_clock_reference = nullptr;
    uint8_t transport_private_data_length;
    uint8_t adaptation_field_extension_length;

    size_t stuffing_bytes_count;

    [[nodiscard]] ClockReference* parse_program_clock_reference() const;
    [[nodiscard]] ClockReference* parse_original_program_clock_reference() const;
    [[nodiscard]] uint8_t parse_transport_private_data_length() const;
    [[nodiscard]] uint8_t parse_adaptation_field_extension_length() const;

    [[nodiscard]] size_t count_stuffing_bytes() const;

  public:
    PacketAdaptationField(const uint8_t* buffer, const PacketHeader* header);

    ~PacketAdaptationField()
    {
      delete program_clock_reference;
      delete original_program_clock_reference;
    }

    [[nodiscard]] std::string to_string() const;
  };
}
