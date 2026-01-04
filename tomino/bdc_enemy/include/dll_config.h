#pragma once

#ifdef BDCENEMY_EXPORTS
#define BDC_ENEMY_API __declspec(dllexport)
#else
#define BDC_ENEMY_API __declspec(dllimport)
#endif