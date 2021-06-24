#include "PES_Assembler.h"

namespace pes
{
  void Assembler::process_packet()
  {
    const auto data_buffer = packet->get_data_buffer();
    for (const auto& data : data_buffer) assembling_file << data;

    delete packet;
    packet = nullptr;
  }

  Assembler::Assembler(const uint16_t& packet_identifier, const std::string& assembling_filename)
  {
    this->packet_identifier = packet_identifier;

    assembling_file.open(assembling_filename, std::ios::binary);

    if (!assembling_file.is_open())
      throw std::exception("Couldn't open assembling file");
  }

  void Assembler::absorb_ts_packet(const int& idx, const ts::Packet* ts_packet)
  {
    const auto* ts_packet_header = ts_packet->get_header();

    if (packet_identifier != ts_packet_header->get_packet_identifier())
      throw std::exception("TS packet PID is different from PES assembler");

    if (continuity_counter != ts_packet_header->get_continuity_counter())
      throw std::exception("CC corrupted while assembling PES packet");

    if (ts_packet_header->get_payload_unit_start_indicator())
    {
      if (packet)
      {
        const auto* packet_header = packet->get_header();
        const auto& packet_length = packet_header->get_guaranteed_fields()->packet_length;
        const auto& buffer_size = packet->get_buffer_size();

        if (packet_length != 0 && packet_length != buffer_size - PacketHeader::GuaranteedFields::LENGTH)
          throw std::exception("Payload hasn't been fully loaded while assembling PES packet");

        process_packet();
      }

      packet = new Packet{};
    }

    if (packet == nullptr)
      throw std::exception("Didn't get payload unit start indicator to start assembling packet");

    packet->absorb_ts_packet(idx, ts_packet);
    ++continuity_counter;

    auto* packet_header = packet->get_header();
    const auto& buffer_size = packet->get_buffer_size();

    if (!packet_header->has_guaranteed_fields()
      && buffer_size >= PacketHeader::GuaranteedFields::LENGTH)
    {
      packet_header->init_guaranteed_fields();
    }

    if (packet_header->has_guaranteed_fields()
      && packet_header->can_have_extra_fields()
      && !packet_header->has_extra_fields()
      && buffer_size >= PacketHeader::ExtraFields::HEADER_DATA_LENGTH_NEEDED)
    {
      packet_header->init_extra_fields();
    }

    if (packet_header->has_guaranteed_fields()
      && packet_header->can_have_extra_fields()
      && packet_header->has_extra_fields()
      && packet_header->can_have_optional_extra_fields()
      && !packet_header->has_optional_extra_fields())
    {
      const auto& timestamp_flags = packet_header->get_extra_fields()->timestamp_flags;

      if ((timestamp_flags == 0b10
          && buffer_size >= PacketHeader::ExtraFields::OptionalExtraFields::PTS_LENGTH_NEEDED)
        || (timestamp_flags == 0b11
          && buffer_size >= PacketHeader::ExtraFields::OptionalExtraFields::DTS_LENGTH_NEEDED))
      {
        packet_header->init_optional_extra_fields();
      }
    }
  }
}
