#pragma once

#include "Core/Core.hpp"
#include "Engine/Graphics/OpenGLEnums.hpp"

enum class Texture2DTarget : u32
{
  Texture2D = 0x0DE1, // GL_TEXTURE_2D
  Texture2DArray = 0x8C1A, // GL_TEXTURE_2D_ARRAY
  TextureRectangle = 0x84F5, // GL_TEXTURE_RECTANGLE
  Texture2DMultisample = 0x9100, // GL_TEXTURE_2D_MULTISAMPLE
  Texture2DMultisampleArray = 0x9102  // GL_TEXTURE_2D_MULTISAMPLE_ARRAY
};
enum class Texture2DInternalFormat : u32
{
  R8 = 0x8229, // GL_R8
  RG8 = 0x822B, // GL_RG8
  RGB8 = 0x8051, // GL_RGB8
  RGBA8 = 0x8058, // GL_RGBA8
  RGB10_A2 = 0x8059, // GL_RGB10_A2
  RGBA16F = 0x881A, // GL_RGBA16F
  RGB16F = 0x881B, // GL_RGB16F
  RGBA32F = 0x8814, // GL_RGBA32F
  RGB32F = 0x8815, // GL_RGB32F
  SRGB8 = 0x8C41, // GL_SRGB8
  SRGB8_ALPHA8 = 0x8C43, // GL_SRGB8_ALPHA8

  DepthComponent16 = 0x81A5, // GL_DEPTH_COMPONENT16
  DepthComponent24 = 0x81A6, // GL_DEPTH_COMPONENT24
  DepthComponent32F = 0x8CAC, // GL_DEPTH_COMPONENT32F
  Depth24Stencil8 = 0x88F0, // GL_DEPTH24_STENCIL8
  Depth32FStencil8 = 0x8CAD, // GL_DEPTH32F_STENCIL8
  StencilIndex8 = 0x8D48  // GL_STENCIL_INDEX8
};
enum class TextureParameteriName : u32 
{
  TextureMinFilter    = 0x2801, // GL_TEXTURE_MIN_FILTER
  TextureMagFilter    = 0x2800, // GL_TEXTURE_MAG_FILTER
  TextureWrapS        = 0x2802, // GL_TEXTURE_WRAP_S
  TextureWrapT        = 0x2803, // GL_TEXTURE_WRAP_T
  TextureWrapR        = 0x8072, // GL_TEXTURE_WRAP_R
  TextureBaseLevel    = 0x813C, // GL_TEXTURE_BASE_LEVEL
  TextureMaxLevel     = 0x813D, // GL_TEXTURE_MAX_LEVEL
  TextureCompareMode  = 0x884C, // GL_TEXTURE_COMPARE_MODE
  TextureCompareFunc  = 0x884D, // GL_TEXTURE_COMPARE_FUNC
  TextureSwizzleR     = 0x8E42, // GL_TEXTURE_SWIZZLE_R
  TextureSwizzleG     = 0x8E43, // GL_TEXTURE_SWIZZLE_G
  TextureSwizzleB     = 0x8E44, // GL_TEXTURE_SWIZZLE_B
  TextureSwizzleA     = 0x8E45, // GL_TEXTURE_SWIZZLE_A
  TextureBorderColor  = 0x1004  // GL_TEXTURE_BORDER_COLOR
};
enum class TextureParameteriParam : i32 
{
  Nearest               = 0x2600, // GL_NEAREST
  Linear                = 0x2601, // GL_LINEAR
  NearestMipmapNearest  = 0x2700, // GL_NEAREST_MIPMAP_NEAREST
  LinearMipmapNearest   = 0x2701, // GL_LINEAR_MIPMAP_NEAREST
  NearestMipmapLinear   = 0x2702, // GL_NEAREST_MIPMAP_LINEAR
  LinearMipmapLinear    = 0x2703, // GL_LINEAR_MIPMAP_LINEAR
  Repeat                = 0x2901, // GL_REPEAT
  ClampToEdge           = 0x812F, // GL_CLAMP_TO_EDGE
  ClampToBorder         = 0x812D, // GL_CLAMP_TO_BORDER
  MirroredRepeat        = 0x8370, // GL_MIRRORED_REPEAT
  CompareRefToTexture   = 0x884E, // GL_COMPARE_REF_TO_TEXTURE
  CompareRToTexture     = 0x884E  // GL_COMPARE_R_TO_TEXTURE (simile)
};
enum class TextureType : u32
{
  MeshTexture,
  Icon
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
    internalFormat{ 0 },
    format{ 0 },
    nChannels{ 0 },
    width{ 0 },
    height{ 0 },
    relativePath{}
  {}

  /** @brief Constructor which initialises a Texture2D object by loading data from an image file. */
  Texture2D(Texture2DTarget target, const fs::path& absolutePath, bool gammaCorrection);
  ~Texture2D() = default;

  /** @brief Loads image data from file and stores it in the texture. */
  void LoadImageData(const fs::path& absolutePath, bool gammaCorrection);

  /** @brief Create texture object */
  void Create(Texture2DTarget target);

  /** @brief Delete texture object and invalidates the name associated with the texture object */
  void Delete();

  /** @brief Bind the texture object to the specified texture unit. */
  void BindTextureUnit(i32 unit) const;

  /** @brief Set texture parameters */
  void SetParameteri(TextureParameteriName name, TextureParameteriParam value) const;
  void SetParameterfv(TextureParameteriName name, f32* values) const;
  void SetCompareFunc(CompareFunc func) const;

  /** @brief Generate mipmaps for the texture object */
  void GenerateMipmap() const;

  /** @brief The storage is created here, but the contents of that storage is undefined. */
  void CreateStorage(Texture2DInternalFormat internalFormat, i32 width, i32 height);

  /** @brief Specify storage for multisample texture. */
  void CreateStorageMultisampled(Texture2DInternalFormat internalFormat, i32 samples, i32 width, i32 height);

  /** @brief Specify a two-dimensional texture subimage. */
  void UpdateStorage(i32 level, i32 xoffset, i32 yoffset, i32 type, const void* pixels) const;

  /** @brief Fills all the texture image with a constant value. */
  void ClearStorage(i32 level, i32 type, const void* data) const;
  
  /** @return A texture image into pixels. */
  void GetTextureImage(i32 level, i32 type, i32 buffSize, void* pixels) const;

  bool Compare(const Texture2D& other) const { return id == other.id; }
  bool IsValid() const;
  
  u32 id;

  i32 format;
  Texture2DInternalFormat internalFormat;

  i32 width;
  i32 height;

  i32 nChannels;
  
  fs::path relativePath;
};