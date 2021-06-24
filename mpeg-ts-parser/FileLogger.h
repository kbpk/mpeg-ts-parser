#pragma once
#include <fstream>
#include <ostream>
#include <string>

class FileLogger
{
  std::ofstream log_file;

public:
  explicit FileLogger(const std::string& log_filename);

  void log_line(const std::string& line);
};
