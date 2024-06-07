#pragma once

#include "Core/Core.hpp"

class TextureCubemap
{
public:
	TextureCubemap();
	~TextureCubemap() = default;

	/**
	 * Create texture object
	 *
	 */
	void Create();

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

  void LoadImages(const array<string, 6>& images);

	uint32_t id;

	const int target;
};