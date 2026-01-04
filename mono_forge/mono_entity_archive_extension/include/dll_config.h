#pragma once

#ifdef mono_entity_archive_extension_EXPORTS
#define MONO_ENTITY_ARCHIVE_EXT_DLL __declspec(dllexport)
#else
#define MONO_ENTITY_ARCHIVE_EXT_DLL __declspec(dllimport)
#endif