#pragma once

#include "Core/Core.hpp"
#include "Engine/Graphics/OpenGLEnums.hpp"

enum class Texture2DTarget : u32
{
  TEXTURE_2D = 0x0DE1, // GL_TEXTURE_2D
  TEXTURE_2D_ARRAY = 0x8C1A, // GL_TEXTURE_2D_ARRAY
  TEXTURE_RECTANGLE = 0x84F5, // GL_TEXTURE_RECTANGLE
  TEXTURE_2D_MULTISAMPLE = 0x9100, // GL_TEXTURE_2D_MULTISAMPLE
  TEXTURE_2D_MULTISAMPLE_ARRAY = 0x9102  // GL_TEXTURE_2D_MULTISAMPLE_ARRAY
};
enum class Texture2DFormat : u32
{
  RED = 0x1903, // GL_RED
  RG = 0x8227, // GL_RG
  RGB = 0x1907, // GL_RGB
  BGR = 0x80E0, // GL_BGR
  RGBA = 0x1908, // GL_RGBA
  BGRA = 0x80E1, // GL_BGRA
  STENCIL_INDEX = 0x1901, // GL_STENCIL_INDEX
  DEPTH_COMPONENT = 0x1902, // GL_DEPTH_COMPONENT
  DEPTH_STENCIL = 0x84F9, // GL_DEPTH_STENCIL
  RED_INTEGER = 0x8D94, // GL_RED_INTEGER
  RG_INTEGER = 0x8228, // GL_RG_INTEGER
  RGB_INTEGER = 0x8D98, // GL_RGB_INTEGER
  BGR_INTEGER = 0x8D9A, // GL_BGR_INTEGER
  RGBA_INTEGER = 0x8D99, // GL_RGBA_INTEGER
  BGRA_INTEGER = 0x8D9B  // GL_BGRA_INTEGER
};
enum class Texture2DInternalFormat : u32
{
  R8 = 0x8229, // GL_R8
  R8_SNORM = 0x8F94, // GL_R8_SNORM
  R16 = 0x822A, // GL_R16
  R16_SNORM = 0x8F98, // GL_R16_SNORM
  RG8 = 0x822B, // GL_RG8
  RG8_SNORM = 0x8F95, // GL_RG8_SNORM
  RG16 = 0x822C, // GL_RG16
  RG16_SNORM = 0x8F99, // GL_RG16_SNORM
  RGB8 = 0x8051, // GL_RGB8
  RGB8_SNORM = 0x8F96, // GL_RGB8_SNORM
  RGB16 = 0x8054, // GL_RGB16
  RGB16_SNORM = 0x8F9A, // GL_RGB16_SNORM
  RGBA8 = 0x8058, // GL_RGBA8
  RGBA8_SNORM = 0x8F97, // GL_RGBA8_SNORM
  RGBA16 = 0x805B, // GL_RGBA16
  RGBA16_SNORM = 0x8F9B, // GL_RGBA16_SNORM
  SRGB8 = 0x8C41, // GL_SRGB8
  SRGB8_ALPHA8 = 0x8C43, // GL_SRGB8_ALPHA8
  R16F = 0x822D, // GL_R16F
  RG16F = 0x822F, // GL_RG16F
  RGB16F = 0x881B, // GL_RGB16F
  RGBA16F = 0x881A, // GL_RGBA16F
  R32F = 0x822E, // GL_R32F
  RG32F = 0x8230, // GL_RG32F
  RGB32F = 0x8815, // GL_RGB32F
  RGBA32F = 0x8814, // GL_RGBA32F
  R11F_G11F_B10F = 0x8C3A, // GL_R11F_G11F_B10F
  RGB9_E5 = 0x8C3D, // GL_RGB9_E5
  DEPTH_COMPONENT16 = 0x81A5, // GL_DEPTH_COMPONENT16
  DEPTH_COMPONENT24 = 0x81A6, // GL_DEPTH_COMPONENT24
  DEPTH_COMPONENT32F = 0x8CAC, // GL_DEPTH_COMPONENT32F
  DEPTH24_STENCIL8 = 0x88F0, // GL_DEPTH24_STENCIL8
  DEPTH32F_STENCIL8 = 0x8CAD  // GL_DEPTH32F_STENCIL8
};
enum class Texture2DSubImageType : u32
{
  UNSIGNED_BYTE = 0x1401, // GL_UNSIGNED_BYTE
  BYTE = 0x1400, // GL_BYTE
  UNSIGNED_SHORT = 0x1403, // GL_UNSIGNED_SHORT
  SHORT = 0x1402, // GL_SHORT
  UNSIGNED_INT = 0x1405, // GL_UNSIGNED_INT
  INT = 0x1404, // GL_INT
  FLOAT = 0x1406, // GL_FLOAT
  UNSIGNED_BYTE_3_3_2 = 0x8032, // GL_UNSIGNED_BYTE_3_3_2
  UNSIGNED_BYTE_2_3_3_REV = 0x8362, // GL_UNSIGNED_BYTE_2_3_3_REV
  UNSIGNED_SHORT_5_6_5 = 0x8363, // GL_UNSIGNED_SHORT_5_6_5
  UNSIGNED_SHORT_5_6_5_REV = 0x8364, // GL_UNSIGNED_SHORT_5_6_5_REV
  UNSIGNED_SHORT_4_4_4_4 = 0x8033, // GL_UNSIGNED_SHORT_4_4_4_4
  UNSIGNED_SHORT_5_5_5_1 = 0x8034  // GL_UNSIGNED_SHORT_5_5_5_1
};
enum class Texture2DClearImageType : u32
{
  UNSIGNED_BYTE = 0x1401, // GL_UNSIGNED_BYTE
  BYTE = 0x1400, // GL_BYTE
  UNSIGNED_SHORT = 0x1403, // GL_UNSIGNED_SHORT
  SHORT = 0x1402, // GL_SHORT
  UNSIGNED_INT = 0x1405, // GL_UNSIGNED_INT
  INT = 0x1404, // GL_INT
  FLOAT = 0x1406, // GL_FLOAT
  UNSIGNED_INT_10F_11F_11F_REV = 0x8C3B, // GL_UNSIGNED_INT_10F_11F_11F_REV
  UNSIGNED_INT_5_9_9_9_REV = 0x8C3E, // GL_UNSIGNED_INT_5_9_9_9_REV
  UNSIGNED_INT_2_10_10_10_REV = 0x8368  // GL_UNSIGNED_INT_2_10_10_10_REV
};
enum class Texture2DGetImageType : u32
{
  UNSIGNED_BYTE = 0x1401, // GL_UNSIGNED_BYTE
  BYTE = 0x1400, // GL_BYTE
  UNSIGNED_SHORT = 0x1403, // GL_UNSIGNED_SHORT
  SHORT = 0x1402, // GL_SHORT
  UNSIGNED_INT = 0x1405, // GL_UNSIGNED_INT
  INT = 0x1404, // GL_INT
  FLOAT = 0x1406, // GL_FLOAT
  HALF_FLOAT = 0x140B, // GL_HALF_FLOAT
  UNSIGNED_INT_10F_11F_11F_REV = 0x8C3B, // GL_UNSIGNED_INT_10F_11F_11F_REV
  UNSIGNED_INT_5_9_9_9_REV = 0x8C3E, // GL_UNSIGNED_INT_5_9_9_9_REV
  UNSIGNED_INT_2_10_10_10_REV = 0x8368  // GL_UNSIGNED_INT_2_10_10_10_REV
};

