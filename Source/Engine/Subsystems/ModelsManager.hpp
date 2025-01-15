#pragma once

#include "Core/Core.hpp"
#include "Engine/ECS/StaticMesh.hpp"
#include "Engine/ECS/Skeleton/SkeletalMesh.hpp"

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

	const SkeletalMesh* FindSkeletalMesh(const fs::path& path) const;
	const SkeletalMesh* InsertSkeletalMesh(const fs::path& path);

	void DestroyAll();
private:
	ModelsManager() = default;
	~ModelsManager() = default;

	UnorderedMap<fs::path, StaticMesh> _cacheStaticMesh;
	UnorderedMap<fs::path, SkeletalMesh> _cacheSkeletalMesh;
};
