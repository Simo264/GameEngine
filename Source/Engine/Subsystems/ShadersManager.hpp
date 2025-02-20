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
	const Shader& GetShader(StringView shaderName) const;
	const Shader& GetOrCreateShader(StringView shaderName);
	const Shader& CreateShader(StringView shaderName);

	/** @brief Retrieve a program by its name. */
	const Program& GetProgram(StringView programName) const;
	const Program& CreateProgram(StringView programName);

private:
	ShadersManager() = default;
	~ShadersManager() = default;
	
	void ReadConfig(class IniFileHandler& conf);
	i32 ResolveShaderType(StringView ext);
	void SetProgramsUniforms() const;

	Vector<Shader> _shaders;
	Vector<Program> _programs;

	// Map storing shader index and shader name
	Vector<std::pair<u32, String>> _shaderNames;
	Vector<std::pair<u32, String>> _programNames;
};
