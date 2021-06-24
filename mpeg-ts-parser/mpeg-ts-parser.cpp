#include <filesystem>
#include <iomanip>
#include <iostream>
#include <sstream>

#include "Options.h"
#include "PES_Assembler.h"
#include "TS_Packet.h"
#include "TS_Parser.h"

int main(int argc, char* argv[])
{
  const auto [ts_filename, video_pids, audio_pids,show_status] = Options::parse_options(argc, argv);

  uintmax_t ts_file_size;

  try
  {
    ts_file_size = std::filesystem::file_size(ts_filename);
  }
  catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  const auto predicted_no_ts_packets = ts_file_size / ts::Packet::LENGTH;

  ts::Parser ts_parser{ts_filename};
  FileLogger logger{ts_filename + ".log"};

  std::vector<pes::Assembler*> assemblers;
  assemblers.reserve(video_pids.size() + audio_pids.size());

  for (const auto& v_pid : video_pids)
    assemblers.push_back(new pes::Assembler{
      static_cast<uint16_t>(v_pid), "PID" + std::to_string(v_pid) + ".264"
    });

  for (const auto& a_pid : audio_pids)
    assemblers.push_back(new pes::Assembler{
      static_cast<uint16_t>(a_pid), "PID" + std::to_string(a_pid) + ".mp2"
    });

  ts::Packet* packet = nullptr;

  std::clog << "\033[?25l"; // hide cursor

  try
  {
    for (int i = 0; ts_parser.has_maybe_next_packet(); ++i)
    {
      packet = ts_parser.next_packet();

      std::stringstream ss;

      auto handle_assembling = [&](pes::Assembler* assembler) -> void
      {
        if (packet->get_header()->get_packet_identifier() ==
          assembler->get_packet_identifier())
          assembler->absorb_ts_packet(i, packet);
      };

      for (auto* assembler : assemblers) handle_assembling(assembler);

      ss << std::setw(10) << std::setfill('0') << i << " "
        << packet->to_string();

      logger.log_line(ss.str());

      if (show_status)
        std::clog << i + 1 << "/" << predicted_no_ts_packets << "\r" << std::flush;

      delete packet;
      packet = nullptr;
    }

    for (const auto* assembler : assemblers) delete assembler;
  }
  catch (const std::exception& e)
  {
    delete packet;
    for (const auto* assembler : assemblers) delete assembler;

    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  std::clog << "\r" << "Completed" << std::endl;
  return EXIT_SUCCESS;
}
