#include "PES_PacketHeader.h"
#include "parameters.h"

namespace pes
{
  double PacketHeader::Timestamp::seconds() const
  {
    const double val_seconds = value / static_cast<double>(BASE_CLOCK_FREQUENCY_Hz);

    return val_seconds;
  }

  PacketHeader::Timestamp* PacketHeader::Timestamp::parse(const std::vector<uint8_t>& buffer, int index)
  {
    const uint64_t val = (((static_cast<uint64_t>(buffer[index++]) >> 1) & 0x07) << 29)
      | (static_cast<uint64_t>(buffer[index++]) << 22)
      | (((static_cast<uint64_t>(buffer[index++]) >> 1) & 0x7F) << 15)
      | (static_cast<uint64_t>(buffer[index++]) << 7)
      | ((static_cast<uint64_t>(buffer[index]) >> 1) & 0x7F);

    return new Timestamp{val};
  }

  size_t PacketHeader::ExtraFields::length() const
  {
    return GUARANTEED_LENGTH + header_data_length;
  }

  uint8_t PacketHeader::ExtraFields::parse_timestamp_flags(const std::vector<uint8_t>& buffer)
  {
    const uint8_t flags_value = (buffer[TIMESTAMP_FLAGS_IDX] >> 6) & 0x03;

    return flags_value;
  }

  uint8_t PacketHeader::ExtraFields::parse_header_data_length(const std::vector<uint8_t>& buffer)
  {
    const uint8_t length_data = buffer[HEADER_DATA_LENGTH_IDX];

    return length_data;
  }

  PacketHeader::PacketHeader(const std::vector<uint8_t>& buffer) : buffer(buffer)
  {
  }

  void PacketHeader::init_guaranteed_fields()
  {
    const uint32_t packet_start_code_prefix = (static_cast<uint32_t>(buffer[0]) << 16)
      | (static_cast<uint16_t>(buffer[1]) << 8)
      | buffer[2];
    const uint8_t stream_id = buffer[3];
    const uint16_t packet_length = (static_cast<uint16_t>(buffer[4]) << 8) | buffer[5];

    guaranteed_fields = new GuaranteedFields{packet_start_code_prefix, stream_id, packet_length};
  }

  void PacketHeader::init_extra_fields()
  {
    const auto timestamp_flags = ExtraFields::parse_timestamp_flags(buffer);
    const auto header_data_length = ExtraFields::parse_header_data_length(buffer);

    extra_fields = new ExtraFields{timestamp_flags, header_data_length};
  }

  void PacketHeader::init_optional_extra_fields() const
  {
    const auto& flags = extra_fields->timestamp_flags;

    auto* presentation_timestamp = (flags == 0b10 || flags == 0b11)
                                     ? Timestamp::parse(buffer, ExtraFields::OptionalExtraFields::PTS_START_IDX)
                                     : nullptr;
    auto* decoding_timestamp = (flags == 0b11)
                                 ? Timestamp::parse(buffer, ExtraFields::OptionalExtraFields::DTS_START_IDX)
                                 : nullptr;

    extra_fields->optional_extra_fields = new ExtraFields::OptionalExtraFields{
      presentation_timestamp, decoding_timestamp
    };
  }

  bool PacketHeader::can_have_extra_fields() const
  {
    return
      guaranteed_fields->stream_id != program_stream_map
      && guaranteed_fields->stream_id != padding_stream
      && guaranteed_fields->stream_id != private_stream_2
      && guaranteed_fields->stream_id != ECM
      && guaranteed_fields->stream_id != EMM
      && guaranteed_fields->stream_id != program_stream_directory
      && guaranteed_fields->stream_id != DSMCC_stream
      && guaranteed_fields->stream_id != ITUT_H222_1_type_E;
  }

  bool PacketHeader::can_have_optional_extra_fields() const
  {
    const auto& flags = extra_fields->timestamp_flags;

    return flags == 0b10 || flags == 0b11;
  }
}
