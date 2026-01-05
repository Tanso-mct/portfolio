#pragma once

#include "mono_entity_archive_extension/include/dll_config.h"

#include "ecs/include/world.h"
#include "component_editor/include/json.hpp"
#include "mono_service/include/service_proxy_manager.h"

namespace mono_entity_archive_extension
{

// Create entities from exported JSON data
MONO_ENTITY_ARCHIVE_EXT_DLL bool CreateEntitiesFromExportedJSON(
    ecs::World& world, const nlohmann::json& json, mono_service::ServiceProxyManager& service_proxy_manager,
    std::vector<ecs::Entity>* out_created_entities = nullptr);

} // namespace mono_entity_archive_extension