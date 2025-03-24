#pragma once

#include "Core/Core.hpp"
#include "Engine/Graphics/Shader.hpp"

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
	Shader GetShader(StringView shaderName) const;
	Shader CreateShader(StringView shaderName);
	Shader GetOrCreateShader(StringView shaderName);

	/** @brief Retrieve a program by its name. */
	Program GetProgram(StringView programName) const;
	Program CreateProgram(StringView programName);

private:
	ShadersManager() = default;
	~ShadersManager() = default;
	
	void LoadConfig(const fs::path& path);

	struct PairShaderName
	{
		Shader shader;					// Shader object
		Array<char, 32> name{}; // Shader name
	};
	struct PairProgramName
	{
		Program program;				// Program object
		Array<char, 32> name{}; // Program name
	};

	// Stores all loaded shaders
	Vector<PairShaderName> _shaders;
	// Stores all loaded programs
	Vector<PairProgramName> _programs;
};
