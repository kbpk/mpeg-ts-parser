#pragma once
#include <cstdint>
#include <vector>

#include "PES_PacketHeader.h"
#include "TS_Packet.h"

namespace pes
{
  class Packet
  {
    std::vector<uint8_t> buffer;
    std::vector<std::pair<int, ts::Packet*>> ts_packet_buffer;
    PacketHeader* header = new PacketHeader{buffer};

  public:
    ~Packet()
    {
      for (auto&& [i, ts_packet] : ts_packet_buffer)
        delete ts_packet;

      delete header;
    }

    void absorb_ts_packet(const int& idx, const ts::Packet* ts_packet);

    [[nodiscard]] PacketHeader* get_header() { return header; }
  };
}
