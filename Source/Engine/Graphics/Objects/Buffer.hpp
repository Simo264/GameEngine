#pragma once

#include "Core/Core.hpp"

enum class BufferUsage : u32 
{
	STREAM_DRAW	= 0x88E0, // GL_STREAM_DRAW
	STREAM_READ	= 0x88E1, // GL_STREAM_READ
	STREAM_COPY	= 0x88E2, // GL_STREAM_COPY
	STATIC_DRAW	= 0x88E4, // GL_STATIC_DRAW
	STATIC_READ	= 0x88E5, // GL_STATIC_READ
	STATIC_COPY	= 0x88E6, // GL_STATIC_COPY
	DYNAMIC_DRAW = 0x88E8, // GL_DYNAMIC_DRAW
	DYNAMIC_READ = 0x88E9, // GL_DYNAMIC_READ
	DYNAMIC_COPY = 0x88EA  // GL_DYNAMIC_COPY
};

enum class BufferAccess : u32 
{
	READ_ONLY	= 0x88B8, // GL_READ_ONLY
	WRITE_ONLY = 0x88B9, // GL_WRITE_ONLY
	READ_WRITE = 0x88BA  // GL_READ_WRITE
};

enum class BufferTarget : u32 
{
	ATOMIC_COUNTER = 0x92C0, // GL_ATOMIC_COUNTER_BUFFER
	SHADER_STORAGE = 0x90D2, // GL_SHADER_STORAGE_BUFFER
	TRANSFORM_FEEDBACK = 0x8C8E, // GL_TRANSFORM_FEEDBACK_BUFFER
	UNIFORM	= 0x8A11  // GL_UNIFORM_BUFFER
};

enum class BufferInternalFormat : u32
{
	R8 = 0x8229,              // GL_R8
	R8_SNORM = 0x8F94,        // GL_R8_SNORM
	R16 = 0x822A,             // GL_R16
	R16_SNORM = 0x8F98,       // GL_R16_SNORM
	RG8 = 0x822B,             // GL_RG8
	RG8_SNORM = 0x8F95,       // GL_RG8_SNORM
	RG16 = 0x822C,            // GL_RG16
	RG16_SNORM = 0x8F99,      // GL_RG16_SNORM
	R3_G3_B2 = 0x2A10,        // GL_R3_G3_B2
	RGB4 = 0x804F,            // GL_RGB4
	RGB5 = 0x8050,            // GL_RGB5
	RGB8 = 0x8051,            // GL_RGB8
	RGB8_SNORM = 0x8F96,      // GL_RGB8_SNORM
	RGB10 = 0x8052,           // GL_RGB10
	RGB12 = 0x8053,           // GL_RGB12
	RGB16 = 0x8054,           // GL_RGB16
	RGB16_SNORM = 0x8F9A,     // GL_RGB16_SNORM
	RGBA2 = 0x8055,           // GL_RGBA2
	RGBA4 = 0x8056,           // GL_RGBA4
	RGB5_A1 = 0x8057,         // GL_RGB5_A1
	RGBA8 = 0x8058,           // GL_RGBA8
	RGBA8_SNORM = 0x8F97,     // GL_RGBA8_SNORM
	RGB10_A2 = 0x8059,        // GL_RGB10_A2
	RGB10_A2UI = 0x906F,      // GL_RGB10_A2UI
	RGBA12 = 0x805A,          // GL_RGBA12
	RGBA16 = 0x805B,          // GL_RGBA16
	SRGB8 = 0x8C41,           // GL_SRGB8
	SRGB8_ALPHA8 = 0x8C43,    // GL_SRGB8_ALPHA8
	R16F = 0x822D,            // GL_R16F
	RG16F = 0x822F,           // GL_RG16F
	RGB16F = 0x881B,          // GL_RGB16F
	RGBA16F = 0x881A,         // GL_RGBA16F
	R32F = 0x822E,            // GL_R32F
	RG32F = 0x8230,           // GL_RG32F
	RGB32F = 0x8815,          // GL_RGB32F
	RGBA32F = 0x8814,         // GL_RGBA32F
	R11F_G11F_B10F = 0x8C3A,  // GL_R11F_G11F_B10F
	RGB9_E5 = 0x8C3D,         // GL_RGB9_E5
	R8I = 0x8231,             // GL_R8I
	R8UI = 0x8232,            // GL_R8UI
	R16I = 0x8233,            // GL_R16I
	R16UI = 0x8234,           // GL_R16UI
	R32I = 0x8235,            // GL_R32I
	R32UI = 0x8236,           // GL_R32UI
	RG8I = 0x8237,            // GL_RG8I
	RG8UI = 0x8238,           // GL_RG8UI
	RG16I = 0x8239,           // GL_RG16I
	RG16UI = 0x823A,          // GL_RG16UI
	RG32I = 0x823B,           // GL_RG32I
	RG32UI = 0x823C,          // GL_RG32UI
	RGB8I = 0x8D8F,           // GL_RGB8I
	RGB8UI = 0x8D7D,          // GL_RGB8UI
	RGB16I = 0x8D89,          // GL_RGB16I
	RGB16UI = 0x8D77,         // GL_RGB16UI
	RGB32I = 0x8D83,          // GL_RGB32I
	RGB32UI = 0x8D71,         // GL_RGB32UI
	RGBA8I = 0x8D8E,          // GL_RGBA8I
	RGBA8UI = 0x8D7C,         // GL_RGBA8UI
	RGBA16I = 0x8D88,         // GL_RGBA16I
	RGBA16UI = 0x8D76,        // GL_RGBA16UI
	RGBA32I = 0x8D82,         // GL_RGBA32I
	RGBA32UI = 0x8D70         // GL_RGBA32UI
};

