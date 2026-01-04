#include "example/src/pch.h"
#include "example/include/window_game_example/window.h"

#include "example/include/window_game_example/facade_factory.h"
#include "example/include/window_game_example/event_factory.h"

const size_t &example::GameExampleWindowID()
{
    static size_t id = wb::IDFactory::CreateWindowID();
    return id;
}

namespace example
{
    WB_REGISTER_WINDOW(GameExampleWindowID, GameExampleWindowFacadeFactory, GameExampleWindowEventFactory);
}