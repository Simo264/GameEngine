#pragma once

#include "Core/Core.hpp"
#include "Core/DesignPatterns/Singleton.hpp"
#include "Engine/Components/StaticMesh.hpp"

/**
 * @brief Factory class implementing the Prototype Design Pattern for static meshes.
 *
 * This class manages the creation and retrieval of static mesh prototypes,
 * following the Prototype Design Pattern. It ensures efficient reuse of
 * preloaded mesh instances instead of creating new ones from scratch,
 * minimizing memory usage and improving performance.
 *
 * The Prototype Design Pattern is leveraged here to:
 * - Store already-loaded mesh instances.
 * - Clone existing prototypes instead of reloading them.
 * - Provide quick access to assets using unique identifiers (prototypeID).
 */
class StaticMeshFactory : public Singleton<StaticMeshFactory>
{
public:
	void Cleanup();

	/** @brief Retrieves an existing prototype if available. */
	SharedPointer<const Components::StaticMesh> GetPrototype(const fs::path& relativePathToFile);

	/** @brief Loads a new file and creates a prototype. */
	SharedPointer<const Components::StaticMesh> CreatePrototype(const fs::path& absolutePathToFile);

	/** @brief Retrieves the relative file path associated with a given prototype. */
	fs::path GetPrototypePath(i32 prototypeID);

private:
	/** @brief Stores loaded static mesh prototypes mapped by relative file paths. */
	UnorderedMap<fs::path, SharedPointer<Components::StaticMesh>> _prototypes;
};
