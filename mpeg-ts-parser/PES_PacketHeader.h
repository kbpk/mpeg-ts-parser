#pragma once
#include <cstdint>
#include <vector>

namespace pes
{
  class PacketHeader
  {
  public:
    struct Timestamp
    {
      static constexpr size_t LENGTH = 5;

      uint64_t value : 33 = 0;

      [[nodiscard]] double seconds() const;

      [[nodiscard]] static Timestamp* parse(const std::vector<uint8_t>& buffer, int index);
    };

    struct GuaranteedFields
    {
      static constexpr size_t LENGTH = 6;

      uint32_t packet_start_code_prefix : 24;
      uint8_t stream_id;
      uint16_t packet_length;
    };

    struct ExtraFields
    {
      struct OptionalExtraFields
      {
        static constexpr size_t PTS_START_IDX = (GuaranteedFields::LENGTH + 4) - 1;
        static constexpr size_t DTS_START_IDX = (GuaranteedFields::LENGTH + 4) - 1 + Timestamp::LENGTH;

        Timestamp* presentation_timestamp = nullptr;
        Timestamp* decoding_timestamp = nullptr;

        ~OptionalExtraFields()
        {
          delete presentation_timestamp;
          delete decoding_timestamp;
        }
      };

      static constexpr size_t GUARANTEED_LENGTH = 3;
      static constexpr size_t TIMESTAMP_FLAGS_IDX = (GuaranteedFields::LENGTH + 2) - 1;
      static constexpr size_t HEADER_DATA_LENGTH_IDX = (GuaranteedFields::LENGTH + GUARANTEED_LENGTH) - 1;

      uint8_t timestamp_flags : 2;
      uint8_t header_data_length;
      OptionalExtraFields* optional_extra_fields = nullptr;

      ~ExtraFields()
      {
        delete optional_extra_fields;
      }

      [[nodiscard]] size_t length() const;

      [[nodiscard]] static uint8_t parse_timestamp_flags(const std::vector<uint8_t>& buffer);
      [[nodiscard]] static uint8_t parse_header_data_length(const std::vector<uint8_t>& buffer);
    };

    enum StreamId : uint8_t
    {
      program_stream_map = 0xBC,
      padding_stream = 0xBE,
      private_stream_2 = 0xBF,
      ECM = 0xF0,
      EMM = 0xF1,
      program_stream_directory = 0xFF,
      DSMCC_stream = 0xF2,
      ITUT_H222_1_type_E = 0xF8,
    };

  private:
    GuaranteedFields* guaranteed_fields = nullptr;
    ExtraFields* extra_fields = nullptr;

    const std::vector<uint8_t>& buffer;

  public:
    explicit PacketHeader(const std::vector<uint8_t>& buffer);

    ~PacketHeader()
    {
      delete guaranteed_fields;
      delete extra_fields;
    }

    void init_guaranteed_fields();
    void init_extra_fields();
    void init_optional_extra_fields() const;

    [[nodiscard]] bool can_have_extra_fields() const;
    [[nodiscard]] bool can_have_optional_extra_fields() const;

    [[nodiscard]] bool has_guaranteed_fields() const { return guaranteed_fields; }
    [[nodiscard]] bool has_extra_fields() const { return extra_fields; }
    [[nodiscard]] bool has_optional_extra_fields() const { return extra_fields && extra_fields->optional_extra_fields; }
  };
}
