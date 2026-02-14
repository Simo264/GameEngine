#include "Archetype.hpp"

// -----------------------------------
//					Archetype
// -----------------------------------

void Archetype::GetAllowedComponentNames(Vector<StringView>& out) const
{
	out.clear();
	out.assign(_componentNames.begin(), _componentNames.end());
}
bool Archetype::IsComponentAllowed(ComponentTypeId componentType) const
{
	return _allowedComponents.contains(componentType);
}

// -----------------------------------
//					ArchetypeRegistry
// -----------------------------------

ArchetypeId ArchetypeRegistry::RegisterArchetype(const Archetype& archetype)
{
	auto newName = StringView(archetype.GetName());
	for (ArchetypeId i = 0; i < _archetypes.size(); ++i)
		if (newName.compare(_archetypes[i].GetName()) == 0)
			return i;

	auto id = _archetypes.size();
	_archetypes.push_back(archetype);
	return ArchetypeId(id);
}
const Archetype& ArchetypeRegistry::GetArchetype(ArchetypeId id) const
{
	if (id >= _archetypes.size())
		throw std::runtime_error("INVALID ARCHETYPE ID");

	return _archetypes[id];
}
ArchetypeId ArchetypeRegistry::GetArchetypeId(StringView name) const
{
	for (ArchetypeId i = 0; i < _archetypes.size(); ++i)
		if (name.compare(_archetypes[i].GetName()) == 0)
			return i;
	return INVALID_ARCHETYPE_ID;
}
