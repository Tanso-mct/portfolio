#include "mono_adapter/src/pch.h"
#include "mono_adapter/include/service_adapter.h"

namespace mono_adapter
{

GraphicsServiceAdapter::GraphicsServiceAdapter(mono_service::Service& graphics_service) :
    graphics_service_(graphics_service)
{
}

} // namespace mono_adapter