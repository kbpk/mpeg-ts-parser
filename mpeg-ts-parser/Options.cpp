#include "Options.h"

Options Options::parse_options(int argc, char* argv[])
{
  std::string ts_filename;
  std::vector<int> video_pids;
  std::vector<int> audio_pids;
  bool show_status;

  try
  {
    cxxopts::Options options("mpeg-ts-parser");

    options.add_options()
      ("i,input", "TS file", cxxopts::value<std::string>())
      ("v,video_pid", "Video PID to extract", cxxopts::value<std::vector<int>>())
      ("a,audio_pid", "Audio PID to extract", cxxopts::value<std::vector<int>>())
      ("s,show_status", "Show status", cxxopts::value<bool>()->default_value("false"))
      ("h,help", "Print usage");

    const auto options_result = options.parse(argc, argv);

    if (options_result.count("help"))
    {
      std::cout << options.help() << std::endl;
      exit(EXIT_SUCCESS);
    }

    if (!options_result.count("input"))
    {
      throw std::exception("Input TS file must be specified");
    }

    ts_filename = options_result["input"].as<std::string>();

    if (options_result.count("video_pid"))
      video_pids = options_result["video_pid"].as<std::vector<int>>();

    if (options_result.count("audio_pid"))
      audio_pids = options_result["audio_pid"].as<std::vector<int>>();

    show_status = options_result["show_status"].as<bool>();
  }
  catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
    exit(EXIT_FAILURE);
  }

  return {ts_filename, video_pids, audio_pids, show_status};
}
