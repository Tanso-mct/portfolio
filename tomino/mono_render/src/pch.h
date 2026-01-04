#pragma once

#include "riaecs/riaecs.h"
#include "mono_d3d12/mono_d3d12.h"
#include "mono_identity/mono_identity.h"
#include "mono_transform/mono_transform.h"
#include "mono_scene/mono_scene.h"
#include "mono_asset/mono_asset.h"

#include <d3d12.h>
#include <wrl/client.h>

#include <DirectXMath.h>

#include <shared_mutex>
#include <vector>
#include <memory>
#include <unordered_map>
#include <cassert>