enum class BufferFormat : u32
{
	RED = 0x1903,                // GL_RED
	RG = 0x8227,                 // GL_RG
	RGB = 0x1907,                // GL_RGB
	BGR = 0x80E0,                // GL_BGR
	RGBA = 0x1908,               // GL_RGBA
	BGRA = 0x80E1,               // GL_BGRA
	RED_INTEGER = 0x8D94,        // GL_RED_INTEGER
	RG_INTEGER = 0x8228,         // GL_RG_INTEGER
	RGB_INTEGER = 0x8D98,        // GL_RGB_INTEGER
	BGR_INTEGER = 0x8D9A,        // GL_BGR_INTEGER
	RGBA_INTEGER = 0x8D99,       // GL_RGBA_INTEGER
	BGRA_INTEGER = 0x8D9B,       // GL_BGRA_INTEGER
	STENCIL_INDEX = 0x1901,      // GL_STENCIL_INDEX
	DEPTH_COMPONENT = 0x1902,    // GL_DEPTH_COMPONENT
	DEPTH_STENCIL = 0x84F9       // GL_DEPTH_STENCIL
};

enum class BufferDataType : u32
{
	UNSIGNED_BYTE = 0x1401,            // GL_UNSIGNED_BYTE
	BYTE = 0x1400,                     // GL_BYTE
	UNSIGNED_SHORT = 0x1403,           // GL_UNSIGNED_SHORT
	SHORT = 0x1402,                    // GL_SHORT
	UNSIGNED_INT = 0x1405,             // GL_UNSIGNED_INT
	INT = 0x1404,                      // GL_INT
	HALF_FLOAT = 0x140B,               // GL_HALF_FLOAT
	FLOAT = 0x1406,                    // GL_FLOAT
	UNSIGNED_BYTE_3_3_2 = 0x8032,      // GL_UNSIGNED_BYTE_3_3_2
	UNSIGNED_BYTE_2_3_3_REV = 0x8362,  // GL_UNSIGNED_BYTE_2_3_3_REV
	UNSIGNED_SHORT_5_6_5 = 0x8363,     // GL_UNSIGNED_SHORT_5_6_5
	UNSIGNED_SHORT_5_6_5_REV = 0x8364, // GL_UNSIGNED_SHORT_5_6_5_REV
	UNSIGNED_SHORT_4_4_4_4 = 0x8033,   // GL_UNSIGNED_SHORT_4_4_4_4
	UNSIGNED_SHORT_4_4_4_4_REV = 0x8365, // GL_UNSIGNED_SHORT_4_4_4_4_REV
	UNSIGNED_SHORT_5_5_5_1 = 0x8034,   // GL_UNSIGNED_SHORT_5_5_5_1
	UNSIGNED_SHORT_1_5_5_5_REV = 0x8366, // GL_UNSIGNED_SHORT_1_5_5_5_REV
	UNSIGNED_INT_8_8_8_8 = 0x8035,     // GL_UNSIGNED_INT_8_8_8_8
	UNSIGNED_INT_8_8_8_8_REV = 0x8367, // GL_UNSIGNED_INT_8_8_8_8_REV
	UNSIGNED_INT_10_10_10_2 = 0x8036,  // GL_UNSIGNED_INT_10_10_10_2
	UNSIGNED_INT_2_10_10_10_REV = 0x8368, // GL_UNSIGNED_INT_2_10_10_10_REV
	UNSIGNED_INT_24_8 = 0x84FA,        // GL_UNSIGNED_INT_24_8
	UNSIGNED_INT_10F_11F_11F_REV = 0x8C3B, // GL_UNSIGNED_INT_10F_11F_11F_REV
	UNSIGNED_INT_5_9_9_9_REV = 0x8C3E, // GL_UNSIGNED_INT_5_9_9_9_REV
	FLOAT_32_UNSIGNED_INT_24_8_REV = 0x8DAD // GL_FLOAT_32_UNSIGNED_INT_24_8_REV
};

