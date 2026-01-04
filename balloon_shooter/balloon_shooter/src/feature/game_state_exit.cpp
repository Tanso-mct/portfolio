#include "balloon_shooter/src/pch.h"
#include "balloon_shooter/include/feature/game_state_exit.h"

#include "balloon_shooter/include/feature/id_factory_game_state.h"

#include "wbp_identity/plugin.h"
#pragma comment(lib, "wbp_identity.lib")

const size_t &balloon_shooter::ExitGameStateID()
{
    static size_t id = balloon_shooter::GameStateIDFactory::Create();
    return id;
}

const size_t &balloon_shooter::ExitGameState::GetID() const
{
    return balloon_shooter::ExitGameStateID();
}

void balloon_shooter::ExitGameState::UIUpdate(const wb::SystemArgument &args)
{
    
}

void balloon_shooter::ExitGameState::Switched(const wb::SystemArgument &args)
{
    
}
