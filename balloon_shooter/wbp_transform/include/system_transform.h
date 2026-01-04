#pragma once
#include "wbp_transform/include/dll_config.h"
#include "windows_base/windows_base.h"

namespace wbp_transform
{
    const WBP_TRANSFORM_API size_t &TransformSystemID();

    class WBP_TRANSFORM_API TransformSystem : public wb::ISystem
    {
    public:
        TransformSystem() = default;
        virtual ~TransformSystem() override = default;

        /***************************************************************************************************************
         * ISystem implementation
        /**************************************************************************************************************/

        virtual const size_t &GetID() const override;
        void Initialize(wb::IAssetContainer &assetCont) override;
        void Update(const wb::SystemArgument &args) override;

        /***************************************************************************************************************
         * TransformSystem specific methods
        /**************************************************************************************************************/

        void UpdateRootTransform
        (
            wb::IEntity *root, 
            wb::IEntityContainer &entityCont, wb::IComponentContainer &componentCont
        );
        
        void UpdateChildTransform
        (
            wb::IEntity *parent, 
            wb::IEntityContainer &entityCont, wb::IComponentContainer &componentCont
        );
    };

} // namespace wbp_transform