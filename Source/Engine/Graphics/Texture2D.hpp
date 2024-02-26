#pragma once

#include "Core/Core.hpp"

/* -----------------------------------
      Texture 2D class
  ----------------------------------- */
class Texture2D
{
public:
  Texture2D() : textureID{ 0 } {}

  /* Disable copy constructor */
  Texture2D(Texture2D const&) = delete;
  Texture2D& operator=(Texture2D const&) = delete;

  void InitTexture(Path path, bool gammaCorrection);
  void DestroyTexture() const { glDeleteTextures(1, &textureID); }
  void BindTexture() const { glBindTexture(GL_TEXTURE_2D, textureID); }

  uint32_t textureID;
  Path texturePath;

private:
  void LoadImageData(bool gammaCorrection) const;
};