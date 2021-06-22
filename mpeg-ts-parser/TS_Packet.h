﻿#pragma once
#include "TS_PacketHeader.h"

namespace ts
{
  class Packet
  {
  public:
    static constexpr size_t LENGTH = 188;

  private:
    PacketHeader* header = nullptr;
    uint8_t* buffer = new uint8_t[LENGTH];

  public:
    explicit Packet(const uint8_t* buffer);

    ~Packet()
    {
      delete header;
      delete[] buffer;
    }

    [[nodiscard]] const PacketHeader* get_header() const { return header; }
    [[nodiscard]] const uint8_t* get_buffer() const { return buffer; }
  };
}