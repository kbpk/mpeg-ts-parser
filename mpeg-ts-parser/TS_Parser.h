#pragma once
#include <cstdint>
#include <fstream>
#include <string>

#include "TS_Packet.h"

namespace ts
{
  class Parser
  {
    std::ifstream ts_file;
    uint8_t* buffer;
  public:
    explicit Parser(const std::string& ts_filename);

    ~Parser()
    {
      delete[] buffer;
    }

    [[nodiscard]] bool has_maybe_next_packet() { return ts_file.peek() != std::ifstream::traits_type::eof(); }
    [[nodiscard]] Packet* next_packet();
  };
}
