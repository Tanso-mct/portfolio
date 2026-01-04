#pragma once

#include "utility_header/id.h"
#include "class_template/singleton.h"

#include "mono_scene_extension/include/dll_config.h"

namespace mono_scene_extension
{

// The type used to identify scenes
using SceneID = utility_header::ID;

// The ID generator for scene tags
class MONO_SCENE_EXT_DLL SceneIDGenerator :
    public class_template::Singleton<SceneIDGenerator>,
    public utility_header::IDGenerator
{
public:
    SceneIDGenerator() = default;
    virtual ~SceneIDGenerator() override = default;
};

// The template class for scene tags of specific types
template <typename TagTag>
class SceneTag
{
public:
    SceneTag() = default;
    virtual ~SceneTag() = default;

    // Get the unique ID of the scene tag type
    static SceneID ID()
    {
        static const SceneID id = SceneIDGenerator::GetInstance().Generate();
        return id;
    }
};

} // namespace mono_scene_extension