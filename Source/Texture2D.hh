#pragma once

#include "Core.hh"
#include "NonCopyable.hh"

/* -----------------------------------
      Texture 2D class
  ----------------------------------- */
class Texture2D : public NonCopyable
{
public:
  Texture2D() : textureID{ 0 } {}

  void InitTexture(Path path, bool gammaCorrection);
  void DestroyTexture() const { glDeleteTextures(1, &textureID); }
  void BindTexture() const { glBindTexture(GL_TEXTURE_2D, textureID); }

  uint32_t textureID;
  Path texturePath;

private:
  void LoadImageData(bool gammaCorrection) const;
};