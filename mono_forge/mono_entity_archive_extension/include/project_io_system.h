#pragma once

#include "ecs/include/system.h"
#include "mono_service/include/service.h"
#include "mono_service/include/service_proxy_manager.h"

#include "mono_entity_archive_extension/include/dll_config.h"
#include "mono_service/include/service.h"

namespace mono_entity_archive_extension
{

// The default file path for entity archive JSON within a project from the project root
constexpr const wchar_t* PROJECT_ENTITY_ARCHIVE_JSON_FILE_PATH = L"/scene/entity_archive.json";

// The default file path for asset archive JSON within a project from the project root
constexpr const wchar_t* PROJECT_ASSET_ARCHIVE_JSON_FILE_PATH = L"/asset/asset_archive.json";

// The default file path for material archive JSON within a project from the project root
constexpr const wchar_t* PROJECT_MATERIAL_ARCHIVE_JSON_FILE_PATH = L"/material/material_archive.json";

// The handle class for the system
class MONO_ENTITY_ARCHIVE_EXT_DLL ProjectIOSystemHandle :
    public ecs::SystemHandle<ProjectIOSystemHandle> {};

// The system class
class MONO_ENTITY_ARCHIVE_EXT_DLL ProjectIOSystem :
    public ecs::System
{
public:
    ProjectIOSystem(
        std::unique_ptr<mono_service::ServiceProxy> entity_archive_service_proxy,
        std::unique_ptr<mono_service::ServiceProxy> asset_service_proxy,
        mono_service::ServiceProxyManager& service_proxy_manager);
    virtual ~ProjectIOSystem() override;

    virtual bool PreUpdate(ecs::World& world) override;
    virtual bool Update(ecs::World& world) override;
    virtual bool PostUpdate(ecs::World& world) override;
    virtual ecs::SystemID GetID() const override;

private:
    // The entity archive service proxy
    std::unique_ptr<mono_service::ServiceProxy> entity_archive_service_proxy_ = nullptr;

    // The asset service proxy
    std::unique_ptr<mono_service::ServiceProxy> asset_service_proxy_ = nullptr;

    // Service proxy manager
    mono_service::ServiceProxyManager& service_proxy_manager_;

};


} // namespace mono_entity_archive_extension