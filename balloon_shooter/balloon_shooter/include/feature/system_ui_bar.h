#pragma once
#include "windows_base/windows_base.h"

namespace balloon_shooter
{
    const size_t &UIBarSystemID();

    class UIBarSystem : public wb::ISystem
    {
    private:
        std::vector<size_t> targetUIBarComponentIDs_;
        
    public:
        UIBarSystem() = default;
        ~UIBarSystem() override = default;

        UIBarSystem(const UIBarSystem &) = delete;
        UIBarSystem &operator=(const UIBarSystem &) = delete;

        /***************************************************************************************************************
         * ISystem implementation
        /**************************************************************************************************************/

        const size_t &GetID() const override;
        void Initialize(wb::IAssetContainer &assetContainer) override;
        void Update(const wb::SystemArgument &args) override;

    };

} // namespace balloon_shooter