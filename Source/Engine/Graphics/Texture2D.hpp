#pragma once

#include "Core/Core.hpp"

/**
 *  https://www.khronos.org/opengl/wiki/Texture
 * 
 *  An image is defined as a single array of pixels of a certain dimensionality (1D, 2D, or 3D), 
 *  with a particular size, and a specific format.
 * 
 *  A texture is a container of one or more images. 
 *  But textures do not store arbitrary images; a texture has specific constraints on the images it can contain. 
 *  There are three defining characteristics of a texture, each of them defining part of those constraints: 
 *    1. the texture type: defines the arrangement of images within the texture
 *    2. thw texture size: defines the size of the images in the texture
 *    3. the image format: used for images in the texture, defines the format that all of these images share.
 * 
 *  --- Mip maps ---
 *  When a texture is directly applied to a surface, how many pixels of that texture (commonly called "texels") 
 *  are used depends on the angle at which that surface is rendered. 
 *  A texture mapped to a plane that is almost edge-on with the camera will only use a fraction of the pixels of the texture. 
 *  Similarly, looking directly down on the texture from far away will show fewer texels than an up-close version.
 * 
 *  Mip maps are pre-shrunk versions of the full-sized image. 
 *  Each mipmap is half the size (rounded down) of the previous one in the chain, continuing until all dimensions are 1. 
 *  So a 64x16 2D texture can have 6 mip-maps: 32x8, 16x4, 8x2, 4x1, 2x1, and 1x1. 
 *  OpenGL does not require that the entire mipmap chain is complete; 
 *  you can specify what range of mipmaps in a texture are available.
 */
class Texture2D
{
public:
  Texture2D(const Path& path, bool gammaCorrection);

  /**
   * Create texture object
   */
  void Create();

  /**
   * Delete texture object
   */
  void Delete() const;

  /**
   * Bind texture object
   */
  void Bind() const;

  /**
   * Unbind texture object
   */
  void Unbind() const;

  /**
   * Set texture parameters
   */
  void SetParameteri(int name, int value);

  /**
   * Generate mipmaps for the texture object
   */
  void GenerateMipmap() const;

  /**
   * Allocate images with the given size (width​, height), with the number of mipmaps given by levels​. 
   * The storage is created here, but the contents of that storage is undefined
   * 
   * @param levels:         specify the number of texture levels. Level 0 is the base image level
   * @param internalformat: specifies the sized internal format to be used to store texture image data
   * @param width:          specifies the width of the texture, in texels
   * @param height:         specifies the height of the texture, in texels
   */
  void CreateStorage(int levels, int internalformat, int width, int height);

  /**
   * Specify a two-dimensional texture subimage
   * 
   * @param level:    specifies the level-of-detail number. Level 0 is the base image level
   * @param xoffset:  specifies a texel offset in the x direction within the texture array.
   * @param yoffset:  specifies a texel offset in the y direction within the texture array
   * @param format:   specifies the format of the pixel data. 
   *                  The following symbolic values are accepted: GL_RED, GL_RG, GL_RGB, GL_BGR, ... 
   * @param type:     specifies the data type of the pixel data.
   *                  The following symbolic values are accepted: GL_UNSIGNED_BYTE, GL_BYTE, GL_UNSIGNED_SHORT, ...
   * @param pixels:   specifies a pointer to the image data in memory
   */
  void UpdateStorage(int level, int xoffset, int yoffset, const void* pixels) const;

  void Copy();

  constexpr bool Compare(const Texture2D& other) const { return id == other.id; }

  constexpr bool IsValid() const { return id != static_cast<uint32_t>(-1); }

  uint32_t id;        /* the texture object id */
  
  int type;           /* the the data type of the pixel data */
  
  int format;         /* the format of the pixel data */
  int internalformat; /* the number of color components in the texture */

  int width;          /* the texture size */
  int height;

  Path path;          /* the texture path (if loaded from file) */

private:
  void LoadImageData(const Path& path, bool gammaCorrection);
};