#pragma once

#include "Core/Core.hpp"
#include "Engine/Graphics/Shader.hpp"

class IniFileHandler;

/**
 * @class ShaderManager.
 * 
 * @brief Singleton class responsible for managing shader and program objects.
 * 
 * This class handles shader compilation, program linking, and cleanup.
 */
class ShadersManager
{
public:
	/** @brief Deleted copy constructor to enforce singleton pattern. */
	ShadersManager(const ShadersManager&) = delete;
	/** @brief Deleted assignment constructor to enforce singleton pattern. */
	void operator=(const ShadersManager&) = delete;

	/** Get the singleton instance of ShaderManager. */
	static ShadersManager& Get()
	{
		static ShadersManager shaderManager;
		return shaderManager;
	}

	/** @brief Load all shaders and programs. */
	void Initialize();
	/** @brief Destroy all shader objects and program objects. */
	void CleanUp();

	/** @brief Retrieve a shader by its filename. */
	Shader& GetShader(StringView shaderName);
	Shader& GetOrInsertShader(StringView shaderName);
	Shader& InsertShader(StringView shaderName);

	/** @brief Retrieve a program by its name. */
	Program& GetProgram(StringView programName);
	Program& InsertProgram(StringView programName);

private:
	ShadersManager() = default;
	~ShadersManager() = default;

	Vector<Shader> _shaders;
	Vector<Program> _programs;

	// Map storing shader filename and its index.
	UnorderedMap<String, u32> _shaderMap;
	// Map storing program name and its index.
	UnorderedMap<String, u32> _programMap;

	void ReadConfig(class IniFileHandler& conf);

	// Return the shader type based on file extension
	i32 ResolveShaderType(StringView ext);

	void SetProgramsUniforms();
};
