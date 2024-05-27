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
  Texture2D();

  Texture2D(const fspath& path, bool gammaCorrection);

  void LoadImageData(const fspath& path, bool gammaCorrection);

  /**
   * Create texture object
   * 
   * @param target: specifies an array in which names of the new texture objects are stored. Must be one of:
   *                GL_TEXTURE_1D, GL_TEXTURE_2D, GL_TEXTURE_3D, GL_TEXTURE_1D_ARRAY, GL_TEXTURE_2D_ARRAY, 
   *                GL_TEXTURE_RECTANGLE, GL_TEXTURE_CUBE_MAP, GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_BUFFER, 
   *                GL_TEXTURE_2D_MULTISAMPLE or GL_TEXTURE_2D_MULTISAMPLE_ARRAY
   * 
   * https://registry.khronos.org/OpenGL-Refpages/gl4/html/glCreateTextures.xhtml
   */
  void Create(int target);

  /**
   * Delete texture object and invalidates the name associated with the texture object 
   */
  void Delete();

  /**
   * Bind texture object
   */
  void Bind() const;

  /**
   * Unbind texture object
   */
  void Unbind() const;

  /**
   * Bind the texture object to the specified texture unit
   *
   * @param unit: specifies the texture unit, to which the texture object should be bound to
   */
  void BindTextureUnit(int unit) const;

  /**
   * Set texture parameters
   */
  void SetParameteri(int name, int value) const;
  void SetParameteriv(int name, int* values) const;
  void SetParameterf(int name, float value) const;
  void SetParameterfv(int name, float* values) const;

  /**
   * Generate mipmaps for the texture object
   */
  void GenerateMipmap() const;

  /**
   * The storage is created here, but the contents of that storage is undefined
   * 
   * @param width:  specifies the width of the texture, in texels
   * @param height: specifies the height of the texture, in texels
   */
  void CreateStorage(int width, int height);

  /**
   * Specify storage for multisample texture
   * 
   * @param samples:              specify the number of samples in the texture
   * @param width:                specifies the width of the texture, in texels
   * @param height:               specifies the height of the texture, in texels
   * @param fixedsamplelocations: specifies whether the image will use identical sample locations and the same number 
   *                              of samples for all texels in the image
   */
  void CreateStorageMultisampled(int samples, int width, int height, bool fixedsamplelocations = true);

  /**
   * Specify a two-dimensional texture subimage
   * 
   * @param level:    specifies the level-of-detail number. Level 0 is the base image level
   * @param xoffset:  specifies a texel offset in the x direction within the texture array.
   * @param yoffset:  specifies a texel offset in the y direction within the texture array
   * 
   * @param type:     specifies the data type of the pixel data.
   *                  The following symbolic values are accepted: GL_UNSIGNED_BYTE, GL_BYTE, GL_UNSIGNED_SHORT, ...
   *                  https://registry.khronos.org/OpenGL-Refpages/gl4/html/glTexSubImage2D.xhtml
   * 
   * @param pixels:   specifies a pointer to the image data in memory
   */
  void UpdateStorage(int level, int xoffset, int yoffset, int type, const void* pixels) const;

  /**
   * Fills all the texture image with a constant value
   * 
   * @param level:  the level of texture containing the region to be cleared. Level 0 is the base image level
   * 
   * @param type:   the type of the data whose address in memory is given by data.
   *                https://registry.khronos.org/OpenGL-Refpages/gl4/html/glClearTexImage.xhtml
   * 
   * @param data:   the address in memory of the data to be used to clear the specified region
   */
  void ClearStorage(int level, int type, const void* data) const;
  
  /**
   * Perform a raw data copy between two images. 
   * This requires the destination texture to already be allocated with an image of appropriate size and format
   * 
   * @param level: the mipmap level to read from the source. Level 0 is the base image level
   */
  void CopyStorage(int level, const Texture2D& dest) const;

  constexpr bool Compare(const Texture2D& other) const { return id == other.id; }
  constexpr bool IsValid() const { return id != static_cast<uint32_t>(-1); }
  constexpr int GetWidth() const { return _width; }
  constexpr int GetHeight() const { return _height; }
  constexpr int GetMipmapLevels() const { return _mipmapLevels; }
  constexpr int GetSamples() const { return _samples; }
  constexpr bool IsMultisampled() const { return _isMultisampled; }
  

  uint32_t id; /* the texture object id */
  
  /**
   * Specifies the format of the pixel data.
   * The following symbolic values are accepted:
   * GL_RED, GL_RG, GL_RGB, GL_BGR, GL_RGBA, GL_BGRA, GL_DEPTH_COMPONENT, and GL_STENCIL_INDEX.
   */
  int format;

  /**
   * Specifies the sized internal format to be used to store texture image data.
   * Must be one of the sized internal formats given in Table 1 below:
   * https://registry.khronos.org/OpenGL-Refpages/gl4/html/glTexStorage2D.xhtml
   */
  int internalformat;

  fspath path; /* the texture path (if loaded from file) */

private:
  int   _mipmapLevels;
  int   _width;
  int   _height;
  int   _samples;
  bool  _isMultisampled;
};