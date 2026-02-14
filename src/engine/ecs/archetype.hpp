#pragma once

#include "Core/Types.hpp"
#include "Core/Containers.hpp"
#include "Core/Strings.hpp"
#include "Types.hpp"
#include "Components/Components.hpp"

class Archetype
{
public:
	static constexpr u64 MAX_NAME_LENGTH = 32;
	using ArchetypeName = Array<char, MAX_NAME_LENGTH>;

	Archetype(StringView name)
	{
		using namespace Components;
		_allowedComponents.insert(entt::type_hash<Tag>::value());
		_allowedComponents.insert(entt::type_hash<ArchetypeIdentifier>::value());
		_componentNames.insert(GetTypeName<Tag>());
		_componentNames.insert(GetTypeName<ArchetypeIdentifier>());

		auto n = std::min(name.size(), MAX_NAME_LENGTH - 1);
		std::copy_n(name.begin(), n, _archetypeName.begin());
		_archetypeName[n] = '\0';
	}

	template<typename... Comps>
	auto& AllowComponents()
	{
		using namespace Components;
		((void)_allowedComponents.insert(entt::type_hash<Comps>::value()), ...);
		((void)_componentNames.insert(GetTypeName<Comps>()), ...);
		return *this;
	}

	bool IsComponentAllowed(ComponentTypeId componentType) const;
	void GetAllowedComponentNames(Vector<StringView>& out) const;
	auto GetName() const { return _archetypeName.data(); }

private:
	ArchetypeName _archetypeName;
	Set<ComponentTypeId> _allowedComponents;
	Set<StringView> _componentNames;
};
class ArchetypeRegistry
{
public:
	ArchetypeRegistry() = default;

	ArchetypeId RegisterArchetype(const Archetype& archetype);
	ArchetypeId GetArchetypeId(StringView name) const;
	const Archetype& GetArchetype(ArchetypeId id) const;
	auto GetArchetypeCount() const { return _archetypes.size(); }
	auto& GetArchetypeVector() const { return _archetypes; }

private:
	Vector<Archetype> _archetypes; // The ArchetypeId is the index
};