/**
 * @brief 
 * https://www.khronos.org/opengl/wiki/Texture
 * 
 * An image is defined as a single array of pixels of a certain dimensionality (1D, 2D, or 3D), 
 * with a particular size, and a specific format.
 * 
 * A texture is a container of one or more images. 
 * But textures do not store arbitrary images; a texture has specific constraints on the images it can contain. 
 * There are three defining characteristics of a texture, each of them defining part of those constraints: 
 *   1. the texture type: defines the arrangement of images within the texture
 *   2. the texture size: defines the size of the images in the texture
 *   3. the image format: used for images in the texture, defines the format that all of these images share.
 * 
 * --- Mip maps ---
 * When a texture is directly applied to a surface, how many pixels of that texture (commonly called "texels") 
 * are used depends on the angle at which that surface is rendered. 
 * A texture mapped to a plane that is almost edge-on with the camera will only use a fraction of the pixels of the texture. 
 * Similarly, looking directly down on the texture from far away will show fewer texels than an up-close version.
 * 
 * Mip maps are pre-shrunk versions of the full-sized image. 
 * Each mipmap is half the size (rounded down) of the previous one in the chain, continuing until all dimensions are 1. 
 * So a 64x16 2D texture can have 6 mip-maps: 32x8, 16x4, 8x2, 4x1, 2x1, and 1x1. 
 * OpenGL does not require that the entire mipmap chain is complete; 
 * you can specify what range of mipmaps in a texture are available.
 */
class Texture2D
{
public:
  Texture2D() : 
    id{ 0 },
    path{}
  {}

  // <path> the absolute path to the image file
  Texture2D(Texture2DTarget target, 
            const fs::path& absolute);
  
  ~Texture2D() = default;

  // <path> the absolute path to the image file
  void LoadImageData(const fs::path& absolute);

  /** @brief Create texture object */
  void Create(Texture2DTarget target);

  /** @brief Delete texture object and invalidates the name associated with the texture object */
  void Delete();

  /** @brief Bind the texture object to the specified texture unit. */
  void BindTextureUnit(i32 unit) const;

  /** @brief Set texture parameters */
  void SetParameteri(TextureParameteriName name, 
                     TextureParameteriParam value) const;
  
  void SetParameterfv(TextureParameteriName name, f32* values) const;
  
  void SetCompareFunc(CompareFunc func) const;

  /** @brief Generate mipmaps for the texture object */
  void GenerateMipmap() const;

  /** @brief The storage is created here, but the contents of that storage is undefined. */
  void CreateStorage(Texture2DInternalFormat internalFormat, 
                     i32 width, 
                     i32 height) const;

  /** @brief Specify storage for multisample texture. */
  void CreateStorageMultisampled(Texture2DInternalFormat internalFormat, 
                                 i32 samples, 
                                 i32 width, 
                                 i32 height) const;

  /** @brief Specify a two-dimensional texture subimage. */
  void UpdateStorage(i32 level,
                     i32 width,
                     i32 height,
                     Texture2DFormat format,
                     Texture2DSubImageType type, 
                     const void* pixels,
                     i32 xoffset = 0,
                     i32 yoffset = 0
                     ) const;

  /** @brief Fills all the texture image with a constant value. */
  void ClearStorage(i32 level,
                    Texture2DFormat format,
                    Texture2DClearImageType type,
                    const void* data) const;
  
  /** @return A texture image into pixels. */
  void GetTextureImage(i32 level,
                       Texture2DFormat format,
                       Texture2DGetImageType type, 
                       i32 buffSize, 
                       void* pixels) const;

  i32 GetWidth() const;
  i32 GetHeight() const;
  Texture2DInternalFormat GetInternalFormat() const;
  Texture2DFormat GetFormat(Texture2DInternalFormat internalFormat) const;
  i32 GetNumChannels(Texture2DInternalFormat internalFormat) const;

  bool Compare(const Texture2D& other) const { return id == other.id; }
  bool IsValid() const;
  
  u32 id;
  fs::path path;
};