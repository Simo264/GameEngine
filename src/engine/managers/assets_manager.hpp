#pragma once

#include "Core/Types.hpp"
#include "Core/FileSystem.hpp"
#include "Core/Containers.hpp"
#include "Core/Functional.hpp"
#include "Core/Singleton.hpp"
#include "Engine/ECS/Types.hpp"

/**
 * @brief Central manager for assigning and retrieving AssetIds.
 * Its role is to map full file paths to a unique numeric
 * identifier (AssetId). This allows the rest of the application
 * to reference assets using a simple, efficient numeric ID.
 *
 * @note This class only manages identification and mapping; it does not
 * handle the actual loading or unloading of asset data.
 */
class AssetsManager : public Singleton<AssetsManager>
{
public:
	/**
	 * @brief Registers a new asset or returns the existing ID.
	 * If the path is already registered, it returns the existing AssetId. 
	 * Otherwise, it assigns a new incremental AssetId, registers it, and returns the new ID.
	 */
	AssetId RegisterAsset(const fs::path& fullPath);
	
	/** @brief Retrieves the file path associated with a given AssetId. */
	Optional<const fs::path*> GetAssetPath(AssetId assetId) const;
	/** @brief Retrieves the AssetId associated with a given file path. */
	AssetId GetAssetId(const fs::path& fullPath) const;
	/** @brief Removes all registered assets and resets the next identifier.*/
	void Clear();

private:
	// Map storing the association between the full file path and its AssetId.
	UnorderedMap<fs::path, AssetId> _pathToId;
	AssetId _nextAssetId = 0;
};
