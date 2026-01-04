#pragma once
#include "mono_identity/include/dll_config.h"
#include "riaecs/riaecs.h"

#include <string>

namespace mono_identity
{
    constexpr size_t ComponentIdentityMaxCount = 10000;
    class MONO_IDENTITY_API ComponentIdentity
    {
    private:
        std::string name_;
        size_t tag_ = 0;
        size_t layer_ = 0;
        bool activeSelf_ = true;

    public:
        ComponentIdentity();
        ~ComponentIdentity();

        struct SetupParam
        {
            std::string name = "GameObject";
            size_t tag = 0;
            size_t layer = 0;
            bool activeSelf = true;
        };
        void Setup(SetupParam &param);

        const std::string &GetName() const { return name_; }
        void SetName(const std::string &name) { name_ = name; }

        size_t GetTag() const { return tag_; }
        void SetTag(size_t tag) { tag_ = tag; }

        size_t GetLayer() const { return layer_; }
        void SetLayer(size_t layer) { layer_ = layer; }

        bool IsActiveSelf() const { return activeSelf_; }
        void SetActiveSelf(bool active) { activeSelf_ = active; }
    };
    extern MONO_IDENTITY_API riaecs::ComponentRegistrar<ComponentIdentity, ComponentIdentityMaxCount> ComponentIdentityID;

} // namespace mono_identity