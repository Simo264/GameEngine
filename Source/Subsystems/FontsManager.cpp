#include "FontsManager.hh"
#include "../Logger.hh"

std::filesystem::path FontsManager::GetFont(std::filesystem::path filePath)
{
  const static std::filesystem::path fontsDir = std::filesystem::current_path().parent_path() / "Fonts"; // VC++ solution path

  filePath = fontsDir / filePath.lexically_normal();

  if (!std::filesystem::exists(filePath))
  {
    CONSOLE_ERROR("Exception in GetShaderPathByName: {} not exists", filePath.string());
    throw std::runtime_error("");
  }
  return filePath;
}
