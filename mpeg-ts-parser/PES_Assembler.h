#pragma once
#include <cstdint>
#include <fstream>

#include "PES_Packet.h"

namespace pes
{
  class Assembler
  {
    uint16_t packet_identifier : 13;
    uint8_t continuity_counter : 4 = 0;
    std::ofstream assembling_file;

    Packet* packet = nullptr;

    void process_packet();
  public:
    Assembler(const uint16_t& packet_identifier, const std::string& assembling_filename);

    ~Assembler()
    {
      if (packet)
        process_packet();
    }

    [[nodiscard]] uint16_t get_packet_identifier() const { return packet_identifier; }

    void absorb_ts_packet(const int& idx, const ts::Packet* ts_packet);
  };
}
