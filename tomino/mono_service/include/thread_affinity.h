#pragma once

#include "class_template/singleton.h"
#include "utility_header/id.h"

#include "mono_service/include/dll_config.h"

namespace mono_service
{

using ServiceThreadAffinityID = utility_header::ID;

// The singleton service thread affinity ID generator class
class MONO_SERVICE_DLL ServiceThreadAffinityIDGenerator :
    public class_template::Singleton<ServiceThreadAffinityIDGenerator>,
    public utility_header::IDGenerator
{
public:
    ServiceThreadAffinityIDGenerator() = default;
    virtual ~ServiceThreadAffinityIDGenerator() = default;
};

// The template class for service thread affinity handles of specific types
// It must only be used on the EXE side
template <typename ServiceThreadAffinityTag>
class ServiceThreadAffinityHandle
{
public:
    ServiceThreadAffinityHandle() = default;
    virtual ~ServiceThreadAffinityHandle() = default;

    // Get the unique ID of the service thread affinity handle type
    static ServiceThreadAffinityID ID()
    {
        static const ServiceThreadAffinityID id = ServiceThreadAffinityIDGenerator::GetInstance().Generate();
        return id;
    }
};

} // namespace mono_service