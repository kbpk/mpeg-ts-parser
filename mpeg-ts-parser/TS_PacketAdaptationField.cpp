#include "TS_PacketAdaptationField.h"

#include <iomanip>
#include <sstream>

#include "parameters.h"

namespace ts
{
  uint64_t PacketAdaptationField::ClockReference::value() const
  {
    const uint64_t val = base * BASE_TO_EXTERNAL_CLOCK_MULTIPLIER + extension;
    return val;
  }

  double PacketAdaptationField::ClockReference::seconds() const
  {
    const auto val = value();
    const double valSeconds = val / static_cast<double>(EXTERNAL_CLOCK_FREQUENCY_Hz);
    return valSeconds;
  }

  PacketAdaptationField::ClockReference* PacketAdaptationField::ClockReference::parse(const uint8_t* buffer, int index)
  {
    uint64_t base = static_cast<uint64_t>(buffer[index++]) << 25;
    base |= static_cast<uint64_t>(buffer[index++]) << 17;
    base |= static_cast<uint64_t>(buffer[index++]) << 9;
    base |= static_cast<uint64_t>(buffer[index++]) << 1;
    base |= buffer[index] >> 7;

    uint16_t extension = static_cast<uint16_t>(buffer[index++] & 0x01) << 8;
    extension |= buffer[index];

    return new ClockReference{base, extension};
  }

  PacketAdaptationField::ClockReference* PacketAdaptationField::parse_program_clock_reference() const
  {
    if (!program_clock_reference_flag)
      return nullptr;

    if (const auto& pid = header->packet_identifier;
      pid != PacketHeader::PacketIdentifier::PAT
      && pid != PacketHeader::PacketIdentifier::CAT
      && !(pid >= PacketHeader::PacketIdentifier::NIT && pid < PacketHeader::PacketIdentifier::NuLL))
    {
      throw std::exception(" PID not allowed to carry a PCR");
    }

    const auto index = CLOCK_REFERENCE_START_INDEX;

    return ClockReference::parse(buffer, index);
  }

  PacketAdaptationField::ClockReference* PacketAdaptationField::parse_original_program_clock_reference() const
  {
    if (!original_program_clock_reference_flag)
      return nullptr;

    if (!program_clock_reference_flag)
      throw std::exception("A OPCR field shall be coded only if the PCR field is present");

    const auto index = CLOCK_REFERENCE_START_INDEX
      + (program_clock_reference_flag ? ClockReference::LENGTH : 0);

    return ClockReference::parse(buffer, index);
  }

  uint8_t PacketAdaptationField::parse_transport_private_data_length() const
  {
    if (!transport_private_data_flag)
      return 0;

    const auto index = CLOCK_REFERENCE_START_INDEX
      + (program_clock_reference_flag ? ClockReference::LENGTH : 0)
      + (original_program_clock_reference_flag ? ClockReference::LENGTH : 0)
      + (splicing_point_flag ? 1 : 0);

    return buffer[index];
  }

  uint8_t PacketAdaptationField::parse_adaptation_field_extension_length() const
  {
    if (!adaptation_field_extension_flag)
      return 0;

    const int index = CLOCK_REFERENCE_START_INDEX
      + (program_clock_reference_flag ? ClockReference::LENGTH : 0)
      + (original_program_clock_reference_flag ? ClockReference::LENGTH : 0)
      + (splicing_point_flag ? 1 : 0)
      + (transport_private_data_flag ? 1 : 0)
      + transport_private_data_length;

    return buffer[index];
  }

  size_t PacketAdaptationField::count_stuffing_bytes() const
  {
    int length = adaptation_field_length;

    if (length > 0)
    {
      length -= 1; // indicators & flags
      if (program_clock_reference_flag) length -= ClockReference::LENGTH;
      if (original_program_clock_reference_flag) length -= ClockReference::LENGTH;
      if (splicing_point_flag) length -= 1;
      if (transport_private_data_flag) length -= 1;
      length -= transport_private_data_length;
      if (adaptation_field_extension_flag) length -= 1;
      length -= adaptation_field_extension_length;
    }

    if (length < 0)
      throw std::exception("Stuffing bytes count cannot be < 0");

    return length;
  }

