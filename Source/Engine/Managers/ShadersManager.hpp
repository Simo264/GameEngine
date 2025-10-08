#pragma once

#include "Core/Types.hpp"
#include "Core/Strings.hpp"
#include "Core/Containers.hpp"
#include "Core/FileSystem.hpp"

#include "Core/Singleton.hpp"
#include "Engine/Graphics/Shader.hpp"

/**
 * @brief Singleton class responsible for managing shader and program objects.
 */
class ShadersManager : public Singleton<ShadersManager>
{
public:
	/** @brief Load all shaders and programs. */
	void Initialize();
	/** @brief Destroy all shader objects and program objects. */
	void Cleanup();

	/** @brief Retrieve a shader by its filename. */
	Shader GetShader(StringView shaderName) const;
	Shader CreateShader(StringView shaderName, ShaderType type);
	Shader GetOrCreateShader(StringView shaderName, ShaderType type);

	/** @brief Retrieve a program by its name. */
	Program GetProgram(StringView programName) const;
	Program CreateProgram(StringView programName);

private:
	void __LoadConfig(const fs::path& path);

	struct __PairShaderName
	{
		Shader shader;					// Shader object
		Array<char, 32> name{}; // Shader name
	};
	struct __PairProgramName
	{
		Program program;				// Program object
		Array<char, 32> name{}; // Program name
	};

	Vector<__PairShaderName> _shaders;
	Vector<__PairProgramName> _programs;
};
