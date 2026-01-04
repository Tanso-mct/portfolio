#pragma once

#ifdef BDCGAMEFLOW_EXPORTS
#define BDC_GAME_FLOW_API __declspec(dllexport)
#else
#define BDC_GAME_FLOW_API __declspec(dllimport)
#endif