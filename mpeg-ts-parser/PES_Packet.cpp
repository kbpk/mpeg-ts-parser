#include "PES_Packet.h"

namespace pes
{
  void Packet::absorb_ts_packet(const int& idx,
                                const ts::Packet* ts_packet)
  {
    ts_packet_buffer.emplace_back(idx, new ts::Packet{ts_packet});

    const auto& payload_length = ts_packet->get_payload_length();
    const auto* ts_buffer = ts_packet->get_buffer();

    for (auto i = ts::Packet::LENGTH - payload_length; i < ts::Packet::LENGTH; ++i)
      buffer.push_back(ts_buffer[i]);
  }
}
