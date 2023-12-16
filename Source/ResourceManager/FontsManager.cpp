#include "FontsManager.hh"

#include <spdlog/spdlog.h>

std::filesystem::path FontsManager::_fontsDir;

void FontsManager::Init()
{
  _fontsDir = std::filesystem::current_path(); // VC++ solution path
  _fontsDir = _fontsDir.parent_path();
  _fontsDir = _fontsDir / "Fonts";
}

std::filesystem::path FontsManager::GetFontByName(const char* filename)
{
  std::filesystem::path fontFilePath = _fontsDir / filename;
  
  if (!std::filesystem::exists(fontFilePath))
  {
    spdlog::error("Exception in GetShaderPathByName: {} not exists", fontFilePath.string());
    throw std::runtime_error("");
  }
  return fontFilePath;
}
