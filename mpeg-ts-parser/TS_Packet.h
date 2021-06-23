#pragma once
#include <string>

#include "TS_PacketHeader.h"
#include "TS_PacketAdaptationField.h"

namespace ts
{
  class Packet
  {
  public:
    static constexpr size_t LENGTH = 188;

  private:
    PacketHeader* header = nullptr;
    PacketAdaptationField* adaptation_field = nullptr;
    uint8_t* buffer = new uint8_t[LENGTH];

  public:
    explicit Packet(const uint8_t* buffer);
    explicit Packet(const Packet* other);

    ~Packet()
    {
      delete header;
      delete adaptation_field;
      delete[] buffer;
    }

    [[nodiscard]] const PacketHeader* get_header() const { return header; }
    [[nodiscard]] const uint8_t* get_buffer() const { return buffer; }
    [[nodiscard]] size_t get_payload_length() const;

    [[nodiscard]] std::string to_string() const;
  };
}
