#pragma once
#include "windows_base/windows_base.h"

namespace balloon_shooter
{
    const size_t &UIBtnSystemID();

    class UIBtnSystem : public wb::ISystem
    {
    private:
        std::vector<size_t> targetUIBtnComponentIDs_;
        
    public:
        UIBtnSystem() = default;
        ~UIBtnSystem() override = default;

        UIBtnSystem(const UIBtnSystem &) = delete;
        UIBtnSystem &operator=(const UIBtnSystem &) = delete;

        /***************************************************************************************************************
         * ISystem implementation
        /**************************************************************************************************************/

        const size_t &GetID() const override;
        void Initialize(wb::IAssetContainer &assetContainer) override;
        void Update(const wb::SystemArgument &args) override;

    };

} // namespace balloon_shooter