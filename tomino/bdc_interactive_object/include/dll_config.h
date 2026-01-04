#pragma once

#ifdef BDCINTERACTIVEOBJECT_EXPORTS
#define BDC_INTERACTIVE_OBJECT_API __declspec(dllexport)
#else
#define BDC_INTERACTIVE_OBJECT_API __declspec(dllimport)
#endif