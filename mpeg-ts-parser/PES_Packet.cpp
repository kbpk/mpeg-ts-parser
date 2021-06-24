#include "PES_Packet.h"

#include <iomanip>
#include <sstream>

namespace pes
{
  void Packet::absorb_ts_packet(const int& idx, const ts::Packet* ts_packet)
  {
    ts_packet_buffer.emplace_back(idx, new ts::Packet{ts_packet});

    const auto& payload_length = ts_packet->get_payload_length();
    const auto* ts_buffer = ts_packet->get_buffer();

    for (auto i = ts::Packet::LENGTH - payload_length; i < ts::Packet::LENGTH; ++i)
      buffer.push_back(ts_buffer[i]);
  }

  std::vector<uint8_t> Packet::get_data_buffer() const
  {
    std::vector<uint8_t> data_buffer;

    const auto& buffer_size = buffer.size();
    const auto data_length = buffer_size - header->get_length();

    for (auto i = buffer_size - data_length; i < buffer_size; ++i)
      data_buffer.push_back(buffer[i]);

    return data_buffer;
  }

  std::string Packet::summary_to_string() const
  {
    std::stringstream ss;

    const auto& buffer_size = buffer.size();
    const auto& header_length = header->get_length();
    const auto data_length = buffer_size - header_length;

    ss
      << "PES: PcktLen=" << buffer_size
      << " HeadLen=" << header_length
      << " DataLen=" << data_length;

    return ss.str();
  }

  std::string Packet::ts_packet_buffer_to_string() const
  {
    std::stringstream ss;

    const auto& ts_packet_buffer_size = ts_packet_buffer.size();

    for (auto i = 0; i < ts_packet_buffer_size; ++i)
    {
      const auto& [idx, ts_packet] = ts_packet_buffer[i];

      ss << std::setw(10) << std::setfill('0') << idx << " " << ts_packet->to_string();

      if (i == 0)
      {
        ss << " Assembling Started " << header->to_string();
        if (ts_packet_buffer_size != 1)
          ss << std::endl;
      }
      if ((i != 0 && i == ts_packet_buffer_size - 1) || (i == 0 && ts_packet_buffer_size == 1))
      {
        ss << " Assembling Finished " << summary_to_string();
        break;
      }
      if (i != 0)
      {
        ss << " Assembling Continue " << std::endl;
      }
    }

    return ss.str();
  }
}
