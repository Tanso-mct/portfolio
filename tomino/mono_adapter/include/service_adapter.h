#pragma once

#include "mono_adapter/include/dll_config.h"
#include "class_template/singleton.h"
#include "mono_service/include/service.h"

namespace mono_adapter
{

// The adapter class for graphics service
class MONO_ADAPTER_API GraphicsServiceAdapter :
    public class_template::Singleton<GraphicsServiceAdapter>
{
public:
    GraphicsServiceAdapter(mono_service::Service& graphics_service);
    ~GraphicsServiceAdapter() = default;

    // Get the underlying graphics service
    mono_service::Service& GetGraphicsService() { return graphics_service_; }

private:
    // The underlying graphics service
    mono_service::Service& graphics_service_;
};

} // namespace mono_adapter