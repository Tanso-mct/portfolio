#pragma once

#include "windows_base/include/container_static.h"
#include "windows_base/include/container_dynamic.h"

#include "windows_base/include/interfaces/entity.h"
#include "windows_base/include/interfaces/component.h"
#include "windows_base/include/interfaces/system.h"
#include "windows_base/include/interfaces/asset.h"
#include "windows_base/include/interfaces/scene.h"
#include "windows_base/include/interfaces/monitor.h"
#include "windows_base/include/interfaces/window.h"
#include "windows_base/include/interfaces/shared.h"

namespace wb
{
    using EntityContainer = DynamicContainer<IEntity>;
    using ComponentContainer = DynamicContainer<IComponent>;
    using SystemContainer = StaticContainer<ISystem>;
    using AssetContainer = StaticContainer<IAsset>;
    using SceneContainer = StaticContainer<ISceneFacade>;
    using MonitorContainer = StaticContainer<IMonitor>;
    using WindowContainer = StaticContainer<IWindowFacade>;
    using SharedContainer = StaticContainer<ISharedFacade>;

} // namespace wb