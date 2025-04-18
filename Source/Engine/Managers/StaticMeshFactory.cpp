#include "StaticMeshFactory.hpp"

#include "Core/Log/Logger.hpp"
#include "Core/Paths/Paths.hpp"

#include "Engine/Importers/StaticMeshLoader.hpp"

const SharedPointer<const StaticMesh> StaticMeshFactory::GetPrototype(const fs::path& relativePathToFile)
{
	auto it = _prototypes.find(relativePathToFile);
	if (it != _prototypes.end())
		return it->second;

	return nullptr;
}

const SharedPointer<const StaticMesh> StaticMeshFactory::CreatePrototype(const fs::path& absolutePathToFile)
{
	fs::path relative = fs::relative(absolutePathToFile, Paths::GetStaticModelsPath());

	auto it = _prototypes.find(relative);
	if (it != _prototypes.end())
		return it->second;

	auto [vals, success] = _prototypes.emplace(relative, std::make_shared<StaticMesh>());
	auto& prototype = vals->second;
	prototype->prototypeID = _prototypes.size();

	StaticMeshLoader loader;
	loader.LoadDataFromFile(absolutePathToFile, *prototype);
	
	return prototype;
}

fs::path StaticMeshFactory::GetPrototypePath(i32 prototypeID)
{
	for (const auto& [path, prototype] : _prototypes)
		if (prototype->prototypeID == prototypeID)
			return path;
	
	CONSOLE_WARN("Invalid prototypeID ({})", prototypeID);
	return fs::path("none");
}
