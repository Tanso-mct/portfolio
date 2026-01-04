#pragma once

#ifdef $PROJECTNAME$_EXPORTS
#define WBP_$PLUGIN_NAME$_API __declspec(dllexport)
#else
#define WBP_$PLUGIN_NAME$_API __declspec(dllimport)
#endif