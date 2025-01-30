#pragma once

#include "Core/Core.hpp"
#include "Engine/Graphics/Objects/Texture2D.hpp"

enum class Texture3DFormat : u32
{
  RED = 0x1903, // GL_RED
  RG = 0x8227, // GL_RG
  RGB = 0x1907, // GL_RGB
  RGBA = 0x1908, // GL_RGBA
  DEPTH_COMPONENT = 0x1902, // GL_DEPTH_COMPONENT
  RED_INTEGER = 0x8D94, // GL_RED_INTEGER
  RG_INTEGER = 0x8D95, // GL_RG_INTEGER
  RGB_INTEGER = 0x8D96, // GL_RGB_INTEGER
  RGBA_INTEGER = 0x8D97, // GL_RGBA_INTEGER
  DEPTH_STENCIL = 0x84F9, // GL_DEPTH_STENCIL
};
enum class Texture3DType : u32
{
  UNSIGNED_BYTE = 0x1401, // GL_UNSIGNED_BYTE
  UNSIGNED_SHORT = 0x1403, // GL_UNSIGNED_SHORT
  UNSIGNED_INT = 0x1405, // GL_UNSIGNED_INT
  FLOAT = 0x1406, // GL_FLOAT
  UNSIGNED_BYTE_3_3_2 = 0x8032, // GL_UNSIGNED_BYTE_3_3_2
  UNSIGNED_SHORT_5_6_5 = 0x8363, // GL_UNSIGNED_SHORT_5_6_5
  UNSIGNED_SHORT_4_4_4_4 = 0x8033, // GL_UNSIGNED_SHORT_4_4_4_4
  UNSIGNED_SHORT_5_5_5_1 = 0x8034, // GL_UNSIGNED_SHORT_5_5_5_1
  UNSIGNED_INT_8_8_8_8 = 0x8035, // GL_UNSIGNED_INT_8_8_8_8
  UNSIGNED_INT_10_10_10_2 = 0x8036, // GL_UNSIGNED_INT_10_10_10_2
  FLOAT_32_UNSIGNED_INT_24_8_REV = 0x8DAD, // GL_FLOAT_32_UNSIGNED_INT_24_8_REV
};


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
  void SubImage3D(i32 level,
                  i32 xoffset,
                  i32 yoffset,
                  i32 zoffset,
                  i32 width,
                  i32 height,
                  i32 depth,
                  Texture3DFormat format,
                  Texture3DType type,
                  const void* pixels) const;

  /** @brief Delete texture object and invalidates the name associated with the texture object */
  void Delete();

  /**
   * @brief Bind the texture object to the specified texture unit
   * @param unit: specifies the texture unit, to which the texture object should be bound to
   */
  void BindTextureUnit(i32 unit) const;

  /** @brief Set texture parameters */
  void SetParameteri(TextureParameteriName name, 
                     TextureParameteriParam value) const;

  void LoadImages(const Array<const Texture2D*, 6>& images) const;

  bool IsValid() const;

	u32 id;
};