#pragma once

#include "Core/Types.hpp"
#include <entt/entity/entity.hpp>
#include <entt/entity/registry.hpp>

using ComponentTypeId = entt::id_type;

using EntityId = entt::entity;
constexpr EntityId INVALID_ENTITY_ID = entt::null;

using EntityRegistry = entt::registry;

using ArchetypeId = u32;
constexpr ArchetypeId INVALID_ARCHETYPE_ID = static_cast<ArchetypeId>(-1);

using AssetId = u32;
constexpr auto INVALID_ASSET_ID = static_cast<AssetId>(-1);
