#pragma once

#ifdef mono_entity_archive_service_EXPORTS
#define MONO_ENTITY_ARCHIVE_SERVICE_DLL __declspec(dllexport)
#else
#define MONO_ENTITY_ARCHIVE_SERVICE_DLL __declspec(dllimport)
#endif