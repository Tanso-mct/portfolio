#pragma once

#include <functional>

#include "mono_forge/include/ecs_hub.h"
#include "mono_service/include/service_proxy_manager.h"

namespace mono_forge
{

// The global system id generator
extern ecs::SystemIDGenerator g_system_id_generator;

// The global system registrations function
extern std::vector<std::function<void(mono_forge::ECSHub&, mono_service::ServiceProxyManager&)>> g_system_registrations;

// The global system execution order
extern std::vector<ecs::SystemID> g_system_execution_order;

} // namespace mono_forge