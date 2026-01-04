#pragma once

#ifdef BDCPLAYER_EXPORTS
#define BDC_PLAYER_API __declspec(dllexport)
#else
#define BDC_PLAYER_API __declspec(dllimport)
#endif