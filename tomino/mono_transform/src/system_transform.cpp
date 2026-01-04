#include "mono_transform/src/pch.h"
#include "mono_transform/include/system_transform.h"

#include "mono_transform/include/component_transform.h"

#include "mono_identity/mono_identity.h"
#pragma comment(lib, "mono_identity.lib")

#pragma comment(lib, "riaecs.lib")

mono_transform::SystemTransform::SystemTransform()
{
}

mono_transform::SystemTransform::~SystemTransform()
{
}

bool mono_transform::SystemTransform::Update
(
    riaecs::IECSWorld &ecsWorld, riaecs::IAssetContainer &assetCont, 
    riaecs::ISystemLoopCommandQueue &systemLoopCmdQueue
){
    // Update count for last transform update
    updateCount_++;

    for (const riaecs::Entity &entity : ecsWorld.View(mono_transform::ComponentTransformID())())
    {
        mono_transform::ComponentTransform* transform
        = riaecs::GetComponentWithCheck<mono_transform::ComponentTransform>(
            ecsWorld, entity, mono_transform::ComponentTransformID(), "ComponentTransform", RIAECS_LOG_LOC);

        if (!transform->IsInitialized())
        {
            mono_identity::ComponentIdentity* identity
            = riaecs::GetComponentWithCheck<mono_identity::ComponentIdentity>(
                ecsWorld, entity, mono_identity::ComponentIdentityID(), "ComponentIdentity", RIAECS_LOG_LOC);

            riaecs::NotifyError
            (
                {
                    "Entity:" + identity->GetName() + "'s transform component is not initialized. ",
                    "Please make sure to initialize the transform component after adding it.",
                    "Entity index: " + std::to_string(entity.GetIndex()),
                    "Entity generation: " + std::to_string(entity.GetGeneration())
                }, RIAECS_LOG_LOC
            );
        }

        if (transform->GetParent().IsValid())
            continue; // Skip non-root transforms

        if (updateCount_ >= lastTransformUpdateInterval_)
        {
            mono_transform::UpdateRootTransform(transform);
            mono_transform::UpdateChildsTransform(entity, ecsWorld);
        }
        else
        {
            mono_transform::UpdateRootTransformNoLastTransform(transform);
            mono_transform::UpdateChildsTransformNoLastTransform(entity, ecsWorld);
        }
    }

    if (updateCount_ >= lastTransformUpdateInterval_)
        updateCount_ = 0;

    return true; // Continue running
}

MONO_TRANSFORM_API riaecs::SystemFactoryRegistrar<mono_transform::SystemTransform> mono_transform::SystemTransformID;