#include "wbp_identity/src/pch.h"
#include "wbp_identity/include/component_identity.h"

const WBP_IDENTITY_API size_t &wbp_identity::IdentityComponentID()
{
    static size_t id = wb::IDFactory::CreateComponentID();
    return id;
}

const size_t &wbp_identity::IdentityComponent::GetID() const
{
    return IdentityComponentID();
}

namespace wbp_identity
{
    WB_REGISTER_COMPONENT(IdentityComponentID(), IdentityComponent);

} // namespace wbp_identity