/**
 * @brief
 * https://www.khronos.org/opengl/wiki/Buffer_Object
 * 
 * Buffer Objects are OpenGL Objects that store an array of unformatted memory allocated by the OpenGL context.
 * These can be used to store vertex data, pixel data retrieved from images or the framebuffer, 
 * and a variety of other things.
 */
class Buffer
{
public:
	Buffer() : id{ 0 } {}
	Buffer(u64 size, const void* data, BufferUsage usage);
	~Buffer() = default;

	/** @brief Create buffer object */
	void Create();

	/** @brief Delete buffer object and invalidates the name associated with the buffer object */
	void Delete();
	
	/**
	 * @brief Create a new data store for the buffer object.
	 * While creating the new storage, any pre-existing data store is deleted.
	 * The new data store is created with the specified size in bytes and usage.
	 * 
	 * @note If data is NULL, a data store of the specified size is still created, 
	 * but its contents remain uninitialized and thus undefined.
	 */
	void CreateStorage(u64 size, const void* data, BufferUsage usage) const;

	/**
	 * @brief Updates a subset of the buffer object's data store.
	 * 
	 * @param offset: specifies the offset (in bytes) into the buffer object's data store where data replacement will begin
	 * @param size:		specifies the size in bytes of the data store region being replaced
	 * 
	 * @note If data is NULL no data is to be copied.
	 */
	void UpdateStorage(i32 offset, u32 size, const void* data) const;

	/**
	 * @brief Clears a portion of the buffer storage with specified values.
	 * This function clears a range of the buffer's storage using a specified
	 * internal format, data format, and data type. It allows replacing part or
	 * all of the buffer with new data or zeroing it out if `data` is nullptr.
	 *
	 * @param internalformat Specifies the internal format used for clearing the buffer.
	 *                       Must be a valid `BufferInternalFormat` value.
	 * @param offset The byte offset into the buffer where clearing starts.
	 * @param size The number of bytes to clear. If `size` is set to the buffer's full size,
	 *             the entire buffer will be cleared.
	 * @param format Specifies the format of the data stored in the buffer.
	 *               Must be a valid `BufferFormat` value.
	 * @param type Specifies the data type used in the buffer.
	 *             Must be a valid `BufferDataType` value.
	 * @param data A pointer to the new data used to clear the buffer.
	 *             If set to `nullptr`, the buffer is cleared to zero.
	 */
	void ClearStorage(BufferInternalFormat internalformat, 
										i32 offset, 
										i32 size, 
										BufferFormat format,
										BufferDataType type,
										void* data) const;

	/**
	 * @brief Copy all or part of the data store of the buffer object to the data store of another buffer object
	 *
	 * @param size specifies the size of the data to be copied from the source buffer object
	 *							to the destination buffer object.
	 * @param readOffset specifies the offset within the data store of the source buffer at which
	 *                    data will be read
	 * @para writeOffset specifies the offset within the data store of the destination buffer
	 *                    at which data will be written.
	 */
	void CopyStorage(Buffer writeBuffer, i32 readOffset, i32 writeOffset, u64 size) const;

	/** @brief Map all of the buffer object's data store into the client's address space */
	void* MapStorage(BufferAccess access) const;

	/** @brief Release the mapping of the buffer object's data store into the client's address space */
	bool UnmapStorage() const;

	/** @brief Bind the buffer object to an indexed buffer target. */
	void BindBase(BufferTarget target, i32 bindingpoint) const;

	/** @brief Bind a range within the buffer object to an indexed buffer target. */
	void BindRange(BufferTarget target, i32 bindingpoint, i32 offset, u64 size) const;

	bool IsValid() const;

	u32 id;
};
