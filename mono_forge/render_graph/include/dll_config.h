#pragma once

#ifdef render_graph_EXPORTS
#define RENDER_GRAPH_DLL __declspec(dllexport)
#else
#define RENDER_GRAPH_DLL __declspec(dllimport)
#endif