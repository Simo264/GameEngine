#pragma once

#include "Core/Core.hpp"

/* -----------------------------------
      Texture 2D class
  ----------------------------------- */
class Texture2D
{
public:
  Texture2D(Path path, bool gammaCorrection);

  /* Disable copy constructor */
  Texture2D(Texture2D const&) = delete;
  Texture2D& operator=(Texture2D const&) = delete;

  void Destroy() const;
  void Bind() const;

  uint32_t textureID;
  Path texturePath;

private:
  void LoadImageData(bool gammaCorrection) const;
};