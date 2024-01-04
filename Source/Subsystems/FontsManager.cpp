#include "FontsManager.hh"
#include "../Logger.hh"

Path FontsManager::GetFont(Path filePath)
{
  filePath = _fontsDir / filePath.lexically_normal();

  if (!std::filesystem::exists(filePath))
  {
    CONSOLE_ERROR("Exception in GetShaderPathByName: {} not exists", filePath.string());
    throw RuntimeError("");
  }
  return filePath;
}
