#pragma once

#ifdef $PROJECTNAME$_EXPORTS
#define $API_NAME$ __declspec(dllexport)
#else
#define $API_NAME$ __declspec(dllimport)
#endif