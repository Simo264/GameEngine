#include "StaticMeshFactory.hpp"

#include "Core/Log/Logger.hpp"
#include "Core/Paths/Paths.hpp"

#include "Engine/Importers/StaticMeshLoader.hpp"

SharedPointer<const Components::StaticMesh> StaticMeshFactory::GetPrototype(const fs::path& relativePathToFile)
{
	auto it = _prototypes.find(relativePathToFile);
	if (it != _prototypes.end())
		return it->second;

	return nullptr;
}

SharedPointer<const Components::StaticMesh> StaticMeshFactory::CreatePrototype(const fs::path& absolutePathToFile)
{
	auto relative = fs::relative(absolutePathToFile, Paths::GetModelsPath());
	auto it = _prototypes.find(relative);
	if (it != _prototypes.end())
		return it->second;

	auto newStaticMesh = std::make_shared<Components::StaticMesh>();
	auto [vals, success] = _prototypes.emplace(relative, newStaticMesh);
	newStaticMesh->prototypeID = _prototypes.size();

	auto loader = StaticMeshLoader{};
	loader.LoadDataFromFile(absolutePathToFile, *newStaticMesh);
	return newStaticMesh;
}

fs::path StaticMeshFactory::GetPrototypePath(i32 prototypeID)
{
	for (const auto& [path, prototype] : _prototypes)
		if (prototype->prototypeID == prototypeID)
			return path;
	
	CONSOLE_WARN("Invalid prototypeID ({})", prototypeID);
	return fs::path("none");
}

void StaticMeshFactory::Cleanup()
{
	for (auto& [path, staticMesh] : _prototypes)
		staticMesh->Destroy();
}
