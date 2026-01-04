#pragma once

#ifdef WBPMODEL_EXPORTS
#define WBP_MODEL_API __declspec(dllexport)
#else
#define WBP_MODEL_API __declspec(dllimport)
#endif