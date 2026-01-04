#pragma once

#include "class_template/thread_safer.h"
#include "mono_service/include/service_command.h"

namespace mono_service
{

class ServiceView
{
public:
    ServiceView(const ServiceAPI& service_api) :
        service_api_(service_api)
    {
    }
    
    virtual ~ServiceView() = default;

protected:
    const ServiceAPI& service_api_;

};

} // namespace mono_service