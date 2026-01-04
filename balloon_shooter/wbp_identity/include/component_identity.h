#pragma once
#include "windows_base/windows_base.h"
#include "wbp_identity/include/dll_config.h"

#include "wbp_identity/include/interfaces/component_identity.h"

namespace wbp_identity
{
    const WBP_IDENTITY_API size_t &IdentityComponentID();

    class WBP_IDENTITY_API IdentityComponent : public IIdentityComponent
    {
    private:
        std::string name_;
        size_t tag_ = 0;
        size_t layer_ = 0;
        bool activeSelf_ = true;

    public:
        IdentityComponent() = default;
        ~IdentityComponent() override = default;

        /***************************************************************************************************************
         * IComponent implementation
        /**************************************************************************************************************/

        const size_t &GetID() const override;

        /***************************************************************************************************************
         * IIdentityComponent implementation
        /**************************************************************************************************************/

        std::string_view GetName() override { return name_; }
        void SetName(std::string_view name) override { name_ = name.data(); }

        const size_t &GetTag() const override { return tag_; }
        void SetTag(const size_t &tag) override { tag_ = tag; }

        const size_t &GetLayer() const override { return layer_; }
        void SetLayer(const size_t &layer) override { layer_ = layer; }

        bool IsActiveSelf() const override { return activeSelf_; }
        void SetActiveSelf(bool active) override { activeSelf_ = active; }
    };

} // namespace wbp_identity