#pragma once

#include "Core/Core.hpp"

class StaticMesh;

class ModelsManager
{
public:
	/** @brief Deleted copy constructor to enforce singleton pattern. */
	ModelsManager(const ModelsManager&) = delete;
	/** @brief Deleted assignment constructor to enforce singleton pattern. */
	void operator=(const ModelsManager&) = delete;

	/** @brief Retrieves the singleton instance of ModelsManager. */
	static ModelsManager& Get()
	{
		static ModelsManager modelsManager;
		return modelsManager;
	}

	const StaticMesh* FindStaticMesh(const fs::path& path) const;
	const StaticMesh* InsertStaticMesh(const fs::path& path);

private:
	ModelsManager() = default;
	~ModelsManager() = default;

	UnorderedMap<fs::path, StaticMesh> _cacheStaticMesh;
};
