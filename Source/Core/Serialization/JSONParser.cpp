#include "JSONParser.hpp"

namespace JSONParser
{
  nlohmann::json ParseFile(const fs::path& path)
  {
    IStream file(path);
    nlohmann::json data = nlohmann::json::parse(file);
    return data;
  }
}
