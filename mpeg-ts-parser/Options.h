#pragma once
#include <string>
#include <vector>

#include "cxxopts.hpp"

struct Options
{
  std::string ts_filename;
  std::vector<int> video_pids;
  std::vector<int> audio_pids;

  [[nodiscard]] static Options parse_options(int argc, char* argv[]);
};
