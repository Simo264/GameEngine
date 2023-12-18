#include "FontsManager.hh"

#include <spdlog/spdlog.h>

std::filesystem::path FontsManager::_fontsDir;

void FontsManager::Init()
{
  _fontsDir = std::filesystem::current_path().parent_path() / "Fonts"; // VC++ solution path
}

std::filesystem::path FontsManager::GetFont(std::filesystem::path filePath)
{
  filePath = _fontsDir / filePath.lexically_normal();

  if (!std::filesystem::exists(filePath))
  {
    spdlog::error("Exception in GetShaderPathByName: {} not exists", filePath.string());
    throw std::runtime_error("");
  }
  return filePath;
}
