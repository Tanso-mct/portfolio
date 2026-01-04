#pragma once

#ifdef ecs_EXPORTS
#define ECS_DLL __declspec(dllexport)
#else
#define ECS_DLL __declspec(dllimport)
#endif