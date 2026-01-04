#include "balloon_shooter/src/pch.h"
#include "balloon_shooter/include/feature/id_factory_game_state.h"

size_t balloon_shooter::GameStateIDFactory::Create()
{
    static size_t idCounter = 0;

    size_t createdId = idCounter;
    idCounter++;

    return createdId;
}