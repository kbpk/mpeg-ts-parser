#include "TS_Parser.h"

namespace ts
{
  Parser::Parser(const std::string& ts_filename)
  {
    ts_file.open(ts_filename, std::ios::binary);

    if (!ts_file.is_open())
      throw std::exception("Couldn't open ts file");
  }

  Packet* Parser::next_packet()
  {
    if (ts_file.read(reinterpret_cast<char*>(buffer), Packet::LENGTH))
      return new Packet{buffer};

    throw std::exception("Error while reading ts file");
  }
}
