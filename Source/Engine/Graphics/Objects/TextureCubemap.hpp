#pragma once

#include "Core/Core.hpp"
#include "Engine/Graphics/Objects/Texture2D.hpp"

class TextureCubemap
{
public:
	TextureCubemap() : 
    id{ 0 } 
  {}
	~TextureCubemap() = default;

	/** @brief Create texture object */
	void Create();

  /** @brief Allocate the entire cubemap with a single call glTextureStorage2D. */
  void CreateStorage(Texture2DInternalFormat internalFormat, i32 width, i32 height) const;

  /** @brief Specify a three-dimensional texture subimage. */
  void SubImage3D(
    i32 level,
    i32 xoffset,
    i32 yoffset,
    i32 zoffset,
    i32 width,
    i32 height,
    i32 depth,
    i32 format,
    i32 type,
    const void* pixels) const;

  /** @brief Delete texture object and invalidates the name associated with the texture object */
  void Delete();

  /**
   * @brief Bind the texture object to the specified texture unit
   * @param unit: specifies the texture unit, to which the texture object should be bound to
   */
  void BindTextureUnit(i32 unit) const;

  /** @brief Set texture parameters */
  void SetParameteri(TextureParameteriName name, TextureParameteriParam value) const;

  void LoadImages(const Array<const Texture2D*, 6>& images) const;

  bool IsValid() const;

	u32 id;
};