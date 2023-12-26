#pragma once

#include "Core.hh"

class Texture2D
{
public:
  Texture2D() : textureID{ 0 } {};
  ~Texture2D() = default;

  Texture2D(const Texture2D&) = delete;            // delete copy constructor
  Texture2D& operator=(const Texture2D&) = delete; // delete assign op

  void Create(std::filesystem::path path);
  void Destroy() const { glDeleteTextures(1, &textureID); }

  uint32_t textureID;
  std::filesystem::path texturePath;

private:
  void LoadImageData();
};