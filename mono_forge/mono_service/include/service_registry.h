#pragma once

#include "class_template/singleton.h"
#include "utility_header/id.h"
#include "utility_header/registry.h"

#include "mono_service/include/dll_config.h"
#include "mono_service/include/service.h"

namespace mono_service
{

// The type used to identify service handles
using ServiceHandleID = utility_header::ID;

// The singleton service ID generator class
class MONO_SERVICE_DLL ServiceIDGenerator :
    public class_template::Singleton<ServiceIDGenerator>,
    public utility_header::IDGenerator
{
public:
    ServiceIDGenerator() = default;
    virtual ~ServiceIDGenerator() = default;
};

// The template class for service handles of specific types
// It must only be used on the EXE side
template <typename ServiceTag>
class ServiceHandle
{
public:
    ServiceHandle() = default;
    virtual ~ServiceHandle() = default;

    // Get the unique ID of the service handle type
    static ServiceHandleID ID()
    {
        static const ServiceHandleID id = ServiceIDGenerator::GetInstance().Generate();
        return id;
    }
};

// The registry type for services
using ServiceRegistry = utility_header::Registry<Service>;

} // namespace mono_service