  PacketAdaptationField::PacketAdaptationField(const uint8_t* buffer, const PacketHeader* header)
  {
    this->buffer = buffer;
    this->header = header;

    const auto& afc = this->header->adaptation_field_control;

    adaptation_field_length = this->buffer[4];

    if (afc == 0b10 && adaptation_field_extension_length != 183)
      throw std::exception("When the adaptation_field_control is '10' af length shall be 183");

    if (afc == 0b11 && !(adaptation_field_length > 0 && adaptation_field_length <= 182))
      throw std::exception("When the adaptation_field_control is '11' af length shall be in the range 0 to 182");

    length = 1ull + adaptation_field_length;

    discontinuity_indicator = this->buffer[5] & 0x80;
    random_access_indicator = this->buffer[5] & 0x40;
    elementary_stream_priority_indicator = this->buffer[5] & 0x20;
    program_clock_reference_flag = this->buffer[5] & 0x10;
    original_program_clock_reference_flag = this->buffer[5] & 0x08;
    splicing_point_flag = this->buffer[5] & 0x04;
    transport_private_data_flag = this->buffer[5] & 0x02;
    adaptation_field_extension_flag = this->buffer[5] & 0x01;

    program_clock_reference = parse_program_clock_reference();
    original_program_clock_reference = parse_original_program_clock_reference();

    transport_private_data_length = parse_transport_private_data_length();
    adaptation_field_extension_length = parse_adaptation_field_extension_length();

    stuffing_bytes_count = count_stuffing_bytes();
  }

  PacketAdaptationField::PacketAdaptationField(const uint8_t* buffer, const PacketHeader* header,
                                               const PacketAdaptationField* other)
  {
    this->buffer = buffer;
    this->header = header;
    length = other->length;
    adaptation_field_length = other->adaptation_field_length;
    discontinuity_indicator = other->discontinuity_indicator;
    random_access_indicator = other->random_access_indicator;
    elementary_stream_priority_indicator = other->elementary_stream_priority_indicator;
    program_clock_reference_flag = other->program_clock_reference_flag;
    original_program_clock_reference_flag = other->original_program_clock_reference_flag;
    splicing_point_flag = other->splicing_point_flag;
    transport_private_data_flag = other->transport_private_data_flag;
    adaptation_field_extension_flag = other->adaptation_field_extension_flag;
    transport_private_data_length = other->transport_private_data_length;
    adaptation_field_extension_length = other->adaptation_field_extension_length;
    stuffing_bytes_count = other->stuffing_bytes_count;

    if (const auto* pcr = other->program_clock_reference; pcr)
      program_clock_reference = new ClockReference{pcr->base, pcr->extension};

    if (const auto* opcr = other->original_program_clock_reference; opcr)
      original_program_clock_reference = new ClockReference{opcr->base, opcr->extension};
  }

  std::string PacketAdaptationField::to_string() const
  {
    std::stringstream ss;

    ss
      << "L=" << std::setw(3) << static_cast<unsigned>(adaptation_field_length)
      << " DC=" << discontinuity_indicator
      << " RA=" << random_access_indicator
      << " SP=" << elementary_stream_priority_indicator
      << " PR=" << program_clock_reference_flag
      << " OR=" << original_program_clock_reference_flag
      << " SP=" << splicing_point_flag
      << " TP=" << transport_private_data_flag
      << " EX=" << adaptation_field_extension_flag;

    if (program_clock_reference_flag)
    {
      const auto pcr = program_clock_reference->value();
      const auto pcrSeconds = program_clock_reference->seconds();
      ss << " PCR=" << pcr << " (Time=" << pcrSeconds << "s)";
    }

    if (original_program_clock_reference_flag)
    {
      const auto opcr = original_program_clock_reference->value();
      const auto opcrSeconds = original_program_clock_reference->seconds();
      ss << " OPCR=" << opcr << " (Time=" << opcrSeconds << "s)";
    }

    ss << " Stuffing=" << stuffing_bytes_count;

    return ss.str();
  }
}
