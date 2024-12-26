#pragma once

#include "Core/Core.hpp"

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
 *   2. thw texture size: defines the size of the images in the texture
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
    internalFormat{ 0 },
    format{ 0 },
    nChannels{ 0 },
    width{ 0 },
    height{ 0 },
    path{}
  {}

  /**
   * @param target: GL_TEXTURE_2D or GL_TEXTURE_2D_MULTISAMPLE
   */
  Texture2D(i32 target, const fs::path& imagePath, bool gammaCorrection);
  ~Texture2D() = default;

  /**
   * @brief
   * Load data from an image file
   */
  void LoadImageData(const fs::path& imagePath, bool gammaCorrection);

  /**
   * @brief
   * Create texture object
   * 
   * @param target: GL_TEXTURE_2D or GL_TEXTURE_2D_MULTISAMPLE
   */
  void Create(i32 target);

  /**
   * @brief
   * Delete texture object and invalidates the name associated with the texture object 
   */
  void Delete();

  /**
   * @brief
   * Bind the texture object to the specified texture unit
   *
   * @param unit: specifies the texture unit, to which the texture object should be bound to
   */
  void BindTextureUnit(i32 unit) const;

  /**
   * @brief
   * Set texture parameters
   */
  void SetParameteri(i32 name, i32 value) const;
  void SetParameteriv(i32 name, i32* values) const;
  void SetParameterf(i32 name, f32 value) const;
  void SetParameterfv(i32 name, f32* values) const;

  /**
   * @brief
   * Generate mipmaps for the texture object
   */
  void GenerateMipmap() const;

  /**
   * @brief
   * The storage is created here, but the contents of that storage is undefined.
   * See https://registry.khronos.org/OpenGL-Refpages/gl4/html/glTexStorage2D.xhtml
   */
  void CreateStorage(i32 internalFormat, i32 width, i32 height);

  /**
   * @brief
   * Specify storage for multisample texture.
   * See https://registry.khronos.org/OpenGL-Refpages/gl4/html/glTexStorage2DMultisample.xhtml
   */
  void CreateStorageMultisampled(i32 internalFormat, i32 samples, i32 width, i32 height);

  /**
   * @brief
   * Specify a two-dimensional texture subimage.
   * See https://registry.khronos.org/OpenGL-Refpages/gl4/html/glTexSubImage2D.xhtml
   */
  void UpdateStorage(i32 level, i32 xoffset, i32 yoffset, i32 type, const void* pixels) const;

  /**
   * @brief
   * Fills all the texture image with a constant value.
   * See https://registry.khronos.org/OpenGL-Refpages/gl4/html/glClearTexImage.xhtml
   */
  void ClearStorage(i32 level, i32 type, const void* data) const;
  
  /**
   * @return
   * A texture image into pixels. See https://registry.khronos.org/OpenGL-Refpages/gl4/html/glGetTexImage.xhtml
   */
  void GetTextureImage(i32 level, i32 type, i32 buffSize, void* pixels) const;

  bool Compare(const Texture2D& other) const { return id == other.id; }
  bool IsValid() const;
  
  u32 id;

  i32 format;
  i32 internalFormat;

  i32 width;
  i32 height;

  i32 nChannels;
  
  fs::path path;
};