#include "FileLogger.h"

FileLogger::FileLogger(const std::string& log_filename)
{
  log_file.open(log_filename);
  if (!log_file.is_open())
    throw std::exception("Couldn't open log file");
}

void FileLogger::log_line(const std::string& line)
{
  log_file << line << std::endl;
}
