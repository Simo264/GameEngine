#pragma once

#include "Core.hh"
#include "UncopyableObject.hh"

class Texture2D : public UncopyableObject
{
public:
  Texture2D() : textureID{ 0 } {};

  void InitTexture(Path path);
  void DestroyTexture() const { glDeleteTextures(1, &textureID); }
  void BindTexture() const { glBindTexture(GL_TEXTURE_2D, textureID); }

  uint32_t textureID;
  Path texturePath;

private:
  void LoadImageData();
};